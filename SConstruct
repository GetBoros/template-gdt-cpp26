#------------------------------------------------------------------------------------------------------------
# Build configuration for GDExtension with C++26 (GCC 16)
#------------------------------------------------------------------------------------------------------------
import sys

# 1. Initialize godot-cpp environment with Godot 4.7 API
env = SConscript("godot-cpp/SConstruct", {"api_version": "4.7"})

# 2. Enforce GCC 16
env.Replace(CXX="g++-16")
env.Replace(CC="gcc-16")

# 3. Enforce GNU C++26 Standard
env["CXXFLAGS"] = [flag for flag in env["CXXFLAGS"] if not flag.startswith("-std=")]
env.Append(CXXFLAGS=["-std=gnu++26"])

# 4. Enable GDExtension Hot Reloading support
env.Append(CPPDEFINES=["HOT_RELOAD_ENABLED"])
# 5. Include paths
env.Append(CPPPATH=["src"])

# 6. Collect sources from src/
sources = Glob("src/*.cpp")

# 7. Target shared library in bin/
target_lib = f"bin/libgame.{env['platform']}.{env['target']}.x86_64{env['SHLIBSUFFIX']}"
library = env.SharedLibrary(target=target_lib, source=sources)

Default(library)
