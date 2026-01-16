#include <iostream>
#include <dlfcn.h>
#include <filesystem>
#include <vector>
#include "plugin_interface.h"

namespace fs = std::filesystem;

class PluginLoader {
private:
    struct LoadedPlugin {
        void* handle;
        IPlugin* instance;
        std::string path;
    };

    std::vector<LoadedPlugin> loadedPlugins;

public:
    ~PluginLoader() {
        unloadAll();
    }

    bool loadPlugin(const std::string& path) {
        void* handle = dlopen(path.c_str(), RTLD_LAZY);
        if (!handle) {
            std::cerr << "Failed to load plugin: " << dlerror() << std::endl;
            return false;
        }

        // Clear any existing errors
        dlerror();

        // Load the create function
        CreatePluginFunc createFunc = (CreatePluginFunc)dlsym(handle, "createPlugin");
        const char* dlsym_error = dlerror();
        if (dlsym_error) {
            std::cerr << "Cannot load symbol createPlugin: " << dlsym_error << std::endl;
            dlclose(handle);
            return false;
        }

        // Create plugin instance
        IPlugin* plugin = createFunc();
        if (!plugin) {
            std::cerr << "Failed to create plugin instance" << std::endl;
            dlclose(handle);
            return false;
        }

        loadedPlugins.push_back({handle, plugin, path});
        std::cout << "✓ Loaded plugin: " << plugin->getName() 
                  << " v" << plugin->getVersion() << std::endl;

        return true;
    }

    void enableAll() {
        std::cout << "\n=== Enabling all plugins ===" << std::endl;
        for (auto& loaded : loadedPlugins) {
            std::cout << "\nEnabling: " << loaded.instance->getName() << std::endl;
            loaded.instance->onEnable();
        }
    }

    void disableAll() {
        std::cout << "\n=== Disabling all plugins ===" << std::endl;
        for (auto& loaded : loadedPlugins) {
            std::cout << "\nDisabling: " << loaded.instance->getName() << std::endl;
            loaded.instance->onDisable();
        }
    }

    void unloadAll() {
        for (auto& loaded : loadedPlugins) {
            DestroyPluginFunc destroyFunc = (DestroyPluginFunc)dlsym(loaded.handle, "destroyPlugin");
            if (destroyFunc) {
                destroyFunc(loaded.instance);
            }
            dlclose(loaded.handle);
        }
        loadedPlugins.clear();
    }
};

int main(int argc, char* argv[]) {
    std::cout << "=== Plugin System Demo ===" << std::endl;

    std::string pluginDir = "build/plugins_injected";
    if (argc > 1) {
        pluginDir = argv[1];
    }

    PluginLoader loader;

    // Load all .so files from the plugin directory
    if (fs::exists(pluginDir) && fs::is_directory(pluginDir)) {
        std::cout << "Loading plugins from: " << pluginDir << "\n" << std::endl;
        
        for (const auto& entry : fs::directory_iterator(pluginDir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".so") {
                std::cout << "Found plugin: " << entry.path().filename() << std::endl;
                loader.loadPlugin(entry.path().string());
            }
        }
    } else {
        std::cerr << "Plugin directory not found: " << pluginDir << std::endl;
        return 1;
    }

    // Enable all plugins
    loader.enableAll();

    std::cout << "\n[Press Enter to stop plugins...]" << std::endl;
    std::cin.get();

    // Disable all plugins
    loader.disableAll();

    std::cout << "\n=== Plugin system shutdown complete ===" << std::endl;
    return 0;
}
