# Troubleshooting

## The Plugin Does Not Load

Check that the binary matches your server platform:

- Linux: `.so`
- Windows: `.dll`

Check that the server is running a compatible Endstone version.

## The Sidebar Does Not Appear

Try:

```text
/scorehud on
```

Also check that the player has:

```text
sh.command.sh
```

## Tags Are Blank

Unknown tags render as empty text.

Check the tag name carefully:

```yaml
- "{scorehud.online}"
```

If using a custom tag, confirm the C++ code calls `setServerTag` or `setPlayerTag`.

## Memory Tags Are Blank

Enable memory tags:

```yaml
tag-factory:
  enable-memory-tags: true
```

On non-Linux platforms, process memory tags may not provide the same values because this port reads Linux `/proc/self/status`.

## Config Changes Do Not Apply

Restart the server after editing:

```text
config.yml
scorehud.yml
```

## Per-World Boards Do Not Work

Per-world boards are not supported in this Endstone port because Endstone does not currently expose PocketMine-style multiworld APIs.

Use `default-board`.
