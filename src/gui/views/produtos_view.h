#ifndef PRODUTOS_VIEW_H
#define PRODUTOS_VIEW_H

#include <gtk/gtk.h>
#include "../widgets/custom_header.h"
#include "../widgets/custom_button.h"
#include "../widgets/custom_dialog.h"

// Enumeração para as colunas da TreeView
enum {
    COL_ID,
    COL_NOME,
    COL_DESCRICAO,
    COL_PRECO,
    COL_PRECO_IVA,
    NUM_COLS
};

// Estrutura para armazenar widgets da interface de produtos
typedef struct {
    GtkWidget *container;
    GtkWidget *grid_view;
    GtkWidget *list_view;
    GtkWidget *view_switcher;
    GtkWidget *search_entry;
    GtkWidget *filter_box;
    GtkWidget *price_slider;
    GtkWidget *price_label;
    GtkListStore *store;
    GtkTreeModelFilter *filter;
    GtkTreeModelSort *sort;
} ProdutosView;

// Funções principais
GtkWidget* criar_tela_produtos(void);
void atualizar_lista_produtos(void);
void atualizar_grid_produtos(void);

// Callbacks
void on_view_switch(GtkToggleButton *button, gpointer user_data);
void on_produto_activated(GtkListBox *box, GtkListBoxRow *row, gpointer user_data);
void on_adicionar_produto(GtkButton *button, gpointer user_data);
void on_editar_produto(GtkButton *button, gpointer user_data);
void on_excluir_produto(GtkButton *button, gpointer user_data);
void on_search_changed(GtkSearchEntry *entry, gpointer user_data);
void on_price_filter_changed(GtkRange *range, gpointer user_data);

// Funções auxiliares
GtkWidget* criar_card_produto(const char *nome, const char *descricao, 
                            float preco, float preco_iva);
void configurar_colunas_lista(GtkTreeView *treeview);
gboolean filtrar_produtos(GtkTreeModel *model, GtkTreeIter *iter, gpointer data);

#endif // PRODUTOS_VIEW_H
