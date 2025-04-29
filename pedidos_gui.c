#include <gtk/gtk.h>
#include <string.h>
#include "pedidos_gui.h"
#include "backend.h"

// Variável global para armazenar os widgets da interface de pedidos
static PedidosGui pedidos_gui;

// Array temporário para armazenar itens do pedido atual
static ItemPedido itens_pedido[MAX_PRODUTOS];
static int num_itens_pedido = 0;
static float total_pedido = 0.0f;
static float total_iva_pedido = 0.0f;

// Enumeração para as colunas da TreeView de pedidos
enum {
    COL_PEDIDO_ID,
    COL_PEDIDO_CLIENTE,
    COL_PEDIDO_TOTAL,
    COL_PEDIDO_TOTAL_IVA,
    COL_PEDIDO_METODO,
    COL_PEDIDO_DATA,
    NUM_COLS_PEDIDO
};

// Enumeração para as colunas da TreeView de itens do pedido
enum {
    COL_ITEM_ID,
    COL_ITEM_NOME,
    COL_ITEM_QUANTIDADE,
    COL_ITEM_PRECO,
    COL_ITEM_SUBTOTAL,
    COL_ITEM_IVA,
    COL_ITEM_TOTAL,
    NUM_COLS_ITEM
};

// Função para calcular o IVA
static float calcular_iva(float valor) {
    return valor * (TAXA_IVA / 100.0f);
}

// Função para atualizar os totais do pedido
void atualizar_totais_pedido() {
    total_pedido = 0.0f;
    total_iva_pedido = 0.0f;

    for (int i = 0; i < num_itens_pedido; i++) {
        total_pedido += itens_pedido[i].subtotal;
        total_iva_pedido += itens_pedido[i].iva;
    }

    char texto_total[50], texto_total_iva[50];
    snprintf(texto_total, sizeof(texto_total), "Total: %.2f Kz", total_pedido);
    snprintf(texto_total_iva, sizeof(texto_total_iva), "Total com IVA: %.2f Kz", total_pedido + total_iva_pedido);

    gtk_label_set_text(GTK_LABEL(pedidos_gui.label_total), texto_total);
    gtk_label_set_text(GTK_LABEL(pedidos_gui.label_total_iva), texto_total_iva);
}

// Callback para quando a quantidade é editada
void on_quantidade_editada(GtkCellRendererText *renderer, gchar *path, gchar *new_text, gpointer user_data) {
    GtkTreeIter iter;
    GtkTreePath *treepath = gtk_tree_path_new_from_string(path);
    
    if (gtk_tree_model_get_iter(GTK_TREE_MODEL(pedidos_gui.liststore_produtos_pedido), &iter, treepath)) {
        int quantidade = atoi(new_text);
        if (quantidade > 0) {
            int id_produto;
            float preco_unitario;
            
            gtk_tree_model_get(GTK_TREE_MODEL(pedidos_gui.liststore_produtos_pedido), &iter,
                             COL_ITEM_ID, &id_produto,
                             COL_ITEM_PRECO, &preco_unitario,
                             -1);

            // Atualizar o item no array temporário
            for (int i = 0; i < num_itens_pedido; i++) {
                if (itens_pedido[i].id_produto == id_produto) {
                    itens_pedido[i].quantidade = quantidade;
                    itens_pedido[i].subtotal = preco_unitario * quantidade;
                    itens_pedido[i].iva = calcular_iva(itens_pedido[i].subtotal);
                    itens_pedido[i].total_com_iva = itens_pedido[i].subtotal + itens_pedido[i].iva;
                    
                    // Atualizar a linha na TreeView
                    gtk_list_store_set(pedidos_gui.liststore_produtos_pedido, &iter,
                        COL_ITEM_QUANTIDADE, quantidade,
                        COL_ITEM_SUBTOTAL, itens_pedido[i].subtotal,
                        COL_ITEM_IVA, itens_pedido[i].iva,
                        COL_ITEM_TOTAL, itens_pedido[i].total_com_iva,
                        -1);
                    break;
                }
            }
            
            atualizar_totais_pedido();
        }
    }
    
    gtk_tree_path_free(treepath);
}

// Callback para quando o valor pago é alterado
void on_valor_pago_changed(GtkEditable *editable, gpointer user_data) {
    const char *texto = gtk_entry_get_text(GTK_ENTRY(editable));
    float valor_pago = atof(texto);
    float troco = valor_pago - (total_pedido + total_iva_pedido);

    char texto_troco[50];
    snprintf(texto_troco, sizeof(texto_troco), "Troco: %.2f Kz", troco);
    gtk_label_set_text(GTK_LABEL(pedidos_gui.label_troco), texto_troco);
}

// Callback para quando o método de pagamento é alterado
void on_metodo_pagamento_changed(GtkComboBox *widget, gpointer user_data) {
    if (gtk_combo_box_get_active(widget) == 0) { // TPA
        gtk_widget_set_sensitive(pedidos_gui.entry_valor_pago, FALSE);
        gtk_entry_set_text(GTK_ENTRY(pedidos_gui.entry_valor_pago), "");
        gtk_label_set_text(GTK_LABEL(pedidos_gui.label_troco), "Troco: 0.00 Kz");
    } else { // Dinheiro
        gtk_widget_set_sensitive(pedidos_gui.entry_valor_pago, TRUE);
    }
}

// Função para adicionar produto ao pedido
void on_adicionar_produto_pedido_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        "Adicionar Produto",
        GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button))),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "Cancelar", GTK_RESPONSE_CANCEL,
        "Adicionar", GTK_RESPONSE_ACCEPT,
        NULL
    );

    // Criar ComboBox para seleção de produto
    GtkWidget *combo_produtos = gtk_combo_box_text_new();
    for (int i = 0; i < numProdutos; i++) {
        char texto[100];
        snprintf(texto, sizeof(texto), "%s (%.2f Kz)", produtos[i].nome, produtos[i].preco);
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_produtos), texto);
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo_produtos), 0);

    // Criar SpinButton para quantidade
    GtkWidget *spin_quantidade = gtk_spin_button_new_with_range(1, 100, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_quantidade), 1);

    // Adicionar widgets ao diálogo
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
    gtk_container_set_border_width(GTK_CONTAINER(grid), 10);

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Produto:"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), combo_produtos, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Quantidade:"), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), spin_quantidade, 1, 1, 1, 1);

    gtk_container_add(GTK_CONTAINER(content_area), grid);
    gtk_widget_show_all(dialog);

    int response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_ACCEPT) {
        int produto_index = gtk_combo_box_get_active(GTK_COMBO_BOX(combo_produtos));
        int quantidade = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_quantidade));

        // Verificar se o produto já está no pedido
        int produto_existente = -1;
        for (int i = 0; i < num_itens_pedido; i++) {
            if (itens_pedido[i].id_produto == produtos[produto_index].id) {
                produto_existente = i;
                break;
            }
        }

        if (produto_existente >= 0) {
            // Atualizar quantidade do produto existente
            itens_pedido[produto_existente].quantidade += quantidade;
            itens_pedido[produto_existente].subtotal = produtos[produto_index].preco * itens_pedido[produto_existente].quantidade;
            itens_pedido[produto_existente].iva = calcular_iva(itens_pedido[produto_existente].subtotal);
            itens_pedido[produto_existente].total_com_iva = itens_pedido[produto_existente].subtotal + itens_pedido[produto_existente].iva;

            // Atualizar TreeView
            GtkTreeIter iter;
            gboolean valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(pedidos_gui.liststore_produtos_pedido), &iter);
            while (valid) {
                int id;
                gtk_tree_model_get(GTK_TREE_MODEL(pedidos_gui.liststore_produtos_pedido), &iter, COL_ITEM_ID, &id, -1);
                if (id == produtos[produto_index].id) {
                    gtk_list_store_set(pedidos_gui.liststore_produtos_pedido, &iter,
                        COL_ITEM_QUANTIDADE, itens_pedido[produto_existente].quantidade,
                        COL_ITEM_SUBTOTAL, itens_pedido[produto_existente].subtotal,
                        COL_ITEM_IVA, itens_pedido[produto_existente].iva,
                        COL_ITEM_TOTAL, itens_pedido[produto_existente].total_com_iva,
                        -1);
                    break;
                }
                valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(pedidos_gui.liststore_produtos_pedido), &iter);
            }
        } else {
            // Adicionar novo item ao pedido
            ItemPedido novo_item;
            novo_item.id_produto = produtos[produto_index].id;
            strncpy(novo_item.nome, produtos[produto_index].nome, sizeof(novo_item.nome) - 1);
            novo_item.quantidade = quantidade;
            novo_item.preco_unitario = produtos[produto_index].preco;
            novo_item.subtotal = produtos[produto_index].preco * quantidade;
            novo_item.iva = calcular_iva(novo_item.subtotal);
            novo_item.total_com_iva = novo_item.subtotal + novo_item.iva;

            itens_pedido[num_itens_pedido++] = novo_item;

            // Adicionar à TreeView
            GtkTreeIter iter;
            gtk_list_store_append(pedidos_gui.liststore_produtos_pedido, &iter);
            gtk_list_store_set(pedidos_gui.liststore_produtos_pedido, &iter,
                COL_ITEM_ID, novo_item.id_produto,
                COL_ITEM_NOME, novo_item.nome,
                COL_ITEM_QUANTIDADE, novo_item.quantidade,
                COL_ITEM_PRECO, novo_item.preco_unitario,
                COL_ITEM_SUBTOTAL, novo_item.subtotal,
                COL_ITEM_IVA, novo_item.iva,
                COL_ITEM_TOTAL, novo_item.total_com_iva,
                -1);
        }

        atualizar_totais_pedido();
    }

    gtk_widget_destroy(dialog);
}

// Função para remover produto do pedido
void on_remover_produto_pedido_clicked(GtkButton *button, gpointer user_data) {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(pedidos_gui.tree_produtos_pedido));
    GtkTreeModel *model;
    GtkTreeIter iter;

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        int id_produto;
        gtk_tree_model_get(model, &iter, COL_ITEM_ID, &id_produto, -1);

        // Remover do array de itens
        for (int i = 0; i < num_itens_pedido; i++) {
            if (itens_pedido[i].id_produto == id_produto) {
                for (int j = i; j < num_itens_pedido - 1; j++) {
                    itens_pedido[j] = itens_pedido[j + 1];
                }
                num_itens_pedido--;
                break;
            }
        }

        // Remover da TreeView
        gtk_list_store_remove(pedidos_gui.liststore_produtos_pedido, &iter);
        atualizar_totais_pedido();
    }
}

// Função para criar novo pedido
void on_novo_pedido_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *dialog = criar_dialogo_novo_pedido(GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button))));
    
    int response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_ACCEPT) {
        // Verificar se há itens no pedido
        if (num_itens_pedido == 0) {
            GtkWidget *error_dialog = gtk_message_dialog_new(
                GTK_WINDOW(dialog),
                GTK_DIALOG_MODAL,
                GTK_MESSAGE_ERROR,
                GTK_BUTTONS_OK,
                "Erro: Adicione pelo menos um produto ao pedido!"
            );
            gtk_dialog_run(GTK_DIALOG(error_dialog));
            gtk_widget_destroy(error_dialog);
            gtk_widget_destroy(dialog);
            return;
        }

        // Criar novo pedido
        Pedido novo_pedido;
        novo_pedido.id = numPedidos + 1;
        
        // Obter cliente selecionado
        GtkTreeIter iter;
        if (gtk_combo_box_get_active_iter(GTK_COMBO_BOX(pedidos_gui.combo_clientes), &iter)) {
            gtk_tree_model_get(gtk_combo_box_get_model(GTK_COMBO_BOX(pedidos_gui.combo_clientes)),
                             &iter, 0, &novo_pedido.idCliente, -1);
        }

        // Copiar itens do pedido
        novo_pedido.numProdutos = num_itens_pedido;
        for (int i = 0; i < num_itens_pedido; i++) {
            novo_pedido.idProduto[i] = itens_pedido[i].id_produto;
            novo_pedido.quantidade[i] = itens_pedido[i].quantidade;
        }

        // Calcular totais
        novo_pedido.total = total_pedido + total_iva_pedido;

        // Método de pagamento
        const char *metodos[] = {"TPA", "Dinheiro"};
        int metodo_index = gtk_combo_box_get_active(GTK_COMBO_BOX(pedidos_gui.combo_pagamento));
        strncpy(novo_pedido.metodoPagamento, metodos[metodo_index], sizeof(novo_pedido.metodoPagamento) - 1);

        // Valor pago e troco
        const char *valor_pago_texto = gtk_entry_get_text(GTK_ENTRY(pedidos_gui.entry_valor_pago));
        if (metodo_index == 0) { // TPA
            novo_pedido.valorPago = novo_pedido.total;
            novo_pedido.troco = 0;
        } else { // Dinheiro
            novo_pedido.valorPago = atof(valor_pago_texto);
            novo_pedido.troco = novo_pedido.valorPago - novo_pedido.total;
        }

        // Salvar pedido
        if (fazerPedido(novo_pedido) == 0) {
            // Gerar comprovante
            gerar_comprovante_pedido(&novo_pedido);
            
            // Atualizar interface
            atualizar_lista_pedidos();
            
            // Mostrar mensagem de sucesso
            GtkWidget *msg_dialog = gtk_message_dialog_new(
                GTK_WINDOW(dialog),
                GTK_DIALOG_MODAL,
                GTK_MESSAGE_INFO,
                GTK_BUTTONS_OK,
                "Pedido realizado com sucesso!"
            );
            gtk_dialog_run(GTK_DIALOG(msg_dialog));
            gtk_widget_destroy(msg_dialog);
        }
    }
    
    gtk_widget_destroy(dialog);
}

// Função para visualizar detalhes do pedido
void on_visualizar_pedido_clicked(GtkButton *button, gpointer user_data) {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(pedidos_gui.treeview));
    GtkTreeModel *model;
    GtkTreeIter iter;

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        int id_pedido;
        gtk_tree_model_get(model, &iter, COL_PEDIDO_ID, &id_pedido, -1);

        // Encontrar pedido
        Pedido *pedido = NULL;
        for (int i = 0; i < numPedidos; i++) {
            if (pedidos[i].id == id_pedido) {
                pedido = &pedidos[i];
                break;
            }
        }

        if (pedido) {
            GtkWidget *dialog = gtk_dialog_new_with_buttons(
                "Detalhes do Pedido",
                GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button))),
                GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                "Fechar", GTK_RESPONSE_CLOSE,
                NULL
            );

            GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
            GtkWidget *grid = gtk_grid_new();
            gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
            gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
            gtk_container_set_border_width(GTK_CONTAINER(grid), 10);

            // Informações do pedido
            char texto[100];
            
            snprintf(texto, sizeof(texto), "Pedido #%d", pedido->id);
            GtkWidget *label_titulo = gtk_label_new(NULL);
            char *markup = g_markup_printf_escaped("<span size='large' weight='bold'>%s</span>", texto);
            gtk_label_set_markup(GTK_LABEL(label_titulo), markup);
            g_free(markup);
            
            // Cliente
            Cliente *cliente = &clientes[pedido->idCliente - 1];
            snprintf(texto, sizeof(texto), "Cliente: %s", cliente->nome);
            GtkWidget *label_cliente = gtk_label_new(texto);
            
            // Lista de produtos
            GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
            gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                        GTK_POLICY_AUTOMATIC,
                                        GTK_POLICY_AUTOMATIC);
            
            GtkListStore *store = gtk_list_store_new(5,
                G_TYPE_STRING,  // Nome
                G_TYPE_INT,     // Quantidade
                G_TYPE_FLOAT,   // Preço
                G_TYPE_FLOAT,   // IVA
                G_TYPE_FLOAT    // Total
            );
            
            GtkWidget *tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
            
            // Adicionar colunas
            GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
            gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree),
                -1, "Produto", renderer, "text", 0, NULL);
            
            renderer = gtk_cell_renderer_text_new();
            gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree),
                -1, "Qtd", renderer, "text", 1, NULL);
            
            renderer = gtk_cell_renderer_text_new();
            gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree),
                -1, "Preço", renderer, "text", 2, NULL);
            
            renderer = gtk_cell_renderer_text_new();
            gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree),
                -1, "IVA", renderer, "text", 3, NULL);
            
            renderer = gtk_cell_renderer_text_new();
            gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree),
                -1, "Total", renderer, "text", 4, NULL);
            
            // Adicionar produtos
            for (int i = 0; i < pedido->numProdutos; i++) {
                Produto *prod = &produtos[pedido->idProduto[i] - 1];
                float subtotal = prod->preco * pedido->quantidade[i];
                float iva = calcular_iva(subtotal);
                
                GtkTreeIter iter;
                gtk_list_store_append(store, &iter);
                gtk_list_store_set(store, &iter,
                    0, prod->nome,
                    1, pedido->quantidade[i],
                    2, prod->preco,
                    3, iva,
                    4, subtotal + iva,
                    -1);
            }
            
            gtk_container_add(GTK_CONTAINER(scrolled), tree);
            
            // Totais
            snprintf(texto, sizeof(texto), "Total: %.2f Kz", pedido->total);
            GtkWidget *label_total = gtk_label_new(texto);
            
            snprintf(texto, sizeof(texto), "Método de Pagamento: %s", pedido->metodoPagamento);
            GtkWidget *label_metodo = gtk_label_new(texto);
            
            snprintf(texto, sizeof(texto), "Valor Pago: %.2f Kz", pedido->valorPago);
            GtkWidget *label_valor_pago = gtk_label_new(texto);
            
            snprintf(texto, sizeof(texto), "Troco: %.2f Kz", pedido->troco);
            GtkWidget *label_troco = gtk_label_new(texto);
            
            // Adicionar widgets ao grid
            int row = 0;
            gtk_grid_attach(GTK_GRID(grid), label_titulo, 0, row++, 2, 1);
            gtk_grid_attach(GTK_GRID(grid), label_cliente, 0, row++, 2, 1);
            gtk_grid_attach(GTK_GRID(grid), scrolled, 0, row++, 2, 1);
            gtk_grid_attach(GTK_GRID(grid), label_total, 0, row++, 2, 1);
            gtk_grid_attach(GTK_GRID(grid), label_metodo, 0, row++, 2, 1);
            gtk_grid_attach(GTK_GRID(grid), label_valor_pago, 0, row++, 2, 1);
            gtk_grid_attach(GTK_GRID(grid), label_troco, 0, row++, 2, 1);
            
            gtk_container_add(GTK_CONTAINER(content_area), grid);
            gtk_widget_show_all(dialog);
            
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
        }
    }
}

// Função para criar diálogo de novo pedido
GtkWidget* criar_dialogo_novo_pedido(GtkWindow *parent) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        "Novo Pedido",
        parent,
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "Cancelar", GTK_RESPONSE_CANCEL,
        "Finalizar Pedido", GTK_RESPONSE_ACCEPT,
        NULL
    );

    gtk_window_set_default_size(GTK_WINDOW(dialog), 600, 400);

    // Container principal
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(main_box), 10);

    // Seleção de cliente
    GtkWidget *frame_cliente = gtk_frame_new("Cliente");
    GtkWidget *box_cliente = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(box_cliente), 5);

    pedidos_gui.combo_clientes = gtk_combo_box_text_new();
    for (int i = 0; i < numClientes; i++) {
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(pedidos_gui.combo_clientes), clientes[i].nome);
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(pedidos_gui.combo_clientes), 0);

    gtk_box_pack_start(GTK_BOX(box_cliente), pedidos_gui.combo_clientes, TRUE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(frame_cliente), box_cliente);
    gtk_box_pack_start(GTK_BOX(main_box), frame_cliente, FALSE, FALSE, 0);

    // Lista de produtos do pedido
    GtkWidget *frame_produtos = gtk_frame_new("Produtos do Pedido");
    GtkWidget *box_produtos = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(box_produtos), 5);

    // Botões para adicionar/remover produtos
    GtkWidget *box_botoes = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *btn_adicionar = gtk_button_new_with_label("Adicionar Produto");
    GtkWidget *btn_remover = gtk_button_new_with_label("Remover Produto");
    
    gtk_box_pack_start(GTK_BOX(box_botoes), btn_adicionar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_botoes), btn_remover, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_produtos), box_botoes, FALSE, FALSE, 0);

    // TreeView para produtos do pedido
    pedidos_gui.liststore_produtos_pedido = gtk_list_store_new(NUM_COLS_ITEM,
        G_TYPE_INT,     // ID
        G_TYPE_STRING,  // Nome
        G_TYPE_INT,     // Quantidade
        G_TYPE_FLOAT,   // Preço
        G_TYPE_FLOAT,   // Subtotal
        G_TYPE_FLOAT,   // IVA
        G_TYPE_FLOAT    // Total
    );

    pedidos_gui.tree_produtos_pedido = gtk_tree_view_new_with_model(
        GTK_TREE_MODEL(pedidos_gui.liststore_produtos_pedido));

    // Configurar colunas
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    // Coluna Nome
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Produto", renderer, "text", COL_ITEM_NOME, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(pedidos_gui.tree_produtos_pedido), column);

    // Coluna Quantidade (editável)
    renderer = gtk_cell_renderer_text_new();
    g_object_set(renderer, "editable", TRUE, NULL);
    g_signal_connect(renderer, "edited", G_CALLBACK(on_quantidade_editada), NULL);
    column = gtk_tree_view_column_new_with_attributes("Qtd", renderer, "text", COL_ITEM_QUANTIDADE, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(pedidos_gui.tree_produtos_pedido), column);

    // Coluna Preço
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Preço (Kz)", renderer, "text", COL_ITEM_
