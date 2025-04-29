#include "custom_dialog.h"
#include "custom_button.h"

static GtkWidget* create_labeled_entry(const char *label_text, 
                                     const char *placeholder_text,
                                     const char *initial_value) {
    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), placeholder_text);
    if (initial_value != NULL) {
        gtk_entry_set_text(GTK_ENTRY(entry), initial_value);
    }
    gtk_style_context_add_class(gtk_widget_get_style_context(entry), "custom-entry");
    return entry;
}

static GtkWidget* create_labeled_spinbutton(const char *label_text,
                                          double min_value,
                                          double max_value,
                                          double step,
                                          double initial_value) {
    GtkAdjustment *adjustment = gtk_adjustment_new(initial_value,
                                                 min_value,
                                                 max_value,
                                                 step,
                                                 step * 10,
                                                 0);
    GtkWidget *spinbutton = gtk_spin_button_new(adjustment, 1, 2);
    gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spinbutton), TRUE);
    gtk_style_context_add_class(gtk_widget_get_style_context(spinbutton), 
                               "custom-spinbutton");
    return spinbutton;
}

CustomDialog* custom_dialog_new(GtkWindow *parent, const char *title) {
    CustomDialog *dialog_data = g_malloc(sizeof(CustomDialog));
    
    dialog_data->dialog = gtk_dialog_new_with_buttons(
        title,
        parent,
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        NULL
    );
    
    gtk_window_set_default_size(GTK_WINDOW(dialog_data->dialog), 400, -1);
    
    dialog_data->content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog_data->dialog));
    dialog_data->action_area = gtk_dialog_get_action_area(GTK_DIALOG(dialog_data->dialog));
    
    gtk_container_set_border_width(GTK_CONTAINER(dialog_data->dialog), 16);
    gtk_box_set_spacing(GTK_BOX(dialog_data->content_area), 16);
    
    dialog_data->form_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(dialog_data->form_grid), 12);
    gtk_grid_set_column_spacing(GTK_GRID(dialog_data->form_grid), 12);
    gtk_container_set_border_width(GTK_CONTAINER(dialog_data->form_grid), 8);
    
    gtk_box_pack_start(GTK_BOX(dialog_data->content_area),
                      dialog_data->form_grid,
                      TRUE, TRUE, 0);
    
    dialog_data->entries = NULL;
    dialog_data->row_count = 0;
    
    gtk_style_context_add_class(gtk_widget_get_style_context(dialog_data->dialog),
                               "custom-dialog");
    
    return dialog_data;
}

void custom_dialog_add_field(CustomDialog *dialog,
                           const char *label_text,
                           const char *placeholder_text,
                           const char *initial_value) {
    GtkWidget *label = gtk_label_new(label_text);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_style_context_add_class(gtk_widget_get_style_context(label),
                               "custom-label");
    
    GtkWidget *entry = create_labeled_entry(label_text,
                                          placeholder_text,
                                          initial_value);
    
    gtk_grid_attach(GTK_GRID(dialog->form_grid),
                   label,
                   0, dialog->row_count, 1, 1);
    gtk_grid_attach(GTK_GRID(dialog->form_grid),
                   entry,
                   1, dialog->row_count, 1, 1);
    
    dialog->entries = g_list_append(dialog->entries, entry);
    dialog->row_count++;
}

void custom_dialog_add_numeric_field(CustomDialog *dialog,
                                   const char *label_text,
                                   double min_value,
                                   double max_value,
                                   double step,
                                   double initial_value) {
    GtkWidget *label = gtk_label_new(label_text);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_style_context_add_class(gtk_widget_get_style_context(label),
                               "custom-label");
    
    GtkWidget *spinbutton = create_labeled_spinbutton(label_text,
                                                     min_value,
                                                     max_value,
                                                     step,
                                                     initial_value);
    
    gtk_grid_attach(GTK_GRID(dialog->form_grid),
                   label,
                   0, dialog->row_count, 1, 1);
    gtk_grid_attach(GTK_GRID(dialog->form_grid),
                   spinbutton,
                   1, dialog->row_count, 1, 1);
    
    dialog->entries = g_list_append(dialog->entries, spinbutton);
    dialog->row_count++;
}

void custom_dialog_add_combobox(CustomDialog *dialog,
                              const char *label_text,
                              const char **options,
                              int num_options,
                              int initial_selection) {
    GtkWidget *label = gtk_label_new(label_text);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_style_context_add_class(gtk_widget_get_style_context(label),
                               "custom-label");
    
    GtkWidget *combobox = gtk_combo_box_text_new();
    for (int i = 0; i < num_options; i++) {
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combobox), options[i]);
    }
    
    if (initial_selection >= 0 && initial_selection < num_options) {
        gtk_combo_box_set_active(GTK_COMBO_BOX(combobox), initial_selection);
    }
    
    gtk_style_context_add_class(gtk_widget_get_style_context(combobox),
                               "custom-combobox");
    
    gtk_grid_attach(GTK_GRID(dialog->form_grid),
                   label,
                   0, dialog->row_count, 1, 1);
    gtk_grid_attach(GTK_GRID(dialog->form_grid),
                   combobox,
                   1, dialog->row_count, 1, 1);
    
    dialog->entries = g_list_append(dialog->entries, combobox);
    dialog->row_count++;
}

void custom_dialog_add_action_button(CustomDialog *dialog,
                                   const char *label,
                                   const char *icon_name,
                                   GCallback callback,
                                   gpointer user_data) {
    GtkWidget *button;
    
    if (icon_name != NULL) {
        button = custom_button_new_with_icon(label, icon_name, BUTTON_STYLE_PRIMARY);
    } else {
        button = custom_button_new(label, BUTTON_STYLE_PRIMARY);
    }
    
    gtk_widget_set_margin_top(button, 8);
    
    if (callback != NULL) {
        g_signal_connect(button, "clicked", callback, user_data);
    }
    
    gtk_container_add(GTK_CONTAINER(dialog->action_area), button);
}

GtkWidget* custom_dialog_get_entry(CustomDialog *dialog, int index) {
    return GTK_WIDGET(g_list_nth_data(dialog->entries, index));
}

void custom_dialog_set_response_callback(CustomDialog *dialog,
                                       GCallback callback,
                                       gpointer user_data) {
    g_signal_connect(dialog->dialog, "response", callback, user_data);
}

void custom_dialog_destroy(CustomDialog *dialog) {
    g_list_free(dialog->entries);
    gtk_widget_destroy(dialog->dialog);
    g_free(dialog);
}
