#ifndef Kind_CONFIG_H
#define Kind_CONFIG_H

#include <fstream>
#include <map>
#include <string>
#include "Strings.h"
#include "stringtools.h"

class KindConfig
{
public:
  KindConfig() {}
  KindConfig(const std::string& fn);
  KindConfig(std::istream& is);

  void addFile(const std::string& fn);
  void addFile(std::istream& is);
  void add(const std::string& key, const std::string& value)
  {
    settings[key].push_back(value);
  }

  bool hasKey(std::string key) const;

  std::string getString(std::string key) const;
  Strings getStrings(std::string key) const;

  bool getBool(std::string key) const;

  void print() const;
private:
  void split(const std::string& line,
             std::string& key,
             std::string& del,
             std::string& value);

  std::map<std::string, Strings> settings;
};

#endif
