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
#include "kind.h"
# 29 "kind.ag"

# 30 "kind.ag"
#include "expiretools.h"
# 31 "kind.ag"
#include "excludetools.h"
# 32 "kind.ag"

# 33 "kind.ag"
/*AppGen
# 34 "kind.ag"
  %%  Beschreibung des Programmes:
# 35 "kind.ag"
  prog: archiving backup
# 36 "kind.ag"
  %% Beschreibung Parameter
# 37 "kind.ag"
  % symbolischerName, Art, Typ,   Variablenname, Erklärung, Default-Wert
# 38 "kind.ag"
  para: vault_or_group, required, string, vault, Vault to backup
# 39 "kind.ag"
  %% Beschreibung der Optionen
# 40 "kind.ag"
  % kurz-Option, lang-Option, Typ, Variablenname, Erklärung, Default-Wert
# 41 "kind.ag"
  opt: c, masterconfig, string, masterConfig, Master config file, ""
# 42 "kind.ag"
  opt2: if not given or empty kind looks for
# 43 "kind.ag"
  opt2:   /etc/kind/master.conf
# 44 "kind.ag"
  opt2:   /ffp/etc/kind/master.conf
# 45 "kind.ag"
  opt: f, full, void, fullImage, Force full image == initial backup, false
# 46 "kind.ag"
  opt: B, backup, void, doBackup, Backup, false
# 47 "kind.ag"
  opt: E, expire, void, doExpire, Expire, false
# 48 "kind.ag"
  opt: C, listconfig, void, listConfig, Show configuration, false
# 49 "kind.ag"
  opt: I, listimages, void, listImages, List data of images, false
# 50 "kind.ag"
  opt2:   if none of backup, expire, listconfig and listimages is specified,
# 51 "kind.ag"
  opt2:   backup and expire is assumed.
# 52 "kind.ag"
  opt2:   listconfig and listimages cannot be combined with other actions
# 53 "kind.ag"
  opt: D, dryrun, Void, dryRun, Dry run (no real backup), false
# 54 "kind.ag"
  opt: F, forcebackup, string, forcedBackupSet, Create image for specified backup set, ""
# 55 "kind.ag"
  opt: v, verbose, Void, verbose,  Verbose,  false
# 56 "kind.ag"
  opt: d, debug, Void, debug, Debug output of many data, false
# 57 "kind.ag"
  opt: q, quiet, Void, quiet, Be quiet - no messages, false
# 58 "kind.ag"
  opt: h, help, usage, ignored , This help
# 59 "kind.ag"
AppGen*/
# 60 "kind.ag"

# 61 "kind.ag"
using namespace std;
# 62 "kind.ag"

# 63 "kind.ag"
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
# 64 "kind.ag"

# 65 "kind.ag"
Strings banks;
# 66 "kind.ag"

# 67 "kind.ag"
string findVault(const string& v);
# 68 "kind.ag"

# 69 "kind.ag"
typedef pair<long int, long int> Sizes;
# 70 "kind.ag"
map<string, Sizes> sizes;
# 71 "kind.ag"

# 72 "kind.ag"
void readSizes(const string& logSizeFile)
# 73 "kind.ag"
{
# 74 "kind.ag"
  if (!logSizeFile.empty() && fileExists(logSizeFile))
# 75 "kind.ag"
    {
# 76 "kind.ag"
      vector<string> ss;
# 77 "kind.ag"
      file2Strings(logSizeFile, ss);
# 78 "kind.ag"
      for (const string& s : ss)
# 79 "kind.ag"
        {
# 80 "kind.ag"
          unsigned int i = 0;
# 81 "kind.ag"
          string vault = getWord(s, i);
# 82 "kind.ag"
          long int s1 = getLongInt(s, i);
# 83 "kind.ag"
          long int s2 = getLongInt(s, i);
# 84 "kind.ag"
          try
# 85 "kind.ag"
            {
# 86 "kind.ag"
              findVault(vault);
# 87 "kind.ag"
              sizes[vault] = Sizes(s1, s2);
# 88 "kind.ag"
            }
# 89 "kind.ag"
          catch (...)
# 90 "kind.ag"
            {
# 91 "kind.ag"
              // ignore missing vaults
# 92 "kind.ag"
            }
# 93 "kind.ag"
        }
# 94 "kind.ag"
    }
# 95 "kind.ag"
}
# 96 "kind.ag"

# 97 "kind.ag"
void writeSizes(const string logSizeFile)
# 98 "kind.ag"
{
# 99 "kind.ag"
  if (!logSizeFile.empty())
# 100 "kind.ag"
    {
# 101 "kind.ag"
      Strings st;
# 102 "kind.ag"
      for (auto s : sizes)
# 103 "kind.ag"
        {
# 104 "kind.ag"
          string h = s.first + " " + to_string(s.second.first) + " " + to_string(s.second.second);
# 105 "kind.ag"
          st.push_back(h);
# 106 "kind.ag"
        }
# 107 "kind.ag"
      strings2File(st, logSizeFile);
# 108 "kind.ag"
    }
# 109 "kind.ag"
}
# 110 "kind.ag"

# 111 "kind.ag"
void verbosePrint(const string& text)
# 112 "kind.ag"
{
# 113 "kind.ag"
  if (verbose)
# 114 "kind.ag"
    cout << "  " << text << endl;
# 115 "kind.ag"
}
# 116 "kind.ag"

# 117 "kind.ag"
void debugPrint(const string& text)
# 118 "kind.ag"
{
# 119 "kind.ag"
  if (debug)
# 120 "kind.ag"
    cout << "    " << text << endl;
# 121 "kind.ag"
}
# 122 "kind.ag"

# 123 "kind.ag"
void readMasterConfig1(const string& fn, KindConfig& conf)
# 124 "kind.ag"
{
# 125 "kind.ag"
  verbosePrint("reading master config " + fn);
# 126 "kind.ag"
  conf.addFile(fn);
# 127 "kind.ag"
}
# 128 "kind.ag"

# 129 "kind.ag"
void readMasterConfig(const string& fn, KindConfig& conf)
# 130 "kind.ag"
{
# 131 "kind.ag"
  if (!fn.empty())  // master config given by user on commandline
# 132 "kind.ag"
    readMasterConfig1(fn, conf);
# 133 "kind.ag"
  else if (fileExists("/etc/kind/master.conf"))
# 134 "kind.ag"
    readMasterConfig1("/etc/kind/master.conf", conf);
# 135 "kind.ag"
  else if (fileExists("/ffp/etc/kind/master.conf"))
# 136 "kind.ag"
    readMasterConfig1("/ffp/etc/kind/master.conf", conf);
# 137 "kind.ag"
  else
# 138 "kind.ag"
    throw Exception("MasterConfig", "no file");
# 139 "kind.ag"
}
# 140 "kind.ag"

# 141 "kind.ag"
string findVault(const string& v)
# 142 "kind.ag"
{
# 143 "kind.ag"
  bool found = false;
# 144 "kind.ag"
  FileName fn;
# 145 "kind.ag"
  fn.setName(v);
# 146 "kind.ag"
  for (unsigned int i = 0; !found && i < banks.size(); ++i)
# 147 "kind.ag"
    {
# 148 "kind.ag"
      fn.setPath(banks[i]);
# 149 "kind.ag"
      if (dirExists(fn.getFileName()))
# 150 "kind.ag"
        found = true;
# 151 "kind.ag"
    }
# 152 "kind.ag"
  if (!found)
# 153 "kind.ag"
    throw Exception("find vault", v + " not found");
# 154 "kind.ag"
  verbosePrint("using vault " + fn.getFileName());
# 155 "kind.ag"
  return fn.getFileName();
# 156 "kind.ag"
}
# 157 "kind.ag"

# 158 "kind.ag"
void readVaultConfig(const string& vault, KindConfig& conf)
# 159 "kind.ag"
{
# 160 "kind.ag"
  string vaultpath = findVault(vault);
# 161 "kind.ag"
  const string& vaultConfigName = vaultpath + '/' + conf.getString("vaultConfigName");
# 162 "kind.ag"
  verbosePrint("reading vault config:");
# 163 "kind.ag"
  verbosePrint("  " + vaultConfigName);
# 164 "kind.ag"
  conf.addFile(vaultConfigName);
# 165 "kind.ag"
}
# 166 "kind.ag"

# 167 "kind.ag"
string getImageName(const KindConfig& conf,
# 168 "kind.ag"
                    const string& vaultPath,
# 169 "kind.ag"
                    const DateTime& imageTime)
# 170 "kind.ag"
{
# 171 "kind.ag"
  bool nonPortable = false;
# 172 "kind.ag"
  string imageName = conf.getString("imageName");
# 173 "kind.ag"
  for (unsigned int i = 0; !nonPortable && i < imageName.size(); ++i)
# 174 "kind.ag"
    {
# 175 "kind.ag"
      char c = imageName[i];
# 176 "kind.ag"
      if (!isalnum(c) && c != '.' && c != '_')
# 177 "kind.ag"
        nonPortable = true;
# 178 "kind.ag"
    }
# 179 "kind.ag"
  if (nonPortable)
# 180 "kind.ag"
    throw Exception("getImageName", "Invalid character in image name " + imageName);
# 181 "kind.ag"

# 182 "kind.ag"
  if (!imageName.empty())
# 183 "kind.ag"
    imageName += '-';
# 184 "kind.ag"

# 185 "kind.ag"
  string imageFullName =  vaultPath + "/" + imageName ;
# 186 "kind.ag"

# 187 "kind.ag"
  if (conf.getBool("longImageName"))
# 188 "kind.ag"
    imageFullName += imageTime.getString('m');
# 189 "kind.ag"
  else
# 190 "kind.ag"
    imageFullName += imageTime.getString('s');
# 191 "kind.ag"

# 192 "kind.ag"
  return imageFullName;
# 193 "kind.ag"
}
# 194 "kind.ag"

# 195 "kind.ag"
Images findImages(const string& vaultpath, const KindConfig& conf, bool all)
# 196 "kind.ag"
{
# 197 "kind.ag"
  Strings dirs;
# 198 "kind.ag"
  debugPrint("searching images in " + vaultpath);
# 199 "kind.ag"
  dirList(vaultpath, dirs);
# 200 "kind.ag"

# 201 "kind.ag"
  Images imageList;
# 202 "kind.ag"
  for (string dir : dirs)
# 203 "kind.ag"
    {
# 204 "kind.ag"
      FileName fn(dir);
# 205 "kind.ag"
      string imgname = conf.getString("imageName");
# 206 "kind.ag"
      if (startsWith(fn.getName(), imgname))
# 207 "kind.ag"
        {
# 208 "kind.ag"
          debugPrint("Checking " + dir);
# 209 "kind.ag"
          Image image(dir);
# 210 "kind.ag"

# 211 "kind.ag"
          if (all || image.valid)
# 212 "kind.ag"
            imageList.push_back(image);
# 213 "kind.ag"
        }
# 214 "kind.ag"
    }
# 215 "kind.ag"
  if (imageList.size() > 1)
# 216 "kind.ag"
    sort(imageList.begin(), imageList.end());
# 217 "kind.ag"
  return imageList;
# 218 "kind.ag"
}
# 219 "kind.ag"

# 220 "kind.ag"
void listImageInfo(const string& vault,
# 221 "kind.ag"
                   KindConfig conf /*Copy!*/ ,
# 222 "kind.ag"
                   const DateTime& imageTime,
# 223 "kind.ag"
                   const string& backupSet)
# 224 "kind.ag"
{
# 225 "kind.ag"
  readVaultConfig(vault, conf);
# 226 "kind.ag"
  string vaultPath = findVault(vault);
# 227 "kind.ag"
  Images imageList = findImages(vaultPath, conf, true);
# 228 "kind.ag"
  cout << "== " << vault << " ==" << endl;
# 229 "kind.ag"
  for (auto img : imageList)
# 230 "kind.ag"
    {
# 231 "kind.ag"
      if (img.series == backupSet || backupSet.empty())
# 232 "kind.ag"
        {
# 233 "kind.ag"
          img.printInfo();
# 234 "kind.ag"
          cout << "---" << endl;
# 235 "kind.ag"
        }
# 236 "kind.ag"
    }
# 237 "kind.ag"
}
# 238 "kind.ag"

# 239 "kind.ag"
void doBackup(const string& vault,
# 240 "kind.ag"
              const string& imageFullName,
# 241 "kind.ag"
              const string& referenceImage,
# 242 "kind.ag"
              const KindConfig& conf)
# 243 "kind.ag"
{
# 244 "kind.ag"
  // create image path
# 245 "kind.ag"

# 246 "kind.ag"
  bool shellMode = true;
# 247 "kind.ag"

# 248 "kind.ag"
  // create source descriptor
# 249 "kind.ag"
  string host;
# 250 "kind.ag"
  if (conf.hasKey("host"))
# 251 "kind.ag"
    host = conf.getString("host");
# 252 "kind.ag"

# 253 "kind.ag"
  string server;
# 254 "kind.ag"
  if (conf.hasKey("server"))
# 255 "kind.ag"
    {
# 256 "kind.ag"
      server = conf.getString("server");
# 257 "kind.ag"
      shellMode = false;
# 258 "kind.ag"
    }
# 259 "kind.ag"

# 260 "kind.ag"
  if (!host.empty() && !server.empty())
# 261 "kind.ag"
    throw Exception("backupVault", "Cannot have host and server");
# 262 "kind.ag"

# 263 "kind.ag"
  if (host.empty() && server.empty())
# 264 "kind.ag"
    throw Exception("backupVault", "No host or server specified");
# 265 "kind.ag"

# 266 "kind.ag"
  // ping host / server
# 267 "kind.ag"
  // ping -c 1 -W 5 -q $HOST
# 268 "kind.ag"
  string pingCommand = conf.getString("ping");
# 269 "kind.ag"
  debugPrint("PingCommand: " + pingCommand);
# 270 "kind.ag"
  if (!pingCommand.empty())
# 271 "kind.ag"
    {
# 272 "kind.ag"
      if (!host.empty())
# 273 "kind.ag"
        replacePlaceHolder(pingCommand, "%host", host);
# 274 "kind.ag"
      else
# 275 "kind.ag"
        replacePlaceHolder(pingCommand, "%host", server);
# 276 "kind.ag"
      int rc = 0;
# 277 "kind.ag"
      Strings pingResult = localExec(pingCommand, rc, debug);
# 278 "kind.ag"
      if (rc != 0)
# 279 "kind.ag"
        throw Exception("Host not available", pingCommand);
# 280 "kind.ag"
    }
# 281 "kind.ag"

# 282 "kind.ag"
  string path = conf.getString("path");
# 283 "kind.ag"
  if (path.empty())
# 284 "kind.ag"
    throw Exception("rsync", "empty source path");
# 285 "kind.ag"
  if (path.back() != '/')
# 286 "kind.ag"
    path += '/';
# 287 "kind.ag"

# 288 "kind.ag"
  string rsyncCmd = "rsync -vrltH --delete --stats -D --numeric-ids ";
# 289 "kind.ag"
  if (!conf.getBool("ignorePermission"))
# 290 "kind.ag"
    rsyncCmd += "-pgo";
# 291 "kind.ag"
  vector<string> rso = conf.getStrings("rsyncOption");
# 292 "kind.ag"
  for (const string& opt : rso)
# 293 "kind.ag"
    rsyncCmd += opt + " ";
# 294 "kind.ag"

# 295 "kind.ag"
  // excludes
# 296 "kind.ag"
  Strings excluded = getExclusions(conf, shellMode);
# 297 "kind.ag"

# 298 "kind.ag"
  // create image path
# 299 "kind.ag"
  if (!dryRun)
# 300 "kind.ag"
    if (mkdir(imageFullName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
# 301 "kind.ag"
      throw Exception("Create image", "failed to create " + imageFullName);
# 302 "kind.ag"

# 303 "kind.ag"
  // error message
# 304 "kind.ag"
  // we write an generic error message to mark backup as unsuccessful
# 305 "kind.ag"
  // will be deleted at successful end of rsync
# 306 "kind.ag"
  string errorfile = imageFullName + "/error";
# 307 "kind.ag"
  if (!dryRun)
# 308 "kind.ag"
    {
# 309 "kind.ag"
      ofstream error(errorfile);
# 310 "kind.ag"
      error << "failed" << endl;
# 311 "kind.ag"
      error.close();
# 312 "kind.ag"
    }
# 313 "kind.ag"

# 314 "kind.ag"
  if (shellMode)  // shell mode
# 315 "kind.ag"
    {
# 316 "kind.ag"
      // cout << "USING SHELLMODE '" << host << "'" << endl;
# 317 "kind.ag"
      string remoteShell = conf.getString("remoteShell");
# 318 "kind.ag"
      string userAtHost = conf.getString("user") + "@" + conf.getString("host");
# 319 "kind.ag"
      string rshCommand = remoteShell;
# 320 "kind.ag"
      if (remoteShell.empty())
# 321 "kind.ag"
        rshCommand = "ssh";
# 322 "kind.ag"

# 323 "kind.ag"
      rshCommand += " " + userAtHost;
# 324 "kind.ag"

# 325 "kind.ag"
      if (!dryRun)
# 326 "kind.ag"
        strings2File(excluded, imageFullName + "/exclude");
# 327 "kind.ag"

# 328 "kind.ag"
      // rsync image
# 329 "kind.ag"

# 330 "kind.ag"
      if (!remoteShell.empty())
# 331 "kind.ag"
        rsyncCmd += " -e \'" + remoteShell + "\' ";
# 332 "kind.ag"

# 333 "kind.ag"
      rsyncCmd += "--exclude-from=" + imageFullName + "/exclude ";
# 334 "kind.ag"
      if (!referenceImage.empty())
# 335 "kind.ag"
        rsyncCmd += "--link-dest=" + referenceImage + "/tree ";
# 336 "kind.ag"
      rsyncCmd += userAtHost + ":" + path + " ";
# 337 "kind.ag"
      rsyncCmd += imageFullName + "/tree";
# 338 "kind.ag"
    } // shell mode
# 339 "kind.ag"
  else
# 340 "kind.ag"
    {
# 341 "kind.ag"
      // cout << "USING SERVERMODE" << endl;
# 342 "kind.ag"
      // we cannot use find without shell access
# 343 "kind.ag"
      // and do not read an exclude file on client side
# 344 "kind.ag"

# 345 "kind.ag"
      if (!dryRun)
# 346 "kind.ag"
        strings2File(excluded, imageFullName + "/exclude");
# 347 "kind.ag"

# 348 "kind.ag"
      rsyncCmd += "--exclude-from=" + imageFullName + "/exclude ";
# 349 "kind.ag"
      if (!referenceImage.empty())
# 350 "kind.ag"
        rsyncCmd += "--link-dest=" + referenceImage + "/tree ";
# 351 "kind.ag"
      rsyncCmd += conf.getString("server") + "::" + path + " ";
# 352 "kind.ag"
      rsyncCmd += imageFullName + "/tree";
# 353 "kind.ag"
    }
# 354 "kind.ag"

# 355 "kind.ag"
  debugPrint("Action: " + rsyncCmd);
# 356 "kind.ag"

# 357 "kind.ag"
  vector<string> backupResult;
# 358 "kind.ag"
  if (!dryRun)
# 359 "kind.ag"
    {
# 360 "kind.ag"
      verbosePrint("syncing (" + rsyncCmd + ")");
# 361 "kind.ag"
      int rc;
# 362 "kind.ag"
      backupResult = localExec(rsyncCmd, rc, debug, imageFullName + "/rsync-log");
# 363 "kind.ag"
      if (rc == 0 ||
# 364 "kind.ag"
      rc == 24 || // "no error" or "vanished source files" (ignored)
# 365 "kind.ag"
      rc == 6144) // workaround for wrong exit code ??!!
# 366 "kind.ag"
        {
# 367 "kind.ag"
          unlink(errorfile.c_str());
# 368 "kind.ag"
          long int st = 0;
# 369 "kind.ag"
          long int sc = 0;
# 370 "kind.ag"
          for (auto bl : backupResult)
# 371 "kind.ag"
            {
# 372 "kind.ag"
              if (startsWith(bl, "Total file size"))
# 373 "kind.ag"
                st = getNumber(bl);
# 374 "kind.ag"
              else if (startsWith(bl, "Total transferred file size"))
# 375 "kind.ag"
                sc = getNumber(bl);
# 376 "kind.ag"
            }
# 377 "kind.ag"
          // sizes[vault] = pair<long int, long int>(st, sc);
# 378 "kind.ag"
          sizes[vault] = Sizes(st, sc);
# 379 "kind.ag"
          //  cout << vault << " " << st << " || " << sc << endl;
# 380 "kind.ag"
        }
# 381 "kind.ag"
      else
# 382 "kind.ag"
        throw Exception("Backup", "Failed to execute rsync (result: " + to_string(rc) + ")");
# 383 "kind.ag"
    }
# 384 "kind.ag"
  else
# 385 "kind.ag"
    cout << "Not executing " << rsyncCmd << endl;
# 386 "kind.ag"
}
# 387 "kind.ag"

# 388 "kind.ag"
bool backupVault(const string& vault,
# 389 "kind.ag"
                 KindConfig conf /*Copy!*/ ,
# 390 "kind.ag"
                 const DateTime& imageTime,
# 391 "kind.ag"
                 bool fullImage,
# 392 "kind.ag"
                 const string& forcedBackupSet)
# 393 "kind.ag"
{
# 394 "kind.ag"
  if (!quiet)
# 395 "kind.ag"
    cout << DateTime::now().getString('h') << ": Backup of vault " << vault << endl;
# 396 "kind.ag"
  try
# 397 "kind.ag"
    {
# 398 "kind.ag"
      readVaultConfig(vault, conf);
# 399 "kind.ag"

# 400 "kind.ag"
      // where to store
# 401 "kind.ag"
      string vaultPath = findVault(vault);
# 402 "kind.ag"

# 403 "kind.ag"
      // image path
# 404 "kind.ag"
      string imageFullName = getImageName(conf, vaultPath, imageTime);
# 405 "kind.ag"

# 406 "kind.ag"
      bool backupNow = true;
# 407 "kind.ag"

# 408 "kind.ag"
      // existing images
# 409 "kind.ag"
      Images validImageList = findImages(vaultPath, conf, false);
# 410 "kind.ag"
      string currentSet = "expire"; // we are not using backupSets
# 411 "kind.ag"

# 412 "kind.ag"
      // check if we are using backup sets
# 413 "kind.ag"

# 414 "kind.ag"
      map<string, int> setIdx;
# 415 "kind.ag"
      vector<SetRule> backupSetRule;
# 416 "kind.ag"
      int setRuleIdx = -1;
# 417 "kind.ag"

# 418 "kind.ag"
      if (conf.hasKey("setRule"))
# 419 "kind.ag"
        {
# 420 "kind.ag"
          readSetRules(conf, setIdx, backupSetRule);
# 421 "kind.ag"
          if (!setIdx.empty())
# 422 "kind.ag"
            {
# 423 "kind.ag"
              if (forcedBackupSet.empty())
# 424 "kind.ag"
                {
# 425 "kind.ag"
                  backupNow = false;
# 426 "kind.ag"

# 427 "kind.ag"
                  // find time for nextBackup for every backupSet
# 428 "kind.ag"
                  // defaults to now == imageTime;
# 429 "kind.ag"
                  vector<DateTime> nextBackup(backupSetRule.size(), imageTime);
# 430 "kind.ag"

# 431 "kind.ag"
                  // find time for next backup
# 432 "kind.ag"

# 433 "kind.ag"
                  for (const Image& image : validImageList)
# 434 "kind.ag"
                    {
# 435 "kind.ag"
                      if (image.series != "expire")
# 436 "kind.ag"
                        {
# 437 "kind.ag"
                          string s = image.series;
# 438 "kind.ag"
                          if (setIdx.count(s) > 0) // rule for set exists?
# 439 "kind.ag"
                            {
# 440 "kind.ag"
                              int rIdx = setIdx[s];
# 441 "kind.ag"
                              // image is valid for this and "lower level" backupSets
# 442 "kind.ag"
                              for (unsigned int i = rIdx; i < backupSetRule.size(); ++i)
# 443 "kind.ag"
                                if (nextBackup[i] < image.time + backupSetRule[i].distance)
# 444 "kind.ag"
                                  nextBackup[i] =  image.time + backupSetRule[i].distance;
# 445 "kind.ag"
                            }
# 446 "kind.ag"
                        }
# 447 "kind.ag"
                    }
# 448 "kind.ag"
                  if (debug)
# 449 "kind.ag"
                    for (unsigned int i = 0; i < backupSetRule.size(); ++i)
# 450 "kind.ag"
                      cout << "       Next backup for " << backupSetRule[i].name << " at " <<  nextBackup[i].getString('h') << endl;
# 451 "kind.ag"

# 452 "kind.ag"
                  // find backupSet that
# 453 "kind.ag"
                  //    - needs backup
# 454 "kind.ag"
                  //    - has longest time to keep
# 455 "kind.ag"
                  // because of ordered list backupSetRule this is the first set, that need
# 456 "kind.ag"

# 457 "kind.ag"
                  currentSet = "";
# 458 "kind.ag"
                  for (unsigned int i = 0; i < backupSetRule.size() && currentSet.empty(); ++i)
# 459 "kind.ag"
                    {
# 460 "kind.ag"
                      string name = backupSetRule[i].name;
# 461 "kind.ag"
                      if (nextBackup[i] <= imageTime + 5) // small offset of 5s for "jitter"
# 462 "kind.ag"
                        {
# 463 "kind.ag"
                          backupNow = true;
# 464 "kind.ag"
                          currentSet = name;
# 465 "kind.ag"
                          setRuleIdx = i;
# 466 "kind.ag"
                        }
# 467 "kind.ag"
                    }
# 468 "kind.ag"
                }
# 469 "kind.ag"
              else
# 470 "kind.ag"
                {
# 471 "kind.ag"
                  if (setIdx.count(forcedBackupSet) > 0)
# 472 "kind.ag"
                    {
# 473 "kind.ag"
                      currentSet = forcedBackupSet;
# 474 "kind.ag"
                      setRuleIdx = setIdx[forcedBackupSet];
# 475 "kind.ag"
                    }
# 476 "kind.ag"
                  else
# 477 "kind.ag"
                    throw Exception("force backup of set " + forcedBackupSet, " set not exists");
# 478 "kind.ag"
                }
# 479 "kind.ag"
            } // if (!setIdx.empty())
# 480 "kind.ag"
        } // (conf.hasKey("setRule"))
# 481 "kind.ag"

# 482 "kind.ag"
      if (backupNow)
# 483 "kind.ag"
        {
# 484 "kind.ag"
          verbosePrint("backup to \"" + imageFullName + "\"");
# 485 "kind.ag"
          if (setRuleIdx >= 0 && !quiet)
# 486 "kind.ag"
            cout << "  backup set is \"" << currentSet << "\"" << endl;
# 487 "kind.ag"
        }
# 488 "kind.ag"
      else if (!quiet)
# 489 "kind.ag"
        cout << "  no backup set needs update" << endl;
# 490 "kind.ag"

# 491 "kind.ag"
      if (backupNow)
# 492 "kind.ag"
        {
# 493 "kind.ag"
          // find reference image
# 494 "kind.ag"
          string referenceImage;
# 495 "kind.ag"
          if (!fullImage)
# 496 "kind.ag"
            {
# 497 "kind.ag"
              if (validImageList.empty())
# 498 "kind.ag"
                throw Exception("backupVault", "no reference image found");
# 499 "kind.ag"
              // last image is newest image
# 500 "kind.ag"
              referenceImage = validImageList.back().name;
# 501 "kind.ag"
            }
# 502 "kind.ag"

# 503 "kind.ag"
          doBackup(vault, imageFullName, referenceImage, conf);
# 504 "kind.ag"

# 505 "kind.ag"
          if (!dryRun)
# 506 "kind.ag"
            {
# 507 "kind.ag"
              string lastPath = vaultPath + "/last";
# 508 "kind.ag"
              struct stat fstat;
# 509 "kind.ag"

# 510 "kind.ag"
              // remove last (dir or symlink)
# 511 "kind.ag"
              if (lstat(lastPath.c_str(), &fstat) == 0) // last exists
# 512 "kind.ag"
                {
# 513 "kind.ag"
                  if (S_ISDIR(fstat.st_mode))
# 514 "kind.ag"
                    removeDir(lastPath);
# 515 "kind.ag"
                  else
# 516 "kind.ag"
                    unlink(lastPath.c_str());
# 517 "kind.ag"
                }
# 518 "kind.ag"

# 519 "kind.ag"
              string linkType = conf.getString("lastLink");
# 520 "kind.ag"
              if (linkType == "hardLink")
# 521 "kind.ag"
                {
# 522 "kind.ag"
                  int rc;
# 523 "kind.ag"
                  string hardLinkCommand = "cp -al " + imageFullName + " " + lastPath;
# 524 "kind.ag"
                  Strings res = localExec(hardLinkCommand, rc, debug);
# 525 "kind.ag"
                }
# 526 "kind.ag"
              else if (linkType == "symLink")
# 527 "kind.ag"
                {
# 528 "kind.ag"
                  // set symlink to last image
# 529 "kind.ag"
                  symlink(imageFullName.c_str(), lastPath.c_str());
# 530 "kind.ag"
                }
# 531 "kind.ag"
              else if (linkType != "null")
# 532 "kind.ag"
                cerr << "invalid Value in \"lastLink\"" << endl;
# 533 "kind.ag"

# 534 "kind.ag"
              // write expire date to file
# 535 "kind.ag"
              DateTime expireTime;
# 536 "kind.ag"
              string rule;
# 537 "kind.ag"
              if (setRuleIdx < 0) // not backup set based
# 538 "kind.ag"
                expireTime = getExpireDate(imageTime, conf, rule);
# 539 "kind.ag"
              else
# 540 "kind.ag"
                {
# 541 "kind.ag"
                  expireTime = imageTime + backupSetRule[setRuleIdx].keep;
# 542 "kind.ag"
                  rule =  backupSetRule[setRuleIdx].rule;
# 543 "kind.ag"
                }
# 544 "kind.ag"

# 545 "kind.ag"
              ofstream expireFile(imageFullName + "/expires");
# 546 "kind.ag"
              expireFile << currentSet << "-" << expireTime.getString('m') << endl;
# 547 "kind.ag"
              expireFile << rule << endl;
# 548 "kind.ag"
            }
# 549 "kind.ag"
        }
# 550 "kind.ag"
      return backupNow;
# 551 "kind.ag"
    }
# 552 "kind.ag"
  catch (Exception ex)
# 553 "kind.ag"
    {
# 554 "kind.ag"
      cerr << "Exception in vault " << vault << ": " << ex.what() << endl;
# 555 "kind.ag"
      return false;
# 556 "kind.ag"
    }
# 557 "kind.ag"
}
# 558 "kind.ag"

# 559 "kind.ag"
void expireVault(const string& vault, KindConfig conf, DateTime now)
# 560 "kind.ag"
{
# 561 "kind.ag"
  if (!quiet)
# 562 "kind.ag"
    cout << DateTime::now().getString('h') << ": Expiring images in vault " << vault << endl;
# 563 "kind.ag"

# 564 "kind.ag"
  readVaultConfig(vault, conf);
# 565 "kind.ag"

# 566 "kind.ag"
  string vaultpath = findVault(vault);
# 567 "kind.ag"

# 568 "kind.ag"
  Images imagelist = findImages(vaultpath, conf, true);
# 569 "kind.ag"

# 570 "kind.ag"
  string lastValidImage;
# 571 "kind.ag"
  for (Image image : imagelist)
# 572 "kind.ag"
    {
# 573 "kind.ag"
      if (image.valid)
# 574 "kind.ag"
        lastValidImage = image.name;
# 575 "kind.ag"
    }
# 576 "kind.ag"

# 577 "kind.ag"
  for (Image image : imagelist)
# 578 "kind.ag"
    {
# 579 "kind.ag"
      if (debug)
# 580 "kind.ag"
        image.printInfo();
# 581 "kind.ag"

# 582 "kind.ag"
      DateTime imageTime = image.time;
# 583 "kind.ag"

# 584 "kind.ag"
      if (imageTime != now &&          // ignore just created image
# 585 "kind.ag"
      image.name != lastValidImage // ignore last valid image
# 586 "kind.ag"
         )
# 587 "kind.ag"
        {
# 588 "kind.ag"
          DateTime expireTime;
# 589 "kind.ag"
          string expireRule;
# 590 "kind.ag"
          if (!image.valid) // invalid image?
# 591 "kind.ag"
            {
# 592 "kind.ag"
              time_t expPeriod = stot(conf.getString("expireFailedImage"));
# 593 "kind.ag"
              if (expPeriod < 0)
# 594 "kind.ag"
                throw Exception("expireFailedImage", "Time period must be positive");
# 595 "kind.ag"
              expireTime = imageTime + stot(conf.getString("expireFailedImage"));
# 596 "kind.ag"
              expireRule = "invalid image: " + conf.getString("expireFailedImage");
# 597 "kind.ag"
              debugPrint("- invalid image");
# 598 "kind.ag"
            }
# 599 "kind.ag"
          else
# 600 "kind.ag"
            {
# 601 "kind.ag"
              debugPrint("- valid image");
# 602 "kind.ag"
              expireTime = image.expire;
# 603 "kind.ag"
              expireRule = image.expireRule;
# 604 "kind.ag"
            }
# 605 "kind.ag"

# 606 "kind.ag"
          if (expireTime < now)
# 607 "kind.ag"
            {
# 608 "kind.ag"
              if (!quiet)
# 609 "kind.ag"
                cout << "  removing image " << image.name << endl;
# 610 "kind.ag"
              try
# 611 "kind.ag"
                {
# 612 "kind.ag"
                  if (removeDir(image.name) != 0)
# 613 "kind.ag"
                    cout << "Error removing " <<  image.name << endl;
# 614 "kind.ag"
                }
# 615 "kind.ag"
              catch (Exception ex)
# 616 "kind.ag"
                {
# 617 "kind.ag"
                  cerr << "Exception: " << ex.what() << endl;
# 618 "kind.ag"
                }
# 619 "kind.ag"
            }
# 620 "kind.ag"
        }
# 621 "kind.ag"
      else
# 622 "kind.ag"
        debugPrint("- current image - ignored");
# 623 "kind.ag"
    }
# 624 "kind.ag"
}
# 625 "kind.ag"

# 626 "kind.ag"
/*AppGen:Main*/
string ag_programName;

void usage()
{
  cout << ag_programName << " - archiving backup" << endl;
  cout << "Usage:" << endl;
  cout << ag_programName << " [<options>] vault_or_group " << endl;
  cout << "  vault_or_group - Vault to backup" << endl;
  cout << "Options:" << endl;
  cout << "  -c <s>    --masterconfig=<s>" << endl;
  cout << "     Master config file (default: \"\")" << endl;

  cout << "     if not given or empty kind looks for" << endl;
  cout << "     /etc/kind/master.conf" << endl;
  cout << "     /ffp/etc/kind/master.conf" << endl;
  cout << "  -f        --full" << endl;
  cout << "     Force full image == initial backup (default: false)" << endl;

  cout << "  -B        --backup" << endl;
  cout << "     Backup (default: false)" << endl;

  cout << "  -E        --expire" << endl;
  cout << "     Expire (default: false)" << endl;

  cout << "  -C        --listconfig" << endl;
  cout << "     Show configuration (default: false)" << endl;

  cout << "  -I        --listimages" << endl;
  cout << "     List data of images (default: false)" << endl;

  cout << "     if none of backup, expire, listconfig and listimages is specified," << endl;
  cout << "     backup and expire is assumed." << endl;
  cout << "     listconfig and listimages cannot be combined with other actions" << endl;
  cout << "  -D        --dryrun" << endl;
  cout << "     Dry run (no real backup) (default: false)" << endl;

  cout << "  -F <s>    --forcebackup=<s>" << endl;
  cout << "     Create image for specified backup set (default: \"\")" << endl;

  cout << "  -v        --verbose" << endl;
  cout << "     Verbose (default: false)" << endl;

  cout << "  -d        --debug" << endl;
  cout << "     Debug output of many data (default: false)" << endl;

  cout << "  -q        --quiet" << endl;
  cout << "     Be quiet - no messages (default: false)" << endl;

  cout << "  -h        --help" << endl;
  cout << "     This help" << endl;

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
  bool listImages = false;
  string forcedBackupSet = "";

  string vault = "";
  static struct option ag_long_options[] =
  {
    {"masterconfig", required_argument, 0, 'c' },
    {"full", no_argument, 0, 'f' },
    {"backup", no_argument, 0, 'B' },
    {"expire", no_argument, 0, 'E' },
    {"listconfig", no_argument, 0, 'C' },
    {"listimages", no_argument, 0, 'I' },
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
  while ((rc = getopt_long(argc, argv, ":c:fBECIDF:vdqh", ag_long_options, NULL)) >= 0)
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

        case 'I':
          listImages = true;
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
# 630 "kind.ag"

# 631 "kind.ag"
  int exitCode = 0;
# 632 "kind.ag"
  string lockFile;
# 633 "kind.ag"
  try
# 634 "kind.ag"
    {
# 635 "kind.ag"
      // handling of parameters and switches
# 636 "kind.ag"
      if (debug)        // debug implies verbose
# 637 "kind.ag"
        verbose = true;
# 638 "kind.ag"

# 639 "kind.ag"
      if (!doBackup && !doExpire && !listConfig && !listImages)
# 640 "kind.ag"
        {
# 641 "kind.ag"
          doBackup = true;
# 642 "kind.ag"
          doExpire = true;
# 643 "kind.ag"
        }
# 644 "kind.ag"

# 645 "kind.ag"
      KindConfig conf;
# 646 "kind.ag"

# 647 "kind.ag"
      // default-values
# 648 "kind.ag"
      conf.add("imageName", "image");
# 649 "kind.ag"
      conf.add("vaultConfigName", "kind/vault.conf");
# 650 "kind.ag"
      conf.add("expireFailedImage", "3 days");
# 651 "kind.ag"
      conf.add("expireRule", "* * * * 1 month");
# 652 "kind.ag"
      conf.add("ping", "ping -c 1 -W 5 %host");
# 653 "kind.ag"
      conf.add("rsyncOption", ""); // no additional rsync option
# 654 "kind.ag"
      conf.add("remoteShell", "");
# 655 "kind.ag"
      conf.add("lockfile", "/var/lock/kind");
# 656 "kind.ag"
      conf.add("userExcludeFile", "nobackup.list");
# 657 "kind.ag"
      conf.add("userExcludeCommand",
# 658 "kind.ag"
               "find %path -type f -iname '*nobackup' -printf '%P\\\\n'");
# 659 "kind.ag"
      conf.add("logSize", "");
# 660 "kind.ag"
      conf.add("lastLink", "symLink");
# 661 "kind.ag"

# 662 "kind.ag"
      if (listConfig)
# 663 "kind.ag"
        {
# 664 "kind.ag"
          cout << "builtin config" << endl;
# 665 "kind.ag"
          conf.print(".   ");
# 666 "kind.ag"
        }
# 667 "kind.ag"

# 668 "kind.ag"
      readMasterConfig(masterConfig, conf);
# 669 "kind.ag"

# 670 "kind.ag"
      banks = conf.getStrings("bank");
# 671 "kind.ag"
      if (banks.empty())
# 672 "kind.ag"
        throw Exception("read master configuration", "no banks defined");
# 673 "kind.ag"

# 674 "kind.ag"
      vector<string> vaults;
# 675 "kind.ag"
      string groupname = "group_" + vault;
# 676 "kind.ag"
      if (conf.hasKey(groupname))
# 677 "kind.ag"
        {
# 678 "kind.ag"
          vaults = conf.getStrings(groupname);
# 679 "kind.ag"
          vault.clear(); // no single vault but group
# 680 "kind.ag"
        }
# 681 "kind.ag"
      else
# 682 "kind.ag"
        vaults.push_back(vault);
# 683 "kind.ag"

# 684 "kind.ag"
      if (listConfig)
# 685 "kind.ag"
        {
# 686 "kind.ag"
          cout << "global config:" << endl;
# 687 "kind.ag"
          conf.print(".   ");
# 688 "kind.ag"
          if (!vault.empty())
# 689 "kind.ag"
            {
# 690 "kind.ag"
              readVaultConfig(vault, conf);
# 691 "kind.ag"
              cout << "vault config:" << endl;
# 692 "kind.ag"
              conf.print(".   ");
# 693 "kind.ag"
            }
# 694 "kind.ag"
          else
# 695 "kind.ag"
            cout << "specify single vault (not group) to see vault config" << endl;
# 696 "kind.ag"
          exit(0);
# 697 "kind.ag"
        }
# 698 "kind.ag"

# 699 "kind.ag"
      DateTime imageTime = DateTime::now();
# 700 "kind.ag"

# 701 "kind.ag"
      if (listImages)
# 702 "kind.ag"
        {
# 703 "kind.ag"
          for (string vault : vaults)
# 704 "kind.ag"
            listImageInfo(vault, conf, imageTime, forcedBackupSet);
# 705 "kind.ag"
          exit(0);
# 706 "kind.ag"
        }
# 707 "kind.ag"

# 708 "kind.ag"
      // previous actions do not need locking
# 709 "kind.ag"
      lockFile = conf.getString("lockfile");
# 710 "kind.ag"
      createLock(lockFile);
# 711 "kind.ag"

# 712 "kind.ag"
      string logSizeFile = conf.getString("logSize");
# 713 "kind.ag"
      readSizes(logSizeFile);
# 714 "kind.ag"

# 715 "kind.ag"
      for (string vault : vaults)
# 716 "kind.ag"
        {
# 717 "kind.ag"
          if (doBackup)
# 718 "kind.ag"
            if (backupVault(vault, conf, imageTime, fullImage, forcedBackupSet))
# 719 "kind.ag"
              writeSizes(logSizeFile);
# 720 "kind.ag"
          if (doExpire)
# 721 "kind.ag"
            expireVault(vault, conf, imageTime);
# 722 "kind.ag"
        }
# 723 "kind.ag"

# 724 "kind.ag"
      if (!quiet)
# 725 "kind.ag"
        cout << DateTime::now().getString('h') << ": finished" << endl;
# 726 "kind.ag"

# 727 "kind.ag"
    }
# 728 "kind.ag"
  catch (const Exception& ex)
# 729 "kind.ag"
    {
# 730 "kind.ag"
      cerr << "Exception: " << ex.what() << endl;
# 731 "kind.ag"
      exitCode = 1;
# 732 "kind.ag"
    }
# 733 "kind.ag"
  catch (const char* msg)
# 734 "kind.ag"
    {
# 735 "kind.ag"
      cerr << "Exception(char*): " << msg << endl;
# 736 "kind.ag"
      exitCode = 1;
# 737 "kind.ag"
    }
# 738 "kind.ag"
  catch (const string& msg)
# 739 "kind.ag"
    {
# 740 "kind.ag"
      cerr << "Exception(string): " << msg << endl;
# 741 "kind.ag"
      exitCode = 1;
# 742 "kind.ag"
    }
# 743 "kind.ag"
  removeLock(lockFile);
# 744 "kind.ag"
  return exitCode;
# 745 "kind.ag"
}
