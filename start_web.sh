#!/bin/bash

echo "========================================"
echo "  Starting Plugin Injector Web GUI"
echo "========================================"
echo ""

# Check if already built
if [ ! -f "build/web_server" ]; then
    echo "🔨 Web server not found. Building..."
    ./build.sh
    if [ $? -ne 0 ]; then
        echo "❌ Build failed!"
        exit 1
    fi
    echo ""
fi

# Start the web server
cd build
echo "🚀 Starting web server..."
echo ""
./web_server 8080
