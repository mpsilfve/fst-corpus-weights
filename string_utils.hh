#ifndef HEADER_string_utils_hh
#define HEADER_string_utils_hh

#include <string>
#include <vector>
#include <utility>

typedef std::vector<std::string> StringVector;
typedef std::pair<std::string, std::string> CharPair;
typedef std::vector<CharPair> CharPairVector;

void split(const std::string &str, const std::string &sep, StringVector &v);
void read_char_pairs(const std::string &str, CharPairVector &v);
bool is_flag_diacritic(const std::string &sym);

#endif // HEADER_string_utils_hh
