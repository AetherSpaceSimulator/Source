#ifndef _DUMB_H_
#define _DUMB_H_

#include "stdafx.h"

class DumbShaderClass
{
public:
	DumbShaderClass();
	DumbShaderClass(const DumbShaderClass&);
	~DumbShaderClass();

	bool Initialize();

	void set_dumb_shader();

	bool SetShaderParameters(Matrix worldMatrix, Matrix viewMatrix,
		Matrix projectionMatrix, Color pixelColor);

private:

	struct MatrixBufferType
	{
		Matrix world;
		Matrix view;
		Matrix projection;
	};

	struct PixelBufferType
	{
		Color pixelColor;
	};

	ID3D11VertexShader* d_vertexShader;
	ID3D11PixelShader* d_pixelShader;
	ID3D11InputLayout* d_layout;

	ID3D11Buffer* d_matrixBuffer;
	ID3D11Buffer* d_pixelBuffer;
};

#endif