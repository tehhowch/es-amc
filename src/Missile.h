/*
Class describing a missile
 */

#ifndef MISSILE_H_
#define MISSILE_H_

#include "Point.h"

#include <string>

class Point;

class Missile {
public:
	// Fire this missile at the origin.
	Point Launch();
	
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
