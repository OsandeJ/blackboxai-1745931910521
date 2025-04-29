#ifndef PRODUTOS_GUI_H
#define PRODUTOS_GUI_H

#include <gtk/gtk.h>

// Estrutura para armazenar widgets da interface de produtos
typedef struct {
    GtkWidget *treeview;
    GtkWidget *slider_preco;
    GtkWidget *label_preco_min;
    GtkWidget *label_preco_max;
    GtkListStore *liststore;
    GtkTreeModelFilter *filter;
} ProdutosGui;

// Funções principais
GtkWidget* criar_tela_produtos(void);
void atualizar_lista_produtos(void);

// Callbacks
void on_adicionar_produto_clicked(GtkButton *button, gpointer user_data);
void on_editar_produto_clicked(GtkButton *button, gpointer user_data);
void on_excluir_produto_clicked(GtkButton *button, gpointer user_data);
void on_slider_preco_changed(GtkRange *range, gpointer user_data);

#endif // PRODUTOS_GUI_H
