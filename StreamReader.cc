#include "StreamReader.hh"

#ifndef TEST_StreamReader_cc

StreamReader::StreamReader(void):
  pin(&std::cin),
  counter(0)
{};

StreamReader::StreamReader(const std::string &fn):
  in(fn.c_str()),
  pin(0),
  counter(0)
{}

StreamReader::StreamReader(std::istream &in):
  pin(&in),
  counter(0)
{}

const std::string &StreamReader::getline(void) const
{
  return line; 
}

bool StreamReader::readline(void)
{
  ++counter;

  if (pin != 0)
    {
      std::getline(*pin, line);
      return pin->good();  
    }
  else
    {
      std::getline(in, line);
      return in.good();
    }  
}

int StreamReader::getlinenumber(void)
{ return counter; }

#else // TEST_StreamReader_cc

int main(void)
{
  StreamReader sr;

  while(sr.readline())
    {
      std::cout << "CP: " << sr.getline() << std::endl;
    }
}

#endif // TEST_StreamReader_cc
