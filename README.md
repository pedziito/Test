# C++ Plugin System med Code Injection

Et komplet plugin-system i C++ der tillader dynamisk indlæsning af plugins og kode-injektion.

## Hvad er det?

Dette projekt demonstrerer et plugin-system hvor:
- Du kan lave plugins som separate shared libraries (.so på Linux, .dll på Windows)
- Programmet dynamisk indlæser plugins ved runtime
- Plugin-koden bliver "injiceret" og eksekveret i det kørende program
- Plugins kan tilføjes uden at recompile hovedprogrammet

## Funktioner

- **Dynamisk plugin loading**: Indlæs plugins fra en mappe ved runtime
- **Plugin interface**: Klar interface som alle plugins skal implementere
- **Lifecycle management**: Initialize, execute, og shutdown hooks for plugins
- **Cross-platform**: Virker på Linux, macOS, og Windows
- **Eksempel plugins**: 3 forskellige eksempel-plugins inkluderet
  - `HelloPlugin`: Viser en simpel hilsen
  - `MathPlugin`: Udfører matematiske beregninger
  - `TextPlugin`: Behandler tekst (med dansk tekst!)

## Projektstruktur

```
.
├── plugin_interface.h      # Plugin interface definition
├── plugin_manager.h/cpp    # Manager til at håndtere plugins
├── main.cpp                # Hoved-applikation
├── plugins/                # Plugin implementeringer
│   ├── hello_plugin.cpp
│   ├── math_plugin.cpp
│   └── text_plugin.cpp
├── CMakeLists.txt          # CMake build configuration
├── Makefile                # Nem build wrapper
└── README.md               # Denne fil
```

## Kom i gang

### Forudsætninger

- C++ compiler med C++17 support (g++, clang, eller MSVC)
- CMake 3.10 eller nyere
- Make (valgfrit, for nem build)

### Build og Kør

#### Nem måde (med Make):

```bash
# Build alt
make

# Eller build og kør direkte
make run
```

#### Manuel måde (med CMake):

```bash
# Opret build directory
mkdir build
cd build

# Configure og build
cmake ..
make

# Kør programmet
./bin/plugin_app ./plugins
```

### Output eksempel

```
==================================
C++ Plugin System with Code Injection
==================================

Scanning for plugins in: ./build/plugins
Loading plugin: ./build/plugins/hello_plugin.so
[HelloPlugin] Initializing...
Successfully loaded plugin: HelloPlugin (v1.0.0)
...

--- Executing all plugins (injecting code) ---
Executing plugin: HelloPlugin
[HelloPlugin] Hello from the injected plugin code!
[HelloPlugin] This code was dynamically loaded and executed!
...
--- Plugin execution complete ---
```

## Sådan laver du dit eget plugin

1. Opret en ny `.cpp` fil i `plugins/` mappen
2. Include plugin interface: `#include "../plugin_interface.h"`
3. Lav en klasse der arver fra `IPlugin`
4. Implementer alle required metoder:
   - `getName()` - Plugin navn
   - `getVersion()` - Version
   - `initialize()` - Initialization kode
   - `shutdown()` - Cleanup kode
   - `execute()` - Din injicerede kode her!
5. Eksporter plugin factory funktioner:

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

6. Tilføj dit plugin til `CMakeLists.txt`:

```cmake
add_library(your_plugin SHARED
    plugins/your_plugin.cpp
)
target_include_directories(your_plugin PRIVATE ${CMAKE_SOURCE_DIR})
set_target_properties(your_plugin PROPERTIES PREFIX "")
```

7. Rebuild projektet: `make`

## Arkitektur

### Plugin Interface

Alle plugins implementerer `IPlugin` interface med metoder for:
- Metadata (navn, version)
- Lifecycle (initialize, shutdown)
- Execution (hvor din kode bliver injiceret)

### Plugin Manager

`PluginManager` klassen håndterer:
- Indlæsning af shared libraries
- Oprettelse af plugin instances
- Lifecycle management
- Eksekvering af alle plugins

### Main Application

Hovedprogrammet:
1. Scanner plugin directory
2. Indlæser alle plugin libraries
3. Initialiserer hver plugin
4. Eksekverer alle plugins (code injection!)
5. Cleaner op ved shutdown

## Make targets

```bash
make          # Build alt (default)
make build    # Build hovedprogram og alle plugins
make plugins  # Build kun plugins
make run      # Build og kør applikationen
make clean    # Fjern alle build artifacts
make help     # Vis hjælp
```

## Platform noter

### Linux/macOS
- Plugins kompileres som `.so` filer
- Bruger `dlopen()` til dynamisk loading

### Windows
- Plugins kompileres som `.dll` filer  
- Bruger `LoadLibrary()` til dynamisk loading

## Licens

Dette er et eksempel projekt for uddannelsesformål.
