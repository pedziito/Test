# Plugin Injector System

Et C++ plugin system hvor du kan lægge plugins ind, og de får automatisk injiceret debugger-kode under build-processen.

## 🚀 Funktioner

- **Automatisk Code Injection**: Debugger-kode bliver automatisk injiceret i alle plugins
- **Discord Webhook Integration**: Send notifikationer til Discord når plugins starter/stopper
- **UUID Authorization**: Sikkerhedssystem med autoriserede UUID'er
- **Dynamic Plugin Loading**: Plugins kompileres som shared libraries (.so filer)
- **Logging System**: Detaljeret logging af alle plugin-operationer

## 📁 Projekt Struktur

```
.
├── include/
│   ├── debugger.h           # Debugger interface
│   └── plugin_interface.h   # Plugin base interface
├── src/
│   ├── debugger.cpp         # Debugger implementation
│   ├── plugin_injector.cpp  # Code injection tool
│   └── plugin_loader.cpp    # Plugin loader application
├── plugins/                 # Original plugin source files
│   ├── example_plugin.cpp
│   └── another_plugin.cpp
├── plugins_injected/        # Generated: plugins med injected kode
├── build/                   # Generated: build artifacts
│   └── plugins_injected/    # Generated: compiled .so files
├── CMakeLists.txt          # Build configuration
└── build.sh                # Automated build script
```

## 🔧 Installation og Build

### Krav
- C++17 compiler (g++ eller clang++)
- CMake 3.15+
- libcurl (for webhook funktionalitet)

### Installer Dependencies (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install build-essential cmake libcurl4-openssl-dev
```

### Build Hele Systemet
```bash
chmod +x build.sh
./build.sh
```

Dette script vil:
1. Oprette build directory
2. Konfigurere projektet med CMake
3. Bygge plugin injector værktøjet
4. Køre injector på alle plugins i `plugins/` mappen
5. Bygge alle plugins med injected kode

## 📝 Sådan Bruger Du Det

### Web Interface (Anbefalet) 🌐

Den nemmeste måde at bruge systemet på er gennem den grafiske web-interface:

```bash
chmod +x start_web.sh
./start_web.sh
```

Åbn derefter din browser og gå til: `http://localhost:8080`

**Web Interface Features:**
- 🎯 **Drag & Drop**: Træk .cpp filer direkte ind i browseren
- ⚙️ **Live Konfiguration**: Rediger webhook URL, command prefix og UUID'er
- 🔨 **Build & Inject**: Klik på én knap for at injicere debugger-kode
- ⬇️ **Download**: Download det injecterede plugin direkte
- 📊 **Real-time Log**: Se hvad der sker i real-time
- 📋 **Plugin Liste**: Oversigt over alle processerede plugins

### Command Line Interface

Hvis du foretrækker kommandolinjen:

### 1. Opret Et Nyt Plugin

Opret en ny `.cpp` fil i `plugins/` mappen:

```cpp
#include "plugin_interface.h"
#include <iostream>

class MyPlugin : public IPlugin {
public:
    void onEnable() override {
        std::cout << "MyPlugin starting..." << std::endl;
    }

    void onDisable() override {
        std::cout << "MyPlugin stopping..." << std::endl;
    }

    std::string getName() const override {
        return "MyPlugin";
    }

    std::string getVersion() const override {
        return "1.0.0";
    }
};

extern "C" {
    IPlugin* createPlugin() { return new MyPlugin(); }
    void destroyPlugin(IPlugin* p) { delete p; }
}
```

### 2. Konfigurer Webhook og UUID

Rediger `src/plugin_injector.cpp` for at ændre:
- Webhook URL
- Command prefix
- Autoriserede UUID'er

```cpp
std::string webhookURL = "https://discord.com/api/webhooks/YOUR_WEBHOOK_URL";
std::string commandPrefix = "?cmd";
std::vector<std::string> authorizedUUIDs = {"your-uuid-here"};
```

### 3. Build Projektet

```bash
./build.sh
```

### 4. Kør Plugin Systemet

```bash
cd build
./plugin_loader
```

Du vil se:
- Alle plugins blive loaded
- `onEnable()` bliver kaldt på alle plugins
- Debugger sender beskeder til Discord webhook
- Tryk Enter for at stoppe og disable plugins

## 🔍 Hvad Sker Der Under Build?

### Før Injection (`plugins/example_plugin.cpp`):
```cpp
void onEnable() override {
    std::cout << "Plugin starting..." << std::endl;
}
```

### Efter Injection (`plugins_injected/example_plugin.cpp`):
```cpp
#include "debugger.h"  // <-- Added automatically

void onEnable() override {
    // [INJECTED] Debugger initialization
    std::vector<std::string> authorizedUUIDs = {"75b1032d-3064-4d04-97f5-96a6ad779214"};
    Debugger* debugger = new Debugger(this, true, authorizedUUIDs, "?cmd", "https://...");
    debugger->sendWebhookMessage("Plugin example_plugin is now enabled!");
    debugger->logDebugMessage("Plugin fully operational");
    // [END INJECTED]
    
    std::cout << "Plugin starting..." << std::endl;  // <-- Original kode
}
```

## 🎯 Manual Build Steps

Hvis du foretrækker at bygge manuelt:

```bash
# 1. Opret build directory
mkdir -p build && cd build

# 2. Konfigurer
cmake .. -DCMAKE_BUILD_TYPE=Release

# 3. Byg injector
cmake --build . --target plugin_injector

# 4. Kør injector
./plugin_injector ../plugins ../plugins_injected

# 5. Rekonfigurer og byg alt
cmake ..
cmake --build .

# 6. Kør plugin loader
./plugin_loader
```

## 🛠️ Kun Injector Tool

Hvis du kun vil bruge injector værktøjet uden at bygge plugins:

```bash
cd build
./plugin_injector /path/to/plugins /path/to/output
```

## 📊 Debugger Features

Debugger klassen tilbyder:
- **Logging**: Alle handlinger logges med timestamps
- **UUID Authorization**: Tjek om en UUID er autoriseret
- **Webhook Messages**: Send beskeder til Discord webhook
- **Enabled/Disabled State**: Kan slås til/fra

Eksempel på brug i din plugin:
```cpp
debugger->logDebugMessage("Custom log message");
bool authorized = debugger->isUUIDAuthorized("some-uuid");
debugger->sendWebhookMessage("Custom webhook notification");
```

## 🎨 Tilpasning

### Tilføj Din Egen Injection Logic

Rediger `src/plugin_injector.cpp`:
```cpp
std::string generateDebuggerInitCode() {
    // Tilføj din custom kode her
    oss << "    // Din egen kode\n";
    oss << "    myCustomFunction();\n";
    return oss.str();
}
```

### Tilføj Flere Plugins

Bare placer nye `.cpp` filer i `plugins/` og kør `./build.sh` igen!

## 🔐 Sikkerhed

⚠️ **VIGTIGT**: Dette projekt indeholder hardcoded webhook URLs og UUID'er. I produktion bør du:
- Bruge environment variables
- Gemme secrets sikkert
- Implementere proper authentication
- Ikke committe secrets til git

## 📜 License

Dette er et eksempel projekt til uddannelsesformål.

## 🤝 Support

Hvis du har spørgsmål eller problemer, opret et issue i repository'et.
