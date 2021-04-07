#include "util.h"

#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>

char *string_upcase(const char *from)
{
  if (!from) {
    return NULL;
  }
  // Add + 1 for '\0'
  size_t size = strlen(from) + 1;
  char *to = malloc(size);
  if (!to) {
    return NULL;
  }
  // Since we loop through the full size of the strings
  // we get the NULL terminator copied in the loop ;)
  for (size_t i = 0; i < size; ++i) {
    to[i] = toupper(from[i]);
  }
  return to;
}
