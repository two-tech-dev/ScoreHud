# ScoreHud

ScoreHud is an Endstone C++ plugin that displays a configurable sidebar scoreboard for Minecraft Bedrock Dedicated Server.

This project is an Endstone port of ScoreHud maintained by TwoTech. It targets the current Endstone world model and uses one global scoreboard layout from `default-board`.

## Features

- Configurable sidebar title and lines
- Optional flickering titles
- Built-in player and server tags
- Runtime tag refresh for TPS, tick usage, time, date, ping, health, item, position, and online count
- `/scorehud` and `/sh` command support
- Per-player HUD toggle with `/scorehud on` and `/scorehud off`
- Generated default `config.yml` and `scorehud.yml`

## Not Supported

Endstone does not currently expose PocketMine-style multiworld support. ScoreHud does not implement per-world boards, disabled worlds, or `scoreboards:` sections. Use `default-board` for every player.

## Requirements

- Endstone `0.11`
- CMake `3.15+`
- C++20 compiler
- Linux: Clang with libc++
- Windows: Visual Studio with Desktop development with C++

## Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Linux builds should use Clang:

```bash
CC=clang CXX=clang++ cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

The plugin binary is written to the build directory:

- Linux: `build/endstone_scorehud.so`
- Windows: `build/Release/endstone_scorehud.dll`

## Install

1. Build or download the ScoreHud binary for your server platform.
2. Copy the binary into the server `plugins/` folder.
3. Start the server.
4. Edit the generated files in the ScoreHud data folder:
   - `config.yml`
   - `scorehud.yml`
5. Restart the server after changing configuration.

## Commands

| Command | Description |
| --- | --- |
| `/scorehud` | Show command usage |
| `/scorehud on` | Enable the sidebar for yourself |
| `/scorehud off` | Disable the sidebar for yourself |
| `/scorehud about` | Show plugin information |
| `/scorehud help` | Show command usage |
| `/sh` | Alias for `/scorehud` |

Permission:

```text
sh.command.sh
```

Default: everyone.

## Configuration

`scorehud.yml` controls the sidebar title and lines.

```yaml
titles:
  flicker: false
  period: 5
  title: "§l§aServer §dName"
  lines:
  - "§l§aServer §dName"
  - "§l§bServer §cName"

default-board:
- ""
- "§dName: §5{scorehud.name}"
- ""
- "§bOnline: §a{scorehud.online} §c/ §d{scorehud.max_online}"
- ""
- "§dTPS: §5{scorehud.tps}"
- "§bLoad: §a{scorehud.load}"
```

`config.yml` controls refresh behavior and formatting.

```yaml
tag-factory:
  enable: true
  update-period: 5
  enable-memory-tags: false

time:
  zone: false
  format:
    time: "H:i:s"
    date: "d-m-Y"
```

## Built-In Tags

Common tags:

```text
{scorehud.name}
{scorehud.online}
{scorehud.max_online}
{scorehud.tps}
{scorehud.load}
{scorehud.time}
{scorehud.date}
{scorehud.x}
{scorehud.y}
{scorehud.z}
{scorehud.health}
{scorehud.max_health}
{scorehud.ping}
{scorehud.item_name}
{scorehud.item_count}
{scorehud.world_name}
{scorehud.gamemode}
```

Unknown tags render as empty text.

## Documentation

Wiki-style documentation is available in [`docs/wiki`](docs/wiki/Home.md).

## Project Layout

```text
include/
  plugin.h
  listener.h
  scorehud/
    score_tag.h
    scoreboard_view.h
    session_manager.h
    settings.h
    tag_resolver.h
src/
  plugin.cpp
  listener.cpp
  scorehud/
    score_tag.cpp
    scoreboard_view.cpp
    session_manager.cpp
    settings.cpp
    tag_resolver.cpp
```

## License

See [`LICENSE`](LICENSE).
