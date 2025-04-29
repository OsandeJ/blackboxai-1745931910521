#include "custom_header.h"
#include <gtk/gtk.h>

// Callback para alternar entre temas claro/escuro
static void on_theme_switch_toggled(GtkSwitch *widget, gpointer user_data) {
    GtkSettings *settings = gtk_settings_get_default();
    gboolean is_dark = gtk_switch_get_active(widget);
    
    g_object_set(settings,
                 "gtk-application-prefer-dark-theme",
                 is_dark,
                 NULL);
}

CustomHeader* custom_header_new(const char *title, const char *subtitle) {
    CustomHeader *header = g_malloc(sizeof(CustomHeader));
    
    // Container principal
    header->container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_margin_start(header->container, 16);
    gtk_widget_set_margin_end(header->container, 16);
    gtk_widget_set_margin_top(header->container, 16);
    gtk_widget_set_margin_bottom(header->container, 16);
    
    // Container para título e ações
    GtkWidget *header_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    
    // Área de título
    GtkWidget *title_area = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    
    // Título
    header->title = gtk_label_new(NULL);
    char *markup = g_markup_printf_escaped("<span size='x-large' weight='bold'>%s</span>", title);
    gtk_label_set_markup(GTK_LABEL(header->title), markup);
    g_free(markup);
    gtk_widget_set_halign(header->title, GTK_ALIGN_START);
    
    // Subtítulo
    header->subtitle = gtk_label_new(subtitle);
    gtk_widget_set_halign(header->subtitle, GTK_ALIGN_START);
    gtk_widget_set_opacity(header->subtitle, 0.7);
    
    // Adicionar título e subtítulo à área de título
    gtk_box_pack_start(GTK_BOX(title_area), header->title, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(title_area), header->subtitle, FALSE, FALSE, 0);
    
    // Container para ações (botões e switch de tema)
    header->actions_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_widget_set_halign(header->actions_box, GTK_ALIGN_END);
    gtk_widget_set_hexpand(header->actions_box, TRUE);
    
    // Switch para tema claro/escuro
    GtkWidget *theme_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    GtkWidget *theme_icon = gtk_image_new_from_icon_name("weather-clear-night-symbolic", GTK_ICON_SIZE_SMALL_TOOLBAR);
    header->theme_switch = gtk_switch_new();
    
    gtk_box_pack_start(GTK_BOX(theme_box), theme_icon, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(theme_box), header->theme_switch, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(header->actions_box), theme_box, FALSE, FALSE, 0);
    
    g_signal_connect(header->theme_switch, "notify::active",
                    G_CALLBACK(on_theme_switch_toggled), NULL);
    
    // Montar layout final
    gtk_box_pack_start(GTK_BOX(header_box), title_area, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(header_box), header->actions_box, TRUE, TRUE, 0);
    
    // Adicionar separador
    GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_set_margin_top(separator, 8);
    
    // Adicionar elementos ao container principal
    gtk_box_pack_start(GTK_BOX(header->container), header_box, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(header->container), separator, FALSE, FALSE, 0);
    
    // Aplicar classes CSS
    gtk_style_context_add_class(gtk_widget_get_style_context(header->container), "header-bar");
    gtk_style_context_add_class(gtk_widget_get_style_context(header->title), "title");
    gtk_style_context_add_class(gtk_widget_get_style_context(header->subtitle), "subtitle");
    
    return header;
}

void custom_header_add_action_button(CustomHeader *header, const char *icon_name,
                                   const char *tooltip_text, GCallback callback,
                                   gpointer user_data) {
    GtkWidget *button = gtk_button_new_from_icon_name(icon_name, GTK_ICON_SIZE_SMALL_TOOLBAR);
    gtk_widget_set_tooltip_text(button, tooltip_text);
    gtk_style_context_add_class(gtk_widget_get_style_context(button), "circular");
    
    g_signal_connect(button, "clicked", callback, user_data);
    
    gtk_box_pack_start(GTK_BOX(header->actions_box), button, FALSE, FALSE, 0);
}

void custom_header_set_title(CustomHeader *header, const char *title) {
    char *markup = g_markup_printf_escaped("<span size='x-large' weight='bold'>%s</span>", title);
    gtk_label_set_markup(GTK_LABEL(header->title), markup);
    g_free(markup);
}

void custom_header_set_subtitle(CustomHeader *header, const char *subtitle) {
    gtk_label_set_text(GTK_LABEL(header->subtitle), subtitle);
}

GtkWidget* custom_header_get_widget(CustomHeader *header) {
    return header->container;
}
