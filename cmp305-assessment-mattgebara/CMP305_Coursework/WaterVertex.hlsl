// Light vertex shader
// Standard issue vertex shader, apply matrices, pass info to pixel shader
cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer WaterBuffer : register(b1)
{
    float terrainSize;
    float resolution;
    float offSet;
    float padding;
};

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;

};

struct OutputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPos : TEXCOORD1;

};

OutputType main(InputType input)
{
    OutputType output;
    //input.position.y = 0;
    float3 temp;
    temp.x = (input.position.x * resolution) / (terrainSize);
    temp.z = (input.position.z * resolution) / (terrainSize);
    temp.y = 0;

    float pullOutStuff = (offSet * (temp.x + temp.z)) / 50.f;


    for (int i = 4; i < 8; ++i)
    {
        float noiseFloat = pullOutStuff * pow(2, i);
        float k = noise(noiseFloat);
        k *= (15.f / pow(2, i));
        k = abs(k);
        //  temp.y += k; breaks the map
    }
    input.position.y = temp.y;


    output.worldPos = input.position.xyz;
    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

    // Calculate the normal vector against the world matrix only and normalise.
    output.normal = mul(input.normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);

    return output;

}