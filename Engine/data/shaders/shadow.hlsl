#include "shadow_common.hlsl"

cbuffer LightBuffer
{
    float4 m_ambientColor;
    float4 m_diffuseColor;
    float3 m_lightDirection;
    float m_lightIntensity;
    float m_lightType;
    float m_isSoftShadow;
    float m_isDirection;
    float m_ShadowSize;
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
    output.normal = normalize(mul(input.normal, (float3x3) worldMatrix));
	
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

    if (m_lightType == LIGHT_DIRECTIONAL) {
        intensity = saturate(dot(input.normal, -m_lightDirection));
    } else if (m_lightType == LIGHT_SPOT) {
        intensity = saturate(dot(input.normal, -m_lightDirection));
    } else {
        intensity = saturate(dot(input.normal, input.lightPos));
    }
    //intensity = saturate(dot(input.normal, -m_lightDirection));
    float4 defaultColor = m_diffuseColor * intensity * m_lightIntensity;
    float4 textureColor = shaderTexture.Sample(SampleTypeTexture, input.tex);
    
    g_ShadowSize = m_ShadowSize;
    g_isSoftShadow = m_isSoftShadow;
    
    /*color = m_ambientColor + calcShadow(input.lightViewPosition, defaultColor, intensity);
    color = saturate(color) * textureColor;
    return color;*/
    
    /////
    float3 normalWS = normalize(input.normal);
    float3 positionWS = input.position.xyz;
    
    
    float nDotL = saturate(dot(normalWS, -m_lightDirection));
    float nmlOffsetScale = saturate(1.0f - nDotL);
    float texelSize = 2.0f / 2048.0f;

    float3 offset = texelSize * nmlOffsetScale * normalWS;
    float3 samplePos = positionWS + offset;
    //float3 shadowPosition = mul(float4(samplePos, 1.0f), lightViewMatrix * lightProjectionMatrix).xyz;
    float4 shadowPosition = input.lightViewPosition;
    float lightDepth = shadowPosition.z;
    float bias = 0.0050f;
    
    
    lightDepth = input.lightViewPosition.z / input.lightViewPosition.w;
    lightDepth -= bias;
    
    float2 projectTexCoord;
    projectTexCoord.x = input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
    projectTexCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w / 2.0f + 0.5f;
    
    float shadowVisibility = depthMapTexture.SampleCmpLevelZero(SamplePointCmp, projectTexCoord.xy, lightDepth).r;
    //float shadowVisibility = depthMapTexture.SampleCmpLevelZero(SamplePointCmp, shadowPosition.xy, lightDepth).r;
    //return float4(shadowVisibility, 0, 0, 1);
    //return depthMapTexture.Sample(SampleTypeShadow, shadowPosition.xy);
    
    
    
    color = nDotL * m_ambientColor * m_diffuseColor * m_lightIntensity * (1.0f / 3.14159f) * shadowVisibility;
    color += 0.1f * textureColor;
    //return color;
    return float4(max(color.xyz, 0.0001f), 1.0f);
    /////
}
