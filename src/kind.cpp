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
  if (host.empty() && server.empty())
# 239 "kind.ag"
    throw Exception("backupVault", "No host or server specified");
# 240 "kind.ag"

# 241 "kind.ag"
  // ping host / server
# 242 "kind.ag"
  // ping -c 1 -W 5 -q $HOST
# 243 "kind.ag"
  string pingCommand = conf.getString("ping");
# 244 "kind.ag"
  debugPrint("PingCommand: " + pingCommand);
# 245 "kind.ag"
  if (!pingCommand.empty())
# 246 "kind.ag"
    {
# 247 "kind.ag"
      if (!host.empty())
# 248 "kind.ag"
        replacePlaceHolder(pingCommand, "%host", host);
# 249 "kind.ag"
      else
# 250 "kind.ag"
        replacePlaceHolder(pingCommand, "%host", server);
# 251 "kind.ag"
      int rc = 0;
# 252 "kind.ag"
      Strings pingResult = myPopen(pingCommand, rc, debug);
# 253 "kind.ag"
      if (rc != 0)
# 254 "kind.ag"
	{
# 255 "kind.ag"
	  strings2File(pingResult, errorfile);
# 256 "kind.ag"
	  throw Exception("Host not available",pingCommand);
# 257 "kind.ag"
	}
# 258 "kind.ag"
    }
# 259 "kind.ag"

# 260 "kind.ag"
      string path = conf.getString("path");
# 261 "kind.ag"
      if (path.empty())
# 262 "kind.ag"
        throw Exception("rsync", "empty source path");
# 263 "kind.ag"
      if (path.back() != '/')
# 264 "kind.ag"
        path += '/';
# 265 "kind.ag"

# 266 "kind.ag"
      string rsyncCmd = "rsync -vrltH --delete --stats -D --numeric-ids ";
# 267 "kind.ag"
      if (!conf.getBool("ignorePermission"))
# 268 "kind.ag"
        rsyncCmd += "-pgo";
# 269 "kind.ag"
      vector<string> rso = conf.getStrings("rsyncOption");
# 270 "kind.ag"
      for (const string& opt : rso)
# 271 "kind.ag"
        rsyncCmd += opt + " ";
# 272 "kind.ag"

# 273 "kind.ag"
      // excludes
# 274 "kind.ag"
      Strings excluded;
# 275 "kind.ag"

# 276 "kind.ag"
      if (conf.hasKey("exclude"))
# 277 "kind.ag"
        excluded += conf.getStrings("exclude");
# 278 "kind.ag"

# 279 "kind.ag"
      if (!host.empty())  // shell mode
# 280 "kind.ag"
        {
# 281 "kind.ag"
          // cout << "USING SHELLMODE '" << host << "'" << endl;
# 282 "kind.ag"
          string remoteShell = conf.getString("remoteShell");
# 283 "kind.ag"
          string userAtHost = conf.getString("user") + "@" +
# 284 "kind.ag"
                              conf.getString("host");
# 285 "kind.ag"
          string rshCommand = remoteShell;
# 286 "kind.ag"
          if (remoteShell.empty())
# 287 "kind.ag"
            rshCommand = "ssh";
# 288 "kind.ag"

# 289 "kind.ag"
          rshCommand += " " + userAtHost;
# 290 "kind.ag"

# 291 "kind.ag"
          string userExcludeCommand = conf.getString("userExcludeCommand");
# 292 "kind.ag"

# 293 "kind.ag"
          if (!userExcludeCommand.empty())
# 294 "kind.ag"
            {
# 295 "kind.ag"
              replacePlaceHolder(userExcludeCommand, "%path", path);
# 296 "kind.ag"
              string excludeCommand = rshCommand + " " + userExcludeCommand;
# 297 "kind.ag"

# 298 "kind.ag"
              verbosePrint("searching for exclusions (" + excludeCommand + ")");
# 299 "kind.ag"

# 300 "kind.ag"
              int rc;
# 301 "kind.ag"
              Strings excludedFiles = myPopen(excludeCommand, rc, debug);
# 302 "kind.ag"
              if (rc > 0)
# 303 "kind.ag"
                {
# 304 "kind.ag"
                  // return Strings should contain error messages
# 305 "kind.ag"
                  strings2File(excludedFiles, errorfile);
# 306 "kind.ag"
                  throw Exception("Find exludes", "Search for excludes failed");
# 307 "kind.ag"
                }
# 308 "kind.ag"

# 309 "kind.ag"
              for (unsigned int i = 0; i < excludedFiles.size(); ++i)
# 310 "kind.ag"
                {
# 311 "kind.ag"
                  FileName fn(excludedFiles[i]);
# 312 "kind.ag"
                  excluded.push_back('/' + fn.getPath());
# 313 "kind.ag"
                  debugPrint("Excluding: " + excluded.back());
# 314 "kind.ag"
                }
# 315 "kind.ag"
            }
# 316 "kind.ag"

# 317 "kind.ag"
          string userExcludeFile = conf.getString("userExcludeFile");
# 318 "kind.ag"
          if (!userExcludeFile.empty())
# 319 "kind.ag"
            {
# 320 "kind.ag"
              userExcludeFile = path + userExcludeFile;
# 321 "kind.ag"
              string getExcludeFileCommand = rshCommand;
# 322 "kind.ag"
              getExcludeFileCommand += " \" if [ -f '" + userExcludeFile + "' ]; then ";
# 323 "kind.ag"
              getExcludeFileCommand += " cat '" + userExcludeFile + "' ; fi \"";
# 324 "kind.ag"
              // cout << getExcludeFileCommand << endl;
# 325 "kind.ag"
              int rc;
# 326 "kind.ag"
              Strings excludes2 = myPopen(getExcludeFileCommand, rc, debug);
# 327 "kind.ag"
              if (rc == 0)
# 328 "kind.ag"
                excluded += excludes2;
# 329 "kind.ag"
            }
# 330 "kind.ag"

# 331 "kind.ag"
          if (!dryRun)
# 332 "kind.ag"
            strings2File(excluded, imageFullName + "/exclude");
# 333 "kind.ag"

# 334 "kind.ag"
          // rsync image
# 335 "kind.ag"

# 336 "kind.ag"
          if (!remoteShell.empty())
# 337 "kind.ag"
            rsyncCmd += " -e \'" + remoteShell + "\' ";
# 338 "kind.ag"

# 339 "kind.ag"
          rsyncCmd += "--exclude-from=" + imageFullName + "/exclude ";
# 340 "kind.ag"
          if (!referenceImage.empty())
# 341 "kind.ag"
            rsyncCmd += "--link-dest=" + referenceImage + "/tree ";
# 342 "kind.ag"
          rsyncCmd += userAtHost + ":" + path + " ";
# 343 "kind.ag"
          rsyncCmd += imageFullName + "/tree";
# 344 "kind.ag"
        } // shell mode
# 345 "kind.ag"
      else
# 346 "kind.ag"
        {
# 347 "kind.ag"
          // cout << "USING SERVERMODE" << endl;
# 348 "kind.ag"
          // we cannot use find without shell access
# 349 "kind.ag"
          // and do not read an exclude file on client side
# 350 "kind.ag"

# 351 "kind.ag"
          if (!dryRun)
# 352 "kind.ag"
            strings2File(excluded, imageFullName + "/exclude");
# 353 "kind.ag"

# 354 "kind.ag"
          rsyncCmd += "--exclude-from=" + imageFullName + "/exclude ";
# 355 "kind.ag"
          if (!referenceImage.empty())
# 356 "kind.ag"
            rsyncCmd += "--link-dest=" + referenceImage + "/tree ";
# 357 "kind.ag"
          rsyncCmd += conf.getString("server") + "::" + path + " ";
# 358 "kind.ag"
          rsyncCmd += imageFullName + "/tree";
# 359 "kind.ag"
        }
# 360 "kind.ag"

# 361 "kind.ag"
      debugPrint("Action: " + rsyncCmd);
# 362 "kind.ag"

# 363 "kind.ag"
      vector<string> backupResult;
# 364 "kind.ag"
      if (!dryRun)
# 365 "kind.ag"
        {
# 366 "kind.ag"
          verbosePrint("syncing (" + rsyncCmd + ")");
# 367 "kind.ag"
          int rc;
# 368 "kind.ag"
          backupResult = myPopen(rsyncCmd, rc, debug, imageFullName + "/rsync-log");
# 369 "kind.ag"
          if (rc == 0 ||
# 370 "kind.ag"
              rc == 24 || // "no error" or "vanished source files" (ignored)
# 371 "kind.ag"
              rc == 6144) // workaround for wrong exit code ??!!
# 372 "kind.ag"
            {
# 373 "kind.ag"
              unlink(errorfile.c_str());
# 374 "kind.ag"
              long int st = 0;
# 375 "kind.ag"
              long int sc = 0;
# 376 "kind.ag"
              for (auto bl : backupResult)
# 377 "kind.ag"
                {
# 378 "kind.ag"
                  if (startsWith(bl, "Total file size"))
# 379 "kind.ag"
                    st = getNumber(bl);
# 380 "kind.ag"
                  else if (startsWith(bl, "Total transferred file size"))
# 381 "kind.ag"
                    sc = getNumber(bl);
# 382 "kind.ag"
                }
# 383 "kind.ag"
              // sizes[vault] = pair<long int, long int>(st, sc);
# 384 "kind.ag"
              sizes[vault] = Sizes(st, sc);
# 385 "kind.ag"
              //  cout << vault << " " << st << " || " << sc << endl;
# 386 "kind.ag"
            }
# 387 "kind.ag"
          else
# 388 "kind.ag"
            throw Exception("Backup", "Failed to execute rsync (result: " + to_string(rc) + ")");
# 389 "kind.ag"
        }
# 390 "kind.ag"
      else
# 391 "kind.ag"
        cout << "Not executing " << rsyncCmd << endl;
# 392 "kind.ag"
}
# 393 "kind.ag"

# 394 "kind.ag"
void backupVault(const string& vault,
# 395 "kind.ag"
                 KindConfig conf /*Copy!*/ ,
# 396 "kind.ag"
                 const DateTime& imageTime,
# 397 "kind.ag"
                 bool fullImage,
# 398 "kind.ag"
                 const string& forcedBackupSet)
# 399 "kind.ag"
{
# 400 "kind.ag"
  if (!quiet)
# 401 "kind.ag"
    cout << DateTime::now().getString('h') << ": Backup of vault " << vault << endl;
# 402 "kind.ag"
  try
# 403 "kind.ag"
    {
# 404 "kind.ag"
      sizes[vault].second = 0; // nothing backed up yet
# 405 "kind.ag"

# 406 "kind.ag"
      readVaultConfig(vault, conf);
# 407 "kind.ag"

# 408 "kind.ag"
      // where to store
# 409 "kind.ag"
      string vaultPath = findVault(vault);
# 410 "kind.ag"

# 411 "kind.ag"
      // image path
# 412 "kind.ag"
      string imageFullName = getImageName(conf, vaultPath, imageTime);
# 413 "kind.ag"

# 414 "kind.ag"
      bool backupNow = true;
# 415 "kind.ag"

# 416 "kind.ag"
      // existing images
# 417 "kind.ag"
      Images validImageList = findImages(vaultPath, conf, false);
# 418 "kind.ag"
      string currentSet = "expire"; // we are not using backupSets
# 419 "kind.ag"

# 420 "kind.ag"
      // check if we are using backup sets
# 421 "kind.ag"

# 422 "kind.ag"
      map<string, int> setIdx;
# 423 "kind.ag"
      vector<SetRule> backupSetRule;
# 424 "kind.ag"
      int setRuleIdx = -1;
# 425 "kind.ag"

# 426 "kind.ag"
      if (conf.hasKey("setRule"))
# 427 "kind.ag"
        {
# 428 "kind.ag"
          readSetRules(conf, setIdx, backupSetRule);
# 429 "kind.ag"
          if (!setIdx.empty())
# 430 "kind.ag"
            {
# 431 "kind.ag"
              if (forcedBackupSet.empty())
# 432 "kind.ag"
                {
# 433 "kind.ag"
                  backupNow = false;
# 434 "kind.ag"

# 435 "kind.ag"
                  // find time for nextBackup for every backupSet
# 436 "kind.ag"
                  // defaults to now == imageTime;
# 437 "kind.ag"
                  vector<DateTime> nextBackup(backupSetRule.size(), imageTime);
# 438 "kind.ag"

# 439 "kind.ag"
                  // find time for next backup
# 440 "kind.ag"

# 441 "kind.ag"
                  for (const Image& image : validImageList)
# 442 "kind.ag"
                    {
# 443 "kind.ag"
                      if (image.series != "expire")
# 444 "kind.ag"
                        {
# 445 "kind.ag"
                          string s = image.series;
# 446 "kind.ag"
                          if (setIdx.count(s) > 0) // rule for set exists?
# 447 "kind.ag"
                            {
# 448 "kind.ag"
                              int rIdx = setIdx[s];
# 449 "kind.ag"
                              // image is valid for this and "lower level" backupSets
# 450 "kind.ag"
                              for (unsigned int i = rIdx; i < backupSetRule.size(); ++i)
# 451 "kind.ag"
                                if (nextBackup[i] < image.time + backupSetRule[i].distance)
# 452 "kind.ag"
                                  nextBackup[i] =  image.time + backupSetRule[i].distance;
# 453 "kind.ag"
                            }
# 454 "kind.ag"
                        }
# 455 "kind.ag"
                    }
# 456 "kind.ag"
                  if (debug)
# 457 "kind.ag"
                    for (unsigned int i = 0; i < backupSetRule.size(); ++i)
# 458 "kind.ag"
                      cout << "       Next backup for " << backupSetRule[i].name << " at " <<  nextBackup[i].getString('h') << endl;
# 459 "kind.ag"

# 460 "kind.ag"
                  // find backupSet that
# 461 "kind.ag"
                  //    - needs backup
# 462 "kind.ag"
                  //    - has longest time to keep
# 463 "kind.ag"
                  // because of ordered list backupSetRule this is the first set, that need
# 464 "kind.ag"

# 465 "kind.ag"
                  currentSet = "";
# 466 "kind.ag"
                  for (unsigned int i = 0; i < backupSetRule.size() && currentSet.empty(); ++i)
# 467 "kind.ag"
                    {
# 468 "kind.ag"
                      string name = backupSetRule[i].name;
# 469 "kind.ag"
                      if (nextBackup[i] <= imageTime + 5) // small offset of 5s for "jitter"
# 470 "kind.ag"
                        {
# 471 "kind.ag"
                          backupNow = true;
# 472 "kind.ag"
                          currentSet = name;
# 473 "kind.ag"
                          setRuleIdx = i;
# 474 "kind.ag"
                        }
# 475 "kind.ag"
                    }
# 476 "kind.ag"
                }
# 477 "kind.ag"
              else
# 478 "kind.ag"
                {
# 479 "kind.ag"
                  if (setIdx.count(forcedBackupSet) > 0)
# 480 "kind.ag"
                    {
# 481 "kind.ag"
                      currentSet = forcedBackupSet;
# 482 "kind.ag"
                      setRuleIdx = setIdx[forcedBackupSet];
# 483 "kind.ag"
                    }
# 484 "kind.ag"
                  else
# 485 "kind.ag"
                    throw Exception("force backup of set " + forcedBackupSet, " set not exists");
# 486 "kind.ag"
                }
# 487 "kind.ag"
            } // if (!setIdx.empty())
# 488 "kind.ag"
        } // (conf.hasKey("setRule"))
# 489 "kind.ag"

# 490 "kind.ag"
      if (backupNow)
# 491 "kind.ag"
        {
# 492 "kind.ag"
          verbosePrint("backup to \"" + imageFullName + "\"");
# 493 "kind.ag"
          if (setRuleIdx >= 0 && !quiet)
# 494 "kind.ag"
            cout << "  backup set is \"" << currentSet << "\"" << endl;
# 495 "kind.ag"
        }
# 496 "kind.ag"
      else if (!quiet)
# 497 "kind.ag"
        cout << "  no backup set needs update" << endl;
# 498 "kind.ag"

# 499 "kind.ag"
      if (backupNow)
# 500 "kind.ag"
        {
# 501 "kind.ag"

# 502 "kind.ag"
          // find reference image
# 503 "kind.ag"
          string referenceImage;
# 504 "kind.ag"
          if (!fullImage)
# 505 "kind.ag"
            {
# 506 "kind.ag"
              if (validImageList.empty())
# 507 "kind.ag"
                throw Exception("backupVault", "no reference image found");
# 508 "kind.ag"
              // last image is newest image
# 509 "kind.ag"
              referenceImage = validImageList.back().name;
# 510 "kind.ag"
            }
# 511 "kind.ag"

# 512 "kind.ag"
          doBackup(vault, imageFullName, referenceImage, conf);
# 513 "kind.ag"

# 514 "kind.ag"
          if (!dryRun)
# 515 "kind.ag"
            {
# 516 "kind.ag"
              // set symlink to last image
# 517 "kind.ag"
              string lastLink = vaultPath + "/last";
# 518 "kind.ag"
              unlink(lastLink.c_str());
# 519 "kind.ag"
              symlink(imageFullName.c_str(), lastLink.c_str());
# 520 "kind.ag"

# 521 "kind.ag"
              // write expire date to file
# 522 "kind.ag"
              DateTime expireTime;
# 523 "kind.ag"
              string rule;
# 524 "kind.ag"
              if (setRuleIdx < 0) // not backup set based
# 525 "kind.ag"
                expireTime = getExpireDate(imageTime, conf, rule);
# 526 "kind.ag"
              else
# 527 "kind.ag"
                {
# 528 "kind.ag"
                  expireTime = imageTime + backupSetRule[setRuleIdx].keep;
# 529 "kind.ag"
                  rule =  backupSetRule[setRuleIdx].rule;
# 530 "kind.ag"
                }
# 531 "kind.ag"

# 532 "kind.ag"
              ofstream expireFile(imageFullName + "/expires");
# 533 "kind.ag"
              expireFile << currentSet << "-" << expireTime.getString('m') << endl;
# 534 "kind.ag"
              expireFile << rule << endl;
# 535 "kind.ag"
            }
# 536 "kind.ag"
        }
# 537 "kind.ag"
    }
# 538 "kind.ag"
  catch (Exception ex)
# 539 "kind.ag"
    {
# 540 "kind.ag"
      cerr << "Exception in vault " << vault << ": " << ex.what() << endl;
# 541 "kind.ag"
    }
# 542 "kind.ag"
}
# 543 "kind.ag"

# 544 "kind.ag"
void expireVault(const string& vault, KindConfig conf, DateTime now)
# 545 "kind.ag"
{
# 546 "kind.ag"
  if (!quiet)
# 547 "kind.ag"
    cout << DateTime::now().getString('h') << ": Expiring images in vault " << vault << endl;
# 548 "kind.ag"

# 549 "kind.ag"
  readVaultConfig(vault, conf);
# 550 "kind.ag"

# 551 "kind.ag"
  string vaultpath = findVault(vault);
# 552 "kind.ag"

# 553 "kind.ag"
  Images imagelist = findImages(vaultpath, conf, true);
# 554 "kind.ag"

# 555 "kind.ag"
  string lastValidImage;
# 556 "kind.ag"
  for (Image image : imagelist)
# 557 "kind.ag"
    {
# 558 "kind.ag"
      if (image.valid)
# 559 "kind.ag"
        lastValidImage = image.name;
# 560 "kind.ag"
    }
# 561 "kind.ag"

# 562 "kind.ag"
  for (Image image : imagelist)
# 563 "kind.ag"
    {
# 564 "kind.ag"
      debugPrint(image.name);
# 565 "kind.ag"

# 566 "kind.ag"
      DateTime imageTime = image.time;
# 567 "kind.ag"

# 568 "kind.ag"
      if (imageTime != now &&          // ignore just created image
# 569 "kind.ag"
          image.name != lastValidImage // ignore last valid image
# 570 "kind.ag"
         )
# 571 "kind.ag"
        {
# 572 "kind.ag"
          DateTime expireTime;
# 573 "kind.ag"
          string expireRule;
# 574 "kind.ag"
          if (!image.valid) // invalid image?
# 575 "kind.ag"
            {
# 576 "kind.ag"
              time_t expPeriod = stot(conf.getString("expireFailedImage"));
# 577 "kind.ag"
              if (expPeriod < 0)
# 578 "kind.ag"
                throw Exception("expireFailedImage", "Time period must be positive");
# 579 "kind.ag"
              expireTime = imageTime + stot(conf.getString("expireFailedImage"));
# 580 "kind.ag"
              expireRule = "invalid image: " + conf.getString("expireFailedImage");
# 581 "kind.ag"
              debugPrint("- invalid image");
# 582 "kind.ag"
            }
# 583 "kind.ag"
          else
# 584 "kind.ag"
            {
# 585 "kind.ag"
              debugPrint("- valid image");
# 586 "kind.ag"
              expireTime = image.expire;
# 587 "kind.ag"
              expireRule = image.expireRule;
# 588 "kind.ag"
            }
# 589 "kind.ag"

# 590 "kind.ag"
          if (debug)
# 591 "kind.ag"
            {
# 592 "kind.ag"
              cout << "    image: " << imageTime.getString('h') << endl;
# 593 "kind.ag"
              cout << "      expire: " << expireTime.getString('h') << " " << expireRule << endl;
# 594 "kind.ag"
              cout << "      now: " << now.getString('h') << endl;
# 595 "kind.ag"
            }
# 596 "kind.ag"

# 597 "kind.ag"
          if (expireTime < now)
# 598 "kind.ag"
            {
# 599 "kind.ag"
              if (!quiet)
# 600 "kind.ag"
                cout << "  removing image " << image.name << endl;
# 601 "kind.ag"
              try
# 602 "kind.ag"
                {
# 603 "kind.ag"
                  if (removeDir(image.name) != 0)
# 604 "kind.ag"
                    cout << "Error removing " <<  image.name << endl;
# 605 "kind.ag"
                }
# 606 "kind.ag"
              catch (Exception ex)
# 607 "kind.ag"
                {
# 608 "kind.ag"
                  cerr << "Exception: " << ex.what() << endl;
# 609 "kind.ag"
                }
# 610 "kind.ag"
            }
# 611 "kind.ag"
        }
# 612 "kind.ag"
      else
# 613 "kind.ag"
        debugPrint("- current image - ignored");
# 614 "kind.ag"
    }
# 615 "kind.ag"
}
# 616 "kind.ag"

# 617 "kind.ag"
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

  cout << "     if none of backup, expire or listconfig is specified," << endl;
  cout << "     backup and expire is assumed." << endl;
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
# 621 "kind.ag"

# 622 "kind.ag"
  int exitCode = 0;
# 623 "kind.ag"
  string lockFile;
# 624 "kind.ag"
  try
# 625 "kind.ag"
    {
# 626 "kind.ag"
      // handling of parameters and switches
# 627 "kind.ag"
      if (debug)        // debug implies verbose
# 628 "kind.ag"
        verbose = true;
# 629 "kind.ag"

# 630 "kind.ag"
      if (!doBackup && !doExpire && !listConfig)
# 631 "kind.ag"
        {
# 632 "kind.ag"
          doBackup = true;
# 633 "kind.ag"
          doExpire = true;
# 634 "kind.ag"
        }
# 635 "kind.ag"

# 636 "kind.ag"
      KindConfig conf;
# 637 "kind.ag"

# 638 "kind.ag"
      // default-values
# 639 "kind.ag"
      conf.add("imageName", "image");
# 640 "kind.ag"
      conf.add("vaultConfigName", "kind/vault.conf");
# 641 "kind.ag"
      conf.add("expireFailedImage", "3 days");
# 642 "kind.ag"
      conf.add("expireRule", "* * * * 1 month");
# 643 "kind.ag"
      conf.add("ping", "ping -c 1 -W 5 %host");
# 644 "kind.ag"
      conf.add("rsyncOption", ""); // no additional rsync option
# 645 "kind.ag"
      conf.add("remoteShell", "");
# 646 "kind.ag"
      conf.add("lockfile", "/var/lock/kind");
# 647 "kind.ag"
      conf.add("userExcludeFile", "nobackup.list");
# 648 "kind.ag"
      conf.add("userExcludeCommand",
# 649 "kind.ag"
               "find %path -type f -iname '*nobackup' -printf '%P\\\\n'");
# 650 "kind.ag"
      conf.add("logSize", "");
# 651 "kind.ag"

# 652 "kind.ag"
      if (listConfig)
# 653 "kind.ag"
        {
# 654 "kind.ag"
          cout << "builtin config" << endl;
# 655 "kind.ag"
          conf.print(".   ");
# 656 "kind.ag"
        }
# 657 "kind.ag"

# 658 "kind.ag"
      readMasterConfig(masterConfig, conf);
# 659 "kind.ag"

# 660 "kind.ag"
      banks = conf.getStrings("bank");
# 661 "kind.ag"
      if (banks.empty())
# 662 "kind.ag"
        throw Exception("read master configuration", "no banks defined");
# 663 "kind.ag"

# 664 "kind.ag"
      if (listConfig)
# 665 "kind.ag"
        {
# 666 "kind.ag"
          cout << "global config:" << endl;
# 667 "kind.ag"
          conf.print(".   ");
# 668 "kind.ag"
          readVaultConfig(vault, conf);
# 669 "kind.ag"
          cout << "vault config:" << endl;
# 670 "kind.ag"
          conf.print(".   ");
# 671 "kind.ag"
          exit(0);
# 672 "kind.ag"
        }
# 673 "kind.ag"

# 674 "kind.ag"
      lockFile = conf.getString("lockfile");
# 675 "kind.ag"
      createLock(lockFile);
# 676 "kind.ag"

# 677 "kind.ag"
      DateTime imageTime = DateTime::now();
# 678 "kind.ag"

# 679 "kind.ag"
      string logSizeFile = conf.getString("logSize");
# 680 "kind.ag"
      if (!logSizeFile.empty())
# 681 "kind.ag"
        readSizes(logSizeFile);
# 682 "kind.ag"

# 683 "kind.ag"
      vector<string> vaults;
# 684 "kind.ag"
      string groupname = "group_" + vault;
# 685 "kind.ag"
      if (conf.hasKey(groupname))
# 686 "kind.ag"
        vaults = conf.getStrings(groupname);
# 687 "kind.ag"
      else
# 688 "kind.ag"
        vaults.push_back(vault);
# 689 "kind.ag"

# 690 "kind.ag"
      if (doBackup)
# 691 "kind.ag"
        for (string vault : vaults)
# 692 "kind.ag"
          {
# 693 "kind.ag"
            backupVault(vault, conf, imageTime, fullImage, forcedBackupSet);
# 694 "kind.ag"
            writeSizes(logSizeFile);
# 695 "kind.ag"
          }
# 696 "kind.ag"

# 697 "kind.ag"
      if (doExpire)
# 698 "kind.ag"
        for (unsigned int i = 0; i < vaults.size(); ++i)
# 699 "kind.ag"
          expireVault(vaults[i], conf, imageTime);
# 700 "kind.ag"

# 701 "kind.ag"
      if (!quiet)
# 702 "kind.ag"
        cout << DateTime::now().getString('h') << ": finished" << endl;
# 703 "kind.ag"

# 704 "kind.ag"
    }
# 705 "kind.ag"
  catch (const Exception& ex)
# 706 "kind.ag"
    {
# 707 "kind.ag"
      cerr << "Exception: " << ex.what() << endl;
# 708 "kind.ag"
      exitCode = 1;
# 709 "kind.ag"
    }
# 710 "kind.ag"
  catch (const char* msg)
# 711 "kind.ag"
    {
# 712 "kind.ag"
      cerr << "Exception(char*): " << msg << endl;
# 713 "kind.ag"
      exitCode = 1;
# 714 "kind.ag"
    }
# 715 "kind.ag"
  catch (const string& msg)
# 716 "kind.ag"
    {
# 717 "kind.ag"
      cerr << "Exception(string): " << msg << endl;
# 718 "kind.ag"
      exitCode = 1;
# 719 "kind.ag"
    }
# 720 "kind.ag"
  removeLock(lockFile);
# 721 "kind.ag"
  return exitCode;
# 722 "kind.ag"
}
