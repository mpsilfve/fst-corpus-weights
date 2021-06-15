#include <iostream>

#include <hfst/HfstTransducer.h>

#include "path_utils.hh"
#include "StreamReader.hh"
#include "string_utils.hh"

using hfst::HfstInputStream;
using hfst::HfstOutputStream;
using hfst::HfstTransducer;
using hfst::implementations::HfstBasicTransducer;
using hfst::TROPICAL_OPENFST_TYPE;
using hfst::HfstTokenizer;
using hfst::HfstTwoLevelPath;
using hfst::HfstTokenizer;
using hfst::HfstBasicTransducer;
using hfst::StringSet;

#define MISSING 0

// Choose what kind of weighting to apply. 
//
// PLAIN: Simply add weight to each transition on the correct path for
// each example.
// 
// PERCEPTRON: Reduce the weight of the current analysis guess path
// and add weight to the gold analysis path.
// 
// AVG_PERCEPTRON: To be implemented.
//
// When adding types, remember to edit BOTH WeightType AND weighters.
enum WeightedType
  { PLAIN, PERCEPTRON, AVG_PERCEPTRON, NO_TYPE };

StringVector weighters { "PLAIN", "PERCEPTRON", "AVG_PERCEPTRON" };

WeightedType get_weighted_type(const std::string param)
{
  for (int type = 0; type < NO_TYPE; ++type)
    {
      if (param == weighters[type])
	{ return static_cast<WeightedType>(type); }
    }
  return NO_TYPE;
}

void print_usage(const std::string &prog_name)
{
  std::cerr << "USAGE: cat string_pairs | " 
	    << prog_name 
	    << " input_fst_file output_fst_file [";
    
  for (size_t i = 0; i < weighters.size(); ++i)
    {
      std::cerr << weighters[i] 
		<< (i + 1 == weighters.size() ? "]" : " | ");
    }
  std::cerr << std::endl;
}

void weight_string_pair(const StringVector &fields, 
			HfstBasicTransducer &bfst, 
			const HfstTransducer &fst,
			const HfstTokenizer &tok,
			WeightedType &wtype)
{
  const std::string &inputstr  = fields[0];
  const std::string &outputstr = fields[1];

  //  HfstOneLevelPaths * paths = fst.lookup_fd(inputstr);
  HfstTwoLevelPaths paths;
  bfst.lookup(tok.tokenize_one_level(inputstr), paths, NULL, NULL, true);
  HfstOneLevelPath match;
  if (not get_first_match(paths, outputstr, match) and MISSING)
    { 
      std::cerr << "String pair \"" << inputstr << ":" << outputstr << "\""
		<< " is not recognized." << std::endl
		<< "Skipping!" << std::endl << std::endl; 
      return;
    }
  std::cout << "gfs sto" << std::endl;
  // Tokenize input string into utf8 symbols.
  StringVector inputtok = tok.tokenize_one_level(inputstr);

  // Get the path of symbol pairs corresponding to the input and
  // output string.
  HfstTwoLevelPath aligned_path;
  align_paths(HfstOneLevelPath(0, inputtok), match, bfst, aligned_path);

  HfstOneLevelPath sys_guess;

  if (wtype == PERCEPTRON and sys_guess != match)
    {
      get_heaviest_path(paths, sys_guess);

      HfstTwoLevelPath aligned_sys_path;
      align_paths(HfstOneLevelPath(0, inputtok), sys_guess, 
		  bfst, aligned_sys_path);

      incr_path_weight(aligned_sys_path, bfst, -1);
      incr_path_weight(aligned_path,     bfst,  1);
    }
  // Increment the weight of all transitions on the path as well as the
  // final state.
  else if (wtype == PLAIN)
    { incr_path_weight(aligned_path, bfst, 1.0); }
  std::cout << "stop" << std::endl;
}

int main(int argc, char * argv[])
{
  WeightedType wtype;

  if (argc != 4 or (wtype = get_weighted_type(argv[3])) == NO_TYPE)
    {
      print_usage(argv[0]);
      exit(1);
    }
  if (wtype == AVG_PERCEPTRON)
    {
      std::cerr << "Error: Averaged perceptron estimation not implemented."
		<< std::endl;
      exit(1);      
    }
  
  std::cerr << "Reading input fst from " << argv[1] << "."
	    << std::endl;
  HfstInputStream in(argv[1]);
  HfstTransducer fst(in);

  // FIXME: Doesn't know how to tokenize input multichar symbols!
  HfstTokenizer tok;

  StringSet alphabet = fst.get_alphabet();
  for (const std::string & s : alphabet)
    { tok.add_multichar_symbol(s); }

  HfstBasicTransducer bfst(fst);

  StreamReader stdin_reader;
  std::cerr << "Reading string pairs from STDIN."
	    << std::endl;
  std::vector<StringVector> lines;
  while(stdin_reader.readline())
    {
      const std::string &line = stdin_reader.getline();
      
      // Skip empty lines.
      if (line == "")
	{ continue; }
      
      StringVector fields;
      split(line, "\t", fields);
      
      if (fields.size() != 2)
	{
	  std::cerr << "Syntax error on line " 
		    << stdin_reader.getlinenumber()
		    << std::endl;
	  exit(1);
	}
      
      // Skip input strings which weren't recognized.
      if (fields[1] == "+?")
	{ continue; }
      
      lines.push_back(fields);
    }
  
  for (int i = 0; i < 1; ++i)
    {
      std::cerr << "Epoch " << i << std::endl;

      for (size_t j = 0; j < lines.size(); ++j)
	   
	{ 
	  std::cerr << j + 1 << " of " << lines.size() << "\r";
 
	  const StringVector &fields = lines[j];
	  weight_string_pair(fields, bfst, fst, tok, wtype); 
	}
      std::cerr << std::endl;
    }

  std::cerr << "Writing weighted fst to " << argv[2] << std::endl;
  HfstTransducer res(bfst, TROPICAL_OPENFST_TYPE);
  HfstOutputStream out(argv[2], TROPICAL_OPENFST_TYPE);
  out << res;
}
