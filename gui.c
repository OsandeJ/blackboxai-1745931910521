#include <gtk/gtk.h>
#include "gui.h"
#include "backend.h"

// Callback para fechar a aplicação
static void on_app_quit(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

// Callback para menu Produtos
static void on_menu_produtos(GtkWidget *widget, gpointer data) {
    g_print("Menu Produtos selecionado\n");
    // Aqui será implementada a navegação para a tela de produtos
}

// Callback para menu Clientes
static void on_menu_clientes(GtkWidget *widget, gpointer data) {
    g_print("Menu Clientes selecionado\n");
    // Aqui será implementada a navegação para a tela de clientes
}

// Callback para menu Pedidos
static void on_menu_pedidos(GtkWidget *widget, gpointer data) {
    g_print("Menu Pedidos selecionado\n");
    // Aqui será implementada a navegação para a tela de pedidos
}

void iniciar_gui(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Sistema de Gestão - GUI");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    g_signal_connect(window, "destroy", G_CALLBACK(on_app_quit), NULL);

    // Criar menu principal
    GtkWidget *menubar = gtk_menu_bar_new();

    // Menu Produtos
    GtkWidget *menu_produtos = gtk_menu_item_new_with_label("Produtos");
    g_signal_connect(menu_produtos, "activate", G_CALLBACK(on_menu_produtos), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), menu_produtos);

    // Menu Clientes
    GtkWidget *menu_clientes = gtk_menu_item_new_with_label("Clientes");
    g_signal_connect(menu_clientes, "activate", G_CALLBACK(on_menu_clientes), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), menu_clientes);

    // Menu Pedidos
    GtkWidget *menu_pedidos = gtk_menu_item_new_with_label("Pedidos");
    g_signal_connect(menu_pedidos, "activate", G_CALLBACK(on_menu_pedidos), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), menu_pedidos);

    // Menu Sair
    GtkWidget *menu_sair = gtk_menu_item_new_with_label("Sair");
    g_signal_connect(menu_sair, "activate", G_CALLBACK(on_app_quit), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), menu_sair);

    // Layout vertical principal
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);

    // Container principal para conteúdo dinâmico
    GtkWidget *container_principal = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), container_principal, TRUE, TRUE, 5);

    gtk_container_add(GTK_CONTAINER(window), vbox);

    gtk_widget_show_all(window);

    gtk_main();
}
