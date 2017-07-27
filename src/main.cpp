// C++ program to simulate various Endless-Sky antimissile systems
// that are firing upon an arbitrary projectile.

#include "AntiMissile.h"
#include "Missile.h"
#include "Point.h"
#include "Random.h"

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

using namespace std;

// Read the data from the specified file in this directory.
void ReadData(std::string path);

// Print data to standard output.
void PrintMissiles();
void PrintAntiMissiles();
void PrintData();
void PrintString(std::string output);

std::set<AntiMissile *> ams;
std::set<Missile *> missiles;
std::map<AntiMissile *, std::map<Missile *, std::map<size_t, std::pair<int, int>>>> data;

int main(){
	PrintMissiles();
	return 0;
}


void PrintMissiles()
{
	if(missiles.empty())
	{
		cout << "No missiles loaded." << endl;
		return;
	}
	
	cout << "Missile" << '\t' << "Strength" << '\t' << "Velocity" << '\t' << "TriggerRadius" << endl;
	for(auto &it : missiles)
	{
		cout << it->Name() << '\t' << it->Strength() << '\t' << it->Velocity() << '\t' << it->TriggerRadius() << '\n';
	}
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



void ReadData(std::string path)
{
	// Open 'path' as a file and read from it.
	ifstream file(path);

	bool isMissile = false;
	if(file.is_open())
	{
		// Read the header line.
		string headerLine;
		getline(file, headerLine);
		
		// The file data type is determined by the number of fields: 4 = antimissile
		file.close()
	}
}
