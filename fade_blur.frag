uniform sampler2D texture;
uniform float fadeFactor;
uniform vec2 resolution;
uniform float pixelSize;
uniform float blendFactor;
uniform float fadeThreshold;

// Enhancement effect uniforms
uniform float saturationBoost;
uniform float ditherStrength;
uniform float time; // For temporal dithering

// Function to generate a random value based on UV coordinates
float random(vec2 uv) {
    return fract(sin(dot(uv.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

// Convert RGB to HSV
vec3 rgb2hsv(vec3 c) {
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

// Convert HSV to RGB
vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
    vec2 uv = gl_TexCoord[0].xy;

    vec4 color = texture2D(texture, uv);

    // Apply a 5x5 Gaussian blur
    vec4 blurColor = vec4(0.0);
    float blurSize = 1.0 / resolution.x;

    // Flattened Gaussian kernel weights
    float kernel[25];
    kernel[0] = 1.0;  kernel[1] = 4.0;  kernel[2] = 6.0;  kernel[3] = 4.0;  kernel[4] = 1.0;
    kernel[5] = 4.0;  kernel[6] = 16.0; kernel[7] = 24.0; kernel[8] = 16.0; kernel[9] = 4.0;
    kernel[10] = 6.0; kernel[11] = 24.0; kernel[12] = 36.0; kernel[13] = 24.0; kernel[14] = 6.0;
    kernel[15] = 4.0; kernel[16] = 16.0; kernel[17] = 24.0; kernel[18] = 16.0; kernel[19] = 4.0;
    kernel[20] = 1.0; kernel[21] = 4.0;  kernel[22] = 6.0;  kernel[23] = 4.0;  kernel[24] = 1.0;

    float kernelSum = 256.0;

    int index = 0;
    for (int x = -2; x <= 2; x++) {
        for (int y = -2; y <= 2; y++) {
            blurColor += texture2D(texture, uv + vec2(x, y) * blurSize) * kernel[index];
    index++;
        }
    }
    blurColor /= kernelSum;

    // Pixelation effect with UV centering
    vec2 pixelatedUV = (floor(uv * resolution / pixelSize) + 0.5) * pixelSize / resolution;

    // Jittered sampling for anti-aliasing
    vec4 pixelatedColor = vec4(0.0);
    int sampleCount = 8;
    for (int i = 0; i < sampleCount; ++i) {
        for (int j = 0; j < sampleCount; ++j) {
          float angle = random(uv + vec2(i, j)) * 6.2831853;
     float radius = random(uv + vec2(j, i));
          radius *= (1.0 / resolution.x);

     vec2 jitter = vec2(cos(angle), sin(angle)) * radius;
        pixelatedColor += texture2D(texture, pixelatedUV + jitter);
        }
    }
    pixelatedColor /= float(sampleCount * sampleCount);

    // Blend pixelated effect with blur effect using blendFactor
    vec4 finalColor = mix(blurColor, pixelatedColor, blendFactor);

    // Apply improved fade effect with exponential decay
    finalColor.rgb *= pow(fadeFactor, 1.5);
  
    // === Saturation Boost (AFTER fade) ===
    // This preserves vibrant colors even as they fade
    if (saturationBoost > 1.0) {
   vec3 hsv = rgb2hsv(finalColor.rgb);
        hsv.y *= saturationBoost;
        hsv.y = clamp(hsv.y, 0.0, 1.0);
        finalColor.rgb = hsv2rgb(hsv);
    }
    
    // === Visible Dithering ===
    // Stronger dithering to actually see the effect
    if (ditherStrength > 0.0) {
     // Use spatial-temporal noise
        float noise = random(uv * 100.0 + vec2(time * 0.1));
        noise = (noise - 0.5) * ditherStrength;
      finalColor.rgb += vec3(noise);
    }
    
    // Apply threshold to eliminate very dark pixels completely
    float finalLuminance = dot(finalColor.rgb, vec3(0.299, 0.587, 0.114));
    
    float threshold = max(fadeThreshold, 0.01);
    if (finalLuminance < threshold) {
        finalColor.rgb = vec3(0.0, 0.0, 0.0);
    }
    
 finalColor.a = 1.0;

    gl_FragColor = finalColor;
}
