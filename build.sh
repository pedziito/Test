#!/bin/bash

echo "==================================="
echo "  Plugin Injector Build Script"
echo "==================================="
echo ""

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Step 1: Create build directory
echo -e "${BLUE}[1/5]${NC} Creating build directory..."
mkdir -p build
cd build

# Step 2: Configure with CMake
echo -e "${BLUE}[2/5]${NC} Configuring project with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release
if [ $? -ne 0 ]; then
    echo -e "${RED}✗ CMake configuration failed!${NC}"
    exit 1
fi

# Step 3: Build the injector tool
echo -e "${BLUE}[3/5]${NC} Building plugin injector tool..."
cmake --build . --target plugin_injector
if [ $? -ne 0 ]; then
    echo -e "${RED}✗ Failed to build injector!${NC}"
    exit 1
fi

# Step 4: Run the injector to process plugins
echo -e "${BLUE}[4/5]${NC} Running plugin injector..."
./plugin_injector ../plugins ../plugins_injected
if [ $? -ne 0 ]; then
    echo -e "${RED}✗ Plugin injection failed!${NC}"
    exit 1
fi

# Step 5: Reconfigure and build everything including plugins
echo -e "${BLUE}[5/5]${NC} Building all plugins with injected code..."
cmake ..
cmake --build .
if [ $? -ne 0 ]; then
    echo -e "${RED}✗ Failed to build plugins!${NC}"
    exit 1
fi

cd ..

echo ""
echo -e "${GREEN}✓ Build completed successfully!${NC}"
echo ""
echo "==================================="
echo "  Build Summary"
echo "==================================="
echo "Injected plugin sources: plugins_injected/"
echo "Built plugin binaries:   build/plugins_injected/"
echo "Executables:             build/"
echo ""
echo "To run the plugin system:"
echo "  CLI Mode:  cd build && ./plugin_loader"
echo "  Web Mode:  cd build && ./web_server"
echo "             Then open http://localhost:8080 in your browser"
echo ""
