# Sådan laver du dit eget plugin

Dette dokument viser dig trin-for-trin hvordan du laver dit eget plugin.

## Eksempel: Lav et Logger Plugin

Lad os lave et plugin der logger forskellige typer beskeder.

### Trin 1: Opret plugin filen

Opret en ny fil: `plugins/logger_plugin.cpp`

```cpp
#include "../plugin_interface.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

class LoggerPlugin : public IPlugin {
private:
    std::string getCurrentTime() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
public:
    std::string getName() const override {
        return "LoggerPlugin";
    }
    
    std::string getVersion() const override {
        return "1.0.0";
    }
    
    bool initialize() override {
        std::cout << "[LoggerPlugin] Initializing logger system..." << std::endl;
        return true;
    }
    
    void shutdown() override {
        std::cout << "[LoggerPlugin] Logger system shutting down..." << std::endl;
    }
    
    void execute() override {
        std::cout << "[LoggerPlugin] Starting log entries..." << std::endl;
        
        // Log forskellige typer beskeder
        std::cout << "[LoggerPlugin] [INFO] " << getCurrentTime() 
                  << " - Application started successfully" << std::endl;
        
        std::cout << "[LoggerPlugin] [DEBUG] " << getCurrentTime() 
                  << " - Debug information available" << std::endl;
        
        std::cout << "[LoggerPlugin] [WARNING] " << getCurrentTime() 
                  << " - This is a warning message" << std::endl;
        
        std::cout << "[LoggerPlugin] Log entries complete!" << std::endl;
    }
};

// Plugin export funktioner - VIGTIGT!
extern "C" {
    PLUGIN_EXPORT IPlugin* createPlugin() {
        return new LoggerPlugin();
    }
    
    PLUGIN_EXPORT void destroyPlugin(IPlugin* plugin) {
        delete plugin;
    }
}
```

### Trin 2: Tilføj til CMakeLists.txt

Åbn `CMakeLists.txt` og tilføj disse linjer før den sidste `add_custom_target`:

```cmake
# Logger Plugin
add_library(logger_plugin SHARED
    plugins/logger_plugin.cpp
)
target_include_directories(logger_plugin PRIVATE ${CMAKE_SOURCE_DIR})
set_target_properties(logger_plugin PROPERTIES PREFIX "")
```

### Trin 3: Build dit nye plugin

```bash
# Build alt igen
make build

# Eller kun dit plugin
cd build
make logger_plugin
```

### Trin 4: Test det

```bash
make run
```

Du skulle nu se output fra dit LoggerPlugin!

## Plugin Struktur

Hvert plugin skal have:

### 1. Klasse der arver fra IPlugin

```cpp
class YourPlugin : public IPlugin {
    // Din implementation her
};
```

### 2. Implementer alle metoder

- `getName()` - Returnerer plugin navnet
- `getVersion()` - Returnerer version (f.eks. "1.0.0")
- `initialize()` - Kaldes når plugin indlæses (return true hvis success)
- `shutdown()` - Kaldes når plugin lukkes ned
- `execute()` - DIN KODE HER! Dette er hvor code injection sker!

### 3. Export funktioner

```cpp
extern "C" {
    PLUGIN_EXPORT IPlugin* createPlugin() {
        return new YourPlugin();
    }
    
    PLUGIN_EXPORT void destroyPlugin(IPlugin* plugin) {
        delete plugin;
    }
}
```

Disse funktioner gør det muligt for hovedprogrammet at dynamisk oprette og ødelægge dit plugin.

## Tips og tricks

### Brug private members

```cpp
class MyPlugin : public IPlugin {
private:
    int counter_;
    std::string data_;
    
public:
    bool initialize() override {
        counter_ = 0;
        data_ = "initial data";
        return true;
    }
    
    void execute() override {
        counter_++;
        std::cout << "Counter: " << counter_ << std::endl;
    }
};
```

### Fejlhåndtering

```cpp
bool initialize() override {
    try {
        // Din initialization kode
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}
```

### Brug standard biblioteker

Du kan inkludere og bruge alle C++ standard biblioteker:

```cpp
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
// osv...
```

## Flere eksempler

Se de eksisterende plugins for inspiration:
- `plugins/hello_plugin.cpp` - Simpel hilsen
- `plugins/math_plugin.cpp` - Matematiske beregninger
- `plugins/text_plugin.cpp` - Text processing

## Fejlfinding

### Plugin indlæses ikke

1. Check at export funktionerne er korrekt (`extern "C"` og `PLUGIN_EXPORT`)
2. Check at klassen implementerer alle IPlugin metoder
3. Check at pluginet er tilføjet i CMakeLists.txt
4. Rebuild: `make clean && make build`

### Compile fejl

- Sørg for `#include "../plugin_interface.h"` er øverst i filen
- Check at alle metoder har `override` keyword
- Check at metoderne returnerer korrekte typer

### Runtime fejl

- Check at `initialize()` returnerer `true`
- Check at du ikke bruger uninitialiserede variabler
- Tilføj debug output for at se hvor fejlen sker

## Næste skridt

Nu kan du lave dine egne plugins! Prøv at lave:
- Et plugin der læser/skriver filer
- Et plugin der arbejder med netværk
- Et plugin der integrerer med eksterne biblioteker
- Et plugin der udfører komplekse beregninger

God fornøjelse! 🚀
