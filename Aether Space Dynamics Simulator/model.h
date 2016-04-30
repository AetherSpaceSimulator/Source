#ifndef _INSTANCEDMODEL_H_
#define _INSTANCEDMODEL_H_

#include "stdafx.h"
#include "object.h"

class modelClass
{
public:
	modelClass();
	modelClass(const modelClass&);
	~modelClass();

	bool load_model(std::string modelFilename);
	bool create_rings(int sections, float innerRadius, float outerRadius, float beginAngle, float endAngle, std::string texture_name);
	void set_buffers();
	ID3D11ShaderResourceView* GetTexture(int n);
	int GetIndexSize(int n);
	int GetIndexStart(int n);
	int GetMaterialCount();
	int GetIndexCount();
	int GetVertexCount();
	int Get_diffuse_map(int n){ return material[n].diffuse_map; };
	int Get_light_map(int n){ return material[n].light_map; };
	bool get_HasTransparencies(){ return has_transparencies; };
	std::string GetMaterialName(int n){ return material[n].group_name; };
	ID3D11ShaderResourceView *Get_texture_pool(int n){ return texture_pool[n]; };
	float getScale() { return scale; };
	float getIlumination(int n);
	bool Get_has_light_map(int n);
	bool Get_has_night_map(int n);
	Vector3 getDiffuseColor(int n);
	float getSpecularPower(int n);
	float getTransparency(int n);
	void setTransparency(int material_, float ammount_);
	void setHasTransparencies(bool ht_){ has_transparencies = ht_; };
	bool getLoaded();
	std::wstring string2widestring(const std::string& s);
	bool create_dynamic_sphere(std::string modelFilename, int main_iterations, int sub_iterations);
	bool update_dynamic_sphere_VertexBuffer(int vertexCount_, FACET* facet);

	bool create_dynamic_landing_pad(std::string modelFilename);
	bool update_dynamic_landing_pad_VertexBuffer(FACET* facet, std::vector<objectClass>& object);

private:

	struct CUSTOMVERTEX
	{
		Vector3 position;
		Vector2 texture;
		Vector3 normal;
	};

	struct material_struct
	{
		material_struct()
		{
			has_light_map = false;
			has_night_map = false;
		};
		int start, size, diffuse_map, light_map;
		Vector3 diffuse_color;
		float ilumination, specular_power, transparency;
		std::string group_name, name, filename;
		bool has_light_map, has_night_map;
	};

private:
	ID3D11Buffer *vertexBuffer, *indexBuffer;
	int vertexCount, indexCount, material_count, texture_count;
	float scale;
	bool loaded, has_transparencies;
	material_struct* material;
	std::vector <ID3D11ShaderResourceView*> texture_pool;
};

#endif