#include "stdafx.h"
#include "DepthShaderClass_tr.h"

DepthShaderClass_tr::DepthShaderClass_tr()
{
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_layout = nullptr;
	m_matrixBuffer = nullptr;
	m_sampleState = nullptr;
	tr_Buffer = nullptr;
}


DepthShaderClass_tr::DepthShaderClass_tr(const DepthShaderClass_tr& other)
{
}


DepthShaderClass_tr::~DepthShaderClass_tr()
{
	SAFE_RELEASE(tr_Buffer);
	SAFE_RELEASE(m_sampleState);
	SAFE_RELEASE(m_matrixBuffer);
	SAFE_RELEASE(m_layout);
	SAFE_RELEASE(m_pixelShader);
	SAFE_RELEASE(m_vertexShader);
}


bool DepthShaderClass_tr::Initialize(HWND hwnd)
{
	bool result;


	// Initialize the vertex and pixel shaders.
	result = InitializeShader(hwnd, L"shaders/Depth transparent shader/depth_tr_vs.cso", L"shaders/Depth transparent shader/depth_tr_ps.cso");
	if (!result)
	{
		return false;
	}

	return true;
}

bool DepthShaderClass_tr::InitializeShader(HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC tr_desc;

	std::ifstream shader_stream;
	size_t vs_size;
	char* vs_data;

	shader_stream.open(vsFilename, std::ifstream::in | std::ifstream::binary);
	if (shader_stream.good())
	{
		shader_stream.seekg(0, std::ios::end);
		vs_size = size_t(shader_stream.tellg());
		vs_data = new char[vs_size];
		shader_stream.seekg(0, std::ios::beg);
		shader_stream.read(&vs_data[0], vs_size);
		shader_stream.close();

		result = D3D_device->CreateVertexShader(vs_data, vs_size, 0, &m_vertexShader);
		if (FAILED(result))
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	size_t vs_sizeb;
	char* vs_datab;

	shader_stream.open(psFilename, std::ifstream::in | std::ifstream::binary);
	if (shader_stream.good())
	{
		shader_stream.seekg(0, std::ios::end);
		vs_sizeb = size_t(shader_stream.tellg());
		vs_datab = new char[vs_sizeb];
		shader_stream.seekg(0, std::ios::beg);
		shader_stream.read(&vs_datab[0], vs_sizeb);
		shader_stream.close();

		result = D3D_device->CreatePixelShader(vs_datab, vs_sizeb, 0, &m_pixelShader);
		if (FAILED(result))
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXTURE";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = D3D_device->CreateInputLayout(polygonLayout, numElements, vs_data, vs_size, &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	SAFE_DELETE_ARRAY(vs_data);
	SAFE_DELETE_ARRAY(vs_datab);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = D3D_device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = D3D_device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
	{
		return false;
	}


	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	tr_desc.Usage = D3D11_USAGE_DYNAMIC;
	tr_desc.ByteWidth = sizeof(b_BufferType);
	tr_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tr_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tr_desc.MiscFlags = 0;
	tr_desc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the pixel shader constant buffer from within this class.
	result = D3D_device->CreateBuffer(&tr_desc, NULL, &tr_Buffer);
	if (FAILED(result))
	{
		return false;
	}


	return true;
}

void DepthShaderClass_tr::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	size_t bufferSize, i;
	std::ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for (i = 0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}


bool DepthShaderClass_tr::SetShaderParameters(Matrix worldMatrix, Matrix viewMatrix, Matrix projectionMatrix,
	ID3D11ShaderResourceView* texture, float alpha_b)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	MatrixBufferType* dataPtr;
	b_BufferType* dataPtr2;


	// Transpose the matrices to prepare them for the shader.
	worldMatrix = worldMatrix.Transpose();
	viewMatrix = viewMatrix.Transpose();
	projectionMatrix = projectionMatrix.Transpose();

	// Lock the constant buffer so it can be written to.
	result = D3D_context->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// Unlock the constant buffer.
	D3D_context->Unmap(m_matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Now set the constant buffer in the vertex shader with the updated values.
	D3D_context->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// Set shader texture resource in the pixel shader.
	D3D_context->PSSetShaderResources(0, 1, &texture);


	// Lock the basic constant buffer so it can be written to.
	result = D3D_context->Map(tr_Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr2 = (b_BufferType*)mappedResource.pData;

	// Copy the lighting variables into the constant buffer.
	dataPtr2->alpha_ = alpha_b;
	dataPtr2->extra1 = 0.0f;
	dataPtr2->extra2 = 0.0f;
	dataPtr2->extra3 = 0.0f;

	// Unlock the constant buffer.
	D3D_context->Unmap(tr_Buffer, 0);

	// Set the position of the light constant buffer in the pixel shader.
	bufferNumber = 0;

	// Finally set the light constant buffer in the pixel shader with the updated values.
	D3D_context->PSSetConstantBuffers(bufferNumber, 1, &tr_Buffer);

	return true;
}


void DepthShaderClass_tr::set_transparent_depth_shader(int indexCount)
{
	// Set the vertex input layout.
	D3D_context->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	D3D_context->VSSetShader(m_vertexShader, NULL, 0);
	D3D_context->PSSetShader(m_pixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	D3D_context->PSSetSamplers(0, 1, &m_sampleState);

	return;
}