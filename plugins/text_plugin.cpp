#include "../plugin_interface.h"
#include <iostream>
#include <string>
#include <cctype>

// Example plugin that processes text
class TextPlugin : public IPlugin {
private:
    std::string message_;
    
public:
    std::string getName() const override {
        return "TextPlugin";
    }
    
    std::string getVersion() const override {
        return "1.0.0";
    }
    
    bool initialize() override {
        std::cout << "[TextPlugin] Initializing..." << std::endl;
        message_ = "Dette er en tekst fra plugin!";
        return true;
    }
    
    void shutdown() override {
        std::cout << "[TextPlugin] Shutting down..." << std::endl;
    }
    
    void execute() override {
        std::cout << "[TextPlugin] Processing text..." << std::endl;
        std::cout << "[TextPlugin] Original: " << message_ << std::endl;
        
        // Convert to uppercase
        std::string upper = message_;
        for (char& c : upper) {
            c = std::toupper(c);
        }
        std::cout << "[TextPlugin] Uppercase: " << upper << std::endl;
        
        // Count characters
        std::cout << "[TextPlugin] Length: " << message_.length() << " characters" << std::endl;
    }
};

// Plugin export functions
extern "C" {
    PLUGIN_EXPORT IPlugin* createPlugin() {
        return new TextPlugin();
    }
    
    PLUGIN_EXPORT void destroyPlugin(IPlugin* plugin) {
        delete plugin;
    }
}
