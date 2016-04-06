#include <iostream>

#include "Exception.h"
#include "KindConfig.h"

using namespace std;

KindConfig::KindConfig(const std::string& fn)
{
  // settings.clear();
  addFile(fn);
}

KindConfig::KindConfig(std::istream& is)
{
  // settings.clear();
  addFile(is);
}

void KindConfig::addFile(std::istream& is)
{
  string line;
  string lastkey;
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
            settings[key].clear();

          settings[key].push_back(value);
          lastkey = key;
        }
    }
}

void KindConfig::addFile(const std::string& fn)
{
  ifstream is(fn);
  if (!is.good())
    throw Exception("read config", string("Cannot open ") + fn + " for reading");
  addFile(is);
}

bool KindConfig::hasKey(std::string key) const
{
  key = trim(key);
  auto it = settings.find(key);
  return it != settings.end();
}

std::string KindConfig::getString(std::string key) const
{
  key = trim(key);
  auto it = settings.find(key);
  if (it == settings.end())
    throw Exception("get config key", std::string("No key \"") + key + "\"");
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
  return it->second;
}

bool KindConfig::getBool(std::string key) const
{
  key = trim(key);
  auto it = settings.find(key);
  if (it == settings.end())
    return false;
  if (it->second.size() != 1)
    throw Exception("get config key", std::string("Key \"") + key + "\" is no single value");
  std::string val = it->second[0];
  if (val.empty() || val == "true")
    return true;
  if (val != "false")
    throw Exception("get boolean key " + key, std::string("Invalid value \"") + val + "\"");
  return false;
}

void KindConfig::print() const
{
  for (auto it = settings.begin(); it != settings.end(); ++it)
    {
      cout << it->first << ": ";
      if (it->second.size() > 1)
        for (string v : it->second)
          cout << v << "|";
      else if (!it->second.empty())
        cout << it->second[0];
      cout << endl;
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
