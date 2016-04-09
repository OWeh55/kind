#include <iostream>

#include "Exception.h"
#include "Lexer.h"
#include "FileName.h"
#include "filetools.h"
#include "rulecomp.h"

#include "expiretools.h"

using namespace std;

void debugPrint(const std::string& s);

DateTime stringToDate(const string& dateString)
{
  Strings ss;
  split(dateString, ss, '-');
  if (ss.size() < 5)
    throw Exception("stringToDate", "date format invalid");
  int Y = stoi(ss[1]);
  int M = stoi(ss[2]);
  int D = stoi(ss[3]);
  int h = stoi(ss[4]);
  int m = 0, s = 0;
  if (ss.size() > 5) // longImageName
    m = stoi(ss[5]);
  if (ss.size() > 6)
    s = stoi(ss[6]);
  return DateTime(Y, M, D, h, m, s);
}

DateTime imageDate(const string& image)
{
  FileName fn(image);
  return stringToDate(fn.getName());
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
}


DateTime getExpireDate(const DateTime& imageTime, const KindConfig& conf, string& rule)
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
#if 0
      cout << expireRules[k] << endl;
      for (auto v:M) cout << v << " ";
      cout << endl;
      for (auto v:D) cout << v << " ";
      cout << endl;
      for (auto v:W) cout << v << " ";
      cout << endl;
      for (auto v:h) cout << v << " ";
      cout << endl;
      cout << imageTime.getString('m') << endl;
#endif
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

void createExpireFile(const string& image, const KindConfig& conf, string& rule)
{
  string expireFileName = image + "/expires";
  // epire date from image date + rules
  DateTime imageTime = imageDate(image);
  DateTime expireTime = getExpireDate(imageTime, conf, rule);

  // create file
  ofstream xfile(expireFileName);
  // we use prefix "expire-" to allow same parsing as for imagenames
  xfile << "expire-" << expireTime.getString('m') << endl;
  xfile << rule << endl;
  xfile.close();
}

DateTime expireDate(const string& image, const KindConfig& conf, string& rule)
{
  string expireFileName = image + "/expires";
  if (!fileExists(expireFileName)) // lost expire file ?
    {
      cout << "Recreate expire file " << expireFileName << endl;
      createExpireFile(image, conf, rule);
    }

  debugPrint("reading " + expireFileName);
  Strings s;
  file2Strings(expireFileName, s);
  if (s.empty())
    throw Exception("expireDate", "expire empty");
  if (s.size() > 1)
    rule = s[1];
  return stringToDate(s[0]);
}

