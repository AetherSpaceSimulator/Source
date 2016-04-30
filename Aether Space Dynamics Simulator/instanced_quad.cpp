////////////////////////////////////////////////////////////////////////////////
// Filename: InstancedModelClass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "instanced_quad.h"


InstancedModelClass::InstancedModelClass(std::vector<objectClass>& object_) :object(object_)
{
	m_max_vertexCount = 50000;
	camPos = Vector3{ 0.f, 0.f, 0.f };
	particle_type = 0;

	at_least_one_particle_alive = false;

	m_vertexBuffer = nullptr;
	instances = nullptr;
	m_instanceBuffer = nullptr;
	m_Texture = nullptr;
}

InstancedModelClass::~InstancedModelClass()
{
	SAFE_DELETE(m_Texture);
	SAFE_DELETE_ARRAY(instances);
	SAFE_RELEASE(m_vertexBuffer);
	SAFE_RELEASE(m_instanceBuffer);
}

bool InstancedModelClass::sortBylugar(const InstanceType &lhs, const InstanceType &rhs)
{
	return lhs.depth > rhs.depth;
}

//*
bool asortBylugar(const InstancedModelClass::InstanceType &lhs, const InstancedModelClass::InstanceType &rhs)
{
	return lhs.depth > rhs.depth;
}
//*/

bool InstancedModelClass::Initialize(std::string filename, int p_type)
{
	bool result;

	particle_type = p_type;

	// Initialize the vertex and instance buffers.
	result = InitializeBuffers();
	if (!result)
	{
		return false;
	}

	// Load the texture for this model.
	result = LoadTexture(filename);
	if (!result)
	{
		return false;
	}

	return true;
}

int InstancedModelClass::GetVertexCount()
{
	return m_vertexCount;
}


int InstancedModelClass::GetInstanceCount()
{
	return m_instanceCount;
}


ID3D11ShaderResourceView* InstancedModelClass::GetTexture()
{
	return m_Texture->get_Texture();
}


bool InstancedModelClass::InitializeBuffers()
{
	VertexType* vertices;
	D3D11_BUFFER_DESC vertexBufferDesc, instanceBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, instanceData;
	HRESULT result;

	// Set the number of vertices in the vertex array.
	m_vertexCount = 6;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Load the vertex array with data.
	vertices[0].position = Vector3(-.5f, -.5f, 0.0f);  // Bottom left.
	vertices[0].texture = Vector2(0.0f, 1.0f);
	vertices[0].color = Color{ 1.f, 1.f, 1.f, 1.f };

	vertices[1].position = Vector3(.5f, .5f, 0.0f);  // Top right.
	vertices[1].texture = Vector2(1.0f, 0.0f);
	vertices[1].color = Color{ 1.f, 1.f, 1.f, 1.f };

	vertices[2].position = Vector3(.5f, -.5f, 0.0f);  // Bottom right.
	vertices[2].texture = Vector2(1.0f, 1.0f);
	vertices[2].color = Color{ 1.f, 1.f, 1.f, 1.f };

	vertices[3].position = Vector3(-.5f, -.5f, 0.0f);  // Bottom left.
	vertices[3].texture = Vector2(0.0f, 1.0f);
	vertices[3].color = Color{ 1.f, 1.f, 1.f, 1.f };

	vertices[4].position = Vector3(-.5f, .5f, 0.0f);  // Top left.
	vertices[4].texture = Vector2(0.0f, 0.0f);
	vertices[4].color = Color{ 1.f, 1.f, 1.f, 1.f };

	vertices[5].position = Vector3(.5f, .5f, 0.0f);  // Top right.
	vertices[5].texture = Vector2(1.0f, 0.0f);
	vertices[5].color = Color{ 1.f, 1.f, 1.f, 1.f };


	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType)* m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer. // aqui
	result = D3D_device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the vertex array now that the vertex buffer has been created and loaded.
	SAFE_DELETE_ARRAY(vertices);

	// Set the number of instances in the array.
	m_instanceCount = m_max_vertexCount;

	// Create the instance array.
	instances = new InstanceType[m_instanceCount];
	if (!instances)
	{
		return false;
	}

	for (int n = 0; n < m_instanceCount; n++)
	{
		instances[n].position = Vector3(0.f, 0.f, 0.0f);
		instances[n].velocity = Vector3(0.f, 0.f, 0.0f);
		instances[n].color = Color{ 1.f, 1.f, 1.f, 1.f };
		instances[n].alive = false;
		instances[n].tipo = 0;
		instances[n].max_diamond_life_time = 0.f;
		instances[n].unmodified_size = 0.f;
		instances[n].unmodified_position = Vector3(0.f, 0.f, 0.0f);
		instances[n].max_smoke_life_time = 0.f;
		instances[n].lifetime = 0.f;
		instances[n].max_lifetime = 0.f;
		instances[n].growth = 0.f;
		instances[n].size = 1.f;
		instances[n].motor_number = 0;
		instances[n].alpha = 1.f;
		instances[n].former_position = Vector3(0.f, 0.f, 0.0f);
		instances[n].new_pos = Vector3(0.f, 0.f, 0.0f);
		instances[n].new_vel = Vector3(0.f, 0.f, 0.0f);
		instances[n].modified_vel = false;
		instances[n].object_number = 0;
		instances[n].color_start = Color{ 1.f, 1.f, 1.f, 1.f };
		instances[n].color_end = Color{ 1.f, 1.f, 1.f, 1.f };
	}

	// Set up the description of the instance buffer.
	instanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	instanceBufferDesc.ByteWidth = sizeof(InstanceType)* m_instanceCount;
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the instance data.
	instanceData.pSysMem = instances;
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;

	// Create the instance buffer.
	result = D3D_device->CreateBuffer(&instanceBufferDesc, &instanceData, &m_instanceBuffer);
	if (FAILED(result))
	{
		return false;
	}

	m_instanceCount = 0;

	return true;
}

void InstancedModelClass::RenderBuffers()
{
	unsigned int strides[2];
	unsigned int offsets[2];
	ID3D11Buffer* bufferPointers[2];

	// Set the buffer strides.
	strides[0] = sizeof(VertexType);
	strides[1] = sizeof(InstanceType);

	// Set the buffer offsets.
	offsets[0] = 0;
	offsets[1] = 0;

	// Set the array of pointers to the vertex and instance buffers.
	bufferPointers[0] = m_vertexBuffer;
	bufferPointers[1] = m_instanceBuffer;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	D3D_context->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);

	return;
}


bool InstancedModelClass::LoadTexture(std::string filename)
{
	bool result;

	// Create the texture object.
	m_Texture = new textureClass;
	if (!m_Texture)
	{
		return false;
	}

	// Initialize the texture object.
	result = m_Texture->load_texture(filename);
	if (!result)
	{
		return false;
	}

	return true;
}

void InstancedModelClass::setcamPos(Vector3 camPos_)
{
	camPos = camPos_;
}

bool InstancedModelClass::update_shock_diamonds(float camera_in_atmosphere)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	InstanceType* InstancesPtr;
	float angle = 0.f;

	for (int tob_ = 0; tob_ < total_objects; tob_++)
	{
		if (object[tob_].getVisible() && object[tob_].getType() == 4 &&
			(object[tob_].getTotalEngines() > 0 ||
			object[tob_].getTotalHoverEngines() > 0 ||
			object[tob_].getTotalRCSNozzles() > 0))
		{
			float rcs_power = float(object[tob_].getRCSpower() / object[tob_].getMass()) * dt_b;

			int Total_engines = object[tob_].getTotalEngines();
			Vector3 Axys_Z = vec_to_d3d(GetAxisZ(object[tob_].getTotalRotation()));

			float MECurrentPower;
			if (object[tob_].getQuickMeBurst()) MECurrentPower = 100.f;
			else MECurrentPower = float(object[tob_].getMECurrentPower());

			int Total_hover_engines = object[tob_].getTotalHoverEngines();
			Vector3 Axys_Y = vec_to_d3d(-GetAxisY(object[tob_].getTotalRotation()));

			float HoverCurrentPower;
			if (object[tob_].getQuickHoverBurst()) HoverCurrentPower = 100.f;
			else HoverCurrentPower = float(object[tob_].getHoverCurrentPower());

			Vector3 Axys_X = vec_to_d3d(GetAxisX(object[tob_].getTotalRotation()));
			int totalForward = object[tob_].getTotalForwardNozzles();
			int totalBackward = object[tob_].getTotalBackwardNozzles();
			int totalUp = object[tob_].getTotalUpNozzles();
			int totalDown = object[tob_].getTotalDownNozzles();
			int totalRight = object[tob_].getTotalRightNozzles();
			int totalLeft = object[tob_].getTotalLeftNozzles();
			int totalXpositive = object[tob_].getTotalPositiveXNozzles();
			int totalYpositive = object[tob_].getTotalPositiveYNozzles();
			int totalZpositive = object[tob_].getTotalPositiveZNozzles();
			int totalXnegative = object[tob_].getTotalNegativeXNozzles();
			int totalYnegative = object[tob_].getTotalNegativeYNozzles();
			int totalZnegative = object[tob_].getTotalNegativeZNozzles();

			bool Forward_on = false;
			bool Backward_on = false;
			bool Up_on = false;
			bool Down_on = false;
			bool Right_on = false;
			bool Left_on = false;
			bool x_positive_on = false;
			bool y_positive_on = false;
			bool z_positive_on = false;
			bool x_negative_on = false;
			bool y_negative_on = false;
			bool z_negative_on = false;

			float Forward_power = 0.f;
			float Backward_power = 0.f;
			float Up_power = 0.f;
			float Down_power = 0.f;
			float Right_power = 0.f;
			float Left_power = 0.f;
			float x_positive_power = 0.f;
			float y_positive_power = 0.f;
			float z_positive_power = 0.f;
			float x_negative_power = 0.f;
			float y_negative_power = 0.f;
			float z_negative_power = 0.f;

			// si estamos usando el RCS entonces pasar los datos de la potencia de los nozzles activos
			if (!object[tob_].getAerodynamicControls())
			{
				Forward_on = object[tob_].get_Forward_on();
				Backward_on = object[tob_].get_Backward_on();
				Up_on = object[tob_].get_Up_on();
				Down_on = object[tob_].get_Down_on();
				Right_on = object[tob_].get_Right_on();
				Left_on = object[tob_].get_Left_on();
				x_positive_on = object[tob_].get_x_positive_rot_on();
				y_positive_on = object[tob_].get_y_positive_rot_on();
				z_positive_on = object[tob_].get_z_positive_rot_on();
				x_negative_on = object[tob_].get_x_negative_rot_on();
				y_negative_on = object[tob_].get_y_negative_rot_on();
				z_negative_on = object[tob_].get_z_negative_rot_on();

				Forward_power = min(100.f, object[tob_].get_Forward_power() * 100.f / rcs_power);
				Backward_power = min(100.f, fabs(object[tob_].get_Backward_power()) * 100.f / rcs_power);
				Up_power = min(100.f, object[tob_].get_Up_power() * 100.f / rcs_power);
				Down_power = min(100.f, fabs(object[tob_].get_Down_power()) * 100.f / rcs_power);
				Right_power = min(100.f, object[tob_].get_Right_power() * 100.f / rcs_power);
				Left_power = min(100.f, fabs(object[tob_].get_Left_power()) * 100.f / rcs_power);
				x_positive_power = min(100.f, object[tob_].get_x_positive_power() * 100.f / rcs_power);
				y_positive_power = min(100.f, object[tob_].get_y_positive_power() * 100.f / rcs_power);
				z_positive_power = min(100.f, object[tob_].get_z_positive_power() * 100.f / rcs_power);
				x_negative_power = min(100.f, fabs(object[tob_].get_x_negative_power()) * 100.f / rcs_power);
				y_negative_power = min(100.f, fabs(object[tob_].get_y_negative_power()) * 100.f / rcs_power);
				z_negative_power = min(100.f, fabs(object[tob_].get_z_negative_power()) * 100.f / rcs_power);
			}

			object[tob_].set_Forward_power(0.f);
			object[tob_].set_Backward_power(0.f);
			object[tob_].set_Up_power(0.f);
			object[tob_].set_Down_power(0.f);
			object[tob_].set_Right_power(0.f);
			object[tob_].set_Left_power(0.f);

			object[tob_].set_Forward_on(false);
			object[tob_].set_Backward_on(false);
			object[tob_].set_Up_on(false);
			object[tob_].set_Down_on(false);
			object[tob_].set_Right_on(false);
			object[tob_].set_Left_on(false);

			object[tob_].set_x_positive_power(0.f);
			object[tob_].set_y_positive_power(0.f);
			object[tob_].set_z_positive_power(0.f);

			object[tob_].set_x_negative_power(0.f);
			object[tob_].set_y_negative_power(0.f);
			object[tob_].set_z_negative_power(0.f);

			object[tob_].set_x_positive_rot_on(false);
			object[tob_].set_y_positive_rot_on(false);
			object[tob_].set_z_positive_rot_on(false);

			object[tob_].set_x_negative_rot_on(false);
			object[tob_].set_y_negative_rot_on(false);
			object[tob_].set_z_negative_rot_on(false);

			// Anulamos los calculos para los motores inactivos
			if (object[tob_].getMECurrentPower() == 0.f) Total_engines = 0;
			if (object[tob_].getHoverCurrentPower() == 0.f) Total_hover_engines = 0;

			if (!Forward_on || Forward_power < 5.f) totalForward = 0;
			if (!Backward_on || Backward_power < 5.f) totalBackward = 0;
			if (!Up_on || Up_power < 5.f) totalUp = 0;
			if (!Down_on || Down_power < 5.f) totalDown = 0;
			if (!Right_on || Right_power < 5.f) totalRight = 0;
			if (!Left_on || Left_power < 5.f) totalLeft = 0;

			if (!x_positive_on || x_positive_power < 5.f) totalXpositive = 0;
			if (!y_positive_on || y_positive_power < 5.f) totalYpositive = 0;
			if (!z_positive_on || z_positive_power < 5.f) totalZpositive = 0;

			if (!x_negative_on || x_negative_power < 5.f) totalXnegative = 0;
			if (!y_negative_on || y_negative_power < 5.f) totalYnegative = 0;
			if (!z_negative_on || z_negative_power < 5.f) totalZnegative = 0;

			hover_engines_counter = Total_engines + Total_hover_engines;
			x_positive_counter = hover_engines_counter + totalXpositive;
			x_negative_counter = x_positive_counter + totalXnegative;
			y_positive_counter = x_negative_counter + totalYpositive;
			y_negative_counter = y_positive_counter + totalYnegative;
			z_positive_counter = y_negative_counter + totalZpositive;
			z_negative_counter = z_positive_counter + totalZnegative;
			up_counter = z_negative_counter + totalUp;
			down_counter = up_counter + totalDown;
			right_counter = down_counter + totalRight;
			left_counter = right_counter + totalLeft;
			forward_counter = left_counter + totalForward;
			backward_counter = forward_counter + totalBackward;
			total_active_engines = backward_counter;

			// Creamos nuevas particulas
			for (int m = 0; m < total_active_engines; m++)
			{
				// si la cantidad de particulas existentes es menor al maximo permitido entonces crear una nueva particula
				if (m_instanceCount < m_max_vertexCount)
				{
					int new_particle_place_number = 0;

					bool found_place = false;

					// revisar si alguna de las particulas ya existentes esta marcada como muerta y rehusarla
					for (int n = 0; n < m_instanceCount; n++)
					{
						if (!instances[n].alive)
						{
							found_place = true;
							new_particle_place_number = n;
						}
					}

					// si no hay ninguna muerta entonces crear una nueva
					if (!found_place)
					{
						new_particle_place_number = m_instanceCount;
						m_instanceCount++;

					}

					// creamos las particulas de los motores principales
					if (m < Total_engines)
					{
						float LO = 0.295f;
						float HI = 0.3f;
						float r3 = (LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)))) * object[tob_].Engine[m].size;

						//*************************************************************************************//
						instances[new_particle_place_number].object_number = tob_;
						instances[new_particle_place_number].tipo = 0;
						instances[new_particle_place_number].alive = true;

						instances[new_particle_place_number].color = object[tob_].getEngineColorStart();
						instances[new_particle_place_number].color_start = object[tob_].getEngineColorStart();
						instances[new_particle_place_number].color_end = object[tob_].getEngineColorEnd();

						instances[new_particle_place_number].depth = 0.f;
						instances[new_particle_place_number].velocity = Axys_Z*(r3*MECurrentPower / 100.f);
						instances[new_particle_place_number].unmodified_position = Vector3{ 0.f, 0.f, 0.f };
						instances[new_particle_place_number].unmodified_size = object[tob_].Engine[m].size;
						instances[new_particle_place_number].lifetime = 0.f;
						instances[new_particle_place_number].motor_number = m;
						instances[new_particle_place_number].max_diamond_life_time = .5f;
						instances[new_particle_place_number].max_smoke_life_time = (2.5f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (7.5f - 2.5f)))*MECurrentPower) / 100.f;
						if (camera_in_atmosphere < 1.f) instances[new_particle_place_number].max_smoke_life_time *= 1.f - camera_in_atmosphere;
						else instances[new_particle_place_number].max_smoke_life_time = 0.f;
						instances[new_particle_place_number].max_lifetime = instances[new_particle_place_number].max_diamond_life_time + instances[new_particle_place_number].max_smoke_life_time;
					}

					// creamos las particulas de los motores hover
					else if (m < hover_engines_counter)
					{
						int engine_number = m - Total_engines;
						float LO = 0.295f;
						float HI = 0.3f;
						float r3 = (LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)))) * object[tob_].Hover_Engine[engine_number].size;

						instances[new_particle_place_number].object_number = tob_;
						instances[new_particle_place_number].tipo = 2;
						instances[new_particle_place_number].alive = true;
						
						instances[new_particle_place_number].color = object[tob_].getEngineColorStart();
						instances[new_particle_place_number].color_start = object[tob_].getEngineColorStart();
						instances[new_particle_place_number].color_end = object[tob_].getEngineColorEnd();

						instances[new_particle_place_number].depth = 0.f;
						instances[new_particle_place_number].velocity = Axys_Y*(r3*HoverCurrentPower / 100.f);
						instances[new_particle_place_number].unmodified_position = Vector3{ 0.f, 0.f, 0.f };
						instances[new_particle_place_number].unmodified_size = object[tob_].Hover_Engine[engine_number].size;
						instances[new_particle_place_number].lifetime = 0.f;
						instances[new_particle_place_number].motor_number = engine_number;
						instances[new_particle_place_number].max_diamond_life_time = .5f;
						instances[new_particle_place_number].max_smoke_life_time = (2.5f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (7.5f - 2.5f)))*HoverCurrentPower) / 100.f;
						if (camera_in_atmosphere < 1.f) instances[new_particle_place_number].max_smoke_life_time *= 1.f - camera_in_atmosphere;
						else instances[new_particle_place_number].max_smoke_life_time = 0.f;
						instances[new_particle_place_number].max_lifetime = instances[new_particle_place_number].max_diamond_life_time + instances[new_particle_place_number].max_smoke_life_time;
					}

					// Positive X
					else if (m < x_positive_counter)
					{
						int engine_number = m - hover_engines_counter;
						float eng_size = object[tob_].RCS_Nozzle[object[tob_].PositiveXnozzle[engine_number]].size;
						float size_ = eng_size * x_positive_power / 100.f;
						float LO = 0.725f;
						float HI = 0.75f;
						float r3 = (LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)))) * size_;

						instances[new_particle_place_number].object_number = tob_;
						instances[new_particle_place_number].tipo = 4;
						instances[new_particle_place_number].alive = true;
						instances[new_particle_place_number].color = Color{ 0.7f, 0.7f, 0.7f, 0.2f };
						instances[new_particle_place_number].depth = 0.f;

						instances[new_particle_place_number].velocity =
							getLocalAxis(object[tob_].RCS_Nozzle[object[tob_].PositiveXnozzle[engine_number]].orientation, tob_)*r3;

						instances[new_particle_place_number].unmodified_position = Vector3{ 0.f, 0.f, 0.f };
						instances[new_particle_place_number].unmodified_size = size_;
						instances[new_particle_place_number].growth = (0.25f * eng_size * x_positive_power) / 100.f;
						instances[new_particle_place_number].lifetime = 0.f;
						instances[new_particle_place_number].motor_number = object[tob_].PositiveXnozzle[engine_number];
						instances[new_particle_place_number].max_diamond_life_time = .5f;
						instances[new_particle_place_number].max_smoke_life_time = 0.f;
						instances[new_particle_place_number].max_lifetime = instances[new_particle_place_number].max_diamond_life_time;
					}

					// Negative X
					else if (m < x_negative_counter)
					{
						int engine_number = m - x_positive_counter;
						float eng_size = object[tob_].RCS_Nozzle[object[tob_].NegativeXnozzle[engine_number]].size;
						float size_ = eng_size * x_negative_power / 100.f;
						float LO = 0.725f;
						float HI = 0.75f;
						float r3 = (LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)))) * size_;

						instances[new_particle_place_number].object_number = tob_;
						instances[new_particle_place_number].tipo = 4;
						instances[new_particle_place_number].alive = true;
						instances[new_particle_place_number].color = Color{ 0.7f, 0.7f, 0.7f, 0.2f };
						instances[new_particle_place_number].depth = 0.f;

						instances[new_particle_place_number].velocity =
							getLocalAxis(object[tob_].RCS_Nozzle[object[tob_].NegativeXnozzle[engine_number]].orientation, tob_)*r3;

						instances[new_particle_place_number].unmodified_position = Vector3{ 0.f, 0.f, 0.f };
						instances[new_particle_place_number].unmodified_size = size_;
						instances[new_particle_place_number].growth = (0.25f * eng_size * x_negative_power) / 100.f;
						instances[new_particle_place_number].lifetime = 0.f;
						instances[new_particle_place_number].motor_number = object[tob_].NegativeXnozzle[engine_number];
						instances[new_particle_place_number].max_diamond_life_time = .5f;
						instances[new_particle_place_number].max_smoke_life_time = 0.f;
						instances[new_particle_place_number].max_lifetime = instances[new_particle_place_number].max_diamond_life_time;
					}

					// Positive Y
					else if (m < y_positive_counter)
					{
						int engine_number = m - x_negative_counter;
						float eng_size = object[tob_].RCS_Nozzle[object[tob_].PositiveYnozzle[engine_number]].size;
						float size_ = eng_size * y_positive_power / 100.f;
						float LO = 0.725f;
						float HI = 0.75f;
						float r3 = (LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)))) * size_;

						instances[new_particle_place_number].object_number = tob_;
						instances[new_particle_place_number].tipo = 4;
						instances[new_particle_place_number].alive = true;
						instances[new_particle_place_number].color = Color{ 0.7f, 0.7f, 0.7f, 0.2f };
						instances[new_particle_place_number].depth = 0.f;

						instances[new_particle_place_number].velocity =
							getLocalAxis(object[tob_].RCS_Nozzle[object[tob_].PositiveYnozzle[engine_number]].orientation, tob_)*r3;

						instances[new_particle_place_number].unmodified_position = Vector3{ 0.f, 0.f, 0.f };
						instances[new_particle_place_number].unmodified_size = size_;
						instances[new_particle_place_number].growth = (0.25f * eng_size * y_positive_power) / 100.f;
						instances[new_particle_place_number].lifetime = 0.f;
						instances[new_particle_place_number].motor_number = object[tob_].PositiveYnozzle[engine_number];
						instances[new_particle_place_number].max_diamond_life_time = .5f;
						instances[new_particle_place_number].max_smoke_life_time = 0.f;
						instances[new_particle_place_number].max_lifetime = instances[new_particle_place_number].max_diamond_life_time;
					}

					// Negative Y
					else if (m < y_negative_counter)
					{
						int engine_number = m - y_positive_counter;
						float eng_size = object[tob_].RCS_Nozzle[object[tob_].NegativeYnozzle[engine_number]].size;
						float size_ = eng_size * y_negative_power / 100.f;
						float LO = 0.725f;
						float HI = 0.75f;
						float r3 = (LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)))) * size_;

						instances[new_particle_place_number].object_number = tob_;
						instances[new_particle_place_number].tipo = 4;
						instances[new_particle_place_number].alive = true;
						instances[new_particle_place_number].color = Color{ 0.7f, 0.7f, 0.7f, 0.2f };
						instances[new_particle_place_number].depth = 0.f;

						instances[new_particle_place_number].velocity =
							getLocalAxis(object[tob_].RCS_Nozzle[object[tob_].NegativeYnozzle[engine_number]].orientation, tob_)*r3;

						instances[new_particle_place_number].unmodified_position = Vector3{ 0.f, 0.f, 0.f };
						instances[new_particle_place_number].unmodified_size = size_;
						instances[new_particle_place_number].growth = (0.25f * eng_size * y_negative_power) / 100.f;
						instances[new_particle_place_number].lifetime = 0.f;
						instances[new_particle_place_number].motor_number = object[tob_].NegativeYnozzle[engine_number];
						instances[new_particle_place_number].max_diamond_life_time = .5f;
						instances[new_particle_place_number].max_smoke_life_time = 0.f;
						instances[new_particle_place_number].max_lifetime = instances[new_particle_place_number].max_diamond_life_time;
					}

					// Positive Z
					else if (m < z_positive_counter)
					{
						int engine_number = m - y_negative_counter;

						float eng_size = object[tob_].RCS_Nozzle[object[tob_].PositiveZnozzle[engine_number]].size;
						float size_ = eng_size * z_positive_power / 100.f;
						float LO = 0.725f;
						float HI = 0.75f;
						float r3 = (LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)))) * size_;

						instances[new_particle_place_number].object_number = tob_;
						instances[new_particle_place_number].tipo = 4;
						instances[new_particle_place_number].alive = true;
						instances[new_particle_place_number].color = Color{ 0.7f, 0.7f, 0.7f, 0.2f };
						instances[new_particle_place_number].depth = 0.f;

						instances[new_particle_place_number].velocity =
							getLocalAxis(object[tob_].RCS_Nozzle[object[tob_].PositiveZnozzle[engine_number]].orientation, tob_)*r3;

						instances[new_particle_place_number].unmodified_position = Vector3{ 0.f, 0.f, 0.f };
						instances[new_particle_place_number].unmodified_size = size_;
						instances[new_particle_place_number].growth = (0.25f * eng_size * z_positive_power) / 100.f;
						instances[new_particle_place_number].lifetime = 0.f;
						instances[new_particle_place_number].motor_number = object[tob_].PositiveZnozzle[engine_number];
						instances[new_particle_place_number].max_diamond_life_time = .5f;
						instances[new_particle_place_number].max_smoke_life_time = 0.f;
						instances[new_particle_place_number].max_lifetime = instances[new_particle_place_number].max_diamond_life_time;
					}

					// Negative Z
					else if (m < z_negative_counter)
					{
						int engine_number = m - z_positive_counter;

						float eng_size = object[tob_].RCS_Nozzle[object[tob_].NegativeZnozzle[engine_number]].size;
						float size_ = eng_size * z_negative_power / 100.f;
						float LO = 0.725f;
						float HI = 0.75f;
						float r3 = (LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)))) * size_;

						instances[new_particle_place_number].object_number = tob_;
						instances[new_particle_place_number].tipo = 4;
						instances[new_particle_place_number].alive = true;
						instances[new_particle_place_number].color = Color{ 0.7f, 0.7f, 0.7f, 0.2f };
						instances[new_particle_place_number].depth = 0.f;

						instances[new_particle_place_number].velocity =
							getLocalAxis(object[tob_].RCS_Nozzle[object[tob_].NegativeZnozzle[engine_number]].orientation, tob_)*r3;

						instances[new_particle_place_number].unmodified_position = Vector3{ 0.f, 0.f, 0.f };
						instances[new_particle_place_number].unmodified_size = size_;
						instances[new_particle_place_number].growth = (0.25f * eng_size * z_negative_power) / 100.f;
						instances[new_particle_place_number].lifetime = 0.f;
						instances[new_particle_place_number].motor_number = object[tob_].NegativeZnozzle[engine_number];
						instances[new_particle_place_number].max_diamond_life_time = .5f;

						instances[new_particle_place_number].max_smoke_life_time = 0.f;
						instances[new_particle_place_number].max_lifetime = instances[new_particle_place_number].max_diamond_life_time;
					}

					// Up
					else if (m < up_counter)
					{
						int engine_number = m - z_negative_counter;

						float eng_size = object[tob_].RCS_Nozzle[object[tob_].UpNozzle[engine_number]].size;
						float size_ = eng_size * Up_power / 100.f;
						float LO = 0.725f;
						float HI = 0.75f;
						float r3 = (LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)))) * size_;

						instances[new_particle_place_number].object_number = tob_;
						instances[new_particle_place_number].tipo = 4;
						instances[new_particle_place_number].alive = true;
						instances[new_particle_place_number].color = Color{ 0.7f, 0.7f, 0.7f, 0.2f };
						instances[new_particle_place_number].depth = 0.f;

						instances[new_particle_place_number].velocity =
							getLocalAxis(object[tob_].RCS_Nozzle[object[tob_].UpNozzle[engine_number]].orientation, tob_)*r3;

						instances[new_particle_place_number].unmodified_position = Vector3{ 0.f, 0.f, 0.f };
						instances[new_particle_place_number].unmodified_size = size_;
						instances[new_particle_place_number].growth = (0.25f * eng_size * Up_power) / 100.f;
						instances[new_particle_place_number].lifetime = 0.f;
						instances[new_particle_place_number].motor_number = object[tob_].UpNozzle[engine_number];
						instances[new_particle_place_number].max_diamond_life_time = .5f;

						instances[new_particle_place_number].max_smoke_life_time = 0.f;
						instances[new_particle_place_number].max_lifetime = instances[new_particle_place_number].max_diamond_life_time;
					}

					// Down
					else if (m < down_counter)
					{
						int engine_number = m - up_counter;

						float eng_size = object[tob_].RCS_Nozzle[object[tob_].DownNozzle[engine_number]].size;
						float size_ = eng_size * Down_power / 100.f;
						float LO = 0.725f;
						float HI = 0.75f;
						float r3 = (LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)))) * size_;

						instances[new_particle_place_number].object_number = tob_;
						instances[new_particle_place_number].tipo = 4;
						instances[new_particle_place_number].alive = true;
						instances[new_particle_place_number].color = Color{ 0.7f, 0.7f, 0.7f, 0.2f };
						instances[new_particle_place_number].depth = 0.f;

						instances[new_particle_place_number].velocity =
							getLocalAxis(object[tob_].RCS_Nozzle[object[tob_].DownNozzle[engine_number]].orientation, tob_)*r3;

						instances[new_particle_place_number].unmodified_position = Vector3{ 0.f, 0.f, 0.f };
						instances[new_particle_place_number].unmodified_size = size_;
						instances[new_particle_place_number].growth = (0.25f * eng_size * Down_power) / 100.f;
						instances[new_particle_place_number].lifetime = 0.f;
						instances[new_particle_place_number].motor_number = object[tob_].DownNozzle[engine_number];
						instances[new_particle_place_number].max_diamond_life_time = .5f;

						instances[new_particle_place_number].max_smoke_life_time = 0.f;
						instances[new_particle_place_number].max_lifetime = instances[new_particle_place_number].max_diamond_life_time;
					}

					// Right
					else if (m < right_counter)
					{
						int engine_number = m - down_counter;

						float eng_size = object[tob_].RCS_Nozzle[object[tob_].RightNozzle[engine_number]].size;
						float size_ = eng_size * Right_power / 100.f;
						float LO = 0.725f;
						float HI = 0.75f;
						float r3 = (LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)))) * size_;

						instances[new_particle_place_number].object_number = tob_;
						instances[new_particle_place_number].tipo = 4;
						instances[new_particle_place_number].alive = true;
						instances[new_particle_place_number].color = Color{ 0.7f, 0.7f, 0.7f, 0.2f };
						instances[new_particle_place_number].depth = 0.f;

						instances[new_particle_place_number].velocity =
							getLocalAxis(object[tob_].RCS_Nozzle[object[tob_].RightNozzle[engine_number]].orientation, tob_)*r3;

						instances[new_particle_place_number].unmodified_position = Vector3{ 0.f, 0.f, 0.f };
						instances[new_particle_place_number].unmodified_size = size_;
						instances[new_particle_place_number].growth = (0.25f * eng_size * Right_power) / 100.f;
						instances[new_particle_place_number].lifetime = 0.f;
						instances[new_particle_place_number].motor_number = object[tob_].RightNozzle[engine_number];
						instances[new_particle_place_number].max_diamond_life_time = .5f;

						instances[new_particle_place_number].max_smoke_life_time = 0.f;
						instances[new_particle_place_number].max_lifetime = instances[new_particle_place_number].max_diamond_life_time;
					}

					// Left
					else if (m < left_counter)
					{
						int engine_number = m - right_counter;

						float eng_size = object[tob_].RCS_Nozzle[object[tob_].LeftNozzle[engine_number]].size;
						float size_ = eng_size * Left_power / 100.f;
						float LO = 0.725f;
						float HI = 0.75f;
						float r3 = (LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)))) * size_;

						instances[new_particle_place_number].object_number = tob_;
						instances[new_particle_place_number].tipo = 4;
						instances[new_particle_place_number].alive = true;
						instances[new_particle_place_number].color = Color{ 0.7f, 0.7f, 0.7f, 0.2f };
						instances[new_particle_place_number].depth = 0.f;

						instances[new_particle_place_number].velocity =
							getLocalAxis(object[tob_].RCS_Nozzle[object[tob_].LeftNozzle[engine_number]].orientation, tob_)*r3;

						instances[new_particle_place_number].unmodified_position = Vector3{ 0.f, 0.f, 0.f };
						instances[new_particle_place_number].unmodified_size = size_;
						instances[new_particle_place_number].growth = (0.25f * eng_size * Left_power) / 100.f;
						instances[new_particle_place_number].lifetime = 0.f;
						instances[new_particle_place_number].motor_number = object[tob_].LeftNozzle[engine_number];
						instances[new_particle_place_number].max_diamond_life_time = .5f;

						instances[new_particle_place_number].max_smoke_life_time = 0.f;
						instances[new_particle_place_number].max_lifetime = instances[new_particle_place_number].max_diamond_life_time;
					}

					// Forward
					else if (m < forward_counter)
					{
						int engine_number = m - left_counter;

						float eng_size = object[tob_].RCS_Nozzle[object[tob_].ForwardNozzle[engine_number]].size;
						float size_ = eng_size * Forward_power / 100.f;
						float LO = 0.725f;
						float HI = 0.75f;
						float r3 = (LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)))) * size_;

						instances[new_particle_place_number].object_number = tob_;
						instances[new_particle_place_number].tipo = 4;
						instances[new_particle_place_number].alive = true;
						instances[new_particle_place_number].color = Color{ 0.7f, 0.7f, 0.7f, 0.2f };
						instances[new_particle_place_number].depth = 0.f;

						instances[new_particle_place_number].velocity =
							getLocalAxis(object[tob_].RCS_Nozzle[object[tob_].ForwardNozzle[engine_number]].orientation, tob_)*r3;

						instances[new_particle_place_number].unmodified_position = Vector3{ 0.f, 0.f, 0.f };
						instances[new_particle_place_number].unmodified_size = size_;
						instances[new_particle_place_number].growth = (0.25f * eng_size * Forward_power) / 100.f;
						instances[new_particle_place_number].lifetime = 0.f;
						instances[new_particle_place_number].motor_number = object[tob_].ForwardNozzle[engine_number];
						instances[new_particle_place_number].max_diamond_life_time = .5f;

						instances[new_particle_place_number].max_smoke_life_time = 0.f;
						instances[new_particle_place_number].max_lifetime = instances[new_particle_place_number].max_diamond_life_time;
					}

					// Backward
					else if (m < backward_counter)
					{
						int engine_number = m - forward_counter;

						float eng_size = object[tob_].RCS_Nozzle[object[tob_].BackwardNozzle[engine_number]].size;
						float size_ = eng_size * Backward_power / 100.f;
						float LO = 0.725f;
						float HI = 0.75f;
						float r3 = (LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)))) * size_;

						instances[new_particle_place_number].object_number = tob_;
						instances[new_particle_place_number].tipo = 4;
						instances[new_particle_place_number].alive = true;
						instances[new_particle_place_number].color = Color{ 0.7f, 0.7f, 0.7f, 0.2f };
						instances[new_particle_place_number].depth = 0.f;

						instances[new_particle_place_number].velocity =
							getLocalAxis(object[tob_].RCS_Nozzle[object[tob_].BackwardNozzle[engine_number]].orientation, tob_)*r3;

						instances[new_particle_place_number].unmodified_position = Vector3{ 0.f, 0.f, 0.f };
						instances[new_particle_place_number].unmodified_size = size_;
						instances[new_particle_place_number].growth = (0.25f * eng_size * Backward_power) / 100.f;
						instances[new_particle_place_number].lifetime = 0.f;
						instances[new_particle_place_number].motor_number = object[tob_].BackwardNozzle[engine_number];
						instances[new_particle_place_number].max_diamond_life_time = .5f;

						instances[new_particle_place_number].max_smoke_life_time = 0.f;
						instances[new_particle_place_number].max_lifetime = instances[new_particle_place_number].max_diamond_life_time;
					}
				}
			}
		}
	}

	// reset the single particle life check
	at_least_one_particle_alive = false;

	// update particle properties
	for (int i = 0; i < m_instanceCount; i++)
	{
		if (instances[i].alive)
		{
			at_least_one_particle_alive = true;

			// Modificamos las particulas de diamantes de choque mientras su tiempo de vida no exceda el maximo establecido
			if (instances[i].lifetime <= instances[i].max_diamond_life_time)
			{
				if (instances[i].tipo != 4)
				{					
					instances[i].color = instances[i].color_start * (1.f - (instances[i].lifetime / instances[i].max_diamond_life_time)) +
						instances[i].color_end * ((instances[i].lifetime / instances[i].max_diamond_life_time));

					instances[i].color.A(1.f - (instances[i].lifetime / instances[i].max_diamond_life_time));

					instances[i].unmodified_size = instances[i].unmodified_size - 0.001f;
					instances[i].unmodified_position = instances[i].unmodified_position + instances[i].velocity;
					instances[i].lifetime += 0.025f;
				}

				// RCS nozzles
				else
				{
					instances[i].color.A(min(instances[i].color.A(), 1.f - (instances[i].lifetime / instances[i].max_diamond_life_time)));
					instances[i].unmodified_size = instances[i].unmodified_size + instances[i].growth;

					instances[i].unmodified_position = instances[i].unmodified_position + instances[i].velocity;
					instances[i].lifetime += 0.025f;
				}
			}

			// Cuando las particulas exceden su tiempo de vida como diamantes de choque las transformamos en particulas de humo.
			// Aqui se modifican las particulas de humo mientras estas no excedan su tiempo de vida como tales
			else if (instances[i].lifetime <= instances[i].max_smoke_life_time)
			{
				// si la particula todavia no se transformo en humo entonces la transformamos
				if (instances[i].tipo == 0 || instances[i].tipo == 2)
				{
					// definimos aleatoriamente la velocidad y el crecimiento de cada particula de humo
					float vel_ = (1.5f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2.f - 1.5f))))*0.015f;

					// definimos sus parametros iniciales para que se comporte como humo
					instances[i].color = Color{ .4f, .4f, .4f, .2f };
					instances[i].depth = 0.f;
					instances[i].growth = .0025f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (.005f - 0.0025f)));

					// definimos la direccion del humo y su tipo a ser usado para definir su posicion mas abajo
					if (instances[i].tipo == 0)
					{

						instances[i].velocity = vec_to_d3d(GetAxisZ(object[instances[i].object_number].getTotalRotation()))*
							(vel_ * float(object[instances[i].object_number].getMECurrentPower()) / 100.f);

						instances[i].tipo = 1; // tipo 1 es humo de motor principal
					}

					else
					{

						instances[i].velocity = vec_to_d3d(-GetAxisY(object[instances[i].object_number].getTotalRotation()))*
							(vel_ * float(object[instances[i].object_number].getHoverCurrentPower()) / 100.f);
						instances[i].tipo = 3; // tipo 3 es humo de motor hover
					}
				}

				// definimos su trasparencia de acuerdo a su tiempo de vida
				if (instances[i].lifetime > instances[i].max_lifetime / 2) instances[i].alpha = min(instances[i].alpha, 1.f - (instances[i].lifetime / instances[i].max_lifetime));
				else instances[i].alpha = min(instances[i].alpha, instances[i].lifetime / instances[i].max_lifetime);

				// modificamos su transparencia de acuerdo a la altura en la atmosfera
				// *** DEBUG *** tiene que ser de acuerdo a la altura del vehiculo y no de la camara
				if (camera_in_atmosphere < 1.f) instances[i].color.A(min(instances[i].alpha, (1.f - camera_in_atmosphere) / 3.f));
				else instances[i].color.A(instances[i].alpha);

				// aplicamos el crecimiento establecido para cada particula
				instances[i].unmodified_size = instances[i].unmodified_size + instances[i].growth;


				// movemos la particula de acuerdo a su velocidad establecida
				instances[i].unmodified_position = instances[i].unmodified_position + instances[i].velocity;

				// usamos el dt y no 0.0025 para dar un poco de variacion al humo, no estoy seguro que sea una muy buena idea
				instances[i].lifetime += dt_b;
			}

			// si las particulas ya excedieron su tiempo de vida como particulas de humo entonces las marcamos como muertas para ser
			// convertidas en nuevas particulas
			else
			{
				instances[i].alive = false;
				instances[i].modified_vel = false;
				instances[i].color.A(0.f);
			}

			instances[i].size = instances[i].unmodified_size * object[instances[i].object_number].getScale_b();

			instances[i].former_position = instances[i].position;
			Vector3 Wpos_ = { 0.f, 0.f, 0.f };

			Matrix& World_ = object[instances[i].object_number].getWorld();
			Vector3 pos_b = object[instances[i].object_number].getPosition_b();

			if (instances[i].tipo == 0 || instances[i].tipo == 1)
			{
				Wpos_ = vec_to_d3d(RowVec3byMat4d(d3d_to_vec(object[instances[i].object_number].Engine[instances[i].motor_number].position),
					D3DXMATRIX_to_Mat4d(World_))) + pos_b;
			}

			else if (instances[i].tipo == 2 || instances[i].tipo == 3)
			{
				//MessageBoxA(m_hwnd, std::to_string(instances[i].object_number).c_str(), "Hello", MB_OK);

				Wpos_ = vec_to_d3d(RowVec3byMat4d(d3d_to_vec(object[instances[i].object_number].Hover_Engine[instances[i].motor_number].position),
					D3DXMATRIX_to_Mat4d(World_))) + pos_b;
			}

			else if (instances[i].tipo == 4)
			{
				//MessageBoxA(m_hwnd, std::to_string(instances[i].object_number).c_str(), "Hello", MB_OK);

				Wpos_ = vec_to_d3d(RowVec3byMat4d(
					d3d_to_vec(object[instances[i].object_number].RCS_Nozzle[instances[i].motor_number].position),
					D3DXMATRIX_to_Mat4d(World_))) + pos_b;
			}

			instances[i].position = Wpos_ + instances[i].unmodified_position * object[instances[i].object_number].getScale_b();

			// modificar la posicion y velocidad si la particula es humo y traspasa la tierra
			if ((instances[i].tipo == 1 || instances[i].tipo == 3))
			{
				if (!instances[i].modified_vel)
				{
					float orbited_scale_div2 = object[object[instances[i].object_number].getOrbiting()].getScale_b() / 2.f;
					Vector3 orbited_pos = object[object[instances[i].object_number].getOrbiting()].getPosition_b();

					Vector3 Smoke_vec = instances[i].position - object[object[instances[i].object_number].getOrbiting()].getPosition_b();
					if (length_f(Smoke_vec) < orbited_scale_div2)
					{
						Vector3 engine_vec = Wpos_ - orbited_pos;
						float dist_ = (length_f(engine_vec) - orbited_scale_div2);

						instances[i].new_pos = normalized_f(orbited_pos) * dist_ + Wpos_;

						float x_ = (.1f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (.4f - .1f)))) - .2f;
						float y_ = (.1f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (.4f - .1f)))) - .2f;
						float z_ = (.1f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (.4f - .1f)))) - .2f;


						instances[i].new_vel = Vector3{ x_, y_, z_ }*universe_scale;
						instances[i].modified_vel = true;
					}
				}

				else
				{
					instances[i].new_pos += instances[i].new_vel;
					instances[i].position = instances[i].new_pos;
				}
			}

			angle = angle_from_two_vectors_f(camPos - instances[i].position, camPos);
			instances[i].depth = cosf(angle)*distances_f(instances[i].position, camPos);
		}

		// if the particle is dead remove it from the frustum so no ghost will haunt the view
		else instances[i].depth = -1.f;
	}

	if (at_least_one_particle_alive)
	{
		// sort each element according to its depth in the camera frustum
		std::sort(instances, instances + m_instanceCount, asortBylugar);
		/*
		std::sort(instances, instances + m_instanceCount,
		std::bind(&InstancedModelClass::sortBylugar, this,
		std::placeholders::_1, std::placeholders::_2));
		//*/

		// Lock the instance buffer.
		result = D3D_context->Map(m_instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
		{
			return false;
		}
		// Get a pointer to the data in the vertex buffer.
		InstancesPtr = (InstanceType*)mappedResource.pData;

		// Copy the data into the vertex buffer.
		memcpy(InstancesPtr, (void*)instances, (sizeof(InstanceType)* m_instanceCount));

		// Unlock the vertex buffer.
		D3D_context->Unmap(m_instanceBuffer, 0);
	}

	// if no particle is alive reset the particle number to zero to avoid useless checks
	else m_instanceCount = 0;

	return true;
}

Vector3 InstancedModelClass::getLocalAxis(Vector3 axis_, int objeto)
{
	if (axis_ == Vector3{ 1, 0, 0 }) return vec_to_d3d(GetAxisX(object[objeto].getTotalRotation()));
	else if (axis_ == Vector3{ -1, 0, 0 }) return vec_to_d3d(-GetAxisX(object[objeto].getTotalRotation()));

	else if (axis_ == Vector3{ 0, 1, 0 }) return vec_to_d3d(GetAxisY(object[objeto].getTotalRotation()));
	else if (axis_ == Vector3{ 0, -1, 0 }) return vec_to_d3d(-GetAxisY(object[objeto].getTotalRotation()));

	else if (axis_ == Vector3{ 0, 0, 1 }) return vec_to_d3d(GetAxisZ(object[objeto].getTotalRotation()));
	else if (axis_ == Vector3{ 0, 0, -1 }) return vec_to_d3d(-GetAxisZ(object[objeto].getTotalRotation()));

	else return Vector3{ 0, 0, 0 };
}