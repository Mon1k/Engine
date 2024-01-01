#include "shadow_common.hlsl"
/*
Texture2D shaderTexture : register(t0);
Texture2D depthMapTexture : register(t1);

SamplerState SampleTypeClamp : register(s0);
SamplerState SampleTypeWrap : register(s1);
SamplerComparisonState SamplePointCmp : register(s2);*/

cbuffer LightBuffer
{
    float4 m_ambientColor;
    float4 m_diffuseColor;
    float3 m_lightDirection;
    float m_lightIntensity;
    float m_isSoftShadow;
    float m_isDirection;
    float m_ShadowSize;
    float m_padding;
};

cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix lightViewMatrix;
    matrix lightProjectionMatrix;
};

cbuffer LightBuffer2
{
    float3 lightPosition;
    float padding;
};


struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float4 lightViewPosition : TEXCOORD1;
    float3 lightPos : TEXCOORD2;
};

/*
float2 texOffset(int u, int v)
{
    return float2(u * 1.0f / m_ShadowSize, v * 1.0f / m_ShadowSize);
}

float4 calcShadow(float4 lightViewPosition, float4 defaultColor, float intensity)
{
    float4 color = 0;
    float shadow = 0;
    float bias = 0.000001f;
    float depthValue;
    float lightDepthValue;
    float2 projectTexCoord;
    
    projectTexCoord.x = lightViewPosition.x / lightViewPosition.w / 2.0f + 0.5f;
    projectTexCoord.y = -lightViewPosition.y / lightViewPosition.w / 2.0f + 0.5f;
    
    // Determine if the projected coordinates are in the 0 to 1 range.  If so then this pixel is in the view of the light.
    if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y)) {
        // Sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location.
        depthValue = 1.0f - depthMapTexture.Sample(SampleTypeClamp, projectTexCoord).r;

        // Calculate the depth of the light.
        lightDepthValue = lightViewPosition.z / lightViewPosition.w;
        lightDepthValue -= bias;
        
        // inside light frustum
        // Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
        // If the light is in front of the object then light the pixel, if not then shadow this pixel since an object (occluder) is casting a shadow on it.
        if (lightDepthValue < depthValue) {
            if (intensity > 0.0f) {
                // object with shadow but not shadow
                // Determine the final diffuse color based on the diffuse color and the amount of light intensity.
                color += defaultColor;
            }
        } else {
            // frustum view light
            // outside shadow but in view frustum light as normal light
            if (depthValue == 0.0f) {
                color += defaultColor;
                
            // shadow
            } else /* if (m_lightDirection.z > 0 && input.normal.z <= 0)* {
                int gradientShadowSize = 7;
                float gradientShadowSizeLimit = gradientShadowSize / 2;
                int gradientShadowSizeDelimiter = (gradientShadowSize + 1) * 4;
    
                if (!m_isSoftShadow) {
                    gradientShadowSizeLimit = 0.5;
                    gradientShadowSizeDelimiter = 1;
                }
                
                float x, y;
                for (y = -gradientShadowSizeLimit; y <= gradientShadowSizeLimit; y += 1.0) {
                    for (x = -gradientShadowSizeLimit; x <= gradientShadowSizeLimit; x += 1.0) {
                        shadow += depthMapTexture.SampleCmpLevelZero(SamplePointCmp, projectTexCoord.xy + texOffset(x, y), lightDepthValue);
                    }
                }
                shadow /= gradientShadowSizeDelimiter;
                color += shadow;
            }
        }
        
    // outside frustum light view
    } else if (intensity > 0.0f) {
        color += defaultColor;
    }
    
    return color;
}*/

PixelInputType ShadowVertexShader(VertexInputType input)
{
    PixelInputType output;
    float4 worldPosition;
    
    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // Calculate the position of the vertice as viewed by the light source.
    output.lightViewPosition = mul(input.position, worldMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, lightViewMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, lightProjectionMatrix);

    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;
    
    // Calculate the normal vector against the world matrix only.
    output.normal = mul(input.normal, (float3x3) worldMatrix);
	
    // Normalize the normal vector.
    output.normal = normalize(output.normal);

    // Calculate the position of the vertex in the world.
    worldPosition = mul(input.position, worldMatrix);

    // Determine the light position based on the position of the light and the position of the vertex in the world.
    output.lightPos = lightPosition.xyz - worldPosition.xyz;

    // Normalize the light position vector.
    output.lightPos = normalize(output.lightPos);

    return output;
}

float4 ShadowPixelShader(PixelInputType input) : SV_TARGET
{
    float4 color;
    float intensity;

    if (m_isDirection) {
        intensity = saturate(dot(input.normal, -m_lightDirection));
    } else {
        intensity = saturate(dot(input.normal, input.lightPos));
    }
    
    float4 defaultColor = m_diffuseColor * intensity * m_lightIntensity;
    float4 textureColor = shaderTexture.Sample(SampleTypeWrap, input.tex); 
    
    g_ShadowSize = m_ShadowSize;
    g_isSoftShadow = m_isSoftShadow;
    
    color = m_ambientColor + calcShadow(input.lightViewPosition, defaultColor, intensity, true);
    color = saturate(color) * textureColor;

    return color;
}
