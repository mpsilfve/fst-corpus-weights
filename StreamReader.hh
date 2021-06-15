#ifndef HEADER_StreamReader_hh
#define HEADER_StreamReader_hh

#include <iostream>
#include <fstream>
#include <string>

class StreamReader
{
public:
  StreamReader(void);
  StreamReader(const std::string &fn);
  StreamReader(std::istream &in);

  bool readline(void);
  const std::string &getline(void) const;
  int getlinenumber(void);
private:
  std::ifstream in;
  std::istream * pin;
  std::string line;
  int counter;
};

#endif // HEADER_StreamReader_hh
