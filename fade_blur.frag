uniform sampler2D texture;
uniform float fadeFactor;
uniform vec2 resolution;
uniform float pixelSize;
uniform float blendFactor;
uniform float fadeThreshold; // Remove the initialization

// Function to generate a random value based on UV coordinates
float random(vec2 uv) {
    return fract(sin(dot(uv.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    vec2 uv = gl_TexCoord[0].xy;

    vec4 color = texture2D(texture, uv);

    // Apply a 5x5 Gaussian blur
    vec4 blurColor = vec4(0.0);
    float blurSize = 1.0 / resolution.x; // Adjust blur size as needed

    // Flattened Gaussian kernel weights
    float kernel[25];
    kernel[0] = 1.0;  kernel[1] = 4.0;  kernel[2] = 6.0;  kernel[3] = 4.0;  kernel[4] = 1.0;
    kernel[5] = 4.0;  kernel[6] = 16.0; kernel[7] = 24.0; kernel[8] = 16.0; kernel[9] = 4.0;
    kernel[10] = 6.0; kernel[11] = 24.0; kernel[12] = 36.0; kernel[13] = 24.0; kernel[14] = 6.0;
    kernel[15] = 4.0; kernel[16] = 16.0; kernel[17] = 24.0; kernel[18] = 16.0; kernel[19] = 4.0;
    kernel[20] = 1.0; kernel[21] = 4.0;  kernel[22] = 6.0;  kernel[23] = 4.0;  kernel[24] = 1.0;

    float kernelSum = 256.0; // Sum of all kernel weights

    int index = 0;
    for (int x = -2; x <= 2; x++) {
        for (int y = -2; y <= 2; y++) {
            blurColor += texture2D(texture, uv + vec2(x, y) * blurSize) * kernel[index];
            index++;
        }
    }
    blurColor /= kernelSum; // Normalize the color

    // Pixelation effect with UV centering
    vec2 pixelatedUV = (floor(uv * resolution / pixelSize) + 0.5) * pixelSize / resolution;

    // Jittered sampling for anti-aliasing
    vec4 pixelatedColor = vec4(0.0);
    int sampleCount = 8;
    for (int i = 0; i < sampleCount; ++i) {
        for (int j = 0; j < sampleCount; ++j) {
            // Generate a random angle in [0, 2*PI)
            float angle = random(uv + vec2(i, j)) * 6.2831853;
            // Generate a random radius in [0, 1)
            float radius = random(uv + vec2(j, i));
            // Scale the radius to keep jitter within a pixel
            radius *= (1.0 / resolution.x); // or use y for square pixels

            vec2 jitter = vec2(cos(angle), sin(angle)) * radius;
            pixelatedColor += texture2D(texture, pixelatedUV + jitter);
        }
    }
    pixelatedColor /= float(sampleCount * sampleCount);

    // Blend pixelated effect with blur effect using blendFactor
    vec4 finalColor = mix(blurColor, pixelatedColor, blendFactor);

    // Apply improved fade effect with exponential decay for more natural fading
    finalColor.rgb *= pow(fadeFactor, 1.5);
    
    // Apply threshold to eliminate very dark pixels completely
    float luminance = dot(finalColor.rgb, vec3(0.299, 0.587, 0.114));
    
    // Use 0.01 as the default threshold if not explicitly set
    float threshold = max(fadeThreshold, 0.01);
    if (luminance < threshold) {
        finalColor.rgb = vec3(0.0, 0.0, 0.0);
    }
    
    finalColor.a = 1.0;

    gl_FragColor = finalColor;
}
