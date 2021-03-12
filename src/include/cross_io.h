#ifndef __CROSS_IO_H__
#define __CROSS_IO_H__
#include <stdio.h>

/**
 * @file cross_io.h
 *
 * @brief This file provides macros expanding to I/O functions used in the
 *project in order to take advantage of their _s versions on MSVC for example.
 *
 **/

#if defined(_WIN32)
#define PRINTF(...) printf_s(__VA_ARGS__)
#define FPRINTF(TARGET_FILE, ...) fprintf_s(TARGET_FILE, __VA_ARGS__)
#define SPRINTF(BUFFER, SIZE_OF_BUFFER, FORMAT, ...) \
  sprintf_s(BUFFER, SIZE_OF_BUFFER, FORMAT, __VA_ARGS__)
#define SCANF(...) scanf_s(__VA_ARGS__)
#define FSCANF(...) fscanf_s(__VA_ARGS__)
#define FOPEN(STREAM, ...) fopen_s(&STREAM, __VA_ARGS__)
#define FCLOSE(...) fclose(__VA_ARGS__)
#define STRCAT(DEST, SOURCE, DEST_SIZE) strcat_s(DEST, DEST_SIZE, SOURCE)
#define STRCPY(DEST, SOURCE, DEST_SIZE) strcpy_s(DEST, DEST_SIZE, SOURCE)
#else
#define PRINTF(...) printf(__VA_ARGS__)
#define FPRINTF(TARGET_FILE, ...) fprintf(TARGET_FILE, __VA_ARGS__)
#define SPRINTF(BUFFER, SIZE_OF_BUFFER, FORMAT, ...) \
  sprintf(BUFFER, FORMAT, __VA_ARGS__)
#define SCANF(...) scanf(__VA_ARGS__)
#define FSCANF(...) fscanf(__VA_ARGS__)
#define FOPEN(STREAM, ...) STREAM = fopen(__VA_ARGS__)
#define FCLOSE(...) fclose(__VA_ARGS__)
#define STRCAT(DEST, SOURCE, DEST_SIZE) strncat(DEST, SOURCE, DEST_SIZE)
#define STRCPY(DEST, SOURCE, DEST_SIZE) strncpy(DEST, SOURCE, DEST_SIZE)
#endif

#endif  // __CROSS_IO_H__
