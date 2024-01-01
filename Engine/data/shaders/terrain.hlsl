#include "shadow_common.hlsl"

Texture2D normalTexture1 : register(t2);
Texture2D shaderTexture2 : register(t3);
Texture2D normalTexture2 : register(t4);
Texture2D shaderTexture3 : register(t5);
Texture2D normalTexture3 : register(t6);
Texture2D shaderTexture4 : register(t7);
Texture2D normalTexture4 : register(t8);
Texture2D alphaTexture1 : register(t9);

cbuffer LightBuffer
{
    float4 m_ambientColor;
    float4 m_diffuseColor;
    float3 m_lightDirection;
    float m_lightIntensity;
    float m_lightDetailIntensity;
    float m_distanceIntensity;
    float m_countLayers;
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

struct VertexInputType
{
    float4 position : POSITION;
    float4 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float2 tex2 : TEXCOORD1;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float4 depthPosition : TEXCOORD1;
    float2 tex2 : TEXCOORD2;
    float4 lightViewPosition : TEXCOORD3;
};

PixelInputType TerrainVertexShader(VertexInputType input)
{
    PixelInputType output;

    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    output.lightViewPosition = mul(input.position, worldMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, lightViewMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, lightProjectionMatrix);

    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;
    output.tex2 = input.tex2;
    
    // Calculate the normal vector against the world matrix only.
    output.normal = mul(input.normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);

    // Calculate the tangent vector against the world matrix only and then normalize the final value.
    output.tangent = mul(input.tangent, (float3x3) worldMatrix);
    output.tangent = normalize(output.tangent);

    // Calculate the binormal vector against the world matrix only and then normalize the final value.
    output.binormal = mul(input.binormal, (float3x3) worldMatrix);
    output.binormal = normalize(output.binormal);

    // Store the position value in a second input value for depth value calculations.
    output.depthPosition = output.position;

    return output;
}

float4 TerrainPixelShader(PixelInputType input) : SV_TARGET
{
    float depthValue;
    float detailBrightness;
    float4 detailColor;
    float4 bumpMap;
    float3 bumpNormal;
    float lightIntensity1;
    float lightIntensity2;
    float lightIntensity3;
    float lightIntensity4;
    float4 textureColor1;
    float4 textureColor2;
    float4 textureColor3;
    float4 textureColor4;
    float3 lightDir;
    float4 alphaMap1;
    float4 color;

    // Invert the light direction for calculations.
    lightDir = -m_lightDirection;

    // Get the depth value of the pixel by dividing the Z pixel depth by the homogeneous W coordinate.
    depthValue = input.depthPosition.z / input.depthPosition.w;

    bumpNormal = input.normal;
    lightIntensity1 = saturate(dot(bumpNormal, lightDir));
    lightIntensity2 = lightIntensity1;
    lightIntensity3 = lightIntensity1;
    lightIntensity4 = lightIntensity1;

    // Check if the depth value is close to the screen, if so we will apply the detail texture.
    if (depthValue < m_distanceIntensity) {
        /*if (isDetailTexture) {
            detailColor = detailTexture.Sample(SampleTypeWrap, input.tex.zw);
            detailBrightness = lightDetailIntensity;
            textureColor = textureColor * detailColor * detailBrightness;
        }*/

        bumpMap = normalTexture1.Sample(SampleTypeWrap, input.tex.xy);
        bumpMap = (bumpMap * 2.0f) - 1.0f;
        bumpNormal = input.normal + bumpMap.x * input.tangent + bumpMap.y * input.binormal;
        bumpNormal = normalize(bumpNormal);
        lightIntensity1 = saturate(dot(bumpNormal, lightDir));

    }
    
    color = m_ambientColor;
    float4 defaultColor = m_diffuseColor * m_lightIntensity * lightIntensity1;
    
    if (m_ShadowSize) {
        g_ShadowSize = m_ShadowSize;
        g_isSoftShadow = true;
        color = m_ambientColor + calcShadow(input.lightViewPosition, defaultColor, lightIntensity1, (m_lightDirection.z > 0 && input.normal.z <= 0));
    } else {
        color += defaultColor;
    }
    
    textureColor1 = shaderTexture.Sample(SampleTypeWrap, input.tex.xy);
    color = saturate(color * textureColor1);

    if (m_countLayers > 2) {
        if (m_countLayers == 8) {
            alphaMap1 = alphaTexture1.Sample(SampleTypeWrap, input.tex2);
        } else if (m_countLayers == 6) {
            alphaMap1 = shaderTexture4.Sample(SampleTypeWrap, input.tex2);
        } else {
            alphaMap1 = shaderTexture3.Sample(SampleTypeWrap, input.tex2);
        }

        if (depthValue < m_distanceIntensity) {
            bumpMap = normalTexture2.Sample(SampleTypeWrap, input.tex);
            bumpMap = (bumpMap * 2.0f) - 1.0f;
            bumpNormal = input.normal + bumpMap.x * input.tangent + bumpMap.y * input.binormal;
            bumpNormal = normalize(bumpNormal);
            lightIntensity2 = saturate(dot(bumpNormal, lightDir));
        }

        textureColor2 = shaderTexture2.Sample(SampleTypeWrap, input.tex);
        textureColor2 = saturate(lightIntensity2 * textureColor2);
        color = lerp(color, textureColor2, alphaMap1.r);

        if (m_countLayers > 4) {
            if (depthValue < m_distanceIntensity) {
                bumpMap = normalTexture3.Sample(SampleTypeWrap, input.tex);
                bumpMap = (bumpMap * 2.0f) - 1.0f;
                bumpNormal = input.normal + bumpMap.x * input.tangent + bumpMap.y * input.binormal;
                bumpNormal = normalize(bumpNormal);
                lightIntensity3 = saturate(dot(bumpNormal, lightDir));
            }

            textureColor3 = shaderTexture3.Sample(SampleTypeWrap, input.tex);
            textureColor3 = saturate(lightIntensity3 * textureColor3);
            color = lerp(color, textureColor3, alphaMap1.g);

            if (m_countLayers > 6) {
                if (depthValue < m_distanceIntensity) {
                    bumpMap = normalTexture4.Sample(SampleTypeWrap, input.tex);
                    bumpMap = (bumpMap * 2.0f) - 1.0f;
                    bumpNormal = input.normal + bumpMap.x * input.tangent + bumpMap.y * input.binormal;
                    bumpNormal = normalize(bumpNormal);
                    lightIntensity4 = saturate(dot(bumpNormal, lightDir));
                }

                textureColor4 = shaderTexture4.Sample(SampleTypeWrap, input.tex);
                textureColor4 = saturate(lightIntensity4 * textureColor4);
                color = lerp(color, textureColor4, alphaMap1.b);
            }
        }
    }
    
    return color;
}
