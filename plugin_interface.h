#ifndef PLUGIN_INTERFACE_H
#define PLUGIN_INTERFACE_H

#include <string>

// Base class for all plugins
class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    // Plugin metadata
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    
    // Plugin lifecycle
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    
    // Plugin execution - this is where the injected code runs
    virtual void execute() = 0;
};

// Plugin factory function type
typedef IPlugin* (*CreatePluginFunc)();
typedef void (*DestroyPluginFunc)(IPlugin*);

// Macro to help create plugin exports
#ifdef _WIN32
    #define PLUGIN_EXPORT __declspec(dllexport)
#else
    #define PLUGIN_EXPORT
#endif

#endif // PLUGIN_INTERFACE_H
