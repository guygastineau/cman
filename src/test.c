#include "test.h"

#include <stdio.h>
#include <string.h>
#include <stddef.h>

static const char *test_template = "\
#include \"vendor/unity.h\"
#include \"../src/%s.h\"

void setUp(void)
{
}

void tearDown(void)
{
}

static void test_lib(void)
{
  TEST_EQUAL(\"Hello, World!\", hello());
}

int main(void)
{
   UnityBegin(\"test/test_%s.c\");

   RUN_TEST(test_lib);

   return UnityEnd();
}
"

int write_test(const char *fname, const char *project)
{
  FILE *dest;
  dest = fopen(fname, "wb");
  if (dest == NULL) {
    fprintf(stderr, "cman error: couldn't open %s\n", fname);
    return 1;
  }
  fprintf(dest, test_template, project, project);
  fclose(dest);
  return 0;
}
