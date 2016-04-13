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
#if 0
# 168 "kind.ag"
bool isValidImage(const string& imageName)
# 169 "kind.ag"
{
# 170 "kind.ag"
  return dirExists(imageName) &&
# 171 "kind.ag"
  !fileExists(imageName + "/error") &&
# 172 "kind.ag"
  fileExists(imageName + "/expires") &&
# 173 "kind.ag"
  dirExists(imageName + "/tree");
# 174 "kind.ag"
}
# 175 "kind.ag"
#endif
# 176 "kind.ag"

# 177 "kind.ag"
Images findImages(const string& vaultpath, const KindConfig& conf, bool all)
# 178 "kind.ag"
{
# 179 "kind.ag"
  Strings dirs;
# 180 "kind.ag"
  debugPrint("searching images in " + vaultpath);
# 181 "kind.ag"
  dirList(vaultpath, dirs);
# 182 "kind.ag"

# 183 "kind.ag"
  Images imageList;
# 184 "kind.ag"
  for (string dir : dirs)
# 185 "kind.ag"
    {
# 186 "kind.ag"
      FileName fn(dir);
# 187 "kind.ag"
      string imgname = getImageName(conf);
# 188 "kind.ag"
      if (startsWith(fn.getName(), imgname))
# 189 "kind.ag"
        {
# 190 "kind.ag"
          debugPrint("Checking " + dir);
# 191 "kind.ag"
          Image image(dir);
# 192 "kind.ag"

# 193 "kind.ag"
          if (all || image.valid)
# 194 "kind.ag"
            imageList.push_back(image);
# 195 "kind.ag"
        }
# 196 "kind.ag"
    }
# 197 "kind.ag"
  if (imageList.size() > 1)
# 198 "kind.ag"
    sort(imageList.begin(), imageList.end());
# 199 "kind.ag"
  return imageList;
# 200 "kind.ag"
}
# 201 "kind.ag"

# 202 "kind.ag"
void doBackup(const string& vault,
# 203 "kind.ag"
              const string& imageFullName,
# 204 "kind.ag"
              const string& referenceImage,
# 205 "kind.ag"
              const KindConfig& conf)
# 206 "kind.ag"
{
# 207 "kind.ag"
  // create image path
# 208 "kind.ag"
  if (!dryRun)
# 209 "kind.ag"
    if (mkdir(imageFullName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
# 210 "kind.ag"
      throw Exception("Create image", "failed to create " + imageFullName);
# 211 "kind.ag"

# 212 "kind.ag"
  // error message
# 213 "kind.ag"
  // we write an generic error message to mark backup as unsuccessful
# 214 "kind.ag"
  // will be deleted at successful end of rsync
# 215 "kind.ag"
  string errorfile = imageFullName + "/error";
# 216 "kind.ag"
  if (!dryRun)
# 217 "kind.ag"
    {
# 218 "kind.ag"
      ofstream error(errorfile);
# 219 "kind.ag"
      error << "failed" << endl;
# 220 "kind.ag"
      error.close();
# 221 "kind.ag"
    }
# 222 "kind.ag"

# 223 "kind.ag"
  // create source descriptor
# 224 "kind.ag"
  string host;
# 225 "kind.ag"
  if (conf.hasKey("host"))
# 226 "kind.ag"
    host = conf.getString("host");
# 227 "kind.ag"

# 228 "kind.ag"
  string server;
# 229 "kind.ag"
  if (conf.hasKey("server"))
# 230 "kind.ag"
    server = conf.getString("server");
# 231 "kind.ag"

# 232 "kind.ag"
  if (!host.empty() && !server.empty())
# 233 "kind.ag"
    throw Exception("backupVault", "Cannot have host and server");
# 234 "kind.ag"

# 235 "kind.ag"
  string path = conf.getString("path");
# 236 "kind.ag"
  if (path.empty())
# 237 "kind.ag"
    throw Exception("rsync", "empty source path");
# 238 "kind.ag"
  if (path.back() != '/')
# 239 "kind.ag"
    path += '/';
# 240 "kind.ag"

# 241 "kind.ag"
  string rsyncCmd = "rsync -vrltH --delete --stats -D --numeric-ids ";
# 242 "kind.ag"
  if (!conf.getBool("ignorePermission"))
# 243 "kind.ag"
    rsyncCmd += "-pgo";
# 244 "kind.ag"
  vector<string> rso = conf.getStrings("rsyncOption");
# 245 "kind.ag"
  for (const string& opt : rso)
# 246 "kind.ag"
    rsyncCmd += opt + " ";
# 247 "kind.ag"

# 248 "kind.ag"
  // excludes
# 249 "kind.ag"
  Strings excluded;
# 250 "kind.ag"

# 251 "kind.ag"
  if (conf.hasKey("exclude"))
# 252 "kind.ag"
    excluded += conf.getStrings("exclude");
# 253 "kind.ag"

# 254 "kind.ag"
  if (!host.empty())  // shell mode
# 255 "kind.ag"
    {
# 256 "kind.ag"
      // cout << "USING SHELLMODE '" << host << "'" << endl;
# 257 "kind.ag"
      string remoteShell = conf.getString("remoteShell");
# 258 "kind.ag"
      string userAtHost = conf.getString("user") + "@" +
# 259 "kind.ag"
      conf.getString("host");
# 260 "kind.ag"
      string rshCommand = remoteShell;
# 261 "kind.ag"
      if (remoteShell.empty())
# 262 "kind.ag"
        rshCommand = "ssh";
# 263 "kind.ag"

# 264 "kind.ag"
      rshCommand += " " + userAtHost;
# 265 "kind.ag"

# 266 "kind.ag"
      string userExcludeCommand = conf.getString("userExcludeCommand");
# 267 "kind.ag"

# 268 "kind.ag"
      if (!userExcludeCommand.empty())
# 269 "kind.ag"
        {
# 270 "kind.ag"
          replacePlaceHolder(userExcludeCommand, "%path", path);
# 271 "kind.ag"
          string excludeCommand = rshCommand + " " + userExcludeCommand;
# 272 "kind.ag"

# 273 "kind.ag"
          verbosePrint("searching for exclusions (" + excludeCommand + ")");
# 274 "kind.ag"

# 275 "kind.ag"
          int rc;
# 276 "kind.ag"
          Strings excludedFiles = myPopen(excludeCommand, rc, debug);
# 277 "kind.ag"
          if (rc > 0)
# 278 "kind.ag"
            throw Exception("Find exludes", "Search for excludes failed");
# 279 "kind.ag"

# 280 "kind.ag"
          for (unsigned int i = 0; i < excludedFiles.size(); ++i)
# 281 "kind.ag"
            {
# 282 "kind.ag"
              FileName fn(excludedFiles[i]);
# 283 "kind.ag"
              excluded.push_back('/' + fn.getPath());
# 284 "kind.ag"
              debugPrint("Excluding: " + excluded.back());
# 285 "kind.ag"
            }
# 286 "kind.ag"
        }
# 287 "kind.ag"

# 288 "kind.ag"
      string userExcludeFile = conf.getString("userExcludeFile");
# 289 "kind.ag"
      if (!userExcludeFile.empty())
# 290 "kind.ag"
        {
# 291 "kind.ag"
          userExcludeFile = path + userExcludeFile;
# 292 "kind.ag"
          string getExcludeFileCommand = rshCommand;
# 293 "kind.ag"
          getExcludeFileCommand += " \" if [ -f '" + userExcludeFile + "' ]; then ";
# 294 "kind.ag"
          getExcludeFileCommand += " cat '" + userExcludeFile + "' ; fi \"";
# 295 "kind.ag"
          // cout << getExcludeFileCommand << endl;
# 296 "kind.ag"
          int rc;
# 297 "kind.ag"
          Strings excludes2 = myPopen(getExcludeFileCommand, rc, debug);
# 298 "kind.ag"
          if (rc == 0)
# 299 "kind.ag"
            excluded += excludes2;
# 300 "kind.ag"
        }
# 301 "kind.ag"

# 302 "kind.ag"
      if (!dryRun)
# 303 "kind.ag"
        strings2File(excluded, imageFullName + "/exclude");
# 304 "kind.ag"

# 305 "kind.ag"
      // rsync image
# 306 "kind.ag"

# 307 "kind.ag"
      if (!remoteShell.empty())
# 308 "kind.ag"
        rsyncCmd += " -e \'" + remoteShell + "\' ";
# 309 "kind.ag"

# 310 "kind.ag"
      rsyncCmd += "--exclude-from=" + imageFullName + "/exclude ";
# 311 "kind.ag"
      if (!referenceImage.empty())
# 312 "kind.ag"
        rsyncCmd += "--link-dest=" + referenceImage + "/tree ";
# 313 "kind.ag"
      rsyncCmd += userAtHost + ":" + path + " ";
# 314 "kind.ag"
      rsyncCmd += imageFullName + "/tree";
# 315 "kind.ag"
    } // shell mode
# 316 "kind.ag"
  else
# 317 "kind.ag"
    {
# 318 "kind.ag"
      // cout << "USING SERVERMODE" << endl;
# 319 "kind.ag"

# 320 "kind.ag"
      if (!dryRun)
# 321 "kind.ag"
        strings2File(excluded, imageFullName + "/exclude");
# 322 "kind.ag"

# 323 "kind.ag"
      rsyncCmd += "--exclude-from=" + imageFullName + "/exclude ";
# 324 "kind.ag"
      if (!referenceImage.empty())
# 325 "kind.ag"
        rsyncCmd += "--link-dest=" + referenceImage + "/tree ";
# 326 "kind.ag"
      rsyncCmd += conf.getString("server") + "::" + path + " ";
# 327 "kind.ag"
      rsyncCmd += imageFullName + "/tree";
# 328 "kind.ag"
    }
# 329 "kind.ag"

# 330 "kind.ag"
  debugPrint("Action: " + rsyncCmd);
# 331 "kind.ag"

# 332 "kind.ag"
  vector<string> backupResult;
# 333 "kind.ag"
  if (!dryRun)
# 334 "kind.ag"
    {
# 335 "kind.ag"
      verbosePrint("syncing (" + rsyncCmd + ")");
# 336 "kind.ag"
      int rc;
# 337 "kind.ag"
      backupResult = myPopen(rsyncCmd, rc, debug, imageFullName + "/rsync-log");
# 338 "kind.ag"
      if (rc == 0 ||
# 339 "kind.ag"
      rc == 24 || // "no error" or "vanished source files" (ignored)
# 340 "kind.ag"
      rc == 6144) // workaround for wrong exit code ??!!
# 341 "kind.ag"
        {
# 342 "kind.ag"
          unlink(errorfile.c_str());
# 343 "kind.ag"
          long int st = 0;
# 344 "kind.ag"
          long int sc = 0;
# 345 "kind.ag"
          for (auto bl : backupResult)
# 346 "kind.ag"
            {
# 347 "kind.ag"
              if (startsWith(bl, "Total file size"))
# 348 "kind.ag"
                st = getNumber(bl);
# 349 "kind.ag"
              else if (startsWith(bl, "Total transferred file size"))
# 350 "kind.ag"
                sc = getNumber(bl);
# 351 "kind.ag"
            }
# 352 "kind.ag"
          // sizes[vault] = pair<long int, long int>(st, sc);
# 353 "kind.ag"
          sizes[vault] = Sizes(st, sc);
# 354 "kind.ag"
          //  cout << vault << " " << st << " || " << sc << endl;
# 355 "kind.ag"
        }
# 356 "kind.ag"
      else
# 357 "kind.ag"
        throw Exception("Backup", "Failed to execute rsync (result: " + to_string(rc) + ")");
# 358 "kind.ag"
    }
# 359 "kind.ag"
  else
# 360 "kind.ag"
    cout << "Not executing " << rsyncCmd << endl;
# 361 "kind.ag"
}
# 362 "kind.ag"

# 363 "kind.ag"
void backupVault(const string& vault,
# 364 "kind.ag"
                 KindConfig conf /*Copy!*/ ,
# 365 "kind.ag"
                 const DateTime& imageTime,
# 366 "kind.ag"
                 bool fullImage)
# 367 "kind.ag"
{
# 368 "kind.ag"
  if (!quiet)
# 369 "kind.ag"
    cout << DateTime::now().getString('h') << ": Backup of vault " << vault << endl;
# 370 "kind.ag"
  try
# 371 "kind.ag"
    {
# 372 "kind.ag"
      sizes[vault].second = 0; // nothing backed up yet
# 373 "kind.ag"

# 374 "kind.ag"
      readVaultConfig(vault, conf);
# 375 "kind.ag"

# 376 "kind.ag"
      // where to store
# 377 "kind.ag"
      string vaultpath = findVault(vault);
# 378 "kind.ag"

# 379 "kind.ag"
      // image path
# 380 "kind.ag"
      string imageName = getImageName(conf);
# 381 "kind.ag"
      if (!imageName.empty())
# 382 "kind.ag"
        imageName += '-';
# 383 "kind.ag"

# 384 "kind.ag"
      string imageFullName =  vaultpath + "/" + imageName ;
# 385 "kind.ag"

# 386 "kind.ag"
      if (conf.getBool("longImageName"))
# 387 "kind.ag"
        imageFullName += imageTime.getString('m');
# 388 "kind.ag"
      else
# 389 "kind.ag"
        imageFullName += imageTime.getString('s');
# 390 "kind.ag"

# 391 "kind.ag"
      bool backupNow = true;
# 392 "kind.ag"

# 393 "kind.ag"
      // existing images
# 394 "kind.ag"
      Images validImageList = findImages(vaultpath, conf, false);
# 395 "kind.ag"
      string currentSet = "expire";
# 396 "kind.ag"

# 397 "kind.ag"
      // check if we are using setRules
# 398 "kind.ag"

# 399 "kind.ag"
      map<string, pair<time_t, time_t> > ruleSet;
# 400 "kind.ag"
      map<string, string> backupSetRule;
# 401 "kind.ag"

# 402 "kind.ag"
      if (conf.hasKey("setRules"))
# 403 "kind.ag"
        {
# 404 "kind.ag"
          Strings setRules = conf.getStrings("setRules");
# 405 "kind.ag"
          if (!setRules.empty())
# 406 "kind.ag"
            {
# 407 "kind.ag"
              backupNow = false;
# 408 "kind.ag"
              for (const string& rule : setRules)
# 409 "kind.ag"
                {
# 410 "kind.ag"
                  Strings splittedRule;
# 411 "kind.ag"
                  split(rule, splittedRule, ':');
# 412 "kind.ag"
                  if (splittedRule.size() != 3)
# 413 "kind.ag"
                    throw Exception("config", "Error in setRule: " + rule);
# 414 "kind.ag"
                  string name = splittedRule[0];
# 415 "kind.ag"
                  if (name == "expire")
# 416 "kind.ag"
                    throw Exception("config", "Can't use reserved name expire in setRule");
# 417 "kind.ag"
                  backupSetRule[name] = rule;
# 418 "kind.ag"
                  time_t distance = stot(splittedRule[1]);
# 419 "kind.ag"
                  time_t keep = stot(splittedRule[2]);
# 420 "kind.ag"
                  ruleSet[name] = pair<time_t, time_t>(distance, keep);
# 421 "kind.ag"
                }
# 422 "kind.ag"

# 423 "kind.ag"
              // find time for nextBackup for every backupSet
# 424 "kind.ag"
              map<string, DateTime> nextBackup;
# 425 "kind.ag"

# 426 "kind.ag"
              // set default time for next Backup to now
# 427 "kind.ag"
              for (auto rule : ruleSet)
# 428 "kind.ag"
                nextBackup[rule.first] = imageTime;
# 429 "kind.ag"

# 430 "kind.ag"
              // find time for next backup
# 431 "kind.ag"
              //
# 432 "kind.ag"
              for (const Image& image : validImageList)
# 433 "kind.ag"
                {
# 434 "kind.ag"
                  if (image.series != "expire")
# 435 "kind.ag"
                    {
# 436 "kind.ag"
                      string s = image.series;
# 437 "kind.ag"
                      if (ruleSet.count(s) > 0) // rule for set exists?
# 438 "kind.ag"
                        {
# 439 "kind.ag"
                          if (nextBackup[s] < image.time + ruleSet[s].first)
# 440 "kind.ag"
                            nextBackup[s] = image.time + ruleSet[s].first;
# 441 "kind.ag"
                        }
# 442 "kind.ag"
                    }
# 443 "kind.ag"
                }
# 444 "kind.ag"

# 445 "kind.ag"
              // find backupSet that
# 446 "kind.ag"
              //      needs backup
# 447 "kind.ag"
              //      has longest time to keep
# 448 "kind.ag"
              currentSet = "";
# 449 "kind.ag"
              for (auto rule : ruleSet)
# 450 "kind.ag"
                {
# 451 "kind.ag"
                  string name = rule.first;
# 452 "kind.ag"
                  if (nextBackup[name] <= imageTime)
# 453 "kind.ag"
                    {
# 454 "kind.ag"
                      backupNow = true;
# 455 "kind.ag"
                      if (currentSet.empty())
# 456 "kind.ag"
                        currentSet = name;
# 457 "kind.ag"
                      else
# 458 "kind.ag"
                        if (ruleSet[name].second > ruleSet[currentSet].second)
# 459 "kind.ag"
                          currentSet = name;
# 460 "kind.ag"
                    }
# 461 "kind.ag"
                }
# 462 "kind.ag"
            }
# 463 "kind.ag"
        }
# 464 "kind.ag"

# 465 "kind.ag"
      verbosePrint("backup to \"" + imageFullName + "\"");
# 466 "kind.ag"
      verbosePrint("backup set is \"" + currentSet + "\"");
# 467 "kind.ag"

# 468 "kind.ag"
      if (backupNow)
# 469 "kind.ag"
        {
# 470 "kind.ag"
          // find reference image
# 471 "kind.ag"
          string referenceImage;
# 472 "kind.ag"
          if (!fullImage)
# 473 "kind.ag"
            {
# 474 "kind.ag"
              if (validImageList.empty())
# 475 "kind.ag"
                throw Exception("backupVault", "no reference image found");
# 476 "kind.ag"
              // last image is newest image
# 477 "kind.ag"
              referenceImage = validImageList.back().name;
# 478 "kind.ag"
            }
# 479 "kind.ag"

# 480 "kind.ag"
          doBackup(vault, imageFullName, referenceImage, conf);
# 481 "kind.ag"

# 482 "kind.ag"
          if (!dryRun)
# 483 "kind.ag"
            {
# 484 "kind.ag"
              string lastLink = vaultpath + "/last";
# 485 "kind.ag"
              unlink(lastLink.c_str());
# 486 "kind.ag"
              symlink(imageFullName.c_str(), lastLink.c_str());
# 487 "kind.ag"

# 488 "kind.ag"
              DateTime expireTime;
# 489 "kind.ag"
              string rule;
# 490 "kind.ag"
              if (currentSet == "expire")
# 491 "kind.ag"
                {
# 492 "kind.ag"
                  expireTime = getExpireDate(imageTime, conf, rule);
# 493 "kind.ag"
                }
# 494 "kind.ag"
              else
# 495 "kind.ag"
                {
# 496 "kind.ag"
                  expireTime = imageTime + ruleSet[currentSet].second;
# 497 "kind.ag"
                  rule = backupSetRule[currentSet];
# 498 "kind.ag"
                }
# 499 "kind.ag"

# 500 "kind.ag"
              ofstream expireFile(imageFullName + "/expires");
# 501 "kind.ag"
              expireFile << currentSet << "-" << expireTime.getString('m') << endl;
# 502 "kind.ag"
              expireFile << rule << endl;
# 503 "kind.ag"
            }
# 504 "kind.ag"
        }
# 505 "kind.ag"
      else if (!quiet)
# 506 "kind.ag"
        cout << "    no backup needed now" << endl;
# 507 "kind.ag"
    }
# 508 "kind.ag"
  catch (Exception ex)
# 509 "kind.ag"
    {
# 510 "kind.ag"
      cerr << "Exception in vault " << vault << ": " << ex.what() << endl;
# 511 "kind.ag"
    }
# 512 "kind.ag"
}
# 513 "kind.ag"

# 514 "kind.ag"
void expireVault(const string& vault, KindConfig conf, DateTime now)
# 515 "kind.ag"
{
# 516 "kind.ag"
  if (!quiet)
# 517 "kind.ag"
    cout << DateTime::now().getString('h') << ": Expiring images in vault " << vault << endl;
# 518 "kind.ag"

# 519 "kind.ag"
  readVaultConfig(vault, conf);
# 520 "kind.ag"

# 521 "kind.ag"
  string vaultpath = findVault(vault);
# 522 "kind.ag"

# 523 "kind.ag"
  Images imagelist = findImages(vaultpath, conf, true);
# 524 "kind.ag"

# 525 "kind.ag"
  string lastValidImage;
# 526 "kind.ag"
  for (Image image : imagelist)
# 527 "kind.ag"
    {
# 528 "kind.ag"
      if (image.valid)
# 529 "kind.ag"
        lastValidImage = image.name;
# 530 "kind.ag"
    }
# 531 "kind.ag"

# 532 "kind.ag"
  for (Image image : imagelist)
# 533 "kind.ag"
    {
# 534 "kind.ag"
      debugPrint(image.name);
# 535 "kind.ag"

# 536 "kind.ag"
      DateTime imageTime = imageDate(image.name);
# 537 "kind.ag"

# 538 "kind.ag"
      if (imageTime != now &&          // ignore just created image
# 539 "kind.ag"
      image.name != lastValidImage // ignore last valid image
# 540 "kind.ag"
         )
# 541 "kind.ag"
        {
# 542 "kind.ag"
          DateTime expireTime;
# 543 "kind.ag"
          string expireRule;
# 544 "kind.ag"
          if (!image.valid) // invalid image?
# 545 "kind.ag"
            {
# 546 "kind.ag"
              expireTime = imageTime + stot(conf.getString("expireFailedImage"));
# 547 "kind.ag"
              expireRule = "invalid image: " + conf.getString("expireFailedImage");
# 548 "kind.ag"
              debugPrint("- invalid image");
# 549 "kind.ag"
            }
# 550 "kind.ag"
          else
# 551 "kind.ag"
            {
# 552 "kind.ag"
              debugPrint("- valid image");
# 553 "kind.ag"
              expireTime = image.expire;
# 554 "kind.ag"
              expireRule = image.expireRule;
# 555 "kind.ag"
            }
# 556 "kind.ag"

# 557 "kind.ag"
          if (debug)
# 558 "kind.ag"
            {
# 559 "kind.ag"
              cout << "    image: " << imageTime.getString('h') << endl;
# 560 "kind.ag"
              cout << "      expire: " << expireTime.getString('h') << " " << expireRule << endl;
# 561 "kind.ag"
              cout << "      now: " << now.getString('h') << endl;
# 562 "kind.ag"
            }
# 563 "kind.ag"

# 564 "kind.ag"
          if (expireTime < now)
# 565 "kind.ag"
            {
# 566 "kind.ag"
              if (!quiet)
# 567 "kind.ag"
                cout << "  removing image " << image.name << endl;
# 568 "kind.ag"
              try
# 569 "kind.ag"
                {
# 570 "kind.ag"
                  if (removeDir(image.name) != 0)
# 571 "kind.ag"
                    cout << "Error removing " <<  image.name << endl;
# 572 "kind.ag"
                }
# 573 "kind.ag"
              catch (Exception ex)
# 574 "kind.ag"
                {
# 575 "kind.ag"
                  cerr << "Exception: " << ex.what() << endl;
# 576 "kind.ag"
                }
# 577 "kind.ag"
            }
# 578 "kind.ag"
        }
# 579 "kind.ag"
      else
# 580 "kind.ag"
        debugPrint("- current image - ignored");
# 581 "kind.ag"
    }
# 582 "kind.ag"
}
# 583 "kind.ag"

# 584 "kind.ag"
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
# 588 "kind.ag"

# 589 "kind.ag"
  int exitCode = 0;
# 590 "kind.ag"
  string lockFile;
# 591 "kind.ag"
  try
# 592 "kind.ag"
    {
# 593 "kind.ag"
      // handling of parameters and switches
# 594 "kind.ag"
      if (debug)        // debug implies verbose
# 595 "kind.ag"
        verbose = true;
# 596 "kind.ag"

# 597 "kind.ag"
      if (!doBackup && !doExpire && !listConfig)
# 598 "kind.ag"
        {
# 599 "kind.ag"
          doBackup = true;
# 600 "kind.ag"
          doExpire = true;
# 601 "kind.ag"
        }
# 602 "kind.ag"

# 603 "kind.ag"
      KindConfig conf;
# 604 "kind.ag"

# 605 "kind.ag"
      // default-values
# 606 "kind.ag"
      conf.add("imageName", "image");
# 607 "kind.ag"
      conf.add("vaultConfigName", "kind/vault.conf");
# 608 "kind.ag"
      conf.add("expireFailedImage", "3 days");
# 609 "kind.ag"
      conf.add("expireRule", "* * * * 1 month");
# 610 "kind.ag"
      conf.add("rsyncOption", ""); // no additional rsync option
# 611 "kind.ag"
      conf.add("remoteShell", "");
# 612 "kind.ag"
      conf.add("lockfile", "/var/lock/kind");
# 613 "kind.ag"
      conf.add("userExcludeFile", "nobackup.list");
# 614 "kind.ag"
      conf.add("userExcludeCommand",
# 615 "kind.ag"
               "find %path -type f -iname '*nobackup' -printf '%P\\\\n'");
# 616 "kind.ag"
      conf.add("logSize", "");
# 617 "kind.ag"

# 618 "kind.ag"
      if (listConfig)
# 619 "kind.ag"
        {
# 620 "kind.ag"
          cout << "builtin config" << endl;
# 621 "kind.ag"
          conf.print(".   ");
# 622 "kind.ag"
        }
# 623 "kind.ag"

# 624 "kind.ag"
      readMasterConfig(masterConfig, conf);
# 625 "kind.ag"

# 626 "kind.ag"
      banks = conf.getStrings("bank");
# 627 "kind.ag"
      if (banks.empty())
# 628 "kind.ag"
        throw Exception("read master configuration", "no banks defined");
# 629 "kind.ag"

# 630 "kind.ag"
      if (listConfig)
# 631 "kind.ag"
        {
# 632 "kind.ag"
          cout << "global config:" << endl;
# 633 "kind.ag"
          conf.print(".   ");
# 634 "kind.ag"
          readVaultConfig(vault, conf);
# 635 "kind.ag"
          cout << "vault config:" << endl;
# 636 "kind.ag"
          conf.print(".   ");
# 637 "kind.ag"
          exit(0);
# 638 "kind.ag"
        }
# 639 "kind.ag"

# 640 "kind.ag"
      lockFile = conf.getString("lockfile");
# 641 "kind.ag"
      createLock(lockFile);
# 642 "kind.ag"

# 643 "kind.ag"
      DateTime imageTime = DateTime::now();
# 644 "kind.ag"

# 645 "kind.ag"
      string logSizeFile = conf.getString("logSize");
# 646 "kind.ag"
      if (!logSizeFile.empty())
# 647 "kind.ag"
        readSizes(logSizeFile);
# 648 "kind.ag"

# 649 "kind.ag"
      vector<string> vaults;
# 650 "kind.ag"
      string groupname = "group_" + vault;
# 651 "kind.ag"
      if (conf.hasKey(groupname))
# 652 "kind.ag"
        vaults = conf.getStrings(groupname);
# 653 "kind.ag"
      else
# 654 "kind.ag"
        vaults.push_back(vault);
# 655 "kind.ag"

# 656 "kind.ag"
      if (doBackup)
# 657 "kind.ag"
        for (string vault : vaults)
# 658 "kind.ag"
          {
# 659 "kind.ag"
            backupVault(vault, conf, imageTime, fullImage);
# 660 "kind.ag"
            writeSizes(logSizeFile);
# 661 "kind.ag"
          }
# 662 "kind.ag"

# 663 "kind.ag"
      if (doExpire)
# 664 "kind.ag"
        for (unsigned int i = 0; i < vaults.size(); ++i)
# 665 "kind.ag"
          expireVault(vaults[i], conf, imageTime);
# 666 "kind.ag"

# 667 "kind.ag"
      if (!quiet)
# 668 "kind.ag"
        cout << DateTime::now().getString('h') << ": finished" << endl;
# 669 "kind.ag"

# 670 "kind.ag"
    }
# 671 "kind.ag"
  catch (const Exception& ex)
# 672 "kind.ag"
    {
# 673 "kind.ag"
      cerr << "Exception: " << ex.what() << endl;
# 674 "kind.ag"
      exitCode = 1;
# 675 "kind.ag"
    }
# 676 "kind.ag"
  catch (const char* msg)
# 677 "kind.ag"
    {
# 678 "kind.ag"
      cerr << "Exception(char*): " << msg << endl;
# 679 "kind.ag"
      exitCode = 1;
# 680 "kind.ag"
    }
# 681 "kind.ag"
  catch (const string& msg)
# 682 "kind.ag"
    {
# 683 "kind.ag"
      cerr << "Exception(string): " << msg << endl;
# 684 "kind.ag"
      exitCode = 1;
# 685 "kind.ag"
    }
# 686 "kind.ag"
  removeLock(lockFile);
# 687 "kind.ag"
  return exitCode;
# 688 "kind.ag"
}
