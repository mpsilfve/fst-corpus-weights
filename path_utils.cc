#include "path_utils.hh"

#ifndef TEST_path_utils_cc

#include <iostream>

using hfst::StringVector;
using hfst::implementations::HfstState;
using hfst::HfstBasicTransition;
using hfst::StringPair;
using hfst::StringPairVector;

static std::string epsilon("@_EPSILON_SYMBOL_@");

bool is_special_char(const std::string &symb)
{ return symb.size() > 1 and symb[0] == '@' and symb.back() == '@'; }

void getstring(const HfstOneLevelPath &p, std::string &s)
{
  s.clear();
  
  const StringVector &v = p.second;

  for (const std::string symb: v)
    {
      if (not is_special_char(symb))
	{ s += symb; }
    }
}

void getstring(const HfstTwoLevelPath &p, std::string &s)
{
  s.clear();
  
  const StringPairVector &v = p.second;

  for (const StringPair & symbp: v)
    {
      if (not is_special_char(symbp.second))
	{ s += symbp.second; }
    }
}

HfstOneLevelPath two_level_to_one_level_path(const HfstTwoLevelPath &p)
{
  StringVector r;
  for (const StringPair & sp : p.second)
    {
      r.push_back(sp.second);
    }
  return HfstOneLevelPath(p.first, r);
}

bool get_first_match(HfstOneLevelPaths &paths, 
		     const std::string &string, 
		     HfstOneLevelPath &match)
{
  for (const HfstOneLevelPath &p : paths)
    {
      std::string str;
      getstring(p, str);

      if (str == string)
	{ 
	  match = p;
	  return 1;
	}
    }
  return 0;
}

bool get_first_match(HfstTwoLevelPaths &paths, 
		     const std::string &string, 
		     HfstOneLevelPath &match)
{
  for (const HfstTwoLevelPath &p : paths)
    {
      std::string str;
      getstring(p, str);

      if (str == string)
	{ 
	  match = two_level_to_one_level_path(p);
	  return 1;
	}
    }
  return 0;
}

bool is_matching_pair(const std::string &in1, const std::string &in2,
		      const std::string &out1, const std::string &out2)
{
  return 
    (is_special_char(in1) or is_special_char(in2) or in1 == in2) and
    (out1 == out2);    
}

bool align_transition(StringVector::const_iterator it, 
		      StringVector::const_iterator iend,
		      StringVector::const_iterator ot, 
		      StringVector::const_iterator oend,
		      HfstState s,
		      HfstBasicTransition t,
		      const HfstBasicTransducer &b,
		      HfstTwoLevelPath &aligned)
{
  std::string isymbol = (it == iend ? epsilon : *it);
  std::string osymbol = (ot == oend or *ot == "" ? epsilon : *ot);
  const std::string t_input  = t.get_input_symbol();
  const std::string t_output = t.get_output_symbol();
  
  if (is_matching_pair(isymbol, t_input, osymbol, t_output))
    {
      aligned.second.push_back(StringPair(t_input, t_output));

      if (align_paths(it + (it != iend and t_input  == isymbol ? 1 : 0), iend, 
		      ot + 1, oend,
		      t.get_target_state(), b, aligned))
	{ return 1; }	      
      aligned.second.pop_back();
    }
  return 0;
}

bool align_paths(StringVector::const_iterator it, 
		 StringVector::const_iterator iend,
		 StringVector::const_iterator ot, 
		 StringVector::const_iterator oend,
		 HfstState s,
		 const HfstBasicTransducer &b,
		 HfstTwoLevelPath &aligned)
{
  if (ot == oend and b.is_final_state(s))
    { return 1; }
  else if (ot == oend)
    { return 0; }

  for (const HfstBasicTransition &t : b.transitions(s))
    {
      if (align_transition(it, iend, ot, oend, s, t, b, aligned))
	{
	  std::reverse(aligned.second.begin(), aligned.second.end());
	  return 1;
	}
    }
  
  return 0;
}

bool align_paths(const HfstOneLevelPath &in, 
		 const HfstOneLevelPath &out, 
		 const HfstBasicTransducer &b,
		 HfstTwoLevelPath &aligned)
{
  const StringVector &itoks = in.second;
  const StringVector &otoks = out.second;
  
  bool succ = align_paths(itoks.begin(), itoks.end(), 
			  otoks.begin(), otoks.end(),
			  0, b, aligned);
  return succ;
}

bool incr_path_weight(StringPairVector::const_iterator it,
		      StringPairVector::const_iterator end,
		      HfstState s, HfstBasicTransducer &bfst,
		      float increment)
{ 
  if (it == end)
    { 
      if (bfst.is_final_state(s))
	{
	  bfst.set_final_weight(s, bfst.get_final_weight(s) + increment);
	  return 1; 
	}
      else
	{ return 0; }
    }

  for (HfstBasicTransition &t : bfst.transitions(s))
    {      
      if (t.get_input_symbol() == it->first and 
	  t.get_output_symbol() == it->second)
	{ 
	  if (incr_path_weight(it + 1, end, t.get_target_state(), 
			       bfst, increment))
	    {
	      t.set_weight(t.get_weight() + increment);

	      return 1;
	    }
	}
    }
  
  // Didn't find a suitable transition.
  return 0;
}

bool incr_path_weight(HfstTwoLevelPath &aligned_path, 
		      HfstBasicTransducer &bfst,
		      float increment)
{ 
  return incr_path_weight(aligned_path.second.begin(),
			  aligned_path.second.end(),
			  0, bfst, increment); 
}

void get_heaviest_path(const HfstOneLevelPaths &paths, 
		       HfstOneLevelPath &best_path)
{
  best_path.first = -INFINITY;

  for (const HfstOneLevelPath &p : paths)
    {
      if (p.first > best_path.first)
	{ best_path  = p; }
    }
}

void get_heaviest_path(const HfstTwoLevelPaths &paths, 
		       HfstOneLevelPath &best_path)
{
  best_path.first = -INFINITY;

  for (const HfstTwoLevelPath &tp : paths)
    {
      HfstOneLevelPath p = two_level_to_one_level_path(tp);
      if (p.first > best_path.first)
	{ best_path  = p; }
    }
}

#else // TEST_path_utils_cc

#include <cassert>
#include <string>

#include <hfst/HfstTransducer.h>

using hfst::HfstOneLevelPath;
using hfst::HfstOneLevelPaths;
using hfst::StringVector;
using hfst::implementations::HfstBasicTransition;
using hfst::StringPair;

std::string epsilon("@_EPSILON_SYMBOL_@");

int main(void)
{
  StringVector v;
  HfstOneLevelPath p(0.0, v);
  std::string res;
  getstring(p, res);
  assert(res == "");

  v.push_back("@U.NeedNoun.On@");
  v.push_back("d");
  v.push_back("o");
  v.push_back("@C.NeedNoun@");
  v.push_back("g");
  v.push_back("@SPECIALCHAR@");

  HfstOneLevelPath pdog(0.0, v);
  getstring(pdog, res);
  assert(res == "dog");

  v.clear();
  v.push_back("@U.NeedNoun.On@");
  v.push_back("c");
  v.push_back("a");
  v.push_back("@C.NeedNoun@");
  v.push_back("t");
  v.push_back("@SPECIALCHAR@");
  HfstOneLevelPath pcat(0.0, v);

  HfstOneLevelPaths ps;
  ps.insert(pcat);
  ps.insert(pdog);
  HfstOneLevelPath match;

  assert(get_first_match(ps, "cat", match));
  assert(match == pcat);

  assert(!get_first_match(ps, "elephant", match));
  
  assert(is_matching_pair(epsilon, epsilon, epsilon, epsilon));
  assert(is_matching_pair("a", epsilon, epsilon, epsilon));
  assert(is_matching_pair("a", "a", epsilon, epsilon));
  assert(is_matching_pair("a", "a", "b", "b"));
  assert(is_matching_pair("a", epsilon, "b", "b"));
  assert(is_matching_pair("@U.NeedNoun.ON@", "a", 
			  "@U.NeedNoun.ON@", "@U.NeedNoun.ON@"));

  HfstBasicTransducer b;
  StringVector ia(3, "a");
  StringVector ob(3, "");
  ob.push_back("b");
  ob.push_back("b");
  ob.push_back("b");

  HfstTwoLevelPath aligned;
  assert(not align_paths(HfstOneLevelPath(0, ia),
			 HfstOneLevelPath(0, ob),
			 b, aligned));

  HfstState a_target = b.add_state();
  HfstState b_target = b.add_state();
  b.set_final_weight(b_target, 0.0);

  b.add_transition(0, HfstBasicTransition(a_target, "a", epsilon, 0.0));
  b.add_transition(a_target, 
		   HfstBasicTransition(a_target, "a", epsilon, 0.0));
  b.add_transition(a_target, 
		   HfstBasicTransition(b_target, epsilon, "b", 0.0));
  b.add_transition(b_target, 
		   HfstBasicTransition(b_target, epsilon, "b", 0.0));

  assert(align_paths(HfstOneLevelPath(0, ia),
		     HfstOneLevelPath(0, ob),
		     b, aligned));

  assert(aligned.second.size() == 6);
  assert(aligned.second[0] == StringPair("a", epsilon));
  assert(aligned.second[1] == StringPair("a", epsilon));
  assert(aligned.second[2] == StringPair("a", epsilon));
  assert(aligned.second[3] == StringPair(epsilon, "b"));
  assert(aligned.second[4] == StringPair(epsilon, "b"));
  assert(aligned.second[5] == StringPair(epsilon, "b"));

  ia = StringVector(3, "a");
  ob = StringVector(3, "b");
  ob.push_back("@U.NeedNoun.ON@");
  HfstBasicTransducer bb;
  bb.add_transition(0, HfstBasicTransition(0, "a", "b", 0.0));
  HfstState f = bb.add_state();
  bb.add_transition(0, HfstBasicTransition(f, epsilon, "@U.NeedNoun.ON@",0.0));
  bb.set_final_weight(f, 0.0);
  aligned = HfstTwoLevelPath();
  assert(align_paths(HfstOneLevelPath(0, ia),
		     HfstOneLevelPath(0, ob),
		     bb, aligned));
  assert(aligned.second.size() == 4);
  assert(aligned.second[0] == StringPair("a", "b"));
  assert(aligned.second[1] == StringPair("a", "b"));
  assert(aligned.second[2] == StringPair("a", "b"));
  assert(aligned.second[3] == StringPair(epsilon, "@U.NeedNoun.ON@"));
  assert(incr_path_weight(aligned, bb, 2));

  HfstBasicTransducer::HfstTransitions tr = bb[0];

  for (const HfstBasicTransition &trans : tr)
    {
      if (trans.get_input_symbol() == "a")
	{ 
	  assert(trans.get_weight() == 6.0); 
	}
      if (trans.get_input_symbol() == "@U.NeedNoun.ON@")
	{ assert(trans.get_weight() == 2.0); }
    }
  
  assert(bb.get_final_weight(f) == 2);

  HfstOneLevelPath p1(1, StringVector(1, "a"));
  HfstOneLevelPath p2(2, StringVector(1, "b"));
  HfstOneLevelPath p3(3, StringVector(1, "c"));
  HfstOneLevelPaths paths;
  paths.insert(p1);
  paths.insert(p2);
  paths.insert(p3);

  HfstOneLevelPath best_path;

  get_heaviest_path(paths, best_path);

  assert(best_path.first == 3);
  assert(best_path.second == StringVector(1, "c"));
}

#endif // TEST_path_utils_cc
