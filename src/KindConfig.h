#ifndef Kind_CONFIG_H
#define Kind_CONFIG_H

//#include <fstream>
#include <map>
#include <string>
#include "Strings.h"
#include "stringtools.h"

class KindConfig
{
public:
  KindConfig() {}
  KindConfig(const std::string& fn);
  //  KindConfig(std::istream& is);

  void addFile(const std::string& fn);
  //  void addFile(std::istream& is);
  void add(const std::string& key, const std::string& value)
  {
    settings[key].push_back(value);
  }

  void setBool(const std::string& key, bool value)
  {
    settings[key].resize(1);
    settings[key][0] = value ? "true" : "false";
  }

  void setString(const std::string& key, const std::string& value)
  {
    settings[key].resize(1);
    settings[key][0] = value;
  }

  bool hasKey(std::string key) const;

  std::string getString(std::string key) const;
  Strings getStrings(std::string key) const;

  bool getBool(std::string key) const;

  void print(const std::string& prefix = "") const;
private:
  void split(const std::string& line,
             std::string& key,
             std::string& del,
             std::string& value);

  std::map<std::string, Strings> settings;
};

#endif
