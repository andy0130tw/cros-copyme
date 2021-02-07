#include <glib.h>
#include <gmodule.h>
#include <gtk/gtk.h>
#include <gio/gio.h>
#include <gio/gunixinputstream.h>

struct refs {
  GtkApplication* application;
  GtkWidget* textbox;
};

static gboolean do_exit(gpointer _refs) {
  struct refs* refs = _refs;
  g_application_quit(G_APPLICATION(refs->application));
  g_free(refs);
  return FALSE;
}

static int redraw_count = 0;
static GBytes* user_input;

static gboolean do_copy(GtkApplicationWindow* window, gpointer cr, gpointer _refs) {
  redraw_count++;
  if (redraw_count != 2) goto done;

  struct refs* refs = _refs;
  GtkWidget* textbox = refs->textbox;

  GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textbox));
  GtkClipboard *clipboard = gtk_widget_get_clipboard(textbox,
                  GDK_SELECTION_CLIPBOARD);

  // g_signal_connect(buffer, "paste-done", G_CALLBACK(on_paste_done), refs);

  gtk_text_buffer_copy_clipboard(buffer, clipboard);
  g_message("Copied. Now exiting.");

  // XXX: setting a timeout is not stable
  g_timeout_add(100, do_exit, refs);
done:
  // keep the actual drawing routine from running, thus the window will not be shown
  return TRUE;
}

static void activate(GtkApplication* app, gpointer user_data) {
  GtkWidget *window;
  window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "Crostini copy-to-clipboard utility");
  gtk_window_set_icon_name(GTK_WINDOW(window), "NULL");
  gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);
  // XXX: no use, why?
  gtk_window_set_type_hint(GTK_WINDOW(window), GDK_WINDOW_TYPE_HINT_TOOLTIP);
  gtk_window_set_skip_taskbar_hint(GTK_WINDOW(window), TRUE);

  GtkTextBuffer* buffer = gtk_text_buffer_new(NULL);

  gsize len;
  gchar* content = g_bytes_unref_to_data(user_input, &len);
  content[len] = '\0';
  g_message("Ready to copy %zu bytes", len);

  gtk_text_buffer_set_text(buffer, content, len);
  GtkTextIter iter_start, iter_end;
  gtk_text_buffer_get_start_iter(buffer, &iter_start);
  gtk_text_buffer_get_end_iter(buffer, &iter_end);
  gtk_text_buffer_select_range(buffer, &iter_start, &iter_end);
  GtkWidget* textbox = gtk_text_view_new_with_buffer(buffer);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textbox), GTK_WRAP_CHAR);
  gtk_container_add(GTK_CONTAINER(window), textbox);

  struct refs* context = g_new(struct refs, 1);

  *context = (struct refs) {
    .application = app,
    .textbox = textbox,
  };

  g_signal_connect(window, "draw", G_CALLBACK(do_copy), context);
  gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
  GtkApplication *app;

  app = gtk_application_new("org.gtk.qbane.cros-copyme", G_APPLICATION_FLAGS_NONE);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

  GInputStream* stdin_stream = g_unix_input_stream_new(fileno(stdin), TRUE);
  if (stdin_stream == NULL) {
    g_critical("Cannot optain data from stdin.");
    return 1;
  }

  GByteArray* input_buffer = g_byte_array_new();
  while (1) {
    GBytes* received = g_input_stream_read_bytes(stdin_stream, 4096, NULL, NULL);
    if (received == NULL) {
      g_critical("Error occured during reading from stdin.");
      return 1;
    }

    gsize len;
    guint8* data = g_bytes_unref_to_data(received, &len);
    if (len == 0) break;
    g_byte_array_append(input_buffer, data, len);
    g_free(data);
  }

  user_input = g_byte_array_free_to_bytes(input_buffer);
  g_object_unref(stdin_stream);

  int status;
  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  return status;
}
