#pragma once

#include <filesystem>
#include <windows.h>
#include <string>

inline std::filesystem::path GetExecutableDirectory()
{
    char path[MAX_PATH];
    GetModuleFileNameA(nullptr, path, MAX_PATH);
    return std::filesystem::path(path).parent_path();
}

inline std::string GetAssetPath(const std::string& relativePath)
{
    return (GetExecutableDirectory() / "assets" / relativePath).string();
}