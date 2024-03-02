Texture2D depthMapTexture : register(t0);
Texture2D shaderTexture : register(t1);

SamplerState SampleTypeClamp : register(s0);
SamplerState SampleTypeWrap : register(s1);
SamplerComparisonState SamplePointCmp : register(s2);

static float g_ShadowSize;
static float g_isSoftShadow;

float2 texOffset(int u, int v)
{
    return float2(u * 1.0f / g_ShadowSize, v * 1.0f / g_ShadowSize);
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
            } else {
                int gradientShadowSize = 7;
                float gradientShadowSizeLimit = gradientShadowSize / 2;
                int gradientShadowSizeDelimiter = (gradientShadowSize + 1) * 4;
    
                if (!g_isSoftShadow) {
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
}