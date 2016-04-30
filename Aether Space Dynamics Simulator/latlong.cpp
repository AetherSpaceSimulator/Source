#include "stdafx.h"
#include "latlong.h"

LatLongClass::LatLongClass(std::vector<objectClass>& object_) :object(object_)
{
}

LatLongClass::~LatLongClass()
{
}

LatLongClass::latlon LatLongClass::cartesian_to_geodetic2(int objeto_orbitado, vec3 posic)
{
	int orbited_ = object[control].getOrbiting();
	double lat = 0, lon = 0, alt = 0;
	double x = posic.x;
	double y = posic.y;
	double z = posic.z;
	double semi_major_axis = 0; // semi-major axis

	if (object[orbited_].getFlattening() > 0.0)
	{
		if (object[orbited_].getEquatorialRadius() < object[orbited_].getPolarRadius())
			semi_major_axis = object[orbited_].getPolarRadius() * 1000;

		else semi_major_axis = object[orbited_].getEquatorialRadius() * 1000;
	}

	else
	{
		semi_major_axis = object[orbited_].getScale() / 2;
	}

	double _f = object[orbited_].getFlattening();
	// flatenning = 1 - (semi_minor_axis / semi_major_axis);

	const double eps_h = 1e-3;
	const double eps_b = 1e-10;
	const int imax = 1;  // default: 30

	double q = sqrt(x * x + y * y);
	lon = atan2(y, x);
	alt = 0.;

	double N = semi_major_axis;
	double _e2 = _f * (2. - _f);
	for (int i = 0; i < imax; i++)
	{
		double h0 = alt;
		double b0 = lat;
		lat = atan(z / q / (1 - _e2 * N / (N + alt)));
		double sin_b = sin(lat);
		N = semi_major_axis / sqrt(1. - _e2 * sin_b * sin_b);
		alt = q / cos(lat) - N;
		//cout << i << ": "<< b << ", " << h << endl;
		if (fabs(alt - h0) < eps_h && fabs(lat - b0) < eps_b) return{ -lat, lon, alt }; // invertimos la latitud no se por que, pero asi salen bien los calculos
	}

	return{ -lat, lon, alt }; // invertimos la latitud no se por que, pero asi salen bien los calculos
}

vec3 LatLongClass::geodetic_to_cartesian(int orbited_, double lati, double longi, double alti)
{
	longi *= -1; // invertimos la longitud no se por que, pero asi salen bien los calculos
	lati = ToRadians_d(lati);
	longi = ToRadians_d(longi);
	double xx;
	double yy;
	double zz;
	double semi_major_axis = 0;

	if (object[orbited_].getFlattening() > 0.0)
	{
		if (object[orbited_].getEquatorialRadius() < object[orbited_].getPolarRadius())
			semi_major_axis = object[orbited_].getPolarRadius() * 1000;

		else semi_major_axis = object[orbited_].getEquatorialRadius() * 1000;
	}

	else
	{
		semi_major_axis = object[orbited_].getScale() / 2;
	}

	double slat = sin(lati);
	double clat = cos(lati);

	double r = semi_major_axis / sqrt(1.0 - (object[orbited_].getFlattening() * (2 - object[orbited_].getFlattening())) * slat * slat);

	xx = (r + alti) * clat * cos(longi);
	yy = (r + alti) * clat * sin(longi);
	zz = (r * (1 - (object[orbited_].getFlattening() * (2 - object[orbited_].getFlattening()))) + alti) * slat;

	vec3 rott = vector_rotation_by_quaternion(
	{ xx, zz, -yy },
	~object[orbited_].getTotalRotation());

	return rott;
	// -33.858362, 151.214876, 10
}