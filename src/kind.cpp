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
  opt: I, listimages, void, listImages, List data of images, false
# 47 "kind.ag"
  opt2:   if none of backup, expire, listconfig and listimages is specified,
# 48 "kind.ag"
  opt2:   backup and expire is assumed.
# 49 "kind.ag"
  opt2:   listconfig and listimages cannot be combined with other actions
# 50 "kind.ag"
  opt: D, dryrun, Void, dryRun, Dry run (no real backup), false
# 51 "kind.ag"
  opt: F, forcebackup, string, forcedBackupSet, Create image for specified backup set, ""
# 52 "kind.ag"
  opt: v, verbose, Void, verbose,  Verbose,  false
# 53 "kind.ag"
  opt: d, debug, Void, debug, Debug output of many data, false
# 54 "kind.ag"
  opt: q, quiet, Void, quiet, Be quiet - no messages, false
# 55 "kind.ag"
  opt: h, help, usage, ignored , This help
# 56 "kind.ag"
AppGen*/
# 57 "kind.ag"

# 58 "kind.ag"
using namespace std;
# 59 "kind.ag"

# 60 "kind.ag"
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
# 61 "kind.ag"

# 62 "kind.ag"
Strings banks;
# 63 "kind.ag"

# 64 "kind.ag"
string findVault(const string& v);
# 65 "kind.ag"

# 66 "kind.ag"
typedef pair<long int, long int> Sizes;
# 67 "kind.ag"
map<string, Sizes> sizes;
# 68 "kind.ag"

# 69 "kind.ag"
void readSizes(const string& logSizeFile)
# 70 "kind.ag"
{
# 71 "kind.ag"
  if (!logSizeFile.empty() && fileExists(logSizeFile))
# 72 "kind.ag"
    {
# 73 "kind.ag"
      vector<string> ss;
# 74 "kind.ag"
      file2Strings(logSizeFile, ss);
# 75 "kind.ag"
      for (const string& s : ss)
# 76 "kind.ag"
        {
# 77 "kind.ag"
          unsigned int i = 0;
# 78 "kind.ag"
          string vault = getWord(s, i);
# 79 "kind.ag"
          long int s1 = getLongInt(s, i);
# 80 "kind.ag"
          long int s2 = getLongInt(s, i);
# 81 "kind.ag"
          try {
# 82 "kind.ag"
              findVault(vault);
# 83 "kind.ag"
              sizes[vault] = Sizes(s1, s2);
# 84 "kind.ag"
            }
# 85 "kind.ag"
          catch (...)
# 86 "kind.ag"
            {
# 87 "kind.ag"
              // ignore missing vaults
# 88 "kind.ag"
            }
# 89 "kind.ag"
        }
# 90 "kind.ag"
    }
# 91 "kind.ag"
}
# 92 "kind.ag"

# 93 "kind.ag"
void writeSizes(const string logSizeFile)
# 94 "kind.ag"
{
# 95 "kind.ag"
  if (!logSizeFile.empty())
# 96 "kind.ag"
    {
# 97 "kind.ag"
      Strings st;
# 98 "kind.ag"
      for (auto s : sizes)
# 99 "kind.ag"
        {
# 100 "kind.ag"
          string h = s.first + " " + to_string(s.second.first) + " " + to_string(s.second.second);
# 101 "kind.ag"
          st.push_back(h);
# 102 "kind.ag"
        }
# 103 "kind.ag"
      strings2File(st, logSizeFile);
# 104 "kind.ag"
    }
# 105 "kind.ag"
}
# 106 "kind.ag"

# 107 "kind.ag"
void verbosePrint(const string& text)
# 108 "kind.ag"
{
# 109 "kind.ag"
  if (verbose)
# 110 "kind.ag"
    cout << "  " << text << endl;
# 111 "kind.ag"
}
# 112 "kind.ag"

# 113 "kind.ag"
void debugPrint(const string& text)
# 114 "kind.ag"
{
# 115 "kind.ag"
  if (debug)
# 116 "kind.ag"
    cout << "    " << text << endl;
# 117 "kind.ag"
}
# 118 "kind.ag"

# 119 "kind.ag"
void readMasterConfig1(const string& fn, KindConfig& conf)
# 120 "kind.ag"
{
# 121 "kind.ag"
  verbosePrint("reading master config " + fn);
# 122 "kind.ag"
  conf.addFile(fn);
# 123 "kind.ag"
}
# 124 "kind.ag"

# 125 "kind.ag"
void readMasterConfig(const string& fn, KindConfig& conf)
# 126 "kind.ag"
{
# 127 "kind.ag"
  if (!fn.empty())  // master config given by user on commandline
# 128 "kind.ag"
    readMasterConfig1(fn, conf);
# 129 "kind.ag"
  else if (fileExists("/etc/kind/master.conf"))
# 130 "kind.ag"
    readMasterConfig1("/etc/kind/master.conf", conf);
# 131 "kind.ag"
  else if (fileExists("/ffp/etc/kind/master.conf"))
# 132 "kind.ag"
    readMasterConfig1("/ffp/etc/kind/master.conf", conf);
# 133 "kind.ag"
  else
# 134 "kind.ag"
    throw Exception("MasterConfig", "no file");
# 135 "kind.ag"
}
# 136 "kind.ag"

# 137 "kind.ag"
string findVault(const string& v)
# 138 "kind.ag"
{
# 139 "kind.ag"
  bool found = false;
# 140 "kind.ag"
  FileName fn;
# 141 "kind.ag"
  fn.setName(v);
# 142 "kind.ag"
  for (unsigned int i = 0; !found && i < banks.size(); ++i)
# 143 "kind.ag"
    {
# 144 "kind.ag"
      fn.setPath(banks[i]);
# 145 "kind.ag"
      if (dirExists(fn.getFileName()))
# 146 "kind.ag"
        found = true;
# 147 "kind.ag"
    }
# 148 "kind.ag"
  if (!found)
# 149 "kind.ag"
    throw Exception("find vault", v + " not found");
# 150 "kind.ag"
  verbosePrint("using vault " + fn.getFileName());
# 151 "kind.ag"
  return fn.getFileName();
# 152 "kind.ag"
}
# 153 "kind.ag"

# 154 "kind.ag"
void readVaultConfig(const string& vault, KindConfig& conf)
# 155 "kind.ag"
{
# 156 "kind.ag"
  string vaultpath = findVault(vault);
# 157 "kind.ag"
  const string& vaultConfigName = vaultpath + '/' + conf.getString("vaultConfigName");
# 158 "kind.ag"
  verbosePrint("reading vault config:");
# 159 "kind.ag"
  verbosePrint("  " + vaultConfigName);
# 160 "kind.ag"
  conf.addFile(vaultConfigName);
# 161 "kind.ag"
}
# 162 "kind.ag"

# 163 "kind.ag"
string getImageName(const KindConfig& conf,
# 164 "kind.ag"
                    const string& vaultPath,
# 165 "kind.ag"
                    const DateTime& imageTime)
# 166 "kind.ag"
{
# 167 "kind.ag"
  bool nonPortable = false;
# 168 "kind.ag"
  string imageName = conf.getString("imageName");
# 169 "kind.ag"
  for (unsigned int i = 0; !nonPortable && i < imageName.size(); ++i)
# 170 "kind.ag"
    {
# 171 "kind.ag"
      char c = imageName[i];
# 172 "kind.ag"
      if (!isalnum(c) && c != '.' && c != '_')
# 173 "kind.ag"
        nonPortable = true;
# 174 "kind.ag"
    }
# 175 "kind.ag"
  if (nonPortable)
# 176 "kind.ag"
    throw Exception("getImageName", "Invalid character in image name " + imageName);
# 177 "kind.ag"

# 178 "kind.ag"
  if (!imageName.empty())
# 179 "kind.ag"
    imageName += '-';
# 180 "kind.ag"

# 181 "kind.ag"
  string imageFullName =  vaultPath + "/" + imageName ;
# 182 "kind.ag"

# 183 "kind.ag"
  if (conf.getBool("longImageName"))
# 184 "kind.ag"
    imageFullName += imageTime.getString('m');
# 185 "kind.ag"
  else
# 186 "kind.ag"
    imageFullName += imageTime.getString('s');
# 187 "kind.ag"

# 188 "kind.ag"
  return imageFullName;
# 189 "kind.ag"
}
# 190 "kind.ag"

# 191 "kind.ag"
Images findImages(const string& vaultpath, const KindConfig& conf, bool all)
# 192 "kind.ag"
{
# 193 "kind.ag"
  Strings dirs;
# 194 "kind.ag"
  debugPrint("searching images in " + vaultpath);
# 195 "kind.ag"
  dirList(vaultpath, dirs);
# 196 "kind.ag"

# 197 "kind.ag"
  Images imageList;
# 198 "kind.ag"
  for (string dir : dirs)
# 199 "kind.ag"
    {
# 200 "kind.ag"
      FileName fn(dir);
# 201 "kind.ag"
      string imgname = conf.getString("imageName");
# 202 "kind.ag"
      if (startsWith(fn.getName(), imgname))
# 203 "kind.ag"
        {
# 204 "kind.ag"
          debugPrint("Checking " + dir);
# 205 "kind.ag"
          Image image(dir);
# 206 "kind.ag"

# 207 "kind.ag"
          if (all || image.valid)
# 208 "kind.ag"
            imageList.push_back(image);
# 209 "kind.ag"
        }
# 210 "kind.ag"
    }
# 211 "kind.ag"
  if (imageList.size() > 1)
# 212 "kind.ag"
    sort(imageList.begin(), imageList.end());
# 213 "kind.ag"
  return imageList;
# 214 "kind.ag"
}
# 215 "kind.ag"

# 216 "kind.ag"
void listImageInfo(const string& vault,
# 217 "kind.ag"
                   KindConfig conf /*Copy!*/ ,
# 218 "kind.ag"
                   const DateTime& imageTime,
# 219 "kind.ag"
                   const string& backupSet)
# 220 "kind.ag"
{
# 221 "kind.ag"
  readVaultConfig(vault, conf);
# 222 "kind.ag"
  string vaultPath = findVault(vault);
# 223 "kind.ag"
  Images imageList = findImages(vaultPath, conf, true);
# 224 "kind.ag"
  cout << "---" << endl;
# 225 "kind.ag"
  for (auto img : imageList)
# 226 "kind.ag"
    {
# 227 "kind.ag"
      if (img.series == backupSet || backupSet.empty())
# 228 "kind.ag"
        {
# 229 "kind.ag"
          img.printInfo();
# 230 "kind.ag"
          cout << "---" << endl;
# 231 "kind.ag"
        }
# 232 "kind.ag"
    }
# 233 "kind.ag"
}
# 234 "kind.ag"

# 235 "kind.ag"
void doBackup(const string& vault,
# 236 "kind.ag"
              const string& imageFullName,
# 237 "kind.ag"
              const string& referenceImage,
# 238 "kind.ag"
              const KindConfig& conf)
# 239 "kind.ag"
{
# 240 "kind.ag"
  // create image path
# 241 "kind.ag"
  if (!dryRun)
# 242 "kind.ag"
    if (mkdir(imageFullName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
# 243 "kind.ag"
      throw Exception("Create image", "failed to create " + imageFullName);
# 244 "kind.ag"

# 245 "kind.ag"
  // error message
# 246 "kind.ag"
  // we write an generic error message to mark backup as unsuccessful
# 247 "kind.ag"
  // will be deleted at successful end of rsync
# 248 "kind.ag"
  string errorfile = imageFullName + "/error";
# 249 "kind.ag"
  if (!dryRun)
# 250 "kind.ag"
    {
# 251 "kind.ag"
      ofstream error(errorfile);
# 252 "kind.ag"
      error << "failed" << endl;
# 253 "kind.ag"
      error.close();
# 254 "kind.ag"
    }
# 255 "kind.ag"

# 256 "kind.ag"
  // create source descriptor
# 257 "kind.ag"
  string host;
# 258 "kind.ag"
  if (conf.hasKey("host"))
# 259 "kind.ag"
    host = conf.getString("host");
# 260 "kind.ag"

# 261 "kind.ag"
  string server;
# 262 "kind.ag"
  if (conf.hasKey("server"))
# 263 "kind.ag"
    server = conf.getString("server");
# 264 "kind.ag"

# 265 "kind.ag"
  if (!host.empty() && !server.empty())
# 266 "kind.ag"
    throw Exception("backupVault", "Cannot have host and server");
# 267 "kind.ag"

# 268 "kind.ag"
  if (host.empty() && server.empty())
# 269 "kind.ag"
    throw Exception("backupVault", "No host or server specified");
# 270 "kind.ag"

# 271 "kind.ag"
  // ping host / server
# 272 "kind.ag"
  // ping -c 1 -W 5 -q $HOST
# 273 "kind.ag"
  string pingCommand = conf.getString("ping");
# 274 "kind.ag"
  debugPrint("PingCommand: " + pingCommand);
# 275 "kind.ag"
  if (!pingCommand.empty())
# 276 "kind.ag"
    {
# 277 "kind.ag"
      if (!host.empty())
# 278 "kind.ag"
        replacePlaceHolder(pingCommand, "%host", host);
# 279 "kind.ag"
      else
# 280 "kind.ag"
        replacePlaceHolder(pingCommand, "%host", server);
# 281 "kind.ag"
      int rc = 0;
# 282 "kind.ag"
      Strings pingResult = myPopen(pingCommand, rc, debug);
# 283 "kind.ag"
      if (rc != 0)
# 284 "kind.ag"
        {
# 285 "kind.ag"
          strings2File(pingResult, errorfile);
# 286 "kind.ag"
          throw Exception("Host not available", pingCommand);
# 287 "kind.ag"
        }
# 288 "kind.ag"
    }
# 289 "kind.ag"

# 290 "kind.ag"
  string path = conf.getString("path");
# 291 "kind.ag"
  if (path.empty())
# 292 "kind.ag"
    throw Exception("rsync", "empty source path");
# 293 "kind.ag"
  if (path.back() != '/')
# 294 "kind.ag"
    path += '/';
# 295 "kind.ag"

# 296 "kind.ag"
  string rsyncCmd = "rsync -vrltH --delete --stats -D --numeric-ids ";
# 297 "kind.ag"
  if (!conf.getBool("ignorePermission"))
# 298 "kind.ag"
    rsyncCmd += "-pgo";
# 299 "kind.ag"
  vector<string> rso = conf.getStrings("rsyncOption");
# 300 "kind.ag"
  for (const string& opt : rso)
# 301 "kind.ag"
    rsyncCmd += opt + " ";
# 302 "kind.ag"

# 303 "kind.ag"
  // excludes
# 304 "kind.ag"
  Strings excluded;
# 305 "kind.ag"

# 306 "kind.ag"
  if (conf.hasKey("exclude"))
# 307 "kind.ag"
    excluded += conf.getStrings("exclude");
# 308 "kind.ag"

# 309 "kind.ag"
  if (!host.empty())  // shell mode
# 310 "kind.ag"
    {
# 311 "kind.ag"
      // cout << "USING SHELLMODE '" << host << "'" << endl;
# 312 "kind.ag"
      string remoteShell = conf.getString("remoteShell");
# 313 "kind.ag"
      string userAtHost = conf.getString("user") + "@" +
# 314 "kind.ag"
      conf.getString("host");
# 315 "kind.ag"
      string rshCommand = remoteShell;
# 316 "kind.ag"
      if (remoteShell.empty())
# 317 "kind.ag"
        rshCommand = "ssh";
# 318 "kind.ag"

# 319 "kind.ag"
      rshCommand += " " + userAtHost;
# 320 "kind.ag"

# 321 "kind.ag"
      string userExcludeCommand = conf.getString("userExcludeCommand");
# 322 "kind.ag"

# 323 "kind.ag"
      if (!userExcludeCommand.empty())
# 324 "kind.ag"
        {
# 325 "kind.ag"
          replacePlaceHolder(userExcludeCommand, "%path", path);
# 326 "kind.ag"
          string excludeCommand = rshCommand + " " + userExcludeCommand;
# 327 "kind.ag"

# 328 "kind.ag"
          verbosePrint("searching for exclusions (" + excludeCommand + ")");
# 329 "kind.ag"

# 330 "kind.ag"
          int rc;
# 331 "kind.ag"
          Strings excludedFiles = myPopen(excludeCommand, rc, debug);
# 332 "kind.ag"
          if (rc > 0)
# 333 "kind.ag"
            {
# 334 "kind.ag"
              // return Strings should contain error messages
# 335 "kind.ag"
              strings2File(excludedFiles, errorfile);
# 336 "kind.ag"
              throw Exception("Find exludes", "Search for excludes failed");
# 337 "kind.ag"
            }
# 338 "kind.ag"

# 339 "kind.ag"
          for (unsigned int i = 0; i < excludedFiles.size(); ++i)
# 340 "kind.ag"
            {
# 341 "kind.ag"
              FileName fn(excludedFiles[i]);
# 342 "kind.ag"
              excluded.push_back('/' + fn.getPath());
# 343 "kind.ag"
              debugPrint("Excluding: " + excluded.back());
# 344 "kind.ag"
            }
# 345 "kind.ag"
        }
# 346 "kind.ag"

# 347 "kind.ag"
      string userExcludeFile = conf.getString("userExcludeFile");
# 348 "kind.ag"
      if (!userExcludeFile.empty())
# 349 "kind.ag"
        {
# 350 "kind.ag"
          userExcludeFile = path + userExcludeFile;
# 351 "kind.ag"
          string getExcludeFileCommand = rshCommand;
# 352 "kind.ag"
          getExcludeFileCommand += " \" if [ -f '" + userExcludeFile + "' ]; then ";
# 353 "kind.ag"
          getExcludeFileCommand += " cat '" + userExcludeFile + "' ; fi \"";
# 354 "kind.ag"
          // cout << getExcludeFileCommand << endl;
# 355 "kind.ag"
          int rc;
# 356 "kind.ag"
          Strings excludes2 = myPopen(getExcludeFileCommand, rc, debug);
# 357 "kind.ag"
          if (rc == 0)
# 358 "kind.ag"
            excluded += excludes2;
# 359 "kind.ag"
        }
# 360 "kind.ag"

# 361 "kind.ag"
      if (!dryRun)
# 362 "kind.ag"
        strings2File(excluded, imageFullName + "/exclude");
# 363 "kind.ag"

# 364 "kind.ag"
      // rsync image
# 365 "kind.ag"

# 366 "kind.ag"
      if (!remoteShell.empty())
# 367 "kind.ag"
        rsyncCmd += " -e \'" + remoteShell + "\' ";
# 368 "kind.ag"

# 369 "kind.ag"
      rsyncCmd += "--exclude-from=" + imageFullName + "/exclude ";
# 370 "kind.ag"
      if (!referenceImage.empty())
# 371 "kind.ag"
        rsyncCmd += "--link-dest=" + referenceImage + "/tree ";
# 372 "kind.ag"
      rsyncCmd += userAtHost + ":" + path + " ";
# 373 "kind.ag"
      rsyncCmd += imageFullName + "/tree";
# 374 "kind.ag"
    } // shell mode
# 375 "kind.ag"
  else
# 376 "kind.ag"
    {
# 377 "kind.ag"
      // cout << "USING SERVERMODE" << endl;
# 378 "kind.ag"
      // we cannot use find without shell access
# 379 "kind.ag"
      // and do not read an exclude file on client side
# 380 "kind.ag"

# 381 "kind.ag"
      if (!dryRun)
# 382 "kind.ag"
        strings2File(excluded, imageFullName + "/exclude");
# 383 "kind.ag"

# 384 "kind.ag"
      rsyncCmd += "--exclude-from=" + imageFullName + "/exclude ";
# 385 "kind.ag"
      if (!referenceImage.empty())
# 386 "kind.ag"
        rsyncCmd += "--link-dest=" + referenceImage + "/tree ";
# 387 "kind.ag"
      rsyncCmd += conf.getString("server") + "::" + path + " ";
# 388 "kind.ag"
      rsyncCmd += imageFullName + "/tree";
# 389 "kind.ag"
    }
# 390 "kind.ag"

# 391 "kind.ag"
  debugPrint("Action: " + rsyncCmd);
# 392 "kind.ag"

# 393 "kind.ag"
  vector<string> backupResult;
# 394 "kind.ag"
  if (!dryRun)
# 395 "kind.ag"
    {
# 396 "kind.ag"
      verbosePrint("syncing (" + rsyncCmd + ")");
# 397 "kind.ag"
      int rc;
# 398 "kind.ag"
      backupResult = myPopen(rsyncCmd, rc, debug, imageFullName + "/rsync-log");
# 399 "kind.ag"
      if (rc == 0 ||
# 400 "kind.ag"
      rc == 24 || // "no error" or "vanished source files" (ignored)
# 401 "kind.ag"
      rc == 6144) // workaround for wrong exit code ??!!
# 402 "kind.ag"
        {
# 403 "kind.ag"
          unlink(errorfile.c_str());
# 404 "kind.ag"
          long int st = 0;
# 405 "kind.ag"
          long int sc = 0;
# 406 "kind.ag"
          for (auto bl : backupResult)
# 407 "kind.ag"
            {
# 408 "kind.ag"
              if (startsWith(bl, "Total file size"))
# 409 "kind.ag"
                st = getNumber(bl);
# 410 "kind.ag"
              else if (startsWith(bl, "Total transferred file size"))
# 411 "kind.ag"
                sc = getNumber(bl);
# 412 "kind.ag"
            }
# 413 "kind.ag"
          // sizes[vault] = pair<long int, long int>(st, sc);
# 414 "kind.ag"
          sizes[vault] = Sizes(st, sc);
# 415 "kind.ag"
          //  cout << vault << " " << st << " || " << sc << endl;
# 416 "kind.ag"
        }
# 417 "kind.ag"
      else
# 418 "kind.ag"
        throw Exception("Backup", "Failed to execute rsync (result: " + to_string(rc) + ")");
# 419 "kind.ag"
    }
# 420 "kind.ag"
  else
# 421 "kind.ag"
    cout << "Not executing " << rsyncCmd << endl;
# 422 "kind.ag"
}
# 423 "kind.ag"

# 424 "kind.ag"
bool backupVault(const string& vault,
# 425 "kind.ag"
                 KindConfig conf /*Copy!*/ ,
# 426 "kind.ag"
                 const DateTime& imageTime,
# 427 "kind.ag"
                 bool fullImage,
# 428 "kind.ag"
                 const string& forcedBackupSet)
# 429 "kind.ag"
{
# 430 "kind.ag"
  if (!quiet)
# 431 "kind.ag"
    cout << DateTime::now().getString('h') << ": Backup of vault " << vault << endl;
# 432 "kind.ag"
  try
# 433 "kind.ag"
    {
# 434 "kind.ag"
      readVaultConfig(vault, conf);
# 435 "kind.ag"

# 436 "kind.ag"
      // where to store
# 437 "kind.ag"
      string vaultPath = findVault(vault);
# 438 "kind.ag"

# 439 "kind.ag"
      // image path
# 440 "kind.ag"
      string imageFullName = getImageName(conf, vaultPath, imageTime);
# 441 "kind.ag"

# 442 "kind.ag"
      bool backupNow = true;
# 443 "kind.ag"

# 444 "kind.ag"
      // existing images
# 445 "kind.ag"
      Images validImageList = findImages(vaultPath, conf, false);
# 446 "kind.ag"
      string currentSet = "expire"; // we are not using backupSets
# 447 "kind.ag"

# 448 "kind.ag"
      // check if we are using backup sets
# 449 "kind.ag"

# 450 "kind.ag"
      map<string, int> setIdx;
# 451 "kind.ag"
      vector<SetRule> backupSetRule;
# 452 "kind.ag"
      int setRuleIdx = -1;
# 453 "kind.ag"

# 454 "kind.ag"
      if (conf.hasKey("setRule"))
# 455 "kind.ag"
        {
# 456 "kind.ag"
          readSetRules(conf, setIdx, backupSetRule);
# 457 "kind.ag"
          if (!setIdx.empty())
# 458 "kind.ag"
            {
# 459 "kind.ag"
              if (forcedBackupSet.empty())
# 460 "kind.ag"
                {
# 461 "kind.ag"
                  backupNow = false;
# 462 "kind.ag"

# 463 "kind.ag"
                  // find time for nextBackup for every backupSet
# 464 "kind.ag"
                  // defaults to now == imageTime;
# 465 "kind.ag"
                  vector<DateTime> nextBackup(backupSetRule.size(), imageTime);
# 466 "kind.ag"

# 467 "kind.ag"
                  // find time for next backup
# 468 "kind.ag"

# 469 "kind.ag"
                  for (const Image& image : validImageList)
# 470 "kind.ag"
                    {
# 471 "kind.ag"
                      if (image.series != "expire")
# 472 "kind.ag"
                        {
# 473 "kind.ag"
                          string s = image.series;
# 474 "kind.ag"
                          if (setIdx.count(s) > 0) // rule for set exists?
# 475 "kind.ag"
                            {
# 476 "kind.ag"
                              int rIdx = setIdx[s];
# 477 "kind.ag"
                              // image is valid for this and "lower level" backupSets
# 478 "kind.ag"
                              for (unsigned int i = rIdx; i < backupSetRule.size(); ++i)
# 479 "kind.ag"
                                if (nextBackup[i] < image.time + backupSetRule[i].distance)
# 480 "kind.ag"
                                  nextBackup[i] =  image.time + backupSetRule[i].distance;
# 481 "kind.ag"
                            }
# 482 "kind.ag"
                        }
# 483 "kind.ag"
                    }
# 484 "kind.ag"
                  if (debug)
# 485 "kind.ag"
                    for (unsigned int i = 0; i < backupSetRule.size(); ++i)
# 486 "kind.ag"
                      cout << "       Next backup for " << backupSetRule[i].name << " at " <<  nextBackup[i].getString('h') << endl;
# 487 "kind.ag"

# 488 "kind.ag"
                  // find backupSet that
# 489 "kind.ag"
                  //    - needs backup
# 490 "kind.ag"
                  //    - has longest time to keep
# 491 "kind.ag"
                  // because of ordered list backupSetRule this is the first set, that need
# 492 "kind.ag"

# 493 "kind.ag"
                  currentSet = "";
# 494 "kind.ag"
                  for (unsigned int i = 0; i < backupSetRule.size() && currentSet.empty(); ++i)
# 495 "kind.ag"
                    {
# 496 "kind.ag"
                      string name = backupSetRule[i].name;
# 497 "kind.ag"
                      if (nextBackup[i] <= imageTime + 5) // small offset of 5s for "jitter"
# 498 "kind.ag"
                        {
# 499 "kind.ag"
                          backupNow = true;
# 500 "kind.ag"
                          currentSet = name;
# 501 "kind.ag"
                          setRuleIdx = i;
# 502 "kind.ag"
                        }
# 503 "kind.ag"
                    }
# 504 "kind.ag"
                }
# 505 "kind.ag"
              else
# 506 "kind.ag"
                {
# 507 "kind.ag"
                  if (setIdx.count(forcedBackupSet) > 0)
# 508 "kind.ag"
                    {
# 509 "kind.ag"
                      currentSet = forcedBackupSet;
# 510 "kind.ag"
                      setRuleIdx = setIdx[forcedBackupSet];
# 511 "kind.ag"
                    }
# 512 "kind.ag"
                  else
# 513 "kind.ag"
                    throw Exception("force backup of set " + forcedBackupSet, " set not exists");
# 514 "kind.ag"
                }
# 515 "kind.ag"
            } // if (!setIdx.empty())
# 516 "kind.ag"
        } // (conf.hasKey("setRule"))
# 517 "kind.ag"

# 518 "kind.ag"
      if (backupNow)
# 519 "kind.ag"
        {
# 520 "kind.ag"
          verbosePrint("backup to \"" + imageFullName + "\"");
# 521 "kind.ag"
          if (setRuleIdx >= 0 && !quiet)
# 522 "kind.ag"
            cout << "  backup set is \"" << currentSet << "\"" << endl;
# 523 "kind.ag"
        }
# 524 "kind.ag"
      else if (!quiet)
# 525 "kind.ag"
        cout << "  no backup set needs update" << endl;
# 526 "kind.ag"

# 527 "kind.ag"
      if (backupNow)
# 528 "kind.ag"
        {
# 529 "kind.ag"
          // find reference image
# 530 "kind.ag"
          string referenceImage;
# 531 "kind.ag"
          if (!fullImage)
# 532 "kind.ag"
            {
# 533 "kind.ag"
              if (validImageList.empty())
# 534 "kind.ag"
                throw Exception("backupVault", "no reference image found");
# 535 "kind.ag"
              // last image is newest image
# 536 "kind.ag"
              referenceImage = validImageList.back().name;
# 537 "kind.ag"
            }
# 538 "kind.ag"

# 539 "kind.ag"
          doBackup(vault, imageFullName, referenceImage, conf);
# 540 "kind.ag"

# 541 "kind.ag"
          if (!dryRun)
# 542 "kind.ag"
            {
# 543 "kind.ag"
              string lastPath = vaultPath + "/last";
# 544 "kind.ag"
              struct stat fstat;
# 545 "kind.ag"

# 546 "kind.ag"
              // remove last (dir or symlink)
# 547 "kind.ag"
              if (lstat(lastPath.c_str(), &fstat) == 0) // last exists
# 548 "kind.ag"
                {
# 549 "kind.ag"
                  if (S_ISDIR(fstat.st_mode))
# 550 "kind.ag"
                    removeDir(lastPath);
# 551 "kind.ag"
                  else
# 552 "kind.ag"
                    unlink(lastPath.c_str());
# 553 "kind.ag"
                }
# 554 "kind.ag"

# 555 "kind.ag"
              string linkType = conf.getString("lastLink");
# 556 "kind.ag"
              if (linkType == "hardLink")
# 557 "kind.ag"
                {
# 558 "kind.ag"
                  int rc;
# 559 "kind.ag"
                  string hardLinkCommand = "cp -al " + imageFullName + " " + lastPath;
# 560 "kind.ag"
                  Strings res = myPopen(hardLinkCommand, rc, debug);
# 561 "kind.ag"
                }
# 562 "kind.ag"
              else if (linkType == "symLink")
# 563 "kind.ag"
                {
# 564 "kind.ag"
                  // set symlink to last image
# 565 "kind.ag"
                  symlink(imageFullName.c_str(), lastPath.c_str());
# 566 "kind.ag"
                }
# 567 "kind.ag"
              else if (linkType != "null")
# 568 "kind.ag"
                cerr << "invalid Value in \"lastLink\"" << endl;
# 569 "kind.ag"

# 570 "kind.ag"
              // write expire date to file
# 571 "kind.ag"
              DateTime expireTime;
# 572 "kind.ag"
              string rule;
# 573 "kind.ag"
              if (setRuleIdx < 0) // not backup set based
# 574 "kind.ag"
                expireTime = getExpireDate(imageTime, conf, rule);
# 575 "kind.ag"
              else
# 576 "kind.ag"
                {
# 577 "kind.ag"
                  expireTime = imageTime + backupSetRule[setRuleIdx].keep;
# 578 "kind.ag"
                  rule =  backupSetRule[setRuleIdx].rule;
# 579 "kind.ag"
                }
# 580 "kind.ag"

# 581 "kind.ag"
              ofstream expireFile(imageFullName + "/expires");
# 582 "kind.ag"
              expireFile << currentSet << "-" << expireTime.getString('m') << endl;
# 583 "kind.ag"
              expireFile << rule << endl;
# 584 "kind.ag"
            }
# 585 "kind.ag"
        }
# 586 "kind.ag"
      return backupNow;
# 587 "kind.ag"
    }
# 588 "kind.ag"
  catch (Exception ex)
# 589 "kind.ag"
    {
# 590 "kind.ag"
      cerr << "Exception in vault " << vault << ": " << ex.what() << endl;
# 591 "kind.ag"
      return false;
# 592 "kind.ag"
    }
# 593 "kind.ag"
}
# 594 "kind.ag"

# 595 "kind.ag"
void expireVault(const string& vault, KindConfig conf, DateTime now)
# 596 "kind.ag"
{
# 597 "kind.ag"
  if (!quiet)
# 598 "kind.ag"
    cout << DateTime::now().getString('h') << ": Expiring images in vault " << vault << endl;
# 599 "kind.ag"

# 600 "kind.ag"
  readVaultConfig(vault, conf);
# 601 "kind.ag"

# 602 "kind.ag"
  string vaultpath = findVault(vault);
# 603 "kind.ag"

# 604 "kind.ag"
  Images imagelist = findImages(vaultpath, conf, true);
# 605 "kind.ag"

# 606 "kind.ag"
  string lastValidImage;
# 607 "kind.ag"
  for (Image image : imagelist)
# 608 "kind.ag"
    {
# 609 "kind.ag"
      if (image.valid)
# 610 "kind.ag"
        lastValidImage = image.name;
# 611 "kind.ag"
    }
# 612 "kind.ag"

# 613 "kind.ag"
  for (Image image : imagelist)
# 614 "kind.ag"
    {
# 615 "kind.ag"
      if (debug)
# 616 "kind.ag"
        {
# 617 "kind.ag"
          image.printInfo();
# 618 "kind.ag"
        }
# 619 "kind.ag"

# 620 "kind.ag"
      DateTime imageTime = image.time;
# 621 "kind.ag"

# 622 "kind.ag"
      if (imageTime != now &&          // ignore just created image
# 623 "kind.ag"
      image.name != lastValidImage // ignore last valid image
# 624 "kind.ag"
         )
# 625 "kind.ag"
        {
# 626 "kind.ag"
          DateTime expireTime;
# 627 "kind.ag"
          string expireRule;
# 628 "kind.ag"
          if (!image.valid) // invalid image?
# 629 "kind.ag"
            {
# 630 "kind.ag"
              time_t expPeriod = stot(conf.getString("expireFailedImage"));
# 631 "kind.ag"
              if (expPeriod < 0)
# 632 "kind.ag"
                throw Exception("expireFailedImage", "Time period must be positive");
# 633 "kind.ag"
              expireTime = imageTime + stot(conf.getString("expireFailedImage"));
# 634 "kind.ag"
              expireRule = "invalid image: " + conf.getString("expireFailedImage");
# 635 "kind.ag"
              debugPrint("- invalid image");
# 636 "kind.ag"
            }
# 637 "kind.ag"
          else
# 638 "kind.ag"
            {
# 639 "kind.ag"
              debugPrint("- valid image");
# 640 "kind.ag"
              expireTime = image.expire;
# 641 "kind.ag"
              expireRule = image.expireRule;
# 642 "kind.ag"
            }
# 643 "kind.ag"

# 644 "kind.ag"
          if (expireTime < now)
# 645 "kind.ag"
            {
# 646 "kind.ag"
              if (!quiet)
# 647 "kind.ag"
                cout << "  removing image " << image.name << endl;
# 648 "kind.ag"
              try
# 649 "kind.ag"
                {
# 650 "kind.ag"
                  if (removeDir(image.name) != 0)
# 651 "kind.ag"
                    cout << "Error removing " <<  image.name << endl;
# 652 "kind.ag"
                }
# 653 "kind.ag"
              catch (Exception ex)
# 654 "kind.ag"
                {
# 655 "kind.ag"
                  cerr << "Exception: " << ex.what() << endl;
# 656 "kind.ag"
                }
# 657 "kind.ag"
            }
# 658 "kind.ag"
        }
# 659 "kind.ag"
      else
# 660 "kind.ag"
        debugPrint("- current image - ignored");
# 661 "kind.ag"
    }
# 662 "kind.ag"
}
# 663 "kind.ag"

# 664 "kind.ag"
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
  cout << "  -f        --full=   " << endl;
  cout << "     Force full image == initial backup (default: false)" << endl;

  cout << "  -B        --backup=   " << endl;
  cout << "     Backup (default: false)" << endl;

  cout << "  -E        --expire=   " << endl;
  cout << "     Expire (default: false)" << endl;

  cout << "  -C        --listconfig=   " << endl;
  cout << "     Show configuration (default: false)" << endl;

  cout << "  -I        --listimages=   " << endl;
  cout << "     List data of images (default: false)" << endl;

  cout << "     if none of backup, expire, listconfig and listimages is specified," << endl;
  cout << "     backup and expire is assumed." << endl;
  cout << "     listconfig and listimages cannot be combined with other actions" << endl;
  cout << "  -D        --dryrun=   " << endl;
  cout << "     Dry run (no real backup) (default: false)" << endl;

  cout << "  -F <s>    --forcebackup=<s>" << endl;
  cout << "     Create image for specified backup set (default: \"\")" << endl;

  cout << "  -v        --verbose=   " << endl;
  cout << "     Verbose (default: false)" << endl;

  cout << "  -d        --debug=   " << endl;
  cout << "     Debug output of many data (default: false)" << endl;

  cout << "  -q        --quiet=   " << endl;
  cout << "     Be quiet - no messages (default: false)" << endl;

  cout << "  -h        --help=   " << endl;
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
# 668 "kind.ag"

# 669 "kind.ag"
  int exitCode = 0;
# 670 "kind.ag"
  string lockFile;
# 671 "kind.ag"
  try
# 672 "kind.ag"
    {
# 673 "kind.ag"
      // handling of parameters and switches
# 674 "kind.ag"
      if (debug)        // debug implies verbose
# 675 "kind.ag"
        verbose = true;
# 676 "kind.ag"

# 677 "kind.ag"
      if (!doBackup && !doExpire && !listConfig && !listImages)
# 678 "kind.ag"
        {
# 679 "kind.ag"
          doBackup = true;
# 680 "kind.ag"
          doExpire = true;
# 681 "kind.ag"
        }
# 682 "kind.ag"

# 683 "kind.ag"
      KindConfig conf;
# 684 "kind.ag"

# 685 "kind.ag"
      // default-values
# 686 "kind.ag"
      conf.add("imageName", "image");
# 687 "kind.ag"
      conf.add("vaultConfigName", "kind/vault.conf");
# 688 "kind.ag"
      conf.add("expireFailedImage", "3 days");
# 689 "kind.ag"
      conf.add("expireRule", "* * * * 1 month");
# 690 "kind.ag"
      conf.add("ping", "ping -c 1 -W 5 %host");
# 691 "kind.ag"
      conf.add("rsyncOption", ""); // no additional rsync option
# 692 "kind.ag"
      conf.add("remoteShell", "");
# 693 "kind.ag"
      conf.add("lockfile", "/var/lock/kind");
# 694 "kind.ag"
      conf.add("userExcludeFile", "nobackup.list");
# 695 "kind.ag"
      conf.add("userExcludeCommand",
# 696 "kind.ag"
               "find %path -type f -iname '*nobackup' -printf '%P\\\\n'");
# 697 "kind.ag"
      conf.add("logSize", "");
# 698 "kind.ag"
      conf.add("lastLink", "symLink");
# 699 "kind.ag"

# 700 "kind.ag"
      if (listConfig)
# 701 "kind.ag"
        {
# 702 "kind.ag"
          cout << "builtin config" << endl;
# 703 "kind.ag"
          conf.print(".   ");
# 704 "kind.ag"
        }
# 705 "kind.ag"

# 706 "kind.ag"
      readMasterConfig(masterConfig, conf);
# 707 "kind.ag"

# 708 "kind.ag"
      banks = conf.getStrings("bank");
# 709 "kind.ag"
      if (banks.empty())
# 710 "kind.ag"
        throw Exception("read master configuration", "no banks defined");
# 711 "kind.ag"

# 712 "kind.ag"
      if (listConfig)
# 713 "kind.ag"
        {
# 714 "kind.ag"
          cout << "global config:" << endl;
# 715 "kind.ag"
          conf.print(".   ");
# 716 "kind.ag"
          readVaultConfig(vault, conf);
# 717 "kind.ag"
          cout << "vault config:" << endl;
# 718 "kind.ag"
          conf.print(".   ");
# 719 "kind.ag"
          exit(0);
# 720 "kind.ag"
        }
# 721 "kind.ag"

# 722 "kind.ag"
      DateTime imageTime = DateTime::now();
# 723 "kind.ag"

# 724 "kind.ag"
      if (listImages)
# 725 "kind.ag"
        {
# 726 "kind.ag"
          listImageInfo(vault, conf, imageTime, forcedBackupSet);
# 727 "kind.ag"
          exit(0);
# 728 "kind.ag"
        }
# 729 "kind.ag"

# 730 "kind.ag"
      lockFile = conf.getString("lockfile");
# 731 "kind.ag"
      createLock(lockFile);
# 732 "kind.ag"

# 733 "kind.ag"
      string logSizeFile = conf.getString("logSize");
# 734 "kind.ag"
      readSizes(logSizeFile);
# 735 "kind.ag"

# 736 "kind.ag"
      vector<string> vaults;
# 737 "kind.ag"
      string groupname = "group_" + vault;
# 738 "kind.ag"
      if (conf.hasKey(groupname))
# 739 "kind.ag"
        vaults = conf.getStrings(groupname);
# 740 "kind.ag"
      else
# 741 "kind.ag"
        vaults.push_back(vault);
# 742 "kind.ag"

# 743 "kind.ag"

# 744 "kind.ag"
      for (string vault : vaults)
# 745 "kind.ag"
        {
# 746 "kind.ag"
          if (doBackup)
# 747 "kind.ag"
            if (backupVault(vault, conf, imageTime, fullImage, forcedBackupSet))
# 748 "kind.ag"
              writeSizes(logSizeFile);
# 749 "kind.ag"
          if (doExpire)
# 750 "kind.ag"
            expireVault(vault, conf, imageTime);
# 751 "kind.ag"
        }
# 752 "kind.ag"

# 753 "kind.ag"
      if (!quiet)
# 754 "kind.ag"
        cout << DateTime::now().getString('h') << ": finished" << endl;
# 755 "kind.ag"

# 756 "kind.ag"
    }
# 757 "kind.ag"
  catch (const Exception& ex)
# 758 "kind.ag"
    {
# 759 "kind.ag"
      cerr << "Exception: " << ex.what() << endl;
# 760 "kind.ag"
      exitCode = 1;
# 761 "kind.ag"
    }
# 762 "kind.ag"
  catch (const char* msg)
# 763 "kind.ag"
    {
# 764 "kind.ag"
      cerr << "Exception(char*): " << msg << endl;
# 765 "kind.ag"
      exitCode = 1;
# 766 "kind.ag"
    }
# 767 "kind.ag"
  catch (const string& msg)
# 768 "kind.ag"
    {
# 769 "kind.ag"
      cerr << "Exception(string): " << msg << endl;
# 770 "kind.ag"
      exitCode = 1;
# 771 "kind.ag"
    }
# 772 "kind.ag"
  removeLock(lockFile);
# 773 "kind.ag"
  return exitCode;
# 774 "kind.ag"
}
