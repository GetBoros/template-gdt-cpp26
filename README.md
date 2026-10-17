---

A minimal, high-performance **Godot 4.7+ GDExtension template powered by modern C++26 (GCC 16)**. Tailored for lightweight 2D games, Twitch streamer overlays, and Web (WebGL 2.0 / WebAssembly) exports with near-zero CPU/GPU overhead on Linux (Wayland / X11).

---

## 🌟 Key Features

* **Pure C++26 Core:** Enforces `-std=gnu++26` via GCC 16, fully prepared for Static Reflection (P2996).
* **Zero-Cost Hot Reload:** Instant C++ code reloading in the Godot Editor without restarts (`HOT_RELOAD_ENABLED`).
* **Feature-First Architecture:** Mirrored domain layout (`core/`, `gameplay/`, `ui/`) across GDScript and C++.
* **Zero-Overhead Idle Loop:** Configured with `low_processor_usage_mode = true` (`epoll`-driven, 0.0% CPU/GPU in idle).
* **iGPU Optimized:** Runs on `gl_compatibility` (OpenGL 3.3 / WebGL 2.0) with zero Vulkan overhead.

---

## 1. System Prerequisites

### Required Toolchain (Ubuntu Linux)
* **Godot Engine 4.7+** (installed as system binary `/usr/local/bin/godot`)
* **Compiler:** GCC 16 (`gcc-16`, `g++-16`)
* **Build System:** SCons
* **Version Control:** Git (with submodule support)

### Install Official Godot 4 Binary (CLI)
```bash
cd /tmp
DOWNLOAD_URL=$(curl -sL https://api.github.com/repos/godotengine/godot/releases/latest | grep -oE 'https://[^"]+linux\.x86_64\.zip' | head -n 1)
curl -L -o godot_linux.zip "$DOWNLOAD_URL"
unzip -q godot_linux.zip
sudo install -m 755 Godot_v*linux.x86_64 /usr/local/bin/godot
rm -f godot_linux.zip Godot_v*linux.x86_64
godot --version
```

---

## 2. Quick Start: Clone & Build

### 1. Clone with Submodules
Always clone recursively to pull the `godot-cpp` bindings:
```bash
git clone --recursive https://github.com/GetBoros/template-gdt-cpp26.git
cd template-gdt-cpp26
```
*(If cloned without `--recursive`, run: `git submodule update --init --recursive`)*

### 2. Build the Extension
* **Via Terminal:**
  ```bash
  scons -j$(nproc)
  ```
* **Via VS Code:** Press **`F7`** (or `Ctrl + Shift + B`) to trigger the build task.

### 3. Open in Godot
```bash
godot -e .
```

---

## 3. Project Architecture

The C++ source tree mirrors Godot's scene and asset structure (Feature-First):

```text
template-gdt-cpp26/
├── bin/                        # GDExtension manifest and compiled .so libraries
│   ├── game.gdextension        # Extension entry manifest (reloadable = true)
│   └── libgame.*.so            # Compiled binary output (ignored by git)
│
├── core/                       # Godot scenes (Game orchestrator, root nodes)
├── gameplay/                   # Godot gameplay scenes and levels
├── ui/                         # Godot UI components and menus
│
├── src/                        # C++26 Source Code
│   ├── core/                   # Handshake, ClassDB registration, singletons
│   │   ├── register_types.h
│   │   └── register_types.cpp
│   ├── gameplay/               # Gameplay actors, components, physics
│   │   ├── rotator_2d.h
│   │   └── rotator_2d.cpp
│   └── ui/                     # High-performance C++ UI controls
│
├── godot-cpp/                  # Godot C++ GDExtension bindings (Git submodule)
├── SConstruct                  # SCons build orchestration script (GCC 16, C++26)
├── .clangd                     # Local language server configuration
└── project.godot               # Engine configuration & SSOT
```

---

## 4. IDE Setup (VS Code & clangd)

### Recommended Extensions
* `llvm-vs-code-extensions.vscode-clangd` (C++26 IntelliSense & Diagnostics)
* `geequlim.godot-tools` (GDScript syntax & Language Server)

### Local clangd Configuration
Ensure your local `.clangd` file in the project root points to your workspace paths for zero-error IntelliSense:
```yaml
CompileFlags:
  Add:
    - "-I/path/to/project/src"
    - "-I/path/to/project/godot-cpp/include"
    - "-I/path/to/project/godot-cpp/gen/include"
    - "-I/path/to/project/godot-cpp/gdextension"
    - "-std=gnu++26"
```

---

## 5. CLI Workflow & Cheat Sheet

Run commands from the project root:

```bash
# Open project directly in Godot Editor
godot -e .

# Run the game immediately
godot .

# Run with real-time FPS counter in terminal
godot --print-fps .

# Force Wayland native display driver
godot --display-driver wayland .
```

---

## 6. Engine Profile & Transparent Overlay

### Zero-Cost Idle & Rendering
* **Compatibility Renderer:** Pure OpenGL 3.3 / WebGL 2.0 without Vulkan compositor overhead.
* **Low Processor Usage:** `run/low_processor_usage_mode = true` sleeps the CPU loop until OS compositor events occur.

### Transparent Borderless Window (Twitch Overlay)
Configured in `project.godot`:
```ini
[display]
window/size/transparent=true
window/size/borderless=true
window/size/always_on_top=true
window/subwindows/embed_subwindows=false
window/per_pixel_transparency/allowed=true

[rendering]
viewport/transparent_background=true
```

---

## 7. Using This Template for New Projects

1. Click **Use this template** -> **Create a new repository** on GitHub.
2. Clone your new repository with `--recursive`.
3. Update project name inside `project.godot` (`config/name`).
4. Build with `scons -j$(nproc)` and launch with `godot -e .`.
