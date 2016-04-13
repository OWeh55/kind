#ifndef KIND_EXPIRE_TOOLS
#define KIND_EXPIRE_TOOLS

#include <map>
#include "DateTime.h"
#include "KindConfig.h"

void readSetRules(const KindConfig &conf,
		  std::map<std::string, std::pair<time_t, time_t> > &ruleSet,
		  std::map<std::string, std::string> &backupSetRule);

DateTime imageDate(const std::string& image);

// get expire time using expire rules (not backup sets)
DateTime getExpireDate(const DateTime& imageTime,
                       const KindConfig& conf, std::string& rule);

void stringToDate(const std::string& dateString, DateTime& t, std::string& label);
#endif
