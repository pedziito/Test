#pragma once

#include <string>

// Base interface for all plugins
class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    // Plugin lifecycle methods
    virtual void onEnable() = 0;
    virtual void onDisable() = 0;
    
    // Plugin information
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
};

// Factory function type that creates plugin instances
typedef IPlugin* (*CreatePluginFunc)();
typedef void (*DestroyPluginFunc)(IPlugin*);
