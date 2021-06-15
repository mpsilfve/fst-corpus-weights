#ifndef HEADER_weight_utils_hh
#define HEADER_weight_utils_hh

#include <hfst/HfstTransducer.h>

using hfst::implementations::HfstBasicTransducer;
using hfst::HfstTransducer;
using hfst::HfstOneLevelPath;
using hfst::HfstOneLevelPaths;

void laplace_smooth(float alpha, HfstTransducer &t);
void normalize_locally(HfstTransducer &t);
void prob2tropical(HfstTransducer &t);
void logarithmize(HfstTransducer &t);

#endif // HEADER_weight_utils_hh
