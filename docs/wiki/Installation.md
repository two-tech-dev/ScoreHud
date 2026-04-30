# Installation

## Requirements

- Minecraft Bedrock Dedicated Server running Endstone
- Endstone `0.11`
- ScoreHud binary for your operating system

## Install From a Release

1. Download the ScoreHud binary for your platform.
2. Put it in the server `plugins/` folder.
3. Start the server.
4. Confirm ScoreHud is listed in the plugin load output.

## Build From Source

Install CMake and a C++20 compiler.

Linux:

```bash
CC=clang CXX=clang++ cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Windows:

```powershell
cmake -B build
cmake --build build --config Release
```

Build outputs:

- Linux: `build/endstone_scorehud.so`
- Windows: `build/Release/endstone_scorehud.dll`

## First Run

On first startup, ScoreHud creates:

```text
config.yml
scorehud.yml
```

Restart the server after editing these files.
