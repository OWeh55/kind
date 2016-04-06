#include <iostream>
#include "Exception.h"
#include "stringtools.h"

using namespace std;

string delSpaces(const string& s)
{
  string res = "";
  for (unsigned int i = 0; i < s.size(); ++i)
    {
      if (!isspace(s[i]))
        res += s[i];
    }
  return res;
}

void substitute(std::string& s, char c1, char c2)
{
  for (unsigned int i = 0; i < s.size(); ++i)
    if (s[i] == c1) s[i] = c2;
}

void reduceToOne(std::string& s, char c)
{
  unsigned int pr = 0;
  unsigned int pw = 0;
  while (pr < s.size())
    {
      char ac = s[pr];
      s[pw] = ac;
      ++pr;
      ++pw;
      if (ac == c)
        {
          while (pr < s.size() && s[pr] == c) // skip further characters c
            ++pr;
        }
    }
  s.resize(pw);
}

string trim(const string& s)
{
  string res = "";

  int last = s.size() - 1;
  while (last >= 0 && isspace(s[last]))
    --last;

  int first = 0;
  while (first < (int)s.size() && isspace(s[first]))
    ++first;

  for (int i = first; i <= last; ++i)
    res += s[i];
  return res;
}

bool startsWith(const string& s, const string& start)
{
  int len = start.length();
  return s.substr(0, len) == start;
}

int split(const string& s, Strings& parts, char del, int expectedParts)
{
  parts.clear();
  string p = "";
  if (!s.empty())
    {
      for (unsigned int i = 0; i < s.size(); ++i)
        {
          if (s[i] == del)
            {
              parts.push_back(p);
              p.clear();
            }
          else
            p += s[i];
        }
      parts.push_back(p);
    }
  if (expectedParts > 0 && expectedParts != (int)parts.size())
    throw Exception("split", string("Unexpected number of parts: ") + s);
  return parts.size();
}

void skipWS(const string& s, unsigned  int& i)
{
  static const string ws = " \t\n";
  while (i < s.size() && ws.find(s[i]) != string::npos)
    ++i;
}

int getInt(const string& s, unsigned int& i)
{
  if (!isdigit(s[i]))
    throw Exception("getInt", "digit expected");
  string is;
  while (i < s.size() && isdigit(s[i]))
    {
      is += s[i];
      ++i;
    }
  int res = stoi(is);
  skipWS(s, i);
  return res;
}

long int getLongInt(const string& s, unsigned int& i)
{
  if (!isdigit(s[i]))
    throw Exception("getLongInt", "digit expected");
  string is;
  while (i < s.size() && isdigit(s[i]))
    {
      is += s[i];
      ++i;
    }
  long int res = stol(is);
  skipWS(s, i);
  return res;
}

string getWord(const string& s, unsigned int& i)
{
  if (!isalpha(s[i]) && s[i] != '-' && s[i] != '_')
    throw Exception("getWord", "letter expected");
  string is;
  while (i < s.size() && (isalpha(s[i]) || s[i] == '-' || s[i] == '_'))
    {
      is += s[i];
      ++i;
    }
  skipWS(s, i);
  return is;
}

time_t stot(const string& s)
{
  unsigned int i = 0;
  skipWS(s, i);
  int val = getInt(s, i);
  if (i >= s.length())
    return val;
  string unit = getWord(s, i);
  if (unit.back() != 's')
    unit += 's';
  if (unit == "secs")
    return val;
  val *= 60;
  if (unit == "mins")
    return val;
  val *= 60;
  if (unit == "hours")
    return val;
  val *= 24;
  if (unit == "days")
    return val;
  if (unit == "weeks")
    return val * 7;
  val *= 30;
  if (unit == "months")
    return val;
  val *= 12;
  if (unit != "years")
    throw Exception("Parse time", "unknown time unit " + unit);
  return val;
}

long int getNumber(const string& l)
{
  // read *all* digits from string l ignoring all other characters
  string d;
  for (unsigned int i = 0; i < l.size(); ++i)
    if (isdigit(l[i]))
      d += l[i];
  // return long int value of digits
  return stol(d);
}

void replacePlaceHolder(string& s,
                        const string& placeholder,
                        const string& content)
{
  int psize = placeholder.size();
  size_t pos = s.find(placeholder);
  while (pos != string::npos)
    {
      s = s.substr(0, pos) + content + s.substr(pos + psize);
      pos = s.find(placeholder);
    }
}
