#include "debugger.h"
#include <curl/curl.h>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <sstream>

Debugger::Debugger(void* instance, bool enabled, 
                   const std::vector<std::string>& uuids,
                   const std::string& prefix,
                   const std::string& webhook)
    : pluginInstance(instance), isEnabled(enabled), 
      authorizedUUIDs(uuids), commandPrefix(prefix), webhookURL(webhook) {
    
    logDebugMessage("Debugger initialized");
}

void Debugger::logDebugMessage(const std::string& message) {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::cout << "[Debugger " << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") 
              << "] " << message << std::endl;
}

bool Debugger::isUUIDAuthorized(const std::string& uuid) {
    auto it = std::find(authorizedUUIDs.begin(), authorizedUUIDs.end(), uuid);
    bool authorized = (it != authorizedUUIDs.end());
    
    if (authorized) {
        logDebugMessage("UUID authorization approved: " + uuid);
    } else {
        logDebugMessage("UUID authorization denied: " + uuid);
    }
    
    return authorized;
}

// Callback function for libcurl
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

bool Debugger::sendHttpPost(const std::string& url, const std::string& payload) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        logDebugMessage("Failed to initialize CURL");
        return false;
    }

    std::string response;
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    long response_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        logDebugMessage("Webhook request failed: " + std::string(curl_easy_strerror(res)));
        return false;
    }

    logDebugMessage("Webhook response: " + std::to_string(response_code));
    return (response_code >= 200 && response_code < 300);
}

void Debugger::sendWebhookMessage(const std::string& message) {
    if (!isEnabled) {
        logDebugMessage("Debugger is disabled, no webhook message sent.");
        return;
    }

    // Escape special characters in message for JSON
    std::string escaped_message;
    for (char c : message) {
        switch (c) {
            case '"': escaped_message += "\\\""; break;
            case '\\': escaped_message += "\\\\"; break;
            case '\n': escaped_message += "\\n"; break;
            case '\r': escaped_message += "\\r"; break;
            case '\t': escaped_message += "\\t"; break;
            default: escaped_message += c; break;
        }
    }

    std::string payload = "{\"content\": \"" + escaped_message + "\"}";
    
    if (sendHttpPost(webhookURL, payload)) {
        logDebugMessage("Webhook message sent successfully");
    } else {
        logDebugMessage("Failed to send webhook message");
    }
}
