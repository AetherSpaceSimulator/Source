////////////////////////////////////////////////////////////////////////////////
// Filename: depthshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _DEPTHSHADERCLASS_TR_H_
#define _DEPTHSHADERCLASS_TR_H_

#include "stdafx.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: DepthShaderClass
////////////////////////////////////////////////////////////////////////////////
class DepthShaderClass_tr
{
private:
	struct MatrixBufferType
	{
		Matrix world;
		Matrix view;
		Matrix projection;
	};

	struct b_BufferType
	{
		float alpha_;
		float extra1;
		float extra2;
		float extra3;
	};

public:
	DepthShaderClass_tr();
	DepthShaderClass_tr(const DepthShaderClass_tr&);
	~DepthShaderClass_tr();

	bool Initialize(HWND);

	void set_transparent_depth_shader(int);
	bool SetShaderParameters(Matrix worldMatrix, Matrix viewMatrix, Matrix projectionMatrix,
		ID3D11ShaderResourceView* texture, float alpha_);


private:
	bool InitializeShader(HWND, WCHAR*, WCHAR*);
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* tr_Buffer;

	ID3D11SamplerState* m_sampleState;
};

#endif