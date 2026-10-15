# template-gdt-cpp26

A minimal, high-performance **Godot 4 + C++26** project template tailored for 2D games, WebGL exports, and transparent desktop/stream overlays on Linux (Ubuntu / Wayland).

---

## 1. System Prerequisites (Ubuntu Linux)

### Install Official Godot 4 Binary to System Path
Run the following script to fetch the latest stable Linux build and install it to `/usr/local/bin/godot`:

```bash
cd /tmp
DOWNLOAD_URL=$(curl -sL https://api.github.com/repos/godotengine/godot/releases/latest | grep -oE 'https://[^"]+linux\.x86_64\.zip' | head -n 1)
curl -L -o godot_linux.zip "$DOWNLOAD_URL"
unzip -q godot_linux.zip
sudo install -m 755 Godot_v*linux.x86_64 /usr/local/bin/godot
rm -f godot_linux.zip Godot_v*linux.x86_64
godot --version
```

### C++26 & Build System Requirements
* **Compiler:** GCC 16+ (`g++-16` with `-std=gnu++26`)
* **Build System:** SCons / CMake Tools
* **VS Code Extensions:**
  * `geequlim.godot-tools` (GDScript syntax & Language Server)
  * `ms-vscode.cpptools` (C/C++ IntelliSense & Debugging)

---

## 2. CLI Workflow & Cheat Sheet

Run commands from the project root directory:

```bash
# Open this project directly in Godot Editor (bypassing project manager)
godot -e

# Run the game immediately without opening the editor
godot

# Run with real-time FPS monitoring printed to stdout
godot --print-fps

# Run directly on Wayland display driver (if needed)
godot --display-driver wayland
```

---

## 3. Architecture & Engine Settings (`project.godot`)

### Rendering Backend: Compatibility (OpenGL 3.3 / WebGL 2.0)
* Configured via `renderer/rendering_method="gl_compatibility"`.
* Eliminates Vulkan/Forward+ overhead.
* 1:1 direct compilation to **WebGL 2.0** for browser deployments.
* Runs near 0% GPU load on Intel UHD 630 iGPU.

### Zero-Cost Idle (Event-Driven Loop)
* `run/low_processor_usage_mode = true` blocks execution on OS compositor events (`epoll`).
* When UI/overlay is static, frame rendering drops to 0 FPS, consuming 0.0% CPU/GPU.

### Borderless Transparent Overlay Configuration
To make a fully transparent desktop HUD or Twitch overlay:
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

## 4. VS Code & Language Server (LSP) Setup

1. Open project folder in VS Code:
   ```bash
   code ~/Work/template-gd-cpp26
   ```
2. Enable external editor in Godot:
   * **Editor -> Editor Settings -> Text Editor -> External**
   * Check **Use External Editor**.
   * **Exec Path:** `code`
   * **Exec Flags:** `{project} --goto {file}:{line}:{col}`
3. Godot automatically hosts LSP on port `6005`. The `godot-tools` extension connects automatically.

---

## 5. Repository Template Checklist

When creating a new project from this template:
1. Click **Use this template** on GitHub.
2. Clone repository:
   ```bash
   git clone <REPO_URL>
   cd <REPO_NAME>
   ```
3. Update project name inside `project.godot` (`config/name`).
4. Launch editor with `godot -e`.
```

---
