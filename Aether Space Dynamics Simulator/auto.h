#ifndef _AUTO_H_
#define _AUTO_H_

#include "stdafx.h"
#include "object.h"
#include "aerodynamics.h"
#include "bullet.h"

class AutoClass
{
public:
	AutoClass(std::vector<objectClass>& object_, BulletClass& bullet_, Matrix& ViewMatrix_, Matrix& ProjectionMatrix_,
		Matrix& WorldMatrix_, int total_objects_, AeroDynamicsClass& aerodynamics_);

	AutoClass(const AutoClass&);
	~AutoClass();

	void ApplyTranslation(int engine_type, double speed_, char ceDir, int cuerpo);
	void rotate_axis_to_targetVector(int objeto_, char axis_, vec3& targetVector_);
	void manage_power(int objeto);
	bool update(int n);
	bool Match_speed(int n);
	bool Approach(int objeto_, float dist_);
	void RotateX(int objeto, float distance);
	void RotateY(int objeto, float distance);
	void RotateZ(int objeto, float distance);

private:

	double test_rotationX(int n, char axis_, vec3& targetVector_, double speed_);
	double test_rotationY(int n, char axis_, vec3& targetVector_, double speed_);
	double test_rotationZ(int n, char axis_, vec3& targetVector_, double speed_);

	void kill_rotation(int n, double rcs_power);

	bool test_VelocityX(int n, double speed_, vec3& targetVector_);
	bool test_VelocityY(int n, double speed_, vec3& targetVector_);
	bool test_VelocityZ(int n, double speed_, vec3& targetVector_);

	//bool test_ProximityX(int n, double speed_, vec3& targetVector_);
	//bool test_ProximityY(int n, double speed_, vec3& targetVector_);
	//bool test_ProximityZ(int n, double speed_, vec3& targetVector_);

	void fuel_manager(int objeto);

	int total_objects;
	Matrix& WorldMatrix;
	Matrix& ViewMatrix;
	Matrix& ProjectionMatrix;
	std::vector<objectClass>& object;
	AeroDynamicsClass& aerodynamics;
	BulletClass& bullet_physics;
};

#endif