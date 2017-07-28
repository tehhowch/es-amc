/* AntiMissile.cpp
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

#include "DataNode.h"
#include "Missile.h"
#include "Random.h"

#include <cmath>

using namespace std;

void AntiMissile::Load(const DataNode &node)
{
	if(node.Size() > 1)
		name = node.Token(1);
	
	for(const DataNode &child : node)
	{
		if((child.Token(0) == "strength" || child.Token(0) == "anti-missile strength") && child.Size() > 1)
			strength = child.Value(1);
		else if(child.Token(0) == "range" && child.Size() > 1)
			range = child.Value(1);
		else if(child.Token(0) == "reload" && child.Size() > 1)
			reload = child.Value(1);
		else
			child.PrintTrace("Skipping unrecognized attribute:");
	}
	
	Initialize();
}



void AntiMissile::Initialize(const Point position)
{
	this->position = position;
	cooldown = 0;
}



void AntiMissile::SetPosition(const Point position)
{
	this->position = position;
}



void AntiMissile::SetVelocity(const Point velocity)
{
	this->velocity = velocity;
}



void AntiMissile::Step()
{
	position += velocity;
	cooldown = max(0, cooldown - 1);
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



const string &AntiMissile::Name() const
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



const int AntiMissile::Strength() const
{
	return strength;
}
