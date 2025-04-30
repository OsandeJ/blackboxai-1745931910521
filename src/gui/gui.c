#include "gui.h"
#include "../backend/backend.h"
#include "views/produtos_view.h"
#include "views/clientes_view.h"
#include "views/pedidos_view.h"

// Variável global da interface
MainGui main_gui;

// Callbacks para navegação
static void on_stack_switch(GtkStack *stack, GParamSpec *pspec, gpointer user_data) {
    const gchar *visible_child_name = gtk_stack_get_visible_child_name(stack);
    
    if (g_strcmp0(visible_child_name, "produtos") == 0) {
        custom_header_set_title(main_gui.header, "Gestão de Produtos");
        custom_header_set_subtitle(main_gui.header, "Gerencie seu catálogo de produtos");
    }
    else if (g_strcmp0(visible_child_name, "clientes") == 0) {
        custom_header_set_title(main_gui.header, "Gestão de Clientes");
        custom_header_set_subtitle(main_gui.header, "Gerencie seus clientes");
    }
    else if (g_strcmp0(visible_child_name, "pedidos") == 0) {
        custom_header_set_title(main_gui.header, "Gestão de Pedidos");
        custom_header_set_subtitle(main_gui.header, "Gerencie seus pedidos");
    }
}

// Callback para alternar tema
static void on_theme_switch(GtkSwitch *widget, gboolean state, gpointer user_data) {
    GtkSettings *settings = gtk_settings_get_default();
    g_object_set(settings, "gtk-application-prefer-dark-theme", state, NULL);
    
    // Aplicar cor do texto dos botões baseado no tema
    GtkCssProvider *provider = gtk_css_provider_new();
    const gchar *css = state ? 
        "button { color: white; }" :
        "button { color: black; }";
    gtk_css_provider_load_from_data(provider, css, -1, NULL);
    
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
    
    g_object_unref(provider);
}

// Função para carregar o CSS
void carregar_css() {
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "src/gui/styles/theme.css", NULL);
    
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
    
    g_object_unref(provider);
}


// Função para mostrar notificações
void mostrar_notificacao(const char *mensagem, const char *tipo) {
    GtkWidget *infobar = gtk_info_bar_new();
    GtkWidget *content = gtk_info_bar_get_content_area(GTK_INFO_BAR(infobar));
    GtkWidget *label = gtk_label_new(mensagem);
    
    gtk_container_add(GTK_CONTAINER(content), label);
    gtk_info_bar_set_show_close_button(GTK_INFO_BAR(infobar), TRUE);
    
    if (g_strcmp0(tipo, "erro") == 0) {
        gtk_info_bar_set_message_type(GTK_INFO_BAR(infobar), GTK_MESSAGE_ERROR);
        gtk_style_context_add_class(gtk_widget_get_style_context(infobar), "error");
    }
    else if (g_strcmp0(tipo, "sucesso") == 0) {
        gtk_info_bar_set_message_type(GTK_INFO_BAR(infobar), GTK_MESSAGE_INFO);
        gtk_style_context_add_class(gtk_widget_get_style_context(infobar), "success");
    }
    else if (g_strcmp0(tipo, "aviso") == 0) {
        gtk_info_bar_set_message_type(GTK_INFO_BAR(infobar), GTK_MESSAGE_WARNING);
        gtk_style_context_add_class(gtk_widget_get_style_context(infobar), "warning");
    }
    else {
        gtk_info_bar_set_message_type(GTK_INFO_BAR(infobar), GTK_MESSAGE_OTHER);
        gtk_style_context_add_class(gtk_widget_get_style_context(infobar), "info");
    }
    
    gtk_box_pack_start(GTK_BOX(main_gui.notification_area), infobar, FALSE, FALSE, 0);
    gtk_widget_show_all(infobar);
    
    // Auto-destruir após 5 segundos
    g_timeout_add_seconds(5, (GSourceFunc)gtk_widget_destroy, infobar);
}

void mostrar_erro(const char *mensagem) {
    mostrar_notificacao(mensagem, "erro");
}

void mostrar_sucesso(const char *mensagem) {
    mostrar_notificacao(mensagem, "sucesso");
}

void mostrar_aviso(const char *mensagem) {
    mostrar_notificacao(mensagem, "aviso");
}

void mostrar_info(const char *mensagem) {
    mostrar_notificacao(mensagem, "info");
}

// Funções de diálogo
void mostrar_dialogo_confirmacao(const char *titulo, const char *mensagem,
                               GCallback callback_sim, gpointer user_data) {
    CustomDialog *dialog = custom_dialog_new(GTK_WINDOW(main_gui.window), titulo);
    
    GtkWidget *label = gtk_label_new(mensagem);
    gtk_box_pack_start(GTK_BOX(dialog->content_area), label, TRUE, TRUE, 8);
    
    custom_dialog_add_action_button(dialog, "Cancelar", "window-close-symbolic",
                                  G_CALLBACK(custom_dialog_destroy), dialog);
    
    custom_dialog_add_action_button(dialog, "Confirmar", "object-select-symbolic",
                                  callback_sim, user_data);
    
    gtk_widget_show_all(dialog->dialog);
}

void mostrar_dialogo_erro(const char *titulo, const char *mensagem) {
    CustomDialog *dialog = custom_dialog_new(GTK_WINDOW(main_gui.window), titulo);
    
    GtkWidget *label = gtk_label_new(mensagem);
    gtk_box_pack_start(GTK_BOX(dialog->content_area), label, TRUE, TRUE, 8);
    
    custom_dialog_add_action_button(dialog, "OK", "window-close-symbolic",
                                  G_CALLBACK(custom_dialog_destroy), dialog);
    
    gtk_widget_show_all(dialog->dialog);
}

// Funções de validação
gboolean validar_numero(const char *texto) {
    if (texto == NULL || *texto == '\0') return FALSE;
    
    for (const char *p = texto; *p != '\0'; p++) {
        if (!g_ascii_isdigit(*p)) return FALSE;
    }
    return TRUE;
}

gboolean validar_texto_vazio(const char *texto) {
    return texto != NULL && *texto != '\0';
}

gboolean validar_preco(const char *texto) {
    if (texto == NULL || *texto == '\0') return FALSE;
    
    char *endptr;
    float valor = strtof(texto, &endptr);
    return *endptr == '\0' && valor >= 0;
}

gboolean validar_telefone(const char *texto) {
    if (texto == NULL || *texto == '\0') return FALSE;
    
    int len = strlen(texto);
    if (len != 9) return FALSE;
    
    return validar_numero(texto);
}

// Funções de formatação
char* formatar_moeda(float valor) {
    static char buffer[32];
    snprintf(buffer, sizeof(buffer), "%.2f Kz", valor);
    return buffer;
}

char* formatar_telefone(const char *telefone) {
    static char buffer[16];
    if (strlen(telefone) == 9) {
        snprintf(buffer, sizeof(buffer), "%c%c%c %c%c%c %c%c%c",
                telefone[0], telefone[1], telefone[2],
                telefone[3], telefone[4], telefone[5],
                telefone[6], telefone[7], telefone[8]);
    } else {
        strncpy(buffer, telefone, sizeof(buffer) - 1);
    }
    return buffer;
}

char* formatar_data(const char *data) {
    static char buffer[32];
    // TODO: Implementar formatação de data
    strncpy(buffer, data, sizeof(buffer) - 1);
    return buffer;
}

// Função principal para iniciar a interface
void iniciar_gui(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    
    // Carregar CSS
    carregar_css();
    
    // Criar janela principal
    main_gui.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_gui.window), "Sistema de Gestão");
    gtk_window_set_default_size(GTK_WINDOW(main_gui.window), 1200, 800);
    gtk_window_set_position(GTK_WINDOW(main_gui.window), GTK_WIN_POS_CENTER);
    
    // Container principal
    main_gui.main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(main_gui.window), main_gui.main_box);
    
    // Área de notificações
    main_gui.notification_area = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_box_pack_start(GTK_BOX(main_gui.main_box), 
                      main_gui.notification_area, FALSE, FALSE, 0);
    
    // Cabeçalho
    main_gui.header = custom_header_new("Sistema de Gestão", 
                                      "Gerencie seus produtos, clientes e pedidos");
    
    // Adicionar switch de tema ao cabeçalho
    main_gui.theme_switch = gtk_switch_new();
    g_signal_connect(main_gui.theme_switch, "state-set",
                    G_CALLBACK(on_theme_switch), NULL);
    
    // Criar container para o switch
    GtkWidget *switch_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    GtkWidget *theme_icon = gtk_image_new_from_icon_name("weather-clear-night-symbolic", 
                                                        GTK_ICON_SIZE_SMALL_TOOLBAR);
    gtk_box_pack_start(GTK_BOX(switch_box), theme_icon, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(switch_box), main_gui.theme_switch, FALSE, FALSE, 0);
    
    // Adicionar ao header através da função apropriada do custom_header
    custom_header_add_widget(main_gui.header, switch_box);
    
    gtk_box_pack_start(GTK_BOX(main_gui.main_box), 
                      custom_header_get_widget(main_gui.header), 
                      FALSE, FALSE, 0);
    
    // Container para sidebar e conteúdo
    GtkWidget *content_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(main_gui.main_box), content_box, TRUE, TRUE, 0);
    
    // Stack para conteúdo
    main_gui.stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(main_gui.stack), 
                                GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    
    // Sidebar para navegação
    main_gui.stack_sidebar = gtk_stack_sidebar_new();
    gtk_stack_sidebar_set_stack(GTK_STACK_SIDEBAR(main_gui.stack_sidebar), 
                              GTK_STACK(main_gui.stack));
    
    // Estilizar sidebar
    gtk_style_context_add_class(
        gtk_widget_get_style_context(main_gui.stack_sidebar),
        "sidebar"
    );
    
    // Adicionar páginas ao stack
    gtk_stack_add_titled(GTK_STACK(main_gui.stack), 
                        criar_tela_produtos(),
                        "produtos", "Produtos");
    
    gtk_stack_add_titled(GTK_STACK(main_gui.stack),
                        criar_tela_clientes(),
                        "clientes", "Clientes");
    
    gtk_stack_add_titled(GTK_STACK(main_gui.stack),
                        criar_tela_pedidos(),
                        "pedidos", "Pedidos");
    
    // Conectar sinal para atualizar cabeçalho
    g_signal_connect(main_gui.stack, "notify::visible-child",
                    G_CALLBACK(on_stack_switch), NULL);
    
    // Adicionar sidebar e stack ao container
    gtk_box_pack_start(GTK_BOX(content_box), main_gui.stack_sidebar, 
                      FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(content_box), main_gui.stack, 
                      TRUE, TRUE, 0);
    
    // Conectar sinal para fechar aplicação
    g_signal_connect(main_gui.window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    // Carregar dados iniciais
    carregarProdutos();
    carregarClientes();
    carregarPedidos();
    
    gtk_widget_show_all(main_gui.window);
    gtk_main();
}
