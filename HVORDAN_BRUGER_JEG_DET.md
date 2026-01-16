# Hvordan Bruger Du Det Injecterede Plugin

## ❌ FORKERT måde:
- Prøve at åbne JAR filen i IntelliJ som kildekode
- JAR filer er KOMPILEREDE binære filer, ikke kildekode

## ✅ RIGTIG måde:

### 1. Test Pluginet på din Minecraft Server
```bash
# Kopier til din servers plugins mappe
cp injected_FiskeSystem.jar /path/to/minecraft-server/plugins/

# Start serveren
# Pluginet loader automatisk med Debugger funktionalitet
```

### 2. Inspicér JAR Filen (Optional)
```bash
# Se hvad der er inde i JAR filen
./inspect_jar.sh web_injected/injected_FiskeSystem.jar

# Eller manuelt udpak den
mkdir temp_inspect
cd temp_inspect
unzip ../injected_FiskeSystem.jar
ls -la
```

### 3. Se Kildekoden (Hvis du vil redigere)
Du skal have den **originale kildekode** projektet:
```
FiskeSystem/
├── src/
│   └── main/
│       └── java/
│           └── com/example/FiskeSystem.java  ← DENNE kan du redigere
├── pom.xml
└── ...
```

Efter du kompilerer → får du FiskeSystem.jar
Efter du injector    → får du injected_FiskeSystem.jar


## 🔍 Hvad Kan Du Gøre:

1. **Inspicere indholdet:**
   ```bash
   unzip -l injected_FiskeSystem.jar
   ```

2. **Udpakke og se filerne:**
   ```bash
   unzip injected_FiskeSystem.jar -d unpacked/
   cd unpacked/
   ```

3. **Dekompilere (se bytecode som Java):**
   - Brug JD-GUI eller IntelliJ's "Add as Library" funktion
   - Højreklik på JAR → "Add as Library" → så kan du browse klasserne

4. **Bruge den direkte:**
   - Læg den i Minecraft server's plugins/ mappe
   - Plugin loader automatisk med Debugger


## 📝 Opsummering:

**JAR fil = Færdigt produkt** (ligesom en .exe fil)
- Du "kører" den, ikke "redigerer" den
- Hvis du vil ændre koden, skal du have kildekode projektet
- Det injecterede plugin virker præcis som originalen + ekstra Debugger funktionalitet

**Webhook bliver aktiveret når:**
- Serveren starter → sender besked til Discord
- Plugin enable/disable → sender besked til Discord
