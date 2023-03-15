#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include "error_reporting.h"

#define BUFFER_LENGTH 1024
#define BIG_BUFFER_LENGTH 2048

void test_nominal_use_case() {
    long line;
    long id;
    char message[LAST_ERROR_MESSAGE_BUFFER_CAPACITY];

    // Initialise the library.
    last_error_init();
    assert(0 == strcmp("", last_error_get_file()));
    assert(0 == strcmp("", last_error_get_message()));
    assert(0 == strcmp("", last_error_get_function()));
    assert(-1 == last_error_get_line());
    assert(-1 == last_error_get_id());

    // Report an error.
    id = 10;
    line = __LINE__;
    assert(1 == last_error_set(id, __FILE__, __LINE__, __func__, "%d + %d", 10, 20));
    assert(id == last_error_get_id());
    assert(line+1 == last_error_get_line());
    assert(0 == strcmp(__FILE__, last_error_get_file()));
    snprintf(message, LAST_ERROR_MESSAGE_BUFFER_CAPACITY, "#%010ld [%s:%ld %s()] %s", id, __FILE__, line+1, __func__, "10 + 20");
    assert(0 == strcmp(message, last_error_get_message()));
    assert(0 == strcmp(__func__, last_error_get_function()));
}

void test_buffer_overflow_message() {
    char   *message;
    char   temp[BUFFER_LENGTH];
    size_t message_len;
    size_t prefix_len;
    long   id = 10;
    long   line;

    // Calculate the length `message_len` of an error message that will be too long, by just one byte.
    // Hypothesis: `LAST_ERROR_MESSAGE_BUFFER_CAPACITY` = 200
    //             `prefix_len` = 100 characters (not including the final zero).
    // Thus:
    //      `message_len` = 99 characters (not including the final zero).
    //      `message_len` is the maximum number of characters (not including the final zero).
    //      => we need to create a message that has `message_len+1` characters
    //      => we need to allocate `message_len+2` bytes for the message.

    line = __LINE__;
    snprintf(temp, BUFFER_LENGTH, "#%010ld [%s:%ld %s()] ", id, __FILE__, line, __func__);
    prefix_len  = strlen(temp);
    message_len = LAST_ERROR_MESSAGE_BUFFER_CAPACITY - prefix_len;

    // Initialise the library.
    last_error_init();
    assert(0 == strcmp("", last_error_get_file()));
    assert(0 == strcmp("", last_error_get_message()));
    assert(0 == strcmp("", last_error_get_function()));
    assert(-1 == last_error_get_line());
    assert(-1 == last_error_get_id());

    // Build a message which is too long, by just one byte.
    message = (char*)malloc(sizeof(char) * (message_len + 2));
    assert(NULL != message);
    memset((void*)message, 'a', message_len);
    message[message_len] = 'x';
    message[message_len+1] = 0;

    // Try to report the error.
    assert(0 == last_error_set(id, __FILE__, line, __func__, "%s", message));
    assert(0 == strcmp("", last_error_get_message()));
}

void test_just_long_enough_message() {
    char   *message;
    char   temp[BUFFER_LENGTH];
    char   expected_message[BIG_BUFFER_LENGTH];
    size_t message_len;
    size_t prefix_len;
    long id = 10;
    long line;

    // Calculate the length `message_len` of an error message that will be too long, by just one byte.
    // Hypothesis: `LAST_ERROR_MESSAGE_BUFFER_CAPACITY` = 200
    //             `prefix_len` = 100 characters (not including the final zero).
    // Thus:
    //      `message_len` = 200 - 100 - 1 (-1 because we need to add one zero to the end of the string)
    //      `message_len` = 99 characters (not including the final zero).
    //      `message_len` is the maximum number of characters (not including the final zero !!!).
    //      => we need to create a message that has `message_len` characters
    //      => we need to allocate `message_len+1` bytes for the message.

    line = __LINE__;
    snprintf(temp, BUFFER_LENGTH, "#%010ld [%s:%ld %s()] ", id, __FILE__, line, __func__);
    prefix_len  = strlen(temp);
    message_len = LAST_ERROR_MESSAGE_BUFFER_CAPACITY - prefix_len - 1; // number of characters (not including the final zero !!!).

    // Initialise the library.
    last_error_init();
    assert(0 == strcmp("", last_error_get_file()));
    assert(0 == strcmp("", last_error_get_message()));
    assert(0 == strcmp("", last_error_get_function()));
    assert(-1 == last_error_get_line());
    assert(-1 == last_error_get_id());

    // Build a message which is just long enough.
    message = (char*)malloc(sizeof(char) * (message_len + 1));
    assert(NULL != message);
    memset((void*)message, 'a', message_len-1);
    message[message_len-1] = 'x';
    message[message_len] = 0;

    snprintf(expected_message, BIG_BUFFER_LENGTH, "%s%s", temp, message);

    // Try to report the error.
    assert(1 == last_error_set(id, __FILE__, line, __func__, "%s", message));
    assert(0 == strcmp(expected_message, last_error_get_message()));
    assert(0 == strcmp(__FILE__, last_error_get_file()));
    assert(0 == strcmp(__func__, last_error_get_function()));
    assert(line == last_error_get_line());
    assert(id == last_error_get_id());
}

void test_empty_message() {
    char   message[1] = { 0 };
    char   temp[BUFFER_LENGTH];
    char   expected_message[BIG_BUFFER_LENGTH];
    long   id = 10;
    long   line;

    line = __LINE__;
    snprintf(temp, BUFFER_LENGTH, "#%010ld [%s:%ld %s()] ", id, __FILE__, line, __func__);

    // Initialise the library.
    last_error_init();
    assert(0 == strcmp("", last_error_get_file()));
    assert(0 == strcmp("", last_error_get_message()));
    assert(0 == strcmp("", last_error_get_function()));
    assert(-1 == last_error_get_line());
    assert(-1 == last_error_get_id());

    snprintf(expected_message, BIG_BUFFER_LENGTH, "%s%s", temp, message);

    // Try to report the error.
    assert(1 == last_error_set(id, __FILE__, line, __func__, "%s", message));
    assert(0 == strcmp(expected_message, last_error_get_message()));
    assert(0 == strcmp(__FILE__, last_error_get_file()));
    assert(0 == strcmp(__func__, last_error_get_function()));
    assert(line == last_error_get_line());
    assert(id == last_error_get_id());
}

int main() {
    test_nominal_use_case();
    test_buffer_overflow_message();
    test_just_long_enough_message();
    test_empty_message();
    return 0;
}