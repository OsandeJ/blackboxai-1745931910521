#include <gtk/gtk.h>
#include <string.h>
#include "clientes_gui.h"
#include "backend.h"

// Variável global para armazenar os widgets da interface de clientes
static ClientesGui clientes_gui;

// Enumeração para as colunas da TreeView
enum {
    COL_ID,
    COL_NOME,
    COL_TELEFONE,
    NUM_COLS
};

// Função para criar o diálogo de cliente (adicionar/editar)
static GtkWidget* criar_dialogo_cliente(GtkWindow *parent, const char *titulo, Cliente *cliente) {
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
    GtkWidget *label_telefone = gtk_label_new("Telefone:");

    // Campos de entrada
    GtkWidget *entry_nome = gtk_entry_new();
    GtkWidget *entry_telefone = gtk_entry_new();

    // Configurar campos
    gtk_entry_set_max_length(GTK_ENTRY(entry_nome), 49);
    gtk_entry_set_max_length(GTK_ENTRY(entry_telefone), 14);

    // Se for edição, preencher campos com dados existentes
    if (cliente != NULL) {
        gtk_entry_set_text(GTK_ENTRY(entry_nome), cliente->nome);
        gtk_entry_set_text(GTK_ENTRY(entry_telefone), cliente->telefone);
    }

    // Adicionar widgets ao grid
    gtk_grid_attach(GTK_GRID(grid), label_nome, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_nome, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), label_telefone, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_telefone, 1, 1, 1, 1);

    // Adicionar grid ao diálogo
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_container_add(GTK_CONTAINER(content_area), grid);

    // Armazenar referências aos campos para acessar depois
    g_object_set_data(G_OBJECT(dialog), "entry_nome", entry_nome);
    g_object_set_data(G_OBJECT(dialog), "entry_telefone", entry_telefone);

    gtk_widget_show_all(dialog);
    return dialog;
}

// Callback para o botão Adicionar Cliente
void on_adicionar_cliente_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(button));
    GtkWidget *dialog = criar_dialogo_cliente(GTK_WINDOW(window), "Adicionar Cliente", NULL);

    int response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_ACCEPT) {
        // Obter dados dos campos
        GtkWidget *entry_nome = g_object_get_data(G_OBJECT(dialog), "entry_nome");
        GtkWidget *entry_telefone = g_object_get_data(G_OBJECT(dialog), "entry_telefone");

        Cliente novo_cliente;
        strncpy(novo_cliente.nome, gtk_entry_get_text(GTK_ENTRY(entry_nome)), sizeof(novo_cliente.nome) - 1);
        strncpy(novo_cliente.telefone, gtk_entry_get_text(GTK_ENTRY(entry_telefone)), sizeof(novo_cliente.telefone) - 1);

        // Cadastrar cliente
        if (cadastrarCliente(novo_cliente) == 0) {
            // Atualizar interface
            atualizar_lista_clientes();
            
            // Mostrar mensagem de sucesso
            GtkWidget *msg_dialog = gtk_message_dialog_new(
                GTK_WINDOW(window),
                GTK_DIALOG_MODAL,
                GTK_MESSAGE_INFO,
                GTK_BUTTONS_OK,
                "Cliente cadastrado com sucesso!"
            );
            gtk_dialog_run(GTK_DIALOG(msg_dialog));
            gtk_widget_destroy(msg_dialog);
        }
    }
    gtk_widget_destroy(dialog);
}

// Callback para o botão Editar Cliente
void on_editar_cliente_clicked(GtkButton *button, gpointer user_data) {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(clientes_gui.treeview));
    GtkTreeModel *model;
    GtkTreeIter iter;

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        int id;
        gtk_tree_model_get(model, &iter, COL_ID, &id, -1);

        // Encontrar cliente pelo ID
        Cliente cliente_atual;
        int encontrado = 0;
        for (int i = 0; i < numClientes; i++) {
            if (clientes[i].id == id) {
                cliente_atual = clientes[i];
                encontrado = 1;
                break;
            }
        }

        if (encontrado) {
            GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(button));
            GtkWidget *dialog = criar_dialogo_cliente(GTK_WINDOW(window), "Editar Cliente", &cliente_atual);

            int response = gtk_dialog_run(GTK_DIALOG(dialog));
            if (response == GTK_RESPONSE_ACCEPT) {
                // Obter dados dos campos
                GtkWidget *entry_nome = g_object_get_data(G_OBJECT(dialog), "entry_nome");
                GtkWidget *entry_telefone = g_object_get_data(G_OBJECT(dialog), "entry_telefone");

                Cliente cliente_editado = cliente_atual;
                strncpy(cliente_editado.nome, gtk_entry_get_text(GTK_ENTRY(entry_nome)), sizeof(cliente_editado.nome) - 1);
                strncpy(cliente_editado.telefone, gtk_entry_get_text(GTK_ENTRY(entry_telefone)), sizeof(cliente_editado.telefone) - 1);

                // Editar cliente
                if (editarCliente(id, cliente_editado) == 0) {
                    // Atualizar interface
                    atualizar_lista_clientes();
                    
                    // Mostrar mensagem de sucesso
                    GtkWidget *msg_dialog = gtk_message_dialog_new(
                        GTK_WINDOW(window),
                        GTK_DIALOG_MODAL,
                        GTK_MESSAGE_INFO,
                        GTK_BUTTONS_OK,
                        "Cliente atualizado com sucesso!"
                    );
                    gtk_dialog_run(GTK_DIALOG(msg_dialog));
                    gtk_widget_destroy(msg_dialog);
                }
            }
            gtk_widget_destroy(dialog);
        }
    }
}

// Callback para o botão Excluir Cliente
void on_excluir_cliente_clicked(GtkButton *button, gpointer user_data) {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(clientes_gui.treeview));
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
            "Deseja realmente excluir este cliente?"
        );

        int response = gtk_dialog_run(GTK_DIALOG(dialog));
        if (response == GTK_RESPONSE_YES) {
            if (excluirCliente(id) == 0) {
                atualizar_lista_clientes();
                
                // Mostrar mensagem de sucesso
                GtkWidget *msg_dialog = gtk_message_dialog_new(
                    GTK_WINDOW(window),
                    GTK_DIALOG_MODAL,
                    GTK_MESSAGE_INFO,
                    GTK_BUTTONS_OK,
                    "Cliente excluído com sucesso!"
                );
                gtk_dialog_run(GTK_DIALOG(msg_dialog));
                gtk_widget_destroy(msg_dialog);
            }
        }
        gtk_widget_destroy(dialog);
    }
}

// Função para filtrar clientes pelo nome
static gboolean filtrar_por_nome(GtkTreeModel *model, GtkTreeIter *iter, gpointer data) {
    gchar *nome;
    const gchar *texto_pesquisa = gtk_entry_get_text(GTK_ENTRY(clientes_gui.entry_pesquisa));
    
    if (strlen(texto_pesquisa) == 0) return TRUE; // Mostrar todos se não houver texto

    gtk_tree_model_get(model, iter, COL_NOME, &nome, -1);
    
    gboolean visivel = (nome && strcasestr(nome, texto_pesquisa) != NULL);
    g_free(nome);
    
    return visivel;
}

// Callback para mudança no campo de pesquisa
void on_pesquisar_cliente_changed(GtkEditable *editable, gpointer user_data) {
    gtk_tree_model_filter_refilter(clientes_gui.filter);
}

// Função para atualizar a lista de clientes na interface
void atualizar_lista_clientes() {
    // Limpar lista atual
    gtk_list_store_clear(clientes_gui.liststore);

    // Adicionar clientes à lista
    for (int i = 0; i < numClientes; i++) {
        GtkTreeIter iter;
        gtk_list_store_append(clientes_gui.liststore, &iter);
        
        gtk_list_store_set(clientes_gui.liststore, &iter,
            COL_ID, clientes[i].id,
            COL_NOME, clientes[i].nome,
            COL_TELEFONE, clientes[i].telefone,
            -1);
    }
}

// Função para criar a tela de clientes
GtkWidget* criar_tela_clientes() {
    // Container principal
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);

    // Título
    GtkWidget *label_titulo = gtk_label_new(NULL);
    char *markup = g_markup_printf_escaped("<span size='x-large' weight='bold'>Gestão de Clientes</span>");
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

    // Campo de pesquisa
    GtkWidget *box_pesquisa = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *label_pesquisa = gtk_label_new("Pesquisar:");
    clientes_gui.entry_pesquisa = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(box_pesquisa), label_pesquisa, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(box_pesquisa), clientes_gui.entry_pesquisa, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), box_pesquisa, FALSE, FALSE, 5);

    // Criar TreeView
    clientes_gui.liststore = gtk_list_store_new(NUM_COLS,
        G_TYPE_INT,     // ID
        G_TYPE_STRING,  // Nome
        G_TYPE_STRING   // Telefone
    );

    // Criar filtro
    clientes_gui.filter = GTK_TREE_MODEL_FILTER(
        gtk_tree_model_filter_new(GTK_TREE_MODEL(clientes_gui.liststore), NULL)
    );
    gtk_tree_model_filter_set_visible_func(clientes_gui.filter,
        filtrar_por_nome, NULL, NULL);

    clientes_gui.treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(clientes_gui.filter));

    // Criar colunas
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    // Coluna ID
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("ID", renderer, "text", COL_ID, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(clientes_gui.treeview), column);

    // Coluna Nome
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Nome", renderer, "text", COL_NOME, NULL);
    gtk_tree_view_column_set_resizable(column, TRUE);
    gtk_tree_view_append_column(GTK_TREE_VIEW(clientes_gui.treeview), column);

    // Coluna Telefone
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Telefone", renderer, "text", COL_TELEFONE, NULL);
    gtk_tree_view_column_set_resizable(column, TRUE);
    gtk_tree_view_append_column(GTK_TREE_VIEW(clientes_gui.treeview), column);

    // Adicionar TreeView a um ScrolledWindow
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                 GTK_POLICY_AUTOMATIC,
                                 GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled_window), clientes_gui.treeview);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);

    // Conectar sinais
    g_signal_connect(btn_adicionar, "clicked", G_CALLBACK(on_adicionar_cliente_clicked), NULL);
    g_signal_connect(btn_editar, "clicked", G_CALLBACK(on_editar_cliente_clicked), NULL);
    g_signal_connect(btn_excluir, "clicked", G_CALLBACK(on_excluir_cliente_clicked), NULL);
    g_signal_connect(clientes_gui.entry_pesquisa, "changed", G_CALLBACK(on_pesquisar_cliente_changed), NULL);

    // Carregar dados iniciais
    atualizar_lista_clientes();

    gtk_widget_show_all(vbox);
    return vbox;
}
