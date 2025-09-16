#include "AudioVisualizer.h"
#include <cmath>
#include <iostream>

AudioVisualizer::AudioVisualizer(VisualizerConfig &config)
    : config(config), rotationAngle(0.0f) { // Non-const reference
  waveform.setPrimitiveType(sf::LineStrip);
  thickWaveform.setPrimitiveType(sf::LineStrip);
  largeWaveform.setPrimitiveType(sf::LineStrip);
  largeThickWaveform.setPrimitiveType(sf::LineStrip);
}

AudioVisualizer::~AudioVisualizer() {
  // Nothing to clean up
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
  shader.setUniform("fadeFactor", config.fadeFactor);
  shader.setUniform("resolution", sf::Vector2f(static_cast<float>(width),
                                               static_cast<float>(height)));
  shader.setUniform("blendFactor", config.blendFactor);
  shader.setUniform("pixelSize", static_cast<float>(config.pixelSize));

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
  // Update rotation angle
  rotationAngle += config.rotationSpeed * deltaTime;

  // Update hue based on rotation angle
  config.hue = static_cast<float>(-rotationAngle * config.hueRotationSpeed /
                                  (2.0 * M_PI));

  // Draw main waveform
  drawWaveform(audioBuffer, waveform, thickWaveform, config.waveformHeight,
               config.smoothness, -rotationAngle, config.radiusFactor,
               static_cast<float>(renderTexture.getSize().x),
               static_cast<float>(renderTexture.getSize().y), config.hue,
               static_cast<float>(config.thickness), config.hueOffset);

  // Draw larger waveform
  drawWaveform(audioBuffer, largeWaveform, largeThickWaveform,
               config.waveformHeight * 1.5f, 1, rotationAngle,
               config.radiusFactor * 2.5f,
               static_cast<float>(renderTexture.getSize().x),
               static_cast<float>(renderTexture.getSize().y), config.hue,
               static_cast<float>(config.thickness), config.hueOffset);

  // Reduce opacity for larger waveform
  for (size_t i = 0; i < largeWaveform.getVertexCount(); ++i) {
    largeWaveform[i].color = waveform[i].color;
    largeWaveform[i].color.a = 128;
  }

  for (size_t i = 0; i < largeThickWaveform.getVertexCount(); ++i) {
    largeThickWaveform[i].color = thickWaveform[i].color;
    largeThickWaveform[i].color.a = 128;
  }

  // Update shader uniforms
  shader.setUniform("fadeFactor", config.fadeFactor);
  shader.setUniform("pixelSize", static_cast<float>(config.pixelSize));
  shader.setUniform("blendFactor", config.blendFactor);
  shader.setUniform("fadeThreshold", 0.01f);
}

void AudioVisualizer::render(sf::RenderWindow &window) {
  // Create sprite from render texture
  sf::Sprite sprite(renderTexture.getTexture());

  // Apply shader to create trail effect
  renderTexture.draw(sprite, &shader);

  // Draw all waveforms to render texture
  renderTexture.draw(waveform);
  renderTexture.draw(largeWaveform);
  renderTexture.draw(thickWaveform);
  renderTexture.draw(largeThickWaveform);

  renderTexture.display();

  // Draw accumulated texture to window
  sf::Sprite accumulatedSprite(renderTexture.getTexture());
  window.draw(accumulatedSprite);
}
