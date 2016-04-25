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
#include "Image.h"
# 22 "kind.ag"

# 23 "kind.ag"
#include "KindConfig.h"
# 24 "kind.ag"
#include "filetools.h"
# 25 "kind.ag"
#include "Lexer.h"
# 26 "kind.ag"
#include "rulecomp.h"
# 27 "kind.ag"

# 28 "kind.ag"
#include "expiretools.h"
# 29 "kind.ag"

# 30 "kind.ag"
/*AppGen
# 31 "kind.ag"
  %%  Beschreibung des Programmes:
# 32 "kind.ag"
  prog: archiving backup
# 33 "kind.ag"
  %% Beschreibung Parameter
# 34 "kind.ag"
  % symbolischerName, Art, Typ,   Variablenname, Erklärung, Default-Wert
# 35 "kind.ag"
  para: vault_or_group, required, string, vault, Vault to backup
# 36 "kind.ag"
  %% Beschreibung der Optionen
# 37 "kind.ag"
  % kurz-Option, lang-Option, Typ, Variablenname, Erklärung, Default-Wert
# 38 "kind.ag"
  opt: c, masterconfig, string, masterConfig, Master config file, ""
# 39 "kind.ag"
  opt2: if not given or empty kind looks for
# 40 "kind.ag"
  opt2:   /etc/kind/master.conf
# 41 "kind.ag"
  opt2:   /ffp/etc/kind/master.conf
# 42 "kind.ag"
  opt: f, full, void, fullImage, Force full image == initial backup, false
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
  opt: F, forcebackup, string, forcedBackupSet, Create image for specified backup set, ""
# 50 "kind.ag"
  opt: v, verbose, Void, verbose,  Verbose,  false
# 51 "kind.ag"
  opt: d, debug, Void, debug, Debug output of many data, false
# 52 "kind.ag"
  opt: q, quiet, Void, quiet, Be quiet - no messages, false
# 53 "kind.ag"
  opt: h, help, usage, ignored , This help
# 54 "kind.ag"
AppGen*/
# 55 "kind.ag"

# 56 "kind.ag"
using namespace std;
# 57 "kind.ag"

# 58 "kind.ag"
/*AppGen:Global*/
#include <getopt.h>
#include <string>
#include <string>
#include <string>
bool dryRun = false;
bool verbose = false;
bool debug = false;
bool quiet = false;
/*AppGen:GlobalEnd*/
# 59 "kind.ag"

# 60 "kind.ag"
Strings banks;
# 61 "kind.ag"

# 62 "kind.ag"
typedef pair<long int, long int> Sizes;
# 63 "kind.ag"
map<string, Sizes> sizes;
# 64 "kind.ag"

# 65 "kind.ag"
void readSizes(const string& logSizeFile)
# 66 "kind.ag"
{
# 67 "kind.ag"
  if (fileExists(logSizeFile))
# 68 "kind.ag"
    {
# 69 "kind.ag"
      vector<string> ss;
# 70 "kind.ag"
      file2Strings(logSizeFile, ss);
# 71 "kind.ag"
      for (const string& s : ss)
# 72 "kind.ag"
        {
# 73 "kind.ag"
          unsigned int i = 0;
# 74 "kind.ag"
          string v = getWord(s, i);
# 75 "kind.ag"
          long int s1 = getLongInt(s, i);
# 76 "kind.ag"
          long int s2 = getLongInt(s, i);
# 77 "kind.ag"
          sizes[v] = Sizes(s1, s2);
# 78 "kind.ag"
        }
# 79 "kind.ag"
    }
# 80 "kind.ag"
}
# 81 "kind.ag"

# 82 "kind.ag"
void writeSizes(const string logSizeFile)
# 83 "kind.ag"
{
# 84 "kind.ag"
  if (!logSizeFile.empty())
# 85 "kind.ag"
    {
# 86 "kind.ag"
      Strings st;
# 87 "kind.ag"
      for (auto s : sizes)
# 88 "kind.ag"
        {
# 89 "kind.ag"
          string h = s.first + " " + to_string(s.second.first) + " " + to_string(s.second.second);
# 90 "kind.ag"
          st.push_back(h);
# 91 "kind.ag"
        }
# 92 "kind.ag"
      strings2File(st, logSizeFile);
# 93 "kind.ag"
    }
# 94 "kind.ag"
}
# 95 "kind.ag"

# 96 "kind.ag"
void verbosePrint(const string& text)
# 97 "kind.ag"
{
# 98 "kind.ag"
  if (verbose)
# 99 "kind.ag"
    cout << "  " << text << endl;
# 100 "kind.ag"
}
# 101 "kind.ag"

# 102 "kind.ag"
void debugPrint(const string& text)
# 103 "kind.ag"
{
# 104 "kind.ag"
  if (debug)
# 105 "kind.ag"
    cout << "    " << text << endl;
# 106 "kind.ag"
}
# 107 "kind.ag"

# 108 "kind.ag"
void readMasterConfig1(const string& fn, KindConfig& conf)
# 109 "kind.ag"
{
# 110 "kind.ag"
  verbosePrint("reading master config " + fn);
# 111 "kind.ag"
  conf.addFile(fn);
# 112 "kind.ag"
}
# 113 "kind.ag"

# 114 "kind.ag"
void readMasterConfig(const string& fn, KindConfig& conf)
# 115 "kind.ag"
{
# 116 "kind.ag"
  if (!fn.empty())  // master config given by user on commandline
# 117 "kind.ag"
    readMasterConfig1(fn, conf);
# 118 "kind.ag"
  else if (fileExists("/etc/kind/master.conf"))
# 119 "kind.ag"
    readMasterConfig1("/etc/kind/master.conf", conf);
# 120 "kind.ag"
  else if (fileExists("/ffp/etc/kind/master.conf"))
# 121 "kind.ag"
    readMasterConfig1("/ffp/etc/kind/master.conf", conf);
# 122 "kind.ag"
  else
# 123 "kind.ag"
    throw Exception("MasterConfig", "no file");
# 124 "kind.ag"
}
# 125 "kind.ag"

# 126 "kind.ag"
string findVault(const string& v)
# 127 "kind.ag"
{
# 128 "kind.ag"
  bool found = false;
# 129 "kind.ag"
  FileName fn;
# 130 "kind.ag"
  fn.setName(v);
# 131 "kind.ag"
  for (unsigned int i = 0; !found && i < banks.size(); ++i)
# 132 "kind.ag"
    {
# 133 "kind.ag"
      fn.setPath(banks[i]);
# 134 "kind.ag"
      if (dirExists(fn.getFileName()))
# 135 "kind.ag"
        found = true;
# 136 "kind.ag"
    }
# 137 "kind.ag"
  if (!found)
# 138 "kind.ag"
    throw Exception("find vault", v + " not found");
# 139 "kind.ag"
  verbosePrint("using vault " + fn.getFileName());
# 140 "kind.ag"
  return fn.getFileName();
# 141 "kind.ag"
}
# 142 "kind.ag"

# 143 "kind.ag"
void readVaultConfig(const string& vault, KindConfig& conf)
# 144 "kind.ag"
{
# 145 "kind.ag"
  string vaultpath = findVault(vault);
# 146 "kind.ag"
  const string& vaultConfigName = vaultpath + '/' + conf.getString("vaultConfigName");
# 147 "kind.ag"
  verbosePrint("reading vault config:");
# 148 "kind.ag"
  verbosePrint("  " + vaultConfigName);
# 149 "kind.ag"
  conf.addFile(vaultConfigName);
# 150 "kind.ag"
}
# 151 "kind.ag"

# 152 "kind.ag"
string getImageName(const KindConfig& conf,
# 153 "kind.ag"
                    const string& vaultPath,
# 154 "kind.ag"
                    const DateTime& imageTime)
# 155 "kind.ag"
{
# 156 "kind.ag"
  bool nonPortable = false;
# 157 "kind.ag"
  string imageName = conf.getString("imageName");
# 158 "kind.ag"
  for (unsigned int i = 0; !nonPortable && i < imageName.size(); ++i)
# 159 "kind.ag"
    {
# 160 "kind.ag"
      char c = imageName[i];
# 161 "kind.ag"
      if (!isalnum(c) && c != '.' && c != '_')
# 162 "kind.ag"
        nonPortable = true;
# 163 "kind.ag"
    }
# 164 "kind.ag"
  if (nonPortable)
# 165 "kind.ag"
    throw Exception("getImageName", "Invalid character in image name " + imageName);
# 166 "kind.ag"

# 167 "kind.ag"
  if (!imageName.empty())
# 168 "kind.ag"
    imageName += '-';
# 169 "kind.ag"

# 170 "kind.ag"
  string imageFullName =  vaultPath + "/" + imageName ;
# 171 "kind.ag"

# 172 "kind.ag"
  if (conf.getBool("longImageName"))
# 173 "kind.ag"
    imageFullName += imageTime.getString('m');
# 174 "kind.ag"
  else
# 175 "kind.ag"
    imageFullName += imageTime.getString('s');
# 176 "kind.ag"

# 177 "kind.ag"
  return imageFullName;
# 178 "kind.ag"
}
# 179 "kind.ag"

# 180 "kind.ag"
Images findImages(const string& vaultpath, const KindConfig& conf, bool all)
# 181 "kind.ag"
{
# 182 "kind.ag"
  Strings dirs;
# 183 "kind.ag"
  debugPrint("searching images in " + vaultpath);
# 184 "kind.ag"
  dirList(vaultpath, dirs);
# 185 "kind.ag"

# 186 "kind.ag"
  Images imageList;
# 187 "kind.ag"
  for (string dir : dirs)
# 188 "kind.ag"
    {
# 189 "kind.ag"
      FileName fn(dir);
# 190 "kind.ag"
      string imgname = conf.getString("imageName");
# 191 "kind.ag"
      if (startsWith(fn.getName(), imgname))
# 192 "kind.ag"
        {
# 193 "kind.ag"
          debugPrint("Checking " + dir);
# 194 "kind.ag"
          Image image(dir);
# 195 "kind.ag"

# 196 "kind.ag"
          if (all || image.valid)
# 197 "kind.ag"
            imageList.push_back(image);
# 198 "kind.ag"
        }
# 199 "kind.ag"
    }
# 200 "kind.ag"
  if (imageList.size() > 1)
# 201 "kind.ag"
    sort(imageList.begin(), imageList.end());
# 202 "kind.ag"
  return imageList;
# 203 "kind.ag"
}
# 204 "kind.ag"

# 205 "kind.ag"
void doBackup(const string& vault,
# 206 "kind.ag"
              const string& imageFullName,
# 207 "kind.ag"
              const string& referenceImage,
# 208 "kind.ag"
              const KindConfig& conf)
# 209 "kind.ag"
{
# 210 "kind.ag"
  // create image path
# 211 "kind.ag"
  if (!dryRun)
# 212 "kind.ag"
    if (mkdir(imageFullName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
# 213 "kind.ag"
      throw Exception("Create image", "failed to create " + imageFullName);
# 214 "kind.ag"

# 215 "kind.ag"
  // error message
# 216 "kind.ag"
  // we write an generic error message to mark backup as unsuccessful
# 217 "kind.ag"
  // will be deleted at successful end of rsync
# 218 "kind.ag"
  string errorfile = imageFullName + "/error";
# 219 "kind.ag"
  if (!dryRun)
# 220 "kind.ag"
    {
# 221 "kind.ag"
      ofstream error(errorfile);
# 222 "kind.ag"
      error << "failed" << endl;
# 223 "kind.ag"
      error.close();
# 224 "kind.ag"
    }
# 225 "kind.ag"

# 226 "kind.ag"
  // create source descriptor
# 227 "kind.ag"
  string host;
# 228 "kind.ag"
  if (conf.hasKey("host"))
# 229 "kind.ag"
    host = conf.getString("host");
# 230 "kind.ag"

# 231 "kind.ag"
  string server;
# 232 "kind.ag"
  if (conf.hasKey("server"))
# 233 "kind.ag"
    server = conf.getString("server");
# 234 "kind.ag"

# 235 "kind.ag"
  if (!host.empty() && !server.empty())
# 236 "kind.ag"
    throw Exception("backupVault", "Cannot have host and server");
# 237 "kind.ag"

# 238 "kind.ag"
  string path = conf.getString("path");
# 239 "kind.ag"
  if (path.empty())
# 240 "kind.ag"
    throw Exception("rsync", "empty source path");
# 241 "kind.ag"
  if (path.back() != '/')
# 242 "kind.ag"
    path += '/';
# 243 "kind.ag"

# 244 "kind.ag"
  string rsyncCmd = "rsync -vrltH --delete --stats -D --numeric-ids ";
# 245 "kind.ag"
  if (!conf.getBool("ignorePermission"))
# 246 "kind.ag"
    rsyncCmd += "-pgo";
# 247 "kind.ag"
  vector<string> rso = conf.getStrings("rsyncOption");
# 248 "kind.ag"
  for (const string& opt : rso)
# 249 "kind.ag"
    rsyncCmd += opt + " ";
# 250 "kind.ag"

# 251 "kind.ag"
  // excludes
# 252 "kind.ag"
  Strings excluded;
# 253 "kind.ag"

# 254 "kind.ag"
  if (conf.hasKey("exclude"))
# 255 "kind.ag"
    excluded += conf.getStrings("exclude");
# 256 "kind.ag"

# 257 "kind.ag"
  if (!host.empty())  // shell mode
# 258 "kind.ag"
    {
# 259 "kind.ag"
      // cout << "USING SHELLMODE '" << host << "'" << endl;
# 260 "kind.ag"
      string remoteShell = conf.getString("remoteShell");
# 261 "kind.ag"
      string userAtHost = conf.getString("user") + "@" +
# 262 "kind.ag"
      conf.getString("host");
# 263 "kind.ag"
      string rshCommand = remoteShell;
# 264 "kind.ag"
      if (remoteShell.empty())
# 265 "kind.ag"
        rshCommand = "ssh";
# 266 "kind.ag"

# 267 "kind.ag"
      rshCommand += " " + userAtHost;
# 268 "kind.ag"

# 269 "kind.ag"
      string userExcludeCommand = conf.getString("userExcludeCommand");
# 270 "kind.ag"

# 271 "kind.ag"
      if (!userExcludeCommand.empty())
# 272 "kind.ag"
        {
# 273 "kind.ag"
          replacePlaceHolder(userExcludeCommand, "%path", path);
# 274 "kind.ag"
          string excludeCommand = rshCommand + " " + userExcludeCommand;
# 275 "kind.ag"

# 276 "kind.ag"
          verbosePrint("searching for exclusions (" + excludeCommand + ")");
# 277 "kind.ag"

# 278 "kind.ag"
          int rc;
# 279 "kind.ag"
          Strings excludedFiles = myPopen(excludeCommand, rc, debug);
# 280 "kind.ag"
          if (rc > 0)
# 281 "kind.ag"
            {
# 282 "kind.ag"
              // return Strings should contain error messages
# 283 "kind.ag"
              strings2File(excludedFiles, errorfile);
# 284 "kind.ag"
              throw Exception("Find exludes", "Search for excludes failed");
# 285 "kind.ag"
            }
# 286 "kind.ag"

# 287 "kind.ag"
          for (unsigned int i = 0; i < excludedFiles.size(); ++i)
# 288 "kind.ag"
            {
# 289 "kind.ag"
              FileName fn(excludedFiles[i]);
# 290 "kind.ag"
              excluded.push_back('/' + fn.getPath());
# 291 "kind.ag"
              debugPrint("Excluding: " + excluded.back());
# 292 "kind.ag"
            }
# 293 "kind.ag"
        }
# 294 "kind.ag"

# 295 "kind.ag"
      string userExcludeFile = conf.getString("userExcludeFile");
# 296 "kind.ag"
      if (!userExcludeFile.empty())
# 297 "kind.ag"
        {
# 298 "kind.ag"
          userExcludeFile = path + userExcludeFile;
# 299 "kind.ag"
          string getExcludeFileCommand = rshCommand;
# 300 "kind.ag"
          getExcludeFileCommand += " \" if [ -f '" + userExcludeFile + "' ]; then ";
# 301 "kind.ag"
          getExcludeFileCommand += " cat '" + userExcludeFile + "' ; fi \"";
# 302 "kind.ag"
          // cout << getExcludeFileCommand << endl;
# 303 "kind.ag"
          int rc;
# 304 "kind.ag"
          Strings excludes2 = myPopen(getExcludeFileCommand, rc, debug);
# 305 "kind.ag"
          if (rc == 0)
# 306 "kind.ag"
            excluded += excludes2;
# 307 "kind.ag"
        }
# 308 "kind.ag"

# 309 "kind.ag"
      if (!dryRun)
# 310 "kind.ag"
        strings2File(excluded, imageFullName + "/exclude");
# 311 "kind.ag"

# 312 "kind.ag"
      // rsync image
# 313 "kind.ag"

# 314 "kind.ag"
      if (!remoteShell.empty())
# 315 "kind.ag"
        rsyncCmd += " -e \'" + remoteShell + "\' ";
# 316 "kind.ag"

# 317 "kind.ag"
      rsyncCmd += "--exclude-from=" + imageFullName + "/exclude ";
# 318 "kind.ag"
      if (!referenceImage.empty())
# 319 "kind.ag"
        rsyncCmd += "--link-dest=" + referenceImage + "/tree ";
# 320 "kind.ag"
      rsyncCmd += userAtHost + ":" + path + " ";
# 321 "kind.ag"
      rsyncCmd += imageFullName + "/tree";
# 322 "kind.ag"
    } // shell mode
# 323 "kind.ag"
  else
# 324 "kind.ag"
    {
# 325 "kind.ag"
      // cout << "USING SERVERMODE" << endl;
# 326 "kind.ag"

# 327 "kind.ag"
      if (!dryRun)
# 328 "kind.ag"
        strings2File(excluded, imageFullName + "/exclude");
# 329 "kind.ag"

# 330 "kind.ag"
      rsyncCmd += "--exclude-from=" + imageFullName + "/exclude ";
# 331 "kind.ag"
      if (!referenceImage.empty())
# 332 "kind.ag"
        rsyncCmd += "--link-dest=" + referenceImage + "/tree ";
# 333 "kind.ag"
      rsyncCmd += conf.getString("server") + "::" + path + " ";
# 334 "kind.ag"
      rsyncCmd += imageFullName + "/tree";
# 335 "kind.ag"
    }
# 336 "kind.ag"

# 337 "kind.ag"
  debugPrint("Action: " + rsyncCmd);
# 338 "kind.ag"

# 339 "kind.ag"
  vector<string> backupResult;
# 340 "kind.ag"
  if (!dryRun)
# 341 "kind.ag"
    {
# 342 "kind.ag"
      verbosePrint("syncing (" + rsyncCmd + ")");
# 343 "kind.ag"
      int rc;
# 344 "kind.ag"
      backupResult = myPopen(rsyncCmd, rc, debug, imageFullName + "/rsync-log");
# 345 "kind.ag"
      if (rc == 0 ||
# 346 "kind.ag"
      rc == 24 || // "no error" or "vanished source files" (ignored)
# 347 "kind.ag"
      rc == 6144) // workaround for wrong exit code ??!!
# 348 "kind.ag"
        {
# 349 "kind.ag"
          unlink(errorfile.c_str());
# 350 "kind.ag"
          long int st = 0;
# 351 "kind.ag"
          long int sc = 0;
# 352 "kind.ag"
          for (auto bl : backupResult)
# 353 "kind.ag"
            {
# 354 "kind.ag"
              if (startsWith(bl, "Total file size"))
# 355 "kind.ag"
                st = getNumber(bl);
# 356 "kind.ag"
              else if (startsWith(bl, "Total transferred file size"))
# 357 "kind.ag"
                sc = getNumber(bl);
# 358 "kind.ag"
            }
# 359 "kind.ag"
          // sizes[vault] = pair<long int, long int>(st, sc);
# 360 "kind.ag"
          sizes[vault] = Sizes(st, sc);
# 361 "kind.ag"
          //  cout << vault << " " << st << " || " << sc << endl;
# 362 "kind.ag"
        }
# 363 "kind.ag"
      else
# 364 "kind.ag"
        throw Exception("Backup", "Failed to execute rsync (result: " + to_string(rc) + ")");
# 365 "kind.ag"
    }
# 366 "kind.ag"
  else
# 367 "kind.ag"
    cout << "Not executing " << rsyncCmd << endl;
# 368 "kind.ag"
}
# 369 "kind.ag"

# 370 "kind.ag"
void backupVault(const string& vault,
# 371 "kind.ag"
                 KindConfig conf /*Copy!*/ ,
# 372 "kind.ag"
                 const DateTime& imageTime,
# 373 "kind.ag"
                 bool fullImage,
# 374 "kind.ag"
                 const string& forcedBackupSet)
# 375 "kind.ag"
{
# 376 "kind.ag"
  if (!quiet)
# 377 "kind.ag"
    cout << DateTime::now().getString('h') << ": Backup of vault " << vault << endl;
# 378 "kind.ag"
  try
# 379 "kind.ag"
    {
# 380 "kind.ag"
      sizes[vault].second = 0; // nothing backed up yet
# 381 "kind.ag"

# 382 "kind.ag"
      readVaultConfig(vault, conf);
# 383 "kind.ag"

# 384 "kind.ag"
      // where to store
# 385 "kind.ag"
      string vaultPath = findVault(vault);
# 386 "kind.ag"

# 387 "kind.ag"
      // image path
# 388 "kind.ag"
      string imageFullName = getImageName(conf, vaultPath, imageTime);
# 389 "kind.ag"

# 390 "kind.ag"
      bool backupNow = true;
# 391 "kind.ag"

# 392 "kind.ag"
      // existing images
# 393 "kind.ag"
      Images validImageList = findImages(vaultPath, conf, false);
# 394 "kind.ag"
      string currentSet = "expire"; // we are not using backupSets
# 395 "kind.ag"

# 396 "kind.ag"
      // check if we are using backup sets
# 397 "kind.ag"

# 398 "kind.ag"
      map<string, int> setIdx;
# 399 "kind.ag"
      vector<SetRule> backupSetRule;
# 400 "kind.ag"
      int setRuleIdx = -1;
# 401 "kind.ag"

# 402 "kind.ag"
      if (conf.hasKey("setRule"))
# 403 "kind.ag"
        {
# 404 "kind.ag"
          readSetRules(conf, setIdx, backupSetRule);
# 405 "kind.ag"
          if (!setIdx.empty())
# 406 "kind.ag"
            {
# 407 "kind.ag"
              if (forcedBackupSet.empty())
# 408 "kind.ag"
                {
# 409 "kind.ag"
                  backupNow = false;
# 410 "kind.ag"

# 411 "kind.ag"
                  // find time for nextBackup for every backupSet
# 412 "kind.ag"
                  // defaults to now == imageTime;
# 413 "kind.ag"
                  vector<DateTime> nextBackup(backupSetRule.size(), imageTime);
# 414 "kind.ag"

# 415 "kind.ag"
                  // find time for next backup
# 416 "kind.ag"

# 417 "kind.ag"
                  for (const Image& image : validImageList)
# 418 "kind.ag"
                    {
# 419 "kind.ag"
                      if (image.series != "expire")
# 420 "kind.ag"
                        {
# 421 "kind.ag"
                          string s = image.series;
# 422 "kind.ag"
                          if (setIdx.count(s) > 0) // rule for set exists?
# 423 "kind.ag"
                            {
# 424 "kind.ag"
                              int rIdx = setIdx[s];
# 425 "kind.ag"
                              // image is valid for this and "lower level" backupSets
# 426 "kind.ag"
                              for (unsigned int i = rIdx; i < backupSetRule.size(); ++i)
# 427 "kind.ag"
                                if (nextBackup[i] < image.time + backupSetRule[i].distance)
# 428 "kind.ag"
                                  nextBackup[i] =  image.time + backupSetRule[i].distance;
# 429 "kind.ag"
                            }
# 430 "kind.ag"
                        }
# 431 "kind.ag"
                    }
# 432 "kind.ag"
                  if (debug)
# 433 "kind.ag"
                    for (unsigned int i = 0; i < backupSetRule.size(); ++i)
# 434 "kind.ag"
                      cout << "       Next backup for " << backupSetRule[i].name << " at " <<  nextBackup[i].getString('h') << endl;
# 435 "kind.ag"

# 436 "kind.ag"
                  // find backupSet that
# 437 "kind.ag"
                  //    - needs backup
# 438 "kind.ag"
                  //    - has longest time to keep
# 439 "kind.ag"
                  // because of ordered list backupSetRule this is the first set, that need
# 440 "kind.ag"

# 441 "kind.ag"
                  currentSet = "";
# 442 "kind.ag"
                  for (unsigned int i = 0; i < backupSetRule.size() && currentSet.empty(); ++i)
# 443 "kind.ag"
                    {
# 444 "kind.ag"
                      string name = backupSetRule[i].name;
# 445 "kind.ag"
                      if (nextBackup[i] <= imageTime + 5) // small offset of 5s for "jitter"
# 446 "kind.ag"
                        {
# 447 "kind.ag"
                          backupNow = true;
# 448 "kind.ag"
                          currentSet = name;
# 449 "kind.ag"
                          setRuleIdx = i;
# 450 "kind.ag"
                        }
# 451 "kind.ag"
                    }
# 452 "kind.ag"
                }
# 453 "kind.ag"
              else
# 454 "kind.ag"
                {
# 455 "kind.ag"
                  if (setIdx.count(forcedBackupSet) > 0)
# 456 "kind.ag"
                    {
# 457 "kind.ag"
                      currentSet = forcedBackupSet;
# 458 "kind.ag"
                      setRuleIdx = setIdx[forcedBackupSet];
# 459 "kind.ag"
                    }
# 460 "kind.ag"
                  else
# 461 "kind.ag"
                    throw Exception("force backup of set " + forcedBackupSet, " set not exists");
# 462 "kind.ag"
                }
# 463 "kind.ag"
            } // if (!setIdx.empty())
# 464 "kind.ag"
        } // (conf.hasKey("setRule"))
# 465 "kind.ag"

# 466 "kind.ag"
      if (backupNow)
# 467 "kind.ag"
        {
# 468 "kind.ag"
          verbosePrint("backup to \"" + imageFullName + "\"");
# 469 "kind.ag"
          if (setRuleIdx >= 0 && !quiet)
# 470 "kind.ag"
            cout << "  backup set is \"" << currentSet << "\"" << endl;
# 471 "kind.ag"
        }
# 472 "kind.ag"
      else if (!quiet)
# 473 "kind.ag"
        cout << "  no backup set needs update" << endl;
# 474 "kind.ag"

# 475 "kind.ag"
      if (backupNow)
# 476 "kind.ag"
        {
# 477 "kind.ag"
          // find reference image
# 478 "kind.ag"
          string referenceImage;
# 479 "kind.ag"
          if (!fullImage)
# 480 "kind.ag"
            {
# 481 "kind.ag"
              if (validImageList.empty())
# 482 "kind.ag"
                throw Exception("backupVault", "no reference image found");
# 483 "kind.ag"
              // last image is newest image
# 484 "kind.ag"
              referenceImage = validImageList.back().name;
# 485 "kind.ag"
            }
# 486 "kind.ag"

# 487 "kind.ag"
          doBackup(vault, imageFullName, referenceImage, conf);
# 488 "kind.ag"

# 489 "kind.ag"
          if (!dryRun)
# 490 "kind.ag"
            {
# 491 "kind.ag"
              // set symlink to last image
# 492 "kind.ag"
              string lastLink = vaultPath + "/last";
# 493 "kind.ag"
              unlink(lastLink.c_str());
# 494 "kind.ag"
              symlink(imageFullName.c_str(), lastLink.c_str());
# 495 "kind.ag"

# 496 "kind.ag"
              // write expire date to file
# 497 "kind.ag"
              DateTime expireTime;
# 498 "kind.ag"
              string rule;
# 499 "kind.ag"
              if (setRuleIdx < 0) // not backup set based
# 500 "kind.ag"
                expireTime = getExpireDate(imageTime, conf, rule);
# 501 "kind.ag"
              else
# 502 "kind.ag"
                {
# 503 "kind.ag"
                  expireTime = imageTime + backupSetRule[setRuleIdx].keep;
# 504 "kind.ag"
                  rule =  backupSetRule[setRuleIdx].rule;
# 505 "kind.ag"
                }
# 506 "kind.ag"

# 507 "kind.ag"
              ofstream expireFile(imageFullName + "/expires");
# 508 "kind.ag"
              expireFile << currentSet << "-" << expireTime.getString('m') << endl;
# 509 "kind.ag"
              expireFile << rule << endl;
# 510 "kind.ag"
            }
# 511 "kind.ag"
        }
# 512 "kind.ag"
    }
# 513 "kind.ag"
  catch (Exception ex)
# 514 "kind.ag"
    {
# 515 "kind.ag"
      cerr << "Exception in vault " << vault << ": " << ex.what() << endl;
# 516 "kind.ag"
    }
# 517 "kind.ag"
}
# 518 "kind.ag"

# 519 "kind.ag"
void expireVault(const string& vault, KindConfig conf, DateTime now)
# 520 "kind.ag"
{
# 521 "kind.ag"
  if (!quiet)
# 522 "kind.ag"
    cout << DateTime::now().getString('h') << ": Expiring images in vault " << vault << endl;
# 523 "kind.ag"

# 524 "kind.ag"
  readVaultConfig(vault, conf);
# 525 "kind.ag"

# 526 "kind.ag"
  string vaultpath = findVault(vault);
# 527 "kind.ag"

# 528 "kind.ag"
  Images imagelist = findImages(vaultpath, conf, true);
# 529 "kind.ag"

# 530 "kind.ag"
  string lastValidImage;
# 531 "kind.ag"
  for (Image image : imagelist)
# 532 "kind.ag"
    {
# 533 "kind.ag"
      if (image.valid)
# 534 "kind.ag"
        lastValidImage = image.name;
# 535 "kind.ag"
    }
# 536 "kind.ag"

# 537 "kind.ag"
  for (Image image : imagelist)
# 538 "kind.ag"
    {
# 539 "kind.ag"
      debugPrint(image.name);
# 540 "kind.ag"

# 541 "kind.ag"
      DateTime imageTime = image.time;
# 542 "kind.ag"

# 543 "kind.ag"
      if (imageTime != now &&          // ignore just created image
# 544 "kind.ag"
      image.name != lastValidImage // ignore last valid image
# 545 "kind.ag"
         )
# 546 "kind.ag"
        {
# 547 "kind.ag"
          DateTime expireTime;
# 548 "kind.ag"
          string expireRule;
# 549 "kind.ag"
          if (!image.valid) // invalid image?
# 550 "kind.ag"
            {
# 551 "kind.ag"
              time_t expPeriod = stot(conf.getString("expireFailedImage"));
# 552 "kind.ag"
              if (expPeriod < 0)
# 553 "kind.ag"
                throw Exception("expireFailedImage", "Time period must be positive");
# 554 "kind.ag"
              expireTime = imageTime + stot(conf.getString("expireFailedImage"));
# 555 "kind.ag"
              expireRule = "invalid image: " + conf.getString("expireFailedImage");
# 556 "kind.ag"
              debugPrint("- invalid image");
# 557 "kind.ag"
            }
# 558 "kind.ag"
          else
# 559 "kind.ag"
            {
# 560 "kind.ag"
              debugPrint("- valid image");
# 561 "kind.ag"
              expireTime = image.expire;
# 562 "kind.ag"
              expireRule = image.expireRule;
# 563 "kind.ag"
            }
# 564 "kind.ag"

# 565 "kind.ag"
          if (debug)
# 566 "kind.ag"
            {
# 567 "kind.ag"
              cout << "    image: " << imageTime.getString('h') << endl;
# 568 "kind.ag"
              cout << "      expire: " << expireTime.getString('h') << " " << expireRule << endl;
# 569 "kind.ag"
              cout << "      now: " << now.getString('h') << endl;
# 570 "kind.ag"
            }
# 571 "kind.ag"

# 572 "kind.ag"
          if (expireTime < now)
# 573 "kind.ag"
            {
# 574 "kind.ag"
              if (!quiet)
# 575 "kind.ag"
                cout << "  removing image " << image.name << endl;
# 576 "kind.ag"
              try
# 577 "kind.ag"
                {
# 578 "kind.ag"
                  if (removeDir(image.name) != 0)
# 579 "kind.ag"
                    cout << "Error removing " <<  image.name << endl;
# 580 "kind.ag"
                }
# 581 "kind.ag"
              catch (Exception ex)
# 582 "kind.ag"
                {
# 583 "kind.ag"
                  cerr << "Exception: " << ex.what() << endl;
# 584 "kind.ag"
                }
# 585 "kind.ag"
            }
# 586 "kind.ag"
        }
# 587 "kind.ag"
      else
# 588 "kind.ag"
        debugPrint("- current image - ignored");
# 589 "kind.ag"
    }
# 590 "kind.ag"
}
# 591 "kind.ag"

# 592 "kind.ag"
/*AppGen:Main*/
string ag_programName;

void usage()
{
  cout << ag_programName << " - archiving backup" << endl;
  cout << "Usage:" << endl;
  cout << ag_programName << " [<options>] vault_or_group " << endl;
  cout << "  vault_or_group - Vault to backup" << endl;
  cout << "Options:" << endl;
  cout << "  -c --masterconfig  Master config file (default: \"\")" << endl;
  cout << "                     if not given or empty kind looks for" << endl;
  cout << "                     /etc/kind/master.conf" << endl;
  cout << "                     /ffp/etc/kind/master.conf" << endl;
  cout << "  -f --full          Force full image == initial backup (default: false)" << endl;
  cout << "  -B --backup        Backup (default: false)" << endl;
  cout << "  -E --expire        Expire (default: false)" << endl;
  cout << "  -C --listconfig    Show configuration (default: false)" << endl;
  cout << "                     if none of backup, expire or listconfig is specified," << endl;
  cout << "                     backup and expire is assumed." << endl;
  cout << "  -D --dryrun        Dry run (no real backup) (default: false)" << endl;
  cout << "  -F --forcebackup   Create image for specified backup set (default: \"\")" << endl;
  cout << "  -v --verbose       Verbose (default: false)" << endl;
  cout << "  -d --debug         Debug output of many data (default: false)" << endl;
  cout << "  -q --quiet         Be quiet - no messages (default: false)" << endl;
  cout << "  -h --help          This help" << endl;
  exit(1);
}

void error(const string& msg)
{
  cout << endl << ag_programName << " - error: " << msg << endl << endl;
  usage();
}

int ptoi(const char* para)
{
  char* end;
  int res = strtol(para, &end, 10);
  if (end == para)
    error(string("no int: ") + para);
  if (*end != 0)
    error(string("garbage in int: ") + para);
  return res;
}

double ptod(const char* para)
{
  char* end;
  double res = strtod(para, &end);
  if (end == para)
    error(string("no double: ") + para);
  if (*end != 0)
    error(string("garbage in double: ") + para);
  return res;
}

int main(int argc, char** argv)
{
  string masterConfig = "";
  bool fullImage = false;
  bool doBackup = false;
  bool doExpire = false;
  bool listConfig = false;
  string forcedBackupSet = "";

  string vault = "";
  static struct option ag_long_options[] =
  {
    {"masterconfig", required_argument, 0, 'c' },
    {"full", no_argument, 0, 'f' },
    {"backup", no_argument, 0, 'B' },
    {"expire", no_argument, 0, 'E' },
    {"listconfig", no_argument, 0, 'C' },
    {"dryrun", no_argument, 0, 'D' },
    {"forcebackup", required_argument, 0, 'F' },
    {"verbose", no_argument, 0, 'v' },
    {"debug", no_argument, 0, 'd' },
    {"quiet", no_argument, 0, 'q' },
    {"help", no_argument, 0, 'h' },
    {0,         0,                 0,  0 }
  };
  ag_programName = argv[0];
  int rc;
  opterr = 0;
  while ((rc = getopt_long(argc, argv, ":c:fBECDF:vdqh", ag_long_options, NULL)) >= 0)
    {
      switch (rc)
        {
        case '?':
          error("Unknown option");
          break;
        case ':':
          error("Expecting option parameter");
          break;
        case 'c':
          masterConfig = optarg;
          break;

        case 'f':
          fullImage = true;
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

        case 'F':
          forcedBackupSet = optarg;
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
# 596 "kind.ag"

# 597 "kind.ag"
  int exitCode = 0;
# 598 "kind.ag"
  string lockFile;
# 599 "kind.ag"
  try
# 600 "kind.ag"
    {
# 601 "kind.ag"
      // handling of parameters and switches
# 602 "kind.ag"
      if (debug)        // debug implies verbose
# 603 "kind.ag"
        verbose = true;
# 604 "kind.ag"

# 605 "kind.ag"
      if (!doBackup && !doExpire && !listConfig)
# 606 "kind.ag"
        {
# 607 "kind.ag"
          doBackup = true;
# 608 "kind.ag"
          doExpire = true;
# 609 "kind.ag"
        }
# 610 "kind.ag"

# 611 "kind.ag"
      KindConfig conf;
# 612 "kind.ag"

# 613 "kind.ag"
      // default-values
# 614 "kind.ag"
      conf.add("imageName", "image");
# 615 "kind.ag"
      conf.add("vaultConfigName", "kind/vault.conf");
# 616 "kind.ag"
      conf.add("expireFailedImage", "3 days");
# 617 "kind.ag"
      conf.add("expireRule", "* * * * 1 month");
# 618 "kind.ag"
      conf.add("rsyncOption", ""); // no additional rsync option
# 619 "kind.ag"
      conf.add("remoteShell", "");
# 620 "kind.ag"
      conf.add("lockfile", "/var/lock/kind");
# 621 "kind.ag"
      conf.add("userExcludeFile", "nobackup.list");
# 622 "kind.ag"
      conf.add("userExcludeCommand",
# 623 "kind.ag"
               "find %path -type f -iname '*nobackup' -printf '%P\\\\n'");
# 624 "kind.ag"
      conf.add("logSize", "");
# 625 "kind.ag"

# 626 "kind.ag"
      if (listConfig)
# 627 "kind.ag"
        {
# 628 "kind.ag"
          cout << "builtin config" << endl;
# 629 "kind.ag"
          conf.print(".   ");
# 630 "kind.ag"
        }
# 631 "kind.ag"

# 632 "kind.ag"
      readMasterConfig(masterConfig, conf);
# 633 "kind.ag"

# 634 "kind.ag"
      banks = conf.getStrings("bank");
# 635 "kind.ag"
      if (banks.empty())
# 636 "kind.ag"
        throw Exception("read master configuration", "no banks defined");
# 637 "kind.ag"

# 638 "kind.ag"
      if (listConfig)
# 639 "kind.ag"
        {
# 640 "kind.ag"
          cout << "global config:" << endl;
# 641 "kind.ag"
          conf.print(".   ");
# 642 "kind.ag"
          readVaultConfig(vault, conf);
# 643 "kind.ag"
          cout << "vault config:" << endl;
# 644 "kind.ag"
          conf.print(".   ");
# 645 "kind.ag"
          exit(0);
# 646 "kind.ag"
        }
# 647 "kind.ag"

# 648 "kind.ag"
      lockFile = conf.getString("lockfile");
# 649 "kind.ag"
      createLock(lockFile);
# 650 "kind.ag"

# 651 "kind.ag"
      DateTime imageTime = DateTime::now();
# 652 "kind.ag"

# 653 "kind.ag"
      string logSizeFile = conf.getString("logSize");
# 654 "kind.ag"
      if (!logSizeFile.empty())
# 655 "kind.ag"
        readSizes(logSizeFile);
# 656 "kind.ag"

# 657 "kind.ag"
      vector<string> vaults;
# 658 "kind.ag"
      string groupname = "group_" + vault;
# 659 "kind.ag"
      if (conf.hasKey(groupname))
# 660 "kind.ag"
        vaults = conf.getStrings(groupname);
# 661 "kind.ag"
      else
# 662 "kind.ag"
        vaults.push_back(vault);
# 663 "kind.ag"

# 664 "kind.ag"
      if (doBackup)
# 665 "kind.ag"
        for (string vault : vaults)
# 666 "kind.ag"
          {
# 667 "kind.ag"
            backupVault(vault, conf, imageTime, fullImage, forcedBackupSet);
# 668 "kind.ag"
            writeSizes(logSizeFile);
# 669 "kind.ag"
          }
# 670 "kind.ag"

# 671 "kind.ag"
      if (doExpire)
# 672 "kind.ag"
        for (unsigned int i = 0; i < vaults.size(); ++i)
# 673 "kind.ag"
          expireVault(vaults[i], conf, imageTime);
# 674 "kind.ag"

# 675 "kind.ag"
      if (!quiet)
# 676 "kind.ag"
        cout << DateTime::now().getString('h') << ": finished" << endl;
# 677 "kind.ag"

# 678 "kind.ag"
    }
# 679 "kind.ag"
  catch (const Exception& ex)
# 680 "kind.ag"
    {
# 681 "kind.ag"
      cerr << "Exception: " << ex.what() << endl;
# 682 "kind.ag"
      exitCode = 1;
# 683 "kind.ag"
    }
# 684 "kind.ag"
  catch (const char* msg)
# 685 "kind.ag"
    {
# 686 "kind.ag"
      cerr << "Exception(char*): " << msg << endl;
# 687 "kind.ag"
      exitCode = 1;
# 688 "kind.ag"
    }
# 689 "kind.ag"
  catch (const string& msg)
# 690 "kind.ag"
    {
# 691 "kind.ag"
      cerr << "Exception(string): " << msg << endl;
# 692 "kind.ag"
      exitCode = 1;
# 693 "kind.ag"
    }
# 694 "kind.ag"
  removeLock(lockFile);
# 695 "kind.ag"
  return exitCode;
# 696 "kind.ag"
}
