#include "stdafx.h"
#include "bullet.h"

BulletClass::BulletClass(std::vector<objectClass>& object_) : object(object_)
{
	total_bullet_objects = 0;

	broadphase = nullptr;
	collisionConfiguration = nullptr;
	dispatcher = nullptr;
	solver = nullptr;
	dynamicsWorld = nullptr;
}

BulletClass::~BulletClass()
{
	//cleanup in the reverse order of creation/initialization

	//remove all constraints
	for (int i = dynamicsWorld->getNumConstraints() - 1; i >= 0; i--)
	{
		btTypedConstraint* constraint = dynamicsWorld->getConstraint(i);
		dynamicsWorld->removeConstraint(constraint);
		delete constraint;
	}

	// remove and delete all objects
	for (int n = dynamicsWorld->getNumCollisionObjects() - 1; n >= 0; n--)
	{
		//remove and delete all the child shapes of compound shapes
		if (object[bullet_object_aetherID[n]].getCollisionShapeType() == 1)
		{
			for (int nn = 0; nn < ((btCompoundShape*)bullet_object[n]->getCollisionShape())->getNumChildShapes(); nn++)
				delete ((btCompoundShape*)bullet_object[n]->getCollisionShape())->getChildShape(nn);

			for (int nn = 0; nn < ((btCompoundShape*)bullet_object[n]->getCollisionShape())->getNumChildShapes(); nn++)
				((btCompoundShape*)bullet_object[n]->getCollisionShape())->removeChildShapeByIndex(nn);
		}

		// delete motion states and collision shapes
		delete bullet_object[n]->getMotionState();
		delete bullet_object[n]->getCollisionShape();

		// get a pointer to the objects array and remove and delete all the objects
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[n];
		btRigidBody* body = btRigidBody::upcast(obj);
		dynamicsWorld->removeCollisionObject(obj);
		SAFE_DELETE(obj);
	}

	// clear the vector
	bullet_object.clear();

	SAFE_DELETE(dynamicsWorld);
	SAFE_DELETE(solver);
	SAFE_DELETE(dispatcher);
	SAFE_DELETE(collisionConfiguration);
	SAFE_DELETE(broadphase);
}

bool BulletClass::Initialize()
{
	broadphase = new btDbvtBroadphase();
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	btGImpactCollisionAlgorithm::registerAlgorithm(dispatcher);
	solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, 0, 0));

	return true;
}

bool BulletClass::create_object(int n)
{
	// no shape
	if (object[n].getCollisionShapeType() == -1) return true;

	// primitive shape
	if (object[n].getCollisionShapeType() == 0)
	{
		if (!load_primitive_shape(n, object[n].getCollisionShapeFile())) return false;
	}

	// compound shape
	else if (object[n].getCollisionShapeType() == 1)
	{
		if (!load_compound_shape(n, object[n].getCollisionShapeFile())) return false;
	}

	// convex hull shape
	else if (object[n].getCollisionShapeType() == 2)
	{
		if (!load_convex_hull_shape(n, object[n].getCollisionShapeFile())) return false;
	}

	// other shape type
	else
	{
		std::string message = "Unrecognized collision shape type: " + std::to_string(object[n].getCollisionShapeType());
		MessageBoxA(m_hwnd, message.c_str(), "Error", MB_OK);
		return false;
	}

	dynamicsWorld->addRigidBody(bullet_object[total_bullet_objects]);

	total_bullet_objects++;

	return true;
}

bool BulletClass::destroy_objects()
{
	//cleanup in the reverse order of creation/initialization

	//remove all constraints
	for (int i = dynamicsWorld->getNumConstraints() - 1; i >= 0; i--)
	{
		btTypedConstraint* constraint = dynamicsWorld->getConstraint(i);
		dynamicsWorld->removeConstraint(constraint);
		delete constraint;
	}

	// remove and delete all objects
	for (int n = dynamicsWorld->getNumCollisionObjects() - 1; n >= 0; n--)
	{
		//////////////////////////////////////////////////////////////////////
		//	set the angular velocity, from Bullet World to Aether Local		//
		vec3 rot_vec = getLocalAngularVelocity(bullet_object_aetherID[n]);

		object[bullet_object_aetherID[n]].setAngulosX(rot_vec.x);
		object[bullet_object_aetherID[n]].setAngulosY(rot_vec.y);
		object[bullet_object_aetherID[n]].setAngulosZ(-rot_vec.z);
		//																	//
		//////////////////////////////////////////////////////////////////////

		//remove and delete all the child shapes of compound shapes
		if (object[bullet_object_aetherID[n]].getCollisionShapeType() == 1)
		{
			for (int nn = 0; nn < ((btCompoundShape*)bullet_object[n]->getCollisionShape())->getNumChildShapes(); nn++)
				delete ((btCompoundShape*)bullet_object[n]->getCollisionShape())->getChildShape(nn);

			for (int nn = 0; nn < ((btCompoundShape*)bullet_object[n]->getCollisionShape())->getNumChildShapes(); nn++)
				((btCompoundShape*)bullet_object[n]->getCollisionShape())->removeChildShapeByIndex(nn);
		}

		// delete motion states and collision shapes
		delete bullet_object[n]->getMotionState();
		delete bullet_object[n]->getCollisionShape();

		// get a pointer to the objects array and remove and delete all the objects
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[n];
		btRigidBody* body = btRigidBody::upcast(obj);
		dynamicsWorld->removeCollisionObject(obj);
		SAFE_DELETE(obj);

		object[bullet_object_aetherID[n]].setBulletId(-1);
		object[bullet_object_aetherID[n]].setInBullet(false);
	}

	// clear the vector
	bullet_object.clear();

	total_bullet_objects = 0;
	bullet_object.clear();
	bullet_object_aetherID.clear();

	return true;
}

void BulletClass::step_simulation()
{
	for (int n = 0; n < total_bullet_objects; n++)
	{
		// damos consistencia a la escala de la posicion modificada Aether y la establecemos como la posicion Bullet
		Vector3 pos_b = object[bullet_object_aetherID[n]].getPosition_bullet();
		bullet_object[n]->getWorldTransform().setOrigin(btVector3(btScalar(pos_b.x), btScalar(pos_b.y), btScalar(-pos_b.z)));

		// damos consistencia a la escala de la velocidad modificada Aether y la establecemos como la velocidad Bullet
		Vector3 vel_b = object[bullet_object_aetherID[n]].getVelocity_b();
		bullet_object[n]->setLinearVelocity(btVector3(btScalar(vel_b.x), btScalar(vel_b.y), btScalar(-vel_b.z)));
	}

	dynamicsWorld->stepSimulation(dt_b, 10);

	for (int n = 0; n < total_bullet_objects; n++)
	{
		// *** DEBUG *** verificar si esto hace que las posiciones de bullet y aether concuerden
		//btVector3 pos_ = bullet_object[n]->getCenterOfMassPosition();
		//object[bullet_object_aetherID[n]].setPosition(vec3{ double(pos_.getX()), double(pos_.getY()), double(-pos_.getZ())} + object[control].getPosition());

		// cambiamos la escala de la velocidad Bullet a la escala total Aether y la sumamos a la velocidad total Aether
		btVector3 vel_ = bullet_object[n]->getLinearVelocity();
		object[bullet_object_aetherID[n]].setVelocity(aether_velocity + (vec3{ double(vel_.getX()), double(vel_.getY()), double(-vel_.getZ()) }));

		// establecemos la rotacion total Bullet como la rotacion total Aether
		btQuaternion rot_ = bullet_object[n]->getWorldTransform().getRotation();
		object[bullet_object_aetherID[n]].setTotalRotation(quatChangeCoordSystem(
			~quat{ double(rot_.getX()), double(rot_.getY()), double(rot_.getZ()), double(rot_.getW()) }));
	}
}

double BulletClass::getAngulosX(int n)
{
	if (object[n].getInBullet()) return getLocalAngularVelocity(n).x;
	else return object[n].getAngulosX_();
}

double BulletClass::getAngulosY(int n)
{
	if (object[n].getInBullet()) return getLocalAngularVelocity(n).y;
	else return object[n].getAngulosY_();
}

double BulletClass::getAngulosZ(int n)
{
	if (object[n].getInBullet()) return -getLocalAngularVelocity(n).z;
	else return object[n].getAngulosZ_();
}

vec3 BulletClass::getLocalAngularVelocity(int n)
{
	// set the angular velocity, from Bullet World to Aether Local
	quat unrotated = quatChangeCoordSystem(~object[n].getTotalRotation()) / identity_quat();
	btVector3 ang_vel = bullet_object[object[n].getBulletId()]->getAngularVelocity();

	vec3 angular_velocity = GetAxisX(unrotated)* double(ang_vel.getX());
	angular_velocity += GetAxisY(unrotated)* double(ang_vel.getY());
	angular_velocity += GetAxisZ(unrotated)* double(ang_vel.getZ());

	return angular_velocity;
}

Vector3 BulletClass::getLocalVector(int n, vec3 vec_)
{
	// set the angular velocity, from Bullet World to Aether Local
	quat unrotated = quatChangeCoordSystem(object[n].getTotalRotation()) / identity_quat();

	vec3 ret_vec = GetAxisX(unrotated)* vec_.x;
	ret_vec += GetAxisY(unrotated)* vec_.y;
	ret_vec += GetAxisZ(unrotated)* vec_.z;

	return Vector3{ float(ret_vec.x), float(ret_vec.y), float(ret_vec.z) };
}

void BulletClass::apply_central_force(int n, vec3 force)
{
	bullet_object[object[n].getBulletId()]->setLinearVelocity(bullet_object[object[n].getBulletId()]->getLinearVelocity() +
		btVector3{ btScalar(force.x), btScalar(force.y), btScalar(-force.z) });
	//bullet_object[object[n].getBulletId()].applyCentralForce(btVector3{ btScalar(force.x), btScalar(force.y), btScalar(-force.z) });
}

void BulletClass::applyTorqueX(int n, double angulos)
{
	// si tiene un avatar en bullet
	if (object[n].getInBullet())
	{
		object[n].addAngulosX(angulos);
		//angulos *= 6;

		vec3 axis_ = GetAxisX(object[n].getTotalRotation())*angulos;
		//bullet_object[object[n].getBulletId()].applyTorque(btVector3(btScalar(axis_.x), btScalar(axis_.y), btScalar(-axis_.z)));
		bullet_object[object[n].getBulletId()]->setAngularVelocity(bullet_object[object[n].getBulletId()]->getAngularVelocity() +
			btVector3(btScalar(axis_.x), btScalar(axis_.y), btScalar(-axis_.z)));
	}

	else object[n].addAngulosX(angulos);
};

void BulletClass::applyTorqueY(int n, double angulos)
{
	// si tiene un avatar en bullet
	if (object[n].getInBullet())
	{
		object[n].addAngulosY(angulos);
		//angulos *= 6;

		vec3 axis_ = GetAxisY(object[n].getTotalRotation())*angulos;
		//bullet_object[object[n].getBulletId()].applyTorque(btVector3(btScalar(axis_.x), btScalar(axis_.y), btScalar(-axis_.z)));	
		bullet_object[object[n].getBulletId()]->setAngularVelocity(bullet_object[object[n].getBulletId()]->getAngularVelocity() +
			btVector3(btScalar(axis_.x), btScalar(axis_.y), btScalar(-axis_.z)));
	}

	else object[n].addAngulosY(angulos);
};

void BulletClass::applyTorqueZ(int n, double angulos)
{
	// si tiene un avatar en bullet
	if (object[n].getInBullet())
	{
		object[n].addAngulosZ(angulos);
		//angulos *= 6;

		vec3 axis_ = GetAxisZ(object[n].getTotalRotation())*angulos;
		//bullet_object[object[n].getBulletId()].applyTorque(btVector3(btScalar(axis_.x), btScalar(axis_.y), btScalar(-axis_.z)));
		bullet_object[object[n].getBulletId()]->setAngularVelocity(bullet_object[object[n].getBulletId()]->getAngularVelocity() +
			btVector3(btScalar(axis_.x), btScalar(axis_.y), btScalar(-axis_.z)));
	}

	else object[n].addAngulosZ(angulos);
};

void BulletClass::setAngularVel_X(int n, double angulos)
{
	object[n].setAngulosX(angulos);

	// si tiene un avatar en bullet
	if (object[n].getInBullet())
	{
		vec3 axis_ = GetAxisX(object[n].getTotalRotation())*angulos;
		bullet_object[object[n].getBulletId()]->setAngularVelocity(btVector3(btScalar(axis_.x), btScalar(axis_.y), btScalar(-axis_.z)));
	}
};

void BulletClass::setAngularVel_Y(int n, double angulos)
{
	object[n].setAngulosY(angulos);

	// si tiene un avatar en bullet
	if (object[n].getInBullet())
	{
		vec3 axis_ = GetAxisY(object[n].getTotalRotation())*angulos;
		bullet_object[object[n].getBulletId()]->setAngularVelocity(btVector3(btScalar(axis_.x), btScalar(axis_.y), btScalar(-axis_.z)));
	}
};

void BulletClass::setAngularVel_Z(int n, double angulos)
{
	object[n].setAngulosZ(angulos);

	// si tiene un avatar en bullet
	if (object[n].getInBullet())
	{
		vec3 axis_ = GetAxisZ(object[n].getTotalRotation())*angulos;
		bullet_object[object[n].getBulletId()]->setAngularVelocity(btVector3(btScalar(axis_.x), btScalar(axis_.y), btScalar(-axis_.z)));
	}
};


bool BulletClass::load_convex_hull_shape(int n, std::string convex_hull_file)
{
	// definimos la masa del objeto
	btScalar mass = btScalar(object[n].getMass());
	btVector3 shape_inertia(0, 0, 0);
	Vector3 posi_ = object[n].getPosition_bullet();
	quat rot_ = quatChangeCoordSystem(~object[n].getTotalRotation());

	char input;
	std::ifstream file;
	file.open(convex_hull_file);

	// If it could not open the file then exit.
	if (file.fail())
	{
		std::string message = "Could not open convex hull shape file: " + convex_hull_file;

		MessageBoxA(m_hwnd, message.c_str(), "Error", MB_OK);
		return false;
	}

	Vector3 trid3d;
	int vertex_count;
	btConvexHullShape* hull = new btConvexHullShape();

	if (file.is_open())
	{
		// Read up to the value of vertex count.
		file.get(input);
		while (input != ':')
		{
			file.get(input);
		}

		file >> vertex_count;

		// Read up to the beginning of the data.
		file.get(input);
		while (input != ':')
		{
			file.get(input);
		}
		file.get(input);
		file.get(input);
		////////////////////////////////////////////////////////////////////////////////

		for (int i = 0; i < vertex_count; i++)
		{
			file >> trid3d.x >> trid3d.y >> trid3d.z;
			hull->addPoint({ btScalar(trid3d.x), btScalar(trid3d.y), btScalar(-trid3d.z) }, true);
		}

		btScalar escala = btScalar(object[n].getScale());
		hull->setLocalScaling({ escala, escala, escala });

		// sneaky little bastard, two weeks to find you... TWO WEEKS!
		hull->initializePolyhedralFeatures();
		//hull->setMargin(0.04f);

		file.close();
	}

	//compute inertia
	hull->calculateLocalInertia(mass, shape_inertia);

	// creamos un nuevo motion state
	btDefaultMotionState* motion_state =
		new btDefaultMotionState(btTransform(btQuaternion(btScalar(rot_.x), btScalar(rot_.y), btScalar(rot_.z), btScalar(rot_.w)),
		btVector3(btScalar(posi_.x), btScalar(posi_.y), btScalar(-posi_.z))));

	// creamos el objeto
	btRigidBody::btRigidBodyConstructionInfo objectCI(mass, motion_state, hull, shape_inertia);
	bullet_object.push_back(new btRigidBody(objectCI));
	bullet_object_aetherID.push_back(n);
	bullet_object[total_bullet_objects]->setActivationState(DISABLE_DEACTIVATION);

	// set the angular velocity, from Aether Local to Bullet World
	vec3 rot_vec = GetAxisX(object[n].getTotalRotation())*object[n].getAngulosX_();
	rot_vec += GetAxisY(object[n].getTotalRotation())*object[n].getAngulosY_();
	rot_vec += GetAxisZ(object[n].getTotalRotation())*object[n].getAngulosZ_();
	btVector3 bullet_angular_v = { btScalar(rot_vec.x), btScalar(rot_vec.y), btScalar(-rot_vec.z) };
	bullet_object[total_bullet_objects]->setAngularVelocity(bullet_angular_v);

	return true;
}

bool BulletClass::load_compound_shape(int n, std::string compound_file)
{
	// definimos la masa del objeto
	btScalar mass = btScalar(object[n].getMass());
	btVector3 shape_inertia(0, 0, 0);
	Vector3 posi_ = object[n].getPosition_bullet();
	quat rot_ = quatChangeCoordSystem(~object[n].getTotalRotation());

	std::string type_ = "";
	std::string convex_hull_file = "";
	btVector3 scale_;
	btVector3 position_;
	btQuaternion rotation_;
	btScalar radius, height;

	char input;
	std::ifstream file;
	file.open(compound_file);

	// If it could not open the file then exit.
	if (file.fail())
	{
		std::string message = "Could not open compound shape file: " + compound_file;

		MessageBoxA(m_hwnd, message.c_str(), "Error", MB_OK);
		return false;
	}

	int primitive_count;
	btCompoundShape* compound = new btCompoundShape();

	btTransform tr;

	std::vector<btCollisionShape> child_shape;

	if (file.is_open())
	{
		// Read up to the value of vertex count.
		file.get(input);
		while (input != ':')
		{
			file.get(input);
		}

		file >> primitive_count;

		// Read up to the beginning of the data.
		file.get(input);
		while (input != ':')
		{
			file.get(input);
		}
		file.get(input);
		file.get(input);
		////////////////////////////////////////////////////////////////////////////////

		for (int i = 0; i < primitive_count; i++)
		{
			file.get(input);
			while (input != ':')
			{
				file.get(input);
			}

			file.get(input);
			getline(file, type_);
			////////////////////////////////////////
			if (type_ == "cone")
			{
				file.get(input);
				while (input != ':')
				{
					file.get(input);
				}

				file.get(input);
				file >> radius;
				////////////////////////////////
				file.get(input);
				while (input != ':')
				{
					file.get(input);
				}

				file.get(input);
				file >> height;
			}

			file.get(input);
			while (input != ':')
			{
				file.get(input);
			}

			file.get(input);
			vec3 sca_;
			file >> sca_.x; file >> sca_.y; file >> sca_.z;
			scale_.setX(btScalar(sca_.x));
			scale_.setY(btScalar(sca_.y));
			scale_.setZ(btScalar(sca_.z));
			////////////////////////////////////////
			file.get(input);
			while (input != ':')
			{
				file.get(input);
			}

			file.get(input);
			vec3 pos_;
			file >> pos_.x; file >> pos_.y; file >> pos_.z;
			position_.setX(btScalar(pos_.x));
			position_.setY(btScalar(pos_.y));
			position_.setZ(btScalar(-pos_.z));
			////////////////////////////////////////
			file.get(input);
			while (input != ':')
			{
				file.get(input);
			}

			file.get(input);
			quat rota_;
			file >> rota_.x; file >> rota_.y; file >> rota_.z; file >> rota_.w;
			rota_ = quatChangeCoordSystem(~rota_);
			rotation_.setX(btScalar(rota_.x));
			rotation_.setY(btScalar(rota_.y));
			rotation_.setZ(btScalar(rota_.z));
			rotation_.setW(btScalar(rota_.w));
			////////////////////////////////////////

			btCollisionShape* collision_shape = nullptr;
			if (type_ == "box" || type_ == "cube") collision_shape = new btBoxShape(scale_);
			else if (type_ == "sphere") collision_shape = new btSphereShape(scale_.getX());
			else if (type_ == "cylinder") collision_shape = new btCylinderShape(scale_);
			else if (type_ == "cone")
			{
				collision_shape = new btConeShape(radius, height);
				collision_shape->setLocalScaling(scale_);
			}

			else if (type_ == "convex hull")
			{
				btConvexHullShape* hull = new btConvexHullShape();
				collision_shape = hull;

				file.get(input);
				while (input != ':')
				{
					file.get(input);
				}

				file.get(input);

				file >> convex_hull_file;

				load_convex_hull_shape_for_compound(convex_hull_file, hull, scale_);
			}

			else
			{
				std::string message = "Unrecognized collision shape type: " + type_;

				MessageBoxA(m_hwnd, message.c_str(), "Error", MB_OK);
				return false;
			}

			tr.setIdentity();
			tr.setOrigin(position_);
			tr.setRotation(rotation_);
			compound->addChildShape(tr, collision_shape);
		}

		file.close();
	}

	btScalar escala = btScalar(object[n].getScale());
	compound->setLocalScaling({ escala, escala, escala });

	//compute inertia
	compound->calculateLocalInertia(mass, shape_inertia);

	// creamos un nuevo motion state
	btDefaultMotionState* motion_state =
		new btDefaultMotionState(btTransform(btQuaternion(btScalar(rot_.x), btScalar(rot_.y), btScalar(rot_.z), btScalar(rot_.w)),
		btVector3(btScalar(posi_.x), btScalar(posi_.y), btScalar(-posi_.z))));

	// creamos el objeto
	btRigidBody::btRigidBodyConstructionInfo objectCI(mass, motion_state, compound, shape_inertia);
	bullet_object.push_back(new btRigidBody(objectCI));
	bullet_object_aetherID.push_back(n);
	bullet_object[total_bullet_objects]->setActivationState(DISABLE_DEACTIVATION);

	// set the angular velocity, from Aether Local to Bullet World
	vec3 rot_vec = GetAxisX(object[n].getTotalRotation())*object[n].getAngulosX_();
	rot_vec += GetAxisY(object[n].getTotalRotation())*object[n].getAngulosY_();
	rot_vec += GetAxisZ(object[n].getTotalRotation())*object[n].getAngulosZ_();
	btVector3 bullet_angular_v = { btScalar(rot_vec.x), btScalar(rot_vec.y), btScalar(-rot_vec.z) };
	bullet_object[total_bullet_objects]->setAngularVelocity(bullet_angular_v);

	return true;
}

bool BulletClass::load_primitive_shape(int n, std::string primitive_file)
{
	// definimos la masa del objeto
	btScalar mass = btScalar(object[n].getMass());
	btVector3 shape_inertia(0, 0, 0);
	Vector3 posi_ = object[n].getPosition_bullet();
	quat rot_ = quatChangeCoordSystem(~object[n].getTotalRotation());

	std::string type_ = "";
	btVector3 scale_;
	btVector3 position_;

	char input;
	std::ifstream file;
	file.open(primitive_file);

	// If it could not open the file then exit.
	if (file.fail())
	{
		std::string message = "Could not open compound shape file: " + primitive_file;

		MessageBoxA(m_hwnd, message.c_str(), "Error", MB_OK);
		return false;
	}

	btCollisionShape* primitive = nullptr;

	if (file.is_open())
	{
		// Read up to the beginning of the data.
		file.get(input);
		while (input != ':')
		{
			file.get(input);
		}
		file.get(input);
		file.get(input);
		////////////////////////////////////////////////////////////////////////////////

		file.get(input);
		while (input != ':')
		{
			file.get(input);
		}

		file.get(input);
		file >> type_;
		////////////////////////////////////////
		file.get(input);
		while (input != ':')
		{
			file.get(input);
		}

		file.get(input);
		vec3 sca_;
		file >> sca_.x; file >> sca_.y; file >> sca_.z;
		scale_.setX(btScalar(sca_.x));
		scale_.setY(btScalar(sca_.y));
		scale_.setZ(btScalar(sca_.z));
		////////////////////////////////////////
		file.get(input);
		while (input != ':')
		{
			file.get(input);
		}

		file.get(input);
		vec3 pos_;
		file >> pos_.x; file >> pos_.y; file >> pos_.z;
		position_.setX(btScalar(pos_.x));
		position_.setY(btScalar(pos_.y));
		position_.setZ(btScalar(-pos_.z));
		////////////////////////////////////////

		if (type_ == "box" || type_ == "cube") primitive = new btBoxShape(scale_);
		else if (type_ == "sphere") primitive = new btSphereShape(scale_.getX());
		else if (type_ == "cylinder") primitive = new btCylinderShape(scale_);
		else
		{
			std::string message = "Unrecognized collision shape type: " + type_;

			MessageBoxA(m_hwnd, message.c_str(), "Error", MB_OK);
			return false;
		}

		file.close();
	}

	btScalar escala = btScalar(object[n].getScale());
	primitive->setLocalScaling({ escala, escala, escala });

	//compute inertia
	primitive->calculateLocalInertia(mass, shape_inertia);

	// creamos un nuevo motion state
	btDefaultMotionState* motion_state =
		new btDefaultMotionState(btTransform(btQuaternion(btScalar(rot_.x), btScalar(rot_.y), btScalar(rot_.z), btScalar(rot_.w)),
		btVector3(btScalar(posi_.x), btScalar(posi_.y), btScalar(-posi_.z)) + position_));

	// creamos el objeto
	btRigidBody::btRigidBodyConstructionInfo objectCI(mass, motion_state, primitive, shape_inertia);
	bullet_object.push_back(new btRigidBody(objectCI));
	bullet_object_aetherID.push_back(n);
	bullet_object[total_bullet_objects]->setActivationState(DISABLE_DEACTIVATION);

	// set the angular velocity, from Aether Local to Bullet World
	vec3 rot_vec = GetAxisX(object[n].getTotalRotation())*object[n].getAngulosX_();
	rot_vec += GetAxisY(object[n].getTotalRotation())*object[n].getAngulosY_();
	rot_vec += GetAxisZ(object[n].getTotalRotation())*object[n].getAngulosZ_();
	btVector3 bullet_angular_v = { btScalar(rot_vec.x), btScalar(rot_vec.y), btScalar(-rot_vec.z) };
	bullet_object[total_bullet_objects]->setAngularVelocity(bullet_angular_v);

	return true;
}

bool BulletClass::load_convex_hull_shape_for_compound(std::string convex_hull_file, btConvexHullShape* hull, btVector3 scale_)
{
	char input;
	std::ifstream file;
	file.open(convex_hull_file);

	// If it could not open the file then exit.
	if (file.fail())
	{
		std::string message = "Could not open convex hull shape file: " + convex_hull_file;

		MessageBoxA(m_hwnd, message.c_str(), "Error", MB_OK);
		return false;
	}

	Vector3 trid3d;
	int vertex_count;

	if (file.is_open())
	{
		// Read up to the value of vertex count.
		file.get(input);
		while (input != ':')
		{
			file.get(input);
		}

		file >> vertex_count;

		// Read up to the beginning of the data.
		file.get(input);
		while (input != ':')
		{
			file.get(input);
		}
		file.get(input);
		file.get(input);
		////////////////////////////////////////////////////////////////////////////////

		for (int i = 0; i < vertex_count; i++)
		{
			file >> trid3d.x >> trid3d.y >> trid3d.z;
			hull->addPoint({ btScalar(trid3d.x), btScalar(trid3d.y), btScalar(-trid3d.z) }, true);
		}

		hull->setLocalScaling(scale_);
		hull->initializePolyhedralFeatures();
		//hull->setMargin(0.04f);

		file.close();
	}

	return true;
}

void BulletClass::add_fixed_constraint(int objeto, int target_, float breaking_threshold)
{
	//Config.display_info = std::to_string(1.f/bullet_object[1].getInvMass());
	//*
	btScalar totalMass = 1.f / bullet_object[object[objeto].getBulletId()]->getInvMass() + 1.f / bullet_object[object[target_].getBulletId()]->getInvMass();
	btTransform trA, trB;
	trA.setIdentity();
	trB.setIdentity();

	Vector3 contact_pos = object[objeto].getPosition_bullet() - object[target_].getPosition_bullet();
	contact_pos.Normalize();
	contact_pos *= contact_pos.Distance(object[objeto].getPosition_bullet(), object[target_].getPosition_bullet()) / 2;

	// The following must be changed in the bullet code in order for aether to work properly with constraints:
	// Bullet project: BulletDynamics
	// File: btGeneric6DofSpring2Constraint.cpp
	/*

	void btGeneric6DofSpring2Constraint::calculateTransforms(const btTransform& transA,const btTransform& transB)
{
	// original bullet physics code
	//m_calculatedTransformA = transA * m_frameInA;
	//m_calculatedTransformB = transB * m_frameInB;

	// code modified for Aether Space
	m_calculatedTransformA = m_frameInA;
	m_calculatedTransformB = m_frameInB;

	*/

	trA.setOrigin(btVector3{ btScalar(contact_pos.x), btScalar(contact_pos.y), btScalar(-contact_pos.z) });
	trB.setOrigin(btVector3{ btScalar(contact_pos.x), btScalar(contact_pos.y), btScalar(-contact_pos.z) });

	//*
	btFixedConstraint* fixed = new btFixedConstraint(
		*bullet_object[object[objeto].getBulletId()],
		*bullet_object[object[target_].getBulletId()],
		trA,
		trB);

	fixed->setBreakingImpulseThreshold(breaking_threshold);
	fixed->setOverrideNumSolverIterations(30);
	dynamicsWorld->addConstraint(fixed, false);
	//*/
}

void BulletClass::setBulletMass(int n, double mass)
{
	btVector3 inertia(0, 0, 0);
	bullet_object[object[n].getBulletId()]->getCollisionShape()->calculateLocalInertia(btScalar(mass), inertia);
	bullet_object[object[n].getBulletId()]->setMassProps(btScalar(mass), inertia);
}