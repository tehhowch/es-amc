/* Missile.h
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

#ifndef MISSILE_H_
#define MISSILE_H_

#include "Point.h"

#include <string>

class DataNode;
class Point;

class Missile {
public:
	void Load(const DataNode &node);
	
	// Create this missile at the specified position.
	void SetPosition(const Point position);
	// Fire this missile at the target. Returns the launch point.
	Point Launch(const Point &target, double radius = 1000.);
	// Move the missile. Returns true for a missile 'victory.'
	bool Move(const Point &target);
	
	// Get the properties of this missile.
	const std::string &Name() const;
	const Point &Position() const;
	const double TriggerRadius() const;
	const int Strength() const;
	const double Velocity() const;
	
private:
	std::string name;
	Point position;
	
	double accel = 0.;
	double drag = 1.;
	double velocity = 0.;
	int strength = 1;
	double trigger = 0.;
	
};

#endif // MISSILE_H_
