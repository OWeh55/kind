#include <iostream>
#include <algorithm>

#include "Exception.h"
#include "Lexer.h"
#include "FileName.h"
#include "filetools.h"
#include "rulecomp.h"

#include "expiretools.h"

using namespace std;

void debugPrint(const std::string& s);

void readSetRules(const KindConfig& conf,
                  std::map<std::string, int >& setIdx,
                  std::vector<SetRule>& backupSetRule)
{
  Strings setRules = conf.getStrings("setRule");
  setIdx.clear();
  backupSetRule.clear();
  if (!setRules.empty())
    {
      for (const string& rule : setRules)
        {
          // read rule and store to vector
          Strings splittedRule;
          split(rule, splittedRule, ':');
          if (splittedRule.size() != 3)
            throw Exception("config", "Error in setRule: " + rule);
          string name = splittedRule[0];
          if (name == "expire")
            throw Exception("config", "Use of reserved name >>expire<< in setRule is forbidden");
          SetRule s;
          s.rule = rule;
          s.name = name;
          s.distance = stot(splittedRule[1]);
          s.keep = stot(splittedRule[2]);
          if (s.distance < 0 || s.keep < 0)
            throw Exception("SetRules", "Times must be positive");
          backupSetRule.push_back(s);
        }
      // sort backupSets by time to keep
      sort(backupSetRule.begin(), backupSetRule.end());

      // create map for index
      for (unsigned int i = 0; i < backupSetRule.size(); ++i)
        setIdx[backupSetRule[i].name] = i;
    }
}

void stringToDate(const string& dateString, DateTime& t, string& label)
{
  Strings ss;
  split(dateString, ss, '-');
  if (ss.size() < 5)
    throw Exception("stringToDate", "date format invalid");
  label = ss[0];
  int Y = stoi(ss[1]);
  int M = stoi(ss[2]);
  int D = stoi(ss[3]);
  int h = stoi(ss[4]);
  int m = 0, s = 0;
  try    // longImageName
    {
      if (ss.size() > 5)
        m = stoi(ss[5]);
      if (ss.size() > 6)
        s = stoi(ss[6]);
    }
  catch (const std::invalid_argument& ex)
    {
      // ignore this. image name may contain backup set name
    }
  t = DateTime(Y, M, D, h, m, s);
}

void parseRule(string rule,
               set<int>& M, set<int>& D, set<int>& W, set<int>& h,
               time_t& exptime)
{
  for (unsigned int i = 0; i < rule.size(); ++i)
    rule[i] = tolower(rule[i]);

  substitute(rule, ' ', ',');
  reduceToOne(rule, ',');

  // rule = hour wday mday month <exptime>
  Lexer p(rule);

  h = getValues(p, 0, 23); // hour

  p.expect(',');

  W = getValues(p, 0, 7, 1); // wday

  p.expect(',');

  D = getValues(p, 1, 31); // day of month

  p.expect(',');

  M = getValues(p, 1, 12, 2); // month

#if 0
  // debug-output
  cout << "hour: ";
  for (int i : h)
    cout << i << " ";
  cout << endl;
  cout << "wday: ";
  for (int i : W)
    cout << i << " ";
  cout << endl;
  cout << "mday: ";
  for (int i : D)
    cout << i << " ";
  cout << endl;
  cout << "month: ";
  for (int i : M)
    cout << i << " ";
  cout << endl;
#endif

  string ts = p.getAll();
  substitute(ts, ',', ' ');
  exptime = stot(ts);
  if (exptime < 0)
    throw Exception("ExpireRule " + rule, "Time period must be positive");
}


DateTime getExpireDate(const DateTime& imageTime,
                       const KindConfig& conf, string& rule)
{
  DateTime expireTime;
  rule.clear();
  Strings expireRules = conf.getStrings("expireRule");
  for (unsigned int k = 0; k < expireRules.size(); ++k)
    {
      debugPrint("Checking rule " + expireRules[k]);

      set<int> M, D, W, h;
      set<int> Y, m, s;
      time_t expirePeriod;
      parseRule(expireRules[k], M, D, W, h, expirePeriod);

      if (imageTime.match(Y, M, D, W, h, m, s))
        {
          debugPrint("match");
          expireTime = imageTime + expirePeriod;
          rule = expireRules[k];
          // continue search: last rule matches
        }
    }
  if (rule.empty())
    throw Exception("expire", "no rule found");
  return expireTime;
}

