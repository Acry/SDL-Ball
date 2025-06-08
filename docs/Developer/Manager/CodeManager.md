# CodeManager

```text
SettingsManager → ThemeManager → TextureManager → GameManager
       ↑               ↑             ↑
       └───────────────┴─────────────┘
              ConfigFileManager
                 CodeManager
```

In `main` liefe dann der CodeManager und würde die andere Manager initialisieren.
Der GameManager startet die GameLoop, wenn der CodeManager das Event `GameStart` auslöst.
