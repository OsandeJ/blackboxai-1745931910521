#ifndef CLIENTES_VIEW_H
#define CLIENTES_VIEW_H

#include <gtk/gtk.h>
#include "../widgets/custom_header.h"
#include "../widgets/custom_button.h"
#include "../widgets/custom_dialog.h"
#include "../../backend/backend.h"

// Estrutura para armazenar widgets da interface de clientes
typedef struct {
    GtkWidget *container;
    GtkWidget *cards_container;
    GtkWidget *search_entry;
    GtkWidget *sort_combo;
    GtkListStore *store;
    GtkTreeModelFilter *filter;
    GtkTreeModelSort *sort;
} ClientesView;

// Enumerações
typedef enum {
    SORT_NOME_ASC,
    SORT_NOME_DESC,
    SORT_ID_ASC,
    SORT_ID_DESC
} SortType;

// Enumeração para as colunas
enum {
    COL_CLIENTE_ID,
    COL_CLIENTE_NOME,
    COL_CLIENTE_TELEFONE,
    NUM_COLS_CLIENTE
};

// Funções principais
GtkWidget* criar_tela_clientes(void);
void atualizar_lista_clientes(void);
void atualizar_cards_clientes(void);

// Callbacks
void on_adicionar_cliente(GtkButton *button, gpointer user_data);
void on_editar_cliente(GtkButton *button, gpointer user_data);
void on_excluir_cliente(GtkButton *button, gpointer user_data);
void on_cliente_activated(GtkFlowBox *flow_box, GtkFlowBoxChild *child, gpointer user_data);
void on_search_cliente_changed(GtkSearchEntry *entry, gpointer user_data);
void on_sort_changed(GtkComboBox *combo_box, gpointer user_data);

// Funções auxiliares
GtkWidget* criar_card_cliente(const char *nome, const char *telefone, int id);
gboolean filtrar_clientes(GtkTreeModel *model, GtkTreeIter *iter, gpointer data);
void mostrar_dialogo_cliente(const char *titulo, Cliente *cliente, gboolean editando);

#endif // CLIENTES_VIEW_H
