#include "lib.h"

#include "util.h"

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <errno.h>

static char path[1024];
static char file[1024];

static const char *lib_h_template =
  "#ifndef %s_H\n"
  "#define %s_H\n"
  "\n"
  "const char *hello();\n"
  "\n"
  "#endif // %s_H\n";

static const char *lib_c_template =
  "#include \"%s.h\"\n"
  "\n"
  "const char *hello()\n"
  "{\n"
  "  return \"Hello, World!\";\n"
  "}\n";

const char *mk_libdir(const char *root)
{
  if (snprintf(path, 1024, "%s/%s", root, "src") < 0) {
    return NULL;
  }
  if (mkdir(path, 0777)) {
    switch (errno) {
    case EEXIST:
      break;
    default:
      return NULL;
    }
  }
  return path;
}

int write_lib(const char *project)
{
  const char *libdir = mk_libdir(project);
  if (!libdir) {
    return -1;
  }

  // First write the header file.
  FILE *dest;

  // Make the header file name.
  if (snprintf(file, 1024, "%s/%s.h", libdir, project) < 0) {
    return -1;
  }

  // Get upcased project name to use for header guard.
  char *hname = to_header_name(project);
  if (!hname) {
    fprintf(stderr,
            "cman error: internal error creating "
            "upper case string from '%s'\n",
            project);
    return -1;
  }

  // Open and verify file.
  dest = fopen(file, "wb");
  if (dest == NULL) {
    fprintf(stderr, "cman error: couldn't open %s.h\n", project);
    return -1;
  }
  // Writ ethe header.
  fprintf(dest, lib_h_template, hname, hname, hname);

  // Clean up.
  fclose(dest);
  free(hname);

  // Now write the c file.

  // Make the implementation file name.
  if (snprintf(file, 1024, "%s/%s.c", libdir, project) < 0) {
    return -1;
  }

  // Open and verify file.
  dest = fopen(file, "wb");
  if (dest == NULL) {
    fprintf(stderr, "cman error: couldn't open %s.c\n", project);
    return -1;
  }
  // Write the implementation.
  fprintf(dest, lib_c_template, project);

  // Clean up.
  fclose(dest);

  return 0;
}
