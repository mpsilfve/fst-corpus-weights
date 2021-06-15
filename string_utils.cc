#include "string_utils.hh"

#ifndef TEST_string_utils_cc

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
