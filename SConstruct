#------------------------------------------------------------------------------------------------------------
# Build configuration for GDExtension with C++26 (GCC 16)
#------------------------------------------------------------------------------------------------------------
import sys

# 1. Initialize godot-cpp environment with Godot 4.7 API
env = SConscript("godot-cpp/SConstruct", {"api_version": "4.7"})

# 2. Clone environment specifically for our game module to isolate godot-cpp
game_env = env.Clone()

# 3. Enforce GCC 16 on our game environment
game_env.Replace(CXX="g++-16")
game_env.Replace(CC="gcc-16")

# 4. Enforce GNU C++26 Standard and P2996 Static Reflection
game_env["CXXFLAGS"] = [flag for flag in game_env["CXXFLAGS"] if not flag.startswith("-std=")]
game_env.Append(CXXFLAGS=["-std=gnu++26", "-freflection"])

# 5. Enable GDExtension Hot Reloading support
game_env.Append(CPPDEFINES=["HOT_RELOAD_ENABLED"])

# 6. Include paths
game_env.Append(CPPPATH=["src"])

# 7. Collect sources from src/
sources = Glob("src/*.cpp") + Glob("src/*/*.cpp")

# 8. Target shared library in bin/
target_lib = f"bin/libgame.{game_env['platform']}.{game_env['target']}.x86_64{game_env['SHLIBSUFFIX']}"
library = game_env.SharedLibrary(target=target_lib, source=sources)

Default(library)