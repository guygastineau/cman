#include "files.h"

#include <stdio.h>
#include <string.h>
#include <stddef.h>

#include <sys/stat.h>
#include <errno.h>

static char file[1024];

const char *main_contents =
  "#include \"src/%s.h\"\n"
  "\n"
  "#include <stdio.h>\n"
  "\n"
  "// Simple stub file for binary projects.\n"
  "// Hint: you should erase these comments ;)\n"
  "\n"
  "int main()\n"
  "{\n"
  "  printf(\"%s\\n\", hello());\n"
  "}\n";

const char *gitignore =
  "# Binaries\n"
  "%s\n"
  "/bin/\n"
  "\n"
  "*.o\n"
  "*.a\n"
  "*.out\n"
  "\n"
  "# For Emacs\n"
  "*~\n"
  "\\#*\\#\n"
  ".\\#*\n";

int write_files(const char *project)
{
  FILE *dest;

  // First write main.

  if (snprintf(file, 1024, "%s/main.c", project) < 0) {
    return -1;
  }

  // Open and verify file.
  dest = fopen(file, "wb");
  if (dest == NULL) {
    fprintf(stderr, "cman error: couldn't open %s/main.c\n", project);
    return -1;
  }
  // Writ ethe header.
  fprintf(dest, main_contents, project, "%s");

  // Clean up.
  fclose(dest);

  // Write .gitignore

  if (snprintf(file, 1024, "%s/.gitignore", project) < 0) {
    return -1;
  }

  // Open and verify file.
  dest = fopen(file, "wb");
  if (dest == NULL) {
    fprintf(stderr, "cman error: couldn't open %s/.gitignore\n", project);
    return -1;
  }
  // Writ ethe header.
  fprintf(dest, gitignore, project);

  fclose(dest);

  return 0;
}
