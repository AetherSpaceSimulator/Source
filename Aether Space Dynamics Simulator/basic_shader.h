#ifndef _BASICSHADERCLASS_H_
#define _BASICSHADERCLASS_H_

#include "stdafx.h"

class Basic_shader_class
{
private:

	struct b_MatrixBufferType
	{
		Matrix world;
		Matrix view;
		Matrix projection;

		Matrix lightView;
		Matrix lightProjection;
	};

	struct b_BufferType
	{
		float has_light_map;
		float object;
		float has_night_map;
		float has_atmosphere;
		Vector3 light_pos;
		float ilumination;
		Vector3 cameraPosition;
		float transparency;
		Vector3 diffuse_color;
		float specular_power;

		Vector3 orbited_pos;
		float orbited_radius;

		Vector3 object_pos;
		float object_radius;

		float type;
		float has_specular_map;
		float sun_distance;
		float sunlit;

		Vector3 sunset_color;
		float camera_in_atmoshpere;
		Vector3 daylight_color;
		float extra1;
	};

public:
	Basic_shader_class();
	Basic_shader_class(const Basic_shader_class&);
	~Basic_shader_class();

	bool Initialize(HWND);
	bool set_basic_shader();
	bool Set_Basic_shader_parameters(Matrix worldMatrix,
		Matrix viewMatrix,
		Matrix projectionMatrix,
		Matrix lightViewMatrix, Matrix lightProjectionMatrix,
		Vector3 light_pos_, float ilumination_,
		Vector3 cameraPosition_, float transparency_,
		Vector3 diffuse_color_, float specular_power_,
		ID3D11ShaderResourceView** texture_array, ID3D11ShaderResourceView* depthMapTexture, bool has_light_map_, bool has_night_map_, float object_, float scale_, bool has_atmosphere_,
		Vector3 orbited_pos_, float orbited_radius_, Vector3 object_pos_, float object_radius_, float type_, float has_specular_map_,
		float sun_distance_, float sunlit_, Vector3 sunset_color_, float camera_in_atmosphere_, Vector3 daylight_color_, float extra1_);

private:
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

private:
	ID3D11VertexShader* b_vertexShader;
	ID3D11PixelShader* b_pixelShader;
	ID3D11InputLayout* b_layout;

	ID3D11SamplerState* m_sampleStateWrap;
	ID3D11SamplerState* m_sampleStateClamp;

	ID3D11Buffer* b_matrixBuffer;
	ID3D11Buffer* b_Buffer;
	ID3D11Buffer* m_lightBuffer2;

};


#endif