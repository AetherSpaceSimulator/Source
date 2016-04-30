#include "stdafx.h"
#include "texture.h"

textureClass::textureClass()
{
	loaded = false;
	release_texture = true;

	shader_resource_view = nullptr;
}

textureClass::textureClass(const textureClass& other){}

textureClass::~textureClass()
{
	if (release_texture) SAFE_RELEASE(shader_resource_view);
}

std::wstring textureClass::string2widestring(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	SAFE_DELETE_ARRAY(buf);
	return r;
}

bool textureClass::load_texture(std::string filename)
{
	HRESULT hresult;

	hresult = DirectX::CreateWICTextureFromFile(D3D_device, D3D_context, string2widestring(filename).c_str(), NULL, &shader_resource_view, NULL);

	if (FAILED(hresult))
	{
		MessageBox(0, L"Could not load a texture.", string2widestring(filename).c_str(), MB_OK);
		return false;
	}

	loaded = true;

	return true;
}

bool textureClass::load_texture_from_resource(ID3D11ShaderResourceView* resource_)
{
	shader_resource_view = resource_;
	release_texture = false; // this texture release is managed by another class

	loaded = true;

	return true;
}

ID3D11ShaderResourceView* textureClass::get_Texture()
{
	return shader_resource_view;
}

bool textureClass::getLoaded()
{
	return loaded;
}