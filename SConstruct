#------------------------------------------------------------------------------------------------------------
# Build configuration for GDExtension with C++26 (GCC 16 / Emscripten)
#------------------------------------------------------------------------------------------------------------
import sys

# 1. Initialize godot-cpp environment with Godot 4.7 API
env = SConscript("godot-cpp/SConstruct", {"api_version": "4.7"})

# 2. Clone environment specifically for our game module to isolate godot-cpp
game_env = env.Clone()

is_web = game_env["platform"] == "web"

# 3. Clean up default -std= flags
game_env["CXXFLAGS"] = [flag for flag in game_env["CXXFLAGS"] if not flag.startswith("-std=")]

# 4. Platform-specific Compiler and Flags
if is_web:
    # Web target: use Emscripten (em++ is automatically provided by godot-cpp)
    # Clang supports GNU C++26 standard
    game_env.Append(CXXFLAGS=["-std=gnu++26"])
else:
    # Linux Desktop target: enforce local GCC 16 with P2996 Static Reflection
    game_env.Replace(CXX="g++-16")
    game_env.Replace(CC="gcc-16")
    game_env.Append(CXXFLAGS=["-std=gnu++26", "-freflection"])
    game_env.Append(CPPDEFINES=["HOT_RELOAD_ENABLED"])

# 5. Include paths
game_env.Append(CPPPATH=["src"])

# 6. Collect sources from src/
sources = Glob("src/*.cpp") + Glob("src/*/*.cpp")

# 7. Target library in bin/ (x86_64 for Linux, wasm32 for Web)
arch = game_env.get("arch", "wasm32" if is_web else "x86_64")
target_lib = f"bin/libgame.{game_env['platform']}.{game_env['target']}.{arch}{game_env['SHLIBSUFFIX']}"
library = game_env.SharedLibrary(target=target_lib, source=sources)

Default(library)