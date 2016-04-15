#ifndef KIND_EXPIRE_TOOLS
#define KIND_EXPIRE_TOOLS

#include <map>
#include "DateTime.h"
#include "KindConfig.h"

struct SetRule
{
  std::string name;
  time_t distance;
  time_t keep;
  std::string rule;
  bool operator<(const SetRule& rhs) const
  {
    // !!! trick: use inverse order
    return keep > rhs.keep;
  }
};

void readSetRules(const KindConfig& conf,
                  std::map<std::string, int >& setIdx,
                  std::vector<SetRule>& backupSetRule);

DateTime imageDate(const std::string& image);

// get expire time using expire rules (not backup sets)
DateTime getExpireDate(const DateTime& imageTime,
                       const KindConfig& conf, std::string& rule);

void stringToDate(const std::string& dateString, DateTime& t, std::string& label);
#endif
