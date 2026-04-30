# Commands and Permissions

## Commands

| Command | Description |
| --- | --- |
| `/scorehud` | Show usage |
| `/scorehud help` | Show usage |
| `/scorehud about` | Show plugin information |
| `/scorehud on` | Enable ScoreHud for yourself |
| `/scorehud off` | Disable ScoreHud for yourself |
| `/sh` | Alias for `/scorehud` |

## Permission

```text
sh.command.sh
```

Default:

```text
true
```

All players can use the command unless the server permission setup overrides it.

## Notes

The `/scorehud off` state is stored in memory only. It resets when the server restarts or the plugin reloads.
