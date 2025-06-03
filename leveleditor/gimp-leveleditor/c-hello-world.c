// Es erscheint im GIMP-Menü unter "Filters > Tutorial > Hello"
// This is a simple GIMP plug-in that prints "Hello, world!" to the error console.
#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

#define PLUG_IN_PROC "plug-in-hello"

typedef struct _Hello Hello;
typedef struct _HelloClass HelloClass;

struct _Hello
{
  GimpPlugIn parent_instance;
};

struct _HelloClass
{
  GimpPlugInClass parent_class;
};

// Funktionsdeklarationen
static GList *hello_query_procedures (GimpPlugIn *plug_in);
static GimpProcedure *hello_create_procedure (GimpPlugIn *plug_in, const gchar *name);
static GimpValueArray *hello_run (GimpProcedure *procedure,
                                GimpRunMode run_mode,
                                GimpImage *image,
                                GimpDrawable **drawables,
                                GimpProcedureConfig *config,
                                gpointer run_data);

// GObject Type Definition
G_DEFINE_TYPE (Hello, hello, GIMP_TYPE_PLUG_IN)

static void
hello_class_init (HelloClass *klass)
{
  GimpPlugInClass *plug_in_class = GIMP_PLUG_IN_CLASS (klass);
  plug_in_class->query_procedures = hello_query_procedures;
  plug_in_class->create_procedure = hello_create_procedure;
}

static void
hello_init (Hello *hello)
{
}

static GList *
hello_query_procedures (GimpPlugIn *plug_in)
{
  return g_list_append (NULL, g_strdup (PLUG_IN_PROC));
}

static GimpProcedure *
hello_create_procedure (GimpPlugIn *plug_in,
                       const gchar *name)
{
  GimpProcedure *procedure = NULL;

  if (!strcmp (name, PLUG_IN_PROC))
    {
      procedure = gimp_image_procedure_new (plug_in, name,
                                          GIMP_PDB_PROC_TYPE_PLUGIN,
                                          hello_run, NULL, NULL);

      gimp_procedure_set_menu_label (procedure, "Hello");
      gimp_procedure_add_menu_path (procedure, "<Image>/Filters/Tutorial/");
      gimp_procedure_set_documentation (procedure,
                                      "Prints 'Hello, world!' to the error console",
                                      "A simple GIMP plug-in example",
                                      PLUG_IN_PROC);
      gimp_procedure_set_attribution (procedure, "Your Name", "Your Name", "2025");
      gimp_procedure_set_image_types (procedure, "*");
    }

  return procedure;
}

static GimpValueArray *
hello_run (GimpProcedure *procedure,
          GimpRunMode run_mode,
          GimpImage *image,
          GimpDrawable **drawables,
          GimpProcedureConfig *config,
          gpointer run_data)
{
  gimp_message ("Hello, world!");
  return gimp_procedure_new_return_values (procedure, GIMP_PDB_SUCCESS, NULL);
}

int
main (int argc, char *argv[])
{
  return gimp_main (hello_get_type (), argc, argv);
}