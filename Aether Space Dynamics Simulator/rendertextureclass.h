////////////////////////////////////////////////////////////////////////////////
// Filename: rendertextureclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _RENDERTEXTURECLASS_H_
#define _RENDERTEXTURECLASS_H_

#include "stdafx.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: RenderTextureClass
////////////////////////////////////////////////////////////////////////////////
class RenderTextureClass
{
public:
	RenderTextureClass();
	RenderTextureClass(const RenderTextureClass&);
	~RenderTextureClass();

	bool Initialize(int, int, float, float);

	void SetRenderTarget();
	void ClearRenderTarget(float, float, float, float);
	void ClearDepthBuffer();
	ID3D11ShaderResourceView* GetShaderResourceView();
	void GetProjectionMatrix(Matrix&);
	void GetOrthoMatrix(Matrix&);
	void set_matrices(int textureWidth, int textureHeight, float screenDepth, float screenNear);

private:
	ID3D11Texture2D* m_renderTargetTexture;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11ShaderResourceView* m_shaderResourceView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilView* m_depthStencilView;
	D3D11_VIEWPORT m_viewport;
	Matrix m_projectionMatrix;
	Matrix m_orthoMatrix;

};

#endif