#include "../plugin_interface.h"
#include <iostream>

// Example plugin that prints a greeting
class HelloPlugin : public IPlugin {
public:
    std::string getName() const override {
        return "HelloPlugin";
    }
    
    std::string getVersion() const override {
        return "1.0.0";
    }
    
    bool initialize() override {
        std::cout << "[HelloPlugin] Initializing..." << std::endl;
        return true;
    }
    
    void shutdown() override {
        std::cout << "[HelloPlugin] Shutting down..." << std::endl;
    }
    
    void execute() override {
        std::cout << "[HelloPlugin] Hello from the injected plugin code!" << std::endl;
        std::cout << "[HelloPlugin] This code was dynamically loaded and executed!" << std::endl;
    }
};

// Plugin export functions
extern "C" {
    PLUGIN_EXPORT IPlugin* createPlugin() {
        return new HelloPlugin();
    }
    
    PLUGIN_EXPORT void destroyPlugin(IPlugin* plugin) {
        delete plugin;
    }
}
