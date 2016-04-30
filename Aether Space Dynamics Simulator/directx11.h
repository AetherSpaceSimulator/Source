#ifndef _DIRECTX11_H_
#define _DIRECTX11_H_
#include "stdafx.h"


class directx11
{
public:
	directx11();
	directx11(const directx11&);
	~directx11();

	bool Initialize(bool full_screen_resizeable, int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear);

	void ClearScreen();
	void PresentScene();
	void setFOV(float fov, float near_, float far_);

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
	ID3D11DepthStencilView* GetDepthStencilView();

	Matrix& GetProjectionMatrix();
	Matrix& GetWorldMatrix();
	Matrix& GetOrthoMatrix();
	void setWindowed();
	void enable_transparencies();
	void enable_to_texture_transparencies();
	void disable_transparencies();
	void TurnZBufferOn();
	void TurnZBufferOff();

	void setWorldMatrix(Matrix matrix_);
	void setNearFar(float near_, float far_);
	void set_inversed_render(bool true_);
	void setWireFrame(bool setw);

	void SetBackBufferRenderTarget();
	void ResetViewport();

	void GetVideoCardInfo(char*, int&);
	IDXGISwapChain* getSwapCHain();

private:
	float blendFactor[4], fieldOfView, screenAspect;
	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];
	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilState* m_depthStencilDisabledState;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RasterizerState* m_rasterState;
	ID3D11RasterizerState* m_rasterState_inv_render;
	ID3D11RasterizerState* m_rasterState_wireframe;
	Matrix m_projectionMatrix;
	Matrix m_worldMatrix;
	Matrix m_orthoMatrix;
	ID3D11BlendState* texture_with_alpha_channel;
	ID3D11BlendState* to_texture_with_alpha_channel;
	D3D11_VIEWPORT viewport;
	bool enable_aa;
	int aa_count;
};


#endif

