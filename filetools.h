#ifndef FILE_TOOLS_H
#define FILE_TOOLS_H

#include <string>
#include <vector>
#include "Strings.h"

// collect files matching fn
// if "recursive" also inspect subdirectories
void fileList(const std::string& fn,
              std::vector<std::string>& files,
              bool recursive = false);

void dirList(const std::string& fn,
             std::vector<std::string>& dirs,
             bool recursive = false);

// read list of files from commandline
// if "recursive" expand directories to list of files
void fileList(int argc, char** argv, int optind,
              std::vector<std::string>& files,
              bool recursive = false);

void strings2File(const std::vector<std::string>& s, const std::string& fn);
void file2Strings(const std::string& fn, std::vector<std::string>& s);

bool dirExists(const std::string& name);
bool fileExists(const std::string& name);

Strings myPopen(const std::string& cmd,
                int& rc, bool debug,
                const std::string& logfn = "");

void removeLock(const std::string& lockfilename);
void createLock(const std::string& lockfilename);

#endif
