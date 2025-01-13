#include <stdint.h>
#include <stdio.h>
#include "./ccurty.h"


typedef struct {
  size_t capacity;
  size_t count;
  int *items;
} List;

void test_arrays(void) {
  test_suite("Arrays");
  test("for_n iterates the correct number of times") {;
    int cnt = 0;

    for_n(10) {
      cnt++;
    }

    expect(cnt == 10);
  }

  test("foreach iterates the correct number and provides the correct index") {
    int ar[] = {2, 4, 6, 8};
    int cnt = 0;
    int total = 0;

    foreach(ar) {
      cnt++;
      total += ar[i];
    }

    expect(cnt == 4 && total == 20);
  }


  test("d_append sets count correctly") {
    List list = {0};
    for_n(1000) {
      d_append(&list, i+2);
    }

    expect(list.count == 1000);

    d_free(&list);
  }

  test("d_append_n appends items correctly") {
    List list= {0};
    d_append(&list, 1);

    int new[] = {2, 3, 4};
    d_append_n(&list, &new, 3);

    expect(
        list.items[0]== 1 &&
        list.items[1]== 2 &&
        list.items[2]== 3 &&
        list.items[3]== 4
    );

    d_free(&list);
  }

  test("d_concat appends items correctly") {
    List list     = {0};
    List list_two = {0};

    int new[]     = {1, 2, 3};
    int new_two[] = {8, 7, 6};

    d_append_n(&list, &new, 3);
    d_append_n(&list_two, &new_two, 3);

    d_concat(&list, &list_two);

    expect(
        list.count == 6 &&
        list.items[0]== 1 &&
        list.items[1]== 2 &&
        list.items[2]== 3 &&
        list.items[3]== 8 &&
        list.items[4]== 7 &&
        list.items[5]== 6
    );

    d_free(&list);
    d_free(&list_two);
  }

  test("d_insert appends items correctly") {
    List list     = {0};
    int new[]     = {1, 2, 4};

    d_append_n(&list, &new, 3);

    d_insert(&list, 3, 2);

    expect(
        list.count == 4 &&
        list.items[0]== 1 &&
        list.items[1]== 2 &&
        list.items[2]== 3 &&
        list.items[3]== 4
    );

    d_free(&list);
  }

  test("d_insert_n appends items correctly") {
    List list     = {0};
    int new[]     = {1, 2, 6};

    d_append_n(&list, &new, 3);

    int insert_items[] = {3, 4, 5};

    d_insert_n(&list, &insert_items, 3, 2);

    expect(
        list.count == 6 &&
        list.items[0]== 1 &&
        list.items[1]== 2 &&
        list.items[2]== 3 &&
        list.items[3]== 4 &&
        list.items[4]== 5 &&
        list.items[5]== 6
    );

    d_free(&list);
  }

  test("d_ensure sets count correctly") {
    List list = {0};
    d_append(&list, 1);
    d_append(&list, 1);
    d_append(&list, 1);
    d_append(&list, 1);

    d_ensure(&list, 3);
    expect(list.count == 3);

    d_free(&list);
  }

  test("d_pop removes the last element") {
    List list = {0};
    int items[] = {1, 5, 10};
    d_append_n(&list, &items, 3);

    d_pop(&list);
    expect(list.count == 2 && list.items[list.count -1] == 5);

    d_free(&list);
  }


  test("d_foreach iterates and provides the correct indexes") {
    int cnt = 0;
    int total = 0;

    List list = {0};
    int items[] = {1, 3, 5, 7, 9};
    d_append_n(&list, &items, 5);
    d_foreach(&list) {
      cnt++;
      total += list.items[i];
    }

    expect(cnt == 5 &&  total == 25);

    d_free(&list);
  }

  test("d_delete removes the correct elements and sets count") {
    List list = {0};
    int items[] = {1, 3, 5, 7, 9, 11, 16, 19};

    d_append_n(&list, &items, 8);
    d_delete(&list, 3);
    d_delete(&list, 1);

    expect(
      list.count == 6   &&
      list.items[0]== 1 &&
      list.items[1]== 5 &&
      list.items[2]== 9 &&
      list.items[3]== 11
    );

    d_free(&list);
  }

  test("d_delete_n removes the correct elements and sets count") {
    List list = {0};
    int items[] = {1, 2, 3, 4, 5};

    d_append_n(&list, &items, 5);
    d_delete_n(&list, 1, 2);

    expect(
      list.count == 3   &&
      list.items[0]== 1 &&
      list.items[1]== 4 &&
      list.items[2]== 5
    );

    d_free(&list);
  }



  test("d_shift removes the first element and sets count") {
    List list = {0};
    int items[] = {1, 3, 5, 7 };
    d_append_n(&list, &items, 4);
    d_shift(&list);

    expect(
      list.count == 3   &&
      list.items[0]== 3 &&
      list.items[1]== 5 &&
      list.items[2]== 7
    );

    d_free(&list);
  }
}

void test_hashtable(void) {
  test_suite("HashTable");
  test("insert and get") {
    HashTable table = hashtable_init(256);

    char *key = "testing";
    char *key2= "lol";
    hashtable_insert(&table, key, strlen(key), (void *)true);
    hashtable_insert(&table, key2, strlen(key2), (void *)false);

    int result = (intptr_t)hashtable_get(&table, key, strlen(key));
    int result2= (intptr_t)hashtable_get(&table, key2, strlen(key2));
    void *maybe_result = hashtable_get(&table, "l", true);

    hashtable_free(&table);

    expect(result == true && result2 == false && maybe_result == NULL);
  }

  test("delete") {
    HashTable table = hashtable_init(256);

    char *key = "testing";
    hashtable_insert(&table, key, strlen(key), (void *)true);

    int result = (intptr_t)hashtable_get(&table, key, strlen(key));

    bool was_deleted = hashtable_delete(&table, key, strlen(key));

    void* null_result = hashtable_get(&table, key, strlen(key));

    hashtable_free(&table);

    expect(result == true && was_deleted == true && null_result == NULL);
  }

  test("collisions") {
    HashTable table = hashtable_init(4);
    char* keys[] = {"ab", "ba", "c", "d"};

    foreach(keys) {
      hashtable_insert(&table, keys[i], strlen(keys[i]), (void *)i);
    }

    int result = (intptr_t)hashtable_get(&table, keys[3], 1);

    hashtable_free(&table);

    expect(result == 3);
  }

  test("overwrite") {
    HashTable table = hashtable_init(256);
    char *key_one = "test";
    char *key_two = "other";

    hashtable_insert(&table, key_one, strlen(key_one), (void *)1);
    hashtable_insert(&table, key_two, strlen(key_two), (void *)2);

    hashtable_insert(&table, key_one, strlen(key_one), (void *)3);

    int result = (intptr_t)hashtable_get(&table, key_one, strlen(key_one));

    hashtable_free(&table);

    expect(result == 3);
  }

  test("complex types") {
    HashTable table = hashtable_init(256);
    char *key_one = "test";
    char *key_two = "other";

    typedef struct {
      int list[4];
      size_t count;
    } Value;

    Value v1 = { .list= {1, 3, 5, 7}, .count = 4 };
    Value v2 = { .list= {2, 4, 6, 8}, .count = 4 };

    hashtable_insert(&table, key_one, strlen(key_one), (void *)&v1);
    hashtable_insert(&table, key_two, strlen(key_two), (void *)&v2);

    Value *result_one = (Value *)hashtable_get(&table, key_one, strlen(key_one));
    Value *result_two = (Value *)hashtable_get(&table, key_two, strlen(key_two));

    hashtable_free(&table);

    expect(
      result_one->list[0] == 1 &&
      result_one->list[3] == 7 &&
      result_one->count   == 4 &&
      result_two->list[0] == 2 &&
      result_two->list[3] == 8 &&
      result_two->count   == 4
    );
  }


}

void test_string_builder(void) {
  test_suite("StringBuilder");

  test("sb_append_char") {
    StringBuilder sb = {0};
    sb_append_char(&sb, 't');
    sb_append_char(&sb, 'e');
    sb_append_char(&sb, 's');
    sb_append_char(&sb, 't');
    sb_append_null(&sb);

    expect(strcmp(sb.items, "test") == 0);

    sb_free(&sb);
  }

  test("sb_append") {
    StringBuilder sb = {0};
    sb_append(&sb, "test");

    expect(strcmp(sb.items, "test") == 0);

    sb_free(&sb);
  }

  test("sb_concat") {
    StringBuilder sb = {0};
    sb_append(&sb, "hello ");

    StringBuilder sb2 = {0};
    sb_append(&sb2, "world!");

    sb_concat(&sb, &sb2);

    expect(strcmp(sb.items, "hello world!") == 0);

    sb_free(&sb);
    sb_free(&sb2);
  }

  test("sb_insert") {
    StringBuilder sb = {0};

    sb_append(&sb, "hllo");
    sb_insert(&sb, 'e', 1);

    expect(strcmp(sb.items, "hello") == 0);

    sb_free(&sb);
  }

  test("sb_insert_n") {
    StringBuilder sb = {0};

    sb_append(&sb, "hlo");
    sb_insert_n(&sb, "el", 2, 1);

    expect(strcmp(sb.items, "hello") == 0);

    sb_free(&sb);
  }

}

int main(void) {
    test_time_start();
    test_arrays();
    test_hashtable();
    test_string_builder();
    test_time_end();

    return 0;
}
