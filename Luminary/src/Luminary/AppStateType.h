#pragma once

enum class AppStateType {
    Boot,
    LaunchMenu,
    InScene
};

constexpr const char* to_string(AppStateType type) {
    switch (type) {
    case AppStateType::Boot: return "Boot";
    case AppStateType::LaunchMenu: return "LaunchMenu";
    case AppStateType::InScene: return "InScene";
    default: return "Unknown";
    }
}