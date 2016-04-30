#ifndef _LATLONG_H_
#define _LATLONG_H_

#include "stdafx.h"
#include "object.h"

class LatLongClass
{
public:
	LatLongClass(std::vector<objectClass>& object_);
	~LatLongClass();

	struct latlon { double lat, lon, h; };

	latlon cartesian_to_geodetic2(int objeto_orbitado, vec3 posic);
	vec3 geodetic_to_cartesian(int objeto_orbitado, double lati, double longi, double alti);

private:

	std::vector<objectClass>& object;
};

#endif