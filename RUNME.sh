#!/bin/bash
echo "LLVM week unambitious task. Finding floating-point instructions."

# Change to the build directory
cd /Users/sunwookim028/llvm/llvm-pass-skeleton/build || exit 1

# Run cmake and make
echo "Running cmake..."
cmake .. || exit 1
echo "Running make..."
make || exit 1

# Compile with clang using the pass
echo "Compiling with LLVM pass..."
/opt/homebrew/opt/llvm/bin/clang -fpass-plugin=skeleton/SkeletonPass.dylib -O0 ../../test/main.cpp