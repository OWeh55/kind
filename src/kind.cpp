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
  cout << "---" << endl;
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
        {
# 280 "kind.ag"
          throw Exception("Host not available", pingCommand);
# 281 "kind.ag"
        }
# 282 "kind.ag"
    }
# 283 "kind.ag"

# 284 "kind.ag"
  string path = conf.getString("path");
# 285 "kind.ag"
  if (path.empty())
# 286 "kind.ag"
    throw Exception("rsync", "empty source path");
# 287 "kind.ag"
  if (path.back() != '/')
# 288 "kind.ag"
    path += '/';
# 289 "kind.ag"

# 290 "kind.ag"
  string rsyncCmd = "rsync -vrltH --delete --stats -D --numeric-ids ";
# 291 "kind.ag"
  if (!conf.getBool("ignorePermission"))
# 292 "kind.ag"
    rsyncCmd += "-pgo";
# 293 "kind.ag"
  vector<string> rso = conf.getStrings("rsyncOption");
# 294 "kind.ag"
  for (const string& opt : rso)
# 295 "kind.ag"
    rsyncCmd += opt + " ";
# 296 "kind.ag"

# 297 "kind.ag"
  // excludes
# 298 "kind.ag"
  Strings excluded = getExclusions(conf, shellMode);
# 299 "kind.ag"

# 300 "kind.ag"
  // create image path
# 301 "kind.ag"
  if (!dryRun)
# 302 "kind.ag"
    if (mkdir(imageFullName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
# 303 "kind.ag"
      throw Exception("Create image", "failed to create " + imageFullName);
# 304 "kind.ag"

# 305 "kind.ag"
  // error message
# 306 "kind.ag"
  // we write an generic error message to mark backup as unsuccessful
# 307 "kind.ag"
  // will be deleted at successful end of rsync
# 308 "kind.ag"
  string errorfile = imageFullName + "/error";
# 309 "kind.ag"
  if (!dryRun)
# 310 "kind.ag"
    {
# 311 "kind.ag"
      ofstream error(errorfile);
# 312 "kind.ag"
      error << "failed" << endl;
# 313 "kind.ag"
      error.close();
# 314 "kind.ag"
    }
# 315 "kind.ag"

# 316 "kind.ag"
  if (shellMode)  // shell mode
# 317 "kind.ag"
    {
# 318 "kind.ag"
      // cout << "USING SHELLMODE '" << host << "'" << endl;
# 319 "kind.ag"
      string remoteShell = conf.getString("remoteShell");
# 320 "kind.ag"
      string userAtHost = conf.getString("user") + "@" + conf.getString("host");
# 321 "kind.ag"
      string rshCommand = remoteShell;
# 322 "kind.ag"
      if (remoteShell.empty())
# 323 "kind.ag"
        rshCommand = "ssh";
# 324 "kind.ag"

# 325 "kind.ag"
      rshCommand += " " + userAtHost;
# 326 "kind.ag"

# 327 "kind.ag"
      if (!dryRun)
# 328 "kind.ag"
        strings2File(excluded, imageFullName + "/exclude");
# 329 "kind.ag"

# 330 "kind.ag"
      // rsync image
# 331 "kind.ag"

# 332 "kind.ag"
      if (!remoteShell.empty())
# 333 "kind.ag"
        rsyncCmd += " -e \'" + remoteShell + "\' ";
# 334 "kind.ag"

# 335 "kind.ag"
      rsyncCmd += "--exclude-from=" + imageFullName + "/exclude ";
# 336 "kind.ag"
      if (!referenceImage.empty())
# 337 "kind.ag"
        rsyncCmd += "--link-dest=" + referenceImage + "/tree ";
# 338 "kind.ag"
      rsyncCmd += userAtHost + ":" + path + " ";
# 339 "kind.ag"
      rsyncCmd += imageFullName + "/tree";
# 340 "kind.ag"
    } // shell mode
# 341 "kind.ag"
  else
# 342 "kind.ag"
    {
# 343 "kind.ag"
      // cout << "USING SERVERMODE" << endl;
# 344 "kind.ag"
      // we cannot use find without shell access
# 345 "kind.ag"
      // and do not read an exclude file on client side
# 346 "kind.ag"

# 347 "kind.ag"
      if (!dryRun)
# 348 "kind.ag"
        strings2File(excluded, imageFullName + "/exclude");
# 349 "kind.ag"

# 350 "kind.ag"
      rsyncCmd += "--exclude-from=" + imageFullName + "/exclude ";
# 351 "kind.ag"
      if (!referenceImage.empty())
# 352 "kind.ag"
        rsyncCmd += "--link-dest=" + referenceImage + "/tree ";
# 353 "kind.ag"
      rsyncCmd += conf.getString("server") + "::" + path + " ";
# 354 "kind.ag"
      rsyncCmd += imageFullName + "/tree";
# 355 "kind.ag"
    }
# 356 "kind.ag"

# 357 "kind.ag"
  debugPrint("Action: " + rsyncCmd);
# 358 "kind.ag"

# 359 "kind.ag"
  vector<string> backupResult;
# 360 "kind.ag"
  if (!dryRun)
# 361 "kind.ag"
    {
# 362 "kind.ag"
      verbosePrint("syncing (" + rsyncCmd + ")");
# 363 "kind.ag"
      int rc;
# 364 "kind.ag"
      backupResult = localExec(rsyncCmd, rc, debug, imageFullName + "/rsync-log");
# 365 "kind.ag"
      if (rc == 0 ||
# 366 "kind.ag"
      rc == 24 || // "no error" or "vanished source files" (ignored)
# 367 "kind.ag"
      rc == 6144) // workaround for wrong exit code ??!!
# 368 "kind.ag"
        {
# 369 "kind.ag"
          unlink(errorfile.c_str());
# 370 "kind.ag"
          long int st = 0;
# 371 "kind.ag"
          long int sc = 0;
# 372 "kind.ag"
          for (auto bl : backupResult)
# 373 "kind.ag"
            {
# 374 "kind.ag"
              if (startsWith(bl, "Total file size"))
# 375 "kind.ag"
                st = getNumber(bl);
# 376 "kind.ag"
              else if (startsWith(bl, "Total transferred file size"))
# 377 "kind.ag"
                sc = getNumber(bl);
# 378 "kind.ag"
            }
# 379 "kind.ag"
          // sizes[vault] = pair<long int, long int>(st, sc);
# 380 "kind.ag"
          sizes[vault] = Sizes(st, sc);
# 381 "kind.ag"
          //  cout << vault << " " << st << " || " << sc << endl;
# 382 "kind.ag"
        }
# 383 "kind.ag"
      else
# 384 "kind.ag"
        throw Exception("Backup", "Failed to execute rsync (result: " + to_string(rc) + ")");
# 385 "kind.ag"
    }
# 386 "kind.ag"
  else
# 387 "kind.ag"
    cout << "Not executing " << rsyncCmd << endl;
# 388 "kind.ag"
}
# 389 "kind.ag"

# 390 "kind.ag"
bool backupVault(const string& vault,
# 391 "kind.ag"
                 KindConfig conf /*Copy!*/ ,
# 392 "kind.ag"
                 const DateTime& imageTime,
# 393 "kind.ag"
                 bool fullImage,
# 394 "kind.ag"
                 const string& forcedBackupSet)
# 395 "kind.ag"
{
# 396 "kind.ag"
  if (!quiet)
# 397 "kind.ag"
    cout << DateTime::now().getString('h') << ": Backup of vault " << vault << endl;
# 398 "kind.ag"
  try
# 399 "kind.ag"
    {
# 400 "kind.ag"
      readVaultConfig(vault, conf);
# 401 "kind.ag"

# 402 "kind.ag"
      // where to store
# 403 "kind.ag"
      string vaultPath = findVault(vault);
# 404 "kind.ag"

# 405 "kind.ag"
      // image path
# 406 "kind.ag"
      string imageFullName = getImageName(conf, vaultPath, imageTime);
# 407 "kind.ag"

# 408 "kind.ag"
      bool backupNow = true;
# 409 "kind.ag"

# 410 "kind.ag"
      // existing images
# 411 "kind.ag"
      Images validImageList = findImages(vaultPath, conf, false);
# 412 "kind.ag"
      string currentSet = "expire"; // we are not using backupSets
# 413 "kind.ag"

# 414 "kind.ag"
      // check if we are using backup sets
# 415 "kind.ag"

# 416 "kind.ag"
      map<string, int> setIdx;
# 417 "kind.ag"
      vector<SetRule> backupSetRule;
# 418 "kind.ag"
      int setRuleIdx = -1;
# 419 "kind.ag"

# 420 "kind.ag"
      if (conf.hasKey("setRule"))
# 421 "kind.ag"
        {
# 422 "kind.ag"
          readSetRules(conf, setIdx, backupSetRule);
# 423 "kind.ag"
          if (!setIdx.empty())
# 424 "kind.ag"
            {
# 425 "kind.ag"
              if (forcedBackupSet.empty())
# 426 "kind.ag"
                {
# 427 "kind.ag"
                  backupNow = false;
# 428 "kind.ag"

# 429 "kind.ag"
                  // find time for nextBackup for every backupSet
# 430 "kind.ag"
                  // defaults to now == imageTime;
# 431 "kind.ag"
                  vector<DateTime> nextBackup(backupSetRule.size(), imageTime);
# 432 "kind.ag"

# 433 "kind.ag"
                  // find time for next backup
# 434 "kind.ag"

# 435 "kind.ag"
                  for (const Image& image : validImageList)
# 436 "kind.ag"
                    {
# 437 "kind.ag"
                      if (image.series != "expire")
# 438 "kind.ag"
                        {
# 439 "kind.ag"
                          string s = image.series;
# 440 "kind.ag"
                          if (setIdx.count(s) > 0) // rule for set exists?
# 441 "kind.ag"
                            {
# 442 "kind.ag"
                              int rIdx = setIdx[s];
# 443 "kind.ag"
                              // image is valid for this and "lower level" backupSets
# 444 "kind.ag"
                              for (unsigned int i = rIdx; i < backupSetRule.size(); ++i)
# 445 "kind.ag"
                                if (nextBackup[i] < image.time + backupSetRule[i].distance)
# 446 "kind.ag"
                                  nextBackup[i] =  image.time + backupSetRule[i].distance;
# 447 "kind.ag"
                            }
# 448 "kind.ag"
                        }
# 449 "kind.ag"
                    }
# 450 "kind.ag"
                  if (debug)
# 451 "kind.ag"
                    for (unsigned int i = 0; i < backupSetRule.size(); ++i)
# 452 "kind.ag"
                      cout << "       Next backup for " << backupSetRule[i].name << " at " <<  nextBackup[i].getString('h') << endl;
# 453 "kind.ag"

# 454 "kind.ag"
                  // find backupSet that
# 455 "kind.ag"
                  //    - needs backup
# 456 "kind.ag"
                  //    - has longest time to keep
# 457 "kind.ag"
                  // because of ordered list backupSetRule this is the first set, that need
# 458 "kind.ag"

# 459 "kind.ag"
                  currentSet = "";
# 460 "kind.ag"
                  for (unsigned int i = 0; i < backupSetRule.size() && currentSet.empty(); ++i)
# 461 "kind.ag"
                    {
# 462 "kind.ag"
                      string name = backupSetRule[i].name;
# 463 "kind.ag"
                      if (nextBackup[i] <= imageTime + 5) // small offset of 5s for "jitter"
# 464 "kind.ag"
                        {
# 465 "kind.ag"
                          backupNow = true;
# 466 "kind.ag"
                          currentSet = name;
# 467 "kind.ag"
                          setRuleIdx = i;
# 468 "kind.ag"
                        }
# 469 "kind.ag"
                    }
# 470 "kind.ag"
                }
# 471 "kind.ag"
              else
# 472 "kind.ag"
                {
# 473 "kind.ag"
                  if (setIdx.count(forcedBackupSet) > 0)
# 474 "kind.ag"
                    {
# 475 "kind.ag"
                      currentSet = forcedBackupSet;
# 476 "kind.ag"
                      setRuleIdx = setIdx[forcedBackupSet];
# 477 "kind.ag"
                    }
# 478 "kind.ag"
                  else
# 479 "kind.ag"
                    throw Exception("force backup of set " + forcedBackupSet, " set not exists");
# 480 "kind.ag"
                }
# 481 "kind.ag"
            } // if (!setIdx.empty())
# 482 "kind.ag"
        } // (conf.hasKey("setRule"))
# 483 "kind.ag"

# 484 "kind.ag"
      if (backupNow)
# 485 "kind.ag"
        {
# 486 "kind.ag"
          verbosePrint("backup to \"" + imageFullName + "\"");
# 487 "kind.ag"
          if (setRuleIdx >= 0 && !quiet)
# 488 "kind.ag"
            cout << "  backup set is \"" << currentSet << "\"" << endl;
# 489 "kind.ag"
        }
# 490 "kind.ag"
      else if (!quiet)
# 491 "kind.ag"
        cout << "  no backup set needs update" << endl;
# 492 "kind.ag"

# 493 "kind.ag"
      if (backupNow)
# 494 "kind.ag"
        {
# 495 "kind.ag"
          // find reference image
# 496 "kind.ag"
          string referenceImage;
# 497 "kind.ag"
          if (!fullImage)
# 498 "kind.ag"
            {
# 499 "kind.ag"
              if (validImageList.empty())
# 500 "kind.ag"
                throw Exception("backupVault", "no reference image found");
# 501 "kind.ag"
              // last image is newest image
# 502 "kind.ag"
              referenceImage = validImageList.back().name;
# 503 "kind.ag"
            }
# 504 "kind.ag"

# 505 "kind.ag"
          doBackup(vault, imageFullName, referenceImage, conf);
# 506 "kind.ag"

# 507 "kind.ag"
          if (!dryRun)
# 508 "kind.ag"
            {
# 509 "kind.ag"
              string lastPath = vaultPath + "/last";
# 510 "kind.ag"
              struct stat fstat;
# 511 "kind.ag"

# 512 "kind.ag"
              // remove last (dir or symlink)
# 513 "kind.ag"
              if (lstat(lastPath.c_str(), &fstat) == 0) // last exists
# 514 "kind.ag"
                {
# 515 "kind.ag"
                  if (S_ISDIR(fstat.st_mode))
# 516 "kind.ag"
                    removeDir(lastPath);
# 517 "kind.ag"
                  else
# 518 "kind.ag"
                    unlink(lastPath.c_str());
# 519 "kind.ag"
                }
# 520 "kind.ag"

# 521 "kind.ag"
              string linkType = conf.getString("lastLink");
# 522 "kind.ag"
              if (linkType == "hardLink")
# 523 "kind.ag"
                {
# 524 "kind.ag"
                  int rc;
# 525 "kind.ag"
                  string hardLinkCommand = "cp -al " + imageFullName + " " + lastPath;
# 526 "kind.ag"
                  Strings res = localExec(hardLinkCommand, rc, debug);
# 527 "kind.ag"
                }
# 528 "kind.ag"
              else if (linkType == "symLink")
# 529 "kind.ag"
                {
# 530 "kind.ag"
                  // set symlink to last image
# 531 "kind.ag"
                  symlink(imageFullName.c_str(), lastPath.c_str());
# 532 "kind.ag"
                }
# 533 "kind.ag"
              else if (linkType != "null")
# 534 "kind.ag"
                cerr << "invalid Value in \"lastLink\"" << endl;
# 535 "kind.ag"

# 536 "kind.ag"
              // write expire date to file
# 537 "kind.ag"
              DateTime expireTime;
# 538 "kind.ag"
              string rule;
# 539 "kind.ag"
              if (setRuleIdx < 0) // not backup set based
# 540 "kind.ag"
                expireTime = getExpireDate(imageTime, conf, rule);
# 541 "kind.ag"
              else
# 542 "kind.ag"
                {
# 543 "kind.ag"
                  expireTime = imageTime + backupSetRule[setRuleIdx].keep;
# 544 "kind.ag"
                  rule =  backupSetRule[setRuleIdx].rule;
# 545 "kind.ag"
                }
# 546 "kind.ag"

# 547 "kind.ag"
              ofstream expireFile(imageFullName + "/expires");
# 548 "kind.ag"
              expireFile << currentSet << "-" << expireTime.getString('m') << endl;
# 549 "kind.ag"
              expireFile << rule << endl;
# 550 "kind.ag"
            }
# 551 "kind.ag"
        }
# 552 "kind.ag"
      return backupNow;
# 553 "kind.ag"
    }
# 554 "kind.ag"
  catch (Exception ex)
# 555 "kind.ag"
    {
# 556 "kind.ag"
      cerr << "Exception in vault " << vault << ": " << ex.what() << endl;
# 557 "kind.ag"
      return false;
# 558 "kind.ag"
    }
# 559 "kind.ag"
}
# 560 "kind.ag"

# 561 "kind.ag"
void expireVault(const string& vault, KindConfig conf, DateTime now)
# 562 "kind.ag"
{
# 563 "kind.ag"
  if (!quiet)
# 564 "kind.ag"
    cout << DateTime::now().getString('h') << ": Expiring images in vault " << vault << endl;
# 565 "kind.ag"

# 566 "kind.ag"
  readVaultConfig(vault, conf);
# 567 "kind.ag"

# 568 "kind.ag"
  string vaultpath = findVault(vault);
# 569 "kind.ag"

# 570 "kind.ag"
  Images imagelist = findImages(vaultpath, conf, true);
# 571 "kind.ag"

# 572 "kind.ag"
  string lastValidImage;
# 573 "kind.ag"
  for (Image image : imagelist)
# 574 "kind.ag"
    {
# 575 "kind.ag"
      if (image.valid)
# 576 "kind.ag"
        lastValidImage = image.name;
# 577 "kind.ag"
    }
# 578 "kind.ag"

# 579 "kind.ag"
  for (Image image : imagelist)
# 580 "kind.ag"
    {
# 581 "kind.ag"
      if (debug)
# 582 "kind.ag"
        image.printInfo();
# 583 "kind.ag"

# 584 "kind.ag"
      DateTime imageTime = image.time;
# 585 "kind.ag"

# 586 "kind.ag"
      if (imageTime != now &&          // ignore just created image
# 587 "kind.ag"
      image.name != lastValidImage // ignore last valid image
# 588 "kind.ag"
         )
# 589 "kind.ag"
        {
# 590 "kind.ag"
          DateTime expireTime;
# 591 "kind.ag"
          string expireRule;
# 592 "kind.ag"
          if (!image.valid) // invalid image?
# 593 "kind.ag"
            {
# 594 "kind.ag"
              time_t expPeriod = stot(conf.getString("expireFailedImage"));
# 595 "kind.ag"
              if (expPeriod < 0)
# 596 "kind.ag"
                throw Exception("expireFailedImage", "Time period must be positive");
# 597 "kind.ag"
              expireTime = imageTime + stot(conf.getString("expireFailedImage"));
# 598 "kind.ag"
              expireRule = "invalid image: " + conf.getString("expireFailedImage");
# 599 "kind.ag"
              debugPrint("- invalid image");
# 600 "kind.ag"
            }
# 601 "kind.ag"
          else
# 602 "kind.ag"
            {
# 603 "kind.ag"
              debugPrint("- valid image");
# 604 "kind.ag"
              expireTime = image.expire;
# 605 "kind.ag"
              expireRule = image.expireRule;
# 606 "kind.ag"
            }
# 607 "kind.ag"

# 608 "kind.ag"
          if (expireTime < now)
# 609 "kind.ag"
            {
# 610 "kind.ag"
              if (!quiet)
# 611 "kind.ag"
                cout << "  removing image " << image.name << endl;
# 612 "kind.ag"
              try
# 613 "kind.ag"
                {
# 614 "kind.ag"
                  if (removeDir(image.name) != 0)
# 615 "kind.ag"
                    cout << "Error removing " <<  image.name << endl;
# 616 "kind.ag"
                }
# 617 "kind.ag"
              catch (Exception ex)
# 618 "kind.ag"
                {
# 619 "kind.ag"
                  cerr << "Exception: " << ex.what() << endl;
# 620 "kind.ag"
                }
# 621 "kind.ag"
            }
# 622 "kind.ag"
        }
# 623 "kind.ag"
      else
# 624 "kind.ag"
        debugPrint("- current image - ignored");
# 625 "kind.ag"
    }
# 626 "kind.ag"
}
# 627 "kind.ag"

# 628 "kind.ag"
/*AppGen:Main*/
string ag_programName;

void usage()
{
  cout << ag_programName << " - archiving backup" << endl;
  cout << "Aufruf:" << endl;
  cout << ag_programName << " [<options>] vault_or_group " << endl;
  cout << "  vault_or_group - Vault to backup" << endl;
  cout << "Optionen:" << endl;
  cout << "  -c --masterconfig  Master config file (default: \"\")" << endl;
  cout << "                     if not given or empty kind looks for" << endl;
  cout << "                     /etc/kind/master.conf" << endl;
  cout << "                     /ffp/etc/kind/master.conf" << endl;
  cout << "  -f --full          Force full image == initial backup (default: false)" << endl;
  cout << "  -B --backup        Backup (default: false)" << endl;
  cout << "  -E --expire        Expire (default: false)" << endl;
  cout << "  -C --listconfig    Show configuration (default: false)" << endl;
  cout << "  -I --listimages    List data of images (default: false)" << endl;
  cout << "                     if none of backup, expire, listconfig and listimages is specified," << endl;
  cout << "                     backup and expire is assumed." << endl;
  cout << "                     listconfig and listimages cannot be combined with other actions" << endl;
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
          error("Unbekannte Option");
          break;
        case ':':
          error("erwarte Option-Parameter");
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
          error("Unbekannter Fehler in Optionen");
        }
    }
  if (optind < argc)
    vault = argv[optind++];
  else error("Erwarte Parameter vault_or_group");

  /*AppGen:MainEnd*/
# 632 "kind.ag"

# 633 "kind.ag"
  int exitCode = 0;
# 634 "kind.ag"
  string lockFile;
# 635 "kind.ag"
  try
# 636 "kind.ag"
    {
# 637 "kind.ag"
      // handling of parameters and switches
# 638 "kind.ag"
      if (debug)        // debug implies verbose
# 639 "kind.ag"
        verbose = true;
# 640 "kind.ag"

# 641 "kind.ag"
      if (!doBackup && !doExpire && !listConfig && !listImages)
# 642 "kind.ag"
        {
# 643 "kind.ag"
          doBackup = true;
# 644 "kind.ag"
          doExpire = true;
# 645 "kind.ag"
        }
# 646 "kind.ag"

# 647 "kind.ag"
      KindConfig conf;
# 648 "kind.ag"

# 649 "kind.ag"
      // default-values
# 650 "kind.ag"
      conf.add("imageName", "image");
# 651 "kind.ag"
      conf.add("vaultConfigName", "kind/vault.conf");
# 652 "kind.ag"
      conf.add("expireFailedImage", "3 days");
# 653 "kind.ag"
      conf.add("expireRule", "* * * * 1 month");
# 654 "kind.ag"
      conf.add("ping", "ping -c 1 -W 5 %host");
# 655 "kind.ag"
      conf.add("rsyncOption", ""); // no additional rsync option
# 656 "kind.ag"
      conf.add("remoteShell", "");
# 657 "kind.ag"
      conf.add("lockfile", "/var/lock/kind");
# 658 "kind.ag"
      conf.add("userExcludeFile", "nobackup.list");
# 659 "kind.ag"
      conf.add("userExcludeCommand",
# 660 "kind.ag"
               "find %path -type f -iname '*nobackup' -printf '%P\\\\n'");
# 661 "kind.ag"
      conf.add("logSize", "");
# 662 "kind.ag"
      conf.add("lastLink", "symLink");
# 663 "kind.ag"

# 664 "kind.ag"
      if (listConfig)
# 665 "kind.ag"
        {
# 666 "kind.ag"
          cout << "builtin config" << endl;
# 667 "kind.ag"
          conf.print(".   ");
# 668 "kind.ag"
        }
# 669 "kind.ag"

# 670 "kind.ag"
      readMasterConfig(masterConfig, conf);
# 671 "kind.ag"

# 672 "kind.ag"
      banks = conf.getStrings("bank");
# 673 "kind.ag"
      if (banks.empty())
# 674 "kind.ag"
        throw Exception("read master configuration", "no banks defined");
# 675 "kind.ag"

# 676 "kind.ag"
      if (listConfig)
# 677 "kind.ag"
        {
# 678 "kind.ag"
          cout << "global config:" << endl;
# 679 "kind.ag"
          conf.print(".   ");
# 680 "kind.ag"
          readVaultConfig(vault, conf);
# 681 "kind.ag"
          cout << "vault config:" << endl;
# 682 "kind.ag"
          conf.print(".   ");
# 683 "kind.ag"
          exit(0);
# 684 "kind.ag"
        }
# 685 "kind.ag"

# 686 "kind.ag"
      DateTime imageTime = DateTime::now();
# 687 "kind.ag"

# 688 "kind.ag"
      if (listImages)
# 689 "kind.ag"
        {
# 690 "kind.ag"
          listImageInfo(vault, conf, imageTime, forcedBackupSet);
# 691 "kind.ag"
          exit(0);
# 692 "kind.ag"
        }
# 693 "kind.ag"

# 694 "kind.ag"
      lockFile = conf.getString("lockfile");
# 695 "kind.ag"
      createLock(lockFile);
# 696 "kind.ag"

# 697 "kind.ag"
      string logSizeFile = conf.getString("logSize");
# 698 "kind.ag"
      readSizes(logSizeFile);
# 699 "kind.ag"

# 700 "kind.ag"
      vector<string> vaults;
# 701 "kind.ag"
      string groupname = "group_" + vault;
# 702 "kind.ag"
      if (conf.hasKey(groupname))
# 703 "kind.ag"
        vaults = conf.getStrings(groupname);
# 704 "kind.ag"
      else
# 705 "kind.ag"
        vaults.push_back(vault);
# 706 "kind.ag"

# 707 "kind.ag"

# 708 "kind.ag"
      for (string vault : vaults)
# 709 "kind.ag"
        {
# 710 "kind.ag"
          if (doBackup)
# 711 "kind.ag"
            if (backupVault(vault, conf, imageTime, fullImage, forcedBackupSet))
# 712 "kind.ag"
              writeSizes(logSizeFile);
# 713 "kind.ag"
          if (doExpire)
# 714 "kind.ag"
            expireVault(vault, conf, imageTime);
# 715 "kind.ag"
        }
# 716 "kind.ag"

# 717 "kind.ag"
      if (!quiet)
# 718 "kind.ag"
        cout << DateTime::now().getString('h') << ": finished" << endl;
# 719 "kind.ag"

# 720 "kind.ag"
    }
# 721 "kind.ag"
  catch (const Exception& ex)
# 722 "kind.ag"
    {
# 723 "kind.ag"
      cerr << "Exception: " << ex.what() << endl;
# 724 "kind.ag"
      exitCode = 1;
# 725 "kind.ag"
    }
# 726 "kind.ag"
  catch (const char* msg)
# 727 "kind.ag"
    {
# 728 "kind.ag"
      cerr << "Exception(char*): " << msg << endl;
# 729 "kind.ag"
      exitCode = 1;
# 730 "kind.ag"
    }
# 731 "kind.ag"
  catch (const string& msg)
# 732 "kind.ag"
    {
# 733 "kind.ag"
      cerr << "Exception(string): " << msg << endl;
# 734 "kind.ag"
      exitCode = 1;
# 735 "kind.ag"
    }
# 736 "kind.ag"
  removeLock(lockFile);
# 737 "kind.ag"
  return exitCode;
# 738 "kind.ag"
}
