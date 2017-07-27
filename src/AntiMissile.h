/*
Class describing an anti-missile.
 */

#ifndef ANTI_MISSILE_H_
#define ANTI_MISSILE_H_

#include "Point.h"

#include <string>

class Missile;
class Point;

class AntiMissile {
public:
	void Initialize();
	bool CanFire(const Missile &missile) const;
	int Fire();
	void Step();
	
	// Get properties of this anti-missile outfit.
	const std::string &Name() const;
	const Point &Position() const;
	const double Range() const;
	const double ShotsPerSecond() const;
	const int Strength() const;
	
private:
	std::string name;
	Point position;

	int strength = 0;
	double range = 0.;
	int reload = 1;
	int cooldown = 5000;
};




#endif // ANTI_MISSILE_H_ 
