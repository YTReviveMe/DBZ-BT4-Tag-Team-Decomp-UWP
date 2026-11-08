# BT4 Tag Team for Xbox Dev Mode

UWP port of [DBZ BT4 Tag Team Decomp](https://github.com/NauTaro/DBZ-BT4-Tag-Team-Decomp).

## Install game data

On first launch, choose **Internal** or **External Drive**, select a compatible BT4 B14 Rev2 Spanish (LATINO) or English ISO, then select **Install and Play**. The ISO is not included. Setup extracts the required files and shows progress as a percentage.

Game data goes in `BT4\gamedata` on the selected drive. Saves, settings, and logs stay in `D:\DevelopmentFiles\BT4` on Xbox. Internal textures in `D:\DevelopmentFiles\BT4\textures` take priority over external textures. Windows UWP test installations use the app LocalFolder for internal content.

The installed data must include `gamedata/disc/SLUS_219.78`, `gamedata/disc/BIN/DBZP.BIN`, and `gamedata/disc/BIN/DBZ4.BIN`. UI settings default to English and retain the user's selection. English game text and voices require compatible English disc data.

## Build

Use a Visual Studio 2022 x64 Windows Store environment with Python, `clang`, `clang-cl`, `llc`, `llvm-nm`, CMake, and patched SDL3 UWP source and build trees. The inherited `BT3_UWP` and `BT3_*` build identifiers are required by the existing port code.

1. Set `BT3_SDL3_UWP_SOURCE` to the SDL3 source tree. Run `python port/uwp/build_game_objects.py` and `python port/uwp/build_port_objects.py` from the repository root.
2. Configure `port/uwp` with the Visual Studio 2022 generator, `-A x64`, `-DCMAKE_SYSTEM_NAME=WindowsStore`, `-DCMAKE_SYSTEM_VERSION=10.0`, `-DBT3_GAME=ON`, `-DBT3_SDL3_UWP_SOURCE=<source>`, and `-DBT3_SDL3_UWP_BUILD=<build>`.
3. Build Release. If linking reports unresolved game library symbols from data tables, run `python port/uwp/generate_link_stubs.py <link-log>`, rebuild the host objects, reconfigure, and build again.
4. Run `python port/uwp/make_uwp_dat.py` and `python port/uwp/make_uwp_state.py` after the final link, then rebuild the package.
5. Run `port/uwp/finalize_package.ps1` with the MSIX, final EXE, data map, state map, and development certificate thumbprint. The script verifies the packaged files and signs the MSIX.

The package contains a state map and data map matched to its executable. Install updates through Xbox Dev Mode. Unresolved game library calls use trap stubs and stop the app if reached.

`port/build` and `build` are ignored by Git. Keep ISOs, extracted game data, private keys, and credentials outside the repository.

Splash icon artwork by [EvilZGaruda](https://www.deviantart.com/evilzgaruda).
