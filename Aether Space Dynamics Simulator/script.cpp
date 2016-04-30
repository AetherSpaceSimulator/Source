#include "stdafx.h"
#include "script.h"

ScriptClass::ScriptClass(std::vector<objectClass>& object_, BulletClass& bullet_, AutoClass* auto_b) 
: object(object_), bullet_physics(bullet_), auto_(auto_b)
{
}

ScriptClass::~ScriptClass()
{
}

bool ScriptClass::stages_manager(int objeto)
{
	if (object[objeto].GetStagesFile() != "" && object[objeto].GetStagesFile() != "none")
	{
		char input_ = ' ';
		std::string string_ = "";
		double double_ = 0.0;
		float float_ = 0.f;
		int int_ = 0;
		int stage = 0;
		int total_stages = 0;
		std::string command;

		std::ifstream stages_file;
		stages_file.open(object[objeto].GetStagesFile());

		// If it could not open the file then exit.
		if (stages_file.fail())
		{
			std::string message_ = "Could not open: " + object[objeto].GetStagesFile();
			MessageBoxA(m_hwnd, message_.c_str(), "Error", MB_OK);
			return false;
		}

		if (stages_file.is_open())
		{

			stages_file.get(input_);
			while (input_ != ':')
			{
				stages_file.get(input_);
			}

			stages_file >> total_stages;

			while (!stages_file.eof())
			{
				stages_file >> string_;
				if (string_ == "Stage:")
				{
					stage++;
					if (stage > total_stages) break;

					if (stage == object[objeto].getStage())
					{
						object[objeto].setStage(object[objeto].getStage() + 1);
						int total_commands = 0;

						stages_file.get(input_);
						while (input_ != ':')
						{
							stages_file.get(input_);
						}

						stages_file >> total_commands;

						for (int n = 0; n < total_commands; n++)
						{
							stages_file.get(input_);
							while (input_ != ':' && !stages_file.eof())
							{
								stages_file.get(input_);
							}

							stages_file.get(input_);

							stages_file >> command;

							if (command == "fire_engines")
							{
								bool found = false;
								stages_file.get(input_);
								getline(stages_file, string_);

								for (int n = 0; n < total_objects; n++)
								{
									if (object[n].getName() == string_)
									{
										found = true;
										object[n].setMEcurrentPower(100.0);
									}
								}

								if (!found)
								{
									std::string mess_ = "Could not find object: " + string_;
									MessageBoxA(m_hwnd, mess_.c_str(), "Script Error", MB_OK);
									return false;
								}
							}

							else if (command == "kill_engines")
							{
								bool found = false;
								stages_file.get(input_);
								getline(stages_file, string_);

								for (int n = 0; n < total_objects; n++)
								{
									if (object[n].getName() == string_)
									{
										found = true;
										object[n].setMEcurrentPower(0.0);
									}
								}

								if (!found)
								{
									std::string mess_ = "Could not find object: " + string_;
									MessageBoxA(m_hwnd, mess_.c_str(), "Script Error", MB_OK);
									return false;
								}
							}

							else if (command == "detach")
							{
								bool found = false;
								stages_file.get(input_);
								getline(stages_file, string_);

								for (int n = 0; n < total_objects; n++)
								{
									if (object[n].getName() == string_)
									{
										found = true;
										undock_object(n);
									}
								}

								if (!found)
								{
									std::string mess_ = "Could not find object: " + string_;
									MessageBoxA(m_hwnd, mess_.c_str(), "Script Error", MB_OK);
									return false;
								}
							}

							else if (command == "undock")
							{
								bool found = false;
								stages_file.get(input_);
								getline(stages_file, string_);

								for (int n = 0; n < total_objects; n++)
								{
									if (object[n].getName() == string_)
									{
										found = true;
										undock_object(n);
									}
								}

								if (!found)
								{
									std::string mess_ = "Could not find object: " + string_;
									MessageBoxA(m_hwnd, mess_.c_str(), "Script Error", MB_OK);
									return false;
								}
							}

							else if (command == "push")
							{
								bool found = false;
								stages_file.get(input_);
								getline(stages_file, string_);

								char direction;
								float force;
								stages_file >> direction;
								stages_file >> force;

								for (int n = 0; n < total_objects; n++)
								{
									if (object[n].getName() == string_)
									{
										found = true;
										auto_->ApplyTranslation(3, force * 1e3, direction, n);
									}
								}

								if (!found)
								{
									std::string mess_ = "Could not find object: " + string_;
									MessageBoxA(m_hwnd, mess_.c_str(), "Script Error", MB_OK);
									return false;
								}
							}

							else if (command == "control")
							{
								bool found = false;
								stages_file.get(input_);
								getline(stages_file, string_);

								for (int n = 0; n < total_objects; n++)
								{
									if (object[n].getName() == string_)
									{
										found = true;
										Config.create_projected_sentences = true;
										control = n;
										universe_scale = 1.0f / float(object[control].getScale());
										switched_shadows = true;
										switched_object = true;
									}
								}

								if (!found) 
								{
									std::string mess_ = "Could not find object: " + string_;
									MessageBoxA(m_hwnd, mess_.c_str(), "Script Error", MB_OK);
									return false;
								}
							}

							else
							{
								std::string mess_ = "Unrecognized command: " + command;
								MessageBoxA(m_hwnd, mess_.c_str(), "Script Error", MB_OK);
								return false;
							}
						}

						break;
					}
				}
			}

			stages_file.close();
		}
	}

	return true;
}

void ScriptClass::mass_manager()
{
	if (Config.system_initialized)
	{
		for (int n = 0; n < total_objects; n++)
		{
			if (!object[n].getInBullet())
			{
				object[n].setMass(object[n].getEmptyMass() +
					object[n].getFuel() +
					object[n].getO2() +
					object[n].getRCS());

				object[n].setCenterOfMass(object[n].getPosition());
				object[n].setIsParent(false);
			}
		}

		for (int n = 0; n < total_objects; n++)
		{
			if (!object[n].getInBullet())
			{
				if (object[n].getDockedTo() != -1 && object[object[n].getDockedTo()].getType() == 4)
				{
					add_mass_to_parent(n, object[n].getMass(), n);
				}

				else object[n].setTopArtificialParent(-1);
			}
		}
	}
}

void ScriptClass::add_mass_to_parent(int objeto, double mass_, int original_object)
{
	for (int n = 0; n < total_objects; n++)
	{
		if (object[objeto].getDockedTo() == n)
		{
			if (object[n].getDockedTo() != -1 && object[object[n].getDockedTo()].getType() == 4) add_mass_to_parent(n, mass_, original_object);
			else
			{
				double total_mass = 0;
				double total_x = 0;
				double total_y = 0;
				double total_z = 0;

				total_mass += object[n].getMass();
				total_x += object[n].getPosition().x * object[n].getMass();
				total_y += object[n].getPosition().y * object[n].getMass();
				total_z += object[n].getPosition().z * object[n].getMass();

				total_mass += object[original_object].getMass();
				total_x += object[original_object].getPosition().x * object[original_object].getMass();
				total_y += object[original_object].getPosition().y * object[original_object].getMass();
				total_z += object[original_object].getPosition().z * object[original_object].getMass();

				object[n].setCenterOfMass(object[n].getPosition() - vec3{ total_x / total_mass, total_y / total_mass, total_z / total_mass });

				object[n].setMass(object[n].getMass() + mass_);
				object[original_object].setTopArtificialParent(n);
				object[n].setIsParent(true);
			}
		}
	}
}

void ScriptClass::dock_object(int objeto, int target_, float breaking_threshold)
{
	if (!object[objeto].getInBullet())
	{
		if (object[objeto].getDockedTo() == -1 && object[objeto].getDockReady())
		{
			// dockeamos nuestra nave al objetivo
			object[objeto].setDockedTo(target_);
			object[objeto].set_docked_to_name(object[target_].getName());

			// sumamos la masa de nuestra nave al objetivo
			if (!object[objeto].getInBullet()) mass_manager();

			// inherited rotation = current rotation, unrotated by parent's rotation
			quat unrotated = quat{ 0.0, 0.0, 0.0, 1.0 } / object[target_].getTotalRotation();
			object[objeto].setInheritedRotation(object[objeto].getTotalRotation() * unrotated);
			object[objeto].setVelocity(object[object[objeto].getDockedTo()].getVelocity());

			// seteamos nuestra posicion de dockeado
			object[objeto].setDocked_position(object[objeto].getPosition() - object[target_].getPosition());
			object[objeto].setDocked_position(vector_rotation_by_quaternion(object[objeto].getDocked_position(), ~unrotated));

			// paramos completamente nuestra rotacion
			bullet_physics.setAngularVel_X(objeto, 0.0);
			bullet_physics.setAngularVel_Y(objeto, 0.0);
			bullet_physics.setAngularVel_Z(objeto, 0.0);

			// desactivamos todos los controles del autopilot
			object[control].KillAutopilot();
		}
	}
	
	else bullet_physics.add_fixed_constraint(objeto, target_, breaking_threshold);
}

void ScriptClass::undock_object(int objeto)
{	
	if (object[objeto].getDockedTo() != -1)
	{
		setTopAbsoluteParent(objeto, objeto);

		int target_ = object[objeto].getTopAbsoluteParent();

		// adoptamos la rotacion del objetivo
		bullet_physics.setAngularVel_X(objeto, bullet_physics.getAngulosX(target_));
		bullet_physics.setAngularVel_Y(objeto, bullet_physics.getAngulosY(target_));
		bullet_physics.setAngularVel_Z(objeto, bullet_physics.getAngulosZ(target_));

		// liberamos nuestra nave
		object[objeto].setDockedTo(-1);
		object[objeto].setTopAbsoluteParent(-1);
		object[objeto].setTopArtificialParent(-1);
		object[objeto].set_docked_to_name("N/A");
		object[objeto].setLanded(false);

		// restamos la masa de nuestra nave al objetivo
		if (!object[objeto].getInBullet()) mass_manager();

		// seteamos la velocidad con la que fue liberada
		vec3 orbital_velocity_ = (object[objeto].getVelocity() - object[target_].getVelocity());

		quat total_rotation = object[target_].getTotalRotation();

		quat unrotated = total_rotation / quat{ 0.0, 0.0, 0.0, 1.0 };
		vec3 orbital_position = object[objeto].getPosition() - object[target_].getPosition();
		vec3 rotated_position = vector_rotation_by_quaternion(orbital_position, unrotated);

		quat q_temp = { 0.0, 0.0, 0.0, 1.0 };
		double angle = bullet_physics.getAngulosX(target_);
		q_temp.x = 1.0 * sin(angle / 2.0);
		q_temp.w = cos(angle / 2.0);
		total_rotation = q_temp* total_rotation;

		q_temp = { 0.0, 0.0, 0.0, 1.0 };
		angle = bullet_physics.getAngulosY(target_);
		q_temp.y = 1.0 * sin(angle / 2.0);
		q_temp.w = cos(angle / 2.0);
		total_rotation = q_temp* total_rotation;

		q_temp = { 0.0, 0.0, 0.0, 1.0 };
		angle = bullet_physics.getAngulosZ(target_);
		q_temp.z = 1.0 * sin(angle / 2.0);
		q_temp.w = cos(angle / 2.0);
		total_rotation = q_temp* total_rotation;

		object[objeto].setVelocity(object[target_].getVelocity() + (vector_rotation_by_quaternion(rotated_position, ~total_rotation) - orbital_position));
	}
}

void ScriptClass::setTopAbsoluteParent(int objeto, int original_object)
{
	for (int n = 0; n < total_objects; n++)
	{
		if (object[objeto].getDockedTo() == n)
		{
			if (object[n].getDockedTo() != -1) setTopAbsoluteParent(n, original_object);
			else
			{
				object[original_object].setTopAbsoluteParent(n);
				object[n].setIsParent(true);
			}
		}
	}
}