#ifndef VISUALIZER_CONFIG_H
#define VISUALIZER_CONFIG_H

struct VisualizerConfig {
  int smoothness = 5;            // Smoothness of the waveform
  float rotationSpeed = 0.25f;   // Base rotation speed (radians per second)
  float fadeFactor = 0.99f;      // Fade factor for the shader
  float waveformHeight = 225.0f; // Height of the waveform
  float radiusFactor = 0.6f;     // Radius factor for the waveform
  float hue = 0.0f;              // Initial hue value
  int pixelSize = 25;            // Pixel size for pixelation effect
  float blendFactor = 0.1f;      // Blend factor for the shader
  int thickness = 10;            // Thickness of the waveform
  float hueOffset = 0.95f;       // Hue offset for thick waveforms
  float hueRotationSpeed = 0.5f; // Speed of hue rotation
};

#endif // VISUALIZER_CONFIG_H
