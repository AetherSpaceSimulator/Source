#include "stdafx.h"
#include "atmosphere_shader.h"

Atmosphere_shader_class::Atmosphere_shader_class()
{
	atmos_vertexShader = nullptr;
	atmos_pixelShader = nullptr;
	atmos_layout = nullptr;
	atmos_sampleState = nullptr;
	atmos_matrixBuffer = nullptr;
	atmos_Buffer = nullptr;
}

Atmosphere_shader_class::Atmosphere_shader_class(const Atmosphere_shader_class& other)
{
}

Atmosphere_shader_class::~Atmosphere_shader_class()
{
	SAFE_RELEASE(atmos_vertexShader);
	SAFE_RELEASE(atmos_pixelShader);
	SAFE_RELEASE(atmos_layout);
	SAFE_RELEASE(atmos_sampleState);
	SAFE_RELEASE(atmos_Buffer);
}

bool Atmosphere_shader_class::Initialize(HWND hwnd)
{
	WCHAR* vsFilename = L"shaders/Atmosphere shader/atmosphere_vs.cso";
	WCHAR* psFilename = L"shaders/Atmosphere shader/atmosphere_ps.cso";

	HRESULT result;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int numElements;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;


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

		result = D3D_device->CreateVertexShader(vs_data, vs_size, 0, &atmos_vertexShader);
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

		result = D3D_device->CreatePixelShader(vs_datab, vs_sizeb, 0, &atmos_pixelShader);
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

	polygonLayout[1].SemanticName = "TEXTURE_COORDINATES";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = D3D_device->CreateInputLayout(polygonLayout, numElements, vs_data, vs_size,
		&atmos_layout);
	if (FAILED(result))
	{
		return false;
	}

	SAFE_DELETE_ARRAY(vs_data);
	SAFE_DELETE_ARRAY(vs_datab);

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
	result = D3D_device->CreateSamplerState(&samplerDesc, &atmos_sampleState);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(atmos_MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = D3D_device->CreateBuffer(&matrixBufferDesc, NULL, &atmos_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(atmos_BufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = D3D_device->CreateBuffer(&lightBufferDesc, NULL, &atmos_Buffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void Atmosphere_shader_class::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	std::ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = unsigned long(errorMessage->GetBufferSize());

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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//										Update the variable buffers passed to the Light shader														//
bool Atmosphere_shader_class::Set_Atmosphere_shader_parameters(Matrix worldMatrix, Matrix viewMatrix,
	Matrix projectionMatrix, Vector3 light_pos_, float atmosphere_scale_, Vector3 cameraPosition_, float transparency_,
	Vector3 planet_position_, float specular_power_,
	ID3D11ShaderResourceView** texture_array, float planet_scale_, float object_, float type, float camera_in_atmosphere_,
	Vector3 sunset_color_, float sun_distance_, Vector3 daylight_color_, float atmosphere_thickness_)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	atmos_MatrixBufferType* dataPtr;
	atmos_BufferType* dataPtr2;


	// Transpose the matrices to prepare them for the shader.
	worldMatrix = worldMatrix.Transpose();
	viewMatrix = viewMatrix.Transpose();
	projectionMatrix = projectionMatrix.Transpose();

	// Lock the constant buffer so it can be written to.
	result = D3D_context->Map(atmos_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (atmos_MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// Unlock the constant buffer.
	D3D_context->Unmap(atmos_matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Now set the constant buffer in the vertex shader with the updated values.
	D3D_context->VSSetConstantBuffers(bufferNumber, 1, &atmos_matrixBuffer);

	// Set shader texture resource in the pixel shader.
	D3D_context->PSSetShaderResources(0, 2, texture_array);

	// Lock the light constant buffer so it can be written to.

	result = D3D_context->Map(atmos_Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr2 = (atmos_BufferType*)mappedResource.pData;

	// Copy the lighting variables into the constant buffer.
	dataPtr2->planet_scale = planet_scale_;
	dataPtr2->object = object_;
	dataPtr2->type = type;
	dataPtr2->camera_in_atmosphere = camera_in_atmosphere_;
	dataPtr2->light_pos = light_pos_;
	dataPtr2->atmosphere_scale = atmosphere_scale_;
	dataPtr2->cameraPosition = cameraPosition_;
	dataPtr2->transparency = transparency_;
	dataPtr2->planet_position = planet_position_;
	dataPtr2->specular_power = specular_power_;
	dataPtr2->sunset_color = sunset_color_;
	dataPtr2->sun_distance = sun_distance_;
	dataPtr2->daylight_color = daylight_color_;
	dataPtr2->atmosphere_thickness = atmosphere_thickness_;

	// Unlock the constant buffer.
	D3D_context->Unmap(atmos_Buffer, 0);

	// Set the position of the light constant buffer in the pixel shader.
	bufferNumber = 0;

	// Finally set the light constant buffer in the pixel shader with the updated values.
	D3D_context->PSSetConstantBuffers(bufferNumber, 1, &atmos_Buffer);

	return true;
}
//																																					//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Atmosphere_shader_class::set_atmosphere_shader()
{
	// Set the vertex input layout.
	D3D_context->IASetInputLayout(atmos_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	D3D_context->VSSetShader(atmos_vertexShader, NULL, 0);
	D3D_context->PSSetShader(atmos_pixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	D3D_context->PSSetSamplers(0, 1, &atmos_sampleState);

	return true;
}