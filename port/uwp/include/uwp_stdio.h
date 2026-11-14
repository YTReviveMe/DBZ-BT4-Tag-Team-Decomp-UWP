#ifndef BT3_UWP_STDIO_H
#define BT3_UWP_STDIO_H

#include <stdio.h>
#include <sys/stat.h>

#ifdef __cplusplus
extern "C" {
#endif
FILE *PortUwp_Fopen(const char *path, const char *mode);
int PortUwp_Stat(const char *path, struct stat *info);
#ifdef __cplusplus
}
#endif
#ifndef BT3_UWP_STDIO_IMPLEMENTATION
#define fopen PortUwp_Fopen
#define stat(path, info) PortUwp_Stat(path, info)
#endif

#endif
