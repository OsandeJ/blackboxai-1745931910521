#ifndef CLIENTES_GUI_H
#define CLIENTES_GUI_H

#include <gtk/gtk.h>

// Estrutura para armazenar widgets da interface de clientes
typedef struct {
    GtkWidget *treeview;
    GtkWidget *entry_pesquisa;
    GtkListStore *liststore;
    GtkTreeModelFilter *filter;
} ClientesGui;

// Funções principais
GtkWidget* criar_tela_clientes(void);
void atualizar_lista_clientes(void);

// Callbacks
void on_adicionar_cliente_clicked(GtkButton *button, gpointer user_data);
void on_editar_cliente_clicked(GtkButton *button, gpointer user_data);
void on_excluir_cliente_clicked(GtkButton *button, gpointer user_data);
void on_pesquisar_cliente_changed(GtkEditable *editable, gpointer user_data);

#endif // CLIENTES_GUI_H
