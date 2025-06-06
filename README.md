# Luden Engine

This repository contains a small C++ game engine along with a simple editor and demo game. The build configuration is driven by [Premake](https://premake.github.io/).

## Repository Layout

- **Engine/** – Core engine source and headers.
- **Editor/** – GUI editor built on the engine.
- **Game/** – Minimal example game using the engine.
- **Tools/** – Python utilities used during the build (reflection, version headers).
- **extern/** – Third‑party libraries (e.g. RTTR, SFML, ImGui). Most are ignored by Git and must be installed separately.
- **premake5.lua** – Build script used by Premake.
- **premake5.exe** – Windows build of Premake 5 for convenience.

## Prerequisites

1. C++20 capable compiler (Visual Studio 2022, clang, or gcc).
2. [Premake 5](https://premake.github.io/) in your `PATH` (or use the provided `premake5.exe` on Windows).
3. [SFML](https://www.sfml-dev.org/) development libraries.
4. Python 3 for the helper scripts under `Tools/`.
5. Clone submodules and acquire additional dependencies (ImGui, ImGui‑SFML, sol2, etc.).

```bash
git submodule update --init --recursive
```

## Generating Build Files

Run Premake from the repository root. Pick the action that matches your toolchain.

```bash
# Windows example
premake5.exe vs2022

# Linux/macOS example
premake5 gmake2
```

Premake generates a Visual Studio solution or Makefiles inside the repository. Build them using your compiler or IDE of choice.

## Running the Editor or Game

After building, executables are placed under `bin/<config>-<system>-<architecture>/`.

- **Editor:** `bin/Debug-windows-x86_64/Editor/Editor.exe` (or the corresponding path for your platform).
- **Game:** `bin/Debug-windows-x86_64/Game/Game.exe`.

Launch the editor to create or modify scenes, or run the demo game directly.

