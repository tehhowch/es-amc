/* main.cpp
Copyright (c) 2017 by tehhowch

AntiMissileComparator is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the Free
Software Foundation, either version 3 of the License, or (at your option) any
later version.

AntiMissileComparator is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
details.
*/

#include "AntiMissile.h"
#include "DataFile.h"
#include "Files.h"
#include "Missile.h"

#include <iostream>
#include <map>
#include <set>

using namespace std;

// Read the data from the specified file/directory.
void LoadData(const char * const *argv);
void LoadFile(const string &path);

// Print data to standard output.
void PrintAntiMissiles();
void PrintHelp();
void PrintMissiles();
void PrintData();

namespace {
	// These are the sets of possible systems.
	std::map<std::string, AntiMissile> ams;
	std::map<std::string, Missile> missiles;
	// Data from testing.
	std::map<AntiMissile *, std::map<Missile *, std::map<size_t, std::pair<int, int>>>> data;
}


int main(int argc, char *argv[])
{
	// If the user wants to know args, print the args.
	for(const char * const *it = argv + 1; *it; ++it)
		if((*it)[0] == '-')
		{
			string arg = *it;
			if(arg == "-h" || arg == "--help")
			{
				PrintHelp();
				return 0; // Once we can respond to input, this is unnecessary.
			}
			continue;
		}
	
	// Read in files from the /data directory.
	LoadData(argv);
	
	/*
		Do stuff.
		Namely, get the test matrix from user input.
		Or, run the default test matrix.
		Then, report results.
	 */
	
	return 0;
}



void PrintAntiMissiles()
{
	if(ams.empty())
	{
		cout << "No anti-missiles loaded." << endl;
		return;
	}
	cout << "\n Anti-Missile " << '\t' << "Strength" << '\t' << "Range" << '\t' << "Shots/sec" << '\n';
	for(auto &it : ams)
		cout << it.first << '\t' << it.second.Strength() << '\t' << it.second.Range() << '\t' 
			<< it.second.ShotsPerSecond() << '\n';
	cout << '\n';
	cout.flush();
}



void PrintMissiles()
{
	if(missiles.empty())
	{
		cout << "No missiles loaded." << endl;
		return;
	}
	
	cout << "\n Missile " << '\t' << "Strength" << '\t' << "Velocity" << '\t' << "TriggerRadius" << '\n';
	for(auto &it : missiles)
	{
		cout << it.first << '\t' << it.second.Strength() << '\t' << it.second.Velocity() << '\t' << it.second.TriggerRadius() << '\n';
	}
	cout << '\n';
	cout.flush();
}



void PrintData()
{
	if(data.empty())
	{
		cout << "No data collected." << endl;
		return;
	}
	cout << "Anti-Missile" << '\t' << "Missile" << '\t' << "Angle (deg)" << '\t' << "Win Rate" << '\n';
	for(auto &am : data)
		for(auto &missile : am.second)
			for(auto &angle : missile.second)
				cout << am.first->Name() << '\t' << missile.first->Name() << '\t'
					<< to_string(angle.first) + " deg" << '\t'
					<< static_cast<double>(angle.second.second) * 100 /  angle.second.second << "%" << '\n';
	cout.flush();
}



void LoadData(const char * const *argv)
{
	Files::Init(argv);
	
	vector<string> dataFiles = Files::RecursiveList(Files::Data());
	for(const string &path : dataFiles)
		LoadFile(path);
	
	
	// Respond to user input requests now that data has been loaded.
	for(const char * const *it = argv + 1; *it; ++it)
	{
		if((*it)[0] == '-')
		{
			string arg = *it;
			if(arg == "-a" || arg == "--ams" || arg == "--antimissiles")
				PrintAntiMissiles();
			if(arg == "-m" || arg == "--missiles")
				PrintMissiles();
			continue;
		}
	}
}



void PrintHelp()
{
	cerr << endl;
	cerr << " Command line options:" << endl;
	cerr << "    -h, --help: 		print this help message." << endl;
	cerr << "    -a, --ams, --antimissiles: print table of antimissile attributes." << endl;
	cerr << "    -m, --missiles: 		print table of ship attributes." << endl;
	cerr << "    -r, --resources <path>: 	load resources from given directory." << endl;
	cerr << endl;
	cerr << "Report bugs to: tehhowchmh@gmail.com" << endl;
	cerr << endl;
}



void LoadFile(const string &path)
{
	// This is an ordinary file. Check to see if it is an image.
	if(path.length() < 4 || path.compare(path.length() - 4, 4, ".txt"))
		return;
	
	DataFile data(path);
	for(const DataNode &node : data)
	{
		const string &key = node.Token(0);
		if((key == "am" || key == "antimissile" || key == "anti-missile") && node.Size() > 1)
			ams[node.Token(1)].Load(node);
		if((key == "missile" || key == "ms") && node.Size() > 1)
			missiles[node.Token(1)].Load(node);
	}
}
