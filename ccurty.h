#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/time.h>
#include <assert.h>

#ifndef CCURTY
#define CCURTY

// Random stuff

#define unimplemented(msg) {\
  fprintf(stderr, "Unimplemented: %s\n", msg);\
  exit(1); \
} \

#define array_length(arr) (sizeof((arr))/sizeof((arr)[0]))

// Array

#define for_n(n) \
  for (int i = 0; i< n;i++)

#define foreach(arr) \
  for (size_t i = 0; i < array_length(arr); i++)

// Dynamically re-sized array

#define d_ensure(arr, c)                                       \
  do {                                                         \
      (arr)->capacity = c;                                     \
      size_t size = (arr)->capacity * sizeof((arr)->items[0]); \
      (arr)->items = realloc((arr)->items, size);              \
      if ((arr)->count > c) (arr)->count = c;                  \
  } while (0)

#define d_append(arr, item)                                               \
  do {                                                                    \
    if ((arr)->count >= (arr)->capacity) {                                \
      size_t capacity = (arr)->capacity == 0 ? 256 : (arr)->capacity * 2; \
      d_ensure(arr, capacity);                                            \
     }                                                                    \
    (arr)->items[(arr)->count++] = (item);                                \
  } while(0)

#define d_append_n(arr, new_items, new_items_count)                       \
  do {                                                                    \
    if ((arr)->count + new_items_count >= (arr)->capacity) {              \
      size_t capacity = (arr)->capacity == 0 ? 256 : (arr)->capacity * 2; \
      d_ensure(arr, capacity);                                            \
    }                                                                     \
    size_t size = sizeof((arr)->items[0]);                                \
    memcpy((arr)->items + (arr)->count, new_items, new_items_count*size); \
    (arr)->count += new_items_count;                                      \
  } while(0)

#define d_concat(arr, arr2)                                                 \
  do {                                                                      \
    if ((arr)->count + (arr2)->count >= (arr)->capacity) {                  \
      size_t capacity = (arr)->capacity == 0                                \
        ? (arr2)->capacity                                                  \
        : ((arr)->capacity + (arr2)->capacity) * 2;                         \
      d_ensure(arr, capacity);                                              \
    }                                                                       \
    size_t size = sizeof((arr)->items[0]);                                  \
    memcpy((arr)->items + (arr)->count, (arr2)->items, (arr2)->count*size); \
    (arr)->count += (arr2)->count;                                          \
  } while(0)

#define d_insert(arr, item, index)                                          \
  do {                                                                      \
    if ((arr)->count + 1 >= (arr)->capacity) {                              \
      size_t capacity = (arr)->capacity == 0 ? 256 : (arr)->capacity * 2;   \
      d_ensure(arr, capacity);                                              \
    }                                                                       \
    size_t size = ((arr)->count - index) * sizeof((arr)->items[0]);         \
    memmove((arr)->items + index + 1, (arr)->items + index, size);          \
    (arr)->items[index] = (item);                                           \
    (arr)->count++;                                                         \
  } while (0)

#define d_insert_n(arr, arr2, insert_count, index)                            \
  do {                                                                        \
    if ((arr)->capacity + insert_count >= (arr)->capacity) {                  \
      size_t capacity = (arr)->capacity == 0                                  \
        ? 256                                                                 \
        : (arr)->capacity + insert_count * 2;                                 \
      d_ensure(arr, capacity);                                                \
    }                                                                         \
    size_t size = insert_count * sizeof((arr)->items[0]);                     \
    memmove((arr)->items + index + insert_count, (arr)->items + index, size); \
    memcpy((arr)->items + index, arr2, size); \
    (arr)->count += insert_count; \
  } while (0)

#define d_foreach(arr) for (size_t i = 0; i < (arr)->count; i++)

#define d_pop(arr)  (arr)->count--;

#define d_delete(arr, i)                                       \
  do {                                                         \
    (arr)->count--;                                            \
    int item_size = sizeof((arr)->items[0]);                   \
    int cpy_size = item_size * ((arr)->count - i);             \
    memmove((arr)->items + i, (arr)->items + i + 1, cpy_size); \
  } while(0)                                                   \

#define d_delete_n(arr, i, cnt)                                 \
  do {                                                          \
    (arr)->count -= cnt;                                        \
    int item_size = sizeof((arr)->items[0]);                    \
    int cpy_size = item_size * ((arr)->count - i);              \
    memmove((arr)->items + i, (arr)->items + i + cnt, cpy_size);\
  } while(0)                                                    \


#define d_shift(arr) d_delete(arr, 0);

#define d_free(arr) free((arr)->items);

// HashTable (open addressed)
// This is not as fast as I'd like it to be in some instances - consider
// profiling against buckets of linked lists for slots instead of open
// addressing

typedef struct {
  char *key;
  void *value;
} HashKV;

typedef struct {
  HashKV *items;
  size_t count;
  size_t capacity;
} HashTable;

// DBJ
size_t hash(char *key, size_t size) {
  unsigned long hash = 5381;
  for_n((int)size) {
    hash = key[i] + ((hash << 5) ^ hash);
  }
  return hash;
}

HashTable hashtable_init(size_t capacity) {
  HashTable table = {0};
  table.capacity = capacity;
  table.count = 0;

  int size = sizeof(HashKV)*table.capacity;
  table.items = (HashKV *)malloc(size);
  memset(table.items, 0, size);

  return table;
}

void hashtable_free(HashTable *table) {
  free(table->items);
}

// TODO handle overflow
void hashtable_insert(HashTable *table, char *key, size_t size, void* value) {
  size_t h = hash(key, size)%table->capacity;

  while (table->items[h].key != NULL && strcmp(table->items[h].key , key)) {
    if (h == table->capacity) {
      unimplemented("HashTable overflow");
    }
    h++;
  }

  table->items[h] = (HashKV) {
    .key = key,
    .value = value,
  };
  table->count++;
}

void *hashtable_get(HashTable *table, char *key, size_t size) {
  if (key == NULL) return NULL;

  size_t h = hash(key, size)%table->capacity;

  if (table->items[h].key == NULL) return NULL;

  while (strcmp(table->items[h].key, key) != 0) {
    if (h == table->capacity) return NULL;
    h++;
  }

  return table->items[h].value;
}

bool hashtable_delete(HashTable *table, char *key, size_t size) {
  if (key == NULL) return false;

  size_t h = hash(key, size)%table->capacity;

  if (table->items[h].key == NULL) return false;

  while (strcmp(table->items[h].key, key) != 0) {
    if (h == table->capacity) return false;
    h++;
  }

  table->items[h].key = NULL;
  table->items[h].value = NULL;
  table->count--;

  return true;
}

// Logging

typedef enum {
  DEBUG,
  INFO,
  WARN,
} LOG_LEVELS;

#ifndef LOG_LEVEL
#define LOG_LEVEL INFO
#endif

#define log(level, msg) \
  if (level >= LOG_LEVEL) printf(msg);

#define marker(c, str) c str "\033[0m"
#define marker_black(str) marker("\033[1;30m", str)
#define marker_red(str) marker("\033[1;31m", str)
#define marker_green(str) marker("\033[1;32m", str)
#define marker_yellow(str) marker("\033[1;33m", str)
#define marker_blue(str) marker("\033[1;34m", str)
#define marker_magenta(str) marker("\033[1;35m", str)
#define marker_cyan(str) marker("\033[1;36m", str)

#define marker_bold(str) marker("\033[1;5m", str)

// Testing

#define test_suite(suite) \
  char *_test_name = ""; \
  log(INFO, marker_bold("TEST SUITE: " marker_cyan(suite"\n"))); \

#define test_time_start() \
  struct timeval _test_t1; \
  gettimeofday(&_test_t1, NULL) \

#define test_time_end() \
  do { \
    struct timeval t2; \
    gettimeofday(&t2, NULL); \
    double elapsedTime = (t2.tv_sec - _test_t1.tv_sec) * 1000.0; \
    elapsedTime += (t2.tv_usec - _test_t1.tv_usec) / 1000.0; \
    printf(marker_bold("\nAll test completed in %.3lfms\n"), elapsedTime); \
  } while(0) \

#define test(name)     \
  do {                 \
    _test_name = name; \
  } while(0);          \

#define expect(cond)                          \
  do {                                        \
    printf("  %s... ", _test_name);           \
    if ((cond) == 0) {                        \
      printf("%s", marker_red("FAILED\n"));   \
      exit(1);                                \
    } else {                                  \
      printf("%s", marker_green("PASSED\n")); \
    }                                         \
  } while(0);                                 \

// Dynamic strings

typedef struct {
    char *items;
    size_t count;
    size_t capacity;
} StringBuilder;

#define sb_append_char(sb, ch) d_append(sb, ch)
#define sb_append(sb, str) d_append_n(sb, str, strlen(str))
#define sb_concat(sb, sb2) d_concat(sb, sb2);
#define sb_insert(sb, ch, index) d_insert(sb, ch, index)
#define sb_insert_n(sb, ch, len, index) d_insert_n(sb, ch, len, index)
#define sb_append_null(sb) d_append(sb, '\0')
#define sb_free(sb) free((sb)->items)

// Files

//TODO: error handle lol
char *read_entire_file(const char *path) {
  FILE *f = fopen(path, "rb");
  fseek(f, 0, SEEK_END);

  long fsize = ftell(f);
  fseek(f, 0, SEEK_SET);

  char *contents = (char *)malloc(fsize + 1);
  fread(contents, fsize, 1, f);

  fclose(f);

  contents[fsize] = 0;

  return contents;
}

#endif //CCURTY

