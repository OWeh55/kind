#include <iostream>
#include "Exception.h"
#include "stringtools.h"
#include "Lexer.h"

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

double getDouble(const string& s, unsigned int& i)
{
  if (!isdigit(s[i]) && s[i]!='-')
    throw Exception("getDouble", "digit expected in " );
  string is;
  while (i < s.size() && (isdigit(s[i]) || s[i]=='-' || s[i]=='.' || s[i]=='e' || s[i]=='E'))
    {
      is += s[i];
      ++i;
    }
  double res = stof(is);
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

time_t stot(const string& str)
{
  // converts a string describing time periods to
  // time in seconds
  // numbers without units are in seconds
  // all given values are accumulated
  // use of sign is optional
  // 1 hour
  // 3 hours 5 minutes
  // 1 day -5 minutes
  // 25 -3
  Lexer s(str);
  time_t value = 0;
  while (!s.empty())
    {
      bool minus = false;
      if (s.type == Lexer::singlecharacter)
        {
          if (s.token == "-")
            minus = true;
          else if (s.token != "+")
            throw Exception("string to time", "expected sign");
          s.nextToken();
        }
      time_t thisValue = s.getInt();

      string unit = "s";
      if (s.type == Lexer::identifier)
        unit = s.getWord();
      //eliminate plural s to simplify comparisn
      if (unit.size() > 1 && unit[unit.size() - 1] == 's')
        unit.resize(unit.size() - 1);
      // check all units and common abbreviations
      if (unit == "s" || unit == "sec" || unit == "second")
        thisValue = thisValue;
      else if (unit == "min" || unit == "minute")
        thisValue = thisValue * 60;
      else if (unit == "h" || unit == "hour")
        thisValue = thisValue * 60 * 60;
      else if (unit == "d" || unit == "day")
        thisValue = thisValue * 60 * 60 * 24;
      else if (unit == "week")
        thisValue = thisValue * 60 * 60 * 24 * 7;
      else if (unit == "month")
        thisValue = thisValue * 60 * 60 * 24 * 30;
      else if (unit == "a"  || unit == "year")
        thisValue = thisValue * 60 * 60 * 24 * 365;
      else
        throw  Exception("string to time", "unknown unit " + unit);
      if (!minus)
        value += thisValue;
      else
        value -= thisValue;
    }
  return value;
}

string timeString(time_t t)
{
  string res;
  if (t < 0)
    {
      res = "- ";
      t = -t;
    }
  int sec = t % 60;
  t /= 60;
  int min = t % 60;
  t /= 60;
  int hour = t % 24;
  t /= 24;
  int days = t;
  int nParts = 0;
  if (days > 0)
    {
      res += to_string(days) + " days ";
      nParts++;
    }
  if (hour > 0)
    {
      res += to_string(hour) + " hours ";
      nParts++;
    }
  if (min > 0 && nParts < 2)
    {
      res += to_string(min) + " minutes ";
      nParts++;
    }
  if ((sec > 0 && nParts < 2) || nParts == 0)
    res += to_string(sec) + " seconds ";
  return res;
}

double getNumber(const string& l)
{
  // read *all* digits from string l ignoring all other characters
  // "read 3,000,421 Bytes" => 3000421
  string d;
  for (unsigned int i = 0; i < l.size(); ++i)
    if (isdigit(l[i]))
      d += l[i];
  // return long int value of digits
  long int res = 0;
  try
    {
      res = stof(d);
    }
  catch (...)
    {
      // we ignore overflow here because value is only informative
      cout << "stol failed on " << l << endl;
      res = -1;
    }
  return res;
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
