////////////////////////////////////////////////////////////////////////////////
// Filename: textureshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TEXTURESHADERCLASS_H_
#define _TEXTURESHADERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include "stdafx.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: TextureShaderClass
////////////////////////////////////////////////////////////////////////////////
class TextureShaderClass
{
private:
	struct MatrixBufferType
	{
		Matrix world;
		Matrix view;
		Matrix projection;
	};

	struct Data_
	{
		Color color;
		Vector3 lightPos;
		float extra1;
		float type_;
		float mouse_status;
		float dimmed;
		float extra2;
	};

public:
	TextureShaderClass();
	TextureShaderClass(const TextureShaderClass&);
	~TextureShaderClass();

	bool Initialize(HWND);
	void Shutdown();
	bool Render(int indexCount, Matrix worldMatrix, Matrix viewMatrix,
		Matrix projectionMatrix, ID3D11ShaderResourceView** texture_array, int tipo_, int mouse_status_, Color color_, Vector3 lightPos, float dimmed_);

private:
	bool InitializeShader(HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(Matrix worldMatrix, Matrix viewMatrix,
		Matrix projectionMatrix, ID3D11ShaderResourceView** texture_array, int tipo_, int mouse_status_, Color color_, Vector3 lightPos, float dimmed_);
	void RenderShader(int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* texture_Buffer;
};

#endif