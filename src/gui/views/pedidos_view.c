#include "pedidos_view.h"
#include "../../backend/backend.h"
#include "../gui.h"

// Variável global para a view de pedidos
static PedidosView pedidos_view;

// Função para criar o painel de detalhes do pedido
static GtkWidget* criar_painel_detalhes();

// Callbacks
void on_novo_pedido(GtkButton *button, gpointer user_data);
void on_adicionar_item(GtkButton *button, gpointer user_data);
void on_remover_item(GtkButton *button, gpointer user_data);
void on_quantidade_changed(GtkSpinButton *spin_button, gpointer user_data);
void on_valor_pago_changed(GtkEditable *editable, gpointer user_data);
void on_metodo_pagamento_changed(GtkComboBox *combo, gpointer user_data);
void on_finalizar_pedido(GtkButton *button, gpointer user_data);

// Funções auxiliares
void atualizar_totais_pedido();
void mostrar_dialogo_novo_pedido();
gboolean filtrar_pedidos(GtkTreeModel *model, GtkTreeIter *iter, gpointer data);
void atualizar_lista_pedidos();

// Implementação das funções
static GtkWidget* criar_painel_detalhes() {
    GtkWidget *panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_style_context_add_class(gtk_widget_get_style_context(panel), "card");
    gtk_widget_set_margin_start(panel, 16);
    gtk_widget_set_margin_end(panel, 16);
    gtk_widget_set_margin_top(panel, 16);
    gtk_widget_set_margin_bottom(panel, 16);
    
    // Título
    GtkWidget *titulo = gtk_label_new(NULL);
    char *markup = g_markup_printf_escaped("<span size='x-large' weight='bold'>Detalhes do Pedido</span>");
    gtk_label_set_markup(GTK_LABEL(titulo), markup);
    g_free(markup);
    gtk_widget_set_halign(titulo, GTK_ALIGN_START);
    
    // Informações do cliente
    GtkWidget *cliente_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *cliente_icon = gtk_image_new_from_icon_name("avatar-default-symbolic",
                                                          GTK_ICON_SIZE_SMALL_TOOLBAR);
    GtkWidget *cliente_label = gtk_label_new("Cliente:");
    GtkWidget *cliente_valor = gtk_label_new("");
    gtk_widget_set_halign(cliente_valor, GTK_ALIGN_START);
    
    gtk_box_pack_start(GTK_BOX(cliente_box), cliente_icon, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(cliente_box), cliente_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(cliente_box), cliente_valor, TRUE, TRUE, 0);
    
    // Lista de itens
    GtkWidget *itens_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(itens_scroll),
                                 GTK_POLICY_NEVER,
                                 GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(itens_scroll, -1, 200);
    
    GtkListStore *itens_store = gtk_list_store_new(NUM_COLS_ITEM,
                                                  G_TYPE_INT,    // ID
                                                  G_TYPE_STRING, // Nome
                                                  G_TYPE_INT,    // Quantidade
                                                  G_TYPE_FLOAT,  // Preço unitário
                                                  G_TYPE_FLOAT,  // Subtotal
                                                  G_TYPE_FLOAT,  // IVA
                                                  G_TYPE_FLOAT); // Total
    
    GtkWidget *itens_tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(itens_store));
    g_object_unref(itens_store);
    
    // Configurar colunas
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Produto",
                                                    renderer,
                                                    "text", COL_ITEM_NOME,
                                                    NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(itens_tree), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Qtd",
                                                    renderer,
                                                    "text", COL_ITEM_QUANTIDADE,
                                                    NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(itens_tree), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Preço Unit.",
                                                    renderer,
                                                    "text", COL_ITEM_PRECO_UNIT,
                                                    NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(itens_tree), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Subtotal",
                                                    renderer,
                                                    "text", COL_ITEM_SUBTOTAL,
                                                    NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(itens_tree), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("IVA",
                                                    renderer,
                                                    "text", COL_ITEM_IVA,
                                                    NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(itens_tree), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Total",
                                                    renderer,
                                                    "text", COL_ITEM_TOTAL,
                                                    NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(itens_tree), column);
    
    gtk_container_add(GTK_CONTAINER(itens_scroll), itens_tree);
    
    // Totais
    GtkWidget *totais_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(totais_grid), 8);
    gtk_grid_set_column_spacing(GTK_GRID(totais_grid), 16);
    
    GtkWidget *subtotal_label = gtk_label_new("Subtotal:");
    GtkWidget *iva_label = gtk_label_new("IVA:");
    GtkWidget *total_label = gtk_label_new("Total:");
    GtkWidget *valor_pago_label = gtk_label_new("Valor Pago:");
    GtkWidget *troco_label = gtk_label_new("Troco:");
    
    GtkWidget *subtotal_valor = gtk_label_new("0.00 Kz");
    GtkWidget *iva_valor = gtk_label_new("0.00 Kz");
    GtkWidget *total_valor = gtk_label_new("0.00 Kz");
    GtkWidget *valor_pago_valor = gtk_label_new("0.00 Kz");
    GtkWidget *troco_valor = gtk_label_new("0.00 Kz");
    
    gtk_grid_attach(GTK_GRID(totais_grid), subtotal_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(totais_grid), subtotal_valor, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(totais_grid), iva_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(totais_grid), iva_valor, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(totais_grid), total_label, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(totais_grid), total_valor, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(totais_grid), valor_pago_label, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(totais_grid), valor_pago_valor, 1, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(totais_grid), troco_label, 0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(totais_grid), troco_valor, 1, 4, 1, 1);
    
    // Botões de ação
    GtkWidget *actions_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_widget_set_halign(actions_box, GTK_ALIGN_END);
    
    GtkWidget *btn_imprimir = custom_button_new_with_icon("Imprimir",
                                                         "printer-symbolic",
                                                         BUTTON_STYLE_PRIMARY);
    GtkWidget *btn_cancelar = custom_button_new_with_icon("Cancelar Pedido",
                                                         "process-stop-symbolic",
                                                         BUTTON_STYLE_DANGER);
    
    gtk_box_pack_end(GTK_BOX(actions_box), btn_cancelar, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(actions_box), btn_imprimir, FALSE, FALSE, 0);
    
    // Adicionar widgets ao painel
    gtk_box_pack_start(GTK_BOX(panel), titulo, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(panel), cliente_box, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(panel), itens_scroll, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(panel), totais_grid, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(panel), actions_box, FALSE, FALSE, 0);
    
    return panel;
}

// Callbacks
void on_novo_pedido(GtkButton *button, gpointer user_data) {
    mostrar_dialogo_novo_pedido();
}

void on_adicionar_item(GtkButton *button, gpointer user_data) {
    CustomDialog *dialog = custom_dialog_new(GTK_WINDOW(main_gui.window), "Adicionar Produto");
    
    // Criar combo de produtos
    GtkWidget *combo = gtk_combo_box_text_new();
    for (int i = 0; i < numProdutos; i++) {
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), produtos[i].nome);
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);
    
    // Criar spin button para quantidade
    GtkWidget *spin = gtk_spin_button_new_with_range(1, 100, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin), 1);
    
    // Adicionar campos ao diálogo
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog->dialog));
    
    GtkWidget *produto_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *produto_label = gtk_label_new("Produto:");
    gtk_box_pack_start(GTK_BOX(produto_box), produto_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(produto_box), combo, TRUE, TRUE, 0);
    
    GtkWidget *qtd_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *qtd_label = gtk_label_new("Quantidade:");
    gtk_box_pack_start(GTK_BOX(qtd_box), qtd_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(qtd_box), spin, TRUE, TRUE, 0);
    
    gtk_box_pack_start(GTK_BOX(content_area), produto_box, FALSE, FALSE, 8);
    gtk_box_pack_start(GTK_BOX(content_area), qtd_box, FALSE, FALSE, 8);
    
    // Adicionar botões
    custom_dialog_add_action_button(dialog, "Cancelar", "window-close-symbolic",
                                  G_CALLBACK(custom_dialog_destroy), dialog);
    
    GtkWidget *btn_adicionar = custom_button_new_with_icon("Adicionar",
                                                          "list-add-symbolic",
                                                          BUTTON_STYLE_SUCCESS);
    g_signal_connect(btn_adicionar, "clicked", G_CALLBACK(custom_dialog_destroy), dialog);
    
    gtk_widget_show_all(dialog->dialog);
    
    int response = gtk_dialog_run(GTK_DIALOG(dialog->dialog));
    if (response == GTK_RESPONSE_ACCEPT) {
        int produto_idx = gtk_combo_box_get_active(GTK_COMBO_BOX(combo));
        int quantidade = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin));
        
        GtkTreeIter iter;
        gtk_list_store_append(pedidos_view.novo_pedido.produtos_store, &iter);
        
        float preco = produtos[produto_idx].preco;
        float subtotal = preco * quantidade;
        float iva = subtotal * TAXA_IVA / 100.0f;
        float total = subtotal + iva;
        
        gtk_list_store_set(pedidos_view.novo_pedido.produtos_store, &iter,
                          COL_ITEM_ID, produtos[produto_idx].id,
                          COL_ITEM_NOME, produtos[produto_idx].nome,
                          COL_ITEM_QUANTIDADE, quantidade,
                          COL_ITEM_PRECO_UNIT, preco,
                          COL_ITEM_SUBTOTAL, subtotal,
                          COL_ITEM_IVA, iva,
                          COL_ITEM_TOTAL, total,
                          -1);
        
        atualizar_totais_pedido();
    }
    
    custom_dialog_destroy(dialog);
}

void on_remover_item(GtkButton *button, gpointer user_data) {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(
        GTK_TREE_VIEW(pedidos_view.novo_pedido.produtos_tree)
    );
    GtkTreeModel *model;
    GtkTreeIter iter;
    
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        gtk_list_store_remove(pedidos_view.novo_pedido.produtos_store, &iter);
        atualizar_totais_pedido();
    }
}

void on_quantidade_changed(GtkSpinButton *spin_button, gpointer user_data) {
    atualizar_totais_pedido();
}

void on_valor_pago_changed(GtkEditable *editable, gpointer user_data) {
    const char *texto = gtk_entry_get_text(GTK_ENTRY(editable));
    float valor_pago = atof(texto);
    
    // Obter total atual
    const char *total_text = gtk_label_get_text(GTK_LABEL(pedidos_view.novo_pedido.total_iva_label));
    float total = atof(total_text);
    
    // Calcular troco
    float troco = valor_pago - total;
    
    // Atualizar label do troco
    char troco_str[32];
    snprintf(troco_str, sizeof(troco_str), "Troco: %.2f Kz", troco);
    gtk_label_set_text(GTK_LABEL(pedidos_view.novo_pedido.troco_label), troco_str);
}

void on_metodo_pagamento_changed(GtkComboBox *combo, gpointer user_data) {
    // Implementação futura se necessário
}

void on_finalizar_pedido(GtkButton *button, gpointer user_data) {
    // Criar novo pedido
    Pedido novo_pedido;
    novo_pedido.numProdutos = 0;
    
    // Obter cliente selecionado
    int cliente_idx = gtk_combo_box_get_active(
        GTK_COMBO_BOX(pedidos_view.novo_pedido.cliente_combo)
    );
    novo_pedido.idCliente = clientes[cliente_idx].id;
    
    // Obter produtos
    GtkTreeModel *model = GTK_TREE_MODEL(pedidos_view.novo_pedido.produtos_store);
    GtkTreeIter iter;
    gboolean valid = gtk_tree_model_get_iter_first(model, &iter);
    
    float total = 0.0f;
    
    while (valid) {
        int id_produto, quantidade;
        float preco_total;
        
        gtk_tree_model_get(model, &iter,
                          COL_ITEM_ID, &id_produto,
                          COL_ITEM_QUANTIDADE, &quantidade,
                          COL_ITEM_TOTAL, &preco_total,
                          -1);
        
        novo_pedido.idProduto[novo_pedido.numProdutos] = id_produto;
        novo_pedido.quantidade[novo_pedido.numProdutos] = quantidade;
        novo_pedido.numProdutos++;
        
        total += preco_total;
        
        valid = gtk_tree_model_iter_next(model, &iter);
    }
    
    novo_pedido.total = total;
    
    // Obter valor pago
    const char *valor_pago_str = gtk_entry_get_text(
        GTK_ENTRY(pedidos_view.novo_pedido.valor_pago_entry)
    );
    novo_pedido.valorPago = atof(valor_pago_str);
    
    // Calcular troco
    novo_pedido.troco = novo_pedido.valorPago - novo_pedido.total;
    
    // Obter método de pagamento
    const char *metodo = gtk_combo_box_text_get_active_text(
        GTK_COMBO_BOX_TEXT(pedidos_view.novo_pedido.metodo_pagamento_combo)
    );
    strncpy(novo_pedido.metodoPagamento, metodo, sizeof(novo_pedido.metodoPagamento) - 1);
    
    // Salvar pedido
    if (fazerPedido(novo_pedido) == 0) {
        mostrar_sucesso("Pedido realizado com sucesso!");
        gtk_widget_destroy(pedidos_view.novo_pedido.dialog);
        atualizar_lista_pedidos();
    } else {
        mostrar_erro("Erro ao realizar pedido!");
    }
}

// Função para atualizar totais do pedido
void atualizar_totais_pedido() {
    float subtotal = 0.0f;
    float total_iva = 0.0f;
    
    GtkTreeModel *model = GTK_TREE_MODEL(pedidos_view.novo_pedido.produtos_store);
    GtkTreeIter iter;
    gboolean valid = gtk_tree_model_get_iter_first(model, &iter);
    
    while (valid) {
        float subtotal_item, total_item;
        gtk_tree_model_get(model, &iter,
                          COL_ITEM_SUBTOTAL, &subtotal_item,
                          COL_ITEM_TOTAL, &total_item,
                          -1);
        
        subtotal += subtotal_item;
        total_iva += total_item;
        
        valid = gtk_tree_model_iter_next(model, &iter);
    }
    
    char subtotal_str[32], total_str[32];
    snprintf(subtotal_str, sizeof(subtotal_str), "Total: %.2f Kz", subtotal);
    snprintf(total_str, sizeof(total_str), "Total com IVA: %.2f Kz", total_iva);
    
    gtk_label_set_text(GTK_LABEL(pedidos_view.novo_pedido.total_label), subtotal_str);
    gtk_label_set_text(GTK_LABEL(pedidos_view.novo_pedido.total_iva_label), total_str);
}

// Função para filtrar pedidos
gboolean filtrar_pedidos(GtkTreeModel *model, GtkTreeIter *iter, gpointer data) {
    // Por enquanto, mostrar todos os pedidos
    return TRUE;
}

// Função para atualizar a lista de pedidos
void atualizar_lista_pedidos() {
    gtk_list_store_clear(pedidos_view.store);
    
    for (int i = 0; i < numPedidos; i++) {
        GtkTreeIter iter;
        gtk_list_store_append(pedidos_view.store, &iter);
        
        // Encontrar nome do cliente
        char *nome_cliente = "Cliente não encontrado";
        for (int j = 0; j < numClientes; j++) {
            if (clientes[j].id == pedidos[i].idCliente) {
                nome_cliente = clientes[j].nome;
                break;
            }
        }
        
        gtk_list_store_set(pedidos_view.store, &iter,
                          COL_PEDIDO_ID, pedidos[i].id,
                          COL_PEDIDO_CLIENTE, nome_cliente,
                          COL_PEDIDO_TOTAL, pedidos[i].total,
                          -1);
    }
}

// Função para criar a tela de pedidos
GtkWidget* criar_tela_pedidos() {
    // Container principal com divisor
    GtkWidget *paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    
    // Lado esquerdo - Lista de pedidos
    GtkWidget *lista_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_container_set_border_width(GTK_CONTAINER(lista_box), 16);
    
    // Barra de ferramentas
    GtkWidget *toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    
    // Botão novo pedido
    GtkWidget *btn_novo = custom_button_new_with_icon("Novo Pedido",
                                                     "list-add-symbolic",
                                                     BUTTON_STYLE_SUCCESS);
    g_signal_connect(btn_novo, "clicked", G_CALLBACK(on_novo_pedido), NULL);
    
    // Campo de busca
    pedidos_view.search_entry = gtk_search_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(pedidos_view.search_entry),
                                 "Pesquisar pedidos...");
    
    // Filtros
    GtkWidget *filtros_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    
    pedidos_view.periodo_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(pedidos_view.periodo_combo),
                                 "Hoje");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(pedidos_view.periodo_combo),
                                 "Esta Semana");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(pedidos_view.periodo_combo),
                                 "Este Mês");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(pedidos_view.periodo_combo),
                                 "Todos");
    gtk_combo_box_set_active(GTK_COMBO_BOX(pedidos_view.periodo_combo), 0);
    
    gtk_box_pack_start(GTK_BOX(filtros_box), pedidos_view.periodo_combo,
                      FALSE, FALSE, 0);
    
    // Adicionar widgets à toolbar
    gtk_box_pack_start(GTK_BOX(toolbar), btn_novo, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(toolbar), pedidos_view.search_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(toolbar), filtros_box, FALSE, FALSE, 0);
    
    // Lista de pedidos
    pedidos_view.store = gtk_list_store_new(NUM_COLS_PEDIDO,
                                          G_TYPE_INT,     // ID
                                          G_TYPE_STRING,  // Cliente
                                          G_TYPE_FLOAT,   // Total
                                          G_TYPE_STRING,  // Data
                                          G_TYPE_STRING); // Status
    
    pedidos_view.filter = GTK_TREE_MODEL_FILTER(
        gtk_tree_model_filter_new(GTK_TREE_MODEL(pedidos_view.store), NULL)
    );
    gtk_tree_model_filter_set_visible_func(pedidos_view.filter,
                                         filtrar_pedidos, NULL, NULL);
    
    pedidos_view.sort = GTK_TREE_MODEL_SORT(
        gtk_tree_model_sort_new_with_model(GTK_TREE_MODEL(pedidos_view.filter))
    );
    
    pedidos_view.pedidos_list = gtk_tree_view_new_with_model(
        GTK_TREE_MODEL(pedidos_view.sort)
    );
    
    // Configurar colunas
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("ID",
                                                    renderer,
                                                    "text", COL_PEDIDO_ID,
                                                    NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(pedidos_view.pedidos_list), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Cliente",
                                                    renderer,
                                                    "text", COL_PEDIDO_CLIENTE,
                                                    NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(pedidos_view.pedidos_list), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Total",
                                                    renderer,
                                                    "text", COL_PEDIDO_TOTAL,
                                                    NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(pedidos_view.pedidos_list), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Data",
                                                    renderer,
                                                    "text", COL_PEDIDO_DATA,
                                                    NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(pedidos_view.pedidos_list), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Status",
                                                    renderer,
                                                    "text", COL_PEDIDO_STATUS,
                                                    NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(pedidos_view.pedidos_list), column);
    
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scroll), pedidos_view.pedidos_list);
    
    // Adicionar widgets ao container da lista
    gtk_box_pack_start(GTK_BOX(lista_box), toolbar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(lista_box), scroll, TRUE, TRUE, 0);
    
    // Lado direito - Detalhes do pedido
    pedidos_view.detalhes_panel = criar_painel_detalhes();
    
    // Adicionar painéis ao divisor
    gtk_paned_pack1(GTK_PANED(paned), lista_box, TRUE, TRUE);
    gtk_paned_pack2(GTK_PANED(paned), pedidos_view.detalhes_panel, TRUE, TRUE);
    
    // Definir posição inicial do divisor
    gtk_paned_set_position(GTK_PANED(paned), 400);
    
    // Carregar dados iniciais
    atualizar_lista_pedidos();
    
    return paned;
}

// Função para mostrar diálogo de novo pedido
void mostrar_dialogo_novo_pedido() {
    pedidos_view.novo_pedido.dialog = gtk_dialog_new_with_buttons(
        "Novo Pedido",
        GTK_WINDOW(main_gui.window),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        NULL,
        NULL
    );
    
    gtk_window_set_default_size(GTK_WINDOW(pedidos_view.novo_pedido.dialog), 800, 600);
    
    GtkWidget *content_area = gtk_dialog_get_content_area(
        GTK_DIALOG(pedidos_view.novo_pedido.dialog)
    );
    
    // Cliente
    GtkWidget *cliente_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *cliente_label = gtk_label_new("Cliente:");
    pedidos_view.novo_pedido.cliente_combo = gtk_combo_box_text_new();
    
    // Preencher combo de clientes
    for (int i = 0; i < numClientes; i++) {
        gtk_combo_box_text_append_text(
            GTK_COMBO_BOX_TEXT(pedidos_view.novo_pedido.cliente_combo),
            clientes[i].nome
