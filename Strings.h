#ifndef STRINGS_H
#define STRINGS_H

#include <vector>

class Strings: public std::vector<std::string>
{
public:
  Strings operator+=(const Strings& s2)
  {
    for (const std::string& s : s2)
      push_back(s);
    return *this;
  }
  Strings operator+=(const std::string& s)
  {
    push_back(s);
    return *this;
  }
};


#endif
