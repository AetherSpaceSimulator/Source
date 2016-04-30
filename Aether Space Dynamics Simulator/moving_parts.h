#ifndef _MOVINGPARTS_H_
#define _MOVINGPARTS_H_

#include "stdafx.h"
#include "object.h"
#include "auto.h"
#include "bullet.h"

class MovingPartsClass
{
public:
	MovingPartsClass(std::vector<objectClass>& object_, BulletClass& bullet_, AutoClass* auto_b);
	~MovingPartsClass();

	bool PerformAnimations(int object_number);

private:

	std::vector<objectClass>& object;
	BulletClass& bullet_physics;
	AutoClass* auto_;
};

#endif