#include "test.h"

#include <stdio.h>
#include <string.h>
#include <stddef.h>

#include <sys/stat.h>
#include <errno.h>

static char path[1024];
static char file[1024];

static const char *test_template =
  "#include \"vendor/unity.h\"\n"
  "#include \"../src/%s.h\"\n"
  "\n"
  "void setUp(void)\n"
  "{\n"
  "}\n"
  "\n"
  "void tearDown(void)\n"
  "{\n"
  "}\n"
  "\n"
  "static void test_lib(void)\n"
  "{\n"
  "  TEST_EQUAL(\"Hello, World!\", hello());\n"
  "}\n"
  "\n"
  "int main(void)\n"
  "{\n"
  "   UnityBegin(\"test/test_%s.c\");\n"
  "\n"
  "   RUN_TEST(test_lib);\n"
  "\n"
  "   return UnityEnd();\n"
  "}\n";

int write_test(const char *testdir, const char *fname, const char *project)
{
  if (snprintf(file, 1024, "%s/%s", testdir, fname) < 0) {
    return -1;
  }

  FILE *dest;
  dest = fopen(file, "wb");
  if (dest == NULL) {
    fprintf(stderr, "cman error: couldn't open %s\n", fname);
    return -1;
  }
  fprintf(dest, test_template, project, project);
  fclose(dest);
  return 0;
}

const char *mk_testdir(const char *root)
{
  if (snprintf(path, 1024, "%s/%s", root, "test") < 0) {
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
