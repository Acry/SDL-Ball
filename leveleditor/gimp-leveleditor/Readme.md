Working tutorial code in GIMP Filters Development

___ old
you need gimptool2.0 and gimp dev libs to compile this.

to compile do:
gimptool-2.0 --install gimp-sdlball.c
the plugin will be placed in ~/.gimp/plug-ins/ and be loaded automatically.
Start gimp from a terminal in order to record the output from the plugin (you need that)

___

https://developer.gimp.org/resource/writing-a-plug-in/

GIMP provides a public API for constructing plug-ins, split in 2 main libraries:

libgimp
libgimpui

These 2 libraries and most of their functions are available to C plug-ins, as well as to all supported bindings. We will
talk about this more in the Python tutorial.

init_procedures()
query_procedures()
create_procedure() (mandatory)
quit() (optional)
set_i18n() (optional)

$HOME/.config/GIMP/3.0/plug-ins/c-hello-world/

mkdir -p $HOME/.config/GIMP/3.0/plug-ins/c-hello-world/
cp c-hello-world $HOME/.config/GIMP/3.0/plug-ins/c-hello-world/

___

save to file:

```c++
static GimpValueArray *
sdlball_run (GimpProcedure *procedure,
            GimpRunMode run_mode,
            GimpImage *image,
            GimpDrawable **drawables,
            GimpProcedureConfig *config,
            gpointer run_data)
{
  // ... [Vorhandener Code bleibt gleich bis zur Level-Ausgabe] ...

  // Dateinamen generieren
  char filename[256];
  const char *homedir = g_get_home_dir();
  g_snprintf(filename, sizeof(filename), "%s/sdlball_level.txt", homedir);

  // Datei öffnen
  FILE *f = fopen(filename, "w");
  if (!f) {
    gimp_message_printf(GIMP_MESSAGE_ERROR, "Konnte Datei nicht öffnen: %s", filename);
    return gimp_procedure_new_return_values(procedure, GIMP_PDB_EXECUTION_ERROR, NULL);
  }

  // Level in Datei und Konsole ausgeben
  fprintf(f, "** Start **\n");
  g_print("\n\n** Start **");
  
  for(int row = 0; row < 23; row++) {
    fprintf(f, "\n");
    g_print("\n");
    for(int col = 0; col < 26; col++) {
      fprintf(f, "%c%c", powerups[row][col], level[row][col]);
      g_print("%c%c", powerups[row][col], level[row][col]);
      if(level[row][col] == 'D') {
        fprintf(f, "%s", brickColor[row][col]);
        g_print("%s", brickColor[row][col]);
      }
    }
  }
  
  fprintf(f, "\n** Stop **\n");
  g_print("\n** Stop **\n");

  fclose(f);
  g_object_unref(buffer);

  // Erfolgsmeldung
  gimp_message_printf(GIMP_MESSAGE_INFO, 
                     "Level wurde gespeichert in: %s", filename);

  return gimp_procedure_new_return_values(procedure, GIMP_PDB_SUCCESS, NULL);
}
```

___

[plug-in-sdlball] The catalog directory does not exist: /home/carsten/.config/GIMP/3.0/plug-ins/gimp-sdlball3/locale
[plug-in-sdlball] Override method set_i18n() for the plug-in to customize or disable localization.
[plug-in-sdlball] Localization disabled
[plug-in-sdlball] The catalog directory does not exist: /home/carsten/.config/GIMP/3.0/plug-ins/gimp-sdlball3/locale
[plug-in-sdlball] Override method set_i18n() for the plug-in to customize or disable localization.
[plug-in-sdlball] Localization disabled

** Start **
L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=
L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=
L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=
L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=
L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=
L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=
L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=
L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=
L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=
L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=
L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=
L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=
L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=
L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=
L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=
L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=
L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=
L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=
L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=
L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=
L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=
L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=
L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=L=
** Stop **
