////////////////////////////////////////////////////////////////////////////////
// Filename: bitmapclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _BITMAPCLASS_H_
#define _BITMAPCLASS_H_


//////////////
// INCLUDES //
//////////////
#include "stdafx.h"


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "texture.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: BitmapClass
////////////////////////////////////////////////////////////////////////////////
class BitmapClass
{
private:

	struct VertexType
	{
		Vector3 position;
		Vector2 texture;
	};

public:
	BitmapClass();
	//BitmapClass(const BitmapClass&);
	~BitmapClass();

	void setMouseStatus(int status_){ mouse_status = status_; };

	bool Initialize(int screenWidth, int screenHeight,
		std::string filename1, std::string filename2, std::string filename3, std::string filename4, int bitmapWidth, int bitmapHeight);

	bool Initialize_from_resource_view(int screenWidth, int screenHeight,
		ID3D11ShaderResourceView* shader_resource_view, int bitmapWidth, int bitmapHeight);

	bool LoadTexture_from_resource_view(ID3D11ShaderResourceView* shader_resource_view);
	bool Load_dynamic_Texture_from_resource_view(ID3D11ShaderResourceView* shader_resource_view);

	bool Render(int positionX, int positionY);
	int getBitmapWidth(){ return m_bitmapWidth; };
	int getBitmapHeigth(){ return m_bitmapHeight; };
	int getMouseStatus(){ return mouse_status; };

	int GetIndexCount();
	ID3D11ShaderResourceView** GetTexture();

private:
	bool InitializeBuffers();
	bool UpdateBuffers(int, int);
	void RenderBuffers();

	bool LoadTexture(std::string filename1, std::string filename2, std::string filename3, std::string filename4);

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	int mouse_status;
	textureClass* m_Texture;
	ID3D11ShaderResourceView* retorno[4];

	int m_screenWidth, m_screenHeight;
	int m_bitmapWidth, m_bitmapHeight;
	int m_previousPosX, m_previousPosY;
};

#endif