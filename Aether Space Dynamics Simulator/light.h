#ifndef _LIGHTCLASS_H_
#define _LIGHTCLASS_H_

#include "stdafx.h"

class LightClass
{
public:
	LightClass();
	LightClass(const LightClass&);
	~LightClass();

	void SetAmbientColor(Color color_);
	void SetDiffuseColor(Color color_);
	void SetPosition(Vector3 pos_);
	void SetLookAt(Vector3 pos_);
	void setMultiply(float mul_);
	float getMultiply();

	void Initialize(Vector3 position, Color ambient_color, Color diffuse_color);

	Color GetAmbientColor();
	Color GetDiffuseColor();
	Vector3 GetPosition();

	void GenerateViewMatrix();
	Matrix GetViewMatrix();

	void GenerateOrthoMatrix(float, float, float);
	Matrix GetOrthoMatrix();

	void SetDirection(float, float, float);
	Vector3 GetDirection();

private:
	Color m_ambientColor;
	Color m_diffuseColor;
	Vector3 m_position;
	Vector3 m_lookAt;
	Matrix m_viewMatrix;

	Matrix m_orthoMatrix;
	Vector3 m_direction;
	float multiply_;
};


#endif