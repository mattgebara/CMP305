// Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)

Texture2D texture0 : register(t0);
Texture2D texture1 : register(t1);
Texture2D texture2 : register(t2);
Texture2D texture3 : register(t3);
SamplerState sampler0 : register(s0);

cbuffer LightBuffer : register(b0)
{
	float4 diffuseColour;
	float3 lightDirection;
	float padding;
};

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
    float3 worldPos : TEXCOORD1;

};

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
	float intensity = saturate(dot(normal, lightDirection));
	float4 colour = saturate(diffuse * intensity);
	return colour;
}

float4 main(InputType input) : SV_TARGET
{
	float4 textureColour;
    float4 textureColour1;
    float4 textureColour2;
    float4 textureColour3;
    
	float4 lightColour;

	// Sample the texture. Calculate light intensity and colour, return light*texture for final pixel colour.
	textureColour = texture0.Sample(sampler0, input.tex);
    textureColour1 = texture1.Sample(sampler0, input.tex);
    textureColour2 = texture2.Sample(sampler0, input.tex);
    textureColour3 = texture3.Sample(sampler0, input.tex);


    float Zgrad = 1 - abs(input.normal.z);
    float Xgrad = 1 - abs(input.normal.x);

   // float noiseValue = PerlinNoise::Noise(TEXCOORD0.x, TEXCOORD0.y, 0);

   // float blendFactor = smoothstep(blendThreshold1, blendThreshold2, noiseValue);


    //checking how high the mountain is, whether to make it snowy, green, or brown

    if (input.worldPos.y < 0) //mud
    {
       textureColour = textureColour3;
    }
    else if (abs(input.normal.x) > 0.5 || abs(input.normal.z) > 0.5) //dirt
    {
       textureColour = lerp(textureColour1, textureColour, 0.1);
    }
    else if (input.worldPos.y > 15) //more snow
    {
       textureColour = lerp(textureColour2, textureColour, 0.1);
    }
    else if (input.worldPos.y > 12) //more mixed
    {
        textureColour = lerp(textureColour2, textureColour, 0.2);
    }
    else if (input.worldPos.y > 9) //more mixed
    {
        textureColour = lerp(textureColour2, textureColour, 0.3);
    }
    else if (input.worldPos.y > 6) //more mixed
    {
        textureColour = lerp(textureColour2, textureColour, 0.4);
    }
    else if (input.worldPos.y > 3) //snow
    {
        textureColour = lerp(textureColour2, textureColour, 0.5);
    }
    else //grass
    {
        textureColour = lerp(textureColour, textureColour2, 0.1);
    }

	lightColour = calculateLighting(-lightDirection, input.normal, diffuseColour);
	
	return lightColour * textureColour;

  
}



