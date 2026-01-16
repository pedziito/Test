.PHONY: all build plugins clean run help

# Default target
all: build

# Build everything (main app and plugins)
build:
	@echo "Building plugin system..."
	@mkdir -p build
	@cd build && cmake .. && make
	@echo "Build complete!"
	@echo "Executables in: build/bin/"
	@echo "Plugins in: build/plugins/"

# Build only plugins
plugins:
	@echo "Building plugins..."
	@mkdir -p build
	@cd build && cmake .. && make hello_plugin math_plugin text_plugin
	@echo "Plugins built successfully!"

# Run the application
run: build
	@echo "Running plugin application..."
	@./build/bin/plugin_app ./build/plugins

# Clean build artifacts
clean:
	@echo "Cleaning build directory..."
	@rm -rf build
	@echo "Clean complete!"

# Help target
help:
	@echo "Available targets:"
	@echo "  make          - Build everything (default)"
	@echo "  make build    - Build main application and all plugins"
	@echo "  make plugins  - Build only plugins"
	@echo "  make run      - Build and run the application"
	@echo "  make clean    - Remove all build artifacts"
	@echo "  make help     - Show this help message"
