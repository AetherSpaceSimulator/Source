#include "stdafx.h"
#include "auto.h"

AutoClass::AutoClass(std::vector<objectClass>& object_, BulletClass& bullet_,
	Matrix& ViewMatrix_, Matrix& ProjectionMatrix_, Matrix& WorldMatrix_, int total_objects_, AeroDynamicsClass& aerodynamics_) :
	object(object_), bullet_physics(bullet_), ViewMatrix(ViewMatrix_), ProjectionMatrix(ProjectionMatrix_), WorldMatrix(WorldMatrix_),
	total_objects(total_objects_), aerodynamics(aerodynamics_)
{
}

AutoClass::~AutoClass()
{
}

void AutoClass::ApplyTranslation(int engine_type, double speed_, char ceDir, int cuerpo)
{
	// engine type 1 = main engines
	// engine type 2 = hover engines
	// engine type 3 = rcs nozzles

	if (engine_type == 3)
	{
		// si no podemos trasladar la nave con los rcs entonces no hacer nada
		if (!object[cuerpo].getRCSTraslationEnabled()) return;

		if (ceDir == 'x')
		{
			if (speed_ > 0)
			{
				object[cuerpo].set_Right_on(true);
				object[cuerpo].set_Right_power(max(object[cuerpo].get_Right_power(), float(speed_)));
			}

			else if (speed_ < 0)
			{
				object[cuerpo].set_Left_on(true);
				object[cuerpo].set_Left_power(min(object[cuerpo].get_Left_power(), float(speed_)));
			}
		}

		else if (ceDir == 'y')
		{
			if (speed_ > 0)
			{
				object[cuerpo].set_Up_on(true);
				object[cuerpo].set_Up_power(max(object[cuerpo].get_Up_power(), float(speed_)));
			}

			else if (speed_ < 0)
			{
				object[cuerpo].set_Down_on(true);
				object[cuerpo].set_Down_power(min(object[cuerpo].get_Down_power(), float(speed_)));
			}
		}

		else
		{
			if (speed_ > 0)
			{
				object[cuerpo].set_Forward_on(true);
				object[cuerpo].set_Forward_power(max(object[cuerpo].get_Forward_power(), float(speed_)));
			}

			else if (speed_ < 0)
			{
				object[cuerpo].set_Backward_on(true);
				object[cuerpo].set_Backward_power(min(object[cuerpo].get_Backward_power(), float(speed_)));
			}
		}
	}

	// Si el motor no es de cohete entonces limitarlo al flujo de aire disponible
	if (!object[cuerpo].getRocketEngine() && engine_type != 3) speed_ *= aerodynamics.get_air_density(cuerpo); //***DEBUG *** corregir con un valor mas correcto

	object[cuerpo].setVelocityModified(true);

	vec3 vDir = { 0, 0, 0 };

	//*
	// redirigimos la direccion del impulso de acuerdo a donde apunta el motor
	if (object[cuerpo].getGimbal())
	{
		vDir = vector_vector_rotation_by_angle(GetAxisZ(object[cuerpo].getTotalRotation()),	GetAxisY(object[cuerpo].getTotalRotation()), object[cuerpo].getGimbalAngle());		
	}
	//*/

	else
	{
		if (ceDir == 'z') vDir = GetAxisZ(object[cuerpo].getTotalRotation());
		else if (ceDir == 'x') vDir = GetAxisX(object[cuerpo].getTotalRotation());
		else  vDir = GetAxisY(object[cuerpo].getTotalRotation());
	}

	object[cuerpo].setVelocity(object[cuerpo].getVelocity() + vDir*speed_);
	//if (!object[cuerpo].getInBullet()) object[cuerpo].setVelocity(object[cuerpo].getVelocity() + vDir*speed_);
	//else bullet_physics.apply_central_force(cuerpo, vDir*speed_);
}

double AutoClass::test_rotationX(int n, char axis_, vec3& targetVector_, double speed_)
{
	quat total_rotation_ = object[n].getTotalRotation();
	quat q_temp = { 0.0, 0.0, 0.0, 1.0 };
	double angulo_a_checkear = 0.0;
	double angulo_a_checkear_b = 0.0;

	if (axis_ == 'x') angulo_a_checkear = angle_from_two_vectors(GetAxisX(total_rotation_), targetVector_);
	else if (axis_ == 'y') angulo_a_checkear = angle_from_two_vectors(GetAxisY(total_rotation_), targetVector_);
	else angulo_a_checkear = angle_from_two_vectors(GetAxisZ(total_rotation_), targetVector_);

	q_temp.x = 1.0 * sin(speed_ / 2.0);
	q_temp.w = cos(speed_ / 2.0);
	total_rotation_ = q_temp* total_rotation_;

	if (axis_ == 'x') angulo_a_checkear_b = angle_from_two_vectors(GetAxisX(total_rotation_), targetVector_);
	else if (axis_ == 'y') angulo_a_checkear_b = angle_from_two_vectors(GetAxisY(total_rotation_), targetVector_);
	else angulo_a_checkear_b = angle_from_two_vectors(GetAxisZ(total_rotation_), targetVector_);

	if (angulo_a_checkear_b < angulo_a_checkear) return angulo_a_checkear - angulo_a_checkear_b;

	return false;
}

double AutoClass::test_rotationY(int n, char axis_, vec3& targetVector_, double speed_)
{
	quat total_rotation_ = object[n].getTotalRotation();
	quat q_temp = { 0.0, 0.0, 0.0, 1.0 };
	double angulo_a_checkear = 0.0;
	double angulo_a_checkear_b = 0.0;

	if (axis_ == 'x') angulo_a_checkear = angle_from_two_vectors(GetAxisX(total_rotation_), targetVector_);
	else if (axis_ == 'y') angulo_a_checkear = angle_from_two_vectors(GetAxisY(total_rotation_), targetVector_);
	else angulo_a_checkear = angle_from_two_vectors(GetAxisZ(total_rotation_), targetVector_);

	q_temp.y = 1.0 * sin(speed_ / 2.0);
	q_temp.w = cos(speed_ / 2.0);
	total_rotation_ = q_temp* total_rotation_;

	if (axis_ == 'x') angulo_a_checkear_b = angle_from_two_vectors(GetAxisX(total_rotation_), targetVector_);
	else if (axis_ == 'y') angulo_a_checkear_b = angle_from_two_vectors(GetAxisY(total_rotation_), targetVector_);
	else angulo_a_checkear_b = angle_from_two_vectors(GetAxisZ(total_rotation_), targetVector_);

	if (angulo_a_checkear_b < angulo_a_checkear) return angulo_a_checkear - angulo_a_checkear_b;;

	return false;
}

double AutoClass::test_rotationZ(int n, char axis_, vec3& targetVector_, double speed_)
{
	quat total_rotation_ = object[n].getTotalRotation();
	quat q_temp = { 0.0, 0.0, 0.0, 1.0 };
	double angulo_a_checkear = 0.0;
	double angulo_a_checkear_b = 0.0;

	if (axis_ == 'x') angulo_a_checkear = angle_from_two_vectors(GetAxisX(total_rotation_), targetVector_);
	else if (axis_ == 'y') angulo_a_checkear = angle_from_two_vectors(GetAxisY(total_rotation_), targetVector_);
	else angulo_a_checkear = angle_from_two_vectors(GetAxisZ(total_rotation_), targetVector_);

	q_temp.z = 1.0 * sin(speed_ / 2.0);
	q_temp.w = cos(speed_ / 2.0);
	total_rotation_ = q_temp* total_rotation_;

	if (axis_ == 'x') angulo_a_checkear_b = angle_from_two_vectors(GetAxisX(total_rotation_), targetVector_);
	else if (axis_ == 'y') angulo_a_checkear_b = angle_from_two_vectors(GetAxisY(total_rotation_), targetVector_);
	else angulo_a_checkear_b = angle_from_two_vectors(GetAxisZ(total_rotation_), targetVector_);

	if (angulo_a_checkear_b < angulo_a_checkear) return angulo_a_checkear - angulo_a_checkear_b;;

	return false;
}

void AutoClass::rotate_axis_to_targetVector(int n, char axis_, vec3& targetVector_)
{
	double rcs_power = aerodynamics.getRCS_maneuverability(n);
	double angle;
	if (axis_ == 'x') angle = angle_from_two_vectors(GetAxisX(object[n].getTotalRotation()), targetVector_);
	if (axis_ == 'y') angle = angle_from_two_vectors(GetAxisY(object[n].getTotalRotation()), targetVector_);
	if (axis_ == 'z') angle = angle_from_two_vectors(GetAxisZ(object[n].getTotalRotation()), targetVector_);
	double max_speed = min(rcs_power, angle / 10.f); // dividimos el angulo para suavizar la rotacion en aproximaciones bajas
	double clamp_speed = max_speed * 10;

	if (!object[n].getKillRotation())
	{
		if (test_rotationX(n, axis_, targetVector_, max_speed) > test_rotationX(n, axis_, targetVector_, -max_speed))
		{
			if (bullet_physics.getAngulosX(n) > clamp_speed) bullet_physics.applyTorqueX(n, -max_speed);
			else if (bullet_physics.getAngulosX(n)  < clamp_speed) bullet_physics.applyTorqueX(n, max_speed);
		}

		else
		{
			if (bullet_physics.getAngulosX(n) < -clamp_speed) bullet_physics.applyTorqueX(n, max_speed);
			else if (bullet_physics.getAngulosX(n)  > -clamp_speed) bullet_physics.applyTorqueX(n, -max_speed);
		}
		////////////////////////////////////////////////////////////////////////////////////////////////////////

		if (test_rotationY(n, axis_, targetVector_, max_speed) > test_rotationY(n, axis_, targetVector_, -max_speed))
		{
			if (bullet_physics.getAngulosY(n) > clamp_speed) bullet_physics.applyTorqueY(n, -max_speed);
			else if (bullet_physics.getAngulosY(n)  < clamp_speed) bullet_physics.applyTorqueY(n, max_speed);
		}

		else 
		{
			if (bullet_physics.getAngulosY(n) < -clamp_speed) bullet_physics.applyTorqueY(n, max_speed);
			else if (bullet_physics.getAngulosY(n)  > -clamp_speed) bullet_physics.applyTorqueY(n, -max_speed);
		}
		////////////////////////////////////////////////////////////////////////////////////////////////////////

		if (test_rotationZ(n, axis_, targetVector_, max_speed) > test_rotationZ(n, axis_, targetVector_, -max_speed))
		{
			if (-bullet_physics.getAngulosZ(n) > clamp_speed) bullet_physics.applyTorqueZ(n, -max_speed);
			else if (-bullet_physics.getAngulosZ(n)  < clamp_speed) bullet_physics.applyTorqueZ(n, max_speed);
		}

		else
		{
			if (-bullet_physics.getAngulosZ(n) < -clamp_speed) bullet_physics.applyTorqueZ(n, max_speed);
			else if (-bullet_physics.getAngulosZ(n)  > -clamp_speed) bullet_physics.applyTorqueZ(n, -max_speed);
		}
		////////////////////////////////////////////////////////////////////////////////////////////////////////
	}
}

bool AutoClass::update(int n)
{
	if (object[n].getHoverCurrentPower() > 0.0 || object[n].getMECurrentPower() > 0.0) manage_power(n);

	if (object[n].getDockedTo() == -1)
	{
		int target_ = object[n].getTarget();
		int orbited_ = object[n].getOrbiting();

		object[n].updateOrbitalPositionAndVelocity(object[orbited_].getPosition(), object[orbited_].getVelocity());
		if (target_ > 0) object[target_].updateOrbitalPositionAndVelocity(object[object[target_].getOrbiting()].getPosition(), object[object[target_].getOrbiting()].getVelocity());

		vec3 orbital_position = object[n].getOrbitalPosition();
		vec3 orbital_velocity = object[n].getOrbitalVelocity();

		double rcs_power = aerodynamics.getRCS_maneuverability(n);

		if (object[n].getH_level()) rotate_axis_to_targetVector(n, 'y', object[n].getPosition() - object[orbited_].getPosition());
		if (object[n].getPro_grade()) rotate_axis_to_targetVector(n, 'z', -orbital_velocity);

		if (object[n].getRadialIn()) rotate_axis_to_targetVector(n, 'z', -d3d_to_vec(object[orbited_].getPosition_b()));
		if (object[n].getRadialOut()) rotate_axis_to_targetVector(n, 'z', d3d_to_vec(object[orbited_].getPosition_b()));

		if (object[n].getRetroDiff()) rotate_axis_to_targetVector(n, 'z', orbital_velocity - object[target_].getOrbitalVelocity());

		if (object[n].getRetro_grade()) rotate_axis_to_targetVector(n, 'z', orbital_velocity);
		if (object[n].getNormal()) rotate_axis_to_targetVector(n, 'z', -orbital_velocity ^ orbital_position);
		if (object[n].getAnti_normal()) rotate_axis_to_targetVector(n, 'z', orbital_velocity ^ orbital_position);
		if (object[n].getLock_Target()) rotate_axis_to_targetVector(n, 'z', d3d_to_vec(object[n].getPosition_b() - object[target_].getPosition_b()));

		if (object[n].getMatchSpeed())
		{
			if (!Match_speed(n)) return false;
		}

		if (object[n].getApproach())
		{
			if (!Approach(n, object[n].getApproachDist())) return false;
		}

		if (object[n].getAlignToTargetsUp()) rotate_axis_to_targetVector(n, 'y', GetAxisY(object[target_].getTotalRotation()));

		if (object[n].getKillRotation()) kill_rotation(n, rcs_power);

		if (object[n].getKillRotationX())
		{
			if (bullet_physics.getAngulosX(n)<0.0) bullet_physics.applyTorqueX(n, rcs_power);
			if (bullet_physics.getAngulosX(n)>0.0) bullet_physics.applyTorqueX(n, -rcs_power);
			if (bullet_physics.getAngulosX(n) > -rcs_power && bullet_physics.getAngulosX(n) < rcs_power) bullet_physics.setAngularVel_X(n, 0.0);
			if (bullet_physics.getAngulosX(n) == 0.0) object[n].setKillRotationX(false);
		}

		if (object[n].getKillRotationY())
		{
			if (bullet_physics.getAngulosY(n)<0.0) bullet_physics.applyTorqueY(n, rcs_power);
			if (bullet_physics.getAngulosY(n)>0.0) bullet_physics.applyTorqueY(n, -rcs_power);
			if (bullet_physics.getAngulosY(n) > -rcs_power && bullet_physics.getAngulosY(n) < rcs_power) bullet_physics.setAngularVel_Y(n, 0.0);
			if (bullet_physics.getAngulosY(n) == 0.0) object[n].setKillRotationY(false);
		}

		if (object[n].getKillRotationZ())
		{
			if (bullet_physics.getAngulosZ(n)<0.0) bullet_physics.applyTorqueZ(n, rcs_power);
			if (bullet_physics.getAngulosZ(n)>0.0) bullet_physics.applyTorqueZ(n, -rcs_power);
			if (bullet_physics.getAngulosZ(n) > -rcs_power && bullet_physics.getAngulosZ(n) < rcs_power) bullet_physics.setAngularVel_Z(n, 0.0);
			if (bullet_physics.getAngulosZ(n) == 0.0) object[n].setKillRotationZ(false);
		}
	}

	return true;
}

bool AutoClass::Match_speed(int n)
{
	double rcs_power = aerodynamics.getRCS_maneuverability(n);
	double proximity_ = distances(object[object[n].getTarget()].getVelocity(), object[n].getVelocity());
	double speed_ = min(rcs_power, proximity_);

	if (test_VelocityX(n, speed_, object[object[n].getTarget()].getVelocity())) ApplyTranslation(3, speed_, 'x', n);
	if (test_VelocityX(n, -speed_, object[object[n].getTarget()].getVelocity())) ApplyTranslation(3, -speed_, 'x', n);

	if (test_VelocityY(n, speed_, object[object[n].getTarget()].getVelocity())) ApplyTranslation(3, speed_, 'y', n);
	if (test_VelocityY(n, -speed_, object[object[n].getTarget()].getVelocity())) ApplyTranslation(3, -speed_, 'y', n);

	if (test_VelocityZ(n, speed_, object[object[n].getTarget()].getVelocity())) ApplyTranslation(3, speed_, 'z', n);
	if (test_VelocityZ(n, -speed_, object[object[n].getTarget()].getVelocity())) ApplyTranslation(3, -speed_, 'z', n);

	return true;
}

bool AutoClass::Approach(int objeto_, float t_distance_)
{
	int target_ = object[objeto_].getTarget();
	double distance_ = distances(object[objeto_].getPosition(), object[target_].getPosition());
	double increased_speed = (distance_ - t_distance_)/100;
	vec3 target_speed = object[target_].getVelocity() + normalized(object[target_].getPosition() - object[objeto_].getPosition())*increased_speed;

	double rcs_power = aerodynamics.getRCS_maneuverability(objeto_);
	double proximity_ = distances(target_speed, object[objeto_].getVelocity());
	double speed_ = min(rcs_power, proximity_);

	if (test_VelocityX(objeto_, speed_, target_speed)) ApplyTranslation(3, speed_, 'x', objeto_);
	if (test_VelocityX(objeto_, -speed_, target_speed)) ApplyTranslation(3, -speed_, 'x', objeto_);

	if (test_VelocityY(objeto_, speed_, target_speed)) ApplyTranslation(3, speed_, 'y', objeto_);
	if (test_VelocityY(objeto_, -speed_, target_speed)) ApplyTranslation(3, -speed_, 'y', objeto_);

	if (test_VelocityZ(objeto_, speed_, target_speed)) ApplyTranslation(3, speed_, 'z', objeto_);
	if (test_VelocityZ(objeto_, -speed_, target_speed)) ApplyTranslation(3, -speed_, 'z', objeto_);

	return true;
}

bool AutoClass::test_VelocityX(int n, double speed_, vec3& targetVector_)
{
	vec3 test_speed = GetAxisX(object[n].getTotalRotation())*speed_ + object[n].getVelocity();

	double proximity_a = distances(targetVector_, object[n].getVelocity());
	double proximity_b = distances(targetVector_, test_speed);

	if (proximity_b < proximity_a) return true;

	return false;
}

bool AutoClass::test_VelocityY(int n, double speed_, vec3& targetVector_)
{
	vec3 test_speed = GetAxisY(object[n].getTotalRotation())*speed_ + object[n].getVelocity();

	double proximity_a = distances(targetVector_, object[n].getVelocity());
	double proximity_b = distances(targetVector_, test_speed);

	if (proximity_b < proximity_a) return true;

	return false;
}

bool AutoClass::test_VelocityZ(int n, double speed_, vec3& targetVector_)
{
	vec3 test_speed = GetAxisZ(object[n].getTotalRotation())*speed_ + object[n].getVelocity();

	double proximity_a = distances(targetVector_, object[n].getVelocity());
	double proximity_b = distances(targetVector_, test_speed);

	if (proximity_b < proximity_a) return true;

	return false;
}

void AutoClass::manage_power(int n)
{
	// *** DEBUG *** esto solo hace que el objeto de su empuje al objeto al que esta acoplado
	// y no al objeto padre en el array de objetos acoplados
	//if (object[n].getDockedTo() != -1) n = object[n].getDockedTo();

	//MessageBoxA(m_hwnd, "OK", "Error", MB_OK);

	double main_speed_ = object[n].getMainEnginePower();
	double hover_speed_ = object[n].getHoverPower();

	//if (!object[n].getInBullet())
	//{
	main_speed_ = main_speed_ / object[n].getMass()* dt_b;
	hover_speed_ = hover_speed_ / object[n].getMass()* dt_b;
	//}

	// Aqui enviamos el thrust del motor principal y el motor hover, el valor es el porcentaje del total del thrust del motor en Newtons,
	if (object[n].getMECurrentPower() > 0.0)
	{
		fuel_manager(n);
		ApplyTranslation(1, -object[n].getMECurrentPower()*main_speed_ / 100, 'z', n);
		//MessageBoxA(m_hwnd, "OK", "Error", MB_OK);
	}

	if (object[n].getHoverCurrentPower() > 0.0) ApplyTranslation(2, object[n].getHoverCurrentPower()*hover_speed_ / 100, 'y', n);
}

void AutoClass::RotateX(int n, float angle_)
{
	quat q_temp = { 0.0, 0.0, 0.0, 1.0 };
	q_temp.x = 1.0 * sin(angle_ / 2.0);
	q_temp.w = cos(angle_ / 2.0);
	object[n].setTotalRotation(q_temp*object[n].getTotalRotation());
}

void AutoClass::RotateY(int n, float angle_)
{
	quat q_temp = { 0.0, 0.0, 0.0, 1.0 };
	q_temp.y = 1.0 * sin(angle_ / 2.0);
	q_temp.w = cos(angle_ / 2.0);
	object[n].setTotalRotation(q_temp*object[n].getTotalRotation());
}

void AutoClass::RotateZ(int n, float angle_)
{
	quat q_temp = { 0.0, 0.0, 0.0, 1.0 };
	q_temp.z = 1.0 * sin(angle_ / 2.0);
	q_temp.w = cos(angle_ / 2.0);
	object[n].setTotalRotation(q_temp*object[n].getTotalRotation());
}

void AutoClass::fuel_manager(int objeto)
{
	if  (object[objeto].getFuel() > 0.f)
	{ 
		double fuel_gone = (object[objeto].getFuelConsumption() * (object[objeto].getMECurrentPower() / 100)) * dt_b;
		object[objeto].setFuel(float(object[objeto].getFuel() - fuel_gone));

		if (object[objeto].getTopArtificialParent() == -1) object[objeto].setMass(object[objeto].getMass() - fuel_gone);
		else object[object[objeto].getTopArtificialParent()].setMass(object[object[objeto].getTopArtificialParent()].getMass() - fuel_gone);
	}

	// si se acabo el combustible
	else
	{
		object[objeto].setFuel(0.0);
		object[objeto].setMEcurrentPower(0.0);
	}
}

void AutoClass::kill_rotation(int n, double rcs_power)
{
	double rot_axis_X = bullet_physics.getAngulosX(n);
	double rot_axis_Y = bullet_physics.getAngulosY(n);
	double rot_axis_Z = bullet_physics.getAngulosZ(n);

	bullet_physics.applyTorqueX(n, -sign(rot_axis_X)*min(rcs_power, abs(ToDegrees_d(rot_axis_X)*rcs_power)));
	bullet_physics.applyTorqueY(n, -sign(rot_axis_Y)*min(rcs_power, abs(ToDegrees_d(rot_axis_Y)*rcs_power)));
	bullet_physics.applyTorqueZ(n, -sign(rot_axis_Z)*min(rcs_power, abs(ToDegrees_d(rot_axis_Z)*rcs_power)));

	if (abs(rot_axis_X) < 0.0001 && abs(rot_axis_Y) < 0.0001 && abs(rot_axis_Z) < 0.0001) object[n].setKillRotation(false);
}