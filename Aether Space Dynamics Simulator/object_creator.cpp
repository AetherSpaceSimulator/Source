#include "stdafx.h"
#include "object_creator.h"

ObjectCreatorClass::ObjectCreatorClass(std::vector<objectClass>& object_,
	bool& simplified_atmosphere_,
	int& total_initial_setup_objects_,
	float& starshine_,
	float& SHADOWMAP_DEPTH_,
	std::vector <std::string>& texture_filename_,
	int& default_textures_,
	int& textures_number_,
	std::vector <std::string>& model_filename_,
	int& default_models_,
	int& models_number_,
	double& minimum_gravity_influence_weight_,
	double& calculations_epoch_,
	double& true_julian_)
	: object(object_),
	simplified_atmosphere(simplified_atmosphere_),
	total_initial_setup_objects(total_initial_setup_objects_),
	starshine(starshine_),
	SHADOWMAP_DEPTH(SHADOWMAP_DEPTH_),
	texture_filename(texture_filename_),
	default_textures(default_textures_),
	textures_number(textures_number_),
	model_filename(model_filename_),
	default_models(default_models_),
	models_number(models_number_),
	minimum_gravity_influence_weight(minimum_gravity_influence_weight_),
	calculations_epoch(calculations_epoch_),
	true_julian(true_julian_)
{
	objeto_count = -1;
	object_count_ = 0;
	focus_ = "";
	clouds_texture_ = 0;
	has_clouds_ = false;

	ephemeris_object = new Ephemeris_Class;
	ephemeris_object->Initialize();
}

ObjectCreatorClass::~ObjectCreatorClass()
{
	SAFE_DELETE(ephemeris_object);
}

bool ObjectCreatorClass::create_objects()
{
	char input_ = ' ';
	std::string string_ = "";
	double double_ = 0.0;
	float float_ = 0.f;
	int int_ = 0;

	std::vector <std::string> nombre_objeto;

	std::ifstream objects_file;
	objects_file.open("objects/objects.txt");

	// If it could not open the file then exit.
	if (objects_file.fail())
	{
		MessageBoxA(m_hwnd, "Could not open objects file", "Error", MB_OK);
		return false;
	}

	if (objects_file.is_open())
	{
		while (!objects_file.eof())
		{
			objects_file >> string_;
			if (string_ == "Object:")
			{
				total_objects++;
				objects_file.get(input_);
				getline(objects_file, string_);
				nombre_objeto.push_back(string_);
			}

			else if (string_ == "---Atmosphere")
			{
				// creamos dos esferas atmosfericas sobre el planeta, una para ser vista del reves y otra de frente
				if (!simplified_atmosphere) total_objects += 2;
				else total_objects += 1;
			}

			else if (string_ == "------Clouds") total_objects += 1;

			else if (string_ == "---Innermost") total_objects += 1;
		}

		objects_file.clear();
		objects_file.seekg(0, std::ios::beg);

		total_objects += total_initial_setup_objects; // add the starfield, the astroclock and any other aditional special object

		// creamos los espacios para recibir los objetos
		for (int n = 0; n < total_objects; n++) object.emplace_back();

		// Create the startfield object
		object[0].setName("Starfield");
		object[0].setOriginalName(object[0].getName());
		object[0].setPosition_b({ 0, 0, 0 });
		object[0].setModel(5);
		object[0].setScale_b(1.6e7);
		object[0].setTexture1(1);
		object[0].setIlumination(starshine);
		object[0].setAffects(0);
		object[0].setInitialRotation(ToRadians_d(180.0));
		object[0].setType(0);

		// Create the astro-clock
		object[1].setName("AstroClock");
		object[1].setOriginalName(object[1].getName());
		object[1].setPosition_b({ 0, 0, 0 });
		object[1].setModel(5);
		object[1].setScale_b(100);
		object[1].setTexture1(1);
		object[1].setIlumination(1);
		object[1].setAffects(0);
		object[1].setInitialRotation(0.0);
		object[1].setType(0);
		object[1].setSiderealRotationPeriod(24.0);
		object[1].setAngulosY(ToRadians_d(360.0 / (object[1].getSiderealRotationPeriod() * 3600.0)));

		// Create the shadow receiver
		object[2].setName("Shadow receiver");
		object[2].setOriginalName(object[2].getName());
		object[2].setPosition_b({ 0, 0, 0 });
		object[2].setModel(7);
		object[2].setScale_b(SHADOWMAP_DEPTH*1.2f);
		object[2].setAffects(0);
		object[2].setType(0);

		// Create the landing pad
		object[3].setName("Landing pad");
		object[3].setOriginalName(object[3].getName());
		object[3].setPosition_b({ 0, 0, 0 });
		object[3].setModel(9);
		object[3].setScale_b(1);
		object[3].setAffects(0);
		object[3].setType(9);
		object[3].setVisible(false);
		object[3].setCastsShadows(false);

		//////////////////////////////////////////////////////////////////
		//					Read in the focus object					//
		objects_file.get(input_);
		while (input_ != ':') objects_file.get(input_);
		objects_file.get(input_);
		getline(objects_file, focus_);
		//																//
		//////////////////////////////////////////////////////////////////

		// comenzamos a cargar los objetos que encontramos en el archivo, empezamos a contar desde el ultimo objeto especial cargado antes
		object_count_ = total_initial_setup_objects - 1;
		while (!objects_file.eof())
		{
			objects_file >> string_;

			// si encontramos el texto "Object:" entonces cargar un nuevo objeto
			if (string_ == "Object:")
			{
				// leemos y cargamos el nombre
				object_count_++;
				objects_file.get(input_);
				getline(objects_file, string_);
				object[object_count_].setName(string_);
				object[object_count_].setOriginalName(string_);
				////////////////////////////////////////////////////////////////////////////////

				// leemos y establecemos el tipo de objeto
				objects_file.get(input_);
				while (input_ != ':')
				{
					objects_file.get(input_);
				}

				objects_file.get(input_);
				getline(objects_file, string_);

				if (string_ == "Star")
				{
					object[object_count_].setType(1);
					object[object_count_].setIlumination(1);
				}
				else if (string_ == "Planet") object[object_count_].setType(2);
				else if (string_ == "Natural satellite") object[object_count_].setType(3);
				else if (string_ == "Spacecraft") object[object_count_].setType(4);
				else if (string_ == "Ground object") object[object_count_].setType(10);
				else if (string_ == "Landing surface") object[object_count_].setType(11);
				////////////////////////////////////////////////////////////////////////////////

				// si el objeto es una estrella, planeta o satelite natural
				if (object[object_count_].getType() == 1 || object[object_count_].getType() == 2 || object[object_count_].getType() == 3)
				{
					if (!create_natural_object(objects_file, &nombre_objeto))
					{
						MessageBoxA(m_hwnd, "Could not create natural object", object[object_count_].getName().c_str(), MB_OK);
						return false;
					}
				}

				// si el objeto es artificial
				else if (object[object_count_].getType() == 4 || object[object_count_].getType() == 10 || object[object_count_].getType() == 11)
				{
					if (!create_spaceship(objects_file, &nombre_objeto))
					{
						MessageBoxA(m_hwnd, "Could not create spaceship object", object[object_count_].getName().c_str(), MB_OK);
						return false;
					}
				}
			}
		}

		for (int n = 0; n < total_objects; n++) // go through all the existing objects
		{
			// leave out atmospheres, clouds layers and rings
			if (object[n].getType() != 5 && object[n].getType() != 6 && object[n].getType() != 7 && object[n].getType() != 8 &&
				object[n].getHasRings())
			{
				object_count_++;

				// creamos los anillos
				object[n].setRingObject(object_count_);
				object[object_count_] = object[n];
				object[object_count_].setName(object[n].getName() + "_rings");
				object[object_count_].setOriginalName(object[n].getName() + "_rings");
				object[object_count_].setType(8); // set type as planetary rings
				object[object_count_].setOrbiting(n);
				object[object_count_].set_orbiting_name(object[n].getName());
				object[object_count_].setScale(object[n].getScale());
				object[object_count_].setEquatorialRadius(object[n].getEquatorialRadius());
				object[object_count_].setPolarRadius(object[n].getPolarRadius());
				object[object_count_].setFlattening(float(object[n].getFlattening()));
				object[object_count_].setTexture1(-1);
				object[object_count_].setTexture2(-1);
				object[object_count_].setTexture3(-1);

				//check if model is already loaded
				string_ = object[n].getName() + "_rings";
				bool found_ = false;
				for (UINT j = 0; j < model_filename.size(); j++)
				{
					if (string_ == model_filename[j])
					{
						object[object_count_].setModel(j + default_models);
						found_ = true;
					}
				}

				if (!found_)
				{
					model_filename.push_back(string_);
					models_number++;
					object[object_count_].setModel(int(model_filename.size()) + default_models - 1);
				}
			}
		}

		// create atmospheres (must be created last so we can see all the other objects through them) and set the focus object
		for (int n = 0; n < total_objects; n++) // go through all the existing objects
		{
			if (object[n].getHasClouds()) has_clouds_ = true;
			else has_clouds_ = false;

			// leave out atmospheres, clouds layers and rings
			if (object[n].getType() != 5 && object[n].getType() != 6 && object[n].getType() != 7 && object[n].getType() != 8 &&
				(has_clouds_ || object[n].getHasAtmosphere()))
			{
				object_count_++;

				// creamos la primera esfera atmosferica que sera vista del reves
				object[object_count_] = object[n];
				object[object_count_].setName(object[object_count_].getName() + "_atmosphere2");
				object[object_count_].setOriginalName(object[object_count_].getName() + "_atmosphere2");
				object[object_count_].setType(5); // set type as atmosphere
				object[object_count_].setOrbiting(n);
				object[object_count_].set_orbiting_name(object[n].getName());
				object[object_count_].setScale(object[n].getScale()*object[n].getAtmosphereRatio());
				object[object_count_].setEquatorialRadius(object[n].getEquatorialRadius()*object[n].getAtmosphereRatio());
				object[object_count_].setPolarRadius(object[n].getPolarRadius()*object[n].getAtmosphereRatio());
				object[object_count_].setFlattening(float(object[n].getFlattening()));

				if (has_clouds_)
				{
					// creamos la esfera de nubes luego de la esfera atmosferica visualizada del reves para poder ver dicha atmosfera
					// a traves de esta capa de nubes
					object_count_++;

					object[object_count_] = object[n];
					object[object_count_].setName(object[object_count_].getName() + "_clouds");
					object[object_count_].setOriginalName(object[object_count_].getName() + "_clouds");
					object[object_count_].setHasSpecularMap(0);
					object[object_count_].setType(7); // set type as clouds layer
					object[object_count_].setMass(10);
					object[object_count_].setEmptyMass(object[object_count_].getMass());
					object[object_count_].setHasAtmosphere(true); // seteamos true para que los colores del atardecer afecten tambien a las nubes
					object[object_count_].setAngulosY(object[n].getAngulosY_()*0.9); // ralentizamos la rotacion de las nubes
					object[object_count_].setOrbiting(n);
					object[object_count_].set_orbiting_name(object[n].getName());
					object[object_count_].setScale(object[n].getScale()*object[n].getCloudsRatio());
					object[object_count_].setTexture1(clouds_texture_);
				}


				// creamos la segunda esfera atmosferica que sera vista de frente, la creamos al ultimo asi vemos las nubes y la primera esfera
				// atmosferica visualizada del reves
				if (!simplified_atmosphere)
				{
					object_count_++;

					object[object_count_] = object[n];
					object[object_count_].setName(object[object_count_].getName() + "_atmosphere1");
					object[object_count_].setOriginalName(object[object_count_].getName() + "_atmosphere1");
					object[object_count_].setType(6); // set type as atmosphere
					object[object_count_].setOrbiting(n);
					object[object_count_].set_orbiting_name(object[n].getName());
					object[object_count_].setScale(object[n].getScale()*object[n].getAtmosphereRatio());
					object[object_count_].setEquatorialRadius(object[n].getEquatorialRadius()*object[n].getAtmosphereRatio());
					object[object_count_].setPolarRadius(object[n].getPolarRadius()*object[n].getAtmosphereRatio());
					object[object_count_].setFlattening(float(object[n].getFlattening()));
				}
			}
		}

		objects_file.close();

	}


	for (size_t n = 0; n < objeto.size(); n++)
	{
		object.push_back(objeto[n]);
		total_objects++;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//									Asignar influencia gravitacional si la masa es mayor o igual al minimo especificado								//
	bool found_control = false;
	for (int n = 0; n < total_objects; n++)
	{
		// set the camera focus object
		if (object[n].getName() == focus_)
		{
			found_control = true;
			control = n;
		}

		if (object[n].getAffects() != 0)
		{
			if (object[n].getMass() < minimum_gravity_influence_weight) object[n].setAffects(2);
		}
	}
	//																																					//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if (!found_control)
	{
		std::string mess_ = "Could not find the focus object: " + focus_;
		MessageBoxA(m_hwnd, mess_.c_str(), "Error", MB_OK);
		return false;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//										Convertimos los datos keplerianos a coordenadas cartesianas													//
	for (int n = total_initial_setup_objects + 1; n < total_objects; n++) // does not include element 0 which is the starfield and 1 the central star
	{
		object[n].ConvertKeplerianToCartesianElements(GRAVITATIONAL_CONSTANT*object[object[n].getOrbiting()].getMass());

		if (object[n].getOrbiting() != -1)
		{
			object[n].setPosition(object[n].getPosition() + object[object[n].getOrbiting()].getPosition());
			object[n].setVelocity(object[n].getVelocity() + object[object[n].getOrbiting()].getVelocity());
		}
	}
	//																																					//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//											Update the initial rotations according to the date														//
	double calculations_epoch_c = 2456971.5; // in Julian days
	true_julian = 2440587.0 + (time(NULL) / 86400.0);
	calculations_epoch = int(true_julian) + 0.5;

	//update rotations
	calculations_epoch_c = calculations_epoch - calculations_epoch_c;

	for (int n = 0; n < calculations_epoch_c; n++)
	{
		for (int rot = total_initial_setup_objects; rot<total_objects; rot++)
		{
			if (object[rot].getInitialRotation() != 0.0)
			{
				object[rot].setInitialRotation(object[rot].getInitialRotation() + (2.0 * phi_d) *(24.0 / object[rot].getSiderealRotationPeriod()));
				if (object[rot].getInitialRotation()>2.0 * phi_d)
				{
					object[rot].setInitialRotation(object[rot].getInitialRotation() - 2.0 * phi_d);
				}
			}
		}
	}
	//																																					//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//													Apply the specified tilt to all bodies															//
	quat temp_quaternion = { 0.0, 0.0, 0.0, 1.0 };
	double angle = 0.0;

	for (int n = 0; n < total_objects; n++)
	{
		object[n].setInheritedRotation({ 0.0, 0.0, 0.0, 1.0 });
		object[n].setTotalRotation({ 0.0, 0.0, 0.0, 1.0 });

		if (object[n].getObliquityToOrbit()>0.0)
		{
			angle = object[n].getObliquityToOrbit();
			temp_quaternion.x = 1.0 * sin(angle / 2.0);
			temp_quaternion.y = 0.0 * sin(angle / 2.0);
			temp_quaternion.z = 0.0 * sin(angle / 2.0);
			temp_quaternion.w = cos(angle / 2.0);
			object[n].setTotalRotation(temp_quaternion* object[n].getTotalRotation());
		}

		if (object[n].getInitialRotation() > 0.0)
		{
			angle = object[n].getInitialRotation();
			temp_quaternion.x = 0.0 * sin(angle / 2.0);
			temp_quaternion.y = 1.0 * sin(angle / 2.0);
			temp_quaternion.z = 0.0 * sin(angle / 2.0);
			temp_quaternion.w = cos(angle / 2.0);
			object[n].setTotalRotation(temp_quaternion* object[n].getTotalRotation());
		}
	}
	//																																					//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	universe_scale = 1.0f / float(object[control].getScale());
	for (int n = total_initial_setup_objects; n < total_objects; n++)
	{
		object[n].setPosition(scientific_to_d3d_coords(object[n].getPosition()));
		object[n].setVelocity(scientific_to_d3d_coords(object[n].getVelocity()));
	}

	// ubicamos a todos los objetos que no estan en orbita sobre la superficie del objeto orbitado
	for (int n = total_initial_setup_objects; n < total_objects; n++)
	{
		if (!object[n].getInOrbit())
		{
			int orbited_ = object[n].getOrbiting();

			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//												Geodetic to cartesian conversion														//
			//int orbited_, double lati, double longi, double alti
			double longi = ToRadians_d(object[n].getLongitude()* -1); // invertimos la longitud no se por que, pero asi salen bien los calculos
			double lati = ToRadians_d(object[n].getLatitude());
			double alti = object[n].get_altitude();
			double xx;
			double yy;
			double zz;
			double semi_major_axis = 0;

			if (object[orbited_].getFlattening() > 0.0)
			{
				if (object[orbited_].getEquatorialRadius() < object[orbited_].getPolarRadius())
					semi_major_axis = object[orbited_].getPolarRadius() * 2;

				else semi_major_axis = object[orbited_].getEquatorialRadius() * 2;
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

			object[n].setPosition(vector_rotation_by_quaternion({ xx, zz, -yy }, ~object[orbited_].getTotalRotation())
				+ object[orbited_].getPosition());
			//																																		//
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			// dock the object
			dock_object(object, n, orbited_);

			// set landed flag
			object[n].setLanded(true);
		}
	}

	// Establecemos el flag de los objetos que estan dentro de una atmosfera
	for (int n = total_initial_setup_objects; n < total_objects; n++)
	{
		if (object[n].getType() == 4 && object[object[n].getOrbiting()].getHasAtmosphere())
		{
			if (length(object[n].getPosition() - object[object[n].getOrbiting()].getPosition()) <= (object[object[n].getOrbiting()].getScale() / 2)*object[object[n].getOrbiting()].getAtmosphereRatio())
				object[n].setInAtmosphere(true);
		}

		else object[n].setInAtmosphere(false);
	}

	// check for duplicated names
	for (int n = total_initial_setup_objects; n < total_objects; n++)
	{
		for (int nn = total_initial_setup_objects; nn < total_objects; nn++)
		{
			if (n!=nn && object[n].getName() == object[nn].getName())
			{
				std::string mess_ = "Dude, I've found more than one object with the same name: " + object[n].getName() + "\n\n" +
					"All sort of crazy shit will happen. \n" +
					"Why don't you rename the duplicate to something like " + object[n].getName() + "_b\n" +
					"or some other shit,,, I'd better shut the program 'til you fix it";
				MessageBoxA(m_hwnd, mess_.c_str(), "Error", MB_OK);
				return false;
			}
		}

	}

	return true;
}

bool ObjectCreatorClass::create_natural_object(std::ifstream& objects_file, std::vector <std::string>* nombre_objeto)
{
	char input_ = ' ';
	std::string string_ = "";
	double double_ = 0.0;
	float float_ = 0.f;
	int int_ = 0;

	objects_file.get(input_);
	while (input_ != ':') objects_file.get(input_);

	objects_file >> double_;
	object[object_count_].setMass(double_);
	object[object_count_].setEmptyMass(object[object_count_].getMass());
	////////////////////////////////////////////////////////////////////////////////

	objects_file.get(input_);
	while (input_ != ':') objects_file.get(input_);

	objects_file >> double_;
	object[object_count_].setSemiMayorAxis(AU2meters(double_));
	////////////////////////////////////////////////////////////////////////////////

	objects_file.get(input_);
	while (input_ != ':') objects_file.get(input_);

	objects_file >> double_;
	object[object_count_].setEccentricity(double_);
	////////////////////////////////////////////////////////////////////////////////

	objects_file.get(input_);
	while (input_ != ':') objects_file.get(input_);

	objects_file >> double_;
	object[object_count_].setInclination(ToRadians_d(double_));
	////////////////////////////////////////////////////////////////////////////////

	objects_file.get(input_);
	while (input_ != ':') objects_file.get(input_);

	objects_file >> double_;
	object[object_count_].setArgumentOfPerigee(ToRadians_d(double_));
	////////////////////////////////////////////////////////////////////////////////

	objects_file.get(input_);
	while (input_ != ':') objects_file.get(input_);

	objects_file >> double_;
	object[object_count_].setLongitudeOfAscendingNode(ToRadians_d(double_));
	////////////////////////////////////////////////////////////////////////////////

	objects_file.get(input_);
	while (input_ != ':') objects_file.get(input_);

	objects_file >> double_;
	object[object_count_].setTrueAnomaly(ToRadians_d(double_));
	////////////////////////////////////////////////////////////////////////////////

	objects_file.get(input_);
	while (input_ != ':') objects_file.get(input_);

	objects_file >> double_;
	object[object_count_].setObliquityToOrbit(ToRadians_d(double_) + object[object_count_].getInclination());
	////////////////////////////////////////////////////////////////////////////////

	objects_file.get(input_);
	while (input_ != ':') objects_file.get(input_);

	objects_file >> double_;
	object[object_count_].setInitialRotation(ToRadians_d(double_));
	////////////////////////////////////////////////////////////////////////////////

	objects_file.get(input_);
	while (input_ != ':') objects_file.get(input_);

	objects_file >> double_;
	object[object_count_].setSiderealRotationPeriod(double_);
	if (object[object_count_].getSiderealRotationPeriod() != 0) object[object_count_].setAngulosY(
		ToRadians_d(360.0 / (object[object_count_].getSiderealRotationPeriod() * 3600.0)));
	else object[object_count_].setAngulosY(0.0);
	////////////////////////////////////////////////////////////////////////////////

	// establecemos a quien orbita
	objects_file.get(input_);
	while (input_ != ':') objects_file.get(input_);

	objects_file.get(input_);
	getline(objects_file, string_);

	if (string_ == "Galaxy center") object[object_count_].setOrbiting(-1);
	else
	{
		bool found_b = false;
		for (UINT n = 0; n < nombre_objeto->size(); n++)
		{
			if (string_ == (*nombre_objeto)[n])
			{
				object[object_count_].setOrbiting(n + total_initial_setup_objects);
				object[object_count_].set_orbiting_name(string_);
				found_b = true;
			}
		}
		if (!found_b)
		{
			MessageBoxA(m_hwnd, "Could not find orbited object", object[object_count_].getName().c_str(), MB_OK);
			return false;
		}
	}
	//MessageBoxA(m_hwnd, "Hasta aqui todo bien", object[object_count_].getName().c_str(), MB_OK);
	////////////////////////////////////////////////////////////////////////////////

	objects_file.get(input_);
	while (input_ != ':')
	{
		objects_file.get(input_);
	}

	objects_file >> double_;
	object[object_count_].setEquatorialRadius(double_);
	////////////////////////////////////////////////////////////////////////////////

	objects_file.get(input_);
	while (input_ != ':')
	{
		objects_file.get(input_);
	}

	objects_file >> double_;
	object[object_count_].setPolarRadius(double_);

	object[object_count_].setFlattening(float(1.0 - (object[object_count_].getPolarRadius() / object[object_count_].getEquatorialRadius())));
	object[object_count_].setScale(object[object_count_].getEquatorialRadius() * 2); // double the radius to get the full scale
	////////////////////////////////////////////////////////////////////////////////

	// *** DEBUG *** hacer que las lunas tambien puedan tener atmosferas
	// si es un planeta
	if (object[object_count_].getType() == 2)
	{
		objects_file.get(input_);
		while (input_ != ':')
		{
			objects_file.get(input_);
		}

		objects_file >> string_;
		if (string_ == "true") object[object_count_].setHasAtmosphere(true);
		else object[object_count_].setHasAtmosphere(false);
		////////////////////////////////////////////////////////////////////////////

		if (object[object_count_].getHasAtmosphere())
		{
			objects_file.get(input_);
			while (input_ != ':')
			{
				objects_file.get(input_);
			}

			objects_file >> float_;
			object[object_count_].setAtmosphereRatio(float_);
			////////////////////////////////////////////////////////////////////////

			objects_file.get(input_);
			while (input_ != ':')
			{
				objects_file.get(input_);
			}

			objects_file >> float_;
			object[object_count_].setSunsetColor({ float_, object[object_count_].getSunsetColor().y, object[object_count_].getSunsetColor().z });
			objects_file >> float_;
			object[object_count_].setSunsetColor({ object[object_count_].getSunsetColor().x, float_, object[object_count_].getSunsetColor().z });
			objects_file >> float_;
			object[object_count_].setSunsetColor({ object[object_count_].getSunsetColor().x, object[object_count_].getSunsetColor().y, float_ });
			////////////////////////////////////////////////////////////////////////

			objects_file.get(input_);
			while (input_ != ':')
			{
				objects_file.get(input_);
			}

			objects_file >> float_;
			object[object_count_].setDayLightColor({ float_, object[object_count_].getDayLightColor().y, object[object_count_].getDayLightColor().z });
			objects_file >> float_;
			object[object_count_].setDayLightColor({ object[object_count_].getDayLightColor().x, float_, object[object_count_].getDayLightColor().z });
			objects_file >> float_;
			object[object_count_].setDayLightColor({ object[object_count_].getDayLightColor().x, object[object_count_].getDayLightColor().y, float_ });
			////////////////////////////////////////////////////////////////////////

			objects_file.get(input_);
			while (input_ != ':')
			{
				objects_file.get(input_);
			}

			objects_file >> float_;
			object[object_count_].setAtmosphereThickness(float_);
			////////////////////////////////////////////////////////////////////////

			objects_file.get(input_);
			while (input_ != ':')
			{
				objects_file.get(input_);
			}

			objects_file >> float_;
			object[object_count_].setSeaLevelPressure(float_);
			////////////////////////////////////////////////////////////////////////

			objects_file.get(input_);
			while (input_ != ':')
			{
				objects_file.get(input_);
			}

			objects_file >> float_;
			object[object_count_].setSeaLevelTemperature(float_);
			////////////////////////////////////////////////////////////////////////

			objects_file.get(input_);
			while (input_ != ':')
			{
				objects_file.get(input_);
			}

			objects_file >> float_;
			object[object_count_].setSeaLevelDensity(float_);
			////////////////////////////////////////////////////////////////////////

			objects_file.get(input_);
			while (input_ != ':')
			{
				objects_file.get(input_);
			}

			objects_file >> string_;
			if (string_ == "true") object[object_count_].setHasClouds(true);
			else object[object_count_].setHasClouds(false);
			////////////////////////////////////////////////////////////////////////////

			if (object[object_count_].getHasClouds())
			{
				objects_file.get(input_);
				while (input_ != ':')
				{
					objects_file.get(input_);
				}

				objects_file >> float_;
				object[object_count_].setCloudsRatio(float_);
				////////////////////////////////////////////////////////////////////////

				objects_file.get(input_);
				while (input_ != ':')
				{
					objects_file.get(input_);
				}

				objects_file >> string_;
				if (string_ != "none")
				{
					//check if texture is already loaded
					bool found_ = false;
					for (UINT j = 0; j < texture_filename.size(); j++)
					{
						if (string_ == texture_filename[j])
						{
							clouds_texture_ = j + default_textures;
							found_ = true;
						}
					}

					if (!found_)
					{
						texture_filename.push_back(string_);
						textures_number++;
						clouds_texture_ = int(texture_filename.size()) + default_textures - 1;
					}
				}

				else
				{
					MessageBox(m_hwnd, L"Clouds texture not specified", L"Error", MB_OK);
					return false;
				}
				////////////////////////////////////////////////////////////////////////////////
			}
		}

		objects_file.get(input_);
		while (input_ != ':')
		{
			objects_file.get(input_);
		}

		objects_file >> string_;
		if (string_ == "true") object[object_count_].setHasRings(true);
		else object[object_count_].setHasRings(false);
		////////////////////////////////////////////////////////////////////////////

		if (object[object_count_].getHasRings())
		{
			objects_file.get(input_);
			while (input_ != ':')
			{
				objects_file.get(input_);
			}

			objects_file >> float_;
			object[object_count_].setInnerRing(float_);
			////////////////////////////////////////////////////////////////////////

			objects_file.get(input_);
			while (input_ != ':')
			{
				objects_file.get(input_);
			}

			objects_file >> float_;
			object[object_count_].setOuterRing(float_);
			////////////////////////////////////////////////////////////////////////

			objects_file.get(input_);
			while (input_ != ':')
			{
				objects_file.get(input_);
			}

			objects_file >> string_;
			object[object_count_].setRingsTexture(string_);
			////////////////////////////////////////////////////////////////////////
		}
	}
	////////////////////////////////////////////////////////////////////////////////

	// cargamos el modelo
	objects_file.get(input_);
	while (input_ != ':')
	{
		objects_file.get(input_);
	}

	objects_file >> string_;
	if (string_ != "sphere")
	{
		//check if model is already loaded
		bool found_ = false;
		for (UINT j = 0; j < model_filename.size(); j++)
		{
			if (string_ == model_filename[j])
			{
				object[object_count_].setModel(j + default_models);
				found_ = true;
			}
		}

		if (!found_)
		{
			model_filename.push_back(string_);
			models_number++;
			object[object_count_].setModel(int(model_filename.size()) + default_models - 1);
		}
	}
	else object[object_count_].setModel(5);
	////////////////////////////////////////////////////////////////////////////////

	// cargamos la textura
	objects_file.get(input_);
	while (input_ != ':')
	{
		objects_file.get(input_);
	}

	objects_file >> string_;
	if (string_ != "none")
	{
		//check if the texture is already loaded
		bool found_ = false;
		for (UINT j = 0; j < texture_filename.size(); j++)
		{
			if (string_ == texture_filename[j])
			{
				object[object_count_].setTexture1(j + default_textures);
				found_ = true;
			}
		}

		if (!found_)
		{
			texture_filename.push_back(string_);
			textures_number++;
			object[object_count_].setTexture1(int(texture_filename.size()) + default_textures - 1);
		}
	}

	else object[object_count_].setTexture1(-1);
	////////////////////////////////////////////////////////////////////////////////

	// cargamos la textura nocturna
	objects_file.get(input_);
	while (input_ != ':')
	{
		objects_file.get(input_);
	}

	objects_file >> string_;
	if (string_ != "none")
	{
		object[object_count_].setHasNightMap(true);
		//check if texture is already loaded
		bool found_ = false;
		for (UINT j = 0; j < texture_filename.size(); j++)
		{
			if (string_ == texture_filename[j])
			{
				object[object_count_].setTexture2(j + default_textures);
				found_ = true;
			}
		}

		if (!found_)
		{
			texture_filename.push_back(string_);
			textures_number++;
			object[object_count_].setTexture2(int(texture_filename.size()) + default_textures - 1);
		}
	}

	else object[object_count_].setTexture2(-1);
	////////////////////////////////////////////////////////////////////////////////

	//cargamos el mapa especular
	objects_file.get(input_);
	while (input_ != ':')
	{
		objects_file.get(input_);
	}

	objects_file >> string_;
	if (string_ != "none")
	{
		object[object_count_].setHasSpecularMap(true);
		//check if texture is already loaded
		bool found_ = false;
		for (UINT j = 0; j < texture_filename.size(); j++)
		{
			if (string_ == texture_filename[j])
			{
				object[object_count_].setTexture3(j + default_textures);
				found_ = true;
			}
		}

		if (!found_)
		{
			texture_filename.push_back(string_);
			textures_number++;
			object[object_count_].setTexture3(int(texture_filename.size()) + default_textures - 1);
		}
	}

	else object[object_count_].setTexture3(-1);
	////////////////////////////////////////////////////////////////////////////////

	// cargamos los datos de las efemerides
	objects_file.get(input_);
	while (input_ != ':')
	{
		objects_file.get(input_);
	}

	objects_file >> string_;
	bool ephemeris_ok_ = false;
	double eccentricity_, inclination_, lan_, ap_, ta_, sma_;
	if (string_ != "none")
	{
		if (!ephemeris_object->UpdateEphemeris(string_, eccentricity_, inclination_, lan_, ap_, ta_, sma_, ephemeris_ok_))
		{
			MessageBoxA(m_hwnd, "Could not open ephemeris file.", object[object_count_].getName().c_str(), MB_OK);
			return false;
		}
	}

	if (ephemeris_ok_)
	{
		object[object_count_].setEccentricity(eccentricity_);
		object[object_count_].setInclination(inclination_);
		object[object_count_].setLongitudeOfAscendingNode(lan_);
		object[object_count_].setArgumentOfPerigee(ap_);
		object[object_count_].setTrueAnomaly(ta_);
		object[object_count_].setSemiMayorAxis(sma_);
		object[object_count_].setEphemeris_OK(ephemeris_ok_);
	}
	////////////////////////////////////////////////////////////////////////////////

	return true;
}

bool ObjectCreatorClass::create_spaceship(std::ifstream& objects_file, std::vector <std::string>* nombre_objeto)
{
	char input_ = ' ';
	std::string string_ = "";
	double double_ = 0.0;
	float float_ = 0.f;
	int int_ = 0;

	// anulamos las texturas predefinidas
	object[object_count_].setTexture1(-1);
	object[object_count_].setTexture2(-1);
	object[object_count_].setTexture3(-1);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// leer el archivo de especificaciones tecnicas
	objects_file.get(input_);
	while (input_ != ':')
	{
		objects_file.get(input_);
	}

	objects_file >> string_;

	std::ifstream specs_file;
	specs_file.open(string_);

	if (!read_specs_file(object, string_, object_count_))
	{
		MessageBoxA(m_hwnd, "Could not read specs file", object[object_count_].getName().c_str(), MB_OK);
		return false;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//4: spacecraft
	//11: Landing surface
	if (object[object_count_].getType() == 4 || object[object_count_].getType() == 11)
	{
		// establecer si esta en orbita o no
		objects_file.get(input_);
		while (input_ != ':') objects_file.get(input_);

		objects_file >> string_;
		if (string_ == "true")
		{
			object[object_count_].setInOrbit(true);
			object[object_count_].setCastsShadows(true);
		}
		else object[object_count_].setInOrbit(false);
	}

	else
	{
		//10: Ground object
		if (object[object_count_].getType() == 10) object[object_count_].setInOrbit(false);
	}

	// si no esta en orbita
	if (!object[object_count_].getInOrbit())
	{
		objects_file.get(input_);
		while (input_ != ':') objects_file.get(input_);

		objects_file >> double_;
		object[object_count_].setLatitude(double_);
		////////////////////////////////////////////////////////////////////////////////

		objects_file.get(input_);
		while (input_ != ':') objects_file.get(input_);

		objects_file >> double_;
		object[object_count_].setLongitude(double_);
		////////////////////////////////////////////////////////////////////////////////

		objects_file.get(input_);
		while (input_ != ':') objects_file.get(input_);

		objects_file >> double_;
		object[object_count_].set_altitude(double_);
		////////////////////////////////////////////////////////////////////////////////

		objects_file.get(input_);
		while (input_ != ':') objects_file.get(input_);

		objects_file >> double_;
		double_ += 0.0001; // para eliminar el error del tembleque en el display de los grados cuando tenemos precision absoluta
		object[object_count_].setHeading(ToRadians_d(double_));
		////////////////////////////////////////////////////////////////////////////////
		// set the X rotation
		objects_file.get(input_);
		while (input_ != ':')
		{
			objects_file.get(input_);
		}

		objects_file.get(input_);
		objects_file >> float_;

		object[object_count_].setDocked_rotation_x(float_ + 0.0001f);
		////////////////////////////////////////////////////////////////////////
		// set the Y rotation
		objects_file.get(input_);
		while (input_ != ':')
		{
			objects_file.get(input_);
		}

		objects_file.get(input_);
		objects_file >> float_;

		object[object_count_].setDocked_rotation_y(float_ + 0.0001f);
		////////////////////////////////////////////////////////////////////////
		// set the Z rotation
		objects_file.get(input_);
		while (input_ != ':')
		{
			objects_file.get(input_);
		}

		objects_file.get(input_);
		objects_file >> float_;

		object[object_count_].setDocked_rotation_z(float_ + 0.0001f);
		////////////////////////////////////////////////////////////////////////
	}

	// si esta en orbita
	else
	{
		objects_file.get(input_);
		while (input_ != ':') objects_file.get(input_);

		objects_file >> double_;
		object[object_count_].setSemiMayorAxis(AU2meters(double_));
		////////////////////////////////////////////////////////////////////////////////

		objects_file.get(input_);
		while (input_ != ':') objects_file.get(input_);

		objects_file >> double_;
		object[object_count_].setEccentricity(double_);
		////////////////////////////////////////////////////////////////////////////////

		objects_file.get(input_);
		while (input_ != ':') objects_file.get(input_);

		objects_file >> double_;
		object[object_count_].setInclination(ToRadians_d(double_));
		////////////////////////////////////////////////////////////////////////////////

		objects_file.get(input_);
		while (input_ != ':') objects_file.get(input_);

		objects_file >> double_;
		object[object_count_].setArgumentOfPerigee(ToRadians_d(double_));
		////////////////////////////////////////////////////////////////////////////////

		objects_file.get(input_);
		while (input_ != ':') objects_file.get(input_);

		objects_file >> double_;
		object[object_count_].setLongitudeOfAscendingNode(ToRadians_d(double_));
		////////////////////////////////////////////////////////////////////////////////

		objects_file.get(input_);
		while (input_ != ':') objects_file.get(input_);

		objects_file >> double_;
		object[object_count_].setTrueAnomaly(ToRadians_d(double_));
		////////////////////////////////////////////////////////////////////////////////

		objects_file.get(input_);
		while (input_ != ':') objects_file.get(input_);

		objects_file >> double_;
		object[object_count_].setObliquityToOrbit(ToRadians_d(double_) + object[object_count_].getInclination());
		////////////////////////////////////////////////////////////////////////////////

		objects_file.get(input_);
		while (input_ != ':') objects_file.get(input_);

		objects_file >> double_;
		object[object_count_].setInitialRotation(ToRadians_d(double_));
		////////////////////////////////////////////////////////////////////////////////

		objects_file.get(input_);
		while (input_ != ':') objects_file.get(input_);

		objects_file >> double_;
		object[object_count_].setSiderealRotationPeriod(double_);
		if (object[object_count_].getSiderealRotationPeriod() != 0) object[object_count_].setAngulosY(
			ToRadians_d(360.0 / (object[object_count_].getSiderealRotationPeriod() * 3600.0)));
		else object[object_count_].setAngulosY(0.0);
	}
	////////////////////////////////////////////////////////////////////////////////

	// establecemos a quien esta orbitando
	objects_file.get(input_);
	while (input_ != ':') objects_file.get(input_);

	objects_file.get(input_);
	getline(objects_file, string_);

	if (string_ == "Galaxy center") object[object_count_].setOrbiting(-1);
	else
	{
		bool found_b = false;
		for (UINT n = 0; n < nombre_objeto->size(); n++)
		{
			if (string_ == (*nombre_objeto)[n])
			{
				object[object_count_].setOrbiting(n + total_initial_setup_objects);
				object[object_count_].set_orbiting_name(string_);
				found_b = true;
			}
		}
		if (!found_b)
		{
			MessageBoxA(m_hwnd, "Could not find orbited object", object[object_count_].getName().c_str(), MB_OK);
			return false;
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// si esta en orbita ver si tiene archivo de efemerides
	if (object[object_count_].getInOrbit())
	{
		// cargamos los datos de las efemerides
		objects_file.get(input_);
		while (input_ != ':')
		{
			objects_file.get(input_);
		}

		objects_file >> string_;
		bool ephemeris_ok_ = false;
		double eccentricity_, inclination_, lan_, ap_, ta_, sma_;
		if (string_ != "none")
		{
			if (!ephemeris_object->UpdateEphemeris(string_, eccentricity_, inclination_, lan_, ap_, ta_, sma_, ephemeris_ok_))
			{
				MessageBoxA(m_hwnd, "Could not open ephemeris file.", object[object_count_].getName().c_str(), MB_OK);
				return false;
			}
		}

		if (ephemeris_ok_)
		{
			object[object_count_].setEccentricity(eccentricity_);
			object[object_count_].setInclination(inclination_);
			object[object_count_].setLongitudeOfAscendingNode(lan_);
			object[object_count_].setArgumentOfPerigee(ap_);
			object[object_count_].setTrueAnomaly(ta_);
			object[object_count_].setSemiMayorAxis(sma_);
			object[object_count_].setEphemeris_OK(ephemeris_ok_);
		}
	}
	////////////////////////////////////////////////////////////////////////////////
	// leemos los datos adicionales, if any
	objects_file.get(input_);
	while (input_ != ':')
	{
		objects_file.get(input_);
	}

	objects_file >> string_;

	if (string_ != "none")
	{
		if (!read_special_properties(object, object_count_, object[object_count_].getName(), string_))
		{
			MessageBoxA(m_hwnd, "Could not read special properties", object[object_count_].getName().c_str(), MB_OK);
			return false;
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	return true;
}

bool ObjectCreatorClass::read_specs_file(std::vector <objectClass>& objeto, std::string specs_file_, int objeto_number_)
{
	char input_ = ' ';
	std::string string_ = "";
	double double_ = 0.0;
	float float_ = 0.f;
	int int_ = 0;

	std::ifstream specs_file;
	specs_file.open(specs_file_);

	// If it could not open the file then exit.
	if (specs_file.fail())
	{
		MessageBoxA(m_hwnd, "Could not open the specs file", specs_file_.c_str(), MB_OK);
		return false;
	}

	if (specs_file.is_open())
	{
		// load model
		specs_file.get(input_);
		while (input_ != ':')
		{
			specs_file.get(input_);
		}

		specs_file.get(input_);
		specs_file >> string_;
		if (string_ != "sphere")
		{
			//check if model is already loaded
			bool found_ = false;
			for (UINT j = 0; j < model_filename.size(); j++)
			{
				if (string_ == model_filename[j])
				{
					objeto[objeto_number_].setModel(j + default_models);
					found_ = true;
				}
			}

			if (!found_)
			{
				model_filename.push_back(string_);
				models_number++;
				objeto[objeto_number_].setModel(int(model_filename.size()) + default_models - 1);
			}
		}
		else objeto[objeto_number_].setModel(5);
		////////////////////////////////////////////////////////////////////////////////
		// establecemos el tipo de collision shape
		specs_file.get(input_);
		while (input_ != ':') specs_file.get(input_);

		specs_file.get(input_);
		getline(specs_file, string_);

		if (string_ == "primitive") objeto[objeto_number_].setCollisionShapeType(0);
		else if (string_ == "compound") objeto[objeto_number_].setCollisionShapeType(1);
		else if (string_ == "convex hull") objeto[objeto_number_].setCollisionShapeType(2);		
		////////////////////////////////////////////////////////////////////////////////
		// establecemos el archivo del collision shape
		if (string_ != "none")
		{
			specs_file.get(input_);
			while (input_ != ':') specs_file.get(input_);

			specs_file.get(input_);
			getline(specs_file, string_);

			objeto[objeto_number_].setCollisionShapeFile(string_);
		}	
		////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////
		// establecemos el vertical de la camara
		specs_file.get(input_);
		while (input_ != ':') specs_file.get(input_);

		Vector3 cam_axis;
		specs_file >> cam_axis.x; specs_file >> cam_axis.y; specs_file >> cam_axis.z;

		objeto[objeto_number_].setCameraPreferedAxis(cam_axis);
		////////////////////////////////////////////////////////////////////////////////
		// establecer la masa
		specs_file.get(input_);
		while (input_ != ':') specs_file.get(input_);

		specs_file >> double_;
		objeto[objeto_number_].setMass(double_);
		objeto[objeto_number_].setEmptyMass(objeto[objeto_number_].getMass());

		////////////////////////////////////////////////////////////////////////////////

		specs_file.get(input_);
		while (input_ != ':')
		{
			specs_file.get(input_);
		}

		specs_file.get(input_);
		specs_file >> string_;

		if (string_ == "true")objeto[objeto_number_].setRocketEngine(true);
		else objeto[objeto_number_].setRocketEngine(false);
		////////////////////////////////////////////////////////////////////////////////

		if (objeto[objeto_number_].getRocketEngine())
		{
			specs_file.get(input_);
			while (input_ != ':')
			{
				specs_file.get(input_);
			}

			Color col_ = { 0.f, 0.f, 0.f, 1.f };
			specs_file >> col_.x; specs_file >> col_.y; specs_file >> col_.z;
			objeto[objeto_number_].setEngineColorStart(col_);
			////////////////////////////////////////////////////////////////////////////////

			specs_file.get(input_);
			while (input_ != ':')
			{
				specs_file.get(input_);
			}

			col_ = Color( 0.f, 0.f, 0.f, 1.f );
			specs_file >> col_.x; specs_file >> col_.y; specs_file >> col_.z;
			objeto[objeto_number_].setEngineColorEnd(col_);
		}
		////////////////////////////////////////////////////////////////////////////////

		specs_file.get(input_);
		while (input_ != ':')
		{
			specs_file.get(input_);
		}

		specs_file >> double_;
		objeto[objeto_number_].setMainEnginePower(double_ * 1e3); // convert Newtons to kN
		////////////////////////////////////////////////////////////////////////////////
		// fuel consumption
		specs_file.get(input_);
		while (input_ != ':')
		{
			specs_file.get(input_);
		}

		specs_file >> double_;
		objeto[objeto_number_].setFuelConsumption(double_);
		////////////////////////////////////////////////////////////////////////////////

		specs_file.get(input_);
		while (input_ != ':')
		{
			specs_file.get(input_);
		}

		specs_file >> double_;
		objeto[objeto_number_].setRCSpower(double_ * 1e3); // convert Newtons to kN
		////////////////////////////////////////////////////////////////////////////////

		specs_file.get(input_);
		while (input_ != ':')
		{
			specs_file.get(input_);
		}

		specs_file >> double_;
		objeto[objeto_number_].setHoverPower(double_ * 1e3); // convert Newtons to kN
		////////////////////////////////////////////////////////////////////////////////

		specs_file.get(input_);
		while (input_ != ':')
		{
			specs_file.get(input_);
		}

		specs_file >> int_;
		objeto[objeto_number_].setTotalDockingPorts(int_);
		if (objeto[objeto_number_].getTotalDockingPorts() > 0) objeto[objeto_number_].InitializeDockingPorts(int_);
		////////////////////////////////////////////////////////////////////////////////

		for (int dock_n = 0; dock_n < int_; dock_n++)
		{
			specs_file.get(input_);
			while (input_ != ':')
			{
				specs_file.get(input_);
			}

			Vector3 pos_;
			specs_file >> pos_.x; specs_file >> pos_.y; specs_file >> pos_.z;
			objeto[objeto_number_].setDockingPortPosition(dock_n, pos_);
		}
		////////////////////////////////////////////////////////////////////////////////

		for (int dock_n = 0; dock_n < int_; dock_n++)
		{
			specs_file.get(input_);
			while (input_ != ':')
			{
				specs_file.get(input_);
			}

			Vector3 ori_;
			specs_file >> ori_.x; specs_file >> ori_.y; specs_file >> ori_.z;
			objeto[objeto_number_].setDockingPortOrientation(dock_n, ori_);
		}
		////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////

		specs_file.get(input_);
		while (input_ != ':')
		{
			specs_file.get(input_);
		}

		specs_file >> int_;
		objeto[objeto_number_].setTotalEngines(int_);
		if (objeto[objeto_number_].getTotalEngines() > 0) objeto[objeto_number_].InitializeEngines(int_);
		////////////////////////////////////////////////////////////////////////////////

		for (int engine_n = 0; engine_n < int_; engine_n++)
		{
			specs_file.get(input_);
			while (input_ != ':')
			{
				specs_file.get(input_);
			}

			Vector3 pos_;
			specs_file >> pos_.x; specs_file >> pos_.y; specs_file >> pos_.z;
			objeto[objeto_number_].setEnginePosition(engine_n, pos_);
		}
		////////////////////////////////////////////////////////////////////////////////

		for (int engine_n = 0; engine_n < int_; engine_n++)
		{
			specs_file.get(input_);
			while (input_ != ':')
			{
				specs_file.get(input_);
			}

			specs_file >> float_;
			objeto[objeto_number_].setEngineSize(engine_n, float_);
		}
		////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////

		specs_file.get(input_);
		while (input_ != ':')
		{
			specs_file.get(input_);
		}

		specs_file >> int_;
		objeto[objeto_number_].setTotalHoverEngines(int_);
		if (objeto[objeto_number_].getTotalHoverEngines() > 0) objeto[objeto_number_].InitializeHoverEngines(int_);
		////////////////////////////////////////////////////////////////////////////////

		for (int engine_n = 0; engine_n < int_; engine_n++)
		{
			specs_file.get(input_);
			while (input_ != ':')
			{
				specs_file.get(input_);
			}

			Vector3 pos_;
			specs_file >> pos_.x; specs_file >> pos_.y; specs_file >> pos_.z;
			objeto[objeto_number_].setHoverEnginePosition(engine_n, pos_);
		}
		////////////////////////////////////////////////////////////////////////////////

		for (int engine_n = 0; engine_n < int_; engine_n++)
		{
			specs_file.get(input_);
			while (input_ != ':')
			{
				specs_file.get(input_);
			}

			specs_file >> float_;
			objeto[objeto_number_].setHoverEngineSize(engine_n, float_);
		}
		////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////////////
		//****************************************************************************//
		////////////////////////////////////////////////////////////////////////////////

		specs_file.get(input_);
		while (input_ != ':')
		{
			specs_file.get(input_);
		}

		specs_file >> int_;
		objeto[objeto_number_].setTotalRCSNozzles(int_);
		if (objeto[objeto_number_].getTotalRCSNozzles() > 0)
		{
			objeto[objeto_number_].InitializeRCSNozzles(int_);

			if (int_ > 0)
			{
				specs_file.get(input_);
				while (input_ != ':')
				{
					specs_file.get(input_);
				}

				specs_file.get(input_);
				specs_file >> string_;
				if (string_ == "true") objeto[objeto_number_].setRCSTraslationEnabled(true);
				else objeto[objeto_number_].setRCSTraslationEnabled(false);
			}
		}
		////////////////////////////////////////////////////////////////////////////////

		for (int nozzle_n = 0; nozzle_n < int_; nozzle_n++)
		{
			specs_file.get(input_);
			while (input_ != ':')
			{
				specs_file.get(input_);
			}

			Vector3 pos_;
			specs_file >> pos_.x; specs_file >> pos_.y; specs_file >> pos_.z;
			objeto[objeto_number_].setRCSNozzlePosition(nozzle_n, pos_);
		}
		////////////////////////////////////////////////////////////////////////////////

		for (int nozzle_n = 0; nozzle_n < int_; nozzle_n++)
		{
			specs_file.get(input_);
			while (input_ != ':')
			{
				specs_file.get(input_);
			}

			Vector3 orient_;
			specs_file >> orient_.x; specs_file >> orient_.y; specs_file >> orient_.z;
			objeto[objeto_number_].setRCSNozzleOrientation(nozzle_n, orient_);
		}
		////////////////////////////////////////////////////////////////////////////////

		for (int nozzle_n = 0; nozzle_n < int_; nozzle_n++)
		{
			specs_file.get(input_);
			while (input_ != ':')
			{
				specs_file.get(input_);
			}

			specs_file >> float_;
			objeto[objeto_number_].setRCSNozzleSize(nozzle_n, float_);
		}
		////////////////////////////////////////////////////////////////////////////////
		//****************************************************************************//
		////////////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////////////
		//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX//
		////////////////////////////////////////////////////////////////////////////////
		if (objeto[objeto_number_].getTotalRCSNozzles() > 0)
		{
			if (objeto[objeto_number_].getRCSTraslationEnabled())
			{
				// Forward Nozzles
				specs_file.get(input_);
				while (input_ != ':')
				{
					specs_file.get(input_);
				}

				specs_file >> int_;
				objeto[objeto_number_].setTotalForwardNozzles(int_);
				if (objeto[objeto_number_].getTotalForwardNozzles() > 0) objeto[objeto_number_].InitializeForwardNozzles(int_);
				////////////////////////////////////////////////////////////////////////////////

				for (int nozzle_n = 0; nozzle_n < int_; nozzle_n++)
				{
					specs_file.get(input_);
					while (input_ != ':')
					{
						specs_file.get(input_);
					}

					int n_;
					specs_file >> n_;
					objeto[objeto_number_].setForwardNozzle(nozzle_n, n_);
				}
				////////////////////////////////////////////////////////////////////////////////

				// Backward Nozzles
				specs_file.get(input_);
				while (input_ != ':')
				{
					specs_file.get(input_);
				}

				specs_file >> int_;
				objeto[objeto_number_].setTotalBackwardNozzles(int_);
				if (objeto[objeto_number_].getTotalBackwardNozzles() > 0) objeto[objeto_number_].InitializeBackwardNozzles(int_);
				////////////////////////////////////////////////////////////////////////////////

				for (int nozzle_n = 0; nozzle_n < int_; nozzle_n++)
				{
					specs_file.get(input_);
					while (input_ != ':')
					{
						specs_file.get(input_);
					}

					int n_;
					specs_file >> n_;
					objeto[objeto_number_].setBackwardNozzle(nozzle_n, n_);
				}
				////////////////////////////////////////////////////////////////////////////////

				// Up Nozzles
				specs_file.get(input_);
				while (input_ != ':')
				{
					specs_file.get(input_);
				}

				specs_file >> int_;
				objeto[objeto_number_].setTotalUpNozzles(int_);
				if (objeto[objeto_number_].getTotalUpNozzles() > 0) objeto[objeto_number_].InitializeUpNozzles(int_);
				////////////////////////////////////////////////////////////////////////////////

				for (int nozzle_n = 0; nozzle_n < int_; nozzle_n++)
				{
					specs_file.get(input_);
					while (input_ != ':')
					{
						specs_file.get(input_);
					}

					int n_;
					specs_file >> n_;
					objeto[objeto_number_].setUpNozzle(nozzle_n, n_);
				}
				////////////////////////////////////////////////////////////////////////////////

				// Down Nozzles
				specs_file.get(input_);
				while (input_ != ':')
				{
					specs_file.get(input_);
				}

				specs_file >> int_;
				objeto[objeto_number_].setTotalDownNozzles(int_);
				if (objeto[objeto_number_].getTotalDownNozzles() > 0) objeto[objeto_number_].InitializeDownNozzles(int_);
				////////////////////////////////////////////////////////////////////////////////

				for (int nozzle_n = 0; nozzle_n < int_; nozzle_n++)
				{
					specs_file.get(input_);
					while (input_ != ':')
					{
						specs_file.get(input_);
					}

					int n_;
					specs_file >> n_;
					objeto[objeto_number_].setDownNozzle(nozzle_n, n_);
				}
				////////////////////////////////////////////////////////////////////////////////

				// Right Nozzles
				specs_file.get(input_);
				while (input_ != ':')
				{
					specs_file.get(input_);
				}

				specs_file >> int_;
				objeto[objeto_number_].setTotalRightNozzles(int_);
				if (objeto[objeto_number_].getTotalRightNozzles() > 0) objeto[objeto_number_].InitializeRightNozzles(int_);
				////////////////////////////////////////////////////////////////////////////////

				for (int nozzle_n = 0; nozzle_n < int_; nozzle_n++)
				{
					specs_file.get(input_);
					while (input_ != ':')
					{
						specs_file.get(input_);
					}

					int n_;
					specs_file >> n_;
					objeto[objeto_number_].setRightNozzle(nozzle_n, n_);
				}
				////////////////////////////////////////////////////////////////////////////////

				// Left Nozzles
				specs_file.get(input_);
				while (input_ != ':')
				{
					specs_file.get(input_);
				}

				specs_file >> int_;
				objeto[objeto_number_].setTotalLeftNozzles(int_);
				if (objeto[objeto_number_].getTotalLeftNozzles() > 0) objeto[objeto_number_].InitializeLeftNozzles(int_);
				////////////////////////////////////////////////////////////////////////////////

				for (int nozzle_n = 0; nozzle_n < int_; nozzle_n++)
				{
					specs_file.get(input_);
					while (input_ != ':')
					{
						specs_file.get(input_);
					}

					int n_;
					specs_file >> n_;
					objeto[objeto_number_].setLeftNozzle(nozzle_n, n_);
				}
			}
			////////////////////////////////////////////////////////////////////////////////
			// X possitve nozzles
			specs_file.get(input_);
			while (input_ != ':')
			{
				specs_file.get(input_);
			}

			specs_file >> int_;
			objeto[objeto_number_].setTotalPositiveXNozzles(int_);
			if (objeto[objeto_number_].getTotalPositiveXNozzles() > 0) objeto[objeto_number_].InitializePositiveXNozzles(int_);
			////////////////////////////////////////////////////////////////////////////////

			for (int nozzle_n = 0; nozzle_n < int_; nozzle_n++)
			{
				specs_file.get(input_);
				while (input_ != ':')
				{
					specs_file.get(input_);
				}

				int n_;
				specs_file >> n_;
				objeto[objeto_number_].setPositiveXNozzle(nozzle_n, n_);
			}
			////////////////////////////////////////////////////////////////////////////////

			specs_file.get(input_);
			while (input_ != ':')
			{
				specs_file.get(input_);
			}

			specs_file >> int_;
			objeto[objeto_number_].setTotalNegativeXNozzles(int_);
			if (objeto[objeto_number_].getTotalNegativeXNozzles() > 0) objeto[objeto_number_].InitializeNegativeXNozzles(int_);
			////////////////////////////////////////////////////////////////////////////////

			for (int nozzle_n = 0; nozzle_n < int_; nozzle_n++)
			{
				specs_file.get(input_);
				while (input_ != ':')
				{
					specs_file.get(input_);
				}

				int n_;
				specs_file >> n_;
				objeto[objeto_number_].setNegativeXNozzle(nozzle_n, n_);
			}
			////////////////////////////////////////////////////////////////////////////////

			specs_file.get(input_);
			while (input_ != ':')
			{
				specs_file.get(input_);
			}

			specs_file >> int_;
			objeto[objeto_number_].setTotalPositiveYNozzles(int_);
			if (objeto[objeto_number_].getTotalPositiveYNozzles() > 0) objeto[objeto_number_].InitializePositiveYNozzles(int_);
			////////////////////////////////////////////////////////////////////////////////

			for (int nozzle_n = 0; nozzle_n < int_; nozzle_n++)
			{
				specs_file.get(input_);
				while (input_ != ':')
				{
					specs_file.get(input_);
				}

				int n_;
				specs_file >> n_;
				objeto[objeto_number_].setPositiveYNozzle(nozzle_n, n_);
			}
			////////////////////////////////////////////////////////////////////////////////

			specs_file.get(input_);
			while (input_ != ':')
			{
				specs_file.get(input_);
			}

			specs_file >> int_;
			objeto[objeto_number_].setTotalNegativeYNozzles(int_);
			if (objeto[objeto_number_].getTotalNegativeYNozzles() > 0) objeto[objeto_number_].InitializeNegativeYNozzles(int_);
			////////////////////////////////////////////////////////////////////////////////

			for (int nozzle_n = 0; nozzle_n < int_; nozzle_n++)
			{
				specs_file.get(input_);
				while (input_ != ':')
				{
					specs_file.get(input_);
				}

				int n_;
				specs_file >> n_;
				objeto[objeto_number_].setNegativeYNozzle(nozzle_n, n_);
			}
			////////////////////////////////////////////////////////////////////////////////

			specs_file.get(input_);
			while (input_ != ':')
			{
				specs_file.get(input_);
			}

			specs_file >> int_;
			objeto[objeto_number_].setTotalPositiveZNozzles(int_);
			if (objeto[objeto_number_].getTotalPositiveZNozzles() > 0) objeto[objeto_number_].InitializePositiveZNozzles(int_);
			////////////////////////////////////////////////////////////////////////////////

			for (int nozzle_n = 0; nozzle_n < int_; nozzle_n++)
			{
				specs_file.get(input_);
				while (input_ != ':')
				{
					specs_file.get(input_);
				}

				int n_;
				specs_file >> n_;
				objeto[objeto_number_].setPositiveZNozzle(nozzle_n, n_);
			}
			////////////////////////////////////////////////////////////////////////////////

			specs_file.get(input_);
			while (input_ != ':')
			{
				specs_file.get(input_);
			}

			specs_file >> int_;
			objeto[objeto_number_].setTotalNegativeZNozzles(int_);
			if (objeto[objeto_number_].getTotalNegativeZNozzles() > 0) objeto[objeto_number_].InitializeNegativeZNozzles(int_);
			////////////////////////////////////////////////////////////////////////////////

			for (int nozzle_n = 0; nozzle_n < int_; nozzle_n++)
			{
				specs_file.get(input_);
				while (input_ != ':')
				{
					specs_file.get(input_);
				}

				int n_;
				specs_file >> n_;
				objeto[objeto_number_].setNegativeZNozzle(nozzle_n, n_);
			}
			////////////////////////////////////////////////////////////////////////////////
		}
		////////////////////////////////////////////////////////////////////////////////
		//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX//
		////////////////////////////////////////////////////////////////////////////////

		specs_file.get(input_);
		while (input_ != ':')
		{
			specs_file.get(input_);
		}

		Vector3 pos_;
		specs_file >> pos_.x; specs_file >> pos_.y; specs_file >> pos_.z;
		objeto[objeto_number_].setCockpitCameraPosition(pos_);
		////////////////////////////////////////////////////////////////////////////////

		specs_file.get(input_);
		while (input_ != ':')
		{
			specs_file.get(input_);
		}

		specs_file >> float_;
		objeto[objeto_number_].setFuelCapacity(float_);
		objeto[objeto_number_].setFuel(float_); // llenamos el tanque
		////////////////////////////////////////////////////////////////////////////////

		specs_file.get(input_);
		while (input_ != ':')
		{
			specs_file.get(input_);
		}

		specs_file >> float_;
		objeto[objeto_number_].setRCSCapacity(float_);
		objeto[objeto_number_].setRCS(float_); // llenamos el tanque
		////////////////////////////////////////////////////////////////////////////////

		specs_file.get(input_);
		while (input_ != ':')
		{
			specs_file.get(input_);
		}

		specs_file >> float_;
		objeto[objeto_number_].setO2Capacity(float_);
		objeto[objeto_number_].setO2(float_); // llenamos el tanque
		////////////////////////////////////////////////////////////////////////////////

		specs_file.get(input_);
		while (input_ != ':')
		{
			specs_file.get(input_);
		}

		specs_file >> float_;
		objeto[objeto_number_].setBatteryCapacity(float_);
		objeto[objeto_number_].setBattery(float_);
		////////////////////////////////////////////////////////////////////////////////

		specs_file.get(input_);
		while (input_ != ':')
		{
			specs_file.get(input_);
		}

		specs_file >> float_;
		objeto[objeto_number_].setBatteryChargeRate(float_);
		////////////////////////////////////////////////////////////////////////////////

		specs_file.get(input_);
		while (input_ != ':')
		{
			specs_file.get(input_);
		}

		specs_file >> float_;
		objeto[objeto_number_].setGroundSensorDistance(float_);
		////////////////////////////////////////////////////////////////////////////////

		specs_file.get(input_);
		while (input_ != ':')
		{
			specs_file.get(input_);
		}

		specs_file >> float_;
		objeto[objeto_number_].setLength(float_);
		////////////////////////////////////////////////////////////////////////////////

		specs_file.get(input_);
		while (input_ != ':')
		{
			specs_file.get(input_);
		}

		specs_file >> float_;
		objeto[objeto_number_].setWidth(float_);
		////////////////////////////////////////////////////////////////////////////////

		specs_file.get(input_);
		while (input_ != ':')
		{
			specs_file.get(input_);
		}

		specs_file >> float_;
		objeto[objeto_number_].setHeight(float_);
		////////////////////////////////////////////////////////////////////////////////

		specs_file.get(input_);
		while (input_ != ':')
		{
			specs_file.get(input_);
		}

		specs_file >> float_;
		objeto[objeto_number_].setEquatorialRadius(float_ / 2.f);
		objeto[objeto_number_].setPolarRadius(float_ / 2.f);
		objeto[objeto_number_].setFlattening(float(1.0 - (objeto[objeto_number_].getPolarRadius() / objeto[objeto_number_].getEquatorialRadius())));
		objeto[objeto_number_].setScale(float_);
		////////////////////////////////////////////////////////////////////////////////

		specs_file.get(input_);
		while (input_ != ':')
		{
			specs_file.get(input_);
		}

		specs_file >> float_;
		objeto[objeto_number_].setWingSpan(float_);
		////////////////////////////////////////////////////////////////////////////////

		objeto[objeto_number_].setTopArea(objeto[objeto_number_].getWidth()*objeto[objeto_number_].getLength());
		objeto[objeto_number_].setFrontArea(DirectX::XM_PI*((objeto[objeto_number_].getWidth() / 2)*(objeto[objeto_number_].getWidth() / 2))); // Pi.r2 area of a circle
		objeto[objeto_number_].setSideArea(objeto[objeto_number_].getHeight()*objeto[objeto_number_].getLength());

		specs_file.get(input_);
		while (input_ != ':')
		{
			specs_file.get(input_);
		}

		specs_file >> float_;
		objeto[objeto_number_].setFrontFriction(float_);
		////////////////////////////////////////////////////////////////////////////////

		specs_file.get(input_);
		while (input_ != ':')
		{
			specs_file.get(input_);
		}

		specs_file >> float_;
		objeto[objeto_number_].setMinLift(float_);
		////////////////////////////////////////////////////////////////////////////////

		specs_file.get(input_);
		while (input_ != ':')
		{
			specs_file.get(input_);
		}

		specs_file >> float_;
		objeto[objeto_number_].setMaxLift(float_);
		////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////
	}

	return true;
}

void ObjectCreatorClass::dock_object(std::vector <objectClass>& objeto, int n, int dock_target)
{
	// rotamos al objeto para dejarlo horizontal sobre el objeto orbitado
	//ground_rotation
	quat objeto_rot = vecToVecRotation(GetAxisY(objeto[n].getTotalRotation()), normalized(objeto[n].getPosition() - objeto[dock_target].getPosition()));
	objeto[n].setTotalRotation(objeto[n].getTotalRotation()*~objeto_rot);

	// dockeamos nuestra nave al objetivo
	objeto[n].setDockedTo(dock_target);
	objeto[n].set_docked_to_name(objeto[dock_target].getName());

	// inherited rotation = current rotation, unrotated by parent's rotation
	quat unrotated = quat{ 0.0, 0.0, 0.0, 1.0 } / objeto[dock_target].getTotalRotation();
	objeto[n].setInheritedRotation(objeto[n].getTotalRotation() * unrotated);
	objeto[n].setVelocity(objeto[objeto[n].getDockedTo()].getVelocity());

	// seteamos nuestra posicion de dockeado
	objeto[n].setDocked_position(objeto[n].getPosition() - objeto[dock_target].getPosition());
	objeto[n].setDocked_position(vector_rotation_by_quaternion(objeto[n].getDocked_position(), ~unrotated));

	// paramos completamente nuestra rotacion
	objeto[n].setAngulosX(0.0);
	objeto[n].setAngulosY(0.0);
	objeto[n].setAngulosZ(0.0);

	// deploy landing gear
	objeto[n].setLandingGear(true);

	// ground objetos do cast shadows, always
	objeto[n].setCastsShadows(true);
}

bool ObjectCreatorClass::read_special_properties(std::vector<objectClass>& calling_object, int objeto_number, std::string parent_name, std::string specs_file_)
{
	char input_ = ' ';
	std::string string_ = "";
	double double_ = 0.0;
	float float_ = 0.f;
	int int_ = 0;
	bool bool_ = false;

	//return true;
	std::ifstream specs_file;
	specs_file.open(specs_file_);

	// If it could not open the file then exit.
	if (specs_file.fail())
	{
		std::string mess_ = "Could not open the special properties file: \n" + specs_file_;
		MessageBoxA(m_hwnd, mess_.c_str(), "Error", MB_OK);
		return false;
	}

	if (specs_file.is_open())
	{
		// read number of attached objects
		specs_file.get(input_);
		while (input_ != ':')
		{
			specs_file.get(input_);
		}

		specs_file >> int_;
		if (int_ > 0)
		{
			// creamos la cantidad especificada de objetos acoplados
			for (int n = 0; n < int_; n++)
			{
				objeto_count++;
				objeto.emplace_back();
				////////////////////////////////////////////////////////////////////////
				// set the name
				specs_file.get(input_);
				while (input_ != ':')
				{
					specs_file.get(input_);
				}

				specs_file.get(input_);
				getline(specs_file, string_);

				objeto[objeto_count].setName(string_);
				////////////////////////////////////////////////////////////////////////
				// set the type
				specs_file.get(input_);
				while (input_ != ':')
				{
					specs_file.get(input_);
				}

				specs_file.get(input_);
				specs_file >> string_;
				if (string_ == "Spacecraft") objeto[objeto_count].setType(4);
				else
				{
					MessageBoxA(m_hwnd, "Object type error", objeto[objeto_count].getName().c_str(), MB_OK);
					return false;
				}
				// *** DEBUG *** manejar otros tipos de objetos tambien
				////////////////////////////////////////////////////////////////////////
				// set the technical specifications
				specs_file.get(input_);
				while (input_ != ':')
				{
					specs_file.get(input_);
				}

				specs_file.get(input_);
				specs_file >> string_;

				if (!create_dependant_object(objeto, string_, object_count_)) return false;
				////////////////////////////////////////////////////////////////////////

				objeto[objeto_count].setAttachedTo(parent_name);
				////////////////////////////////////////////////////////////////////////
				// set the position
				specs_file.get(input_);
				while (input_ != ':')
				{
					specs_file.get(input_);
				}

				specs_file.get(input_);
				vec3 pos_;
				specs_file >> pos_.x; specs_file >> pos_.y; specs_file >> pos_.z;

				objeto[objeto_count].setAttachedPosition(pos_);

				bool found_ = false;
				for (int i = 0; i < objeto_count; i++)
				{
					if (objeto[i].getName() == parent_name)
					{
						objeto[objeto_count].setAttachedPosition(objeto[objeto_count].getAttachedPosition() + objeto[i].getAttachedPosition());
						objeto[objeto_count].setDocked_position_b(pos_ + objeto[i].getAttachedPosition());						
						found_ = true;
					}
				}

				if (!found_) objeto[objeto_count].setDocked_position_b(pos_);
				////////////////////////////////////////////////////////////////////////
				// set the X rotation
				specs_file.get(input_);
				while (input_ != ':')
				{
					specs_file.get(input_);
				}

				specs_file.get(input_);
				specs_file >> float_;

				objeto[objeto_count].setDocked_rotation_x(float_ - 0.0001f);
				////////////////////////////////////////////////////////////////////////
				// set the Y rotation
				specs_file.get(input_);
				while (input_ != ':')
				{
					specs_file.get(input_);
				}

				specs_file.get(input_);
				specs_file >> float_;

				objeto[objeto_count].setDocked_rotation_y(float_ - 0.0001f);
				////////////////////////////////////////////////////////////////////////
				// set the Z rotation
				specs_file.get(input_);
				while (input_ != ':')
				{
					specs_file.get(input_);
				}

				specs_file.get(input_);
				specs_file >> float_;

				objeto[objeto_count].setDocked_rotation_z(float_ - 0.0001f);
				////////////////////////////////////////////////////////////////////////
				// leemos los datos adicionales, if any
				specs_file.get(input_);
				while (input_ != ':')
				{
					specs_file.get(input_);
				}

				std::string additional_file;
				specs_file >> additional_file;

				if (additional_file != "none")
				{
					if (!read_special_properties(objeto, objeto_count, objeto[objeto_count].getName(), additional_file))
					{
						MessageBoxA(m_hwnd, "Could not read special properties", object[object_count_].getName().c_str(), MB_OK);
						return false;
					}
				}
			}
			////////////////////////////////////////////////////////////////////////
			// leemos los datos adicionales, if any
			specs_file.get(input_);
			while (input_ != ':')
			{
				specs_file.get(input_);
			}

			specs_file.get(input_);
			std::string stages_file;
			specs_file >> stages_file;

			calling_object[objeto_number].setStagesFile(stages_file);
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// leemos las animaciones, if any
			specs_file.get(input_);
			while (input_ != ':')
			{
				specs_file.get(input_);
			}

			specs_file.get(input_);
			specs_file >> string_;

			if (string_=="true") calling_object[objeto_number].setHasAnimations(true);
			else calling_object[objeto_number].setHasAnimations(false);
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			if (calling_object[objeto_number].getHasAnimations())
			{
				// get the number of animations
				specs_file.get(input_);
				while (input_ != ':')
				{
					specs_file.get(input_);
				}

				specs_file >> int_;

				calling_object[objeto_number].setTotalAnimations(int_);
				if (!calling_object[objeto_number].Inititalize_animations(int_)) return false;
				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// cargamos los datos de cada animacion
				for (int n = 0; n < calling_object[objeto_number].getTotalAnimations(); n++)
				{
					/////////////////////////////////////////////////////////////////////////////////
					// get the animation name
					specs_file.get(input_);
					while (input_ != ':')
					{
						specs_file.get(input_);
					}

					specs_file.get(input_);
					getline(specs_file, string_);

					calling_object[objeto_number].animation[n].name = string_;

					/////////////////////////////////////////////////////////////////////////////////
					// set animation's availability
					specs_file.get(input_);
					while (input_ != ':')
					{
						specs_file.get(input_);
					}

					specs_file.get(input_);
					specs_file >> string_;

					if (string_ == "true") calling_object[objeto_number].animation[n].available = true;
					else calling_object[objeto_number].animation[n].available = false;


					/////////////////////////////////////////////////////////////////////////////////
					// set animation's activated state
					specs_file.get(input_);
					while (input_ != ':')
					{
						specs_file.get(input_);
					}

					specs_file.get(input_);
					specs_file >> string_;

					if (string_ == "true") calling_object[objeto_number].animation[n].activated = true;
					else calling_object[objeto_number].animation[n].activated = false;

					/////////////////////////////////////////////////////////////////////////////////
					// set animation's modification instructions for other animations
					specs_file.get(input_);
					while (input_ != ':')
					{
						specs_file.get(input_);
					}

					specs_file >> int_;

					calling_object[objeto_number].animation[n].modify_total = int_;
					if (calling_object[objeto_number].animation[n].modify_total > 0)
					{
						if (!calling_object[objeto_number].animation[n].Initizalize_other_animations_modification(int_)) return false;
					}

					// cargamos los datos de cada modificacion
					for (int nn = 0; nn < calling_object[objeto_number].animation[n].modify_total; nn++)
					{
						/////////////////////////////////////////////////////////////////////////////////
						// get the name of the animation to modify
						specs_file.get(input_);
						while (input_ != ':')
						{
							specs_file.get(input_);
						}

						specs_file.get(input_);
						getline(specs_file, string_);

						calling_object[objeto_number].animation[n].modify_animation[nn].animation_name = string_;

						/////////////////////////////////////////////////////////////////////////////////
						// set animation's availability
						specs_file.get(input_);
						while (input_ != ':')
						{
							specs_file.get(input_);
						}

						specs_file.get(input_);
						specs_file >> string_;

						if (string_ == "true") calling_object[objeto_number].animation[n].modify_animation[nn].available= true;
						else calling_object[objeto_number].animation[n].modify_animation[nn].available = false;
					}

					/////////////////////////////////////////////////////////////////////////////////
					// get the number of moves per animation
					specs_file.get(input_);
					while (input_ != ':')
					{
						specs_file.get(input_);
					}

					specs_file >> int_;

					calling_object[objeto_number].animation[n].total_moves = int_;
					if (calling_object[objeto_number].animation[n].total_moves > 0)
					{
						if (!calling_object[objeto_number].animation[n].Initizalize_moves(int_)) return false;
					}

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					// cargamos los datos de cada serie de movimientos
					for (int nn = 0; nn < calling_object[objeto_number].animation[n].total_moves; nn++)
					{
						/////////////////////////////////////////////////////////////////////////////////
						// get the name of the animation
						specs_file.get(input_);
						while (input_ != ':')
						{
							specs_file.get(input_);
						}

						specs_file.get(input_);
						getline(specs_file, string_);

						calling_object[objeto_number].animation[n].HandleObject[nn].object_name = string_;

						/////////////////////////////////////////////////////////////////////////////////
						// get the start move time
						specs_file.get(input_);
						while (input_ != ':')
						{
							specs_file.get(input_);
						}

						specs_file >> float_;

						calling_object[objeto_number].animation[n].HandleObject[nn].start_seconds = float_;

						/////////////////////////////////////////////////////////////////////////////////
						// get the end move time
						specs_file.get(input_);
						while (input_ != ':')
						{
							specs_file.get(input_);
						}

						specs_file >> float_;

						calling_object[objeto_number].animation[n].HandleObject[nn].end_seconds = float_;

						/////////////////////////////////////////////////////////////////////////////////
						// get the X rotation angle
						specs_file.get(input_);
						while (input_ != ':')
						{
							specs_file.get(input_);
						}

						specs_file >> double_;

						calling_object[objeto_number].animation[n].HandleObject[nn].rotation_x = double_;

						/////////////////////////////////////////////////////////////////////////////////
						// get the Y rotation angle
						specs_file.get(input_);
						while (input_ != ':')
						{
							specs_file.get(input_);
						}

						specs_file >> double_;

						calling_object[objeto_number].animation[n].HandleObject[nn].rotation_y = double_;

						/////////////////////////////////////////////////////////////////////////////////
						// get the Z rotation angle
						specs_file.get(input_);
						while (input_ != ':')
						{
							specs_file.get(input_);
						}

						specs_file >> double_;

						calling_object[objeto_number].animation[n].HandleObject[nn].rotation_z = double_;

						/////////////////////////////////////////////////////////////////////////////////
						// get the X traslation distance
						specs_file.get(input_);
						while (input_ != ':')
						{
							specs_file.get(input_);
						}

						specs_file >> double_;

						calling_object[objeto_number].animation[n].HandleObject[nn].traslation_x = double_;

						/////////////////////////////////////////////////////////////////////////////////
						// get the Y traslation distance
						specs_file.get(input_);
						while (input_ != ':')
						{
							specs_file.get(input_);
						}

						specs_file >> double_;

						calling_object[objeto_number].animation[n].HandleObject[nn].traslation_y = double_;

						/////////////////////////////////////////////////////////////////////////////////
						// get the Z traslation distance
						specs_file.get(input_);
						while (input_ != ':')
						{
							specs_file.get(input_);
						}

						specs_file >> double_;

						calling_object[objeto_number].animation[n].HandleObject[nn].traslation_z = double_;

						/////////////////////////////////////////////////////////////////////////////////
					}
				}
				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			}
		}

		specs_file.close();
	}

	return true;
}

bool ObjectCreatorClass::create_dependant_object(std::vector <objectClass>& objeto, std::string specifications_file_, int parent_)
{
	// anulamos las texturas predefinidas
	objeto[objeto_count].setTexture1(-1);
	objeto[objeto_count].setTexture2(-1);
	objeto[objeto_count].setTexture3(-1);

	objeto[objeto_count].setInOrbit(object[parent_].getInOrbit());

	// si no esta en orbita
	if (!objeto[objeto_count].getInOrbit())
	{
		objeto[objeto_count].setLatitude(object[parent_].getLatitude());
		objeto[objeto_count].setLongitude(object[parent_].getLongitude());
		objeto[objeto_count].set_altitude(object[parent_].get_altitude());
		objeto[objeto_count].setHeading(object[parent_].getHeading());
	}

	// si esta en orbita
	else
	{
		objeto[objeto_count].setSemiMayorAxis(object[parent_].getSemiMayorAxis());
		objeto[objeto_count].setEccentricity(object[parent_].getEccentricity());
		objeto[objeto_count].setInclination(object[parent_].getInclination());
		objeto[objeto_count].setArgumentOfPerigee(object[parent_].getArgumentOfPerigee());
		objeto[objeto_count].setLongitudeOfAscendingNode(object[parent_].getLongitudeOfAscendingNode());
		objeto[objeto_count].setTrueAnomaly(object[parent_].getTrueAnomaly());
		objeto[objeto_count].setObliquityToOrbit(object[parent_].getObliquityToOrbit());
		objeto[objeto_count].setInitialRotation(object[parent_].getInitialRotation());
		objeto[objeto_count].setSiderealRotationPeriod(object[parent_].getSiderealRotationPeriod());
		objeto[objeto_count].setAngulosY(object[parent_].getAngulosY_());
	}
	////////////////////////////////////////////////////////////////////////////////

	if (!read_specs_file(objeto, specifications_file_, objeto_count))
	{
		MessageBoxA(m_hwnd, "Could not read specs file", objeto[objeto_count].getName().c_str(), MB_OK);
		return false;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	objeto[objeto_count].setOrbiting(object[parent_].getOrbiting());
	objeto[objeto_count].set_orbiting_name(object[parent_].get_orbiting_name());

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	return true;
}