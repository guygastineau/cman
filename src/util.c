#include "util.h"

#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

static size_t quick_copy(const char *from, char **dest)
{
  assert(dest && "buffer_for: `dest' can't be NULL!");
  if (!from) {
    return 0;
  }
  size_t len = strlen(from);
  *dest = malloc(len + 1);
  if (!dest) {
    return 0;
  }
  // Copy the bytes;
  memcpy(*dest, from, len + 1);
  return len;
}

static void upcase_mut(size_t len, char *buff)
{
  if (!len) {
    return;
  }
  assert(buff && "string_upcase_mut: buff cannot be NULL");
  for (size_t i = 0; i < len; ++i) {
    buff[i] = toupper(buff[i]);
  }
}

char *string_upcase(const char *from)
{
  char *to;
  size_t len = quick_copy(from, &to);
  upcase_mut(len, to);
  return to;
}

static void kebabify_mut(size_t len, char *buff)
{
  if (!len) {
    return;
  }
  assert(buff && "slugify_mut: buff must not be NULL");
  for (size_t i = 0; i < len; ++i) {
    if (buff[i] == '_') {
      buff[i] = '-';
    }
  }
}

char *string_kebabify(const char *from)
{
  char *to;
  size_t len = quick_copy(from, &to);
  kebabify_mut(len, to);
  return to;

}

static void snakify_mut(size_t len, char *buff)
{
  // buff can be NULL if size is 0.
  if (!len) {
    return;
  }
  assert(buff && "snakify_mut: `buff' can't be NULL!");
  for (size_t i = 0; i < len; ++i) {
    if (buff[i] == '-') {
      buff[i] = '_';
    }
  }
}

char *string_snakify(const char *from)
{
  char *to;
  size_t len = quick_copy(from, &to);
  snakify_mut(len, to);
  return to;
}

char *to_header_name(const char *from)
{
  char *to;
  size_t len = quick_copy(from, &to);
  upcase_mut(len, to);
  snakify_mut(len, to);
  return to;
}
