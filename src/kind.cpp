# 1 "kind.ag"
#include <dirent.h>
# 2 "kind.ag"
#include <sys/stat.h>
# 3 "kind.ag"
#include <cstring>
# 4 "kind.ag"
#include <unistd.h>
# 5 "kind.ag"

# 6 "kind.ag"
#include <iostream>
# 7 "kind.ag"
#include <fstream>
# 8 "kind.ag"

# 9 "kind.ag"
#include <string>
# 10 "kind.ag"
#include <vector>
# 11 "kind.ag"
#include <set>
# 12 "kind.ag"

# 13 "kind.ag"
#include <algorithm>
# 14 "kind.ag"

# 15 "kind.ag"
#include "stringtools.h"
# 16 "kind.ag"
#include "Exception.h"
# 17 "kind.ag"

# 18 "kind.ag"
#include "DateTime.h"
# 19 "kind.ag"
#include "Strings.h"
# 20 "kind.ag"
#include "FileName.h"
# 21 "kind.ag"

# 22 "kind.ag"
#include "KindConfig.h"
# 23 "kind.ag"
#include "filetools.h"
# 24 "kind.ag"
#include "Lexer.h"
# 25 "kind.ag"
#include "rulecomp.h"
# 26 "kind.ag"

# 27 "kind.ag"
#include "expiretools.h"
# 28 "kind.ag"

# 29 "kind.ag"
/*AppGen
# 30 "kind.ag"
  %%  Beschreibung des Programmes:
# 31 "kind.ag"
  prog: archiving backup
# 32 "kind.ag"
  %% Beschreibung Parameter
# 33 "kind.ag"
  % symbolischerName, Art, Typ,   Variablenname, Erklärung, Default-Wert
# 34 "kind.ag"
  para: vault_or_group, required, string, vault, Vault to backup
# 35 "kind.ag"
  %% Beschreibung der Optionen
# 36 "kind.ag"
  % kurz-Option, lang-Option, Typ, Variablenname, Erklärung, Default-Wert
# 37 "kind.ag"

# 38 "kind.ag"
  opt: f, full, void, fullImage, Force full image == initial backup, false
# 39 "kind.ag"
  opt: c, masterconfig, string, masterConfig, Master config file, ""
# 40 "kind.ag"
  opt2: if not given or empty kind looks for
# 41 "kind.ag"
  opt2:   /etc/kind/master.conf
# 42 "kind.ag"
  opt2:   /ffp/etc/kind/master.conf
# 43 "kind.ag"
  opt: B, backup, void, doBackup, Backup, false
# 44 "kind.ag"
  opt: E, expire, void, doExpire, Expire, false
# 45 "kind.ag"
  opt: C, listconfig, void, listConfig, Show configuration, false
# 46 "kind.ag"
  opt2:   if none of backup, expire or listconfig is specified,
# 47 "kind.ag"
  opt2:   backup and expire is assumed.
# 48 "kind.ag"
  opt: D, dryrun, Void, dryRun, Dry run (no real backup), false
# 49 "kind.ag"
  opt: v, verbose, Void, verbose,  Verbose,  false
# 50 "kind.ag"
  opt: d, debug, Void, debug, Debug output of many data, false
# 51 "kind.ag"
  opt: q, quiet, Void, quiet, Be quiet - no messages, false
# 52 "kind.ag"
  opt: h, help, usage, ignored , This help
# 53 "kind.ag"
AppGen*/
# 54 "kind.ag"

# 55 "kind.ag"
using namespace std;
# 56 "kind.ag"

# 57 "kind.ag"
/*AppGen:Global*/
#include <getopt.h>
#include <string>
#include <string>
bool dryRun = false;
bool verbose = false;
bool debug = false;
bool quiet = false;
/*AppGen:GlobalEnd*/
# 58 "kind.ag"

# 59 "kind.ag"
Strings banks;
# 60 "kind.ag"

# 61 "kind.ag"
typedef pair<long int, long int> Sizes;
# 62 "kind.ag"
map<string, Sizes> sizes;
# 63 "kind.ag"

# 64 "kind.ag"
void readSizes(const string& logSizeFile)
# 65 "kind.ag"
{
# 66 "kind.ag"
  if (fileExists(logSizeFile))
# 67 "kind.ag"
    {
# 68 "kind.ag"
      vector<string> ss;
# 69 "kind.ag"
      file2Strings(logSizeFile, ss);
# 70 "kind.ag"
      for (const string& s : ss)
# 71 "kind.ag"
        {
# 72 "kind.ag"
          unsigned int i = 0;
# 73 "kind.ag"
          string v = getWord(s, i);
# 74 "kind.ag"
          long int s1 = getLongInt(s, i);
# 75 "kind.ag"
          long int s2 = getLongInt(s, i);
# 76 "kind.ag"
          sizes[v] = Sizes(s1, s2);
# 77 "kind.ag"
        }
# 78 "kind.ag"
    }
# 79 "kind.ag"
}
# 80 "kind.ag"

# 81 "kind.ag"
void writeSizes(const string logSizeFile)
# 82 "kind.ag"
{
# 83 "kind.ag"
  if (!logSizeFile.empty())
# 84 "kind.ag"
    {
# 85 "kind.ag"
      Strings st;
# 86 "kind.ag"
      for (auto s : sizes)
# 87 "kind.ag"
        {
# 88 "kind.ag"
          string h = s.first + " " + to_string(s.second.first) + " " + to_string(s.second.second);
# 89 "kind.ag"
          st.push_back(h);
# 90 "kind.ag"
        }
# 91 "kind.ag"
      strings2File(st, logSizeFile);
# 92 "kind.ag"
    }
# 93 "kind.ag"
}
# 94 "kind.ag"

# 95 "kind.ag"
void verbosePrint(const string& text)
# 96 "kind.ag"
{
# 97 "kind.ag"
  if (verbose)
# 98 "kind.ag"
    cout << "  " << text << endl;
# 99 "kind.ag"
}
# 100 "kind.ag"

# 101 "kind.ag"
void debugPrint(const string& text)
# 102 "kind.ag"
{
# 103 "kind.ag"
  if (debug)
# 104 "kind.ag"
    cout << "    " << text << endl;
# 105 "kind.ag"
}
# 106 "kind.ag"

# 107 "kind.ag"
void readMasterConfig1(const string& fn, KindConfig& conf)
# 108 "kind.ag"
{
# 109 "kind.ag"
  verbosePrint("reading master config " + fn);
# 110 "kind.ag"
  conf.addFile(fn);
# 111 "kind.ag"
}
# 112 "kind.ag"

# 113 "kind.ag"
void readMasterConfig(const string& fn, KindConfig& conf)
# 114 "kind.ag"
{
# 115 "kind.ag"
  if (!fn.empty())  // master config given by user on commandline
# 116 "kind.ag"
    readMasterConfig1(fn, conf);
# 117 "kind.ag"
  else if (fileExists("/etc/kind/master.conf"))
# 118 "kind.ag"
    readMasterConfig1("/etc/kind/master.conf", conf);
# 119 "kind.ag"
  else if (fileExists("/ffp/etc/kind/master.conf"))
# 120 "kind.ag"
    readMasterConfig1("/ffp/etc/kind/master.conf", conf);
# 121 "kind.ag"
  else
# 122 "kind.ag"
    throw Exception("MasterConfig", "no file");
# 123 "kind.ag"
}
# 124 "kind.ag"

# 125 "kind.ag"
string findVault(const string& v)
# 126 "kind.ag"
{
# 127 "kind.ag"
  bool found = false;
# 128 "kind.ag"
  FileName fn;
# 129 "kind.ag"
  fn.setName(v);
# 130 "kind.ag"
  for (unsigned int i = 0; !found && i < banks.size(); ++i)
# 131 "kind.ag"
    {
# 132 "kind.ag"
      fn.setPath(banks[i]);
# 133 "kind.ag"
      if (dirExists(fn.getFileName()))
# 134 "kind.ag"
        found = true;
# 135 "kind.ag"
    }
# 136 "kind.ag"
  if (!found)
# 137 "kind.ag"
    throw Exception("find vault", v + " not found");
# 138 "kind.ag"
  verbosePrint("using vault " + fn.getFileName());
# 139 "kind.ag"
  return fn.getFileName();
# 140 "kind.ag"
}
# 141 "kind.ag"

# 142 "kind.ag"
void readVaultConfig(const string& vault, KindConfig& conf)
# 143 "kind.ag"
{
# 144 "kind.ag"
  string vaultpath = findVault(vault);
# 145 "kind.ag"
  const string& vaultConfigName = vaultpath + '/' + conf.getString("vaultConfigName");
# 146 "kind.ag"
  verbosePrint("reading vault config:");
# 147 "kind.ag"
  verbosePrint("  " + vaultConfigName);
# 148 "kind.ag"
  conf.addFile(vaultConfigName);
# 149 "kind.ag"
}
# 150 "kind.ag"

# 151 "kind.ag"
string getImageName(const KindConfig& conf)
# 152 "kind.ag"
{
# 153 "kind.ag"
  bool nonPortable = false;
# 154 "kind.ag"
  string res = conf.getString("imageName");
# 155 "kind.ag"
  for (unsigned int i = 0; !nonPortable && i < res.size(); ++i)
# 156 "kind.ag"
    {
# 157 "kind.ag"
      char c = res[i];
# 158 "kind.ag"
      if (!isalnum(c) && c != '.' && c != '_')
# 159 "kind.ag"
        nonPortable = true;
# 160 "kind.ag"
    }
# 161 "kind.ag"
  if (nonPortable)
# 162 "kind.ag"
    throw Exception("getImageName", "Invalid character in image name " + res);
# 163 "kind.ag"
  return res;
# 164 "kind.ag"
}
# 165 "kind.ag"

# 166 "kind.ag"
Images findImages(const string& vaultpath, const KindConfig& conf, bool all)
# 167 "kind.ag"
{
# 168 "kind.ag"
  Strings dirs;
# 169 "kind.ag"
  debugPrint("searching images in " + vaultpath);
# 170 "kind.ag"
  dirList(vaultpath, dirs);
# 171 "kind.ag"

# 172 "kind.ag"
  Images imageList;
# 173 "kind.ag"
  for (string dir : dirs)
# 174 "kind.ag"
    {
# 175 "kind.ag"
      FileName fn(dir);
# 176 "kind.ag"
      string imgname = getImageName(conf);
# 177 "kind.ag"
      if (startsWith(fn.getName(), imgname))
# 178 "kind.ag"
        {
# 179 "kind.ag"
          debugPrint("Checking " + dir);
# 180 "kind.ag"
          Image image(dir);
# 181 "kind.ag"

# 182 "kind.ag"
          if (all || image.valid)
# 183 "kind.ag"
            imageList.push_back(image);
# 184 "kind.ag"
        }
# 185 "kind.ag"
    }
# 186 "kind.ag"
  if (imageList.size() > 1)
# 187 "kind.ag"
    sort(imageList.begin(), imageList.end());
# 188 "kind.ag"
  return imageList;
# 189 "kind.ag"
}
# 190 "kind.ag"

# 191 "kind.ag"
void doBackup(const string& vault,
# 192 "kind.ag"
              const string& imageFullName,
# 193 "kind.ag"
              const string& referenceImage,
# 194 "kind.ag"
              const KindConfig& conf)
# 195 "kind.ag"
{
# 196 "kind.ag"
  // create image path
# 197 "kind.ag"
  if (!dryRun)
# 198 "kind.ag"
    if (mkdir(imageFullName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
# 199 "kind.ag"
      throw Exception("Create image", "failed to create " + imageFullName);
# 200 "kind.ag"

# 201 "kind.ag"
  // error message
# 202 "kind.ag"
  // we write an generic error message to mark backup as unsuccessful
# 203 "kind.ag"
  // will be deleted at successful end of rsync
# 204 "kind.ag"
  string errorfile = imageFullName + "/error";
# 205 "kind.ag"
  if (!dryRun)
# 206 "kind.ag"
    {
# 207 "kind.ag"
      ofstream error(errorfile);
# 208 "kind.ag"
      error << "failed" << endl;
# 209 "kind.ag"
      error.close();
# 210 "kind.ag"
    }
# 211 "kind.ag"

# 212 "kind.ag"
  // create source descriptor
# 213 "kind.ag"
  string host;
# 214 "kind.ag"
  if (conf.hasKey("host"))
# 215 "kind.ag"
    host = conf.getString("host");
# 216 "kind.ag"

# 217 "kind.ag"
  string server;
# 218 "kind.ag"
  if (conf.hasKey("server"))
# 219 "kind.ag"
    server = conf.getString("server");
# 220 "kind.ag"

# 221 "kind.ag"
  if (!host.empty() && !server.empty())
# 222 "kind.ag"
    throw Exception("backupVault", "Cannot have host and server");
# 223 "kind.ag"

# 224 "kind.ag"
  string path = conf.getString("path");
# 225 "kind.ag"
  if (path.empty())
# 226 "kind.ag"
    throw Exception("rsync", "empty source path");
# 227 "kind.ag"
  if (path.back() != '/')
# 228 "kind.ag"
    path += '/';
# 229 "kind.ag"

# 230 "kind.ag"
  string rsyncCmd = "rsync -vrltH --delete --stats -D --numeric-ids ";
# 231 "kind.ag"
  if (!conf.getBool("ignorePermission"))
# 232 "kind.ag"
    rsyncCmd += "-pgo";
# 233 "kind.ag"
  vector<string> rso = conf.getStrings("rsyncOption");
# 234 "kind.ag"
  for (const string& opt : rso)
# 235 "kind.ag"
    rsyncCmd += opt + " ";
# 236 "kind.ag"

# 237 "kind.ag"
  // excludes
# 238 "kind.ag"
  Strings excluded;
# 239 "kind.ag"

# 240 "kind.ag"
  if (conf.hasKey("exclude"))
# 241 "kind.ag"
    excluded += conf.getStrings("exclude");
# 242 "kind.ag"

# 243 "kind.ag"
  if (!host.empty())  // shell mode
# 244 "kind.ag"
    {
# 245 "kind.ag"
      // cout << "USING SHELLMODE '" << host << "'" << endl;
# 246 "kind.ag"
      string remoteShell = conf.getString("remoteShell");
# 247 "kind.ag"
      string userAtHost = conf.getString("user") + "@" +
# 248 "kind.ag"
                          conf.getString("host");
# 249 "kind.ag"
      string rshCommand = remoteShell;
# 250 "kind.ag"
      if (remoteShell.empty())
# 251 "kind.ag"
        rshCommand = "ssh";
# 252 "kind.ag"

# 253 "kind.ag"
      rshCommand += " " + userAtHost;
# 254 "kind.ag"

# 255 "kind.ag"
      string userExcludeCommand = conf.getString("userExcludeCommand");
# 256 "kind.ag"

# 257 "kind.ag"
      if (!userExcludeCommand.empty())
# 258 "kind.ag"
        {
# 259 "kind.ag"
          replacePlaceHolder(userExcludeCommand, "%path", path);
# 260 "kind.ag"
          string excludeCommand = rshCommand + " " + userExcludeCommand;
# 261 "kind.ag"

# 262 "kind.ag"
          verbosePrint("searching for exclusions (" + excludeCommand + ")");
# 263 "kind.ag"

# 264 "kind.ag"
          int rc;
# 265 "kind.ag"
          Strings excludedFiles = myPopen(excludeCommand, rc, debug);
# 266 "kind.ag"
          if (rc > 0)
# 267 "kind.ag"
            throw Exception("Find exludes", "Search for excludes failed");
# 268 "kind.ag"

# 269 "kind.ag"
          for (unsigned int i = 0; i < excludedFiles.size(); ++i)
# 270 "kind.ag"
            {
# 271 "kind.ag"
              FileName fn(excludedFiles[i]);
# 272 "kind.ag"
              excluded.push_back('/' + fn.getPath());
# 273 "kind.ag"
              debugPrint("Excluding: " + excluded.back());
# 274 "kind.ag"
            }
# 275 "kind.ag"
        }
# 276 "kind.ag"

# 277 "kind.ag"
      string userExcludeFile = conf.getString("userExcludeFile");
# 278 "kind.ag"
      if (!userExcludeFile.empty())
# 279 "kind.ag"
        {
# 280 "kind.ag"
          userExcludeFile = path + userExcludeFile;
# 281 "kind.ag"
          string getExcludeFileCommand = rshCommand;
# 282 "kind.ag"
          getExcludeFileCommand += " \" if [ -f '" + userExcludeFile + "' ]; then ";
# 283 "kind.ag"
          getExcludeFileCommand += " cat '" + userExcludeFile + "' ; fi \"";
# 284 "kind.ag"
          // cout << getExcludeFileCommand << endl;
# 285 "kind.ag"
          int rc;
# 286 "kind.ag"
          Strings excludes2 = myPopen(getExcludeFileCommand, rc, debug);
# 287 "kind.ag"
          if (rc == 0)
# 288 "kind.ag"
            excluded += excludes2;
# 289 "kind.ag"
        }
# 290 "kind.ag"

# 291 "kind.ag"
      if (!dryRun)
# 292 "kind.ag"
        strings2File(excluded, imageFullName + "/exclude");
# 293 "kind.ag"

# 294 "kind.ag"
      // rsync image
# 295 "kind.ag"

# 296 "kind.ag"
      if (!remoteShell.empty())
# 297 "kind.ag"
        rsyncCmd += " -e \'" + remoteShell + "\' ";
# 298 "kind.ag"

# 299 "kind.ag"
      rsyncCmd += "--exclude-from=" + imageFullName + "/exclude ";
# 300 "kind.ag"
      if (!referenceImage.empty())
# 301 "kind.ag"
        rsyncCmd += "--link-dest=" + referenceImage + "/tree ";
# 302 "kind.ag"
      rsyncCmd += userAtHost + ":" + path + " ";
# 303 "kind.ag"
      rsyncCmd += imageFullName + "/tree";
# 304 "kind.ag"
    } // shell mode
# 305 "kind.ag"
  else
# 306 "kind.ag"
    {
# 307 "kind.ag"
      // cout << "USING SERVERMODE" << endl;
# 308 "kind.ag"

# 309 "kind.ag"
      if (!dryRun)
# 310 "kind.ag"
        strings2File(excluded, imageFullName + "/exclude");
# 311 "kind.ag"

# 312 "kind.ag"
      rsyncCmd += "--exclude-from=" + imageFullName + "/exclude ";
# 313 "kind.ag"
      if (!referenceImage.empty())
# 314 "kind.ag"
        rsyncCmd += "--link-dest=" + referenceImage + "/tree ";
# 315 "kind.ag"
      rsyncCmd += conf.getString("server") + "::" + path + " ";
# 316 "kind.ag"
      rsyncCmd += imageFullName + "/tree";
# 317 "kind.ag"
    }
# 318 "kind.ag"

# 319 "kind.ag"
  debugPrint("Action: " + rsyncCmd);
# 320 "kind.ag"

# 321 "kind.ag"
  vector<string> backupResult;
# 322 "kind.ag"
  if (!dryRun)
# 323 "kind.ag"
    {
# 324 "kind.ag"
      verbosePrint("syncing (" + rsyncCmd + ")");
# 325 "kind.ag"
      int rc;
# 326 "kind.ag"
      backupResult = myPopen(rsyncCmd, rc, debug, imageFullName + "/rsync-log");
# 327 "kind.ag"
      if (rc == 0 ||
# 328 "kind.ag"
          rc == 24 || // "no error" or "vanished source files" (ignored)
# 329 "kind.ag"
          rc == 6144) // workaround for wrong exit code ??!!
# 330 "kind.ag"
        {
# 331 "kind.ag"
          unlink(errorfile.c_str());
# 332 "kind.ag"
          long int st = 0;
# 333 "kind.ag"
          long int sc = 0;
# 334 "kind.ag"
          for (auto bl : backupResult)
# 335 "kind.ag"
            {
# 336 "kind.ag"
              if (startsWith(bl, "Total file size"))
# 337 "kind.ag"
                st = getNumber(bl);
# 338 "kind.ag"
              else if (startsWith(bl, "Total transferred file size"))
# 339 "kind.ag"
                sc = getNumber(bl);
# 340 "kind.ag"
            }
# 341 "kind.ag"
          // sizes[vault] = pair<long int, long int>(st, sc);
# 342 "kind.ag"
          sizes[vault] = Sizes(st, sc);
# 343 "kind.ag"
          //  cout << vault << " " << st << " || " << sc << endl;
# 344 "kind.ag"
        }
# 345 "kind.ag"
      else
# 346 "kind.ag"
        throw Exception("Backup", "Failed to execute rsync (result: " + to_string(rc) + ")");
# 347 "kind.ag"
    }
# 348 "kind.ag"
  else
# 349 "kind.ag"
    cout << "Not executing " << rsyncCmd << endl;
# 350 "kind.ag"
}
# 351 "kind.ag"

# 352 "kind.ag"
void backupVault(const string& vault,
# 353 "kind.ag"
                 KindConfig conf /*Copy!*/ ,
# 354 "kind.ag"
                 const DateTime& imageTime,
# 355 "kind.ag"
                 bool fullImage)
# 356 "kind.ag"
{
# 357 "kind.ag"
  if (!quiet)
# 358 "kind.ag"
    cout << DateTime::now().getString('h') << ": Backup of vault " << vault << endl;
# 359 "kind.ag"
  try
# 360 "kind.ag"
    {
# 361 "kind.ag"
      sizes[vault].second = 0; // nothing backed up yet
# 362 "kind.ag"

# 363 "kind.ag"
      readVaultConfig(vault, conf);
# 364 "kind.ag"

# 365 "kind.ag"
      // where to store
# 366 "kind.ag"
      string vaultpath = findVault(vault);
# 367 "kind.ag"

# 368 "kind.ag"
      // image path
# 369 "kind.ag"
      string imageName = getImageName(conf);
# 370 "kind.ag"
      if (!imageName.empty())
# 371 "kind.ag"
        imageName += '-';
# 372 "kind.ag"

# 373 "kind.ag"
      string imageFullName =  vaultpath + "/" + imageName ;
# 374 "kind.ag"

# 375 "kind.ag"
      if (conf.getBool("longImageName"))
# 376 "kind.ag"
        imageFullName += imageTime.getString('m');
# 377 "kind.ag"
      else
# 378 "kind.ag"
        imageFullName += imageTime.getString('s');
# 379 "kind.ag"

# 380 "kind.ag"
      bool backupNow = true;
# 381 "kind.ag"

# 382 "kind.ag"
      // existing images
# 383 "kind.ag"
      Images validImageList = findImages(vaultpath, conf, false);
# 384 "kind.ag"
      string currentSet = "expire"; // we are not using backupSets
# 385 "kind.ag"

# 386 "kind.ag"
      // check if we are using backup sets
# 387 "kind.ag"

# 388 "kind.ag"
      map<string, int> setIdx;
# 389 "kind.ag"
      vector<SetRule> backupSetRule;
# 390 "kind.ag"
      int setRuleIdx = -1;
# 391 "kind.ag"

# 392 "kind.ag"
      if (conf.hasKey("setRule"))
# 393 "kind.ag"
        {
# 394 "kind.ag"
          readSetRules(conf, setIdx, backupSetRule);
# 395 "kind.ag"
          if (!setIdx.empty())
# 396 "kind.ag"
            {
# 397 "kind.ag"
              backupNow = false;
# 398 "kind.ag"

# 399 "kind.ag"
              // find time for nextBackup for every backupSet
# 400 "kind.ag"
              // defaults to now == imageTime;
# 401 "kind.ag"
              vector<DateTime> nextBackup(backupSetRule.size(), imageTime);
# 402 "kind.ag"

# 403 "kind.ag"
              // find time for next backup
# 404 "kind.ag"

# 405 "kind.ag"
              for (const Image& image : validImageList)
# 406 "kind.ag"
                {
# 407 "kind.ag"
                  if (image.series != "expire")
# 408 "kind.ag"
                    {
# 409 "kind.ag"
                      string s = image.series;
# 410 "kind.ag"
                      if (setIdx.count(s) > 0) // rule for set exists?
# 411 "kind.ag"
                        {
# 412 "kind.ag"
                          int rIdx = setIdx[s];
# 413 "kind.ag"
                          // image is valid for this and "lower" backupSets
# 414 "kind.ag"
                          for (unsigned int i = rIdx; i < backupSetRule.size(); ++i)
# 415 "kind.ag"
                            if (nextBackup[i] < image.time + backupSetRule[i].distance)
# 416 "kind.ag"
                              nextBackup[i] =  image.time + backupSetRule[i].distance;
# 417 "kind.ag"
                        }
# 418 "kind.ag"
                    }
# 419 "kind.ag"
                }
# 420 "kind.ag"
              if (debug)
# 421 "kind.ag"
                for (unsigned int i = 0; i < backupSetRule.size(); ++i)
# 422 "kind.ag"
                  cout << "       Next backup for " << backupSetRule[i].name << " at " <<  nextBackup[i].getString('h') << endl;
# 423 "kind.ag"

# 424 "kind.ag"
              // find backupSet that
# 425 "kind.ag"
              //    - needs backup
# 426 "kind.ag"
              //    - has longest time to keep
# 427 "kind.ag"
              // because of ordered list backupSetRule this is the first set, that need
# 428 "kind.ag"

# 429 "kind.ag"
              currentSet = "";
# 430 "kind.ag"
              for (unsigned int i = 0; i < backupSetRule.size() && currentSet.empty(); ++i)
# 431 "kind.ag"
                {
# 432 "kind.ag"
                  string name = backupSetRule[i].name;
# 433 "kind.ag"
                  if (nextBackup[i] <= imageTime + 5) // small offset of 5s for "jitter"
# 434 "kind.ag"
                    {
# 435 "kind.ag"
                      backupNow = true;
# 436 "kind.ag"
                      currentSet = name;
# 437 "kind.ag"
                      setRuleIdx = i;
# 438 "kind.ag"
                    }
# 439 "kind.ag"
                }
# 440 "kind.ag"
            }
# 441 "kind.ag"
        }
# 442 "kind.ag"

# 443 "kind.ag"
      if (backupNow)
# 444 "kind.ag"
        {
# 445 "kind.ag"
          verbosePrint("backup to \"" + imageFullName + "\"");
# 446 "kind.ag"
          if (setRuleIdx >= 0 && !quiet)
# 447 "kind.ag"
            cout << "  backup set is \"" << currentSet << "\"" << endl;
# 448 "kind.ag"
        }
# 449 "kind.ag"
      else if (!quiet)
# 450 "kind.ag"
        cout << "  no backup set needs update" << endl;
# 451 "kind.ag"

# 452 "kind.ag"
      if (backupNow)
# 453 "kind.ag"
        {
# 454 "kind.ag"
          // find reference image
# 455 "kind.ag"
          string referenceImage;
# 456 "kind.ag"
          if (!fullImage)
# 457 "kind.ag"
            {
# 458 "kind.ag"
              if (validImageList.empty())
# 459 "kind.ag"
                throw Exception("backupVault", "no reference image found");
# 460 "kind.ag"
              // last image is newest image
# 461 "kind.ag"
              referenceImage = validImageList.back().name;
# 462 "kind.ag"
            }
# 463 "kind.ag"

# 464 "kind.ag"
          doBackup(vault, imageFullName, referenceImage, conf);
# 465 "kind.ag"

# 466 "kind.ag"
          if (!dryRun)
# 467 "kind.ag"
            {
# 468 "kind.ag"
              // set symlink to last image
# 469 "kind.ag"
              string lastLink = vaultpath + "/last";
# 470 "kind.ag"
              unlink(lastLink.c_str());
# 471 "kind.ag"
              symlink(imageFullName.c_str(), lastLink.c_str());
# 472 "kind.ag"

# 473 "kind.ag"
              // write expire date to file
# 474 "kind.ag"
              DateTime expireTime;
# 475 "kind.ag"
              string rule;
# 476 "kind.ag"
              if (setRuleIdx < 0) // not backup set based
# 477 "kind.ag"
                expireTime = getExpireDate(imageTime, conf, rule);
# 478 "kind.ag"
              else
# 479 "kind.ag"
                {
# 480 "kind.ag"
                  expireTime = imageTime + backupSetRule[setRuleIdx].keep;
# 481 "kind.ag"
                  rule =  backupSetRule[setRuleIdx].rule;
# 482 "kind.ag"
                }
# 483 "kind.ag"

# 484 "kind.ag"
              ofstream expireFile(imageFullName + "/expires");
# 485 "kind.ag"
              expireFile << currentSet << "-" << expireTime.getString('m') << endl;
# 486 "kind.ag"
              expireFile << rule << endl;
# 487 "kind.ag"
            }
# 488 "kind.ag"
        }
# 489 "kind.ag"
    }
# 490 "kind.ag"
  catch (Exception ex)
# 491 "kind.ag"
    {
# 492 "kind.ag"
      cerr << "Exception in vault " << vault << ": " << ex.what() << endl;
# 493 "kind.ag"
    }
# 494 "kind.ag"
}
# 495 "kind.ag"

# 496 "kind.ag"
void expireVault(const string& vault, KindConfig conf, DateTime now)
# 497 "kind.ag"
{
# 498 "kind.ag"
  if (!quiet)
# 499 "kind.ag"
    cout << DateTime::now().getString('h') << ": Expiring images in vault " << vault << endl;
# 500 "kind.ag"

# 501 "kind.ag"
  readVaultConfig(vault, conf);
# 502 "kind.ag"

# 503 "kind.ag"
  string vaultpath = findVault(vault);
# 504 "kind.ag"

# 505 "kind.ag"
  Images imagelist = findImages(vaultpath, conf, true);
# 506 "kind.ag"

# 507 "kind.ag"
  string lastValidImage;
# 508 "kind.ag"
  for (Image image : imagelist)
# 509 "kind.ag"
    {
# 510 "kind.ag"
      if (image.valid)
# 511 "kind.ag"
        lastValidImage = image.name;
# 512 "kind.ag"
    }
# 513 "kind.ag"

# 514 "kind.ag"
  for (Image image : imagelist)
# 515 "kind.ag"
    {
# 516 "kind.ag"
      debugPrint(image.name);
# 517 "kind.ag"

# 518 "kind.ag"
      DateTime imageTime = image.time;
# 519 "kind.ag"

# 520 "kind.ag"
      if (imageTime != now &&          // ignore just created image
# 521 "kind.ag"
          image.name != lastValidImage // ignore last valid image
# 522 "kind.ag"
         )
# 523 "kind.ag"
        {
# 524 "kind.ag"
          DateTime expireTime;
# 525 "kind.ag"
          string expireRule;
# 526 "kind.ag"
          if (!image.valid) // invalid image?
# 527 "kind.ag"
            {
# 528 "kind.ag"
              time_t expPeriod = stot(conf.getString("expireFailedImage"));
# 529 "kind.ag"
              if (expPeriod < 0)
# 530 "kind.ag"
                throw Exception("expireFailedImage", "Time period must be positive");
# 531 "kind.ag"
              expireTime = imageTime + stot(conf.getString("expireFailedImage"));
# 532 "kind.ag"
              expireRule = "invalid image: " + conf.getString("expireFailedImage");
# 533 "kind.ag"
              debugPrint("- invalid image");
# 534 "kind.ag"
            }
# 535 "kind.ag"
          else
# 536 "kind.ag"
            {
# 537 "kind.ag"
              debugPrint("- valid image");
# 538 "kind.ag"
              expireTime = image.expire;
# 539 "kind.ag"
              expireRule = image.expireRule;
# 540 "kind.ag"
            }
# 541 "kind.ag"

# 542 "kind.ag"
          if (debug)
# 543 "kind.ag"
            {
# 544 "kind.ag"
              cout << "    image: " << imageTime.getString('h') << endl;
# 545 "kind.ag"
              cout << "      expire: " << expireTime.getString('h') << " " << expireRule << endl;
# 546 "kind.ag"
              cout << "      now: " << now.getString('h') << endl;
# 547 "kind.ag"
            }
# 548 "kind.ag"

# 549 "kind.ag"
          if (expireTime < now)
# 550 "kind.ag"
            {
# 551 "kind.ag"
              if (!quiet)
# 552 "kind.ag"
                cout << "  removing image " << image.name << endl;
# 553 "kind.ag"
              try
# 554 "kind.ag"
                {
# 555 "kind.ag"
                  if (removeDir(image.name) != 0)
# 556 "kind.ag"
                    cout << "Error removing " <<  image.name << endl;
# 557 "kind.ag"
                }
# 558 "kind.ag"
              catch (Exception ex)
# 559 "kind.ag"
                {
# 560 "kind.ag"
                  cerr << "Exception: " << ex.what() << endl;
# 561 "kind.ag"
                }
# 562 "kind.ag"
            }
# 563 "kind.ag"
        }
# 564 "kind.ag"
      else
# 565 "kind.ag"
        debugPrint("- current image - ignored");
# 566 "kind.ag"
    }
# 567 "kind.ag"
}
# 568 "kind.ag"

# 569 "kind.ag"
/*AppGen:Main*/
string ag_programName;

void usage()
{
  cout << ag_programName << " - archiving backup" << endl;
  cout << "Usage:" << endl;
  cout << ag_programName << " [<options>] vault_or_group " << endl;
  cout << "  vault_or_group - Vault to backup" << endl;
  cout << "Options:" << endl;
  cout << "  -f --full          Force full image == initial backup (default: false)" << endl;
  cout << "  -c --masterconfig  Master config file (default: \"\")" << endl;
  cout << "                     if not given or empty kind looks for" << endl;
  cout << "                     /etc/kind/master.conf" << endl;
  cout << "                     /ffp/etc/kind/master.conf" << endl;
  cout << "  -B --backup        Backup (default: false)" << endl;
  cout << "  -E --expire        Expire (default: false)" << endl;
  cout << "  -C --listconfig    Show configuration (default: false)" << endl;
  cout << "                     if none of backup, expire or listconfig is specified," << endl;
  cout << "                     backup and expire is assumed." << endl;
  cout << "  -D --dryrun        Dry run (no real backup) (default: false)" << endl;
  cout << "  -v --verbose       Verbose (default: false)" << endl;
  cout << "  -d --debug         Debug output of many data (default: false)" << endl;
  cout << "  -q --quiet         Be quiet - no messages (default: false)" << endl;
  cout << "  -h --help          This help" << endl;
  exit(1);
}

void error(const string &msg)
{
  cout << endl << ag_programName << " - error: " << msg << endl << endl;
  usage();
}

int ptoi(const char *para)
{
  char *end;
  int res = strtol(para, &end, 10);
  if (end == para)
    error(string("no int: ") + para);
  if (*end != 0)
    error(string("garbage in int: ") + para);
  return res;
}

double ptod(const char *para)
{
  char *end;
  double res = strtod(para, &end);
  if (end == para)
    error(string("no double: ") + para);
  if (*end != 0)
    error(string("garbage in double: ") + para);
  return res;
}

int main(int argc, char **argv)
{
bool fullImage = false;
string masterConfig = "";
bool doBackup = false;
bool doExpire = false;
bool listConfig = false;

string vault = "";
  static struct option ag_long_options[] =
  {
    {"full", no_argument, 0, 'f' },
    {"masterconfig", required_argument, 0, 'c' },
    {"backup", no_argument, 0, 'B' },
    {"expire", no_argument, 0, 'E' },
    {"listconfig", no_argument, 0, 'C' },
    {"dryrun", no_argument, 0, 'D' },
    {"verbose", no_argument, 0, 'v' },
    {"debug", no_argument, 0, 'd' },
    {"quiet", no_argument, 0, 'q' },
    {"help", no_argument, 0, 'h' },
    {0,         0,                 0,  0 }
  };
  ag_programName = argv[0];
  int rc;
  opterr = 0;
  while ((rc = getopt_long(argc, argv, ":fc:BECDvdqh", ag_long_options, NULL)) >= 0)
    {
      switch (rc)
        {
        case '?':
          error("Unknown option");
          break;
        case ':':
          error("Expecting option parameter");
          break;
        case 'f':
              fullImage = true;
              break;

        case 'c':
              masterConfig = optarg;
              break;

        case 'B':
              doBackup = true;
              break;

        case 'E':
              doExpire = true;
              break;

        case 'C':
              listConfig = true;
              break;

        case 'D':
              dryRun = true;
              break;

        case 'v':
              verbose = true;
              break;

        case 'd':
              debug = true;
              break;

        case 'q':
              quiet = true;
              break;

        case 'h':
              usage();
              break;

        default:
          error("error in options");
        }
    }
  if (optind < argc)
    vault = argv[optind++];
  else error("Parameter vault_or_group needed");

/*AppGen:MainEnd*/
# 573 "kind.ag"

# 574 "kind.ag"
  int exitCode = 0;
# 575 "kind.ag"
  string lockFile;
# 576 "kind.ag"
  try
# 577 "kind.ag"
    {
# 578 "kind.ag"
      // handling of parameters and switches
# 579 "kind.ag"
      if (debug)        // debug implies verbose
# 580 "kind.ag"
        verbose = true;
# 581 "kind.ag"

# 582 "kind.ag"
      if (!doBackup && !doExpire && !listConfig)
# 583 "kind.ag"
        {
# 584 "kind.ag"
          doBackup = true;
# 585 "kind.ag"
          doExpire = true;
# 586 "kind.ag"
        }
# 587 "kind.ag"

# 588 "kind.ag"
      KindConfig conf;
# 589 "kind.ag"

# 590 "kind.ag"
      // default-values
# 591 "kind.ag"
      conf.add("imageName", "image");
# 592 "kind.ag"
      conf.add("vaultConfigName", "kind/vault.conf");
# 593 "kind.ag"
      conf.add("expireFailedImage", "3 days");
# 594 "kind.ag"
      conf.add("expireRule", "* * * * 1 month");
# 595 "kind.ag"
      conf.add("rsyncOption", ""); // no additional rsync option
# 596 "kind.ag"
      conf.add("remoteShell", "");
# 597 "kind.ag"
      conf.add("lockfile", "/var/lock/kind");
# 598 "kind.ag"
      conf.add("userExcludeFile", "nobackup.list");
# 599 "kind.ag"
      conf.add("userExcludeCommand",
# 600 "kind.ag"
               "find %path -type f -iname '*nobackup' -printf '%P\\\\n'");
# 601 "kind.ag"
      conf.add("logSize", "");
# 602 "kind.ag"

# 603 "kind.ag"
      if (listConfig)
# 604 "kind.ag"
        {
# 605 "kind.ag"
          cout << "builtin config" << endl;
# 606 "kind.ag"
          conf.print(".   ");
# 607 "kind.ag"
        }
# 608 "kind.ag"

# 609 "kind.ag"
      readMasterConfig(masterConfig, conf);
# 610 "kind.ag"

# 611 "kind.ag"
      banks = conf.getStrings("bank");
# 612 "kind.ag"
      if (banks.empty())
# 613 "kind.ag"
        throw Exception("read master configuration", "no banks defined");
# 614 "kind.ag"

# 615 "kind.ag"
      if (listConfig)
# 616 "kind.ag"
        {
# 617 "kind.ag"
          cout << "global config:" << endl;
# 618 "kind.ag"
          conf.print(".   ");
# 619 "kind.ag"
          readVaultConfig(vault, conf);
# 620 "kind.ag"
          cout << "vault config:" << endl;
# 621 "kind.ag"
          conf.print(".   ");
# 622 "kind.ag"
          exit(0);
# 623 "kind.ag"
        }
# 624 "kind.ag"

# 625 "kind.ag"
      lockFile = conf.getString("lockfile");
# 626 "kind.ag"
      createLock(lockFile);
# 627 "kind.ag"

# 628 "kind.ag"
      DateTime imageTime = DateTime::now();
# 629 "kind.ag"

# 630 "kind.ag"
      string logSizeFile = conf.getString("logSize");
# 631 "kind.ag"
      if (!logSizeFile.empty())
# 632 "kind.ag"
        readSizes(logSizeFile);
# 633 "kind.ag"

# 634 "kind.ag"
      vector<string> vaults;
# 635 "kind.ag"
      string groupname = "group_" + vault;
# 636 "kind.ag"
      if (conf.hasKey(groupname))
# 637 "kind.ag"
        vaults = conf.getStrings(groupname);
# 638 "kind.ag"
      else
# 639 "kind.ag"
        vaults.push_back(vault);
# 640 "kind.ag"

# 641 "kind.ag"
      if (doBackup)
# 642 "kind.ag"
        for (string vault : vaults)
# 643 "kind.ag"
          {
# 644 "kind.ag"
            backupVault(vault, conf, imageTime, fullImage);
# 645 "kind.ag"
            writeSizes(logSizeFile);
# 646 "kind.ag"
          }
# 647 "kind.ag"

# 648 "kind.ag"
      if (doExpire)
# 649 "kind.ag"
        for (unsigned int i = 0; i < vaults.size(); ++i)
# 650 "kind.ag"
          expireVault(vaults[i], conf, imageTime);
# 651 "kind.ag"

# 652 "kind.ag"
      if (!quiet)
# 653 "kind.ag"
        cout << DateTime::now().getString('h') << ": finished" << endl;
# 654 "kind.ag"

# 655 "kind.ag"
    }
# 656 "kind.ag"
  catch (const Exception& ex)
# 657 "kind.ag"
    {
# 658 "kind.ag"
      cerr << "Exception: " << ex.what() << endl;
# 659 "kind.ag"
      exitCode = 1;
# 660 "kind.ag"
    }
# 661 "kind.ag"
  catch (const char* msg)
# 662 "kind.ag"
    {
# 663 "kind.ag"
      cerr << "Exception(char*): " << msg << endl;
# 664 "kind.ag"
      exitCode = 1;
# 665 "kind.ag"
    }
# 666 "kind.ag"
  catch (const string& msg)
# 667 "kind.ag"
    {
# 668 "kind.ag"
      cerr << "Exception(string): " << msg << endl;
# 669 "kind.ag"
      exitCode = 1;
# 670 "kind.ag"
    }
# 671 "kind.ag"
  removeLock(lockFile);
# 672 "kind.ag"
  return exitCode;
# 673 "kind.ag"
}
