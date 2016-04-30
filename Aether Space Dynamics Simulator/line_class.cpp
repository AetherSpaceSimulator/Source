#include "stdafx.h"
#include "line_class.h"

LineClass::LineClass()
{
	vertexBuffer = nullptr;
}


LineClass::LineClass(const LineClass& other)
{
}


LineClass::~LineClass()
{
	SAFE_RELEASE(vertexBuffer);
}

bool LineClass::create_points_buffer(Vector3* buffer_, int buffer_size)
{
	HRESULT result;
	//MessageBoxA(m_hwnd, std::to_string(buffer_size).c_str(), "Buffer size", MB_OK);
	Vector3* vertices = new Vector3[buffer_size];

	for (int n = 0; n < buffer_size; n++) vertices[n] = buffer_[n];

	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;

	// Set up the description of the STATIC vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vector3)* buffer_size;
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

	SAFE_DELETE_ARRAY(vertices);

	return true;
}

void LineClass::set_buffers()
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(Vector3);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	D3D_context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	return;
}