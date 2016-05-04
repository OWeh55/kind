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
typedef pair<long int, long int> Sizes;
# 65 "kind.ag"
map<string, Sizes> sizes;
# 66 "kind.ag"

# 67 "kind.ag"
void readSizes(const string& logSizeFile)
# 68 "kind.ag"
{
# 69 "kind.ag"
  if (fileExists(logSizeFile))
# 70 "kind.ag"
    {
# 71 "kind.ag"
      vector<string> ss;
# 72 "kind.ag"
      file2Strings(logSizeFile, ss);
# 73 "kind.ag"
      for (const string& s : ss)
# 74 "kind.ag"
        {
# 75 "kind.ag"
          unsigned int i = 0;
# 76 "kind.ag"
          string v = getWord(s, i);
# 77 "kind.ag"
          long int s1 = getLongInt(s, i);
# 78 "kind.ag"
          long int s2 = getLongInt(s, i);
# 79 "kind.ag"
          sizes[v] = Sizes(s1, s2);
# 80 "kind.ag"
        }
# 81 "kind.ag"
    }
# 82 "kind.ag"
}
# 83 "kind.ag"

# 84 "kind.ag"
void writeSizes(const string logSizeFile)
# 85 "kind.ag"
{
# 86 "kind.ag"
  if (!logSizeFile.empty())
# 87 "kind.ag"
    {
# 88 "kind.ag"
      Strings st;
# 89 "kind.ag"
      for (auto s : sizes)
# 90 "kind.ag"
        {
# 91 "kind.ag"
          string h = s.first + " " + to_string(s.second.first) + " " + to_string(s.second.second);
# 92 "kind.ag"
          st.push_back(h);
# 93 "kind.ag"
        }
# 94 "kind.ag"
      strings2File(st, logSizeFile);
# 95 "kind.ag"
    }
# 96 "kind.ag"
}
# 97 "kind.ag"

# 98 "kind.ag"
void verbosePrint(const string& text)
# 99 "kind.ag"
{
# 100 "kind.ag"
  if (verbose)
# 101 "kind.ag"
    cout << "  " << text << endl;
# 102 "kind.ag"
}
# 103 "kind.ag"

# 104 "kind.ag"
void debugPrint(const string& text)
# 105 "kind.ag"
{
# 106 "kind.ag"
  if (debug)
# 107 "kind.ag"
    cout << "    " << text << endl;
# 108 "kind.ag"
}
# 109 "kind.ag"

# 110 "kind.ag"
void readMasterConfig1(const string& fn, KindConfig& conf)
# 111 "kind.ag"
{
# 112 "kind.ag"
  verbosePrint("reading master config " + fn);
# 113 "kind.ag"
  conf.addFile(fn);
# 114 "kind.ag"
}
# 115 "kind.ag"

# 116 "kind.ag"
void readMasterConfig(const string& fn, KindConfig& conf)
# 117 "kind.ag"
{
# 118 "kind.ag"
  if (!fn.empty())  // master config given by user on commandline
# 119 "kind.ag"
    readMasterConfig1(fn, conf);
# 120 "kind.ag"
  else if (fileExists("/etc/kind/master.conf"))
# 121 "kind.ag"
    readMasterConfig1("/etc/kind/master.conf", conf);
# 122 "kind.ag"
  else if (fileExists("/ffp/etc/kind/master.conf"))
# 123 "kind.ag"
    readMasterConfig1("/ffp/etc/kind/master.conf", conf);
# 124 "kind.ag"
  else
# 125 "kind.ag"
    throw Exception("MasterConfig", "no file");
# 126 "kind.ag"
}
# 127 "kind.ag"

# 128 "kind.ag"
string findVault(const string& v)
# 129 "kind.ag"
{
# 130 "kind.ag"
  bool found = false;
# 131 "kind.ag"
  FileName fn;
# 132 "kind.ag"
  fn.setName(v);
# 133 "kind.ag"
  for (unsigned int i = 0; !found && i < banks.size(); ++i)
# 134 "kind.ag"
    {
# 135 "kind.ag"
      fn.setPath(banks[i]);
# 136 "kind.ag"
      if (dirExists(fn.getFileName()))
# 137 "kind.ag"
        found = true;
# 138 "kind.ag"
    }
# 139 "kind.ag"
  if (!found)
# 140 "kind.ag"
    throw Exception("find vault", v + " not found");
# 141 "kind.ag"
  verbosePrint("using vault " + fn.getFileName());
# 142 "kind.ag"
  return fn.getFileName();
# 143 "kind.ag"
}
# 144 "kind.ag"

# 145 "kind.ag"
void readVaultConfig(const string& vault, KindConfig& conf)
# 146 "kind.ag"
{
# 147 "kind.ag"
  string vaultpath = findVault(vault);
# 148 "kind.ag"
  const string& vaultConfigName = vaultpath + '/' + conf.getString("vaultConfigName");
# 149 "kind.ag"
  verbosePrint("reading vault config:");
# 150 "kind.ag"
  verbosePrint("  " + vaultConfigName);
# 151 "kind.ag"
  conf.addFile(vaultConfigName);
# 152 "kind.ag"
}
# 153 "kind.ag"

# 154 "kind.ag"
string getImageName(const KindConfig& conf,
# 155 "kind.ag"
                    const string& vaultPath,
# 156 "kind.ag"
                    const DateTime& imageTime)
# 157 "kind.ag"
{
# 158 "kind.ag"
  bool nonPortable = false;
# 159 "kind.ag"
  string imageName = conf.getString("imageName");
# 160 "kind.ag"
  for (unsigned int i = 0; !nonPortable && i < imageName.size(); ++i)
# 161 "kind.ag"
    {
# 162 "kind.ag"
      char c = imageName[i];
# 163 "kind.ag"
      if (!isalnum(c) && c != '.' && c != '_')
# 164 "kind.ag"
        nonPortable = true;
# 165 "kind.ag"
    }
# 166 "kind.ag"
  if (nonPortable)
# 167 "kind.ag"
    throw Exception("getImageName", "Invalid character in image name " + imageName);
# 168 "kind.ag"

# 169 "kind.ag"
  if (!imageName.empty())
# 170 "kind.ag"
    imageName += '-';
# 171 "kind.ag"

# 172 "kind.ag"
  string imageFullName =  vaultPath + "/" + imageName ;
# 173 "kind.ag"

# 174 "kind.ag"
  if (conf.getBool("longImageName"))
# 175 "kind.ag"
    imageFullName += imageTime.getString('m');
# 176 "kind.ag"
  else
# 177 "kind.ag"
    imageFullName += imageTime.getString('s');
# 178 "kind.ag"

# 179 "kind.ag"
  return imageFullName;
# 180 "kind.ag"
}
# 181 "kind.ag"

# 182 "kind.ag"
Images findImages(const string& vaultpath, const KindConfig& conf, bool all)
# 183 "kind.ag"
{
# 184 "kind.ag"
  Strings dirs;
# 185 "kind.ag"
  debugPrint("searching images in " + vaultpath);
# 186 "kind.ag"
  dirList(vaultpath, dirs);
# 187 "kind.ag"

# 188 "kind.ag"
  Images imageList;
# 189 "kind.ag"
  for (string dir : dirs)
# 190 "kind.ag"
    {
# 191 "kind.ag"
      FileName fn(dir);
# 192 "kind.ag"
      string imgname = conf.getString("imageName");
# 193 "kind.ag"
      if (startsWith(fn.getName(), imgname))
# 194 "kind.ag"
        {
# 195 "kind.ag"
          debugPrint("Checking " + dir);
# 196 "kind.ag"
          Image image(dir);
# 197 "kind.ag"

# 198 "kind.ag"
          if (all || image.valid)
# 199 "kind.ag"
            imageList.push_back(image);
# 200 "kind.ag"
        }
# 201 "kind.ag"
    }
# 202 "kind.ag"
  if (imageList.size() > 1)
# 203 "kind.ag"
    sort(imageList.begin(), imageList.end());
# 204 "kind.ag"
  return imageList;
# 205 "kind.ag"
}
# 206 "kind.ag"

# 207 "kind.ag"
void listImageInfo(const string& vault,
# 208 "kind.ag"
                   KindConfig conf /*Copy!*/ ,
# 209 "kind.ag"
                   const DateTime& imageTime,
# 210 "kind.ag"
                   const string& backupSet)
# 211 "kind.ag"
{
# 212 "kind.ag"
  readVaultConfig(vault, conf);
# 213 "kind.ag"
  string vaultPath = findVault(vault);
# 214 "kind.ag"
  Images imageList = findImages(vaultPath, conf, true);
# 215 "kind.ag"
  for (auto img : imageList)
# 216 "kind.ag"
    {
# 217 "kind.ag"
      if (img.series == backupSet || backupSet.empty())
# 218 "kind.ag"
        {
# 219 "kind.ag"
          cout << img.name << endl;
# 220 "kind.ag"
          if (img.valid)
# 221 "kind.ag"
            {
# 222 "kind.ag"
              if (img.series != "expire")
# 223 "kind.ag"
                cout << "backup set: " << img.series << endl;
# 224 "kind.ag"
              cout << "created: " << img.time.getString() << endl;
# 225 "kind.ag"
              cout << "expires: " << img.expire.getString() << " -  +" << timeString(img.expire - imageTime) << endl;
# 226 "kind.ag"
            }
# 227 "kind.ag"

# 228 "kind.ag"
          else
# 229 "kind.ag"
            cout << "invalid" << endl;
# 230 "kind.ag"
        }
# 231 "kind.ag"
    }
# 232 "kind.ag"
}
# 233 "kind.ag"

# 234 "kind.ag"
void doBackup(const string& vault,
# 235 "kind.ag"
              const string& imageFullName,
# 236 "kind.ag"
              const string& referenceImage,
# 237 "kind.ag"
              const KindConfig& conf)
# 238 "kind.ag"
{
# 239 "kind.ag"
  // create image path
# 240 "kind.ag"
  if (!dryRun)
# 241 "kind.ag"
    if (mkdir(imageFullName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
# 242 "kind.ag"
      throw Exception("Create image", "failed to create " + imageFullName);
# 243 "kind.ag"

# 244 "kind.ag"
  // error message
# 245 "kind.ag"
  // we write an generic error message to mark backup as unsuccessful
# 246 "kind.ag"
  // will be deleted at successful end of rsync
# 247 "kind.ag"
  string errorfile = imageFullName + "/error";
# 248 "kind.ag"
  if (!dryRun)
# 249 "kind.ag"
    {
# 250 "kind.ag"
      ofstream error(errorfile);
# 251 "kind.ag"
      error << "failed" << endl;
# 252 "kind.ag"
      error.close();
# 253 "kind.ag"
    }
# 254 "kind.ag"

# 255 "kind.ag"
  // create source descriptor
# 256 "kind.ag"
  string host;
# 257 "kind.ag"
  if (conf.hasKey("host"))
# 258 "kind.ag"
    host = conf.getString("host");
# 259 "kind.ag"

# 260 "kind.ag"
  string server;
# 261 "kind.ag"
  if (conf.hasKey("server"))
# 262 "kind.ag"
    server = conf.getString("server");
# 263 "kind.ag"

# 264 "kind.ag"
  if (!host.empty() && !server.empty())
# 265 "kind.ag"
    throw Exception("backupVault", "Cannot have host and server");
# 266 "kind.ag"

# 267 "kind.ag"
  if (host.empty() && server.empty())
# 268 "kind.ag"
    throw Exception("backupVault", "No host or server specified");
# 269 "kind.ag"

# 270 "kind.ag"
  // ping host / server
# 271 "kind.ag"
  // ping -c 1 -W 5 -q $HOST
# 272 "kind.ag"
  string pingCommand = conf.getString("ping");
# 273 "kind.ag"
  debugPrint("PingCommand: " + pingCommand);
# 274 "kind.ag"
  if (!pingCommand.empty())
# 275 "kind.ag"
    {
# 276 "kind.ag"
      if (!host.empty())
# 277 "kind.ag"
        replacePlaceHolder(pingCommand, "%host", host);
# 278 "kind.ag"
      else
# 279 "kind.ag"
        replacePlaceHolder(pingCommand, "%host", server);
# 280 "kind.ag"
      int rc = 0;
# 281 "kind.ag"
      Strings pingResult = myPopen(pingCommand, rc, debug);
# 282 "kind.ag"
      if (rc != 0)
# 283 "kind.ag"
        {
# 284 "kind.ag"
          strings2File(pingResult, errorfile);
# 285 "kind.ag"
          throw Exception("Host not available", pingCommand);
# 286 "kind.ag"
        }
# 287 "kind.ag"
    }
# 288 "kind.ag"

# 289 "kind.ag"
  string path = conf.getString("path");
# 290 "kind.ag"
  if (path.empty())
# 291 "kind.ag"
    throw Exception("rsync", "empty source path");
# 292 "kind.ag"
  if (path.back() != '/')
# 293 "kind.ag"
    path += '/';
# 294 "kind.ag"

# 295 "kind.ag"
  string rsyncCmd = "rsync -vrltH --delete --stats -D --numeric-ids ";
# 296 "kind.ag"
  if (!conf.getBool("ignorePermission"))
# 297 "kind.ag"
    rsyncCmd += "-pgo";
# 298 "kind.ag"
  vector<string> rso = conf.getStrings("rsyncOption");
# 299 "kind.ag"
  for (const string& opt : rso)
# 300 "kind.ag"
    rsyncCmd += opt + " ";
# 301 "kind.ag"

# 302 "kind.ag"
  // excludes
# 303 "kind.ag"
  Strings excluded;
# 304 "kind.ag"

# 305 "kind.ag"
  if (conf.hasKey("exclude"))
# 306 "kind.ag"
    excluded += conf.getStrings("exclude");
# 307 "kind.ag"

# 308 "kind.ag"
  if (!host.empty())  // shell mode
# 309 "kind.ag"
    {
# 310 "kind.ag"
      // cout << "USING SHELLMODE '" << host << "'" << endl;
# 311 "kind.ag"
      string remoteShell = conf.getString("remoteShell");
# 312 "kind.ag"
      string userAtHost = conf.getString("user") + "@" +
# 313 "kind.ag"
      conf.getString("host");
# 314 "kind.ag"
      string rshCommand = remoteShell;
# 315 "kind.ag"
      if (remoteShell.empty())
# 316 "kind.ag"
        rshCommand = "ssh";
# 317 "kind.ag"

# 318 "kind.ag"
      rshCommand += " " + userAtHost;
# 319 "kind.ag"

# 320 "kind.ag"
      string userExcludeCommand = conf.getString("userExcludeCommand");
# 321 "kind.ag"

# 322 "kind.ag"
      if (!userExcludeCommand.empty())
# 323 "kind.ag"
        {
# 324 "kind.ag"
          replacePlaceHolder(userExcludeCommand, "%path", path);
# 325 "kind.ag"
          string excludeCommand = rshCommand + " " + userExcludeCommand;
# 326 "kind.ag"

# 327 "kind.ag"
          verbosePrint("searching for exclusions (" + excludeCommand + ")");
# 328 "kind.ag"

# 329 "kind.ag"
          int rc;
# 330 "kind.ag"
          Strings excludedFiles = myPopen(excludeCommand, rc, debug);
# 331 "kind.ag"
          if (rc > 0)
# 332 "kind.ag"
            {
# 333 "kind.ag"
              // return Strings should contain error messages
# 334 "kind.ag"
              strings2File(excludedFiles, errorfile);
# 335 "kind.ag"
              throw Exception("Find exludes", "Search for excludes failed");
# 336 "kind.ag"
            }
# 337 "kind.ag"

# 338 "kind.ag"
          for (unsigned int i = 0; i < excludedFiles.size(); ++i)
# 339 "kind.ag"
            {
# 340 "kind.ag"
              FileName fn(excludedFiles[i]);
# 341 "kind.ag"
              excluded.push_back('/' + fn.getPath());
# 342 "kind.ag"
              debugPrint("Excluding: " + excluded.back());
# 343 "kind.ag"
            }
# 344 "kind.ag"
        }
# 345 "kind.ag"

# 346 "kind.ag"
      string userExcludeFile = conf.getString("userExcludeFile");
# 347 "kind.ag"
      if (!userExcludeFile.empty())
# 348 "kind.ag"
        {
# 349 "kind.ag"
          userExcludeFile = path + userExcludeFile;
# 350 "kind.ag"
          string getExcludeFileCommand = rshCommand;
# 351 "kind.ag"
          getExcludeFileCommand += " \" if [ -f '" + userExcludeFile + "' ]; then ";
# 352 "kind.ag"
          getExcludeFileCommand += " cat '" + userExcludeFile + "' ; fi \"";
# 353 "kind.ag"
          // cout << getExcludeFileCommand << endl;
# 354 "kind.ag"
          int rc;
# 355 "kind.ag"
          Strings excludes2 = myPopen(getExcludeFileCommand, rc, debug);
# 356 "kind.ag"
          if (rc == 0)
# 357 "kind.ag"
            excluded += excludes2;
# 358 "kind.ag"
        }
# 359 "kind.ag"

# 360 "kind.ag"
      if (!dryRun)
# 361 "kind.ag"
        strings2File(excluded, imageFullName + "/exclude");
# 362 "kind.ag"

# 363 "kind.ag"
      // rsync image
# 364 "kind.ag"

# 365 "kind.ag"
      if (!remoteShell.empty())
# 366 "kind.ag"
        rsyncCmd += " -e \'" + remoteShell + "\' ";
# 367 "kind.ag"

# 368 "kind.ag"
      rsyncCmd += "--exclude-from=" + imageFullName + "/exclude ";
# 369 "kind.ag"
      if (!referenceImage.empty())
# 370 "kind.ag"
        rsyncCmd += "--link-dest=" + referenceImage + "/tree ";
# 371 "kind.ag"
      rsyncCmd += userAtHost + ":" + path + " ";
# 372 "kind.ag"
      rsyncCmd += imageFullName + "/tree";
# 373 "kind.ag"
    } // shell mode
# 374 "kind.ag"
  else
# 375 "kind.ag"
    {
# 376 "kind.ag"
      // cout << "USING SERVERMODE" << endl;
# 377 "kind.ag"
      // we cannot use find without shell access
# 378 "kind.ag"
      // and do not read an exclude file on client side
# 379 "kind.ag"

# 380 "kind.ag"
      if (!dryRun)
# 381 "kind.ag"
        strings2File(excluded, imageFullName + "/exclude");
# 382 "kind.ag"

# 383 "kind.ag"
      rsyncCmd += "--exclude-from=" + imageFullName + "/exclude ";
# 384 "kind.ag"
      if (!referenceImage.empty())
# 385 "kind.ag"
        rsyncCmd += "--link-dest=" + referenceImage + "/tree ";
# 386 "kind.ag"
      rsyncCmd += conf.getString("server") + "::" + path + " ";
# 387 "kind.ag"
      rsyncCmd += imageFullName + "/tree";
# 388 "kind.ag"
    }
# 389 "kind.ag"

# 390 "kind.ag"
  debugPrint("Action: " + rsyncCmd);
# 391 "kind.ag"

# 392 "kind.ag"
  vector<string> backupResult;
# 393 "kind.ag"
  if (!dryRun)
# 394 "kind.ag"
    {
# 395 "kind.ag"
      verbosePrint("syncing (" + rsyncCmd + ")");
# 396 "kind.ag"
      int rc;
# 397 "kind.ag"
      backupResult = myPopen(rsyncCmd, rc, debug, imageFullName + "/rsync-log");
# 398 "kind.ag"
      if (rc == 0 ||
# 399 "kind.ag"
      rc == 24 || // "no error" or "vanished source files" (ignored)
# 400 "kind.ag"
      rc == 6144) // workaround for wrong exit code ??!!
# 401 "kind.ag"
        {
# 402 "kind.ag"
          unlink(errorfile.c_str());
# 403 "kind.ag"
          long int st = 0;
# 404 "kind.ag"
          long int sc = 0;
# 405 "kind.ag"
          for (auto bl : backupResult)
# 406 "kind.ag"
            {
# 407 "kind.ag"
              if (startsWith(bl, "Total file size"))
# 408 "kind.ag"
                st = getNumber(bl);
# 409 "kind.ag"
              else if (startsWith(bl, "Total transferred file size"))
# 410 "kind.ag"
                sc = getNumber(bl);
# 411 "kind.ag"
            }
# 412 "kind.ag"
          // sizes[vault] = pair<long int, long int>(st, sc);
# 413 "kind.ag"
          sizes[vault] = Sizes(st, sc);
# 414 "kind.ag"
          //  cout << vault << " " << st << " || " << sc << endl;
# 415 "kind.ag"
        }
# 416 "kind.ag"
      else
# 417 "kind.ag"
        throw Exception("Backup", "Failed to execute rsync (result: " + to_string(rc) + ")");
# 418 "kind.ag"
    }
# 419 "kind.ag"
  else
# 420 "kind.ag"
    cout << "Not executing " << rsyncCmd << endl;
# 421 "kind.ag"
}
# 422 "kind.ag"

# 423 "kind.ag"
void backupVault(const string& vault,
# 424 "kind.ag"
                 KindConfig conf /*Copy!*/ ,
# 425 "kind.ag"
                 const DateTime& imageTime,
# 426 "kind.ag"
                 bool fullImage,
# 427 "kind.ag"
                 const string& forcedBackupSet)
# 428 "kind.ag"
{
# 429 "kind.ag"
  if (!quiet)
# 430 "kind.ag"
    cout << DateTime::now().getString('h') << ": Backup of vault " << vault << endl;
# 431 "kind.ag"
  try
# 432 "kind.ag"
    {
# 433 "kind.ag"
      readVaultConfig(vault, conf);
# 434 "kind.ag"

# 435 "kind.ag"
      // where to store
# 436 "kind.ag"
      string vaultPath = findVault(vault);
# 437 "kind.ag"

# 438 "kind.ag"
      // image path
# 439 "kind.ag"
      string imageFullName = getImageName(conf, vaultPath, imageTime);
# 440 "kind.ag"

# 441 "kind.ag"
      bool backupNow = true;
# 442 "kind.ag"

# 443 "kind.ag"
      // existing images
# 444 "kind.ag"
      Images validImageList = findImages(vaultPath, conf, false);
# 445 "kind.ag"
      string currentSet = "expire"; // we are not using backupSets
# 446 "kind.ag"

# 447 "kind.ag"
      // check if we are using backup sets
# 448 "kind.ag"

# 449 "kind.ag"
      map<string, int> setIdx;
# 450 "kind.ag"
      vector<SetRule> backupSetRule;
# 451 "kind.ag"
      int setRuleIdx = -1;
# 452 "kind.ag"

# 453 "kind.ag"
      if (conf.hasKey("setRule"))
# 454 "kind.ag"
        {
# 455 "kind.ag"
          readSetRules(conf, setIdx, backupSetRule);
# 456 "kind.ag"
          if (!setIdx.empty())
# 457 "kind.ag"
            {
# 458 "kind.ag"
              if (forcedBackupSet.empty())
# 459 "kind.ag"
                {
# 460 "kind.ag"
                  backupNow = false;
# 461 "kind.ag"

# 462 "kind.ag"
                  // find time for nextBackup for every backupSet
# 463 "kind.ag"
                  // defaults to now == imageTime;
# 464 "kind.ag"
                  vector<DateTime> nextBackup(backupSetRule.size(), imageTime);
# 465 "kind.ag"

# 466 "kind.ag"
                  // find time for next backup
# 467 "kind.ag"

# 468 "kind.ag"
                  for (const Image& image : validImageList)
# 469 "kind.ag"
                    {
# 470 "kind.ag"
                      if (image.series != "expire")
# 471 "kind.ag"
                        {
# 472 "kind.ag"
                          string s = image.series;
# 473 "kind.ag"
                          if (setIdx.count(s) > 0) // rule for set exists?
# 474 "kind.ag"
                            {
# 475 "kind.ag"
                              int rIdx = setIdx[s];
# 476 "kind.ag"
                              // image is valid for this and "lower level" backupSets
# 477 "kind.ag"
                              for (unsigned int i = rIdx; i < backupSetRule.size(); ++i)
# 478 "kind.ag"
                                if (nextBackup[i] < image.time + backupSetRule[i].distance)
# 479 "kind.ag"
                                  nextBackup[i] =  image.time + backupSetRule[i].distance;
# 480 "kind.ag"
                            }
# 481 "kind.ag"
                        }
# 482 "kind.ag"
                    }
# 483 "kind.ag"
                  if (debug)
# 484 "kind.ag"
                    for (unsigned int i = 0; i < backupSetRule.size(); ++i)
# 485 "kind.ag"
                      cout << "       Next backup for " << backupSetRule[i].name << " at " <<  nextBackup[i].getString('h') << endl;
# 486 "kind.ag"

# 487 "kind.ag"
                  // find backupSet that
# 488 "kind.ag"
                  //    - needs backup
# 489 "kind.ag"
                  //    - has longest time to keep
# 490 "kind.ag"
                  // because of ordered list backupSetRule this is the first set, that need
# 491 "kind.ag"

# 492 "kind.ag"
                  currentSet = "";
# 493 "kind.ag"
                  for (unsigned int i = 0; i < backupSetRule.size() && currentSet.empty(); ++i)
# 494 "kind.ag"
                    {
# 495 "kind.ag"
                      string name = backupSetRule[i].name;
# 496 "kind.ag"
                      if (nextBackup[i] <= imageTime + 5) // small offset of 5s for "jitter"
# 497 "kind.ag"
                        {
# 498 "kind.ag"
                          backupNow = true;
# 499 "kind.ag"
                          currentSet = name;
# 500 "kind.ag"
                          setRuleIdx = i;
# 501 "kind.ag"
                        }
# 502 "kind.ag"
                    }
# 503 "kind.ag"
                }
# 504 "kind.ag"
              else
# 505 "kind.ag"
                {
# 506 "kind.ag"
                  if (setIdx.count(forcedBackupSet) > 0)
# 507 "kind.ag"
                    {
# 508 "kind.ag"
                      currentSet = forcedBackupSet;
# 509 "kind.ag"
                      setRuleIdx = setIdx[forcedBackupSet];
# 510 "kind.ag"
                    }
# 511 "kind.ag"
                  else
# 512 "kind.ag"
                    throw Exception("force backup of set " + forcedBackupSet, " set not exists");
# 513 "kind.ag"
                }
# 514 "kind.ag"
            } // if (!setIdx.empty())
# 515 "kind.ag"
        } // (conf.hasKey("setRule"))
# 516 "kind.ag"

# 517 "kind.ag"
      if (backupNow)
# 518 "kind.ag"
        {
# 519 "kind.ag"
          verbosePrint("backup to \"" + imageFullName + "\"");
# 520 "kind.ag"
          if (setRuleIdx >= 0 && !quiet)
# 521 "kind.ag"
            cout << "  backup set is \"" << currentSet << "\"" << endl;
# 522 "kind.ag"
        }
# 523 "kind.ag"
      else if (!quiet)
# 524 "kind.ag"
        cout << "  no backup set needs update" << endl;
# 525 "kind.ag"

# 526 "kind.ag"
      if (backupNow)
# 527 "kind.ag"
        {
# 528 "kind.ag"

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
    }
# 587 "kind.ag"
  catch (Exception ex)
# 588 "kind.ag"
    {
# 589 "kind.ag"
      cerr << "Exception in vault " << vault << ": " << ex.what() << endl;
# 590 "kind.ag"
    }
# 591 "kind.ag"
}
# 592 "kind.ag"

# 593 "kind.ag"
void expireVault(const string& vault, KindConfig conf, DateTime now)
# 594 "kind.ag"
{
# 595 "kind.ag"
  if (!quiet)
# 596 "kind.ag"
    cout << DateTime::now().getString('h') << ": Expiring images in vault " << vault << endl;
# 597 "kind.ag"

# 598 "kind.ag"
  readVaultConfig(vault, conf);
# 599 "kind.ag"

# 600 "kind.ag"
  string vaultpath = findVault(vault);
# 601 "kind.ag"

# 602 "kind.ag"
  Images imagelist = findImages(vaultpath, conf, true);
# 603 "kind.ag"

# 604 "kind.ag"
  string lastValidImage;
# 605 "kind.ag"
  for (Image image : imagelist)
# 606 "kind.ag"
    {
# 607 "kind.ag"
      if (image.valid)
# 608 "kind.ag"
        lastValidImage = image.name;
# 609 "kind.ag"
    }
# 610 "kind.ag"

# 611 "kind.ag"
  for (Image image : imagelist)
# 612 "kind.ag"
    {
# 613 "kind.ag"
      debugPrint(image.name);
# 614 "kind.ag"

# 615 "kind.ag"
      DateTime imageTime = image.time;
# 616 "kind.ag"

# 617 "kind.ag"
      if (imageTime != now &&          // ignore just created image
# 618 "kind.ag"
      image.name != lastValidImage // ignore last valid image
# 619 "kind.ag"
         )
# 620 "kind.ag"
        {
# 621 "kind.ag"
          DateTime expireTime;
# 622 "kind.ag"
          string expireRule;
# 623 "kind.ag"
          if (!image.valid) // invalid image?
# 624 "kind.ag"
            {
# 625 "kind.ag"
              time_t expPeriod = stot(conf.getString("expireFailedImage"));
# 626 "kind.ag"
              if (expPeriod < 0)
# 627 "kind.ag"
                throw Exception("expireFailedImage", "Time period must be positive");
# 628 "kind.ag"
              expireTime = imageTime + stot(conf.getString("expireFailedImage"));
# 629 "kind.ag"
              expireRule = "invalid image: " + conf.getString("expireFailedImage");
# 630 "kind.ag"
              debugPrint("- invalid image");
# 631 "kind.ag"
            }
# 632 "kind.ag"
          else
# 633 "kind.ag"
            {
# 634 "kind.ag"
              debugPrint("- valid image");
# 635 "kind.ag"
              expireTime = image.expire;
# 636 "kind.ag"
              expireRule = image.expireRule;
# 637 "kind.ag"
            }
# 638 "kind.ag"

# 639 "kind.ag"
          if (debug)
# 640 "kind.ag"
            {
# 641 "kind.ag"
              cout << "    image: " << imageTime.getString('h') << endl;
# 642 "kind.ag"
              cout << "      expire: " << expireTime.getString('h') << " " << expireRule << endl;
# 643 "kind.ag"
              cout << "      now: " << now.getString('h') << endl;
# 644 "kind.ag"
            }
# 645 "kind.ag"

# 646 "kind.ag"
          if (expireTime < now)
# 647 "kind.ag"
            {
# 648 "kind.ag"
              if (!quiet)
# 649 "kind.ag"
                cout << "  removing image " << image.name << endl;
# 650 "kind.ag"
              try
# 651 "kind.ag"
                {
# 652 "kind.ag"
                  if (removeDir(image.name) != 0)
# 653 "kind.ag"
                    cout << "Error removing " <<  image.name << endl;
# 654 "kind.ag"
                }
# 655 "kind.ag"
              catch (Exception ex)
# 656 "kind.ag"
                {
# 657 "kind.ag"
                  cerr << "Exception: " << ex.what() << endl;
# 658 "kind.ag"
                }
# 659 "kind.ag"
            }
# 660 "kind.ag"
        }
# 661 "kind.ag"
      else
# 662 "kind.ag"
        debugPrint("- current image - ignored");
# 663 "kind.ag"
    }
# 664 "kind.ag"
}
# 665 "kind.ag"

# 666 "kind.ag"
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
# 670 "kind.ag"

# 671 "kind.ag"
  int exitCode = 0;
# 672 "kind.ag"
  string lockFile;
# 673 "kind.ag"
  try
# 674 "kind.ag"
    {
# 675 "kind.ag"
      // handling of parameters and switches
# 676 "kind.ag"
      if (debug)        // debug implies verbose
# 677 "kind.ag"
        verbose = true;
# 678 "kind.ag"

# 679 "kind.ag"
      if (!doBackup && !doExpire && !listConfig && !listImages)
# 680 "kind.ag"
        {
# 681 "kind.ag"
          doBackup = true;
# 682 "kind.ag"
          doExpire = true;
# 683 "kind.ag"
        }
# 684 "kind.ag"

# 685 "kind.ag"
      KindConfig conf;
# 686 "kind.ag"

# 687 "kind.ag"
      // default-values
# 688 "kind.ag"
      conf.add("imageName", "image");
# 689 "kind.ag"
      conf.add("vaultConfigName", "kind/vault.conf");
# 690 "kind.ag"
      conf.add("expireFailedImage", "3 days");
# 691 "kind.ag"
      conf.add("expireRule", "* * * * 1 month");
# 692 "kind.ag"
      conf.add("ping", "ping -c 1 -W 5 %host");
# 693 "kind.ag"
      conf.add("rsyncOption", ""); // no additional rsync option
# 694 "kind.ag"
      conf.add("remoteShell", "");
# 695 "kind.ag"
      conf.add("lockfile", "/var/lock/kind");
# 696 "kind.ag"
      conf.add("userExcludeFile", "nobackup.list");
# 697 "kind.ag"
      conf.add("userExcludeCommand",
# 698 "kind.ag"
               "find %path -type f -iname '*nobackup' -printf '%P\\\\n'");
# 699 "kind.ag"
      conf.add("logSize", "");
# 700 "kind.ag"
      conf.add("lastLink", "symLink");
# 701 "kind.ag"

# 702 "kind.ag"
      if (listConfig)
# 703 "kind.ag"
        {
# 704 "kind.ag"
          cout << "builtin config" << endl;
# 705 "kind.ag"
          conf.print(".   ");
# 706 "kind.ag"
        }
# 707 "kind.ag"

# 708 "kind.ag"
      readMasterConfig(masterConfig, conf);
# 709 "kind.ag"

# 710 "kind.ag"
      banks = conf.getStrings("bank");
# 711 "kind.ag"
      if (banks.empty())
# 712 "kind.ag"
        throw Exception("read master configuration", "no banks defined");
# 713 "kind.ag"

# 714 "kind.ag"
      if (listConfig)
# 715 "kind.ag"
        {
# 716 "kind.ag"
          cout << "global config:" << endl;
# 717 "kind.ag"
          conf.print(".   ");
# 718 "kind.ag"
          readVaultConfig(vault, conf);
# 719 "kind.ag"
          cout << "vault config:" << endl;
# 720 "kind.ag"
          conf.print(".   ");
# 721 "kind.ag"
          exit(0);
# 722 "kind.ag"
        }
# 723 "kind.ag"

# 724 "kind.ag"
      DateTime imageTime = DateTime::now();
# 725 "kind.ag"

# 726 "kind.ag"
      if (listImages)
# 727 "kind.ag"
        {
# 728 "kind.ag"
          listImageInfo(vault, conf, imageTime, forcedBackupSet);
# 729 "kind.ag"
          exit(0);
# 730 "kind.ag"
        }
# 731 "kind.ag"

# 732 "kind.ag"
      lockFile = conf.getString("lockfile");
# 733 "kind.ag"
      createLock(lockFile);
# 734 "kind.ag"

# 735 "kind.ag"
      string logSizeFile = conf.getString("logSize");
# 736 "kind.ag"
      if (!logSizeFile.empty())
# 737 "kind.ag"
        readSizes(logSizeFile);
# 738 "kind.ag"

# 739 "kind.ag"
      vector<string> vaults;
# 740 "kind.ag"
      string groupname = "group_" + vault;
# 741 "kind.ag"
      if (conf.hasKey(groupname))
# 742 "kind.ag"
        vaults = conf.getStrings(groupname);
# 743 "kind.ag"
      else
# 744 "kind.ag"
        vaults.push_back(vault);
# 745 "kind.ag"

# 746 "kind.ag"
      if (doBackup)
# 747 "kind.ag"
        for (string vault : vaults)
# 748 "kind.ag"
          {
# 749 "kind.ag"
            backupVault(vault, conf, imageTime, fullImage, forcedBackupSet);
# 750 "kind.ag"
            writeSizes(logSizeFile);
# 751 "kind.ag"
          }
# 752 "kind.ag"

# 753 "kind.ag"
      if (doExpire)
# 754 "kind.ag"
        for (unsigned int i = 0; i < vaults.size(); ++i)
# 755 "kind.ag"
          expireVault(vaults[i], conf, imageTime);
# 756 "kind.ag"

# 757 "kind.ag"
      if (!quiet)
# 758 "kind.ag"
        cout << DateTime::now().getString('h') << ": finished" << endl;
# 759 "kind.ag"

# 760 "kind.ag"
    }
# 761 "kind.ag"
  catch (const Exception& ex)
# 762 "kind.ag"
    {
# 763 "kind.ag"
      cerr << "Exception: " << ex.what() << endl;
# 764 "kind.ag"
      exitCode = 1;
# 765 "kind.ag"
    }
# 766 "kind.ag"
  catch (const char* msg)
# 767 "kind.ag"
    {
# 768 "kind.ag"
      cerr << "Exception(char*): " << msg << endl;
# 769 "kind.ag"
      exitCode = 1;
# 770 "kind.ag"
    }
# 771 "kind.ag"
  catch (const string& msg)
# 772 "kind.ag"
    {
# 773 "kind.ag"
      cerr << "Exception(string): " << msg << endl;
# 774 "kind.ag"
      exitCode = 1;
# 775 "kind.ag"
    }
# 776 "kind.ag"
  removeLock(lockFile);
# 777 "kind.ag"
  return exitCode;
# 778 "kind.ag"
}
