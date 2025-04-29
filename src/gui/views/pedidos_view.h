#ifndef PEDIDOS_VIEW_H
#define PEDIDOS_VIEW_H

#include <gtk/gtk.h>
#include "../widgets/custom_header.h"
#include "../widgets/custom_button.h"
#include "../widgets/custom_dialog.h"
#include "../../backend/backend.h"

typedef struct {
    GtkWidget *container;
    GtkWidget *pedidos_list;
    GtkWidget *detalhes_panel;
    GtkWidget *search_entry;
    GtkWidget *status_combo;
    GtkWidget *periodo_combo;
    GtkListStore *store;
    GtkTreeModelFilter *filter;
    GtkTreeModelSort *sort;
    
    struct {
        GtkWidget *dialog;
        GtkWidget *cliente_combo;
        GtkWidget *produtos_tree;
        GtkWidget *quantidade_spin;
        GtkWidget *total_label;
        GtkWidget *total_iva_label;
        GtkWidget *valor_pago_entry;
        GtkWidget *troco_label;
        GtkWidget *metodo_pagamento_combo;
        GtkListStore *produtos_store;
    } novo_pedido;
} PedidosView;

typedef enum {
    PERIODO_HOJE,
    PERIODO_SEMANA,
    PERIODO_MES,
    PERIODO_TODOS
} PeriodoFiltro;

enum {
    COL_PEDIDO_ID,
    COL_PEDIDO_CLIENTE,
    COL_PEDIDO_TOTAL,
    COL_PEDIDO_DATA,
    COL_PEDIDO_STATUS,
    NUM_COLS_PEDIDO
};

enum {
    COL_ITEM_ID,
    COL_ITEM_NOME,
    COL_ITEM_QUANTIDADE,
    COL_ITEM_PRECO_UNIT,
    COL_ITEM_SUBTOTAL,
    COL_ITEM_IVA,
    COL_ITEM_TOTAL,
    NUM_COLS_ITEM
};

// Funções principais
GtkWidget* criar_tela_pedidos(void);
void atualizar_lista_pedidos(void);
void atualizar_detalhes_pedido(int id_pedido);
void mostrar_dialogo_novo_pedido(void);
void atualizar_totais_pedido(void);
void gerar_comprovante_pedido(Pedido *pedido);
gboolean filtrar_pedidos(GtkTreeModel *model, GtkTreeIter *iter, gpointer data);

// Callbacks
void on_novo_pedido(GtkButton *button, gpointer user_data);
void on_visualizar_pedido(GtkButton *button, gpointer user_data);
void on_imprimir_pedido(GtkButton *button, gpointer user_data);
void on_cancelar_pedido(GtkButton *button, gpointer user_data);
void on_adicionar_item(GtkButton *button, gpointer user_data);
void on_remover_item(GtkButton *button, gpointer user_data);
void on_quantidade_changed(GtkSpinButton *spin_button, gpointer user_data);
void on_valor_pago_changed(GtkEditable *editable, gpointer user_data);
void on_metodo_pagamento_changed(GtkComboBox *combo, gpointer user_data);
void on_finalizar_pedido(GtkButton *button, gpointer user_data);

#endif // PEDIDOS_VIEW_H
