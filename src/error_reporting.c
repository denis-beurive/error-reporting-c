/**
 * @file error_reporting.c
 *
 * This file implements a simple, yet pretty decent, error reporting system.
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "error_reporting.h"

/**
 * The buffer used to store the path to the file from which the error was raised.
 */
static char LAST_ERROR_FILE[LAST_ERROR_FILE_BUFFER_CAPACITY];

/**
 * The buffer used to store the message that describes the error.
 */

static char LAST_ERROR_MESSAGE[LAST_ERROR_MESSAGE_BUFFER_CAPACITY];

/**
 * The buffer used to store the name of the function from which the error was raised.
 */
static char LAST_ERROR_FUNCTION[LAST_ERROR_FUNCTION_BUFFER_CAPACITY];

/**
 * @brief The ID of the last error.
 * @note Each error should be identifier by a unique ID.
 */
static long LAST_ERROR_ID;

/**
 * @brief The line number (relatively to the file which path is `LAST_ERROR_FILE`) of the line at which the error was raised.
 */
static long LAST_ERROR_LINE;

/**
 * @brief Initialize the reporting system.
 */

void
last_error_init() {
    LAST_ERROR_ID = -1;
    LAST_ERROR_LINE = -1;
    LAST_ERROR_MESSAGE[0] = 0;
    LAST_ERROR_FILE[0] = 0;
    LAST_ERROR_FUNCTION[0] = 0;
}

/**
 * @brief Return the ID of the last error.
 * @return The ID of the last error.
 */

long
last_error_get_id() {
    return LAST_ERROR_ID;
}

/**
 * @brief Return the line number of the code which produced the error.
 * @return The line number of the code which produced the error.
 */

long
last_error_get_line() {
    return LAST_ERROR_LINE;
}

/**
 * @brief Return the message associated with the error.
 * @return The message associated with the error.
 */

const char *
last_error_get_message() {
    return LAST_ERROR_MESSAGE;
}

/**
 * @brief Return the path to the file where the error occurred.
 * @return The path to the file where the error occurred.
 * @note The returned pointer references a statically allocated memory location.
 * Do not call the function `free()` on this pointer!
 */

const char *
last_error_get_file() {
    return LAST_ERROR_FILE;
}

/**
 * @brief return the name of the function that caused the error.
 * @return The name of the function that caused the error.
 * @note The returned pointer references a statically allocated memory location.
 * Do not call the function `free()` on this pointer!
 */

const char *
last_error_get_function() {
    return LAST_ERROR_FUNCTION;
}

/**
 * @brief Set the last error.
 * @param in_error_id An integer that (hopefully) uniquely identifies the error.
 * @param in_file The path to the (C) file that contains the code that raised the error.
 * @param in_line The number of the line, within `in_file`, where the error was raised.
 * @param in_function The name of the function that raised the error.
 * @param in_fmt The format descriptor.
 * @param ... The list of arguments.
 * @return Upon successful completion, the function returns the value 1.
 * Otherwise, it returns the value 0. This means that the buffer used to store the error message is not
 * big enough. However, the content of the buffer used to store the error message is always guaranteed
 * to be zero terminated.
 * @note The return value is likely to be ignored by the calling code. Indeed, this function is intended
 * to be used to store information about an error. We assume that it will do its job just fine.
 */

int
last_error_set(
        const long in_error_id,
        const char *in_file,
        const long in_line,
        const char *in_function,
        const char *in_fmt,
        ...) {
    int    bytes_written      = 0;
    size_t remaining_capacity = LAST_ERROR_MESSAGE_BUFFER_CAPACITY;

    // Copy the error ID, the line number and the name of the source file.
    LAST_ERROR_ID = in_error_id;
    LAST_ERROR_LINE = in_line;
    strncpy(LAST_ERROR_FILE, in_file, LAST_ERROR_FILE_BUFFER_CAPACITY);
    LAST_ERROR_FILE[LAST_ERROR_FILE_BUFFER_CAPACITY-1] = 0;
    strncpy(LAST_ERROR_FUNCTION, in_function, LAST_ERROR_FUNCTION_BUFFER_CAPACITY);
    LAST_ERROR_FUNCTION[LAST_ERROR_FUNCTION_BUFFER_CAPACITY-1] = 0;

    // See the comment for the function `vsnprintf()`. `snprintf` behaves the same way.
    // If the buffer is big enough, then `bytes_written` is the number of bytes actually written,
    // **excluding the final zero character**.
    bytes_written = snprintf(LAST_ERROR_MESSAGE,
                             remaining_capacity,
                             "#%010ld [%s:%ld %s()] ", in_error_id, in_file, in_line, in_function);
    if ((bytes_written < 0) || (bytes_written >= remaining_capacity)) {
        LAST_ERROR_MESSAGE[0] = 0;
        return 0;
    }
    remaining_capacity -= (size_t)bytes_written;

    // NOTE:
    //
    // The function `vsnprintf(char *str, size_t size, const char *format, va_list ap)`
    // writes at most `size` bytes (including the terminating null byte.
    //
    // The function `vsnprintf()` does not write more than `size` bytes
    // (including the terminating null byte ('\0')). If the output was truncated
    // due to this limit then the return value is the number of characters (excluding
    // the terminating null byte) which would have been written to the final string if
    // enough space had been available. Thus, a return value of `size` or more means that
    // the output was truncated.

    va_list arg_ptr;
    va_start(arg_ptr, in_fmt);
    bytes_written = vsnprintf(LAST_ERROR_MESSAGE + bytes_written, // start writing at the end of the string
                              remaining_capacity,
                              in_fmt,
                              arg_ptr);
    if ((bytes_written < 0) || (bytes_written >= remaining_capacity)) {
        LAST_ERROR_MESSAGE[0] = 0;
        return 0;
    }
    return 1;
}
