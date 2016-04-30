#ifndef _AtmosphereSHADERCLASS_H_
#define _AtmosphereSHADERCLASS_H_

#include "stdafx.h"

class Atmosphere_shader_class
{
private:

	struct atmos_MatrixBufferType
	{
		Matrix world;
		Matrix view;
		Matrix projection;
	};

	struct atmos_BufferType
	{
		float planet_scale;
		float object;
		float type;
		float camera_in_atmosphere;
		Vector3 light_pos;
		float atmosphere_scale;
		Vector3 cameraPosition;
		float transparency;
		Vector3 planet_position;
		float specular_power;

		Vector3 sunset_color;
		float sun_distance;
		Vector3 daylight_color;
		float atmosphere_thickness;
	};

public:
	Atmosphere_shader_class();
	Atmosphere_shader_class(const Atmosphere_shader_class&);
	~Atmosphere_shader_class();

	bool Initialize(HWND);
	bool set_atmosphere_shader();
	bool Set_Atmosphere_shader_parameters(Matrix worldMatrix, Matrix viewMatrix,
		Matrix projectionMatrix, Vector3 light_pos_, float atmosphere_scale_, Vector3 cameraPosition_, float transparency_,
		Vector3 planet_position_, float specular_power_,
		ID3D11ShaderResourceView** texture_array, float planet_scale_, float object_, float type, float camera_in_atmosphere_,
		Vector3 sunset_color_, float sun_distance_, Vector3 daylight_color_, float atmosphere_thickness_);

private:
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

private:
	ID3D11VertexShader* atmos_vertexShader;
	ID3D11PixelShader* atmos_pixelShader;
	ID3D11InputLayout* atmos_layout;
	ID3D11SamplerState* atmos_sampleState;
	ID3D11Buffer* atmos_matrixBuffer;
	ID3D11Buffer* atmos_Buffer;

};


#endif