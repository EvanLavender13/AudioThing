# AudioThing

A real-time audio spectrum visualizer that captures system audio and displays dynamic waveform visualizations with configurable effects and GPU-accelerated shaders.

## Features

- Real-time audio capture from system playback
- Circular waveform visualization with FFT processing
- Multiple configurable waveforms with independent settings
- GPU-accelerated shader effects (fade, blur, pixelation)
- Interactive ImGui-based configuration interface
- Preset saving and loading system
- Customizable color schemes with hue rotation
- Filter pipeline for audio processing

## Prerequisites

- Windows operating system
- Visual Studio 2019 or later with C++20 support
- vcpkg package manager
- Git

## Building from Source

### 1. Clone the Repository

```bash
git clone https://github.com/EvanLavender13/AudioThing.git
cd AudioThing
```

### 2. Install Dependencies

This project uses vcpkg for dependency management. If you don't have vcpkg integrated with Visual Studio, follow the [vcpkg integration instructions](https://vcpkg.io/en/getting-started.html).

The required dependencies will be automatically installed when you build the project:
- SFML (graphics and audio)
- FFTW3 (Fast Fourier Transform)
- ImGui-SFML (user interface)
- ImPlot (plotting widgets)

### 3. Build the Project

Open `AudioThing.sln` in Visual Studio and build the solution:

1. Open the solution file in Visual Studio
2. Select your desired configuration (Debug or Release)
3. Select x64 as the platform
4. Build the solution (Ctrl+Shift+B)

The first build may take several minutes as vcpkg downloads and compiles the dependencies.

### 4. Run the Application

Run the project from Visual Studio (F5) or execute the built binary from:
- Debug: `x64\Debug\AudioThing.exe`
- Release: `x64\Release\AudioThing.exe`

## Usage

Upon launching, the application will:
1. Open a window displaying the audio visualizer
2. Begin capturing system audio playback
3. Display real-time waveform visualizations

### Controls

Use the ImGui interface to:
- Adjust shader effects (fade, blur, pixelation)
- Add/remove waveforms
- Configure individual waveform properties (radius, rotation speed, thickness, smoothness)
- Modify global settings (hue rotation, display height)
- Save and load visualization presets

### Configuration

Presets are saved as JSON files and can be managed through the UI's preset manager. The configuration includes:
- Global visualizer settings
- Shader effect parameters
- Individual waveform configurations
- Audio filter settings

## Project Structure

- `src/` - Source code files
  - `AudioThing.cpp` - Main application entry point
  - `AudioCapture.cpp` - System audio capture implementation
  - `AudioVisualizer.cpp` - Visualization rendering logic
  - `UIManager.cpp` - ImGui interface management
  - `Waveform.cpp` - Individual waveform rendering
  - Configuration and utility files
- `*.frag` - GLSL fragment shaders
- `vcpkg.json` - Dependency manifest

## License

This project is provided as-is for educational and personal use.
