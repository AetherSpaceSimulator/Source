#include "stdafx.h"
#include "stars.h"

StarClass::StarClass()
{
	total_stars = 0;
	total_constellations = 0;

	constellation = nullptr;
	star_data = nullptr;
}

StarClass::StarClass(const StarClass& other)
{
}


StarClass::~StarClass()
{
	SAFE_DELETE_ARRAY(constellation);
	SAFE_DELETE_ARRAY(star_data);
}

bool StarClass::Initialize(std::string star_catalog, std::string constellations)
{
	if (!load_stars(star_catalog)) return false;
	if (!load_constellations(constellations)) return false;

	return true;
}

bool StarClass::load_constellations(std::string constellations_)
{
	char input;
	std::string star_name;

	std::ifstream c_file;
	c_file.open(constellations_);

	if (c_file.is_open())
	{
		c_file.get(input);
		while (input != ':')
		{
			c_file.get(input);
		}
		c_file >> total_constellations;

		constellation = new Constellation_Data[total_constellations];

		for (int n = 0; n < total_constellations; n++)
		{
			c_file.get(input);
			while (input != ':')
			{
				c_file.get(input);
			}
			c_file >> constellation[n].name;
			/////////////////////////////////////////////////////////

			c_file.get(input);
			while (input != ':')
			{
				c_file.get(input);
			}
			c_file >> constellation[n].lines_in_constellation;
			constellation[n].points_positions = new Vector3[constellation[n].lines_in_constellation];
			/////////////////////////////////////////////////////////
			for (int nn = 0; nn < constellation[n].lines_in_constellation; nn++)
			{
				c_file >> star_name >>
					constellation[n].points_positions[nn].x >> constellation[n].points_positions[nn].y >> constellation[n].points_positions[nn].z;
			}
		}

		c_file.close();
	}

	else
	{
		std::string message = "Could not open constellations data file: " + constellations_;
		MessageBoxA(m_hwnd, message.c_str(), "Error", MB_OK);
		return false;
	}


	return true;
}

bool StarClass::load_stars(std::string filename)
{
	std::string texto;
	std::string dato[37];
	int star_number = 0;

	std::ifstream data_file;
	data_file.open(filename);

	if (data_file.is_open())
	{
		while (getline(data_file, texto))
		{
			total_stars++;
		}

		star_data = new Star_Data[total_stars];

		data_file.clear();
		data_file.seekg(0, std::ios::beg);

		while (getline(data_file, texto))
		{
			int data = 0;
			dato[0] = "";
			for (int n = 0; n < int(texto.length()); n++)
			{
				if (texto.at(n) == ',')
				{
					data++;
					dato[data] = "";
				}
				else dato[data] += texto.at(n);
			}

			star_data[star_number].position = { atof(dato[17].c_str()), atof(dato[18].c_str()), atof(dato[19].c_str()) };

			star_data[star_number].position_b = vec_to_d3d(star_data[star_number].position);
			star_data[star_number].position_b = Vector3{ -star_data[star_number].position_b.x, star_data[star_number].position_b.z, -star_data[star_number].position_b.y };
			star_data[star_number].position_b.Normalize();
			star_data[star_number].position_b *= 10.0f;


			star_data[star_number].position = { -star_data[star_number].position.x, star_data[star_number].position.z, -star_data[star_number].position.y };

			star_data[star_number].magnitude = float(atof(dato[13].c_str()));
			star_data[star_number].color = float(atof(dato[16].c_str()));
			star_data[star_number].name = dato[6];
			star_data[star_number].BF = dato[5];
			star_data[star_number].distance = atof(dato[9].c_str());

			star_number++;			
		}

		data_file.close();
	}

	else return false;

	return true;
}

int StarClass::get_total_stars()
{
	return total_stars;
}

StarClass::Star_Data& StarClass::get_star_data(int n)
{
	return star_data[n];
}

int StarClass::get_total_constellations()
{
	return total_constellations;
}

StarClass::Constellation_Data& StarClass::get_constellation_data(int n)
{
	return constellation[n];
}