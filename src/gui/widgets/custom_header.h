#ifndef CUSTOM_HEADER_H
#define CUSTOM_HEADER_H

#include <gtk/gtk.h>

// Estrutura para o widget de cabeçalho personalizado
typedef struct {
    GtkWidget *container;
    GtkWidget *title;
    GtkWidget *subtitle;
    GtkWidget *actions_box;
    GtkWidget *theme_switch;
} CustomHeader;

// Funções públicas
CustomHeader* custom_header_new(const char *title, const char *subtitle);
void custom_header_add_action_button(CustomHeader *header, const char *icon_name, 
                                   const char *tooltip_text, GCallback callback, 
                                   gpointer user_data);
void custom_header_set_title(CustomHeader *header, const char *title);
void custom_header_set_subtitle(CustomHeader *header, const char *subtitle);
GtkWidget* custom_header_get_widget(CustomHeader *header);

#endif // CUSTOM_HEADER_H
