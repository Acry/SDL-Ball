// Das Plugin sollte unter "Tools > SDL-Ball > SDL-Ball Level" erscheinen.
// gimp --verbose
#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

#define PLUG_IN_PROC "plug-in-sdlball"

typedef struct _SDLBall SDLBall;
typedef struct _SDLBallClass SDLBallClass;

struct _SDLBall
{
  GimpPlugIn parent_instance;
};

struct _SDLBallClass
{
  GimpPlugInClass parent_class;
};

// Funktionsdeklarationen
static GList *sdlball_query_procedures (GimpPlugIn *plug_in);
static GimpProcedure *sdlball_create_procedure (GimpPlugIn *plug_in, const gchar *name);
static GimpValueArray *sdlball_run (GimpProcedure *procedure,
                                  GimpRunMode run_mode,
                                  GimpImage *image,
                                  GimpDrawable **drawables,
                                  GimpProcedureConfig *config,
                                  gpointer run_data);

// Hilfsfunktion für RGB-Vergleich
static gboolean compare_rgb(const guchar a[], const guchar b[])
{
  return (a[0] == b[0] && a[1] == b[1] && a[2] == b[2]);
}

// GObject Type Definition
G_DEFINE_TYPE (SDLBall, sdlball, GIMP_TYPE_PLUG_IN)

static void
sdlball_class_init (SDLBallClass *klass)
{
  GimpPlugInClass *plug_in_class = GIMP_PLUG_IN_CLASS (klass);
  plug_in_class->query_procedures = sdlball_query_procedures;
  plug_in_class->create_procedure = sdlball_create_procedure;
}

static void
sdlball_init (SDLBall *sdlball)
{
}

static GList *
sdlball_query_procedures (GimpPlugIn *plug_in)
{
  return g_list_append (NULL, g_strdup (PLUG_IN_PROC));
}

static GimpProcedure *
sdlball_create_procedure (GimpPlugIn *plug_in,
                         const gchar *name)
{
  GimpProcedure *procedure = NULL;

  if (!strcmp (name, PLUG_IN_PROC))
    {
      procedure = gimp_image_procedure_new (plug_in, name,
                                          GIMP_PDB_PROC_TYPE_PLUGIN,
                                          sdlball_run, NULL, NULL);

      gimp_procedure_set_menu_label (procedure, "SDL-Ball Level");
    gimp_procedure_add_menu_path (procedure, "<Image>/Filters/Tutorial/");
      gimp_procedure_set_documentation (procedure,
                                      "Creates SDL-Ball level from image",
                                      "Converts image to SDL-Ball level format",
                                      name);
      gimp_procedure_set_attribution (procedure,
                                    "Jimmy Christensen (Port by Acry)",
                                    "SDL-Ball",
                                    "2025");
      gimp_procedure_set_image_types (procedure, "RGB*");
    }

  return procedure;
}

static GimpValueArray *
sdlball_run (GimpProcedure *procedure,
            GimpRunMode run_mode,
            GimpImage *image,
            GimpDrawable **drawables,
            GimpProcedureConfig *config,
            gpointer run_data)
{
  // Farbdefinitionen für Blocktypen
  guchar type[13][3] = {
    {0, 0, 255},     // BLU - '1'
    {255, 255, 0},   // YEL - '2'
    {0xC9, 0xC9, 0xC9}, // CEM - '3'
    {0xFF, 0xC5, 0xC5}, // GLA - '4'
    {0x00, 0xFF, 0x00}, // GRE - '5'
    {0xDB, 0xCF, 0xFF}, // STE - '6'
    {0xFF, 0x00, 0xFF}, // PUR - '7'
    {0xFF, 0xFF, 0xFF}, // WHI - '8'
    {0x00, 0xFF, 0xD8}, // INV - '9'
    {0xFF, 0x00, 0x00}, // RED - 'A'
    {0xFF, 0xB9, 0x00}, // EXP - 'B'
    {0x72, 0x00, 0xFF}, // DOO - 'C'
    {0x00, 0x00, 0x00}  // NON - '0'
  };

  GimpDrawable *drawable = drawables[0];
  GeglBuffer *buffer = gimp_drawable_get_buffer(drawable);

  // Level-Arrays
  char level[23][26];
  char brickColor[23][26][7];
  char powerups[23][26];

  // Pixel-Daten lesen
  guchar pixel[3];
  for(int row = 0; row < 23; row++) {
    for(int col = 0; col < 26; col++) {
      GeglRectangle rect = {col, row, 1, 1};
      gegl_buffer_get(buffer, &rect, 1.0, babl_format("R'G'B' u8"),
                     pixel, GEGL_AUTO_ROWSTRIDE, GEGL_ABYSS_NONE);

      // Block-Typ bestimmen
      char block = '0';
      for(int t = 0; t < 13; t++) {
        if(compare_rgb(pixel, type[t])) {
          block = '1' + t;  // Konvertiere Index zu Blocktyp
          break;
        }
      }

      // Benutzerdefinierte Farbe
      if(block == '0' && (pixel[0] || pixel[1] || pixel[2])) {
        block = 'D';
        sprintf(brickColor[row][col], "%02X%02X%02X",
                pixel[0], pixel[1], pixel[2]);
      }

      level[row][col] = block;

      // Powerups zuweisen (5% Chance)
      powerups[row][col] = (block != '0' && block != '3') ? 'L' : '0';
    }
  }

  // Level ausgeben
  g_print("\n\n** Start **");
  for(int row = 0; row < 23; row++) {
    g_print("\n");
    for(int col = 0; col < 26; col++) {
      g_print("%c%c", powerups[row][col], level[row][col]);
      if(level[row][col] == 'D') {
        g_print("%s", brickColor[row][col]);
      }
    }
  }
  g_print("\n** Stop **\n");

  g_object_unref(buffer);

  return gimp_procedure_new_return_values(procedure, GIMP_PDB_SUCCESS, NULL);
}

int
main (int argc, char *argv[])
{
  return gimp_main (sdlball_get_type (), argc, argv);
}