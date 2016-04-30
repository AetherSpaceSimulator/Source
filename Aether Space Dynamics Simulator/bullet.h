#ifndef _BULLET_H_
#define _BULLET_H_

//#define BT_NO_SIMD_OPERATOR_OVERLOADS // esto evita conflictos entre Bullet y DirectXMath

#include "stdafx.h"
#include "object.h"
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>

class BulletClass
{

public:
	BulletClass(std::vector<objectClass>& object_);
	~BulletClass();

	bool Initialize();
	bool create_object(int n);
	bool destroy_objects();
	void apply_central_force(int n, vec3 force);
	int getTotalObjects(){ return total_bullet_objects; };

	void add_fixed_constraint(int objeto, int target_, float breaking_threshold);

	void step_simulation();

	double getAngulosX(int n);
	double getAngulosY(int n);
	double getAngulosZ(int n);

	void applyTorqueX(int n, double angulos);
	void setAngularVel_X(int n, double angulos);

	void applyTorqueY(int n, double angulos);
	void setAngularVel_Y(int n, double angulos);

	void applyTorqueZ(int n, double angulos);
	void setAngularVel_Z(int n, double angulos);
	void setBulletMass(int n, double mass);

private:

	int total_bullet_objects;
	bool load_convex_hull_shape(int n, std::string convex_hull_file);
	bool load_compound_shape(int n, std::string compound_file);
	bool load_primitive_shape(int n, std::string primitive_file);
	bool load_convex_hull_shape_for_compound(std::string convex_hull_file, btConvexHullShape* hull, btVector3 scale_);
	vec3 getLocalAngularVelocity(int n);
	Vector3 getLocalVector(int n, vec3 vec_);

	btBroadphaseInterface* broadphase;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;

	btAlignedObjectArray<btRigidBody*> bullet_object;
	//std::vector <btRigidBody*> bullet_object;
	std::vector <int> bullet_object_aetherID;

	std::vector<objectClass>& object;
};

#endif