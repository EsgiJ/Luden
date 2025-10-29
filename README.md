# Luden Engine

**Luden Engine** is a lightweight 2D game engine built in modern **C++20**, featuring a custom **Entity–Component System (ECS)**, real-time **ImGui editor**, resource management and scene serialization.  
It’s designed to be data-driven, and easy to extend, ideal for learning engine architecture or building small 2D games from scratch.

---

## Repository Layout
Engine/ – Core engine source and headers
Editor/ – ImGui-based scene editor built on the engine
MyGame/ – Minimal example game using the engine
extern/ – Third-party dependencies (SFML, ImGui, etc.)
premake5.lua – Premake build configuration script
setup-extern.bat – Downloads and sets up external dependencies
generate.bat – Generates Visual Studio solution files using Premake

## Prerequisites

- A **C++20-capable compiler** (Visual Studio 2022, Clang, or GCC)  
- **Premake 5** available in your `PATH` *(or use the included `premake5.exe`)*  
- **SFML 3.0** development libraries  

---

## Getting Started

1. **Clone the repository and its submodules:**
   ```bash
   git clone --recursive https://github.com/yourname/LudenEngine.git

2. **Run the external setup script (Windows):**
   ```bash
    setup-extern.bat
   ```
    This downloads and configures all required third-party libraries (SFML, ImGui, ImGui-SFML, nfd, etc.).

3. **Generate project files:**
     ```bash
    generate.bat
     ```

On Windows, this creates a Visual Studio 2022 solution.

Build the project using your IDE or compiler of choice.

## Running the Engine

After a successful build, binaries are located under:
     ```
    bin/<config>-<system>-<architecture>/
     ```

**Examples:**

- Editor: bin/Debug-windows-x86_64/Editor/Editor.exe

- Engine: bin/Debug-windows-x86_64/Engine/Engine.exe

Launch the Editor to create or modify scenes, or run the Game to test the example project.
