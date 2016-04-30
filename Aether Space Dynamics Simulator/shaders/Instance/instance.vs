////////////////////////////////////////////////////////////////////////////////
// Filename: texture.vs
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
	matrix rotMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};


//////////////
// TYPEDEFS //
//////////////

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;

    float3 instancePosition : TEXCOORD1;
	float4 color : COLOR;
	float4 instanceColor : COLOR1;
	float instanceSize: DEPTH;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;

	float4 color : COLOR;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType InstanceVertexShader(VertexInputType input)
{
    PixelInputType output;
    
		matrix scaleMatrix = matrix(
								input.instanceSize, 0.f, 0.f, 0.f,
								0.f, input.instanceSize, 0.f, 0.f,
								0.f, 0.f, input.instanceSize, 0.f,
								0.f, 0.f, 0.f, 1.f
								);

    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	// hacemos que el quad mire siempre a la camara
	input.position = mul(input.position, rotMatrix);
	input.position = mul(input.position, scaleMatrix);

    // Update the position of the vertices based on the data for this particular instance.
    input.position.x += input.instancePosition.x;
    input.position.y += input.instancePosition.y;
    input.position.z += input.instancePosition.z;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

	// Store the particle color for the pixel shader. 
    output.color = input.instanceColor;
    
    return output;
}