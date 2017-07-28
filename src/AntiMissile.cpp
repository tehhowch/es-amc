/*
Class describing an anti-missile.
 */

#include "AntiMissile.h"

#include "Missile.h"
#include "Random.h"

#include <cmath>
#include <string>

using namespace std;

void AntiMissile::Initialize()
{
	cooldown = 0;
}



bool AntiMissile::CanFire(const Missile &missile) const
{
	return !cooldown && range >= (missile.Position() - position).Length();
}


// Reset the AM's cooldown and return the strength check used to see if
// the projectile that was fired at was killed.
int AntiMissile::Fire()
{
	cooldown = reload;
	return Random::Int(strength);
}



const std::string &AntiMissile::Name() const
{
	return name;
}



const Point &AntiMissile::Position() const
{
	return position;
}



const double AntiMissile::Range() const
{
	return range;
}



const double AntiMissile::ShotsPerSecond() const
{
	return 60. / max(reload, 1);
}



void AntiMissile::Step()
{
	cooldown = std::max(0, cooldown - 1);
}



const int AntiMissile::Strength() const
{
	return strength;
}
