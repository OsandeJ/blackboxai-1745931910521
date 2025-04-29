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
    
    gtk_box_pack_start(GTK_BOX(main_gui.main_box), infobar, FALSE, FALSE, 0);
    gtk_widget_show_all(infobar);
    
    // Auto-destruir após 3 segundos
    g_timeout_add_seconds(3, (GSourceFunc)gtk_widget_destroy, infobar);
}

void mostrar_erro(const char *mensagem) {
    mostrar_notificacao(mensagem, "erro");
}

void mostrar_sucesso(const char *mensagem) {
    mostrar_notificacao(mensagem, "sucesso");
}

// Função para mostrar diálogo de confirmação
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

// Função para mostrar diálogo de erro
void mostrar_dialogo_erro(const char *titulo, const char *mensagem) {
    CustomDialog *dialog = custom_dialog_new(GTK_WINDOW(main_gui.window), titulo);
    
    GtkWidget *label = gtk_label_new(mensagem);
    gtk_box_pack_start(GTK_BOX(dialog->content_area), label, TRUE, TRUE, 8);
    
    custom_dialog_add_action_button(dialog, "OK", "window-close-symbolic",
                                  G_CALLBACK(custom_dialog_destroy), dialog);
    
    gtk_widget_show_all(dialog->dialog);
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
    
    // Cabeçalho
    main_gui.header = custom_header_new("Sistema de Gestão", 
                                      "Gerencie seus produtos, clientes e pedidos");
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
