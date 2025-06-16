# CodeManager

Der CodeManager instanziiert alle anderen Manager und
steuert den Ablauf. Er spricht direkt mit dem SettingsManager, dem Theme-Manager und dem configfileManager.
Der [EventManager](EventManager.md) ist die zentrale Kommunikationsschnittstelle.

```text
SettingsManager → ThemeManager → TextureManager → GameManager
       ↑               ↑             ↑
       └───────────────┴─────────────┘
              ConfigFileManager
                 CodeManager
```

In `main` liefe dann der CodeManager und würde die andere Manager initialisieren.
Der GameManager startet die GameLoop, wenn der CodeManager das Event `GameStart` auslöst.
