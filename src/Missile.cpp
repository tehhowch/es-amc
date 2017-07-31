/* Missile.cpp
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

#include "Missile.h"

#include "DataNode.h"
#include "pi.h"
#include "Point.h"
#include "Random.h"

#include <cmath>

using namespace std;

void Missile::Load(const DataNode &node)
{
	if(node.Size() > 1)
		name = node.Token(1);
	
	for(const DataNode &child : node)
	{
		if((child.Token(0) == "strength" || child.Token(0) == "missile strength") && child.Size() > 1)
			strength = child.Value(1);
		else if((child.Token(0) == "accel" || child.Token(0) == "acceleration") && child.Size() > 1)
			accel = child.Value(1);
		else if(child.Token(0) == "drag" && child.Size() > 1)
			drag = child.Value(1);
		else if(child.Token(0) == "velocity" && child.Size() > 1)
			velocity = child.Value(1);
		else if((child.Token(0) == "trigger" || child.Token(0) == "trigger radius") && child.Size() > 1)
			trigger = child.Value(1);
		else
			child.PrintTrace("Skipping unrecognized attribute:");
	}
	
	if(!velocity)
		velocity = drag ? accel / drag : accel;
}



void Missile::SetPosition(const Point position)
{
	this->position = position;
}



// Create a from a random point along the circle boundary with the given radius.
Point Missile::Launch(const Point &target, double radius)
{
	double theta = Random::Real() * 2 * PI;
	this->position = radius * Point(cos(theta), sin(theta)) + target;
	
	return position;
}



// Advance the missile 1 step towards the target's current position.
// Return true if the target was hit (distance < 1 or < trigger).
bool Missile::Move(const Point &target)
{
	Point tv = target - position;
	double d = Velocity();
	
	// The traveled distance will take us to or beyond the target.
	if(tv.Length() <= d)
		return true;
	
	// Advance d distance in the direction of the target.
	position += tv.Unit() * d;
	double range = (target - position).Length();
	if(range < trigger)
		return true;
	else if(range < 1)
		return true;
		
	return false;
}



const string &Missile::Name() const
{
	return name;
}



const Point &Missile::Position() const
{
	return position;
}


const double Missile::TriggerRadius() const
{
	return trigger;
}



const int Missile::Strength() const
{
	return strength;
}



const double Missile::Velocity() const
{
	if(drag)
		return accel / drag;
	
	return velocity;
}
