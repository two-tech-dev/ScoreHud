# ScoreHud Wiki

ScoreHud adds a configurable sidebar scoreboard to an Endstone server.

## Pages

- [Installation](Installation.md)
- [Configuration](Configuration.md)
- [Tags](Tags.md)
- [Commands and Permissions](Commands-and-Permissions.md)
- [Developer Notes](Developer-Notes.md)
- [Troubleshooting](Troubleshooting.md)

## Current Endstone Scope

ScoreHud uses a single global layout from `default-board`.

PocketMine ScoreHud supported per-world boards, but Endstone does not currently expose PocketMine-style multiworld support. This port intentionally does not include `multi-world`, `disabled-worlds`, or `scoreboards:` configuration.

## Quick Start

1. Copy `endstone_scorehud.so` or `endstone_scorehud.dll` into the server `plugins/` folder.
2. Start the server once.
3. Edit `scorehud.yml` in the ScoreHud data folder.
4. Restart the server.
5. Use `/scorehud off` or `/scorehud on` in game to toggle the sidebar.
