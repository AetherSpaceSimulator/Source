#include "stdafx.h"
#include "model.h"

modelClass::modelClass()
{
	vertexBuffer = nullptr;
	indexBuffer = nullptr;
	material = nullptr;
	loaded = false;
	has_transparencies = false;
}

modelClass::modelClass(const modelClass& other){}

modelClass::~modelClass()
{
	SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(vertexBuffer);
	for (UINT n = 0; n < texture_pool.size(); n++) SAFE_RELEASE(texture_pool[n]);
	SAFE_DELETE_ARRAY(material);
}

int modelClass::GetIndexCount()
{
	return indexCount;
}

int modelClass::GetVertexCount()
{
	return vertexCount;
}

std::wstring modelClass::string2widestring(const std::string& s)
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

bool modelClass::load_model(std::string modelFilename)
{
	HRESULT result;
	CUSTOMVERTEX* vertices = nullptr;
	unsigned long* indices = nullptr;

	std::ifstream fin(modelFilename);
	std::string texto = "";
	char input;

	// If it could not open the file then exit.
	if (fin.fail())
	{
		//m_swapChain->SetFullscreenState(false, NULL); // Make sure we are out of fullscreen

		std::wstring message = L"Could not open model file: ";
		message += string2widestring(modelFilename);

		MessageBox(0, message.c_str(),
			L"Error", MB_OK);
		return false;
	}

	if (fin.is_open())
	{
		// Read up to the value of vertex count.
		fin.get(input);
		while (input != ':')
		{
			fin.get(input);
		}

		fin >> vertexCount;
		////////////////////////////////////////////////////////////////////////////////

		fin.get(input);
		while (input != ':')
		{
			fin.get(input);
		}
		fin >> indexCount;
		////////////////////////////////////////////////////////////////////////////////

		fin.get(input);
		while (input != ':')
		{
			fin.get(input);
		}
		fin >> material_count;
		////////////////////////////////////////////////////////////////////////////////

		fin.get(input);
		while (input != ':')
		{
			fin.get(input);
		}
		fin >> texture_count;
		////////////////////////////////////////////////////////////////////////////////

		fin.get(input);
		while (input != ':')
		{
			fin.get(input);
		}
		fin >> scale;
		////////////////////////////////////////////////////////////////////////////////

		fin.get(input);
		while (input != ':')
		{
			fin.get(input);
		}
		fin >> has_transparencies;
		////////////////////////////////////////////////////////////////////////////////
		ID3D11ShaderResourceView* temp_pool = nullptr;
		for (int n = 0; n < texture_count; n++)
		{
			texture_pool.push_back(temp_pool);
		}

		for (int n = 0; n < texture_count; n++)	texture_pool[n] = nullptr;

		material = new material_struct[material_count];

		if (texture_count > 0)
		{
			std::string texture_pool_name;

			for (int n = 0; n < texture_count; n++)
			{
				fin.get(input);
				while (input != ':')
				{
					fin.get(input);
				}
				fin >> texture_pool_name;

				if (texture_pool_name == "Default") texture_pool_name = "textures/white.png";

				result = DirectX::CreateWICTextureFromFile(D3D_device, D3D_context, string2widestring(texture_pool_name).c_str(), NULL, &texture_pool[n], NULL);
				if (FAILED(result))
				{
					MessageBox(0, L"Could not load a model's texture.", string2widestring(texture_pool_name).c_str(), MB_OK);
					return false;
				}
			}
		}

		if (material_count > 0)
		{
			for (int n = 0; n < material_count; n++)
			{
				fin.get(input);
				while (input != ':')
				{
					fin.get(input);
				}
				fin >> material[n].group_name;
				////////////////////////////////////////////////////////////////////////////////

				fin.get(input);
				while (input != ':')
				{
					fin.get(input);
				}
				fin >> material[n].name;
				////////////////////////////////////////////////////////////////////////////////

				fin.get(input);
				while (input != ':')
				{
					fin.get(input);
				}
				fin >> material[n].start;
				////////////////////////////////////////////////////////////////////////////////

				fin.get(input);
				while (input != ':')
				{
					fin.get(input);
				}
				fin >> material[n].size;
				////////////////////////////////////////////////////////////////////////////////

				fin.get(input);
				while (input != ':')
				{
					fin.get(input);
				}
				fin >> material[n].diffuse_map;
				////////////////////////////////////////////////////////////////////////////////

				fin.get(input);
				while (input != ':')
				{
					fin.get(input);
				}
				fin >> material[n].light_map;
				if (material[n].light_map == 0) material[n].has_light_map = false;
				else material[n].has_light_map = true;
				////////////////////////////////////////////////////////////////////////////////

				fin.get(input);
				while (input != ':')
				{
					fin.get(input);
				}
				fin >> material[n].ilumination;
				////////////////////////////////////////////////////////////////////////////////

				fin.get(input);
				while (input != ':')
				{
					fin.get(input);
				}
				fin >> material[n].diffuse_color.x >> material[n].diffuse_color.y >> material[n].diffuse_color.z;
				////////////////////////////////////////////////////////////////////////////////

				fin.get(input);
				while (input != ':')
				{
					fin.get(input);
				}
				fin >> material[n].specular_power;
				////////////////////////////////////////////////////////////////////////////////

				fin.get(input);
				while (input != ':')
				{
					fin.get(input);
				}
				fin >> material[n].transparency;
				////////////////////////////////////////////////////////////////////////////////
			}
		}

		// Read up to the beginning of the data.
		fin.get(input);
		while (input != ':')
		{
			fin.get(input);
		}
		fin.get(input);
		fin.get(input);

		vertices = new CUSTOMVERTEX[vertexCount];

		if (!vertices)
		{
			return false;
		}

		// Read in the vertex data.
		for (int i = 0; i < vertexCount; i++)
		{
			fin >> vertices[i].position.x >> vertices[i].position.y >> vertices[i].position.z;
			fin >> vertices[i].texture.x >> vertices[i].texture.y;
			fin >> vertices[i].normal.x >> vertices[i].normal.y >> vertices[i].normal.z;
		}

		fin.get(input);
		while (input != ':')
		{
			fin.get(input);
		}
		fin.get(input);

		indices = new unsigned long[indexCount];

		for (int i = 0; i < indexCount; i++)
		{
			fin >> indices[i];
		}
	}

	// Close the model file.
	fin.close();

	// Initialize the buffers
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	// Set up the description of the STATIC vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(CUSTOMVERTEX)* vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = D3D_device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long)* indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = D3D_device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	SAFE_DELETE_ARRAY(indices);
	SAFE_DELETE_ARRAY(vertices);
	loaded = true;

	return true;
}

void modelClass::set_buffers()
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(CUSTOMVERTEX);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	D3D_context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	D3D_context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	return;
}

int modelClass::GetIndexSize(int n){ return material[n].size; }

bool modelClass::Get_has_light_map(int n){ return material[n].has_light_map; }

bool modelClass::Get_has_night_map(int n){ return material[n].has_night_map; }

int modelClass::GetIndexStart(int n){ return material[n].start; }

int modelClass::GetMaterialCount(){ return material_count; }

float modelClass::getIlumination(int n){ return material[n].ilumination; }
Vector3 modelClass::getDiffuseColor(int n){ return material[n].diffuse_color; }
float modelClass::getSpecularPower(int n){ return material[n].specular_power; }
float modelClass::getTransparency(int n){ return material[n].transparency; }

bool modelClass::getLoaded(){ return loaded; };

bool modelClass::create_rings(int sections, float innerRadius, float outerRadius, float beginAngle, float endAngle, std::string texture_name)
{
	HRESULT result;
	CUSTOMVERTEX* vertices;
	unsigned long* indices;
	sections *= 2;

	vertices = new CUSTOMVERTEX[(sections + 2) * 2];

	beginAngle = DirectX::XMConvertToRadians(beginAngle);
	endAngle = DirectX::XMConvertToRadians(endAngle);

	float angle = endAngle - beginAngle;

	for (int i = 0; i <= sections; i += 2) // create the up facing vertices
	{
		float t = (float)i / sections;
		float theta = beginAngle + t * angle;
		float s = (float)sin(theta);
		float c = (float)cos(theta);

		vertices[i] = { { c * innerRadius, 0.0f, -1.0f * (s * innerRadius) }, { 2.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } };
		vertices[i + 1] = { { c * outerRadius, 0.0f, -1.0f * (s * outerRadius) }, { 1.0f, 2.0f }, { 0.0f, 1.0f, 0.0f } };
	}

	for (int i = sections + 2; i <= sections * 2 + 2; i += 2) // create the down facing vertices
	{
		float t = (float)i / sections;
		float theta = beginAngle + t * angle;
		float s = (float)sin(theta);
		float c = (float)cos(theta);

		vertices[i] = { { c * innerRadius, 0.0f, -1.0f * (s * innerRadius) }, { 2.0f, 1.0f }, { 0.0f, -1.0f, 0.0f } };
		vertices[i + 1] = { { c * outerRadius, 0.0f, -1.0f * (s * outerRadius) }, { 1.0f, 2.0f }, { 0.0f, -1.0f, 0.0f } };
	}

	indices = new unsigned long[((sections + 2) * 3) * 2];

	int triangulos = 0; // start from the first vertex
	for (int n = 0; n < sections * 3; n += 6) // assign the up facing indices in a counter-clockwise order
	{
		indices[n] = triangulos;
		indices[n + 1] = triangulos + 1;
		indices[n + 2] = triangulos + 2;

		indices[n + 3] = triangulos + 2;
		indices[n + 4] = triangulos + 1;
		indices[n + 5] = triangulos + 3;
		triangulos += 2;
	}

	triangulos = sections + 2; // start indexing from the first of the down facing vertices
	for (int n = sections * 3; n < (sections * 3) * 2; n += 6) // create the triangles in a clockwise order
	{
		indices[n] = triangulos + 2;
		indices[n + 1] = triangulos + 1;
		indices[n + 2] = triangulos;

		indices[n + 3] = triangulos + 3;
		indices[n + 4] = triangulos + 1;
		indices[n + 5] = triangulos + 2;
		triangulos += 2;
	}



	// Read in the vertex count.
	vertexCount = (sections + 2) * 2;

	// Set the number of indices to be the same as the vertex count.
	indexCount = ((sections + 2) * 3) * 2;



	// Initialize the buffers
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	// Set up the description of the STATIC vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(CUSTOMVERTEX)* vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = D3D_device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long)* indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = D3D_device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	material_count = 1;
	texture_count = 1;
	scale = 1.0f;
	ID3D11ShaderResourceView* temp_pool = nullptr;
	for (int n = 0; n < texture_count; n++)
	{
		texture_pool.push_back(temp_pool);
	}
	material = new material_struct[material_count];

	result = DirectX::CreateWICTextureFromFile(D3D_device, D3D_context, string2widestring(texture_name).c_str(), NULL, &texture_pool[0], NULL);
	if (FAILED(result))
	{
		MessageBox(0, L"Could not load a model's texture.", string2widestring(texture_name).c_str(), MB_OK);
		return false;
	}

	material[0].group_name = "ring_material";
	material[0].name = "rings";
	material[0].start = 0;
	material[0].size = indexCount;
	material[0].diffuse_map = 0;
	material[0].light_map = 0;
	material[0].has_light_map = false;
	material[0].ilumination = 0;
	material[0].diffuse_color = Vector3{ 1.0f, 1.0f, 1.0f };
	material[0].specular_power = 0;
	material[0].transparency = 1.0f; // seteamos un poco de transparencia para que se active el render de transparencias en la textura

	SAFE_DELETE_ARRAY(indices);
	SAFE_DELETE_ARRAY(vertices);

	loaded = true;

	return true;
}

void modelClass::setTransparency(int material_, float ammount_)
{
	material[material_].transparency = ammount_;
}

bool modelClass::create_dynamic_sphere(std::string modelFilename, int main_iterations, int sub_iterations)
{
	HRESULT result;

	std::ifstream fin(modelFilename);
	std::string texto = "";
	char input;

	// If it could not open the file then exit.
	if (fin.fail())
	{
		//m_swapChain->SetFullscreenState(false, NULL); // Make sure we are out of fullscreen

		std::wstring message = L"Could not open model file: ";
		message += string2widestring(modelFilename);

		MessageBox(0, message.c_str(),
			L"Error", MB_OK);
		return false;
	}

	if (fin.is_open())
	{
		// Read up to the value of vertex count.
		fin.get(input);
		while (input != ':')
		{
			fin.get(input);
		}

		fin >> vertexCount;
		////////////////////////////////////////////////////////////////////////////////

		fin.get(input);
		while (input != ':')
		{
			fin.get(input);
		}
		fin >> indexCount;
		////////////////////////////////////////////////////////////////////////////////

		fin.get(input);
		while (input != ':')
		{
			fin.get(input);
		}
		fin >> material_count;
		////////////////////////////////////////////////////////////////////////////////

		fin.get(input);
		while (input != ':')
		{
			fin.get(input);
		}
		fin >> texture_count;
		////////////////////////////////////////////////////////////////////////////////

		fin.get(input);
		while (input != ':')
		{
			fin.get(input);
		}
		fin >> scale;
		////////////////////////////////////////////////////////////////////////////////

		fin.get(input);
		while (input != ':')
		{
			fin.get(input);
		}
		fin >> has_transparencies;
		////////////////////////////////////////////////////////////////////////////////
		ID3D11ShaderResourceView* temp_pool = nullptr;
		for (int n = 0; n < texture_count; n++)
		{
			texture_pool.push_back(temp_pool);
		}

		for (int n = 0; n < texture_count; n++)	texture_pool[n] = nullptr;

		material = new material_struct[material_count];

		if (texture_count > 0)
		{
			std::string texture_pool_name;

			for (int n = 0; n < texture_count; n++)
			{
				fin.get(input);
				while (input != ':')
				{
					fin.get(input);
				}
				fin >> texture_pool_name;

				if (texture_pool_name == "Default") texture_pool_name = "textures/white.png";

				result = DirectX::CreateWICTextureFromFile(D3D_device, D3D_context, string2widestring(texture_pool_name).c_str(), NULL, &texture_pool[n], NULL);
				if (FAILED(result))
				{
					MessageBox(0, L"Could not load a model's texture.", string2widestring(texture_pool_name).c_str(), MB_OK);
					return false;
				}
			}
		}

		if (material_count > 0)
		{
			for (int n = 0; n < material_count; n++)
			{
				fin.get(input);
				while (input != ':')
				{
					fin.get(input);
				}
				fin >> material[n].group_name;
				////////////////////////////////////////////////////////////////////////////////

				fin.get(input);
				while (input != ':')
				{
					fin.get(input);
				}
				fin >> material[n].name;
				////////////////////////////////////////////////////////////////////////////////

				fin.get(input);
				while (input != ':')
				{
					fin.get(input);
				}
				fin >> material[n].start;
				////////////////////////////////////////////////////////////////////////////////

				fin.get(input);
				while (input != ':')
				{
					fin.get(input);
				}
				fin >> material[n].size;
				////////////////////////////////////////////////////////////////////////////////

				fin.get(input);
				while (input != ':')
				{
					fin.get(input);
				}
				fin >> material[n].diffuse_map;
				////////////////////////////////////////////////////////////////////////////////

				fin.get(input);
				while (input != ':')
				{
					fin.get(input);
				}
				fin >> material[n].light_map;
				if (material[n].light_map == 0) material[n].has_light_map = false;
				else material[n].has_light_map = true;
				////////////////////////////////////////////////////////////////////////////////

				fin.get(input);
				while (input != ':')
				{
					fin.get(input);
				}
				fin >> material[n].ilumination;
				////////////////////////////////////////////////////////////////////////////////

				fin.get(input);
				while (input != ':')
				{
					fin.get(input);
				}
				fin >> material[n].diffuse_color.x >> material[n].diffuse_color.y >> material[n].diffuse_color.z;
				////////////////////////////////////////////////////////////////////////////////

				fin.get(input);
				while (input != ':')
				{
					fin.get(input);
				}
				fin >> material[n].specular_power;
				////////////////////////////////////////////////////////////////////////////////

				fin.get(input);
				while (input != ':')
				{
					fin.get(input);
				}
				fin >> material[n].transparency;
				////////////////////////////////////////////////////////////////////////////////
			}
		}
	}

	// Close the model file.
	fin.close();

	int total_facets = int(powf(4.0f, float(main_iterations)) * 8.0f);
	FACET* facet = new FACET[total_facets];

	Vector3 p[6] = { { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f }, { -1.0f, -1.0f, 0.0f },
	{ 1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { -1.0f, 1.0f, 0.0f } }; // the intial shape, a double pyramid

	/* Create the level 0 object */
	float a = 1 / sqrtf(2.0);
	for (int i = 0; i < 6; i++)
	{
		p[i].x *= a;
		p[i].y *= a;
	}

	facet[0].p1 = p[0]; facet[0].p2 = p[3]; facet[0].p3 = p[4];
	facet[1].p1 = p[0]; facet[1].p2 = p[4]; facet[1].p3 = p[5];
	facet[2].p1 = p[0]; facet[2].p2 = p[5]; facet[2].p3 = p[2];
	facet[3].p1 = p[0]; facet[3].p2 = p[2]; facet[3].p3 = p[3];
	facet[4].p1 = p[1]; facet[4].p2 = p[4]; facet[4].p3 = p[3];
	facet[5].p1 = p[1]; facet[5].p2 = p[5]; facet[5].p3 = p[4];
	facet[6].p1 = p[1]; facet[6].p2 = p[2]; facet[6].p3 = p[5];
	facet[7].p1 = p[1]; facet[7].p2 = p[3]; facet[7].p3 = p[2];
	int facet_number = 8;
	int former_facet_number = 0;
	Vector3 pa, pb, pc;

	// seccionar el modelo el numero de iteraciones principales
	if (main_iterations > 0)
	{
		/* Bisect each edge and move to the surface of a unit sphere */
		for (int it = 0; it < main_iterations; it++) {
			former_facet_number = facet_number;
			for (int i = 0; i < former_facet_number; i++)
			{
				pa.x = (facet[i].p1.x + facet[i].p2.x) / 2;
				pa.y = (facet[i].p1.y + facet[i].p2.y) / 2;
				pa.z = (facet[i].p1.z + facet[i].p2.z) / 2;
				pb.x = (facet[i].p2.x + facet[i].p3.x) / 2;
				pb.y = (facet[i].p2.y + facet[i].p3.y) / 2;
				pb.z = (facet[i].p2.z + facet[i].p3.z) / 2;
				pc.x = (facet[i].p3.x + facet[i].p1.x) / 2;
				pc.y = (facet[i].p3.y + facet[i].p1.y) / 2;
				pc.z = (facet[i].p3.z + facet[i].p1.z) / 2;

				pa.Normalize();
				pb.Normalize();
				pc.Normalize();

				facet[facet_number].p1 = facet[i].p1; facet[facet_number].p2 = pa; facet[facet_number].p3 = pc; facet_number++;
				facet[facet_number].p1 = pa; facet[facet_number].p2 = facet[i].p2; facet[facet_number].p3 = pb; facet_number++;
				facet[facet_number].p1 = pb; facet[facet_number].p2 = facet[i].p3; facet[facet_number].p3 = pc; facet_number++;

				facet[i].p1 = pa;
				facet[i].p2 = pb;
				facet[i].p3 = pc;
			}
		}
	}

	vertexCount = total_facets * 3;

	CUSTOMVERTEX* vertices = new CUSTOMVERTEX[vertexCount];

	if (!vertices) return false;

	for (int n = 0; n < total_facets; n++)
	{
		vertices[n * 3].position = Vector3{ facet[n].p1.x, facet[n].p1.y, facet[n].p1.z };
		vertices[n * 3].texture = Vector2{ 0.5f + atan2f(facet[n].p1.z, facet[n].p1.x) / (2 * DirectX::XM_PI), 0.5f - asinf(facet[n].p1.y) / DirectX::XM_PI };
		vertices[n * 3].normal = Vector3{ facet[n].p1.x, facet[n].p1.y, facet[n].p1.z };

		vertices[n * 3 + 1].position = Vector3{ facet[n].p2.x, facet[n].p2.y, facet[n].p2.z };
		vertices[n * 3 + 1].texture = Vector2{ 0.5f + atan2f(facet[n].p2.z, facet[n].p2.x) / (2 * DirectX::XM_PI), 0.5f - asinf(facet[n].p2.y) / DirectX::XM_PI };
		vertices[n * 3 + 1].normal = Vector3{ facet[n].p2.x, facet[n].p2.y, facet[n].p2.z };

		vertices[n * 3 + 2].position = Vector3{ facet[n].p3.x, facet[n].p3.y, facet[n].p3.z };
		vertices[n * 3 + 2].texture = Vector2{ 0.5f + atan2f(facet[n].p3.z, facet[n].p3.x) / (2 * DirectX::XM_PI), 0.5f - asinf(facet[n].p3.y) / DirectX::XM_PI };
		vertices[n * 3 + 2].normal = Vector3{ facet[n].p3.x, facet[n].p3.y, facet[n].p3.z };
	}

	// Initialize the buffers
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;

	// Set up the description of the DYNAMIC vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(CUSTOMVERTEX)* vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Create the vertex buffer.
	result = D3D_device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}
	
	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(facet);
	loaded = true;

	return true;
}

bool modelClass::update_dynamic_sphere_VertexBuffer(int vertexCount_, FACET* facet)
{
	vertexCount = vertexCount_;

	CUSTOMVERTEX* vertices = new CUSTOMVERTEX[vertexCount];

	if (!vertices) return false;

	for (int n = 0; n < vertexCount_ / 3; n++)
	{
		vertices[n * 3].position = Vector3{ facet[n].p1.x, facet[n].p1.y, facet[n].p1.z };
		vertices[n * 3].texture = Vector2{ 0.5f + atan2f(facet[n].p1.z, facet[n].p1.x) / (2 * DirectX::XM_PI), 0.5f - asinf(facet[n].p1.y) / DirectX::XM_PI };
		vertices[n * 3].normal = Vector3{ facet[n].p1.x, facet[n].p1.y, facet[n].p1.z };

		vertices[n * 3 + 1].position = Vector3{ facet[n].p2.x, facet[n].p2.y, facet[n].p2.z };
		vertices[n * 3 + 1].texture = Vector2{ 0.5f + atan2f(facet[n].p2.z, facet[n].p2.x) / (2 * DirectX::XM_PI), 0.5f - asinf(facet[n].p2.y) / DirectX::XM_PI };
		vertices[n * 3 + 1].normal = Vector3{ facet[n].p2.x, facet[n].p2.y, facet[n].p2.z };

		vertices[n * 3 + 2].position = Vector3{ facet[n].p3.x, facet[n].p3.y, facet[n].p3.z };
		vertices[n * 3 + 2].texture = Vector2{ 0.5f + atan2f(facet[n].p3.z, facet[n].p3.x) / (2 * DirectX::XM_PI), 0.5f - asinf(facet[n].p3.y) / DirectX::XM_PI };
		vertices[n * 3 + 2].normal = Vector3{ facet[n].p3.x, facet[n].p3.y, facet[n].p3.z };
	}

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	//	Disable GPU access to the vertex buffer data.
	D3D_context->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	//	Update the vertex buffer here.
	memcpy(mappedResource.pData, vertices, sizeof(CUSTOMVERTEX)* vertexCount_);
	//	Reenable GPU access to the vertex buffer data.
	D3D_context->Unmap(vertexBuffer, 0);

	SAFE_DELETE_ARRAY(vertices);

	return true;
}

bool modelClass::create_dynamic_landing_pad(std::string modelFilename)
{
	HRESULT result;

	std::ifstream fin(modelFilename);
	std::string texto = "";
	char input;

	// If it could not open the file then exit.
	if (fin.fail())
	{
		//m_swapChain->SetFullscreenState(false, NULL); // Make sure we are out of fullscreen

		std::wstring message = L"Could not open model file: ";
		message += string2widestring(modelFilename);

		MessageBox(0, message.c_str(),
			L"Error", MB_OK);
		return false;
	}

	if (fin.is_open())
	{
		// Read up to the value of vertex count.
		fin.get(input);
		while (input != ':')
		{
			fin.get(input);
		}

		fin >> vertexCount;
		////////////////////////////////////////////////////////////////////////////////

		fin.get(input);
		while (input != ':')
		{
			fin.get(input);
		}
		fin >> indexCount;
		////////////////////////////////////////////////////////////////////////////////

		fin.get(input);
		while (input != ':')
		{
			fin.get(input);
		}
		fin >> material_count;
		////////////////////////////////////////////////////////////////////////////////

		fin.get(input);
		while (input != ':')
		{
			fin.get(input);
		}
		fin >> texture_count;
		////////////////////////////////////////////////////////////////////////////////

		fin.get(input);
		while (input != ':')
		{
			fin.get(input);
		}
		fin >> scale;
		////////////////////////////////////////////////////////////////////////////////

		fin.get(input);
		while (input != ':')
		{
			fin.get(input);
		}
		fin >> has_transparencies;
		////////////////////////////////////////////////////////////////////////////////
		ID3D11ShaderResourceView* temp_pool = nullptr;
		for (int n = 0; n < texture_count; n++)
		{
			texture_pool.push_back(temp_pool);
		}

		for (int n = 0; n < texture_count; n++)	texture_pool[n] = nullptr;

		material = new material_struct[material_count];

		if (texture_count > 0)
		{
			std::string texture_pool_name;

			for (int n = 0; n < texture_count; n++)
			{
				fin.get(input);
				while (input != ':')
				{
					fin.get(input);
				}
				fin >> texture_pool_name;

				if (texture_pool_name == "Default") texture_pool_name = "textures/white.png";

				result = DirectX::CreateWICTextureFromFile(D3D_device, D3D_context, string2widestring(texture_pool_name).c_str(), NULL, &texture_pool[n], NULL);
				if (FAILED(result))
				{
					MessageBox(0, L"Could not load a model's texture.", string2widestring(texture_pool_name).c_str(), MB_OK);
					return false;
				}
			}
		}

		if (material_count > 0)
		{
			for (int n = 0; n < material_count; n++)
			{
				fin.get(input);
				while (input != ':')
				{
					fin.get(input);
				}
				fin >> material[n].group_name;
				////////////////////////////////////////////////////////////////////////////////

				fin.get(input);
				while (input != ':')
				{
					fin.get(input);
				}
				fin >> material[n].name;
				////////////////////////////////////////////////////////////////////////////////

				fin.get(input);
				while (input != ':')
				{
					fin.get(input);
				}
				fin >> material[n].start;
				////////////////////////////////////////////////////////////////////////////////

				fin.get(input);
				while (input != ':')
				{
					fin.get(input);
				}
				fin >> material[n].size;
				////////////////////////////////////////////////////////////////////////////////

				fin.get(input);
				while (input != ':')
				{
					fin.get(input);
				}
				fin >> material[n].diffuse_map;
				////////////////////////////////////////////////////////////////////////////////

				fin.get(input);
				while (input != ':')
				{
					fin.get(input);
				}
				fin >> material[n].light_map;
				if (material[n].light_map == 0) material[n].has_light_map = false;
				else material[n].has_light_map = true;
				////////////////////////////////////////////////////////////////////////////////

				fin.get(input);
				while (input != ':')
				{
					fin.get(input);
				}
				fin >> material[n].ilumination;
				////////////////////////////////////////////////////////////////////////////////

				fin.get(input);
				while (input != ':')
				{
					fin.get(input);
				}
				fin >> material[n].diffuse_color.x >> material[n].diffuse_color.y >> material[n].diffuse_color.z;
				////////////////////////////////////////////////////////////////////////////////

				fin.get(input);
				while (input != ':')
				{
					fin.get(input);
				}
				fin >> material[n].specular_power;
				////////////////////////////////////////////////////////////////////////////////

				fin.get(input);
				while (input != ':')
				{
					fin.get(input);
				}
				fin >> material[n].transparency;
				////////////////////////////////////////////////////////////////////////////////
			}
		}
	}

	// Close the model file.
	fin.close();

	int total_facets = 13;
	FACET facet[13];

	for (int n = 0; n < total_facets; n++) facet[n] = { { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } };

	vertexCount = total_facets * 3;

	CUSTOMVERTEX* vertices = new CUSTOMVERTEX[vertexCount];

	if (!vertices) return false;

	for (int n = 0; n < total_facets; n++)
	{
		vertices[n * 3].position = Vector3{ facet[n].p1.x, facet[n].p1.y, facet[n].p1.z };
		vertices[n * 3].texture = Vector2{ 0.5f + atan2f(facet[n].p1.z, facet[n].p1.x) / (2 * DirectX::XM_PI), 0.5f - asinf(facet[n].p1.y) / DirectX::XM_PI };
		vertices[n * 3].normal = Vector3{ facet[n].p1.x, facet[n].p1.y, facet[n].p1.z };

		vertices[n * 3 + 1].position = Vector3{ facet[n].p2.x, facet[n].p2.y, facet[n].p2.z };
		vertices[n * 3 + 1].texture = Vector2{ 0.5f + atan2f(facet[n].p2.z, facet[n].p2.x) / (2 * DirectX::XM_PI), 0.5f - asinf(facet[n].p2.y) / DirectX::XM_PI };
		vertices[n * 3 + 1].normal = Vector3{ facet[n].p2.x, facet[n].p2.y, facet[n].p2.z };

		vertices[n * 3 + 2].position = Vector3{ facet[n].p3.x, facet[n].p3.y, facet[n].p3.z };
		vertices[n * 3 + 2].texture = Vector2{ 0.5f + atan2f(facet[n].p3.z, facet[n].p3.x) / (2 * DirectX::XM_PI), 0.5f - asinf(facet[n].p3.y) / DirectX::XM_PI };
		vertices[n * 3 + 2].normal = Vector3{ facet[n].p3.x, facet[n].p3.y, facet[n].p3.z };
	}

	// Initialize the buffers
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;

	// Set up the description of the DYNAMIC vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(CUSTOMVERTEX)* vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Create the vertex buffer.
	result = D3D_device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	SAFE_DELETE_ARRAY(vertices);
	loaded = true;

	return true;
}

bool modelClass::update_dynamic_landing_pad_VertexBuffer(FACET* facet, std::vector<objectClass>& object)
{
	vertexCount = 39;

	CUSTOMVERTEX* vertices = new CUSTOMVERTEX[vertexCount];

	if (!vertices) return false;

	for (int n = 0; n < vertexCount / 3; n++)
	{
		vertices[n * 3].texture = Vector2{ 0.5f + atan2f(facet[n].p1.z, facet[n].p1.x) / (2 * DirectX::XM_PI), 0.5f - asinf(facet[n].p1.y) / DirectX::XM_PI };
		vertices[n * 3].normal = Vector3{ facet[n].p1.x, facet[n].p1.y, facet[n].p1.z };

		vertices[n * 3 + 1].texture = Vector2{ 0.5f + atan2f(facet[n].p2.z, facet[n].p2.x) / (2 * DirectX::XM_PI), 0.5f - asinf(facet[n].p2.y) / DirectX::XM_PI };
		vertices[n * 3 + 1].normal = Vector3{ facet[n].p2.x, facet[n].p2.y, facet[n].p2.z };

		vertices[n * 3 + 2].texture = Vector2{ 0.5f + atan2f(facet[n].p3.z, facet[n].p3.x) / (2 * DirectX::XM_PI), 0.5f - asinf(facet[n].p3.y) / DirectX::XM_PI };
		vertices[n * 3 + 2].normal = Vector3{ facet[n].p3.x, facet[n].p3.y, facet[n].p3.z };
	}

	double esc_ = 0.0;
	// uso de float cuando se cambia la distancia de la camara para eliminar el gap que aparece en el terreno
	if (Config.changed_scale || time_acceleration > 1) esc_ = object[object[control].getOrbiting()].getScale_b() / 2;
	else esc_ = (object[object[control].getOrbiting()].getScale()*universe_scale) / 2.0;
		
	Mat4d esc_mat;
	esc_mat.scaling({ esc_, esc_, esc_ });

	Mat4d rotar_mat = toMatrix4(object[object[control].getOrbiting()].getTotalRotation());

	vec3 tras_ = { 0.0, 0.0, 0.0 };
	// uso de float cuando se cambia la distancia de la camara para eliminar el gap que aparece en el terreno
	if (Config.changed_scale || time_acceleration > 1) tras_ = d3d_to_vec(object[object[control].getOrbiting()].getPosition_b());
	else tras_ = (object[object[control].getOrbiting()].getPosition() - object[control].getPosition())* universe_scale;
	
	Mat4d tras_mat;
	tras_mat.translation(tras_);

	Mat4d local_ = esc_mat*rotar_mat*tras_mat;

	FACET tf_b[13];

	for (int n = 0; n < 12; n++)
	{
		tf_b[n] = facet[n];
		tf_b[n].p1 = vec_to_d3d(RowVec3byMat4d(d3d_to_vec(tf_b[n].p1), local_) + tras_);
		tf_b[n].p2 = vec_to_d3d(RowVec3byMat4d(d3d_to_vec(tf_b[n].p2), local_) + tras_);
		tf_b[n].p3 = vec_to_d3d(RowVec3byMat4d(d3d_to_vec(tf_b[n].p3), local_) + tras_);
	}

	tf_b[12] = facet[12];
	tf_b[12].p1 = vec_to_d3d(RowVec3byMat4d(d3d_to_vec(tf_b[12].p1), local_) + tras_);
	tf_b[12].p2 = vec_to_d3d(RowVec3byMat4d(d3d_to_vec(tf_b[12].p2), local_) + tras_);
	tf_b[12].p3 = vec_to_d3d(RowVec3byMat4d(d3d_to_vec(tf_b[12].p3), local_) + tras_);


	for (int n = 0; n < vertexCount / 3; n++)
	{
		vertices[n * 3].position = Vector3{ tf_b[n].p1.x, tf_b[n].p1.y, tf_b[n].p1.z };
		vertices[n * 3].normal = vec_to_d3d(RowVec3byMat4d(d3d_to_vec(vertices[n * 3].normal), local_) - tras_);

		vertices[n * 3 + 1].position = Vector3{ tf_b[n].p2.x, tf_b[n].p2.y, tf_b[n].p2.z };
		vertices[n * 3 + 1].normal = vec_to_d3d(RowVec3byMat4d(d3d_to_vec(vertices[n * 3 + 1].normal), local_) - tras_);

		vertices[n * 3 + 2].position = Vector3{ tf_b[n].p3.x, tf_b[n].p3.y, tf_b[n].p3.z };
		vertices[n * 3 + 2].normal = vec_to_d3d(RowVec3byMat4d(d3d_to_vec(vertices[n * 3 + 2].normal), local_) - tras_);
	}


	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	//	Disable GPU access to the vertex buffer data.
	D3D_context->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	//	Update the vertex buffer here.
	memcpy(mappedResource.pData, vertices, sizeof(CUSTOMVERTEX)* vertexCount);
	//	Reenable GPU access to the vertex buffer data.
	D3D_context->Unmap(vertexBuffer, 0);

	SAFE_DELETE_ARRAY(vertices);

	return true;
}