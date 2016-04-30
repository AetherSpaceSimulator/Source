#include "stdafx.h"
#include "moving_parts.h"

MovingPartsClass::MovingPartsClass(std::vector<objectClass>& object_, BulletClass& bullet_, AutoClass* auto_b)
: object(object_), bullet_physics(bullet_), auto_(auto_b)
{
}

MovingPartsClass::~MovingPartsClass()
{
}

bool MovingPartsClass::PerformAnimations(int object_number)
{
	// procesamos cada animacion
	for (int n = 0; n < object[object_number].getTotalAnimations(); n++)
	{
		if (object[object_number].animation[n].active)
		{
			// actualizamos el contador de tiempo de la animacion
			object[object_number].animation[n].time_ += dt_b;

			// procesamos cada movimiento de la animacion
			bool performing_move = false;
			for (int nn = 0; nn < object[object_number].animation[n].total_moves; nn++)
			{
				int objeto_a_mover = 0;
				for (int i = 0; i < total_objects; i++)
				{
					if (object[i].getName() == object[object_number].animation[n].HandleObject[nn].object_name) objeto_a_mover = i;
				}

				// la condicion para que se efectue un movimiento es que su tiempo de inicio y su tiempo de fin esten dentro
				// del valor animation_time actual, o que ambos tiempos sean iguales, en cuyo caso el tiempo del movimiento es
				// indefinido
				//MessageBoxA(m_hwnd, "Bingo!", std::to_string(object[object_number].animation[n].time_).c_str(), MB_OK);
				if ((object[object_number].animation[n].HandleObject[nn].start_seconds <= object[object_number].animation[n].time_ &&
					object[object_number].animation[n].time_ < object[object_number].animation[n].HandleObject[nn].end_seconds) ||
					object[object_number].animation[n].HandleObject[nn].start_seconds == 
					object[object_number].animation[n].HandleObject[nn].end_seconds)
				{
					performing_move = true;

					if (object[object_number].animation[n].HandleObject[nn].rotation_x != 0.0) bullet_physics.setAngularVel_X(objeto_a_mover, ToRadians_d(object[object_number].animation[n].HandleObject[nn].rotation_x));
					if (object[object_number].animation[n].HandleObject[nn].rotation_y != 0.0) bullet_physics.setAngularVel_Y(objeto_a_mover, ToRadians_d(object[object_number].animation[n].HandleObject[nn].rotation_y));
					if (object[object_number].animation[n].HandleObject[nn].rotation_z != 0.0) bullet_physics.setAngularVel_Z(objeto_a_mover, ToRadians_d(object[object_number].animation[n].HandleObject[nn].rotation_z));
				}

				else
				{
					if (object[object_number].animation[n].HandleObject[nn].rotation_x != 0.0) bullet_physics.setAngularVel_X(objeto_a_mover, 0.0);
					if (object[object_number].animation[n].HandleObject[nn].rotation_y != 0.0) bullet_physics.setAngularVel_Y(objeto_a_mover, 0.0);
					if (object[object_number].animation[n].HandleObject[nn].rotation_z != 0.0) bullet_physics.setAngularVel_Z(objeto_a_mover, 0.0);
				}
			}

			// si no se esta efectuando ningun movimiento entonces detener la animacion y resetear el animation_time
			if (!performing_move)
			{
				object[object_number].animation[n].active = false;
				object[object_number].animation[n].time_ = 0.f;

				object[object_number].animation[n].activated = true;
				
				// efectuamos las modificaciones establecidas para otras animaciones
				// repetimos la operacion para cada modificacion
				for (int i = 0; i < object[object_number].animation[n].modify_total; i++)
				{
					// buscamos el nombre de la animacion correcto entre todas las animaciones del objeto
					for (int aa = 0; aa < object[object_number].getTotalAnimations(); aa++)
					{
						// modificamos su disponibilidad de acuerdo al parametro establecido
						if (object[object_number].animation[aa].name == object[object_number].animation[n].modify_animation[i].animation_name)
						{
							object[object_number].animation[aa].available = object[object_number].animation[n].modify_animation[i].available;
						}
					}
				}
			}
		}
	}

	return true;
}