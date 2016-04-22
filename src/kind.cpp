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

# 39 "kind.ag"
  opt: f, full, void, fullImage, Force full image == initial backup, false
# 40 "kind.ag"
  opt: c, masterconfig, string, masterConfig, Master config file, ""
# 41 "kind.ag"
  opt2: if not given or empty kind looks for
# 42 "kind.ag"
  opt2:   /etc/kind/master.conf
# 43 "kind.ag"
  opt2:   /ffp/etc/kind/master.conf
# 44 "kind.ag"
  opt: B, backup, void, doBackup, Backup, false
# 45 "kind.ag"
  opt: E, expire, void, doExpire, Expire, false
# 46 "kind.ag"
  opt: C, listconfig, void, listConfig, Show configuration, false
# 47 "kind.ag"
  opt2:   if none of backup, expire or listconfig is specified,
# 48 "kind.ag"
  opt2:   backup and expire is assumed.
# 49 "kind.ag"
  opt: D, dryrun, Void, dryRun, Dry run (no real backup), false
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
string getImageName(const KindConfig& conf)
# 153 "kind.ag"
{
# 154 "kind.ag"
  bool nonPortable = false;
# 155 "kind.ag"
  string res = conf.getString("imageName");
# 156 "kind.ag"
  for (unsigned int i = 0; !nonPortable && i < res.size(); ++i)
# 157 "kind.ag"
    {
# 158 "kind.ag"
      char c = res[i];
# 159 "kind.ag"
      if (!isalnum(c) && c != '.' && c != '_')
# 160 "kind.ag"
        nonPortable = true;
# 161 "kind.ag"
    }
# 162 "kind.ag"
  if (nonPortable)
# 163 "kind.ag"
    throw Exception("getImageName", "Invalid character in image name " + res);
# 164 "kind.ag"
  return res;
# 165 "kind.ag"
}
# 166 "kind.ag"

# 167 "kind.ag"
Images findImages(const string& vaultpath, const KindConfig& conf, bool all)
# 168 "kind.ag"
{
# 169 "kind.ag"
  Strings dirs;
# 170 "kind.ag"
  debugPrint("searching images in " + vaultpath);
# 171 "kind.ag"
  dirList(vaultpath, dirs);
# 172 "kind.ag"

# 173 "kind.ag"
  Images imageList;
# 174 "kind.ag"
  for (string dir : dirs)
# 175 "kind.ag"
    {
# 176 "kind.ag"
      FileName fn(dir);
# 177 "kind.ag"
      string imgname = getImageName(conf);
# 178 "kind.ag"
      if (startsWith(fn.getName(), imgname))
# 179 "kind.ag"
        {
# 180 "kind.ag"
          debugPrint("Checking " + dir);
# 181 "kind.ag"
          Image image(dir);
# 182 "kind.ag"

# 183 "kind.ag"
          if (all || image.valid)
# 184 "kind.ag"
            imageList.push_back(image);
# 185 "kind.ag"
        }
# 186 "kind.ag"
    }
# 187 "kind.ag"
  if (imageList.size() > 1)
# 188 "kind.ag"
    sort(imageList.begin(), imageList.end());
# 189 "kind.ag"
  return imageList;
# 190 "kind.ag"
}
# 191 "kind.ag"

# 192 "kind.ag"
void doBackup(const string& vault,
# 193 "kind.ag"
              const string& imageFullName,
# 194 "kind.ag"
              const string& referenceImage,
# 195 "kind.ag"
              const KindConfig& conf)
# 196 "kind.ag"
{
# 197 "kind.ag"
  // create image path
# 198 "kind.ag"
  if (!dryRun)
# 199 "kind.ag"
    if (mkdir(imageFullName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
# 200 "kind.ag"
      throw Exception("Create image", "failed to create " + imageFullName);
# 201 "kind.ag"

# 202 "kind.ag"
  // error message
# 203 "kind.ag"
  // we write an generic error message to mark backup as unsuccessful
# 204 "kind.ag"
  // will be deleted at successful end of rsync
# 205 "kind.ag"
  string errorfile = imageFullName + "/error";
# 206 "kind.ag"
  if (!dryRun)
# 207 "kind.ag"
    {
# 208 "kind.ag"
      ofstream error(errorfile);
# 209 "kind.ag"
      error << "failed" << endl;
# 210 "kind.ag"
      error.close();
# 211 "kind.ag"
    }
# 212 "kind.ag"

# 213 "kind.ag"
  // create source descriptor
# 214 "kind.ag"
  string host;
# 215 "kind.ag"
  if (conf.hasKey("host"))
# 216 "kind.ag"
    host = conf.getString("host");
# 217 "kind.ag"

# 218 "kind.ag"
  string server;
# 219 "kind.ag"
  if (conf.hasKey("server"))
# 220 "kind.ag"
    server = conf.getString("server");
# 221 "kind.ag"

# 222 "kind.ag"
  if (!host.empty() && !server.empty())
# 223 "kind.ag"
    throw Exception("backupVault", "Cannot have host and server");
# 224 "kind.ag"

# 225 "kind.ag"
  string path = conf.getString("path");
# 226 "kind.ag"
  if (path.empty())
# 227 "kind.ag"
    throw Exception("rsync", "empty source path");
# 228 "kind.ag"
  if (path.back() != '/')
# 229 "kind.ag"
    path += '/';
# 230 "kind.ag"

# 231 "kind.ag"
  string rsyncCmd = "rsync -vrltH --delete --stats -D --numeric-ids ";
# 232 "kind.ag"
  if (!conf.getBool("ignorePermission"))
# 233 "kind.ag"
    rsyncCmd += "-pgo";
# 234 "kind.ag"
  vector<string> rso = conf.getStrings("rsyncOption");
# 235 "kind.ag"
  for (const string& opt : rso)
# 236 "kind.ag"
    rsyncCmd += opt + " ";
# 237 "kind.ag"

# 238 "kind.ag"
  // excludes
# 239 "kind.ag"
  Strings excluded;
# 240 "kind.ag"

# 241 "kind.ag"
  if (conf.hasKey("exclude"))
# 242 "kind.ag"
    excluded += conf.getStrings("exclude");
# 243 "kind.ag"

# 244 "kind.ag"
  if (!host.empty())  // shell mode
# 245 "kind.ag"
    {
# 246 "kind.ag"
      // cout << "USING SHELLMODE '" << host << "'" << endl;
# 247 "kind.ag"
      string remoteShell = conf.getString("remoteShell");
# 248 "kind.ag"
      string userAtHost = conf.getString("user") + "@" +
# 249 "kind.ag"
      conf.getString("host");
# 250 "kind.ag"
      string rshCommand = remoteShell;
# 251 "kind.ag"
      if (remoteShell.empty())
# 252 "kind.ag"
        rshCommand = "ssh";
# 253 "kind.ag"

# 254 "kind.ag"
      rshCommand += " " + userAtHost;
# 255 "kind.ag"

# 256 "kind.ag"
      string userExcludeCommand = conf.getString("userExcludeCommand");
# 257 "kind.ag"

# 258 "kind.ag"
      if (!userExcludeCommand.empty())
# 259 "kind.ag"
        {
# 260 "kind.ag"
          replacePlaceHolder(userExcludeCommand, "%path", path);
# 261 "kind.ag"
          string excludeCommand = rshCommand + " " + userExcludeCommand;
# 262 "kind.ag"

# 263 "kind.ag"
          verbosePrint("searching for exclusions (" + excludeCommand + ")");
# 264 "kind.ag"

# 265 "kind.ag"
          int rc;
# 266 "kind.ag"
          Strings excludedFiles = myPopen(excludeCommand, rc, debug);
# 267 "kind.ag"
          if (rc > 0)
# 268 "kind.ag"
            {
# 269 "kind.ag"
              strings2File(excludedFiles, errorfile);
# 270 "kind.ag"
              throw Exception("Find exludes", "Search for excludes failed");
# 271 "kind.ag"
            }
# 272 "kind.ag"

# 273 "kind.ag"
          for (unsigned int i = 0; i < excludedFiles.size(); ++i)
# 274 "kind.ag"
            {
# 275 "kind.ag"
              FileName fn(excludedFiles[i]);
# 276 "kind.ag"
              excluded.push_back('/' + fn.getPath());
# 277 "kind.ag"
              debugPrint("Excluding: " + excluded.back());
# 278 "kind.ag"
            }
# 279 "kind.ag"
        }
# 280 "kind.ag"

# 281 "kind.ag"
      string userExcludeFile = conf.getString("userExcludeFile");
# 282 "kind.ag"
      if (!userExcludeFile.empty())
# 283 "kind.ag"
        {
# 284 "kind.ag"
          userExcludeFile = path + userExcludeFile;
# 285 "kind.ag"
          string getExcludeFileCommand = rshCommand;
# 286 "kind.ag"
          getExcludeFileCommand += " \" if [ -f '" + userExcludeFile + "' ]; then ";
# 287 "kind.ag"
          getExcludeFileCommand += " cat '" + userExcludeFile + "' ; fi \"";
# 288 "kind.ag"
          // cout << getExcludeFileCommand << endl;
# 289 "kind.ag"
          int rc;
# 290 "kind.ag"
          Strings excludes2 = myPopen(getExcludeFileCommand, rc, debug);
# 291 "kind.ag"
          if (rc == 0)
# 292 "kind.ag"
            excluded += excludes2;
# 293 "kind.ag"
        }
# 294 "kind.ag"

# 295 "kind.ag"
      if (!dryRun)
# 296 "kind.ag"
        strings2File(excluded, imageFullName + "/exclude");
# 297 "kind.ag"

# 298 "kind.ag"
      // rsync image
# 299 "kind.ag"

# 300 "kind.ag"
      if (!remoteShell.empty())
# 301 "kind.ag"
        rsyncCmd += " -e \'" + remoteShell + "\' ";
# 302 "kind.ag"

# 303 "kind.ag"
      rsyncCmd += "--exclude-from=" + imageFullName + "/exclude ";
# 304 "kind.ag"
      if (!referenceImage.empty())
# 305 "kind.ag"
        rsyncCmd += "--link-dest=" + referenceImage + "/tree ";
# 306 "kind.ag"
      rsyncCmd += userAtHost + ":" + path + " ";
# 307 "kind.ag"
      rsyncCmd += imageFullName + "/tree";
# 308 "kind.ag"
    } // shell mode
# 309 "kind.ag"
  else
# 310 "kind.ag"
    {
# 311 "kind.ag"
      // cout << "USING SERVERMODE" << endl;
# 312 "kind.ag"

# 313 "kind.ag"
      if (!dryRun)
# 314 "kind.ag"
        strings2File(excluded, imageFullName + "/exclude");
# 315 "kind.ag"

# 316 "kind.ag"
      rsyncCmd += "--exclude-from=" + imageFullName + "/exclude ";
# 317 "kind.ag"
      if (!referenceImage.empty())
# 318 "kind.ag"
        rsyncCmd += "--link-dest=" + referenceImage + "/tree ";
# 319 "kind.ag"
      rsyncCmd += conf.getString("server") + "::" + path + " ";
# 320 "kind.ag"
      rsyncCmd += imageFullName + "/tree";
# 321 "kind.ag"
    }
# 322 "kind.ag"

# 323 "kind.ag"
  debugPrint("Action: " + rsyncCmd);
# 324 "kind.ag"

# 325 "kind.ag"
  vector<string> backupResult;
# 326 "kind.ag"
  if (!dryRun)
# 327 "kind.ag"
    {
# 328 "kind.ag"
      verbosePrint("syncing (" + rsyncCmd + ")");
# 329 "kind.ag"
      int rc;
# 330 "kind.ag"
      backupResult = myPopen(rsyncCmd, rc, debug, imageFullName + "/rsync-log");
# 331 "kind.ag"
      if (rc == 0 ||
# 332 "kind.ag"
      rc == 24 || // "no error" or "vanished source files" (ignored)
# 333 "kind.ag"
      rc == 6144) // workaround for wrong exit code ??!!
# 334 "kind.ag"
        {
# 335 "kind.ag"
          unlink(errorfile.c_str());
# 336 "kind.ag"
          long int st = 0;
# 337 "kind.ag"
          long int sc = 0;
# 338 "kind.ag"
          for (auto bl : backupResult)
# 339 "kind.ag"
            {
# 340 "kind.ag"
              if (startsWith(bl, "Total file size"))
# 341 "kind.ag"
                st = getNumber(bl);
# 342 "kind.ag"
              else if (startsWith(bl, "Total transferred file size"))
# 343 "kind.ag"
                sc = getNumber(bl);
# 344 "kind.ag"
            }
# 345 "kind.ag"
          // sizes[vault] = pair<long int, long int>(st, sc);
# 346 "kind.ag"
          sizes[vault] = Sizes(st, sc);
# 347 "kind.ag"
          //  cout << vault << " " << st << " || " << sc << endl;
# 348 "kind.ag"
        }
# 349 "kind.ag"
      else
# 350 "kind.ag"
        throw Exception("Backup", "Failed to execute rsync (result: " + to_string(rc) + ")");
# 351 "kind.ag"
    }
# 352 "kind.ag"
  else
# 353 "kind.ag"
    cout << "Not executing " << rsyncCmd << endl;
# 354 "kind.ag"
}
# 355 "kind.ag"

# 356 "kind.ag"
void backupVault(const string& vault,
# 357 "kind.ag"
                 KindConfig conf /*Copy!*/ ,
# 358 "kind.ag"
                 const DateTime& imageTime,
# 359 "kind.ag"
                 bool fullImage)
# 360 "kind.ag"
{
# 361 "kind.ag"
  if (!quiet)
# 362 "kind.ag"
    cout << DateTime::now().getString('h') << ": Backup of vault " << vault << endl;
# 363 "kind.ag"
  try
# 364 "kind.ag"
    {
# 365 "kind.ag"
      sizes[vault].second = 0; // nothing backed up yet
# 366 "kind.ag"

# 367 "kind.ag"
      readVaultConfig(vault, conf);
# 368 "kind.ag"

# 369 "kind.ag"
      // where to store
# 370 "kind.ag"
      string vaultpath = findVault(vault);
# 371 "kind.ag"

# 372 "kind.ag"
      // image path
# 373 "kind.ag"
      string imageName = getImageName(conf);
# 374 "kind.ag"
      if (!imageName.empty())
# 375 "kind.ag"
        imageName += '-';
# 376 "kind.ag"

# 377 "kind.ag"
      string imageFullName =  vaultpath + "/" + imageName ;
# 378 "kind.ag"

# 379 "kind.ag"
      if (conf.getBool("longImageName"))
# 380 "kind.ag"
        imageFullName += imageTime.getString('m');
# 381 "kind.ag"
      else
# 382 "kind.ag"
        imageFullName += imageTime.getString('s');
# 383 "kind.ag"

# 384 "kind.ag"
      bool backupNow = true;
# 385 "kind.ag"

# 386 "kind.ag"
      // existing images
# 387 "kind.ag"
      Images validImageList = findImages(vaultpath, conf, false);
# 388 "kind.ag"
      string currentSet = "expire"; // we are not using backupSets
# 389 "kind.ag"

# 390 "kind.ag"
      // check if we are using backup sets
# 391 "kind.ag"

# 392 "kind.ag"
      map<string, int> setIdx;
# 393 "kind.ag"
      vector<SetRule> backupSetRule;
# 394 "kind.ag"
      int setRuleIdx = -1;
# 395 "kind.ag"

# 396 "kind.ag"
      if (conf.hasKey("setRule"))
# 397 "kind.ag"
        {
# 398 "kind.ag"
          readSetRules(conf, setIdx, backupSetRule);
# 399 "kind.ag"
          if (!setIdx.empty())
# 400 "kind.ag"
            {
# 401 "kind.ag"
              backupNow = false;
# 402 "kind.ag"

# 403 "kind.ag"
              // find time for nextBackup for every backupSet
# 404 "kind.ag"
              // defaults to now == imageTime;
# 405 "kind.ag"
              vector<DateTime> nextBackup(backupSetRule.size(), imageTime);
# 406 "kind.ag"

# 407 "kind.ag"
              // find time for next backup
# 408 "kind.ag"

# 409 "kind.ag"
              for (const Image& image : validImageList)
# 410 "kind.ag"
                {
# 411 "kind.ag"
                  if (image.series != "expire")
# 412 "kind.ag"
                    {
# 413 "kind.ag"
                      string s = image.series;
# 414 "kind.ag"
                      if (setIdx.count(s) > 0) // rule for set exists?
# 415 "kind.ag"
                        {
# 416 "kind.ag"
                          int rIdx = setIdx[s];
# 417 "kind.ag"
                          // image is valid for this and "lower" backupSets
# 418 "kind.ag"
                          for (unsigned int i = rIdx; i < backupSetRule.size(); ++i)
# 419 "kind.ag"
                            if (nextBackup[i] < image.time + backupSetRule[i].distance)
# 420 "kind.ag"
                              nextBackup[i] =  image.time + backupSetRule[i].distance;
# 421 "kind.ag"
                        }
# 422 "kind.ag"
                    }
# 423 "kind.ag"
                }
# 424 "kind.ag"
              if (debug)
# 425 "kind.ag"
                for (unsigned int i = 0; i < backupSetRule.size(); ++i)
# 426 "kind.ag"
                  cout << "       Next backup for " << backupSetRule[i].name << " at " <<  nextBackup[i].getString('h') << endl;
# 427 "kind.ag"

# 428 "kind.ag"
              // find backupSet that
# 429 "kind.ag"
              //    - needs backup
# 430 "kind.ag"
              //    - has longest time to keep
# 431 "kind.ag"
              // because of ordered list backupSetRule this is the first set, that need
# 432 "kind.ag"

# 433 "kind.ag"
              currentSet = "";
# 434 "kind.ag"
              for (unsigned int i = 0; i < backupSetRule.size() && currentSet.empty(); ++i)
# 435 "kind.ag"
                {
# 436 "kind.ag"
                  string name = backupSetRule[i].name;
# 437 "kind.ag"
                  if (nextBackup[i] <= imageTime + 5) // small offset of 5s for "jitter"
# 438 "kind.ag"
                    {
# 439 "kind.ag"
                      backupNow = true;
# 440 "kind.ag"
                      currentSet = name;
# 441 "kind.ag"
                      setRuleIdx = i;
# 442 "kind.ag"
                    }
# 443 "kind.ag"
                }
# 444 "kind.ag"
            }
# 445 "kind.ag"
        }
# 446 "kind.ag"

# 447 "kind.ag"
      if (backupNow)
# 448 "kind.ag"
        {
# 449 "kind.ag"
          verbosePrint("backup to \"" + imageFullName + "\"");
# 450 "kind.ag"
          if (setRuleIdx >= 0 && !quiet)
# 451 "kind.ag"
            cout << "  backup set is \"" << currentSet << "\"" << endl;
# 452 "kind.ag"
        }
# 453 "kind.ag"
      else if (!quiet)
# 454 "kind.ag"
        cout << "  no backup set needs update" << endl;
# 455 "kind.ag"

# 456 "kind.ag"
      if (backupNow)
# 457 "kind.ag"
        {
# 458 "kind.ag"
          // find reference image
# 459 "kind.ag"
          string referenceImage;
# 460 "kind.ag"
          if (!fullImage)
# 461 "kind.ag"
            {
# 462 "kind.ag"
              if (validImageList.empty())
# 463 "kind.ag"
                throw Exception("backupVault", "no reference image found");
# 464 "kind.ag"
              // last image is newest image
# 465 "kind.ag"
              referenceImage = validImageList.back().name;
# 466 "kind.ag"
            }
# 467 "kind.ag"

# 468 "kind.ag"
          doBackup(vault, imageFullName, referenceImage, conf);
# 469 "kind.ag"

# 470 "kind.ag"
          if (!dryRun)
# 471 "kind.ag"
            {
# 472 "kind.ag"
              // set symlink to last image
# 473 "kind.ag"
              string lastLink = vaultpath + "/last";
# 474 "kind.ag"
              unlink(lastLink.c_str());
# 475 "kind.ag"
              symlink(imageFullName.c_str(), lastLink.c_str());
# 476 "kind.ag"

# 477 "kind.ag"
              // write expire date to file
# 478 "kind.ag"
              DateTime expireTime;
# 479 "kind.ag"
              string rule;
# 480 "kind.ag"
              if (setRuleIdx < 0) // not backup set based
# 481 "kind.ag"
                expireTime = getExpireDate(imageTime, conf, rule);
# 482 "kind.ag"
              else
# 483 "kind.ag"
                {
# 484 "kind.ag"
                  expireTime = imageTime + backupSetRule[setRuleIdx].keep;
# 485 "kind.ag"
                  rule =  backupSetRule[setRuleIdx].rule;
# 486 "kind.ag"
                }
# 487 "kind.ag"

# 488 "kind.ag"
              ofstream expireFile(imageFullName + "/expires");
# 489 "kind.ag"
              expireFile << currentSet << "-" << expireTime.getString('m') << endl;
# 490 "kind.ag"
              expireFile << rule << endl;
# 491 "kind.ag"
            }
# 492 "kind.ag"
        }
# 493 "kind.ag"
    }
# 494 "kind.ag"
  catch (Exception ex)
# 495 "kind.ag"
    {
# 496 "kind.ag"
      cerr << "Exception in vault " << vault << ": " << ex.what() << endl;
# 497 "kind.ag"
    }
# 498 "kind.ag"
}
# 499 "kind.ag"

# 500 "kind.ag"
void expireVault(const string& vault, KindConfig conf, DateTime now)
# 501 "kind.ag"
{
# 502 "kind.ag"
  if (!quiet)
# 503 "kind.ag"
    cout << DateTime::now().getString('h') << ": Expiring images in vault " << vault << endl;
# 504 "kind.ag"

# 505 "kind.ag"
  readVaultConfig(vault, conf);
# 506 "kind.ag"

# 507 "kind.ag"
  string vaultpath = findVault(vault);
# 508 "kind.ag"

# 509 "kind.ag"
  Images imagelist = findImages(vaultpath, conf, true);
# 510 "kind.ag"

# 511 "kind.ag"
  string lastValidImage;
# 512 "kind.ag"
  for (Image image : imagelist)
# 513 "kind.ag"
    {
# 514 "kind.ag"
      if (image.valid)
# 515 "kind.ag"
        lastValidImage = image.name;
# 516 "kind.ag"
    }
# 517 "kind.ag"

# 518 "kind.ag"
  for (Image image : imagelist)
# 519 "kind.ag"
    {
# 520 "kind.ag"
      debugPrint(image.name);
# 521 "kind.ag"

# 522 "kind.ag"
      DateTime imageTime = image.time;
# 523 "kind.ag"

# 524 "kind.ag"
      if (imageTime != now &&          // ignore just created image
# 525 "kind.ag"
      image.name != lastValidImage // ignore last valid image
# 526 "kind.ag"
         )
# 527 "kind.ag"
        {
# 528 "kind.ag"
          DateTime expireTime;
# 529 "kind.ag"
          string expireRule;
# 530 "kind.ag"
          if (!image.valid) // invalid image?
# 531 "kind.ag"
            {
# 532 "kind.ag"
              time_t expPeriod = stot(conf.getString("expireFailedImage"));
# 533 "kind.ag"
              if (expPeriod < 0)
# 534 "kind.ag"
                throw Exception("expireFailedImage", "Time period must be positive");
# 535 "kind.ag"
              expireTime = imageTime + stot(conf.getString("expireFailedImage"));
# 536 "kind.ag"
              expireRule = "invalid image: " + conf.getString("expireFailedImage");
# 537 "kind.ag"
              debugPrint("- invalid image");
# 538 "kind.ag"
            }
# 539 "kind.ag"
          else
# 540 "kind.ag"
            {
# 541 "kind.ag"
              debugPrint("- valid image");
# 542 "kind.ag"
              expireTime = image.expire;
# 543 "kind.ag"
              expireRule = image.expireRule;
# 544 "kind.ag"
            }
# 545 "kind.ag"

# 546 "kind.ag"
          if (debug)
# 547 "kind.ag"
            {
# 548 "kind.ag"
              cout << "    image: " << imageTime.getString('h') << endl;
# 549 "kind.ag"
              cout << "      expire: " << expireTime.getString('h') << " " << expireRule << endl;
# 550 "kind.ag"
              cout << "      now: " << now.getString('h') << endl;
# 551 "kind.ag"
            }
# 552 "kind.ag"

# 553 "kind.ag"
          if (expireTime < now)
# 554 "kind.ag"
            {
# 555 "kind.ag"
              if (!quiet)
# 556 "kind.ag"
                cout << "  removing image " << image.name << endl;
# 557 "kind.ag"
              try
# 558 "kind.ag"
                {
# 559 "kind.ag"
                  if (removeDir(image.name) != 0)
# 560 "kind.ag"
                    cout << "Error removing " <<  image.name << endl;
# 561 "kind.ag"
                }
# 562 "kind.ag"
              catch (Exception ex)
# 563 "kind.ag"
                {
# 564 "kind.ag"
                  cerr << "Exception: " << ex.what() << endl;
# 565 "kind.ag"
                }
# 566 "kind.ag"
            }
# 567 "kind.ag"
        }
# 568 "kind.ag"
      else
# 569 "kind.ag"
        debugPrint("- current image - ignored");
# 570 "kind.ag"
    }
# 571 "kind.ag"
}
# 572 "kind.ag"

# 573 "kind.ag"
/*AppGen:Main*/
string ag_programName;

void usage()
{
  cout << ag_programName << " - archiving backup" << endl;
  cout << "Aufruf:" << endl;
  cout << ag_programName << " [<options>] vault_or_group " << endl;
  cout << "  vault_or_group - Vault to backup" << endl;
  cout << "Optionen:" << endl;
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
# 577 "kind.ag"

# 578 "kind.ag"
  int exitCode = 0;
# 579 "kind.ag"
  string lockFile;
# 580 "kind.ag"
  try
# 581 "kind.ag"
    {
# 582 "kind.ag"
      // handling of parameters and switches
# 583 "kind.ag"
      if (debug)        // debug implies verbose
# 584 "kind.ag"
        verbose = true;
# 585 "kind.ag"

# 586 "kind.ag"
      if (!doBackup && !doExpire && !listConfig)
# 587 "kind.ag"
        {
# 588 "kind.ag"
          doBackup = true;
# 589 "kind.ag"
          doExpire = true;
# 590 "kind.ag"
        }
# 591 "kind.ag"

# 592 "kind.ag"
      KindConfig conf;
# 593 "kind.ag"

# 594 "kind.ag"
      // default-values
# 595 "kind.ag"
      conf.add("imageName", "image");
# 596 "kind.ag"
      conf.add("vaultConfigName", "kind/vault.conf");
# 597 "kind.ag"
      conf.add("expireFailedImage", "3 days");
# 598 "kind.ag"
      conf.add("expireRule", "* * * * 1 month");
# 599 "kind.ag"
      conf.add("rsyncOption", ""); // no additional rsync option
# 600 "kind.ag"
      conf.add("remoteShell", "");
# 601 "kind.ag"
      conf.add("lockfile", "/var/lock/kind");
# 602 "kind.ag"
      conf.add("userExcludeFile", "nobackup.list");
# 603 "kind.ag"
      conf.add("userExcludeCommand",
# 604 "kind.ag"
               "find %path -type f -iname '*nobackup' -printf '%P\\\\n'");
# 605 "kind.ag"
      conf.add("logSize", "");
# 606 "kind.ag"

# 607 "kind.ag"
      if (listConfig)
# 608 "kind.ag"
        {
# 609 "kind.ag"
          cout << "builtin config" << endl;
# 610 "kind.ag"
          conf.print(".   ");
# 611 "kind.ag"
        }
# 612 "kind.ag"

# 613 "kind.ag"
      readMasterConfig(masterConfig, conf);
# 614 "kind.ag"

# 615 "kind.ag"
      banks = conf.getStrings("bank");
# 616 "kind.ag"
      if (banks.empty())
# 617 "kind.ag"
        throw Exception("read master configuration", "no banks defined");
# 618 "kind.ag"

# 619 "kind.ag"
      if (listConfig)
# 620 "kind.ag"
        {
# 621 "kind.ag"
          cout << "global config:" << endl;
# 622 "kind.ag"
          conf.print(".   ");
# 623 "kind.ag"
          readVaultConfig(vault, conf);
# 624 "kind.ag"
          cout << "vault config:" << endl;
# 625 "kind.ag"
          conf.print(".   ");
# 626 "kind.ag"
          exit(0);
# 627 "kind.ag"
        }
# 628 "kind.ag"

# 629 "kind.ag"
      lockFile = conf.getString("lockfile");
# 630 "kind.ag"
      createLock(lockFile);
# 631 "kind.ag"

# 632 "kind.ag"
      DateTime imageTime = DateTime::now();
# 633 "kind.ag"

# 634 "kind.ag"
      string logSizeFile = conf.getString("logSize");
# 635 "kind.ag"
      if (!logSizeFile.empty())
# 636 "kind.ag"
        readSizes(logSizeFile);
# 637 "kind.ag"

# 638 "kind.ag"
      vector<string> vaults;
# 639 "kind.ag"
      string groupname = "group_" + vault;
# 640 "kind.ag"
      if (conf.hasKey(groupname))
# 641 "kind.ag"
        vaults = conf.getStrings(groupname);
# 642 "kind.ag"
      else
# 643 "kind.ag"
        vaults.push_back(vault);
# 644 "kind.ag"

# 645 "kind.ag"
      if (doBackup)
# 646 "kind.ag"
        for (string vault : vaults)
# 647 "kind.ag"
          {
# 648 "kind.ag"
            backupVault(vault, conf, imageTime, fullImage);
# 649 "kind.ag"
            writeSizes(logSizeFile);
# 650 "kind.ag"
          }
# 651 "kind.ag"

# 652 "kind.ag"
      if (doExpire)
# 653 "kind.ag"
        for (unsigned int i = 0; i < vaults.size(); ++i)
# 654 "kind.ag"
          expireVault(vaults[i], conf, imageTime);
# 655 "kind.ag"

# 656 "kind.ag"
      if (!quiet)
# 657 "kind.ag"
        cout << DateTime::now().getString('h') << ": finished" << endl;
# 658 "kind.ag"

# 659 "kind.ag"
    }
# 660 "kind.ag"
  catch (const Exception& ex)
# 661 "kind.ag"
    {
# 662 "kind.ag"
      cerr << "Exception: " << ex.what() << endl;
# 663 "kind.ag"
      exitCode = 1;
# 664 "kind.ag"
    }
# 665 "kind.ag"
  catch (const char* msg)
# 666 "kind.ag"
    {
# 667 "kind.ag"
      cerr << "Exception(char*): " << msg << endl;
# 668 "kind.ag"
      exitCode = 1;
# 669 "kind.ag"
    }
# 670 "kind.ag"
  catch (const string& msg)
# 671 "kind.ag"
    {
# 672 "kind.ag"
      cerr << "Exception(string): " << msg << endl;
# 673 "kind.ag"
      exitCode = 1;
# 674 "kind.ag"
    }
# 675 "kind.ag"
  removeLock(lockFile);
# 676 "kind.ag"
  return exitCode;
# 677 "kind.ag"
}
