#include "stdafx.h"
#include "bitmap.h"

BitmapClass::BitmapClass()
{
	mouse_status = 0;

	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;
	m_Texture = nullptr;
}

//BitmapClass::BitmapClass(const BitmapClass& other){}


BitmapClass::~BitmapClass()
{
	SAFE_RELEASE(m_indexBuffer);
	SAFE_RELEASE(m_vertexBuffer);
	SAFE_DELETE_ARRAY(m_Texture);
}


bool BitmapClass::Initialize(int screenWidth, int screenHeight,
	std::string textureFilename1, std::string textureFilename2, std::string textureFilename3,
	std::string textureFilename4, int bitmapWidth, int bitmapHeight)
{
	// Store the screen size.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// Store the size in pixels that this bitmap should be rendered at.
	m_bitmapWidth = bitmapWidth;
	m_bitmapHeight = bitmapHeight;

	// Initialize the previous rendering position to negative one.
	m_previousPosX = -1;
	m_previousPosY = -1;

	// Initialize the vertex and index buffers.
	if (!InitializeBuffers()) return false;

	// Load the texture for this model.
	if (!LoadTexture(textureFilename1, textureFilename2, textureFilename3, textureFilename4)) return false;

	return true;
}

bool BitmapClass::Initialize_from_resource_view(int screenWidth, int screenHeight,
	ID3D11ShaderResourceView* shader_resource_view, int bitmapWidth, int bitmapHeight)
{
	// Store the screen size.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// Store the size in pixels that this bitmap should be rendered at.
	m_bitmapWidth = bitmapWidth;
	m_bitmapHeight = bitmapHeight;

	// Initialize the previous rendering position to negative one.
	m_previousPosX = -1;
	m_previousPosY = -1;

	// Initialize the vertex and index buffers.
	if (!InitializeBuffers()) return false;

	// Load the texture for this model.
	if (!LoadTexture_from_resource_view(shader_resource_view)) return false;

	return true;
}

bool BitmapClass::Render(int positionX, int positionY)
{
	bool result;


	// Re-build the dynamic vertex buffer for rendering to possibly a different location on the screen.
	result = UpdateBuffers(positionX, positionY);
	if (!result)
	{
		return false;
	}

	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers();

	return true;
}

int BitmapClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView** BitmapClass::GetTexture()
{
	retorno[0] = m_Texture[0].get_Texture();
	retorno[1] = m_Texture[1].get_Texture();
	retorno[2] = m_Texture[2].get_Texture();
	retorno[3] = m_Texture[3].get_Texture();

	return retorno;
}

bool BitmapClass::InitializeBuffers()
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	// Set the number of vertices in the vertex array.
	m_vertexCount = 6;

	// Set the number of indices in the index array.
	m_indexCount = m_vertexCount;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType)* m_vertexCount));

	// Load the index array with data.
	for (i = 0; i<m_indexCount; i++)
	{
		indices[i] = i;
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType)* m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = D3D_device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long)* m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = D3D_device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);

	return true;
}

bool BitmapClass::UpdateBuffers(int positionX, int positionY)
{
	HRESULT result;

	float left, right, top, bottom;
	VertexType* vertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;

	// If the position we are rendering this bitmap to has not changed then don't update the vertex buffer since it
	// currently has the correct parameters.
	if ((positionX == m_previousPosX) && (positionY == m_previousPosY))
	{
		return true;
	}

	// If it has changed then update the position it is being rendered to.
	m_previousPosX = positionX;
	m_previousPosY = positionY;

	// Calculate the screen coordinates of the left side of the bitmap.
	left = (float)((m_screenWidth / 2) * -1) + (float)positionX;

	// Calculate the screen coordinates of the right side of the bitmap.
	right = left + (float)m_bitmapWidth;

	// Calculate the screen coordinates of the top of the bitmap.
	top = (float)(m_screenHeight / 2) - (float)positionY;

	// Calculate the screen coordinates of the bottom of the bitmap.
	bottom = top - (float)m_bitmapHeight;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Load the vertex array with data.
	// First triangle.
	vertices[0].position = Vector3(left, top, 0.0f);  // Top left.
	vertices[0].texture = Vector2(0.0f, 0.0f);

	vertices[1].position = Vector3(right, bottom, 0.0f);  // Bottom right.
	vertices[1].texture = Vector2(1.0f, 1.0f);

	vertices[2].position = Vector3(left, bottom, 0.0f);  // Bottom left.
	vertices[2].texture = Vector2(0.0f, 1.0f);

	// Second triangle.
	vertices[3].position = Vector3(left, top, 0.0f);  // Top left.
	vertices[3].texture = Vector2(0.0f, 0.0f);

	vertices[4].position = Vector3(right, top, 0.0f);  // Top right.
	vertices[4].texture = Vector2(1.0f, 0.0f);

	vertices[5].position = Vector3(right, bottom, 0.0f);  // Bottom right.
	vertices[5].texture = Vector2(1.0f, 1.0f);

	// Lock the vertex buffer so it can be written to.
	result = D3D_context->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType)* m_vertexCount));

	// Unlock the vertex buffer.
	D3D_context->Unmap(m_vertexBuffer, 0);

	// Release the vertex array as it is no longer needed.
	SAFE_DELETE_ARRAY(vertices);

	return true;
}

void BitmapClass::RenderBuffers()
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	D3D_context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	D3D_context->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	D3D_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool BitmapClass::LoadTexture(std::string filename1, std::string filename2, std::string filename3, std::string filename4)
{
	// Create the texture object.
	m_Texture = new textureClass[4];
	if (!m_Texture) return false;

	// Initialize the texture object.
	if (!m_Texture[0].load_texture(filename1)) return false;
	if (!m_Texture[1].load_texture(filename2)) return false;
	if (!m_Texture[2].load_texture(filename3)) return false;
	if (!m_Texture[3].load_texture(filename4)) return false;

	return true;
}

bool BitmapClass::LoadTexture_from_resource_view(ID3D11ShaderResourceView* shader_resource_view)
{
	// Create the texture object.
	m_Texture = new textureClass[4];
	if (!m_Texture) return false;

	// Initialize the texture object.
	if (!m_Texture[0].load_texture_from_resource(shader_resource_view)) return false;
	//if (!m_Texture[1].load_texture_from_resource(m_device, m_dev_context, shader_resource_view)) return false;
	//if (!m_Texture[2].load_texture_from_resource(m_device, m_dev_context, shader_resource_view)) return false;
	//if (!m_Texture[3].load_texture_from_resource(m_device, m_dev_context, shader_resource_view)) return false;

	return true;
}

bool BitmapClass::Load_dynamic_Texture_from_resource_view(ID3D11ShaderResourceView* shader_resource_view)
{
	// Initialize the texture object.
	if (!m_Texture[0].load_texture_from_resource(shader_resource_view)) return false;
	//if (!m_Texture[1].load_texture_from_resource(m_device, m_dev_context, shader_resource_view)) return false;
	//if (!m_Texture[2].load_texture_from_resource(m_device, m_dev_context, shader_resource_view)) return false;
	//if (!m_Texture[3].load_texture_from_resource(m_device, m_dev_context, shader_resource_view)) return false;

	return true;
}


