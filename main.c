#include <gtk/gtk.h>
#include "src/gui/gui.h"
#include "src/backend/backend.h"

int main(int argc, char *argv[]) {
    // Inicializar backend
    carregarProdutos();
    carregarClientes();
    carregarPedidos();
    
    // Inicializar interface gráfica
    iniciar_gui(argc, argv);
    
    // Salvar dados ao encerrar
    salvarProdutos();
    salvarClientes();
    salvarPedidos();
    
    return 0;
}
