#include "plugin_interface.h"
#include <iostream>
#include <string>

class ExamplePlugin : public IPlugin {
private:
    std::string name;
    std::string version;

public:
    ExamplePlugin() : name("ExamplePlugin"), version("1.0.0") {
        std::cout << "[" << name << "] Constructor called" << std::endl;
    }

    ~ExamplePlugin() override {
        std::cout << "[" << name << "] Destructor called" << std::endl;
    }

    void onEnable() override {
        std::cout << "[" << name << "] Plugin is being enabled..." << std::endl;
        std::cout << "[" << name << "] Performing initialization..." << std::endl;
    }

    void onDisable() override {
        std::cout << "[" << name << "] Plugin is being disabled..." << std::endl;
        std::cout << "[" << name << "] Cleaning up resources..." << std::endl;
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
        return new ExamplePlugin();
    }

    void destroyPlugin(IPlugin* plugin) {
        delete plugin;
    }
}
