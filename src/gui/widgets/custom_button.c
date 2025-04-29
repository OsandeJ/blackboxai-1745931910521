#include "custom_button.h"

// Estrutura privada para dados do botão
typedef struct {
    GtkWidget *spinner;
    GtkWidget *icon;
    GtkWidget *label;
    GtkWidget *box;
    ButtonStyle current_style;
} ButtonData;

// Função para obter o nome da classe CSS baseado no estilo
const char* get_style_class_name(ButtonStyle style) {
    switch (style) {
        case BUTTON_STYLE_PRIMARY:
            return "primary";
        case BUTTON_STYLE_SECONDARY:
            return "secondary";
        case BUTTON_STYLE_DANGER:
            return "danger";
        case BUTTON_STYLE_SUCCESS:
            return "success";
        case BUTTON_STYLE_WARNING:
            return "warning";
        default:
            return "primary";
    }
}

// Função privada para criar o conteúdo do botão
static GtkWidget* create_button_content(const char *label_text, const char *icon_name) {
    ButtonData *data = g_malloc(sizeof(ButtonData));
    
    // Criar container
    data->box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_widget_set_margin_start(data->box, 8);
    gtk_widget_set_margin_end(data->box, 8);
    
    // Criar spinner (inicialmente oculto)
    data->spinner = gtk_spinner_new();
    gtk_widget_set_no_show_all(data->spinner, TRUE);
    gtk_box_pack_start(GTK_BOX(data->box), data->spinner, FALSE, FALSE, 0);
    
    // Criar ícone se especificado
    if (icon_name != NULL) {
        data->icon = gtk_image_new_from_icon_name(icon_name, GTK_ICON_SIZE_BUTTON);
        gtk_box_pack_start(GTK_BOX(data->box), data->icon, FALSE, FALSE, 0);
    } else {
        data->icon = NULL;
    }
    
    // Criar label
    data->label = gtk_label_new(label_text);
    gtk_box_pack_start(GTK_BOX(data->box), data->label, TRUE, TRUE, 0);
    
    // Armazenar dados do botão
    g_object_set_data_full(G_OBJECT(data->box), "button-data", data, g_free);
    
    return data->box;
}

GtkWidget* custom_button_new(const char *label, ButtonStyle style) {
    return custom_button_new_with_icon(label, NULL, style);
}

GtkWidget* custom_button_new_with_icon(const char *label, const char *icon_name, ButtonStyle style) {
    // Criar botão
    GtkWidget *button = gtk_button_new();
    
    // Criar e adicionar conteúdo
    GtkWidget *content = create_button_content(label, icon_name);
    gtk_container_add(GTK_CONTAINER(button), content);
    
    // Aplicar estilo
    custom_button_set_style(button, style);
    
    // Armazenar estilo atual
    ButtonData *data = g_object_get_data(G_OBJECT(content), "button-data");
    data->current_style = style;
    
    return button;
}

void custom_button_set_loading(GtkWidget *button, gboolean is_loading) {
    GtkWidget *content = gtk_bin_get_child(GTK_BIN(button));
    ButtonData *data = g_object_get_data(G_OBJECT(content), "button-data");
    
    if (is_loading) {
        // Ocultar ícone se existir
        if (data->icon != NULL) {
            gtk_widget_hide(data->icon);
        }
        
        // Mostrar e ativar spinner
        gtk_widget_show(data->spinner);
        gtk_spinner_start(GTK_SPINNER(data->spinner));
        
        // Desabilitar botão
        gtk_widget_set_sensitive(button, FALSE);
    } else {
        // Mostrar ícone se existir
        if (data->icon != NULL) {
            gtk_widget_show(data->icon);
        }
        
        // Ocultar e parar spinner
        gtk_widget_hide(data->spinner);
        gtk_spinner_stop(GTK_SPINNER(data->spinner));
        
        // Habilitar botão
        gtk_widget_set_sensitive(button, TRUE);
    }
}

void custom_button_set_style(GtkWidget *button, ButtonStyle style) {
    GtkStyleContext *context = gtk_widget_get_style_context(button);
    
    // Remover classes de estilo existentes
    gtk_style_context_remove_class(context, "primary");
    gtk_style_context_remove_class(context, "secondary");
    gtk_style_context_remove_class(context, "danger");
    gtk_style_context_remove_class(context, "success");
    gtk_style_context_remove_class(context, "warning");
    
    // Adicionar nova classe de estilo
    gtk_style_context_add_class(context, get_style_class_name(style));
    
    // Atualizar estilo atual
    GtkWidget *content = gtk_bin_get_child(GTK_BIN(button));
    ButtonData *data = g_object_get_data(G_OBJECT(content), "button-data");
    data->current_style = style;
}
