#!/bin/bash

# Build and run script for the plugin system
# Dette script bygger og kører plugin systemet

set -e  # Exit on error

echo "======================================"
echo "Plugin System - Build and Run Script"
echo "======================================"
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored output
print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

# Check if cmake is installed
if ! command -v cmake &> /dev/null; then
    print_error "CMake is not installed. Please install CMake first."
    exit 1
fi

# Check if make is installed
if ! command -v make &> /dev/null; then
    print_error "Make is not installed. Please install Make first."
    exit 1
fi

# Parse arguments
ACTION="${1:-run}"

case "$ACTION" in
    build)
        print_info "Building plugin system..."
        make build
        print_info "Build complete!"
        ;;
    
    run)
        print_info "Building and running plugin system..."
        make run
        ;;
    
    clean)
        print_info "Cleaning build artifacts..."
        make clean
        print_info "Clean complete!"
        ;;
    
    rebuild)
        print_info "Rebuilding plugin system..."
        make clean
        make build
        print_info "Rebuild complete!"
        ;;
    
    plugins)
        print_info "Building plugins only..."
        make plugins
        print_info "Plugins built!"
        ;;
    
    help|--help|-h)
        echo "Usage: $0 [action]"
        echo ""
        echo "Actions:"
        echo "  build    - Build the plugin system"
        echo "  run      - Build and run the plugin system (default)"
        echo "  clean    - Clean build artifacts"
        echo "  rebuild  - Clean and build from scratch"
        echo "  plugins  - Build only the plugins"
        echo "  help     - Show this help message"
        echo ""
        echo "Examples:"
        echo "  $0           # Build and run"
        echo "  $0 build     # Just build"
        echo "  $0 clean     # Clean build directory"
        ;;
    
    *)
        print_error "Unknown action: $ACTION"
        echo "Use '$0 help' for usage information"
        exit 1
        ;;
esac

exit 0
