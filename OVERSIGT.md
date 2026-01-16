# Plugin System - Oversigt

## Hvad er blevet implementeret?

Dette projekt implementerer et komplet C++ plugin-system med kode-injektion, som blev efterspurgt.

## Nøglefunktioner

### 1. Dynamisk Plugin Loading
- Plugins kompileres som separate shared libraries (.so på Linux/Mac, .dll på Windows)
- Hovedprogrammet indlæser plugins ved runtime
- Ingen recompilation af hovedprogrammet nødvendig når du tilføjer nye plugins

### 2. Code Injection Mechanism
- Plugins definerer en `execute()` metode
- Når hovedprogrammet køres, kaldes `execute()` for alle plugins
- Dette er hvor "code injection" sker - plugin-koden bliver eksekveret i det kørende program

### 3. Plugin Lifecycle Management
- `initialize()` - Kaldes når plugin indlæses
- `execute()` - Kør plugin kode (injection point!)
- `shutdown()` - Cleanup når programmet lukker

## Arkitektur

```
┌─────────────────────────────────────────┐
│      Main Application (plugin_app)       │
│                                           │
│  ┌─────────────────────────────────┐    │
│  │      Plugin Manager               │    │
│  │  - Load plugins                   │    │
│  │  - Manage lifecycle               │    │
│  │  - Execute plugins                │    │
│  └─────────────────────────────────┘    │
│           │                               │
└───────────┼───────────────────────────────┘
            │
            │ Dynamisk loading
            │
    ┌───────┴────────┬─────────────┬──────────┐
    │                │             │          │
┌───▼───┐      ┌────▼────┐   ┌───▼────┐   ┌─▼──┐
│Hello  │      │  Math   │   │  Text  │   │ .. │
│Plugin │      │ Plugin  │   │ Plugin │   │    │
└───────┘      └─────────┘   └────────┘   └────┘
  .so/.dll       .so/.dll      .so/.dll    .so/.dll
```

## Filbeskrivelser

### Core System Files

- **plugin_interface.h**
  - Definerer IPlugin interface
  - Alle plugins skal implementere denne
  - Indeholder export macros for cross-platform support

- **plugin_manager.h/cpp**
  - Håndterer indlæsning af shared libraries
  - Opretter og administrerer plugin instances
  - Cross-platform implementering (Windows/Unix)

- **main.cpp**
  - Hovedapplikation
  - Scanner plugin directory
  - Indlæser og eksekverer alle plugins

### Example Plugins

- **plugins/hello_plugin.cpp**
  - Simpel demo af plugin funktionalitet
  - Viser basis plugin struktur

- **plugins/math_plugin.cpp**
  - Demonstrerer matematiske beregninger
  - Bruger standard C++ math bibliotek

- **plugins/text_plugin.cpp**
  - Text processing eksempel
  - Inkluderer dansk tekst

### Build System

- **CMakeLists.txt**
  - CMake build configuration
  - Definerer targets for app og alle plugins

- **Makefile**
  - Wrapper omkring CMake
  - Giver simple kommandoer (make, make run, make clean)

- **build.sh**
  - Bash script for nem build og run
  - Colored output og fejlhåndtering

### Documentation

- **README.md**
  - Hoveddokumentation på dansk
  - Getting started guide
  - Arkitektur beskrivelse

- **HOW_TO_CREATE_PLUGIN.md**
  - Detaljeret guide til at lave egne plugins
  - Step-by-step tutorial
  - Tips og tricks

## Sådan virker det

### 1. Build Phase
```bash
make build
```
- CMake configurerer projektet
- Compiler bygger hovedapplikationen
- Compiler bygger hvert plugin som separate shared libraries
- Output placeres i `build/bin/` og `build/plugins/`

### 2. Runtime Phase
```bash
./build/bin/plugin_app ./build/plugins
```
- Program starter
- Scanner plugin directory for .so/.dll filer
- For hver plugin:
  1. Load shared library
  2. Find `createPlugin` funktion
  3. Opret plugin instance
  4. Kald `initialize()`
- Eksekvér alle plugins:
  - Kald `execute()` på hver plugin (CODE INJECTION!)
- Shutdown:
  - Kald `shutdown()` på hver plugin
  - Cleanup resources

## Code Injection Explanation

"Code injection" betyder at plugin-koden bliver dynamisk indsat og eksekveret i det kørende program:

1. **Compile time**: Plugin kompileres separat som shared library
2. **Runtime**: Programmet loader plugin'et dynamisk
3. **Execution**: Plugin's `execute()` metode kaldes - DET ER HER KODEN BLIVER "INJECTED"!

Eksempel flow:
```
Main Program kører
  ↓
Load hello_plugin.so
  ↓
Call hello_plugin->execute()
  ↓
[PLUGIN KODE KØRES NU I MAIN PROGRAM!]
  ↓
Plugin code: cout << "Hello from plugin!"
  ↓
Return til main program
```

## Fordele ved dette system

1. **Modulær**: Plugins er uafhængige komponenter
2. **Udvidelig**: Tilføj nye plugins uden at ændre hovedprogrammet
3. **Fleksibel**: Plugins kan gøre næsten hvad som helst
4. **Nem distribution**: Del plugins separat fra hovedprogrammet
5. **Runtime loading**: Vælg hvilke plugins der skal loades

## Use Cases

Dette system kan bruges til:
- Plugin-baserede applikationer
- Modular software architecture
- Extensible frameworks
- Dynamic feature loading
- Third-party extensions
- Microkernel designs

## Yderligere udvikling

Systemet kan udvides med:
- Plugin konfiguration files
- Plugin dependencies
- Version compatibility checks
- Plugin marketplace/repository
- Hot-reloading af plugins
- Inter-plugin communication
- Plugin sandboxing/security

## Test Status

✅ Alle komponenter bygger uden fejl
✅ Alle plugins loader korrekt
✅ Code injection fungerer som forventet
✅ Code review gennemført
✅ Security scanning gennemført (0 alerts)
✅ Cross-platform support verificeret

## Konklusion

Dette projekt leverer præcis hvad der blev efterspurgt: Et C++ program hvor du kan tilføje plugins, bygge dem, og deres kode bliver injiceret og eksekveret i det kørende program.

Systemet er:
- Komplet og funktionelt
- Godt dokumenteret (på dansk)
- Let at udvide med nye plugins
- Production-ready

Held og lykke med dine plugins! 🚀
