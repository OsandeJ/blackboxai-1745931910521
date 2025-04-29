#include <gtk/gtk.h>
#include "produtos_gui.h"
#include "backend.h"

// Variável global para armazenar os widgets da interface de produtos
static ProdutosGui produtos_gui;

// Enumeração para as colunas da TreeView
enum {
    COL_ID,
    COL_NOME,
    COL_DESCRICAO,
    COL_PRECO,
    COL_PRECO_IVA,
    NUM_COLS
};

// Função para calcular o preço com IVA
static float calcular_preco_com_iva(float preco) {
    return preco + (preco * TAXA_IVA / 100.0f);
}

// Função para criar o diálogo de produto (adicionar/editar)
static GtkWidget* criar_dialogo_produto(GtkWindow *parent, const char *titulo, Produto *produto) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        titulo,
        parent,
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "Cancelar", GTK_RESPONSE_CANCEL,
        "Salvar", GTK_RESPONSE_ACCEPT,
        NULL
    );

    // Criar grid para os campos
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_container_set_border_width(GTK_CONTAINER(grid), 10);

    // Labels
    GtkWidget *label_nome = gtk_label_new("Nome:");
    GtkWidget *label_descricao = gtk_label_new("Descrição:");
    GtkWidget *label_preco = gtk_label_new("Preço (Kz):");

    // Campos de entrada
    GtkWidget *entry_nome = gtk_entry_new();
    GtkWidget *entry_descricao = gtk_entry_new();
    GtkWidget *entry_preco = gtk_entry_new();

    // Configurar campos
    gtk_entry_set_max_length(GTK_ENTRY(entry_nome), 49);
    gtk_entry_set_max_length(GTK_ENTRY(entry_descricao), 99);

    // Se for edição, preencher campos com dados existentes
    if (produto != NULL) {
        gtk_entry_set_text(GTK_ENTRY(entry_nome), produto->nome);
        gtk_entry_set_text(GTK_ENTRY(entry_descricao), produto->descricao);
        char preco_str[32];
        snprintf(preco_str, sizeof(preco_str), "%.2f", produto->preco);
        gtk_entry_set_text(GTK_ENTRY(entry_preco), preco_str);
    }

    // Adicionar widgets ao grid
    gtk_grid_attach(GTK_GRID(grid), label_nome, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_nome, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), label_descricao, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_descricao, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), label_preco, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_preco, 1, 2, 1, 1);

    // Adicionar grid ao diálogo
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_container_add(GTK_CONTAINER(content_area), grid);

    // Armazenar referências aos campos para acessar depois
    g_object_set_data(G_OBJECT(dialog), "entry_nome", entry_nome);
    g_object_set_data(G_OBJECT(dialog), "entry_descricao", entry_descricao);
    g_object_set_data(G_OBJECT(dialog), "entry_preco", entry_preco);

    gtk_widget_show_all(dialog);
    return dialog;
}

// Callback para o botão Adicionar Produto
void on_adicionar_produto_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(button));
    GtkWidget *dialog = criar_dialogo_produto(GTK_WINDOW(window), "Adicionar Produto", NULL);

    int response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_ACCEPT) {
        // Obter dados dos campos
        GtkWidget *entry_nome = g_object_get_data(G_OBJECT(dialog), "entry_nome");
        GtkWidget *entry_descricao = g_object_get_data(G_OBJECT(dialog), "entry_descricao");
        GtkWidget *entry_preco = g_object_get_data(G_OBJECT(dialog), "entry_preco");

        Produto novo_produto;
        strncpy(novo_produto.nome, gtk_entry_get_text(GTK_ENTRY(entry_nome)), sizeof(novo_produto.nome) - 1);
        strncpy(novo_produto.descricao, gtk_entry_get_text(GTK_ENTRY(entry_descricao)), sizeof(novo_produto.descricao) - 1);
        novo_produto.preco = atof(gtk_entry_get_text(GTK_ENTRY(entry_preco)));

        // Cadastrar produto
        if (cadastrarProduto(novo_produto) == 0) {
            // Atualizar interface
            atualizar_lista_produtos();
            
            // Mostrar mensagem de sucesso
            GtkWidget *msg_dialog = gtk_message_dialog_new(
                GTK_WINDOW(window),
                GTK_DIALOG_MODAL,
                GTK_MESSAGE_INFO,
                GTK_BUTTONS_OK,
                "Produto cadastrado com sucesso!"
            );
            gtk_dialog_run(GTK_DIALOG(msg_dialog));
            gtk_widget_destroy(msg_dialog);
        }
    }
    gtk_widget_destroy(dialog);
}

// Callback para o botão Editar Produto
void on_editar_produto_clicked(GtkButton *button, gpointer user_data) {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(produtos_gui.treeview));
    GtkTreeModel *model;
    GtkTreeIter iter;

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        int id;
        gtk_tree_model_get(model, &iter, COL_ID, &id, -1);

        // Encontrar produto pelo ID
        Produto produto_atual;
        int encontrado = 0;
        for (int i = 0; i < numProdutos; i++) {
            if (produtos[i].id == id) {
                produto_atual = produtos[i];
                encontrado = 1;
                break;
            }
        }

        if (encontrado) {
            GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(button));
            GtkWidget *dialog = criar_dialogo_produto(GTK_WINDOW(window), "Editar Produto", &produto_atual);

            int response = gtk_dialog_run(GTK_DIALOG(dialog));
            if (response == GTK_RESPONSE_ACCEPT) {
                // Obter dados dos campos
                GtkWidget *entry_nome = g_object_get_data(G_OBJECT(dialog), "entry_nome");
                GtkWidget *entry_descricao = g_object_get_data(G_OBJECT(dialog), "entry_descricao");
                GtkWidget *entry_preco = g_object_get_data(G_OBJECT(dialog), "entry_preco");

                Produto produto_editado = produto_atual;
                strncpy(produto_editado.nome, gtk_entry_get_text(GTK_ENTRY(entry_nome)), sizeof(produto_editado.nome) - 1);
                strncpy(produto_editado.descricao, gtk_entry_get_text(GTK_ENTRY(entry_descricao)), sizeof(produto_editado.descricao) - 1);
                produto_editado.preco = atof(gtk_entry_get_text(GTK_ENTRY(entry_preco)));

                // Editar produto
                if (editarProduto(id, produto_editado) == 0) {
                    // Atualizar interface
                    atualizar_lista_produtos();
                    
                    // Mostrar mensagem de sucesso
                    GtkWidget *msg_dialog = gtk_message_dialog_new(
                        GTK_WINDOW(window),
                        GTK_DIALOG_MODAL,
                        GTK_MESSAGE_INFO,
                        GTK_BUTTONS_OK,
                        "Produto atualizado com sucesso!"
                    );
                    gtk_dialog_run(GTK_DIALOG(msg_dialog));
                    gtk_widget_destroy(msg_dialog);
                }
            }
            gtk_widget_destroy(dialog);
        }
    }
}

// Callback para o botão Excluir Produto
void on_excluir_produto_clicked(GtkButton *button, gpointer user_data) {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(produtos_gui.treeview));
    GtkTreeModel *model;
    GtkTreeIter iter;

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        int id;
        gtk_tree_model_get(model, &iter, COL_ID, &id, -1);

        GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(button));
        GtkWidget *dialog = gtk_message_dialog_new(
            GTK_WINDOW(window),
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_QUESTION,
            GTK_BUTTONS_YES_NO,
            "Deseja realmente excluir este produto?"
        );

        int response = gtk_dialog_run(GTK_DIALOG(dialog));
        if (response == GTK_RESPONSE_YES) {
            if (excluirProduto(id) == 0) {
                atualizar_lista_produtos();
                
                // Mostrar mensagem de sucesso
                GtkWidget *msg_dialog = gtk_message_dialog_new(
                    GTK_WINDOW(window),
                    GTK_DIALOG_MODAL,
                    GTK_MESSAGE_INFO,
                    GTK_BUTTONS_OK,
                    "Produto excluído com sucesso!"
                );
                gtk_dialog_run(GTK_DIALOG(msg_dialog));
                gtk_widget_destroy(msg_dialog);
            }
        }
        gtk_widget_destroy(dialog);
    }
}

// Função para filtrar produtos pelo preço
static gboolean filtrar_por_preco(GtkTreeModel *model, GtkTreeIter *iter, gpointer data) {
    double preco;
    double valor_slider = gtk_range_get_value(GTK_RANGE(produtos_gui.slider_preco));
    
    gtk_tree_model_get(model, iter, COL_PRECO, &preco, -1);
    
    return preco <= valor_slider;
}

// Callback para mudança no slider de preço
void on_slider_preco_changed(GtkRange *range, gpointer user_data) {
    double valor = gtk_range_get_value(range);
    char texto[50];
    snprintf(texto, sizeof(texto), "%.2f Kz", valor);
    gtk_label_set_text(GTK_LABEL(produtos_gui.label_preco_max), texto);
    
    // Atualizar filtro
    gtk_tree_model_filter_refilter(produtos_gui.filter);
}

// Função para atualizar a lista de produtos na interface
void atualizar_lista_produtos() {
    // Limpar lista atual
    gtk_list_store_clear(produtos_gui.liststore);

    // Adicionar produtos à lista
    for (int i = 0; i < numProdutos; i++) {
        GtkTreeIter iter;
        gtk_list_store_append(produtos_gui.liststore, &iter);
        float preco_iva = calcular_preco_com_iva(produtos[i].preco);
        
        gtk_list_store_set(produtos_gui.liststore, &iter,
            COL_ID, produtos[i].id,
            COL_NOME, produtos[i].nome,
            COL_DESCRICAO, produtos[i].descricao,
            COL_PRECO, produtos[i].preco,
            COL_PRECO_IVA, preco_iva,
            -1);
    }

    // Atualizar valor máximo do slider
    float preco_maximo;
    if (obterMaiorPrecoProduto(&preco_maximo) == 0) {
        gtk_range_set_range(GTK_RANGE(produtos_gui.slider_preco), 0.0, preco_maximo);
    }
}

// Função para criar a tela de produtos
GtkWidget* criar_tela_produtos() {
    // Container principal
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);

    // Título
    GtkWidget *label_titulo = gtk_label_new(NULL);
    char *markup = g_markup_printf_escaped("<span size='x-large' weight='bold'>Gestão de Produtos</span>");
    gtk_label_set_markup(GTK_LABEL(label_titulo), markup);
    g_free(markup);
    gtk_box_pack_start(GTK_BOX(vbox), label_titulo, FALSE, FALSE, 5);

    // Criar barra de ferramentas
    GtkWidget *toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    
    // Botões
    GtkWidget *btn_adicionar = gtk_button_new_with_label("Adicionar");
    GtkWidget *btn_editar = gtk_button_new_with_label("Editar");
    GtkWidget *btn_excluir = gtk_button_new_with_label("Excluir");
    
    gtk_box_pack_start(GTK_BOX(toolbar), btn_adicionar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(toolbar), btn_editar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(toolbar), btn_excluir, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 5);

    // Frame para o filtro de preço
    GtkWidget *frame_filtro = gtk_frame_new("Filtrar por Preço");
    GtkWidget *box_filtro = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(box_filtro), 5);
    
    produtos_gui.label_preco_min = gtk_label_new("0.00 Kz");
    produtos_gui.slider_preco = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 1000000, 100);
    produtos_gui.label_preco_max = gtk_label_new("1000000.00 Kz");
    
    gtk_box_pack_start(GTK_BOX(box_filtro), produtos_gui.label_preco_min, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(box_filtro), produtos_gui.slider_preco, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(box_filtro), produtos_gui.label_preco_max, FALSE, FALSE, 5);
    
    gtk_container_add(GTK_CONTAINER(frame_filtro), box_filtro);
    gtk_box_pack_start(GTK_BOX(vbox), frame_filtro, FALSE, FALSE, 5);

    // Criar TreeView
    produtos_gui.liststore = gtk_list_store_new(NUM_COLS,
        G_TYPE_INT,     // ID
        G_TYPE_STRING,  // Nome
        G_TYPE_STRING,  // Descrição
        G_TYPE_FLOAT,   // Preço
        G_TYPE_FLOAT    // Preço com IVA
    );

    // Criar filtro
    produtos_gui.filter = GTK_TREE_MODEL_FILTER(
        gtk_tree_model_filter_new(GTK_TREE_MODEL(produtos_gui.liststore), NULL)
    );
    gtk_tree_model_filter_set_visible_func(produtos_gui.filter,
        filtrar_por_preco, NULL, NULL);

    produtos_gui.treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(produtos_gui.filter));

    // Criar colunas
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    // Coluna ID
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("ID", renderer, "text", COL_ID, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(produtos_gui.treeview), column);

    // Coluna Nome
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Nome", renderer, "text", COL_NOME, NULL);
    gtk_tree_view_column_set_resizable(column, TRUE);
    gtk_tree_view_append_column(GTK_TREE_VIEW(produtos_gui.treeview), column);

    // Coluna Descrição
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Descrição", renderer, "text", COL_DESCRICAO, NULL);
    gtk_tree_view_column_set_resizable(column, TRUE);
    gtk_tree_view_append_column(GTK_TREE_VIEW(produtos_gui.treeview), column);

    // Coluna Preço
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Preço (Kz)", renderer, "text", COL_PRECO, NULL);
    gtk_tree_view_column_set_cell_data_func(column, renderer,
        (GtkTreeCellDataFunc)gtk_cell_renderer_set_alignment, GINT_TO_POINTER(1), NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(produtos_gui.treeview), column);

    // Coluna Preço com IVA
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Preço com IVA (Kz)", renderer, "text", COL_PRECO_IVA, NULL);
    gtk_tree_view_column_set_cell_data_func(column, renderer,
        (GtkTreeCellDataFunc)gtk_cell_renderer_set_alignment, GINT_TO_POINTER(1), NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(produtos_gui.treeview), column);

    // Adicionar TreeView a um ScrolledWindow
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                 GTK_POLICY_AUTOMATIC,
                                 GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled_window), produtos_gui.treeview);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);

    // Conectar sinais
    g_signal_connect(btn_adicionar, "clicked", G_CALLBACK(on_adicionar_produto_clicked), NULL);
    g_signal_connect(btn_editar, "clicked", G_CALLBACK(on_editar_produto_clicked), NULL);
    g_signal_connect(btn_excluir, "clicked", G_CALLBACK(on_excluir_produto_clicked), NULL);
    g_signal_connect(produtos_gui.slider_preco, "value-changed", G_CALLBACK(on_slider_preco_changed), NULL);

    // Carregar dados iniciais
    atualizar_lista_produtos();

    gtk_widget_show_all(vbox);
    return vbox;
}
