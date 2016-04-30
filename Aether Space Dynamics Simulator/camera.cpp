#include "stdafx.h"
#include "camera.h"

CameraClass::CameraClass(std::vector<objectClass>& object_) : object(object_)
{
	up = Vector3{ 0.0f, 1.0f, 0.0f };
	camera_position = Vector3{ 0.0f, 0.0f, 0.0f };
	former_camera_orientation = Quaternion{ 0.0f, 0.0f, 0.0f, 1.0f };
	former_camera_pos = Vector3{ 0.0f, 0.0f, 0.0f };
	lookAt = Vector3{ 0.0f, 0.0f, 0.0f };
	camera_orientation = Quaternion{ 0.0f, 0.0f, 0.0f, 1.0f };
	camera_b_orientation = Quaternion{ 0.0f, 0.0f, 0.0f, 1.0f };
	orientar = true;
	ActivateRotation = false;
	PreferTargetYAxisOrbiting = true;
	rotar_camara = false;
	camera_RotationSpeed = 2.0f;
	camera_OrbitOffsetDistance = -2.2f;
	camera_xAxis = Vector3(1.0f, 0.0f, 0.0f);
	camera_yAxis = Vector3(0.0f, 1.0f, 0.0f);
	camera_zAxis = Vector3(0.0f, 0.0f, 1.0f);
	camera_viewDir = Vector3{ 0.0f, 0.0f, 0.0f };
	fov = 45.5f;
	default_fov = 45.5f;
	fly_by_camera = false;
	in_cockpit = false;

	camera_orientation = Quaternion::Identity;
	camera_b_orientation = Quaternion::Identity;
	camera_viewMatrix = Matrix::Identity;
}

CameraClass::~CameraClass()
{
}

Vector3 CameraClass::GetPosition()
{
	return camera_position;
}

Vector3 CameraClass::GetPositionb()
{
	return camera_positionb;
}

void CameraClass::setPosition(Vector3 pos_)
{
	camera_position = pos_;
	return;
}

void CameraClass::setPositionb(Vector3 pos_b)
{
	camera_positionb = pos_b;
	return;
}

void CameraClass::Render(Vector3 look_at)
{
	camera_viewMatrix = DirectX::XMMatrixLookAtLH(camera_position, look_at, camera_yAxis);

	return;
}

void CameraClass::Render_Pos(Vector3 pos_, Vector3 look_at)
{
	camera_viewMatrix = DirectX::XMMatrixLookAtLH(pos_, look_at, camera_yAxis);

	return;
}

void CameraClass::Renderb(Vector3 look_at)
{
	camera_viewMatrix = DirectX::XMMatrixLookAtLH(camera_positionb, look_at, camera_yAxis);

	return;
}

Matrix& CameraClass::GetViewMatrix()
{
	return camera_viewMatrix;
}

void CameraClass::rotateOrbit(float yawDegrees, float pitchDegrees, float rollDegrees)
{
	// Calls to rotateOrbit() will rotate the orientation. To turn this into a third
	// person style view, the updateViewMatrix() method will move the camera
	// position back 'm_orbitOffsetDistance' world units along the camera's
	// local negative z axis from the orbit target's world position.

	former_camera_orientation = camera_orientation;

	float yaw = DirectX::XMConvertToRadians(yawDegrees);
	float pitch = DirectX::XMConvertToRadians(pitchDegrees);
	float roll = DirectX::XMConvertToRadians(rollDegrees);

	Quaternion rot;

	if (yaw != 0.0f)
	{
		rot = Quaternion::CreateFromAxisAngle(object[control].getCameraPreferedAxis(), yaw);
		camera_orientation = rot*camera_orientation;
	}

	if (pitch != 0.0f)
	{
		rot = Quaternion::CreateFromAxisAngle(Vector3{ 1.0f, 0.0f, 0.0f }, pitch);
		camera_orientation = camera_orientation*rot;
	}


	if (!PreferTargetYAxisOrbiting)
	{
		rot = Quaternion::CreateFromYawPitchRoll(yaw, pitch, roll);
		camera_b_orientation = camera_b_orientation*rot;
	}

	// *** DEBUG ***
	// eliminamos el error del dibujo de los instanced quads cuando la orientacion de la camara es muy exacta
	if (camera_orientation.x == 0.0) camera_orientation.x = 0.001f;
	else if (camera_orientation.y == 0.0) camera_orientation.y = 0.001f;
	else if (camera_orientation.z == 0.0) camera_orientation.z = 0.001f;
}

void CameraClass::updateCameraViewMatrix(Quaternion rotacion_total)
{
	camera_orientation.Normalize();

	if (orientar)
	{
		Quaternion orientado = camera_orientation;
		camera_b_orientation = rotacion_total*orientado;
		camera_viewMatrix = Matrix::CreateFromQuaternion(camera_b_orientation);
	}

	else camera_viewMatrix = Matrix::CreateFromQuaternion(camera_b_orientation);

	camera_xAxis = Vector3(camera_viewMatrix(0, 0), camera_viewMatrix(1, 0), camera_viewMatrix(2, 0));
	camera_yAxis = Vector3(camera_viewMatrix(0, 1), camera_viewMatrix(1, 1), camera_viewMatrix(2, 1));
	camera_zAxis = Vector3(camera_viewMatrix(0, 2), camera_viewMatrix(1, 2), camera_viewMatrix(2, 2));

	camera_viewDir = camera_zAxis;

	former_camera_pos = camera_position;
	if (!fly_by_camera) camera_position = -camera_viewDir * camera_OrbitOffsetDistance;

	// si la camara entra bajo tierra entonces volverla a su posicion y orientacion anterior para evitar ver bajo la tierra
	/*
	if (length_f(camera_position - object[object[control].getOrbiting()].getPosition_b()) <=
		(object[object[control].getOrbiting()].getScale_b() / 2.0) + 1.f * universe_scale && !in_cockpit)
	{
		camera_position = former_camera_pos;
		camera_orientation = former_camera_orientation;
	}
	//*/
}

bool CameraClass::update_camera(float elapsedTimeSec, Vector3 mouse_position, float MOUSE_SPEED_X, float MOUSE_SPEED_Y,
	float dt, Quaternion rotacion_total)
{
	camera_yAxis = object[control].getCameraPreferedAxis();

	float dx = 0;
	float dy = 0;
	float dz = 0;

	if (ActivateRotation)
	{
		dx = (FLOAT)-mouse_position.x  * elapsedTimeSec * (MOUSE_SPEED_X*(fov / default_fov));
		if (!in_cockpit) dy = (FLOAT)mouse_position.y  * elapsedTimeSec * (MOUSE_SPEED_Y*(fov / default_fov));
		else dy = (FLOAT)-mouse_position.y  * elapsedTimeSec * (MOUSE_SPEED_Y*(fov / default_fov));
	}

	if (!orientar && rotar_camara)
	{
		int equis = int(mouse_position.x);
		int y_griega = int(mouse_position.y);

		if (abs(equis)>abs(y_griega)) dz = equis * elapsedTimeSec * MOUSE_SPEED_X;
		else dz = y_griega * elapsedTimeSec * MOUSE_SPEED_Y;
	}

	rotateOrbit(float(dx *= camera_RotationSpeed*dt), float(dy *= camera_RotationSpeed*dt), float(dz *= camera_RotationSpeed*dt));

	updateCameraViewMatrix(rotacion_total); // Set the new camera position and orientation

	return true;
}

void CameraClass::setActivateRotation(bool state)
{
	ActivateRotation = state;
}

void CameraClass::setRotarCamara(bool state)
{
	rotar_camara = state;
}

void CameraClass::setInCockpit(bool ic_)
{
	in_cockpit = ic_;
}

Vector3 CameraClass::getCamera_yAxis()
{
	return camera_yAxis;
}