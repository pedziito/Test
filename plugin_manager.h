#ifndef PLUGIN_MANAGER_H
#define PLUGIN_MANAGER_H

#include "plugin_interface.h"
#include <string>
#include <vector>
#include <memory>

#ifdef _WIN32
    #include <windows.h>
    typedef HMODULE LibHandle;
#else
    #include <dlfcn.h>
    typedef void* LibHandle;
#endif

// Plugin container
struct PluginInfo {
    std::string path;
    LibHandle handle;
    IPlugin* plugin;
    CreatePluginFunc createFunc;
    DestroyPluginFunc destroyFunc;
};

// Manager for loading and managing plugins
class PluginManager {
public:
    PluginManager() = default;
    ~PluginManager();
    
    // Load a plugin from a shared library
    bool loadPlugin(const std::string& path);
    
    // Unload a specific plugin
    void unloadPlugin(const std::string& path);
    
    // Unload all plugins
    void unloadAll();
    
    // Execute all loaded plugins (inject their code)
    void executeAll();
    
    // Get list of loaded plugins
    std::vector<std::string> getLoadedPlugins() const;
    
private:
    std::vector<PluginInfo> plugins_;
    
    LibHandle loadLibrary(const std::string& path);
    void* getSymbol(LibHandle handle, const std::string& name);
    void closeLibrary(LibHandle handle);
};

#endif // PLUGIN_MANAGER_H
