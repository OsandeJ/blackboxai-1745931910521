#ifndef PRODUTOS_VIEW_H
#define PRODUTOS_VIEW_H

#include <gtk/gtk.h>

// Colunas da lista de produtos
enum {
    COL_ID,
    COL_NOME,
    COL_DESCRICAO,
    COL_PRECO,
    COL_PRECO_IVA,
    COL_CATEGORIA,  // Nova coluna
    NUM_COLS
};

// Estrutura para a view de produtos
typedef struct {
    GtkWidget *container;
    GtkWidget *list_view;
    GtkWidget *grid_view;
    GtkWidget *search_entry;
    GtkWidget *filter_box;
    GtkWidget *price_slider;
    GtkWidget *price_label;
    GtkWidget *view_toggle;  // Botão para alternar entre lista/grid
    GtkWidget *export_button; // Botão para exportar dados
    GtkWidget *stats_button;  // Botão para visualizar estatísticas
    GtkListStore *store;
    GtkTreeModelFilter *filter;
    GtkTreeModelSort *sort;
    float preco_maximo;
    gboolean view_as_grid;  // Flag para controle de visualização
} ProdutosView;

// Funções principais
GtkWidget* criar_tela_produtos();
void atualizar_lista_produtos();
void atualizar_slider_preco();
void alternar_visualizacao();
void exportar_produtos();
void mostrar_estatisticas();

// Funções auxiliares
GtkWidget* criar_card_produto(const char *nome, const char *descricao, 
                            float preco, float preco_iva, const char *categoria);
void configurar_colunas_lista(GtkTreeView *treeview);
gboolean filtrar_produtos(GtkTreeModel *model, GtkTreeIter *iter, gpointer data);
float calcular_preco_com_iva(float preco);

// Callbacks
void on_search_changed(GtkSearchEntry *entry, gpointer user_data);
void on_price_filter_changed(GtkRange *range, gpointer user_data);
void on_view_toggle(GtkToggleButton *button, gpointer user_data);
void on_adicionar_produto(GtkButton *button, gpointer user_data);
void on_editar_produto(GtkButton *button, gpointer user_data);
void on_excluir_produto(GtkButton *button, gpointer user_data);
void on_exportar_clicked(GtkButton *button, gpointer user_data);
void on_stats_clicked(GtkButton *button, gpointer user_data);

#endif // PRODUTOS_VIEW_H
