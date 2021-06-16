#include "string_utils.hh"

#ifndef TEST_string_utils_cc

#include <cassert>
#include <algorithm>

std::string unescape(const std::string &sym)
{
  if (sym == "@0@")
    { return "@_EPSILON_SYMBOL_@"; }
  return sym;
}

void split(const std::string &str, const std::string &sep, StringVector &v)
{
  int start = 0;
  int stop = str.find(sep);

  while (stop != std::string::npos)
    {
      v.push_back(str.substr(start, stop - start));
      start = stop + sep.size();
      stop = str.find(sep, start);
    }

  if (start < str.size())
    { v.push_back(str.substr(start)); }
}

void read_char_pairs(const std::string &str, CharPairVector &v)
{
  StringVector pairs;
  split(str," ",pairs);

  for (StringVector::const_iterator it = pairs.begin(); it != pairs.end(); ++it)
    {
      StringVector input_and_output;
      split(*it,":",input_and_output);

      if (input_and_output.size() == 1)
	{ 
	  v.push_back(CharPair(unescape(input_and_output[0]),
			       unescape(input_and_output[0]))); 
	}
      else if (input_and_output.size() == 2)
	{ 
	  v.push_back(CharPair(unescape(input_and_output[0]),
			       unescape(input_and_output[1]))); 
	}
      else
	{
	  // FIXME: Deal with syntax errors and escaping of ":" and " ".
	  assert(0);
	}
    }
}

bool is_flag_diacritic(const std::string &sym)
{
  if (sym.size() < 5)
    { return false; }
  if (sym[0] != '@')
    { return false; }
  if (sym.back() != '@')
    { return false; }
  if (sym.find_first_of("CPUDRNE") != 1)
    { return false; }
  if (sym[2] != '.')
    { return false; }
  if (std::count(sym.begin(), sym.end(), '.') > 2)
    { return false; }
  return true;
}

#else // TEST_string_utils_cc

#include <cassert>

int main(void)
{
  StringVector v;

  split("a dog ", " ", v);

  assert(v.size() == 2);
  assert(v[0] == "a");
  assert(v[1] == "dog");
  
  v.clear();

  split("a dog ", "\t", v);

  assert(v.size() == 1);
  assert(v[0] == "a dog ");

  v.clear();

  split("a.o.dog.o.", ".o.", v);

  assert(v.size() == 2);
  assert(v[0] == "a");
  assert(v[1] == "dog");

  v.clear();

  split("# #", " ", v);

  assert(v.size() == 2);
  assert(v[0] == "#");
  assert(v[1] == "#");
}


#endif // TEST_string_utils_cc
