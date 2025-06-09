# Buildsystems - pure headers

Pure header files can indeed be a hassle in build systems because they’re often not tracked as dependencies properly. If
the build system (e.g., Make, CMake) doesn’t recognize changes in header files, it might skip rebuilding source files
that depend on them.

Here’s why this happens and how to fix it:

Why It’s a Problem
Dependency Tracking: Build systems rely on dependency graphs. If a header file changes but isn’t listed as a dependency
for a source file, the build system won’t trigger a rebuild.

Manual Dependency Management: In some setups, dependencies aren’t automatically detected, especially for pure header
files (no associated .cpp).

Preprocessing Issues: If the header is included indirectly (e.g., through another header), the build system might miss
it unless explicitly told to track it.

Solutions

Use Automatic Dependency Generation:
For Make, use gcc or clang with the -M or -MM flag to generate dependency files (e.g., gcc -M source.c > deps.d).
Include these in your Makefile with -include deps.d.
For CMake, ensure CMAKE_CXX_FLAGS includes -M or use target_include_directories to specify header paths, letting CMake
handle dependencies.

Force Header Dependency Tracking:
In Make, explicitly list header files as dependencies for object files (e.g., myfile.o: myfile.cpp myheader.h).

In CMake, use set_source_files_properties or add headers to target_sources to ensure they’re tracked.

Use a Modern Build System:

Tools like Ninja or Bazel often handle header dependencies better out of the box.

For example, Bazel’s cc_library rules automatically track header changes.

CMake with Ninja backend (cmake -G Ninja) can improve dependency resolution.

Touch Source Files:

As a workaround, if a header changes, touch the related .cpp files to force a rebuild (e.g., touch source.cpp). This is
hacky but works in a pinch.

Compiler-Specific Flags:
Ensure your compiler flags enable dependency tracking. For g++, use -MD or -MP to generate dependency info during
compilation.
For MSVC, use /showIncludes to debug and ensure headers are tracked.

Clean Builds:
If dependencies are consistently missed, run a clean build (make clean && make) to force everything to rebuild. This is
a last resort due to time cost.

Best Practices
Modular Headers: Avoid deeply nested #include chains in headers to simplify dependency tracking.
Header Guards: Use #pragma once or traditional include guards to prevent multiple inclusions, which can confuse some
build systems.
Verbose Build Logs: Enable verbose output (e.g., make V=1) to debug which files are being checked for changes.

Incremental Builds: Test incremental builds after header changes to ensure the build system picks them up.
If you’re using a specific build system or toolchain, let me know, and I can tailor the advice further. For example, are
you on Make, CMake, or something else? Also, any specific issues like missed rebuilds or errors you’re hitting?

`make -B` (force unconditional rebuild) is a solid workaround for when the build system misses header file changes.

Debug Dependencies: Run make -d to see which files Make is checking. Look for missing headers in the dependency graph.
