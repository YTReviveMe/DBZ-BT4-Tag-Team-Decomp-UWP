cmake_minimum_required(VERSION 3.26)
project(bt4_uwp_storage LANGUAGES C CXX)

add_library(bt4_uwp_storage STATIC storage.cpp storage_winrt.cpp audio.cpp http.cpp iso_io.cpp installer.cpp ../setup/native.cpp ../third_party/miniz/miniz.c)
target_compile_features(bt4_uwp_storage PUBLIC cxx_std_17)
target_compile_definitions(bt4_uwp_storage PUBLIC BT3_UWP=1 NOMINMAX)
target_include_directories(bt4_uwp_storage PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(NOT CMAKE_SYSTEM_NAME STREQUAL "WindowsStore")
    message(FATAL_ERROR "This target requires the UWP toolchain")
endif()

set(BT3_SDL3_UWP_SOURCE "" CACHE PATH "Path to the SDL3 UWP source tree")
set(BT3_SDL3_UWP_BUILD "" CACHE PATH "Path to the patched SDL3 UWP CMake build")
if(BT3_SDL3_UWP_SOURCE)
    target_include_directories(bt4_uwp_storage PRIVATE ${BT3_SDL3_UWP_SOURCE}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/imgui)
endif()
option(BT3_GAME "Link the full game from UWP COFF objects" OFF)
if(BT3_GAME)
    if(NOT BT3_SDL3_UWP_SOURCE OR NOT BT3_SDL3_UWP_BUILD)
        message(FATAL_ERROR "The game needs the patched SDL source and build paths")
    endif()
    set(package_assets
        ${CMAKE_CURRENT_SOURCE_DIR}/assets/BT4StoreLogo.png
        ${CMAKE_CURRENT_SOURCE_DIR}/assets/BT4Square150x150Logo.png
        ${CMAKE_CURRENT_SOURCE_DIR}/assets/BT4Square44x44Logo.png
        ${CMAKE_CURRENT_SOURCE_DIR}/assets/BT4Splash620x300.png)
    file(GLOB tbo_assets CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/../tbo/ui/*.png")
    list(APPEND tbo_assets "${CMAKE_CURRENT_SOURCE_DIR}/../tbo/ui/CREDITS.md")
    file(GLOB_RECURSE font_assets CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/fonts/*")
    file(GLOB_RECURSE translation_assets CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/../translations/*")
    file(GLOB_RECURSE mod_assets CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/../examples/mods/*")
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../tbo/ui/bg.png" OR
       NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/fonts/fonts.tsv")
        message(FATAL_ERROR "Tenkaichi Budokai Online artwork and fonts are required in the Xbox package")
    endif()
    set_source_files_properties(${tbo_assets} PROPERTIES
        VS_DEPLOYMENT_CONTENT 1 VS_DEPLOYMENT_LOCATION "tbo/ui")
    foreach(font_asset IN LISTS font_assets)
        file(RELATIVE_PATH font_relative "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/fonts" "${font_asset}")
        get_filename_component(font_subdir "${font_relative}" DIRECTORY)
        if(font_subdir)
            set(font_location "fonts/${font_subdir}")
        else()
            set(font_location "fonts")
        endif()
        set_source_files_properties("${font_asset}" PROPERTIES
            VS_DEPLOYMENT_CONTENT 1 VS_DEPLOYMENT_LOCATION "${font_location}")
    endforeach()
    foreach(translation_asset IN LISTS translation_assets)
        file(RELATIVE_PATH translation_relative "${CMAKE_CURRENT_SOURCE_DIR}/../translations" "${translation_asset}")
        get_filename_component(translation_subdir "${translation_relative}" DIRECTORY)
        set_source_files_properties("${translation_asset}" PROPERTIES
            VS_DEPLOYMENT_CONTENT 1 VS_DEPLOYMENT_LOCATION "translations/${translation_subdir}")
    endforeach()
    foreach(mod_asset IN LISTS mod_assets)
        file(RELATIVE_PATH mod_relative "${CMAKE_CURRENT_SOURCE_DIR}/../examples/mods" "${mod_asset}")
        get_filename_component(mod_subdir "${mod_relative}" DIRECTORY)
        set_source_files_properties("${mod_asset}" PROPERTIES
            VS_DEPLOYMENT_CONTENT 1 VS_DEPLOYMENT_LOCATION "tbo/bundled-mods/${mod_subdir}")
    endforeach()
    set(sdl_dll "${BT3_SDL3_UWP_BUILD}/Release/SDL3.dll")
    set(voice_dlls
        ${CMAKE_CURRENT_SOURCE_DIR}/../voice-runtime/libdatachannel.dll
        ${CMAKE_CURRENT_SOURCE_DIR}/../voice-runtime/libopus-0.dll
        ${CMAKE_CURRENT_SOURCE_DIR}/../voice-runtime/libwinpthread-1.dll)
    set(data_manifest "${CMAKE_CURRENT_SOURCE_DIR}/../build/uwp/pack/BT4decompUWP.dat")
    set_source_files_properties(${package_assets} PROPERTIES
        VS_DEPLOYMENT_CONTENT 1 VS_DEPLOYMENT_LOCATION assets)
    set_source_files_properties("${sdl_dll}" PROPERTIES
        VS_DEPLOYMENT_CONTENT 1 VS_DEPLOYMENT_LOCATION ".")
    set_source_files_properties(${voice_dlls} PROPERTIES
        VS_DEPLOYMENT_CONTENT 1 VS_DEPLOYMENT_LOCATION ".")
    if(EXISTS "${data_manifest}")
        set_source_files_properties("${data_manifest}" PROPERTIES
            VS_DEPLOYMENT_CONTENT 1 VS_DEPLOYMENT_LOCATION ".")
    else()
        message(WARNING "The full game package has no BT4decompUWP.dat yet; run make_uwp_dat.py")
    endif()
    file(GLOB game_objects "${CMAKE_CURRENT_SOURCE_DIR}/../build/uwp/game/*.obj")
    file(GLOB host_objects "${CMAKE_CURRENT_SOURCE_DIR}/../build/uwp/host/*.obj")
    list(FILTER host_objects EXCLUDE REGEX "[/\\\\]port_src_tag_team(_wire)?\\.obj$")
    file(GLOB data_objects "${CMAKE_CURRENT_SOURCE_DIR}/../build/uwp/data/*.obj")
    if(NOT game_objects OR NOT host_objects OR NOT data_objects)
        message(FATAL_ERROR "Run the UWP game and host object builders first")
    endif()
    add_executable(bt4_uwp_game WIN32 game_entry.cpp GamePackage.appxmanifest
                   ${package_assets} ${tbo_assets} ${font_assets} ${translation_assets} ${mod_assets} "${sdl_dll}" ${voice_dlls} "${data_manifest}")
    set_target_properties(bt4_uwp_game PROPERTIES OUTPUT_NAME BT4decompUWP)
    set_property(TARGET bt4_uwp_game PROPERTY VS_WINDOWS_TARGET_PLATFORM_MIN_VERSION "10.0.19041.0")
    set_source_files_properties(game_entry.cpp PROPERTIES COMPILE_OPTIONS /ZW)
    target_include_directories(bt4_uwp_game PRIVATE ${BT3_SDL3_UWP_SOURCE}/include)
    target_link_libraries(bt4_uwp_game PRIVATE bt4_uwp_storage vccorlib WindowsApp
        "${BT3_SDL3_UWP_BUILD}/Release/SDL3.lib" ${game_objects} ${host_objects} ${data_objects})
    target_link_options(bt4_uwp_game PRIVATE /APPCONTAINER /WINMD
        "/WINMDFILE:${CMAKE_CURRENT_BINARY_DIR}/BT4decompUWP.winmd"
        "/MAP:${CMAKE_CURRENT_BINARY_DIR}/BT4decompUWP.map"
        /BASE:0x20000000 /DYNAMICBASE /HIGHENTROPYVA:NO /LARGEADDRESSAWARE:NO /STACK:0x1000000)
    get_filename_component(linker_dir "${CMAKE_LINKER}" DIRECTORY)
    find_program(editbin NAMES editbin.exe PATHS "${linker_dir}" NO_DEFAULT_PATH REQUIRED)
    add_custom_command(TARGET bt4_uwp_game POST_BUILD
        COMMAND "${editbin}" /NOLOGO /LARGEADDRESSAWARE "$<TARGET_FILE:bt4_uwp_game>"
        COMMENT "Allow full x64 process address space while retaining low game image")
endif()
