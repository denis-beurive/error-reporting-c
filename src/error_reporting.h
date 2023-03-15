/**
 * @file error_reporting.h
 *
 * Header file for the error reporting system.
 */

#ifndef LIB_ERROR_REPORTING_H
#define LIB_ERROR_REPORTING_H

/**
 * @brief The total number of bytes allocated for the buffer used to store the message that describes the error.
 * @note This number includes the terminal zero.
 */
#define LAST_ERROR_MESSAGE_BUFFER_CAPACITY 200

/**
 * @brief The total number of bytes allocated for the buffer used to store the path to the file from
 * which the error was raised.
 * @note This number includes the terminal zero.
 */
#define LAST_ERROR_FILE_BUFFER_CAPACITY 10240

/**
 * The total number of bytes allocated for the buffer used to store the name of the function from which
 * the error was raised.
 * @note This number includes the terminal zero.
 */
#define LAST_ERROR_FUNCTION_BUFFER_CAPACITY 10240

void
last_error_init();

long
last_error_get_id();

long
last_error_get_line();

const char *
last_error_get_message();

const char *
last_error_get_file();

const char *
last_error_get_function();

int
last_error_set(
        long in_error_id,
        const char *in_file,
        long in_line,
        const char *in_function,
        const char *in_fmt,
        ...);

#endif //LIB_ERROR_REPORTING_H
