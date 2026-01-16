#include "plugin_interface.h"
#include <iostream>
#include <string>

class AnotherPlugin : public IPlugin {
private:
    std::string name;
    std::string version;

public:
    AnotherPlugin() : name("AnotherPlugin"), version("2.0.0") {
        std::cout << "[" << name << "] Constructor called" << std::endl;
    }

    ~AnotherPlugin() override {
        std::cout << "[" << name << "] Destructor called" << std::endl;
    }

    void onEnable() override {
        std::cout << "[" << name << "] Plugin starting up..." << std::endl;
        std::cout << "[" << name << "] Loading configuration..." << std::endl;
        std::cout << "[" << name << "] Ready to handle events!" << std::endl;
    }

    void onDisable() override {
        std::cout << "[" << name << "] Shutting down..." << std::endl;
        std::cout << "[" << name << "] Saving data..." << std::endl;
        std::cout << "[" << name << "] Goodbye!" << std::endl;
    }

    std::string getName() const override {
        return name;
    }

    std::string getVersion() const override {
        return version;
    }
};

// Export functions for dynamic loading
extern "C" {
    IPlugin* createPlugin() {
        return new AnotherPlugin();
    }

    void destroyPlugin(IPlugin* plugin) {
        delete plugin;
    }
}
