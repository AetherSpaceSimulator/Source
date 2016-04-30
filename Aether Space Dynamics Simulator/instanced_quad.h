////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


//////////////
// INCLUDES //
//////////////
#include "stdafx.h"
#include "texture.h"
#include "object.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////////////////////////////////////////////
class InstancedModelClass
{
private:
	struct VertexType
	{
		Vector3 position;
		Vector2 texture;
		Color color;
	};



public:
	InstancedModelClass(std::vector<objectClass>& object_);
	~InstancedModelClass();

	bool Initialize(std::string filename, int p_type);

	bool update_shock_diamonds(float camera_in_atmosphere);

	void RenderBuffers();

	void setcamPos(Vector3 camPos_);

	int GetVertexCount();
	int GetInstanceCount();
	ID3D11ShaderResourceView* GetTexture();

	struct InstanceType
	{
		Vector3 position;
		Color color;
		float size;
		float original_size;
		float unmodified_size;
		Vector3 unmodified_position;
		bool alive;
		int tipo;
		float lifetime;
		float max_diamond_life_time;
		float max_smoke_life_time;
		float max_lifetime;
		float growth;
		float depth;
		float alpha;
		int motor_number;
		Vector3 velocity;
		Vector3 former_position;
		Vector3 new_vel;
		Vector3 new_pos;
		bool modified_vel;
		int object_number;
		Color color_start;
		Color color_end;
	};

	Vector3 camPos;

private:
	bool InitializeBuffers();

	bool LoadTexture(std::string filename);
	Vector3 getLocalAxis(Vector3 axis_, int objeto);

	bool sortBylugar(const InstanceType &lhs, const InstanceType &rhs);

	void SortMyContainerObjects(InstanceType* instances);

private:
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_instanceBuffer;
	InstanceType* instances;
	textureClass* m_Texture;

	std::vector<objectClass>& object;

	int m_instanceCount;

	int hover_engines_counter, x_positive_counter, x_negative_counter, y_positive_counter, y_negative_counter,
		z_positive_counter, z_negative_counter, up_counter, down_counter, right_counter, left_counter, forward_counter,
		backward_counter, total_active_engines;

	int m_vertexCount, m_max_vertexCount;

	bool at_least_one_particle_alive;
	int particle_type;	
};

#endif