/* AntiMissile.h
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

#ifndef ANTI_MISSILE_H_
#define ANTI_MISSILE_H_

#include "Point.h"

#include <string>

class DataNode;
class Missile;
class Point;

class AntiMissile {
public:
	void Load(const DataNode &node);
	void Initialize(const Point position = Point());
	void SetPosition(const Point position);
	void SetVelocity(const Point velocity = Point());
	
	// Decrements cooldown and move the system by its current velocity.
	void Step();
	// Return true if the system is ready to fire and the missile is in range.
	bool CanFire(const Missile &missile) const;
	// Reset the cooldown and return the strength of the attack: [0, strength).
	int Fire();
	
	// Get properties of this anti-missile outfit.
	const std::string &Name() const;
	const Point &Position() const;
	const double Range() const;
	const double ShotsPerSecond() const;
	const int Strength() const;
	
private:
	std::string name;
	Point position;
	Point velocity;

	int cooldown = 5000;
	double range = 0.;
	int reload = 1;
	int strength = 0;
};




#endif // ANTI_MISSILE_H_ 
