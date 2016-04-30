#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_

#include "stdafx.h"
#include "object.h"

class CameraClass
{
public:
	CameraClass(std::vector<objectClass>& object_);
	~CameraClass();

	bool update_camera(float elapsedTimeSec, Vector3 mouse_position, float MOUSE_SPEED_X, float MOUSE_SPEED_Y,
		float dt, Quaternion rotacion_total);
	Vector3 GetPosition();
	Vector3 GetPositionb();
	void setPosition(Vector3 pos_);
	void setPositionb(Vector3 pos_b);
	Quaternion getOrientation(){ return camera_orientation; }
	Vector3 getCamera_yAxis();

	void Render(Vector3 look_at);
	void Renderb(Vector3 look_at);
	void Render_Pos(Vector3 pos_, Vector3 look_at);
	Matrix& GetViewMatrix();

	void setActivateRotation(bool);
	void setInCockpit(bool ic_);
	void setRotarCamara(bool);
	void setOrientar(bool orientar_) { orientar = orientar_; };
	void setPreferTargetYAxisOrbiting(bool prefer_){ PreferTargetYAxisOrbiting = prefer_; };
	bool getOrientar(){ return orientar; };
	bool getPreferTargetYAxisOrbiting(){ return PreferTargetYAxisOrbiting; };
	void setOrientation(Quaternion ori_){ camera_orientation = ori_; };

private:
	void rotateOrbit(float headingDegrees, float pitchDegrees, float rollDegrees);
	void updateCameraViewMatrix(Quaternion rotacion_total);

	Vector3 up, camera_position, camera_positionb, lookAt, camera_xAxis, camera_yAxis, camera_zAxis, camera_viewDir, former_camera_pos;
	Matrix camera_viewMatrix;
	Quaternion camera_orientation, camera_b_orientation, former_camera_orientation;
	bool orientar, ActivateRotation, PreferTargetYAxisOrbiting, rotar_camara;
	float camera_RotationSpeed, camera_OrbitOffsetDistance;
	int relativeZ, relativeZ_anterior;
	float fov, default_fov;

	bool fly_by_camera, in_cockpit;

	std::vector<objectClass>& object;
};

#endif