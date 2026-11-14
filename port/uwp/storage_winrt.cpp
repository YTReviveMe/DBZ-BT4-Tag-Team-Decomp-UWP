#include "storage.h"
#include "installer.h"
#include "include/uwp_stdio.h"

#include <cstdio>
#include <filesystem>
#include <future>
#include <string>
#include <vector>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.System.Profile.h>

namespace {
std::string read_choice(const std::filesystem::path &path) {
    FILE *file = PortUwp_Fopen(path.string().c_str(), "rb");
    if (!file) return {};
    char text[1024] = {};
    const bool read = fgets(text, sizeof(text), file) != nullptr;
    fclose(file);
    if (!read) return {};
    std::string choice(text);
    while (!choice.empty() && (choice.back() == '\r' || choice.back() == '\n')) choice.pop_back();
    return choice;
}

void save_choice(const std::filesystem::path &path, const std::string &choice) {
    FILE *file = PortUwp_Fopen(path.string().c_str(), "wb");
    if (!file) return;
    fprintf(file, "%s\n", choice.c_str());
    fclose(file);
}

std::vector<std::string> removable_roots() {
    return std::async(std::launch::async, [] {
        std::vector<std::string> roots;
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        const auto folders = winrt::Windows::Storage::KnownFolders::RemovableDevices().GetFoldersAsync().get();
        for (const auto &folder : folders) roots.push_back(winrt::to_string(folder.Path()));
        return roots;
    }).get();
}
}

extern "C" int PortUwp_InitStorage(void) {
    PortUwp_Trace("BT4 startup: resolving storage");
    try {
        const auto app_local = winrt::to_string(winrt::Windows::Storage::ApplicationData::Current().LocalFolder().Path());
        const bool xbox = winrt::Windows::System::Profile::AnalyticsInfo::VersionInfo().DeviceFamily() == L"Windows.Xbox";
        const auto local = xbox ? std::string("D:\\DevelopmentFiles\\BT4") :
            (std::filesystem::path(app_local) / "BT4").string();
        PortUwp_Trace((std::string("Device: ") + (xbox ? "Xbox" : "Windows") + "; LocalFolder: " + app_local).c_str());
        PortUwp_Trace((std::string("Internal BT4: ") + local).c_str());
        const auto stable_choice = std::filesystem::path(local) / "external-content.txt";
        const auto app_choice = std::filesystem::path(app_local) / "external-content.txt";
        std::string external = read_choice(stable_choice);
        if (external.empty()) external = read_choice(app_choice);
        bool configured = PortUwp_ConfigureStorage(local.c_str(), external.c_str()) != 0;
        PortUwp_Trace(configured ? "Prepared content found" : PortUwp_StorageError());
        if (!configured && xbox) {
            try {
                for (const auto &drive : removable_roots()) {
                    const auto candidate = (std::filesystem::path(drive) / "BT4").string();
                    if (PortUwp_ConfigureStorage(local.c_str(), candidate.c_str())) {
                        external = candidate;
                        configured = true;
                        PortUwp_Trace(("Found external BT4 content: " + candidate).c_str());
                        break;
                    }
                }
            } catch (...) { PortUwp_Trace("Could not enumerate external drives"); }
        }
        if (!configured) {
            PortUwp_Trace("Opening BT4 ISO setup");
            std::string installed;
            if (!PortUwp_RunInstaller(local, installed)) {
                PortUwp_Trace(PortUwp_StorageError());
                return 0;
            }
            external = installed == local ? std::string() : installed;
            configured = PortUwp_ConfigureStorage(local.c_str(), external.c_str()) != 0;
        }
        if (configured) {
            if (local == PortUwp_ContentPath()) external.clear();
            save_choice(stable_choice, external);
            save_choice(app_choice, external);
        }
        const bool applied = configured && PortUwp_ApplyStorageEnvironment() != 0;
        if (applied) PortUwp_Trace((std::string("Content root: ") + PortUwp_ContentPath()).c_str());
        PortUwp_Trace(configured ? (applied ? "Storage ready" : "Could not set storage paths") : PortUwp_StorageError());
        return applied;
    } catch (const winrt::hresult_error &e) {
        PortUwp_SetStorageError(winrt::to_string(e.message()).c_str());
        PortUwp_Trace(PortUwp_StorageError());
        return 0;
    } catch (...) {
        PortUwp_SetStorageError("Could not resolve UWP local storage");
        PortUwp_Trace(PortUwp_StorageError());
        return 0;
    }
}
