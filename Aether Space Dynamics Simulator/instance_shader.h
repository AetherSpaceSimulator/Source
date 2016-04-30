////////////////////////////////////////////////////////////////////////////////
// Filename: textureshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _INSTANCESHADERCLASS_H_
#define _INSTANCESHADERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include "stdafx.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: TextureShaderClass
////////////////////////////////////////////////////////////////////////////////
class InstanceShaderClass
{
private:
	struct MatrixBufferType
	{
		Matrix rot_;
		Matrix view;
		Matrix projection;
	};

public:
	InstanceShaderClass();
	InstanceShaderClass(const InstanceShaderClass&);
	~InstanceShaderClass();

	bool Initialize(HWND);
	bool Render(int, int, Matrix rota_, Matrix, Matrix, ID3D11ShaderResourceView*);

private:
	bool InitializeShader(HWND, WCHAR*, WCHAR*);

	bool SetShaderParameters(Matrix, Matrix, Matrix, ID3D11ShaderResourceView*);
	void RenderShader(int, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;
};

#endif