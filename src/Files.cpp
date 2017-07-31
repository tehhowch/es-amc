/* Files.cpp
Copyright (c) 2017 by tehhowch

AntiMissileComparator is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the Free
Software Foundation, either version 3 of the License, or (at your option) any
later version.

AntiMissileComparator is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
details.

Derived from Files.cpp as distributed by Michael Zahniser in the GPLv3-licensed
program Endless Sky.
 */

#include "Files.h"

#include "File.h"

#if defined _WIN32
# if defined __c2__
  struct IUnknown;
# endif
#include <windows.h>
#endif

#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#include <climits>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <mutex>
#include <stdexcept>

using namespace std;

namespace {
	string resources;
	string gamedata;
	
	mutex errorMutex;
	
	// Convert windows-style directory separators ('\\') to standard '/'.
#if defined _WIN32
	void FixWindowsSlashes(string &path)
	{
		for(char &c : path)
			if(c == '\\')
				c = '/';
	}
	wstring ToUTF16(const string &path)
	{
		wstring result;
		if(path.empty())
			return result;
		
		bool endsInSlash = (path.back() == '/' || path.back() == '\\');
		int size = MultiByteToWideChar(CP_UTF8, 0, &path[0], path.length() - endsInSlash, nullptr, 0);
		result.resize(size);
		MultiByteToWideChar(CP_UTF8, 0, &path[0], path.length() - endsInSlash, &result[0], size);
		
		return result;
	}
	string ToUTF8(const wchar_t *str)
	{
		string result;
		if(!str || !*str)
			return result;
		
		// The returned size will include the null character at the end.
		int size = WideCharToMultiByte(CP_UTF8, 0, str, -1, nullptr, 0, nullptr, nullptr) - 1;
		result.resize(size);
		WideCharToMultiByte(CP_UTF8, 0, str, -1, &result[0], size, nullptr, nullptr);
		
		return result;
	}
#endif
}



void Files::Init(const char * const *argv)
{
	// Parse the command line arguments to see if the user has specified a
	// different resource directory to use (default is current directory).
	for(const char * const *it = argv + 1; *it; ++it)
	{
		string arg = *it;
		if((arg == "-r" || arg == "--resources") && *++it)
			resources = *it;
	}
	
	if(resources.empty())
	{
		// Find the path to the resource directory. This will depend on the
		// operating system, and can be overridden by a command line argument.
		char *str = GetBasePath();
		if(!str)
			throw runtime_error("Unable to get path to resource directory!");
		
		resources = str;
#ifndef _WIN32
		free(str);
#endif
	}
#if defined _WIN32
	FixWindowsSlashes(resources);
#endif
	if(resources.back() != '/')
		resources += '/';
#if defined __linux__ || defined __FreeBSD__ || defined __DragonFly__
	// Special case, for Linux: the resource files are not in the same place as
	// the executable, but are under the same prefix (/usr or /usr/local).
	static const string LOCAL_PATH = "/usr/local/";
	static const string STANDARD_PATH = "/usr/";
	static const string RESOURCE_PATH = "share/games/endless-sky/";
	if(!resources.compare(0, LOCAL_PATH.length(), LOCAL_PATH))
		resources = LOCAL_PATH + RESOURCE_PATH;
	else if(!resources.compare(0, STANDARD_PATH.length(), STANDARD_PATH))
		resources = STANDARD_PATH + RESOURCE_PATH;
#elif defined __APPLE__
	// Special case for Mac OS X: the resources are in ../Resources relative to
	// the folder the binary is in.
	resources = resources + "../Resources/";
#endif
	// If the resources are not here, search in the directories containing this
	// one. This allows, for example, a Mac app that does not actually have the
	// resources embedded within it.
	while(!Exists(resources + "credits.txt"))
	{
		size_t pos = resources.rfind('/', resources.length() - 2);
		if(pos == string::npos || pos == 0)
			throw runtime_error("Unable to find the resource directories!");
		resources.erase(pos + 1);
	}
	gamedata = resources + "data/";
	
	// Check that all the directories exist.
	if(!Exists(gamedata))
		throw runtime_error("Unable to find the resource directories!");
}



const string &Files::Resources()
{
	return resources;
}



const string &Files::Data()
{
	return gamedata;
}



vector<string> Files::List(string directory)
{
	if(directory.empty() || directory.back() != '/')
		directory += '/';
	
	vector<string> list;
	
#if defined _WIN32
	WIN32_FIND_DATAW ffd;
	HANDLE hFind = FindFirstFileW(ToUTF16(directory + '*').c_str(), &ffd);
	if(!hFind)
		return list;
	
	do {
		if(!ffd.cFileName || ffd.cFileName[0] == '.')
			continue;
		
		if(!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			list.push_back(directory + ToUTF8(ffd.cFileName));
	} while(FindNextFileW(hFind, &ffd));
	
	FindClose(hFind);
#else
	DIR *dir = opendir(directory.c_str());
	if(!dir)
		return list;
	
	while(true)
	{
		dirent *ent = readdir(dir);
		if(!ent)
			break;
		// Skip dotfiles (including "." and "..").
		if(ent->d_name[0] == '.')
			continue;
		
		string name = directory + ent->d_name;
		// Don't assume that this operating system's implementation of dirent
		// includes the t_type field; in particular, on Windows it will not.
		struct stat buf;
		stat(name.c_str(), &buf);
		bool isRegularFile = S_ISREG(buf.st_mode);
		
		if(isRegularFile)
			list.push_back(name);
	}
	
	closedir(dir);
#endif
	return list;
}



// Get a list of any directories in the given directory.
vector<string> Files::ListDirectories(string directory)
{
	if(directory.empty() || directory.back() != '/')
		directory += '/';
	
	vector<string> list;

#if defined _WIN32
	WIN32_FIND_DATAW ffd;
	HANDLE hFind = FindFirstFileW(ToUTF16(directory + '*').c_str(), &ffd);
	if(!hFind)
		return list;
	
	do {
		if(!ffd.cFileName || ffd.cFileName[0] == '.')
			continue;
		
		if(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			list.push_back(directory + ToUTF8(ffd.cFileName) + '/');
	} while(FindNextFileW(hFind, &ffd));
	
	FindClose(hFind);
#else
	DIR *dir = opendir(directory.c_str());
	if(!dir)
		return list;
	
	while(true)
	{
		dirent *ent = readdir(dir);
		if(!ent)
			break;
		// Skip dotfiles (including "." and "..").
		if(ent->d_name[0] == '.')
			continue;
		
		string name = directory + ent->d_name;
		// Don't assume that this operating system's implementation of dirent
		// includes the t_type field; in particular, on Windows it will not.
		struct stat buf;
		stat(name.c_str(), &buf);
		bool isDirectory = S_ISDIR(buf.st_mode);
		
		if(isDirectory)
		{
			if(name.back() != '/')
				name += '/';
			list.push_back(name);
		}
	}
	
	closedir(dir);
#endif	
	return list;
}



vector<string> Files::RecursiveList(const string &directory)
{
	vector<string> list;
	RecursiveList(directory, &list);
	return list;
}



void Files::RecursiveList(string directory, vector<string> *list)
{
	if(directory.empty() || directory.back() != '/')
		directory += '/';
	
#if defined _WIN32
	WIN32_FIND_DATAW ffd;
	HANDLE hFind = FindFirstFileW(ToUTF16(directory + '*').c_str(), &ffd);
	if(hFind == INVALID_HANDLE_VALUE)
		return;
	
	do {
		if(!ffd.cFileName || ffd.cFileName[0] == '.')
			continue;
		
		if(!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			list->push_back(directory + ToUTF8(ffd.cFileName));
		else
			RecursiveList(directory + ToUTF8(ffd.cFileName) + '/', list);
	} while(FindNextFileW(hFind, &ffd));
	
	FindClose(hFind);
#else
	DIR *dir = opendir(directory.c_str());
	if(!dir)
		return;
	
	while(true)
	{
		dirent *ent = readdir(dir);
		if(!ent)
			break;
		// Skip dotfiles (including "." and "..").
		if(ent->d_name[0] == '.')
			continue;
		
		string name = directory + ent->d_name;
		// Don't assume that this operating system's implementation of dirent
		// includes the t_type field; in particular, on Windows it will not.
		struct stat buf;
		stat(name.c_str(), &buf);
		bool isRegularFile = S_ISREG(buf.st_mode);
		bool isDirectory = S_ISDIR(buf.st_mode);
		
		if(isRegularFile)
			list->push_back(name);
		else if(isDirectory)
			RecursiveList(name + '/', list);
	}
	
	closedir(dir);
#endif
}



bool Files::Exists(const string &filePath)
{
#if defined _WIN32
	struct _stat buf;
	return !_wstat(ToUTF16(filePath).c_str(), &buf);
#else
	struct stat buf;
	return !stat(filePath.c_str(), &buf);
#endif
}



time_t Files::Timestamp(const std::string &filePath)
{
#if defined _WIN32
	struct _stat buf;
	_wstat(ToUTF16(filePath).c_str(), &buf);
#else
	struct stat buf;
	stat(filePath.c_str(), &buf);
#endif
	return buf.st_mtime;
}



void Files::Copy(const string &from, const string &to)
{
#if defined _WIN32
	CopyFileW(ToUTF16(from).c_str(), ToUTF16(to).c_str(), false);
#else
	Write(to, Read(from));
#endif
}



void Files::Move(const string &from, const string &to)
{
#if defined _WIN32
	MoveFileExW(ToUTF16(from).c_str(), ToUTF16(to).c_str(), MOVEFILE_REPLACE_EXISTING);
#else
	rename(from.c_str(), to.c_str());
#endif
}



void Files::Delete(const string &filePath)
{
#if defined _WIN32
	DeleteFileW(ToUTF16(filePath).c_str());
#else
	unlink(filePath.c_str());
#endif
}



// Get the filename from a path.
string Files::Name(const string &path)
{
	// string::npos = -1, so if there is no '/' in the path this will
	// return the entire string, i.e. path.substr(0).
	return path.substr(path.rfind('/') + 1);
}



FILE *Files::Open(const string &path, bool write)
{
#if defined _WIN32
	return _wfopen(ToUTF16(path).c_str(), write ? L"w" : L"rb");
#else
	return fopen(path.c_str(), write ? "wb" : "rb");
#endif
}



string Files::Read(const string &path)
{
	File file(path);
	return Read(file);
}



string Files::Read(FILE *file)
{
	string result;
	if(!file)
		return result;
	
	// Find the remaining number of bytes in the file.
	size_t start = ftell(file);
	fseek(file, 0, SEEK_END);
	size_t size = ftell(file) - start;
	// Reserve one extra byte because DataFile appends a '\n' to the end of each
	// file it reads, and that's the most common use of this function.
	result.reserve(size + 1);
	result.resize(size);
	fseek(file, start, SEEK_SET);
	
	// Read the file data.
	size_t bytes = fread(&result[0], 1, result.size(), file);
	if(bytes != result.size())
		throw runtime_error("Error reading file!");
	
	return result;
}



void Files::Write(const string &path, const string &data)
{
	File file(path, true);
	Write(file, data);
}



void Files::Write(FILE *file, const string &data)
{
	if(!file)
		return;
	
	fwrite(&data[0], 1, data.size(), file);
}



void Files::LogError(const string &message)
{
	lock_guard<mutex> lock(errorMutex);
	cerr << message << endl;
}



// Simple GetBasePath (not very portable).
char *Files::GetBasePath()
{
#ifndef _WIN32
	char *retval = NULL;
	/* is a Linux-style /proc filesystem available? */
	if(access("/proc", F_OK) == 0)
	{
#if defined(__FREEBSD__)
		retval = readSymlink("/proc/curproc/file");
#elif defined(__NETBSD__)
		retval = readSymlink("/proc/curproc/exe");
#else
		retval = readSymlink("/proc/self/exe");
#endif
	}

	if(retval != NULL)
	{
		char *ptr = strrchr(retval, '/');
		if(ptr != NULL)
			*(ptr+1) = '\0';
		else
		{
			free(retval);
			retval = NULL;
		}
	}
	
	if(retval != NULL)
	{
		/* try to shrink buffer... */
		char *ptr = static_cast<char *>(realloc(retval, strlen(retval) + 1));
		if (ptr != NULL)
			retval = ptr;  /* oh well if it failed. */
	}

	return retval;
#elif _WIN32
#ifndef _pgmptr
	vector<LPSTR> pathBuf;
	DWORD copied = 0;
	do {
		pathBuf.resize(pathBuf.size() + MAX_PATH);
		copied = GetModuleFileName(NULL, pathBuf[0], pathBuf.size());
	} while(copied >= pathBuf.size());

	pathBuf.resize(copied);

	wstring path(pathBuf.begin(), pathBuf.end());
	string sPath = ToUTF8(path.c_str());
	vector<char> writeable(sPath.begin(), sPath.end());
	writeable.push_back('\0');
	return &*writeable.begin();
#else
	return _pgmptr;
#endif
#endif
}


#ifndef _WIN32
char *Files::readSymlink(const char *path)
{
	char *retval = NULL;
	ssize_t len = 64;
	ssize_t rc = -1;

	while(1)
	{
		char *ptr = (char *) realloc(retval, (size_t) len);
		if (ptr == NULL)
			break;
		
		retval = ptr;
		
		rc = readlink(path, retval, len);
		if(rc == -1)
			break;  /* not a symlink, i/o error, etc. */
		else if(rc < len)
		{
			retval[rc] = '\0';  /* readlink doesn't null-terminate. */
			return retval;  /* we're good to go. */
		}
		
		len *= 2;  /* grow buffer, try again. */
	}

	free(retval);
	return NULL;
}
#endif