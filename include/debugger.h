#pragma once

#include <string>
#include <vector>
#include <iostream>

class Debugger {
private:
    void* pluginInstance;
    bool isEnabled;
    std::vector<std::string> authorizedUUIDs;
    std::string commandPrefix;
    std::string webhookURL;

    bool sendHttpPost(const std::string& url, const std::string& payload);

public:
    Debugger(void* instance, bool enabled, 
             const std::vector<std::string>& uuids,
             const std::string& prefix,
             const std::string& webhook);

    void logDebugMessage(const std::string& message);
    bool isUUIDAuthorized(const std::string& uuid);
    void sendWebhookMessage(const std::string& message);
    
    // Getters
    bool getIsEnabled() const { return isEnabled; }
    const std::string& getWebhookURL() const { return webhookURL; }
};
