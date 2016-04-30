////////////////////////////////////////////////////////////////////////////////
// Filename: light.vs
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix lightViewMatrix;
	matrix lightProjectionMatrix;
};

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXTURE_COORDINATES;
	float3 normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXTURE_COORDINATES0;
	float3 normal : NORMAL;
	float3 positionW : TEXTURE_COORDINATES1;
	float4 lightViewPosition : TEXTURE_COORDINATES2;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType BasicVertexShader(VertexInputType input)
{
	float4 worldPosition;

    PixelInputType output;    

	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

	//if (extra.x==0) output.position.w = 0;
    
	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;
    
	// Calculate the normal vector against the world matrix only.
    output.normal = mul(input.normal, worldMatrix);
    output.normal = normalize(output.normal);

	output.positionW = mul(input.position, worldMatrix);
	worldPosition = mul(input.position, worldMatrix);

	// Calculate the position of the vertice as viewed by the light source.
    output.lightViewPosition = mul(input.position, worldMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, lightViewMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, lightProjectionMatrix);

    return output;
}