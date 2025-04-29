#ifndef PEDIDOS_GUI_H
#define PEDIDOS_GUI_H

#include <gtk/gtk.h>
#include "backend.h"

// Estrutura para armazenar widgets da interface de pedidos
typedef struct {
    GtkWidget *treeview;
    GtkListStore *liststore;
    GtkWidget *combo_clientes;
    GtkWidget *tree_produtos_pedido;
    GtkListStore *liststore_produtos_pedido;
    GtkWidget *label_total;
    GtkWidget *label_total_iva;
    GtkWidget *entry_valor_pago;
    GtkWidget *label_troco;
    GtkWidget *combo_pagamento;
} PedidosGui;

// Estrutura para armazenar item do pedido temporário
typedef struct {
    int id_produto;
    char nome[50];
    int quantidade;
    float preco_unitario;
    float subtotal;
    float iva;
    float total_com_iva;
} ItemPedido;

// Funções principais
GtkWidget* criar_tela_pedidos(void);
void atualizar_lista_pedidos(void);
GtkWidget* criar_dialogo_novo_pedido(GtkWindow *parent);

// Callbacks
void on_novo_pedido_clicked(GtkButton *button, gpointer user_data);
void on_visualizar_pedido_clicked(GtkButton *button, gpointer user_data);
void on_adicionar_produto_pedido_clicked(GtkButton *button, gpointer user_data);
void on_remover_produto_pedido_clicked(GtkButton *button, gpointer user_data);
void on_quantidade_editada(GtkCellRendererText *renderer, gchar *path, gchar *new_text, gpointer user_data);
void on_valor_pago_changed(GtkEditable *editable, gpointer user_data);
void on_metodo_pagamento_changed(GtkComboBox *widget, gpointer user_data);

// Funções auxiliares
void atualizar_totais_pedido(void);
void gerar_comprovante_pedido(Pedido *pedido);

#endif // PEDIDOS_GUI_H
