# Configuration

ScoreHud has two configuration files:

- `config.yml`: refresh behavior and formatting
- `scorehud.yml`: sidebar title and displayed lines

## config.yml

```yaml
config-version: 11

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

### tag-factory.enable

Controls periodic refresh of built-in tags such as TPS, time, date, memory, ping, item, health, position, and online count.

Default:

```yaml
enable: true
```

### tag-factory.update-period

How often built-in tags are refreshed, in seconds.

Default:

```yaml
update-period: 5
```

### tag-factory.enable-memory-tags

Enables memory-related tags. These read process memory information and should only be enabled when needed.

Default:

```yaml
enable-memory-tags: false
```

### time.zone

Controls the timezone used by `{scorehud.time}` and `{scorehud.date}`.

Use `false` to keep the server process timezone.

```yaml
zone: false
```

Example:

```yaml
zone: "Asia/Ho_Chi_Minh"
```

### time.format

ScoreHud accepts the common PocketMine/PHP-style tokens used by the original plugin:

| Token | Meaning |
| --- | --- |
| `H` | Hour, 24-hour format |
| `i` | Minute |
| `s` | Second |
| `d` | Day |
| `m` | Month |
| `Y` | Year |

## scorehud.yml

```yaml
scorehud-version: 3

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

### titles.flicker

When enabled, ScoreHud rotates through `titles.lines`.

```yaml
flicker: true
```

### titles.period

How often the title changes, in seconds.

```yaml
period: 5
```

### titles.title

The static title shown when flicker is disabled.

### titles.lines

The rotating title list used when flicker is enabled.

### default-board

The scoreboard lines shown to every player.

Minecraft sidebars can display up to 15 lines. Extra lines are ignored.

Empty lines are allowed:

```yaml
- ""
```

## Multiworld

Endstone does not currently support PocketMine-style multiworld APIs. ScoreHud does not read `multi-world`, `disabled-worlds`, or `scoreboards:` sections.

Use `default-board` for all players.
