#include <gtk/gtk.h>
#include <locale.h>
#include "src/gui/gui.h"
#include "src/backend/backend.h"

static void on_app_shutdown(GtkWidget *window, gpointer user_data) {
    (void)window;  // Suprimir warning de parâmetro não usado
    (void)user_data;  // Suprimir warning de parâmetro não usado
    
    // Salvar dados antes de encerrar
    salvarProdutos();
    salvarClientes();
    salvarPedidos();
    gtk_main_quit();
}

int main(int argc, char *argv[]) {
    // Configurar localização para Angola (pt_AO)
    setlocale(LC_ALL, "pt_AO.UTF-8");
    
    // Inicializar backend
    carregarProdutos();
    carregarClientes();
    carregarPedidos();
    
    // Inicializar interface gráfica
    iniciar_gui(argc, argv);
    
    // Conectar sinal para salvar dados ao encerrar
    g_signal_connect(main_gui.window, "destroy", G_CALLBACK(on_app_shutdown), NULL);
    
    // Iniciar loop principal
    gtk_main();
    
    return 0;
}
