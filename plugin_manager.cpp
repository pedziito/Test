#include "plugin_manager.h"
#include <iostream>
#include <algorithm>

PluginManager::~PluginManager() {
    unloadAll();
}

LibHandle PluginManager::loadLibrary(const std::string& path) {
#ifdef _WIN32
    return LoadLibraryA(path.c_str());
#else
    return dlopen(path.c_str(), RTLD_LAZY);
#endif
}

void* PluginManager::getSymbol(LibHandle handle, const std::string& name) {
#ifdef _WIN32
    return (void*)GetProcAddress(handle, name.c_str());
#else
    return dlsym(handle, name.c_str());
#endif
}

void PluginManager::closeLibrary(LibHandle handle) {
#ifdef _WIN32
    FreeLibrary(handle);
#else
    dlclose(handle);
#endif
}

bool PluginManager::loadPlugin(const std::string& path) {
    std::cout << "Loading plugin: " << path << std::endl;
    
    // Load the shared library
    LibHandle handle = loadLibrary(path);
    if (!handle) {
#ifdef _WIN32
        std::cerr << "Failed to load library: " << path << std::endl;
#else
        std::cerr << "Failed to load library: " << path << " - " << dlerror() << std::endl;
#endif
        return false;
    }
    
    // Get the create and destroy functions
    CreatePluginFunc createFunc = (CreatePluginFunc)getSymbol(handle, "createPlugin");
    DestroyPluginFunc destroyFunc = (DestroyPluginFunc)getSymbol(handle, "destroyPlugin");
    
    if (!createFunc || !destroyFunc) {
        std::cerr << "Failed to find plugin functions in: " << path << std::endl;
        closeLibrary(handle);
        return false;
    }
    
    // Create the plugin instance
    IPlugin* plugin = createFunc();
    if (!plugin) {
        std::cerr << "Failed to create plugin instance from: " << path << std::endl;
        closeLibrary(handle);
        return false;
    }
    
    // Initialize the plugin
    if (!plugin->initialize()) {
        std::cerr << "Failed to initialize plugin: " << path << std::endl;
        destroyFunc(plugin);
        closeLibrary(handle);
        return false;
    }
    
    // Store plugin information
    PluginInfo info;
    info.path = path;
    info.handle = handle;
    info.plugin = plugin;
    info.createFunc = createFunc;
    info.destroyFunc = destroyFunc;
    plugins_.push_back(info);
    
    std::cout << "Successfully loaded plugin: " << plugin->getName() 
              << " (v" << plugin->getVersion() << ")" << std::endl;
    
    return true;
}

void PluginManager::unloadPlugin(const std::string& path) {
    auto it = std::find_if(plugins_.begin(), plugins_.end(),
        [&path](const PluginInfo& info) { return info.path == path; });
    
    if (it != plugins_.end()) {
        std::cout << "Unloading plugin: " << it->plugin->getName() << std::endl;
        it->plugin->shutdown();
        it->destroyFunc(it->plugin);
        closeLibrary(it->handle);
        plugins_.erase(it);
    }
}

void PluginManager::unloadAll() {
    for (auto& info : plugins_) {
        std::cout << "Unloading plugin: " << info.plugin->getName() << std::endl;
        info.plugin->shutdown();
        info.destroyFunc(info.plugin);
        closeLibrary(info.handle);
    }
    plugins_.clear();
}

void PluginManager::executeAll() {
    std::cout << "\n--- Executing all plugins (injecting code) ---" << std::endl;
    for (auto& info : plugins_) {
        std::cout << "Executing plugin: " << info.plugin->getName() << std::endl;
        info.plugin->execute();
    }
    std::cout << "--- Plugin execution complete ---\n" << std::endl;
}

std::vector<std::string> PluginManager::getLoadedPlugins() const {
    std::vector<std::string> names;
    for (const auto& info : plugins_) {
        names.push_back(info.plugin->getName());
    }
    return names;
}
