////////////////////////////////////////////////////////////////////////////////
// Filename: fontclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _FONTCLASS_H_
#define _FONTCLASS_H_

#include "stdafx.h"
#include "texture.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: FontClass
////////////////////////////////////////////////////////////////////////////////
class FontClass
{
private:

	struct FontType
	{
		float left, right;
		int size;
	};

	struct VertexType
	{
		Vector3 position;
		Vector2 texture;
	};

public:
	FontClass();
	FontClass(const FontClass&);
	~FontClass();

	bool Initialize(char* fontFilename, std::string textureFilename);

	int get_last_pixel_position();

	ID3D11ShaderResourceView* GetTexture();

	void BuildVertexArray(void*, std::string, float, float);

private:
	bool LoadFontData(char*);
	bool LoadTexture(std::string filename);

private:
	float drawX;
	int last_X;
	FontType* m_Font;
	textureClass* m_Texture;
};

#endif