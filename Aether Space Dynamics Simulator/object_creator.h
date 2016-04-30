#ifndef _OBJECTCREATOR_H_
#define _OBJECTCREATOR_H_

#include "stdafx.h"
#include "object.h"
#include "ephemeris.h"

class ObjectCreatorClass
{
public:
	ObjectCreatorClass(std::vector<objectClass>& object_,
		bool& simplified_atmosphere,
		int& total_initial_setup_objects,
		float& starshine,
		float& SHADOWMAP_DEPTH,
		std::vector <std::string>& texture_filename,
		int& default_textures,
		int& textures_number,
		std::vector <std::string>& model_filename,
		int& default_models,
		int& models_number,
		double& minimum_gravity_influence_weight,
		double& calculations_epoch,
		double& true_julian);

	~ObjectCreatorClass();

	bool create_objects();

private:

	int object_count_, objeto_count;
	std::string focus_;
	int clouds_texture_;
	bool has_clouds_;

	bool create_natural_object(std::ifstream& objects_file, std::vector <std::string>* nombre_objeto);
	bool create_spaceship(std::ifstream& objects_file, std::vector <std::string>* nombre_objeto);
	bool read_specs_file(std::vector <objectClass>& object, std::string string_, int object_number_);
	bool read_special_properties(std::vector<objectClass>& calling_object, int objeto, std::string parent_name, std::string string_);
	void dock_object(std::vector <objectClass>& object, int n, int orbited_);
	bool create_dependant_object(std::vector <objectClass>& object, std::string specifications_file, int parent_);

	bool& simplified_atmosphere;
	int& total_initial_setup_objects;
	float& starshine;
	float& SHADOWMAP_DEPTH;
	std::vector <std::string>& texture_filename;
	int& default_textures;
	int& textures_number;
	std::vector <std::string>& model_filename;
	int& default_models;
	int& models_number;
	double& minimum_gravity_influence_weight;
	double& calculations_epoch;
	double& true_julian;

	std::vector<objectClass> objeto;
	std::vector<objectClass>& object;
	Ephemeris_Class* ephemeris_object;
};

#endif