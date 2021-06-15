#include "weight_utils.hh"

#ifndef TEST_weight_utils_cc

#include <cmath>
#include <limits>

#include "path_utils.hh"

using hfst::HfstBasicTransition;
using hfst::implementations::HfstState;

// This is VERY ugly...
float add_alpha = 1;
float smooth(float f)
{ return f + add_alpha; }

void laplace_smooth(float alpha, 
		    HfstTransducer &t) 
{ 
  add_alpha = alpha;
  t.transform_weights(smooth); 
}

void normalize_locally(HfstTransducer &t)
{
  HfstBasicTransducer res(t);

  for (HfstState s = 0; s <= res.get_max_state(); ++s)
    {
      float total_weight = (res.is_final_state(s) ? res.get_final_weight(s) : 0);

      for (const HfstBasicTransition &t : res[s])
	{ 
	  total_weight += t.get_weight();
	}

      for (HfstBasicTransition &t : res.transitions(s))
	{ 
	  float weight = (total_weight == 0 ? 
			  0 : 
			  t.get_weight()/total_weight);

	  t.set_weight(weight);
	}

      if (res.is_final_state(s))
	{ res.set_final_weight(s, (total_weight == 0 ?
				   0 :
				   res.get_final_weight(s) / total_weight)); }
    }

  t = HfstTransducer(res, t.get_type());
}

float p2t(float f)
{ return -log(f); }

void prob2tropical(HfstTransducer &t)
{ t.transform_weights(p2t); }

// Required because cmath log is double log(double).
float float_log(float f)
{ return log(f); }

void logarithmize(HfstTransducer &t)
{ t.transform_weights(float_log); }

#else // TEST_weight_utils_cc

#include <cassert>
#include <iostream>

using hfst::implementations::HfstBasicTransducer;
using hfst::implementations::HfstState;
using hfst::implementations::HfstBasicTransition;
using hfst::StringVector;
using hfst::HfstOneLevelPaths;
using hfst::HfstOneLevelPath;
using hfst::TROPICAL_OPENFST_TYPE;

int main(void)
{
  HfstBasicTransducer b;
  HfstState s = b.add_state();
  b.set_final_weight(s, 2);
  b.add_transition(0, HfstBasicTransition(s, "a", "a", 0));

  HfstTransducer t(b, TROPICAL_OPENFST_TYPE);
  laplace_smooth(0.5, t);
  b = HfstBasicTransducer(t);

  assert(b[0].size() == 1);
  assert(b[0][0].get_input_symbol() == "a");
  assert(b[0][0].get_output_symbol() == "a");
  assert(b[0][0].get_target_state() == s);
  assert(b[0][0].get_weight() == 0.5);
  assert(!b.is_final_state(0));
  assert(b.is_final_state(s));
  assert(b.get_final_weight(s) == 2.5);

  HfstBasicTransducer bb;
  s = bb.add_state();
  HfstState ss = bb.add_state();
  bb.add_transition(0, HfstBasicTransition(s, "a", "a", 1));
  bb.add_transition(0, HfstBasicTransition(ss, "a", "a", 1));
  bb.set_final_weight(0, 2);
  bb.add_transition(s, HfstBasicTransition(s, "a", "a", 1));
  bb.set_final_weight(s, 0);

  HfstTransducer tt(bb, TROPICAL_OPENFST_TYPE);
  normalize_locally(tt);
  bb = HfstBasicTransducer(tt);
  assert(bb[0].size() == 2);
  assert(bb[0][0].get_weight() == 0.25);
  assert(bb[0][1].get_weight() == 0.25);
  assert(bb.get_final_weight(0) == 0.5);
  assert(bb[s].size() == 1);
  assert(bb[s][0].get_weight() == 1);
  assert(bb.get_final_weight(s) == 0);
}


#endif // TEST_weight_utils_cc
