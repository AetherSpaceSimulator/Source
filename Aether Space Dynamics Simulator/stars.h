#ifndef _STARCLASS_H_
#define _STARCLASS_H_

#include "stdafx.h"

class StarClass
{
public:

	struct Star_Data
	{
		std::string name;
		vec3 position;
		Vector3 position_b;
		float magnitude;
		float color;
		std::string BF;
		double distance;
	};

	struct Constellation_Data
	{
		Constellation_Data()
		{
			name = "";
			lines_in_constellation = 0;
			points_positions = nullptr;
		};

		~Constellation_Data()
		{
			SAFE_DELETE_ARRAY(points_positions);
		};

		std::string name;
		int lines_in_constellation;
		Vector3* points_positions;
	};

	StarClass();
	StarClass(const StarClass&);
	~StarClass();

	bool Initialize(std::string star_catalog, std::string constellations);
	int get_total_stars();
	Star_Data& get_star_data(int n);

	int get_total_constellations();
	Constellation_Data& get_constellation_data(int n);

private:

	bool load_stars(std::string filename);
	bool load_constellations(std::string filename);

	int total_stars, total_constellations;
	Star_Data* star_data;
	Constellation_Data* constellation;
};

#endif