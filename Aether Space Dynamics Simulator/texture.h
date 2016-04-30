#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "stdafx.h"

class textureClass
{
public:
	textureClass();
	textureClass(const textureClass&);
	~textureClass();

	bool getLoaded();

	bool load_texture(std::string filename);
	bool load_texture_from_resource(ID3D11ShaderResourceView* resource_);
	ID3D11ShaderResourceView* get_Texture();

private:
	std::wstring string2widestring(const std::string& s);
	ID3D11ShaderResourceView* shader_resource_view;
	bool loaded;

	bool release_texture;

};

#endif