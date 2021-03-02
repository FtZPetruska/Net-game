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
#define SCANF(...) scanf_s(__VA_ARGS__)
#define FSCANF(...) fscanf_s(__VA_ARGS__)
#define FOPEN(...) fopen_s(__VA_ARGS__)
#define FCLOSE(...) fclose_s(__VA_ARGS__)
#else
#define PRINTF(...) printf(__VA_ARGS__)
#define FPRINTF(TARGET_FILE, ...) fprintf(TARGET_FILE, __VA_ARGS__)
#define SCANF(...) scanf(__VA_ARGS__)
#define FSCANF(...) fscanf(__VA_ARGS__)
#define FOPEN(...) fopen(__VA_ARGS__)
#define FCLOSE(...) fclose(__VA_ARGS__)
#endif

#endif  // __CROSS_IO_H__
