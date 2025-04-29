#include "produtos_view.h"
#include "../../backend/backend.h"
#include "../gui.h"

// Variável global para a view de produtos
static ProdutosView produtos_gui;

// Função para criar um card de produto
GtkWidget* criar_card_produto(const char *nome, const char *descricao, 
                            float preco, float preco_iva) {
    GtkWidget *card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_style_context_add_class(gtk_widget_get_style_context(card), "card");
    
    // Nome do produto
    GtkWidget *nome_label = gtk_label_new(NULL);
    char *markup = g_markup_printf_escaped("<span size='large' weight='bold'>%s</span>", nome);
    gtk_label_set_markup(GTK_LABEL(nome_label), markup);
    g_free(markup);
    
    // Descrição
    GtkWidget *desc_label = gtk_label_new(descricao);
    gtk_label_set_line_wrap(GTK_LABEL(desc_label), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(desc_label), 30);
    
    // Preços
    GtkWidget *preco_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);
    
    char preco_str[32], preco_iva_str[32];
    snprintf(preco_str, sizeof(preco_str), "Preço: %.2f Kz", preco);
    snprintf(preco_iva_str, sizeof(preco_iva_str), "Com IVA: %.2f Kz", preco_iva);
    
    GtkWidget *preco_label = gtk_label_new(preco_str);
    GtkWidget *preco_iva_label = gtk_label_new(preco_iva_str);
    
    gtk_box_pack_start(GTK_BOX(preco_box), preco_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(preco_box), preco_iva_label, FALSE, FALSE, 0);
    
    // Adicionar widgets ao card
    gtk_box_pack_start(GTK_BOX(card), nome_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(card), desc_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(card), preco_box, FALSE, FALSE, 0);
    
    gtk_widget_set_margin_start(card, 8);
    gtk_widget_set_margin_end(card, 8);
    gtk_widget_set_margin_top(card, 8);
    gtk_widget_set_margin_bottom(card, 8);
    
    return card;
}

// Função para configurar as colunas da lista
void configurar_colunas_lista(GtkTreeView *treeview) {
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    
    // Coluna ID
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("ID", renderer,
                                                    "text", COL_ID, NULL);
    gtk_tree_view_append_column(treeview, column);
    
    // Coluna Nome
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Nome", renderer,
                                                    "text", COL_NOME, NULL);
    gtk_tree_view_column_set_resizable(column, TRUE);
    gtk_tree_view_append_column(treeview, column);
    
    // Coluna Descrição
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Descrição", renderer,
                                                    "text", COL_DESCRICAO, NULL);
    gtk_tree_view_column_set_resizable(column, TRUE);
    gtk_tree_view_append_column(treeview, column);
    
    // Coluna Preço
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Preço (Kz)", renderer,
                                                    "text", COL_PRECO, NULL);
    gtk_tree_view_append_column(treeview, column);
    
    // Coluna Preço com IVA
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Preço com IVA (Kz)", renderer,
                                                    "text", COL_PRECO_IVA, NULL);
    gtk_tree_view_append_column(treeview, column);
}

// Função de filtro para produtos
gboolean filtrar_produtos(GtkTreeModel *model, GtkTreeIter *iter, gpointer data) {
    char *nome, *descricao;
    double preco;
    const char *texto_busca = gtk_entry_get_text(GTK_ENTRY(produtos_gui.search_entry));
    double valor_max = gtk_range_get_value(GTK_RANGE(produtos_gui.price_slider));
    
    gtk_tree_model_get(model, iter,
                      COL_NOME, &nome,
                      COL_DESCRICAO, &descricao,
                      COL_PRECO, &preco,
                      -1);
    
    gboolean visivel = TRUE;
    
    // Filtrar por texto
    if (texto_busca && *texto_busca != '\0') {
        char *nome_lower = g_utf8_strdown(nome, -1);
        char *desc_lower = g_utf8_strdown(descricao, -1);
        char *busca_lower = g_utf8_strdown(texto_busca, -1);
        
        visivel = (strstr(nome_lower, busca_lower) != NULL ||
                  strstr(desc_lower, busca_lower) != NULL);
        
        g_free(nome_lower);
        g_free(desc_lower);
        g_free(busca_lower);
    }
    
    // Filtrar por preço
    if (visivel && preco > valor_max) {
        visivel = FALSE;
    }
    
    g_free(nome);
    g_free(descricao);
    
    return visivel;
}

// Callbacks
void on_search_changed(GtkSearchEntry *entry, gpointer user_data) {
    gtk_tree_model_filter_refilter(produtos_gui.filter);
}

void on_price_filter_changed(GtkRange *range, gpointer user_data) {
    double valor = gtk_range_get_value(range);
    char texto[50];
    snprintf(texto, sizeof(texto), "Preço máximo: %.2f Kz", valor);
    gtk_label_set_text(GTK_LABEL(produtos_gui.price_label), texto);
    
    gtk_tree_model_filter_refilter(produtos_gui.filter);
}

void on_adicionar_produto(GtkButton *button, gpointer user_data) {
    CustomDialog *dialog = custom_dialog_new(GTK_WINDOW(main_gui.window), "Novo Produto");
    
    custom_dialog_add_field(dialog, "Nome:", "Nome do produto", NULL);
    custom_dialog_add_field(dialog, "Descrição:", "Descrição do produto", NULL);
    custom_dialog_add_numeric_field(dialog, "Preço:", 0, 1000000, 100, 0);
    
    custom_dialog_add_action_button(dialog, "Cancelar", "window-close-symbolic",
                                  G_CALLBACK(custom_dialog_destroy), dialog);
    
    GtkWidget *btn_salvar = custom_button_new_with_icon("Salvar",
                                                       "document-save-symbolic",
                                                       BUTTON_STYLE_SUCCESS);
    g_signal_connect(btn_salvar, "clicked", G_CALLBACK(custom_dialog_destroy), dialog);
    
    gtk_widget_show_all(dialog->dialog);
    
    int response = gtk_dialog_run(GTK_DIALOG(dialog->dialog));
    if (response == GTK_RESPONSE_ACCEPT) {
        Produto novo_produto;
        
        const char *nome = gtk_entry_get_text(
            GTK_ENTRY(custom_dialog_get_entry(dialog, 0))
        );
        const char *descricao = gtk_entry_get_text(
            GTK_ENTRY(custom_dialog_get_entry(dialog, 1))
        );
        const char *preco_str = gtk_entry_get_text(
            GTK_ENTRY(custom_dialog_get_entry(dialog, 2))
        );
        
        strncpy(novo_produto.nome, nome, sizeof(novo_produto.nome) - 1);
        strncpy(novo_produto.descricao, descricao, sizeof(novo_produto.descricao) - 1);
        novo_produto.preco = atof(preco_str);
        
        if (cadastrarProduto(novo_produto) == 0) {
            mostrar_sucesso("Produto cadastrado com sucesso!");
            atualizar_lista_produtos();
        } else {
            mostrar_erro("Erro ao cadastrar produto!");
        }
    }
    
    custom_dialog_destroy(dialog);
}

void on_editar_produto(GtkButton *button, gpointer user_data) {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(produtos_gui.list_view));
    GtkTreeModel *model;
    GtkTreeIter iter;
    
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        int id;
        gtk_tree_model_get(model, &iter, COL_ID, &id, -1);
        
        // Encontrar produto pelo ID
        Produto *produto = NULL;
        for (int i = 0; i < numProdutos; i++) {
            if (produtos[i].id == id) {
                produto = &produtos[i];
                break;
            }
        }
        
        if (produto != NULL) {
            CustomDialog *dialog = custom_dialog_new(GTK_WINDOW(main_gui.window), "Editar Produto");
            
            custom_dialog_add_field(dialog, "Nome:", "Nome do produto", produto->nome);
            custom_dialog_add_field(dialog, "Descrição:", "Descrição do produto", produto->descricao);
            
            char preco_str[32];
            snprintf(preco_str, sizeof(preco_str), "%.2f", produto->preco);
            custom_dialog_add_numeric_field(dialog, "Preço:", 0, 1000000, 100, produto->preco);
            
            custom_dialog_add_action_button(dialog, "Cancelar", "window-close-symbolic",
                                          G_CALLBACK(custom_dialog_destroy), dialog);
            
            GtkWidget *btn_salvar = custom_button_new_with_icon("Salvar",
                                                               "document-save-symbolic",
                                                               BUTTON_STYLE_SUCCESS);
            g_signal_connect(btn_salvar, "clicked", G_CALLBACK(custom_dialog_destroy), dialog);
            
            gtk_widget_show_all(dialog->dialog);
            
            int response = gtk_dialog_run(GTK_DIALOG(dialog->dialog));
            if (response == GTK_RESPONSE_ACCEPT) {
                Produto produto_editado = *produto;
                
                const char *nome = gtk_entry_get_text(
                    GTK_ENTRY(custom_dialog_get_entry(dialog, 0))
                );
                const char *descricao = gtk_entry_get_text(
                    GTK_ENTRY(custom_dialog_get_entry(dialog, 1))
                );
                const char *preco_str = gtk_entry_get_text(
                    GTK_ENTRY(custom_dialog_get_entry(dialog, 2))
                );
                
                strncpy(produto_editado.nome, nome, sizeof(produto_editado.nome) - 1);
                strncpy(produto_editado.descricao, descricao, sizeof(produto_editado.descricao) - 1);
                produto_editado.preco = atof(preco_str);
                
                if (editarProduto(id, produto_editado) == 0) {
                    mostrar_sucesso("Produto atualizado com sucesso!");
                    atualizar_lista_produtos();
                } else {
                    mostrar_erro("Erro ao atualizar produto!");
                }
            }
            
            custom_dialog_destroy(dialog);
        }
    }
}

void on_excluir_produto(GtkButton *button, gpointer user_data) {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(produtos_gui.list_view));
    GtkTreeModel *model;
    GtkTreeIter iter;
    
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        int id;
        gtk_tree_model_get(model, &iter, COL_ID, &id, -1);
        
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(main_gui.window),
                                                 GTK_DIALOG_MODAL,
                                                 GTK_MESSAGE_QUESTION,
                                                 GTK_BUTTONS_YES_NO,
                                                 "Deseja realmente excluir este produto?");
        
        int response = gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        
        if (response == GTK_RESPONSE_YES) {
            if (excluirProduto(id) == 0) {
                mostrar_sucesso("Produto excluído com sucesso!");
                atualizar_lista_produtos();
            } else {
                mostrar_erro("Erro ao excluir produto!");
            }
        }
    }
}

// Função para atualizar a lista de produtos
void atualizar_lista_produtos() {
    gtk_list_store_clear(produtos_gui.store);
    
    for (int i = 0; i < numProdutos; i++) {
        GtkTreeIter iter;
        float preco_iva = produtos[i].preco + (produtos[i].preco * TAXA_IVA / 100.0f);
        
        gtk_list_store_append(produtos_gui.store, &iter);
        gtk_list_store_set(produtos_gui.store, &iter,
                          COL_ID, produtos[i].id,
                          COL_NOME, produtos[i].nome,
                          COL_DESCRICAO, produtos[i].descricao,
                          COL_PRECO, produtos[i].preco,
                          COL_PRECO_IVA, preco_iva,
                          -1);
    }
}

// Função para criar a tela de produtos
GtkWidget* criar_tela_produtos() {
    // Container principal
    produtos_gui.container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_container_set_border_width(GTK_CONTAINER(produtos_gui.container), 16);
    
    // Barra de ferramentas
    GtkWidget *toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    
    // Botões de ação
    GtkWidget *btn_adicionar = custom_button_new_with_icon("Adicionar", 
                                                          "list-add-symbolic",
                                                          BUTTON_STYLE_SUCCESS);
    GtkWidget *btn_editar = custom_button_new_with_icon("Editar",
                                                       "document-edit-symbolic",
                                                       BUTTON_STYLE_PRIMARY);
    GtkWidget *btn_excluir = custom_button_new_with_icon("Excluir",
                                                        "user-trash-symbolic",
                                                        BUTTON_STYLE_DANGER);
    
    g_signal_connect(btn_adicionar, "clicked", G_CALLBACK(on_adicionar_produto), NULL);
    g_signal_connect(btn_editar, "clicked", G_CALLBACK(on_editar_produto), NULL);
    g_signal_connect(btn_excluir, "clicked", G_CALLBACK(on_excluir_produto), NULL);
    
    gtk_box_pack_start(GTK_BOX(toolbar), btn_adicionar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(toolbar), btn_editar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(toolbar), btn_excluir, FALSE, FALSE, 0);
    
    // Campo de busca
    produtos_gui.search_entry = gtk_search_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(produtos_gui.search_entry),
                                 "Pesquisar produtos...");
    g_signal_connect(produtos_gui.search_entry, "search-changed",
                    G_CALLBACK(on_search_changed), NULL);
    
    // Slider de preço
    produtos_gui.filter_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    produtos_gui.price_slider = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL,
                                                        0, 1000000, 100);
    produtos_gui.price_label = gtk_label_new("Preço máximo: 1000000.00 Kz");
    
    gtk_box_pack_start(GTK_BOX(produtos_gui.filter_box),
                      produtos_gui.price_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(produtos_gui.filter_box),
                      produtos_gui.price_slider, TRUE, TRUE, 0);
    
    g_signal_connect(produtos_gui.price_slider, "value-changed",
                    G_CALLBACK(on_price_filter_changed), NULL);
    
    // Adicionar widgets à toolbar
    gtk_box_pack_end(GTK_BOX(toolbar), produtos_gui.search_entry, TRUE, TRUE, 0);
    
    // Criar modelo de dados
    produtos_gui.store = gtk_list_store_new(NUM_COLS,
                                           G_TYPE_INT,    // ID
                                           G_TYPE_STRING, // Nome
                                           G_TYPE_STRING, // Descrição
                                           G_TYPE_FLOAT,  // Preço
                                           G_TYPE_FLOAT); // Preço com IVA
    
    // Criar filtro
    produtos_gui.filter = GTK_TREE_MODEL_FILTER(
        gtk_tree_model_filter_new(GTK_TREE_MODEL(produtos_gui.store), NULL)
    );
    gtk_tree_model_filter_set_visible_func(produtos_gui.filter,
                                         filtrar_produtos, NULL, NULL);
    
    // Criar ordenação
    produtos_gui.sort = GTK_TREE_MODEL_SORT(
        gtk_tree_model_sort_new_with_model(GTK_TREE_MODEL(produtos_gui.filter))
    );
    
    // Criar views
    produtos_gui.list_view = gtk_tree_view_new_with_model(
        GTK_TREE_MODEL(produtos_gui.sort)
    );
    configurar_colunas_lista(GTK_TREE_VIEW(produtos_gui.list_view));
    
    produtos_gui.grid_view = gtk_flow_box_new();
    gtk_flow_box_set_selection_mode(GTK_FLOW_BOX(produtos_gui.grid_view),
                                  GTK_SELECTION_SINGLE);
    gtk_flow_box_set_max_children_per_line(GTK_FLOW_BOX(produtos_gui.grid_view), 4);
    gtk_flow_box_set_homogeneous(GTK_FLOW_BOX(produtos_gui.grid_view), TRUE);
    
    // Criar scrolled windows
    GtkWidget *list_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(list_scroll), produtos_gui.list_view);
    
    GtkWidget *grid_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(grid_scroll), produtos_gui.grid_view);
    
    // Adicionar widgets ao container principal
    gtk_box_pack_start(GTK_BOX(produtos_gui.container), toolbar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(produtos_gui.container),
                      produtos_gui.filter_box, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(produtos_gui.container), list_scroll, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(produtos_gui.container), grid_scroll, TRUE, TRUE, 0);
    
    // Mostrar view de lista por padrão
    gtk_widget_show(produtos_gui.list_view);
    gtk_widget_hide(produtos_gui.grid_view);
    
    // Carregar dados iniciais
    atualizar_lista_produtos();
    
    return produtos_gui.container;
}
