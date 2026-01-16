#include "plugin_manager.h"
#include <iostream>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [plugin_directory]" << std::endl;
    std::cout << "  plugin_directory: Directory containing plugin libraries (default: ./plugins)" << std::endl;
}

void loadPluginsFromDirectory(PluginManager& manager, const std::string& directory) {
    if (!fs::exists(directory)) {
        std::cout << "Plugin directory does not exist: " << directory << std::endl;
        std::cout << "Creating directory..." << std::endl;
        fs::create_directories(directory);
        return;
    }
    
    std::cout << "Scanning for plugins in: " << directory << std::endl;
    
    int loadedCount = 0;
    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::string path = entry.path().string();
            std::string ext = entry.path().extension().string();
            
            // Check for shared library extensions
#ifdef _WIN32
            if (ext == ".dll") {
#else
            if (ext == ".so") {
#endif
                if (manager.loadPlugin(path)) {
                    loadedCount++;
                }
            }
        }
    }
    
    std::cout << "Loaded " << loadedCount << " plugin(s)" << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "==================================" << std::endl;
    std::cout << "C++ Plugin System with Code Injection" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << std::endl;
    
    // Determine plugin directory
    std::string pluginDir = "./plugins";
    if (argc > 1) {
        if (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h") {
            printUsage(argv[0]);
            return 0;
        }
        pluginDir = argv[1];
    }
    
    // Create plugin manager
    PluginManager manager;
    
    // Load all plugins from directory
    loadPluginsFromDirectory(manager, pluginDir);
    
    // Display loaded plugins
    auto loadedPlugins = manager.getLoadedPlugins();
    if (!loadedPlugins.empty()) {
        std::cout << "\nLoaded plugins:" << std::endl;
        for (const auto& name : loadedPlugins) {
            std::cout << "  - " << name << std::endl;
        }
        std::cout << std::endl;
    }
    
    // Execute all plugins (this is where code injection happens)
    if (!loadedPlugins.empty()) {
        manager.executeAll();
    } else {
        std::cout << "No plugins loaded. Nothing to execute." << std::endl;
        std::cout << "\nTo use plugins:" << std::endl;
        std::cout << "1. Build plugins using: make plugins" << std::endl;
        std::cout << "2. Run the program again" << std::endl;
    }
    
    // Cleanup happens automatically when PluginManager is destroyed
    std::cout << "\nShutting down..." << std::endl;
    
    return 0;
}
