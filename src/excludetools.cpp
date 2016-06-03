#include "kind.h"
#include "Exception.h"
#include "FileName.h"
#include "filetools.h"
#include "excludetools.h"

using namespace std;

Strings getExclusions(const KindConfig& conf, bool shellMode)
{
  Strings exclusions;
  if (conf.hasKey("exclude"))
    exclusions += conf.getStrings("exclude");

  if (shellMode)
    {
      string path = conf.getString("path");
      string remoteShell = conf.getString("remoteShell");
      string userAtHost = conf.getString("user") + "@" + conf.getString("host");
      string rshCommand = remoteShell;
      if (remoteShell.empty())
        rshCommand = "ssh";

      rshCommand += " " + userAtHost;

      string userExcludeCommand = conf.getString("userExcludeCommand");

      if (!userExcludeCommand.empty())
        {
          replacePlaceHolder(userExcludeCommand, "%path", conf.getString("path"));

          verbosePrint("searching for exclusions (" + userExcludeCommand + ")");

          int rc;
          Strings excludedFiles = remoteExec(rshCommand, userExcludeCommand, rc, debug);
          if (rc > 0)
            {
              // return Strings should contain error messages
              throw Exception("Find excludes", "Search for excludes failed");
            }

          for (unsigned int i = 0; i < excludedFiles.size(); ++i)
            {
              FileName fn(excludedFiles[i]);
              exclusions.push_back('/' + fn.getPath());
              debugPrint("Excluding: " + exclusions.back());
            }
        }

      string userExcludeFile = conf.getString("userExcludeFile");
      if (!userExcludeFile.empty())
        {
          userExcludeFile = path + "/" + userExcludeFile;
          string getExcludeFileCommand = " \" if [ -f '" + userExcludeFile + "' ]; then ";
          getExcludeFileCommand += " cat '" + userExcludeFile + "' ; fi \"";
          // cout << getExcludeFileCommand << endl;
          int rc;
          Strings excludes2 = remoteExec(rshCommand, getExcludeFileCommand, rc, debug);
          if (rc == 0)
            exclusions += excludes2;
        } // if (shellMode)
    }
  return exclusions;
}

