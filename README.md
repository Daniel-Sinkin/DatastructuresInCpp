# DatastructuresInCpp

# Release build
```bash
rm -rf build
cmake -G Ninja -S . -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=/opt/homebrew/opt/llvm/bin/clang++
cmake --build build -j8
```

# Debug build
```bash
rm -rf build
cmake -G Ninja -S . -B build -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_COMPILER=/opt/homebrew/opt/llvm/bin/clang++
cmake --build build -j8
```

# Run tests
```bash
ctest --test-dir build --output-on-failure -j8
```