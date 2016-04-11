#ifndef KIND_EXPIRE_TOOLS
#define KIND_EXPIRE_TOOLS

#include "DateTime.h"
#include "KindConfig.h"

DateTime imageDate(const std::string& image);

DateTime getExpireDate(const DateTime& imageTime,
                       const KindConfig& conf, std::string& rule);

void createExpireFile(const std::string& image,
                      const KindConfig& conf, std::string& rule);

DateTime expireDate(const std::string& image,
                    const KindConfig& conf, std::string& rule);

#endif
