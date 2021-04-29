#include "lib.h"

static const char *lib_c_template =
  "#include \"%s.h\"\n"
  "\n"
  "const char *hello()\n"
  "{\n"
  "  return \"Hello, World!\";\n"
  "}\n";

static const char *lib_h_template =
  "#ifndef %s_H\n"
  "#define %s_H\n"
  "\n"
  "const char *hello();\n"
  "\n"
  "#endif // %s_H\n";

const char *mk_libdir(const char *root)
{
  return root;
}

int write_lib(const char *libdir, const char *fname, const char *project)
{
  return (int)(*libdir + *fname + *project);
}
