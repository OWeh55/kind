#include <iostream>
#include <set>
#include <vector>
#include <algorithm>

#include "Lexer.h"
#include "rulecomp.h"

using namespace std;

vector<vector<string> > weekDayString
{
  {"sunday", "sun"},
  {"monday", "mon"},
  {"tuesday", "tue"},
  {"wednesday", "wed"},
  {"thursday", "thu"},
  {"friday", "fri"},
  {"saturday", "sat"}
};

vector<vector<string> > monthString
{
  {"dummy"},
  {"january", "jan"},
  {"febrary", "feb"},
  {"march", "mar"},
  {"april", "apr"},
  {"may"},
  {"june", "jun"},
  {"july", "jul"},
  {"august", "aug"},
  {"september", "sep"},
  {"october", "oct"},
  {"november", "nov"},
  {"december", "dec"}
};

int getNr(const string& w, const vector<vector<string>>& dict)
{
  int res = -1;
  for (unsigned int i = 0; res < 0 && i < dict.size(); ++i)
    {
      const vector<string>& entry = dict[i];
      if (find(entry.begin(), entry.end(), w) != entry.end())
        res = i;
    }
  if (res < 0)
    throw Exception("Range", "unknown identifier: " + w);
  return res;
}

int getNr(Lexer& p, int altstring)
{
  if (p.type == Lexer::integer)
    return p.getInt();
  else if (altstring != 0 && p.type == Lexer::identifier)
    {
      string w = p.getWord();
      int res = getNr(w, altstring == 1 ? weekDayString : monthString);
      return res;
    }
  else
    throw Exception("getNr", "int or identifier expected");
}

std::set<int> getValues(Lexer& p, int first, int last, const int altstring)
{
  int mod = 1;
  set<int> values;

  if (p.token == "*")
    {
      p.nextToken();
      if (p.token == "/")
        {
          p.nextToken();
          mod = p.getInt();
          if (mod <= 0)
            throw Exception("Range", string("Modulo value invalid ") + to_string(mod));
        }
      for (int i = first; i <= last; i += mod)
        values.insert(i);
    }
  else
    {
      int s = getNr(p, altstring);
      if (s < first || s > last)
        throw Exception("Range", "value out of range");
      if (p.token != "-")
        values.insert(s);
      else
        {
          p.nextToken();
          // cout << p.token << endl;
          int e =  getNr(p, altstring);
          if (e < s || e > last)
            throw Exception("Range", "end value out of range");
          if (p.token == "/")
            {
              p.nextToken();
              mod = p.getInt();
            }
          for (int i = s; i <= e; i += mod)
            values.insert(i);
        }
    }
  return values;
}

set<int> getValues(const string& range, int first, int last, int altstringmode) // range is string without spaces
{
  if (range.empty())
    throw Exception("Range", "Description is empty");
  Lexer p(range);
  return getValues(p, first, last, altstringmode);
}
