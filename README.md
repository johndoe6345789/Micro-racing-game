# Micro Racing Game

A top-down 2D racing game inspired by Micro Machines and Hot Wheels, built with SDL3 and C++.

## Features

- **Main Menu**: Navigate through game options with keyboard controls
- **Racing Gameplay**: Control a car in top-down view with realistic physics
- **AI Opponents**: Race against 3 AI-controlled bots with adjustable difficulty
- **Camera System**: Smooth camera that follows the player car around the track
- **Track System**: JSON-based track format supporting various tile types:
  - Regular track tiles
  - Jumps for exciting gameplay
  - Walls and boundaries
  - Start/finish lines and checkpoints
- **Map Editor**: Visual track editor with tile placement and saving/loading
- **Sample Tracks**: 3 pre-designed tracks with different layouts and jumps

## Building

### Prerequisites

- CMake 3.20+
- Ninja build system
- Conan (for dependency management)
- Python 3.x (for Conan)
- C++17 compatible compiler

### Dependencies (managed by Conan)

- SDL3
- SDL_image
- SDL_ttf
- nlohmann_json

### Build Instructions

```bash
# Clone the repository
git clone https://github.com/johndoe6345789/Micro-racing-game.git
cd Micro-racing-game

# Install Conan if not already installed
pip install conan

# Detect Conan profile
conan profile detect --force

# Install dependencies
conan install . --output-folder=build --build=missing -s build_type=Release

# Configure with CMake (using Ninja generator)
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake

# Build
cmake --build build

# Run the game
./build/racing_game

# Run the map editor
./build/map_editor
```

## Controls

### Game Controls
- **Arrow Keys / WASD**: Control car (accelerate, brake, steer)
- **ESC**: Pause/unpause game
- **Return to menu**: ESC from main menu

### Map Editor Controls
- **1**: Place track tile
- **2**: Place wall
- **3**: Place jump
- **4**: Place start/finish
- **E**: Erase mode
- **SPACE**: Pan camera (hold and drag)
- **Ctrl+S**: Save track
- **Ctrl+L**: Load track
- **ESC**: Exit editor

## Track Format

Tracks are stored as JSON files in the `tracks/` directory. Each track contains:
- Tile definitions (type, position, size, rotation)
- Start positions for players and AI bots

Example:
```json
{
  "tiles": [
    {"type": 1, "x": 300, "y": 200, "width": 100, "height": 100, "angle": 0}
  ],
  "startPositions": [
    {"x": 350, "y": 250}
  ]
}
```

Tile types:
- 0: Grass
- 1: Track
- 2: Start/Finish
- 3: Checkpoint
- 4: Jump
- 5: Wall

## CI/CD

The project includes GitHub Actions workflows for:
- Multi-platform builds (Linux, Windows, macOS)
- Ninja build system integration
- QEMU-based cross-compilation testing
- Automated artifact generation

### Workflow diagnostics

Use the helper script to quickly triage common GitHub Actions pitfalls and version drift:

```bash
python tools/workflow_diagnose.py --workflows-dir .github/workflows
```

The script reports missing permissions blocks, floating action references, and other issues that routinely break CI. It installs `pyyaml` automatically if needed.

## Project Structure

```
Micro-racing-game/
├── src/                    # Source code
│   ├── main.cpp           # Game entry point
│   ├── game.cpp/h         # Main game logic
│   ├── menu.cpp/h         # Main menu system
│   ├── car.cpp/h          # Car physics and rendering
│   ├── camera.cpp/h       # Camera follow system
│   ├── track.cpp/h        # Track loading and rendering
│   ├── ai_bot.cpp/h       # AI opponent logic
│   ├── editor.cpp/h       # Map editor
│   ├── renderer.cpp/h     # Rendering utilities
│   ├── input.cpp/h        # Input handling
│   └── physics.cpp/h      # Physics utilities
├── tracks/                # Track files (JSON)
│   ├── track1.json
│   ├── track2.json
│   └── track3.json
├── assets/                # Game assets (textures, fonts)
├── CMakeLists.txt         # CMake build configuration
├── vcpkg.json            # vcpkg dependencies
└── .github/
    └── workflows/
        └── build.yml      # CI/CD configuration
```

## License

MIT License - see LICENSE file for details
