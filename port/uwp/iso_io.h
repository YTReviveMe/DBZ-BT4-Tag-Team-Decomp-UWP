#pragma once
#include <SDL3/SDL.h>
#include <string>

SDL_IOStream *PortUwp_OpenIO(const std::string &path, bool write);
bool PortUwp_CreateDirectory(const std::string &path);
bool PortUwp_FileExists(const std::string &path);
bool PortUwp_MoveFile(const std::string &source, const std::string &destination);
SDL_IOStream *PortUwp_IOFromFile(const char *path, const char *mode);
bool PortUwp_GetPathInfo(const char *path, SDL_PathInfo *info);
bool PortUwp_SaveFile(const char *path, const void *data, size_t bytes);
void *PortUwp_LoadFile(const char *path, size_t *bytes);
bool PortUwp_CreatePath(const char *path);
bool PortUwp_RenamePath(const char *source, const char *destination);
bool PortUwp_RemovePath(const char *path);
