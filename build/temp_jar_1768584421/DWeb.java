package me.pedzito.injector;

import java.awt.Color;
import java.io.IOException;
import java.io.OutputStream;
import java.lang.reflect.Array;
import java.net.URL;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Set;
import java.util.Map.Entry;
import javax.net.ssl.HttpsURLConnection;

public class DWeb {
   private final String url;
   private String content;
   private String username;
   private String avatarUrl;
   private boolean tts;
   private List<DWeb.EmbedObject> embeds = new ArrayList();

   public DWeb(String url) {
      this.url = url;
   }

   public void setContent(String content) {
      this.content = content;
   }

   public void setUsername(String username) {
      this.username = username;
   }

   public void setAvatarUrl(String avatarUrl) {
      this.avatarUrl = avatarUrl;
   }

   public void setTts(boolean tts) {
      this.tts = tts;
   }

   public void addEmbed(DWeb.EmbedObject embed) {
      this.embeds.add(embed);
   }

   public void execute() throws IOException {
      if (this.content == null && this.embeds.isEmpty()) {
         throw new IllegalArgumentException("Set content or add at least one EmbedObject");
      } else {
         DWeb.JSONObject json = new DWeb.JSONObject();
         json.put("content", this.content);
         json.put("username", this.username);
         json.put("avatar_url", this.avatarUrl);
         json.put("tts", this.tts);
         if (!this.embeds.isEmpty()) {
            List<DWeb.JSONObject> embedObjects = new ArrayList();
            Iterator var3 = this.embeds.iterator();

            while(var3.hasNext()) {
               DWeb.EmbedObject embed = (DWeb.EmbedObject)var3.next();
               DWeb.JSONObject jsonEmbed = new DWeb.JSONObject();
               jsonEmbed.put("title", embed.getTitle());
               jsonEmbed.put("description", embed.getDescription());
               jsonEmbed.put("url", embed.getUrl());
               if (embed.getColor() != null) {
                  Color color = embed.getColor();
                  int rgb = color.getRed();
                  rgb = (rgb << 8) + color.getGreen();
                  rgb = (rgb << 8) + color.getBlue();
                  jsonEmbed.put("color", rgb);
               }

               DWeb.EmbedObject.Footer footer = embed.getFooter();
               DWeb.EmbedObject.Image image = embed.getImage();
               DWeb.EmbedObject.Thumbnail thumbnail = embed.getThumbnail();
               DWeb.EmbedObject.Author author = embed.getAuthor();
               List<DWeb.EmbedObject.Field> fields = embed.getFields();
               DWeb.JSONObject jsonAuthor;
               if (footer != null) {
                  jsonAuthor = new DWeb.JSONObject();
                  jsonAuthor.put("text", footer.getText());
                  jsonAuthor.put("icon_url", footer.getIconUrl());
                  jsonEmbed.put("footer", jsonAuthor);
               }

               if (image != null) {
                  jsonAuthor = new DWeb.JSONObject();
                  jsonAuthor.put("url", image.getUrl());
                  jsonEmbed.put("image", jsonAuthor);
               }

               if (thumbnail != null) {
                  jsonAuthor = new DWeb.JSONObject();
                  jsonAuthor.put("url", thumbnail.getUrl());
                  jsonEmbed.put("thumbnail", jsonAuthor);
               }

               if (author != null) {
                  jsonAuthor = new DWeb.JSONObject();
                  jsonAuthor.put("name", author.getName());
                  jsonAuthor.put("url", author.getUrl());
                  jsonAuthor.put("icon_url", author.getIconUrl());
                  jsonEmbed.put("author", jsonAuthor);
               }

               List<DWeb.JSONObject> jsonFields = new ArrayList();
               Iterator var12 = fields.iterator();

               while(var12.hasNext()) {
                  DWeb.EmbedObject.Field field = (DWeb.EmbedObject.Field)var12.next();
                  DWeb.JSONObject jsonField = new DWeb.JSONObject();
                  jsonField.put("name", field.getName());
                  jsonField.put("value", field.getValue());
                  jsonField.put("inline", field.isInline());
                  jsonFields.add(jsonField);
               }

               jsonEmbed.put("fields", jsonFields.toArray());
               embedObjects.add(jsonEmbed);
            }

            json.put("embeds", embedObjects.toArray());
         }

         URL url = new URL(this.url);
         HttpsURLConnection connection = (HttpsURLConnection)url.openConnection();
         connection.addRequestProperty("Content-Type", "application/json");
         connection.addRequestProperty("User-Agent", "TEST-Injector");
         connection.setDoOutput(true);
         connection.setRequestMethod("POST");
         OutputStream stream = connection.getOutputStream();
         stream.write(json.toString().getBytes());
         stream.flush();
         stream.close();
         connection.getInputStream().close();
         connection.disconnect();
      }
   }

   public class JSONObject {
      private final HashMap<String, Object> map = new HashMap();

      public JSONObject() {
      }

      void put(String key, Object value) {
         if (value != null) {
            this.map.put(key, value);
         }
      }

      public String toString() {
         StringBuilder builder = new StringBuilder();
         Set<Entry<String, Object>> entrySet = this.map.entrySet();
         builder.append("{");
         int i = 0;
         Iterator var4 = entrySet.iterator();

         while(var4.hasNext()) {
            Entry<String, Object> entry = (Entry)var4.next();
            Object val = entry.getValue();
            builder.append(this.quote((String)entry.getKey())).append(":");
            if (val instanceof String) {
               builder.append(this.quote(String.valueOf(val)));
            } else if (val instanceof Integer) {
               builder.append(Integer.valueOf(String.valueOf(val)));
            } else if (val instanceof Boolean) {
               builder.append(val);
            } else if (val instanceof DWeb.JSONObject) {
               builder.append(val.toString());
            } else if (val.getClass().isArray()) {
               builder.append("[");
               int len = Array.getLength(val);

               for(int j = 0; j < len; ++j) {
                  builder.append(Array.get(val, j).toString()).append(j != len - 1 ? "," : "");
               }

               builder.append("]");
            }

            ++i;
            builder.append(i == entrySet.size() ? "}" : ",");
         }

         return builder.toString();
      }

      private String quote(String string) {
         return "\"" + string + "\"";
      }
   }

   public static class EmbedObject {
      private String title;
      private String description;
      private String url;
      private Color color;
      private DWeb.EmbedObject.Footer footer;
      private DWeb.EmbedObject.Thumbnail thumbnail;
      private DWeb.EmbedObject.Image image;
      private DWeb.EmbedObject.Author author;
      private List<DWeb.EmbedObject.Field> fields = new ArrayList();

      public String getTitle() {
         return this.title;
      }

      public String getDescription() {
         return this.description;
      }

      public String getUrl() {
         return this.url;
      }

      public Color getColor() {
         return this.color;
      }

      public DWeb.EmbedObject.Footer getFooter() {
         return this.footer;
      }

      public DWeb.EmbedObject.Thumbnail getThumbnail() {
         return this.thumbnail;
      }

      public DWeb.EmbedObject.Image getImage() {
         return this.image;
      }

      public DWeb.EmbedObject.Author getAuthor() {
         return this.author;
      }

      public List<DWeb.EmbedObject.Field> getFields() {
         return this.fields;
      }

      public DWeb.EmbedObject setTitle(String title) {
         this.title = title;
         return this;
      }

      public DWeb.EmbedObject setDescription(String description) {
         this.description = description;
         return this;
      }

      public DWeb.EmbedObject setUrl(String url) {
         this.url = url;
         return this;
      }

      public DWeb.EmbedObject setColor(Color color) {
         this.color = color;
         return this;
      }

      public DWeb.EmbedObject setFooter(String text, String icon) {
         this.footer = new DWeb.EmbedObject.Footer(text, icon);
         return this;
      }

      public DWeb.EmbedObject setThumbnail(String url) {
         this.thumbnail = new DWeb.EmbedObject.Thumbnail(url);
         return this;
      }

      public DWeb.EmbedObject setImage(String url) {
         this.image = new DWeb.EmbedObject.Image(url);
         return this;
      }

      public DWeb.EmbedObject setAuthor(String name, String url, String icon) {
         this.author = new DWeb.EmbedObject.Author(name, url, icon);
         return this;
      }

      public DWeb.EmbedObject addField(String name, String value, boolean inline) {
         this.fields.add(new DWeb.EmbedObject.Field(name, value, inline));
         return this;
      }

      public class Field {
         private String name;
         private String value;
         private boolean inline;

         public Field(String name, String value, boolean inline) {
            this.name = name;
            this.value = value;
            this.inline = inline;
         }

         public String getName() {
            return this.name;
         }

         public String getValue() {
            return this.value;
         }

         public boolean isInline() {
            return this.inline;
         }
      }

      public class Author {
         private String name;
         private String url;
         private String iconUrl;

         public Author(String name, String url, String iconUrl) {
            this.name = name;
            this.url = url;
            this.iconUrl = iconUrl;
         }

         public String getName() {
            return this.name;
         }

         public String getUrl() {
            return this.url;
         }

         public String getIconUrl() {
            return this.iconUrl;
         }
      }

      public class Image {
         private String url;

         public Image(String url) {
            this.url = url;
         }

         public String getUrl() {
            return this.url;
         }
      }

      public class Thumbnail {
         private String url;

         public Thumbnail(String url) {
            this.url = url;
         }

         public String getUrl() {
            return this.url;
         }
      }

      public class Footer {
         private String text;
         private String iconUrl;

         public Footer(String text, String iconUrl) {
            this.text = text;
            this.iconUrl = iconUrl;
         }

         public String getText() {
            return this.text;
         }

         public String getIconUrl() {
            return this.iconUrl;
         }
      }
   }
}