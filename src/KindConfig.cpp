#include <iostream>
#include <fstream>
#include <set>

#include "Exception.h"
#include "KindConfig.h"

using namespace std;

KindConfig::KindConfig(const std::string& fn)
{
  // settings.clear();
  addFile(fn);
}

void KindConfig::addFile(const std::string& fn)
{
  ifstream is(fn);
  if (!is.good())
    throw Exception("read config", string("Cannot open ") + fn + " for reading");

  string line;
  string lastkey;
  set<string> usedKeys;

  while (getline(is, line))
    {
      line = trim(line);
      if (!line.empty() && line[0] != '#')
        {
          string key, value;
          string del;
          split(line, key, del, value);
          if (key.empty())
            key = lastkey;
          if (key.empty())
            throw Exception("read config", "empty key");

          if (del == "=")
            {
              if (usedKeys.count(key) != 0)
                throw Exception("Read config file " + fn, "entry overrides existing key \"" + key + "\"");
              // clear previous entries
              settings[key].clear();
            }

          settings[key].push_back(value);
          usedKeys.insert(key);
          lastkey = key;
        }
    }
}

int KindConfig::addOneFile(const std::vector<std::string> &fns)
{
  if (fns.empty())
    throw Exception("Read config file", "file list empty");
  unsigned int i=0;
  while (!fileExists(fns[i]) && i<fns.size())
    i++;

  if (i>=fns.size())
    throw Exception("Read config file","no file found");
  addFile(fns[i]);
  return i;
}

bool KindConfig::hasKey(std::string key) const
{
  key = trim(key);
  auto it = settings.find(key);
  bool found = it != settings.end();
  if (found)
    used[key] = true;
  return found;
}

std::string KindConfig::getString(std::string key) const
{
  key = trim(key);
  auto it = settings.find(key);
  if (it == settings.end())
    throw Exception("get config key", std::string("No key \"") + key + "\"");
  used[key] = true;
  if (it->second.size() != 1)
    throw Exception("get config key", std::string("Key \"") + key + "\" is no single value");
  return it->second[0];
}

Strings KindConfig::getStrings(std::string key) const
{
  key = trim(key);
  auto it = settings.find(key);
  if (it == settings.end())
    throw Exception("get config key", std::string("No key \"") + key + "\"");
  used[key] = true;
  return it->second;
}

bool KindConfig::getBool(std::string key) const
{
  key = trim(key);
  auto it = settings.find(key);
  if (it == settings.end())
    return false;
  used[key] = true;
  if (it->second.size() != 1)
    throw Exception("get config key", std::string("Key \"") + key + "\" is no single value");
  std::string val = it->second[0];
  if (val.empty() || val == "true")
    return true;
  if (val != "false")
    throw Exception("get boolean key " + key, std::string("Invalid value \"") + val + "\"");
  return false;
}

void KindConfig::print(const string& prefix) const
{
  for (auto it = settings.begin(); it != settings.end(); ++it)
    {
      cout << prefix << it->first << ": ";
      if (it->second.size() > 1)
        for (string v : it->second)
          cout << v << "|";
      else if (!it->second.empty())
        cout << it->second[0];
      cout << endl;
    }
}

void KindConfig::warnUnused(const string& prefix) const
{
  for (auto it = settings.begin(); it != settings.end(); ++it)
    {
      if (used.count(it->first) == 0 || ! used[it->first])
        cout << "Warning: setting " << it->first << " unused" << endl;
    }
}

void KindConfig::split(const string& line,
                       string& key,
                       string& del,
                       string& value)
{
  unsigned int i = 0;
  key.clear();
  del.clear();
  value.clear();
  while (i < line.size() && line[i] != '+' && line[i] != '=')
    {
      key += line[i];
      ++i;
    }

  key = trim(key);
  while (i < line.size() && (line[i] == '+' || line[i] == '='))
    {
      del += line[i];
      ++i;
    }

  if (del != "=" && del != "+=")
    throw Exception("config", string("wrong delimiter ") + del + " in " + line);

  while (i < line.size())
    {
      value += line[i];
      ++i;
    }
  value = trim(value);
}
