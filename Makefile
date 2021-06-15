SHELL=bash
CXX=g++
CXXFLAGS=-Wall -Wextra -std=c++11 -Wfatal-errors -g -O3 -I/usr/local/include
LDLIBS=-lhfst
SRCS=path_utils.cc StreamReader.cc string_utils.cc weight_utils.cc
OBJS=$(SRCS:%.cc=%.o)
TESTS=$(SRCS:%.cc=TEST_%_cc)

all:weight_fst normalize_weights

clean:
	rm -f $(TESTS) $(OBJS) weight_fst normalize_weights

weight_fst:weight_fst.cc $(OBJS)
normalize_weights:normalize_weights.cc $(OBJS)

tests:$(TESTS)

TEST_%_cc:%.cc %.o
	$(CXX) $(CXXFLAGS) -DTEST_$*_cc -o $@ $^ -lhfst

