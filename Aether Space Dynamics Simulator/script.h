#ifndef _SCRIPTCLASS_H_
#define _SCRIPTCLASS_H_

#include "stdafx.h"
#include "object.h"
#include "auto.h"
#include "bullet.h"

class ScriptClass
{
public:
	ScriptClass(std::vector<objectClass>& object_, BulletClass& bullet_, AutoClass* auto_b);
	~ScriptClass();

	bool stages_manager(int objeto);
	void setTopAbsoluteParent(int objeto, int original_object);
	void dock_object(int objeto, int target_, float breaking_threshold);
	void undock_object(int objeto);
	void mass_manager();

private:

	void add_mass_to_parent(int objeto, double mass_, int original_object);

	std::vector<objectClass>& object;
	AutoClass* auto_;

	BulletClass& bullet_physics;
};

#endif