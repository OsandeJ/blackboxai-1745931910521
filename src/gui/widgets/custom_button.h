#ifndef CUSTOM_BUTTON_H
#define CUSTOM_BUTTON_H

#include <gtk/gtk.h>

typedef enum {
    BUTTON_STYLE_PRIMARY,
    BUTTON_STYLE_SECONDARY,
    BUTTON_STYLE_DANGER,
    BUTTON_STYLE_SUCCESS,
    BUTTON_STYLE_WARNING
} ButtonStyle;

// Funções públicas
GtkWidget* custom_button_new(const char *label, ButtonStyle style);
GtkWidget* custom_button_new_with_icon(const char *label, const char *icon_name, ButtonStyle style);
void custom_button_set_loading(GtkWidget *button, gboolean is_loading);
void custom_button_set_style(GtkWidget *button, ButtonStyle style);

// Função para obter o nome da classe CSS baseado no estilo
const char* get_style_class_name(ButtonStyle style);

#endif // CUSTOM_BUTTON_H
