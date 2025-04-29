#ifndef CUSTOM_DIALOG_H
#define CUSTOM_DIALOG_H

#include <gtk/gtk.h>

typedef struct {
    GtkWidget *dialog;
    GtkWidget *content_area;
    GtkWidget *form_grid;
    GtkWidget *action_area;
    GList *entries;  // Lista de campos de entrada
    int row_count;   // Contador de linhas do grid
} CustomDialog;

// Funções públicas
CustomDialog* custom_dialog_new(GtkWindow *parent, const char *title);
void custom_dialog_add_field(CustomDialog *dialog, const char *label_text, 
                           const char *placeholder_text, const char *initial_value);
void custom_dialog_add_numeric_field(CustomDialog *dialog, const char *label_text, 
                                   double min_value, double max_value, 
                                   double step, double initial_value);
void custom_dialog_add_combobox(CustomDialog *dialog, const char *label_text, 
                              const char **options, int num_options, 
                              int initial_selection);
void custom_dialog_add_action_button(CustomDialog *dialog, const char *label, 
                                   const char *icon_name, GCallback callback, 
                                   gpointer user_data);
GtkWidget* custom_dialog_get_entry(CustomDialog *dialog, int index);
void custom_dialog_set_response_callback(CustomDialog *dialog, GCallback callback, 
                                       gpointer user_data);
void custom_dialog_destroy(CustomDialog *dialog);

#endif // CUSTOM_DIALOG_H
