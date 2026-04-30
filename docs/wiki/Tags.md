# Tags

Tags are placeholders inside `default-board` lines.

Example:

```yaml
- "§dName: §5{scorehud.name}"
- "§bOnline: §a{scorehud.online} §c/ §d{scorehud.max_online}"
```

Unknown tags render as empty text.

## Player Tags

| Tag | Description |
| --- | --- |
| `{scorehud.name}` | Player name |
| `{scorehud.real_name}` | Player name |
| `{scorehud.display_name}` | Player display name. Currently the same as player name in this port |
| `{scorehud.item_name}` | Main-hand item identifier |
| `{scorehud.item_count}` | Main-hand item amount |
| `{scorehud.x}` | Player block X |
| `{scorehud.y}` | Player block Y |
| `{scorehud.z}` | Player block Z |
| `{scorehud.level_name}` | Current Endstone level name |
| `{scorehud.world_name}` | Current Endstone level name |
| `{scorehud.level_folder_name}` | Current Endstone level name |
| `{scorehud.world_folder_name}` | Current Endstone level name |
| `{scorehud.world_player_count}` | Online players in the current Endstone level |
| `{scorehud.ip}` | Player IP address |
| `{scorehud.ping}` | Player ping in milliseconds |
| `{scorehud.health}` | Current health |
| `{scorehud.max_health}` | Maximum health |
| `{scorehud.gamemode}` | Current game mode |

## Server Tags

| Tag | Description |
| --- | --- |
| `{scorehud.online}` | Online player count |
| `{scorehud.max_online}` | Server max player count |
| `{scorehud.load}` | Current tick usage |
| `{scorehud.tps}` | Current TPS |
| `{scorehud.mspt}` | Current MSPT |
| `{scorehud.time}` | Formatted local time |
| `{scorehud.date}` | Formatted local date |

## Memory Tags

Memory tags are only available when enabled:

```yaml
tag-factory:
  enable-memory-tags: true
```

| Tag | Description |
| --- | --- |
| `{scorehud.memory_main_thread}` | Process resident memory from `/proc/self/status` |
| `{scorehud.memory_total}` | Process high-water resident memory |
| `{scorehud.memory_virtual}` | Process virtual memory |
| `{scorehud.memory_heap}` | Process data segment memory |
| `{scorehud.memory_global}` | Always `MAX` in this port |

## Custom Tags

The C++ code exposes internal tag update methods:

```cpp
setServerTag(scorehud::ScoreTag{"example.money", "100"});
setPlayerTag(player, scorehud::ScoreTag{"example.rank", "Admin"});
```

Tags set this way can be used in `scorehud.yml`:

```yaml
- "Money: {example.money}"
- "Rank: {example.rank}"
```

This is an internal extension point for this plugin codebase. It is not a cross-plugin public service API.
