/*
Class describing a missile
 */

#include "Missile.h"


#include "Point.h"
#include "Random.h"

#include <string>

using namespace std;


// Fire at the origin from a random point along the unit circle boundary at the unit circle.
Point Missile::Launch()
{
	return Point();
}



const std::string &Missile::Name() const
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
