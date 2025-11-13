#include "AudioVisualizer.h"
#include <cmath>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

AudioVisualizer::AudioVisualizer(VisualizerConfig &config, ShaderConfig &shaderConfig)
    : config(config), shaderConfig(shaderConfig), rotationAngle(0.0f) {
  // Create a default waveform with settings from global config
  WaveformConfig waveConfig;
  waveConfig.displayHeight = config.waveformHeight;
  waveConfig.smoothness = config.smoothness;
  waveConfig.rotationSpeed = config.rotationSpeed;
  waveConfig.radiusFactor = config.radiusFactor;
  waveConfig.thickness = static_cast<float>(config.thickness);
  waveConfig.hueOffset = config.hueOffset;
  waveConfig.alpha = 255;
  waveConfig.thickAlpha = 255;

  waveforms.push_back(new Waveform(waveConfig));
}

AudioVisualizer::~AudioVisualizer() {
  // Clean up waveforms
  for (size_t i = 0; i < waveforms.size(); ++i) {
    delete waveforms[i];
  }
  waveforms.clear();
}

bool AudioVisualizer::initialize(unsigned int width, unsigned int height) {
  // Create render texture
  if (!renderTexture.create(width, height)) {
    std::cerr << "Failed to create render texture" << std::endl;
    return false;
  }
  renderTexture.clear(sf::Color::Black);

  // Load shader
  if (!shader.loadFromFile("fade_blur.frag", sf::Shader::Fragment)) {
    std::cerr << "Failed to load shader" << std::endl;
    return false;
  }

  // Initialize shader uniforms
  shader.setUniform("fadeFactor", shaderConfig.fadeFactor);
  shader.setUniform("resolution", sf::Vector2f(static_cast<float>(width),
                                               static_cast<float>(height)));
  shader.setUniform("blendFactor", shaderConfig.blendFactor);
  shader.setUniform("pixelSize", static_cast<float>(shaderConfig.pixelSize));
  shader.setUniform("fadeThreshold", shaderConfig.fadeThreshold);
  
  // Initialize enhancement effect uniforms
  shader.setUniform("saturationBoost", shaderConfig.saturationBoost);
  shader.setUniform("ditherStrength", shaderConfig.ditherStrength);
  shader.setUniform("time", 0.0f);

  return true;
}

void AudioVisualizer::handleResize(unsigned int width, unsigned int height) {
  // Resize render texture
  renderTexture.create(width, height);
  renderTexture.clear(sf::Color::Black);

  // Update shader resolution
  shader.setUniform("resolution", sf::Vector2f(static_cast<float>(width),
                                               static_cast<float>(height)));
}

void AudioVisualizer::update(const std::vector<double> &audioBuffer,
                             float deltaTime) {
  // Update rotation angle for global hue
  rotationAngle += config.rotationSpeed * deltaTime;

  // Update hue based on rotation angle
  config.hue = static_cast<float>(-rotationAngle * config.hueRotationSpeed /
            (2.0 * M_PI));

  // Update all waveforms
  float width = static_cast<float>(renderTexture.getSize().x);
  float height = static_cast<float>(renderTexture.getSize().y);

  for (size_t i = 0; i < waveforms.size(); ++i) {
    waveforms[i]->update(audioBuffer, config.hue, deltaTime, width, height);
  }

  // Update shader uniforms
  shader.setUniform("fadeFactor", shaderConfig.fadeFactor);
  shader.setUniform("pixelSize", static_cast<float>(shaderConfig.pixelSize));
  shader.setUniform("blendFactor", shaderConfig.blendFactor);
  shader.setUniform("fadeThreshold", shaderConfig.fadeThreshold);
  
  // Update enhancement effect uniforms
  shader.setUniform("saturationBoost", shaderConfig.saturationBoost);
  shader.setUniform("ditherStrength", shaderConfig.ditherStrength);
  
  // Update time for temporal dithering
  static float timeAccumulator = 0.0f;
  timeAccumulator += deltaTime;
  shader.setUniform("time", timeAccumulator);
}

void AudioVisualizer::addWaveform(const WaveformConfig &config) {
  waveforms.push_back(new Waveform(config));
}

void AudioVisualizer::removeWaveform(size_t index) {
  if (index < waveforms.size()) {
    delete waveforms[index];
    waveforms.erase(waveforms.begin() + index);
  }
}

void AudioVisualizer::render(sf::RenderWindow &window) {
  // Create sprite from render texture
  sf::Sprite sprite(renderTexture.getTexture());

  // Apply shader to create trail effect
  renderTexture.draw(sprite, &shader);

  // Render all waveforms
  for (size_t i = 0; i < waveforms.size(); ++i) {
    waveforms[i]->render(renderTexture);
  }

  renderTexture.display();

  // Draw accumulated texture to window
  sf::Sprite accumulatedSprite(renderTexture.getTexture());
  window.draw(accumulatedSprite);
}
