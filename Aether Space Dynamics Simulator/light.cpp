#include "stdafx.h"
#include "light.h"

LightClass::LightClass()
{
	multiply_ = 1.0f;
}


LightClass::LightClass(const LightClass& other)
{
}


LightClass::~LightClass()
{
}

void LightClass::SetAmbientColor(Color color_)
{
	m_ambientColor = color_;
	return;
}


void LightClass::SetDiffuseColor(Color color_)
{
	m_diffuseColor = color_;
	return;
}


void LightClass::SetPosition(Vector3 pos_)
{
	m_position = pos_;
	return;
}

void LightClass::SetLookAt(Vector3 pos_)
{
	m_lookAt = pos_;
	return;
}


Color LightClass::GetAmbientColor()
{
	return m_ambientColor;
}


Color LightClass::GetDiffuseColor()
{
	return m_diffuseColor;
}


Vector3 LightClass::GetPosition()
{
	return m_position;
}

void LightClass::GenerateViewMatrix()
{
	Vector3 up, pos_;

	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	pos_ = m_position - m_lookAt;
	pos_.Normalize();
	pos_ *= multiply_;

	// Create the view matrix from the three vectors.
	m_viewMatrix = DirectX::XMMatrixLookAtLH(pos_, m_lookAt, up);

	return;
}

Matrix LightClass::GetViewMatrix()
{
	return m_viewMatrix;
}

void LightClass::Initialize(Vector3 position, Color ambient_color, Color diffuse_color)
{
	m_position = position;
	m_ambientColor = ambient_color;
	m_diffuseColor = diffuse_color;
}

void LightClass::GenerateOrthoMatrix(float width, float depthPlane, float nearPlane)
{
	// Create the orthographic matrix for the light.
	m_orthoMatrix = DirectX::XMMatrixOrthographicLH(width, width, nearPlane, depthPlane);

	return;
}


Matrix LightClass::GetOrthoMatrix()
{
	return m_orthoMatrix;
}

void LightClass::SetDirection(float x, float y, float z)
{
	m_direction = Vector3(x, y, z);
	return;
}


Vector3 LightClass::GetDirection()
{
	return m_direction;
}

void LightClass::setMultiply(float mul_)
{
	multiply_ = mul_;
}

float LightClass::getMultiply()
{
	return multiply_;
}