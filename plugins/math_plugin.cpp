#include "../plugin_interface.h"
#include <iostream>
#include <cmath>

// Example plugin that performs calculations
class MathPlugin : public IPlugin {
public:
    std::string getName() const override {
        return "MathPlugin";
    }
    
    std::string getVersion() const override {
        return "1.0.0";
    }
    
    bool initialize() override {
        std::cout << "[MathPlugin] Initializing..." << std::endl;
        return true;
    }
    
    void shutdown() override {
        std::cout << "[MathPlugin] Shutting down..." << std::endl;
    }
    
    void execute() override {
        std::cout << "[MathPlugin] Executing math calculations..." << std::endl;
        
        // Perform some calculations
        double pi = 3.14159265359;
        double result = std::sin(pi / 2.0);
        
        std::cout << "[MathPlugin] sin(π/2) = " << result << std::endl;
        std::cout << "[MathPlugin] √2 = " << std::sqrt(2.0) << std::endl;
        std::cout << "[MathPlugin] 2³ = " << std::pow(2, 3) << std::endl;
    }
};

// Plugin export functions
extern "C" {
    PLUGIN_EXPORT IPlugin* createPlugin() {
        return new MathPlugin();
    }
    
    PLUGIN_EXPORT void destroyPlugin(IPlugin* plugin) {
        delete plugin;
    }
}
