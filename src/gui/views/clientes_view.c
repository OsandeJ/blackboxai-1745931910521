#include "clientes_view.h"
#include "../../backend/backend.h"
#include "../gui.h"

// Variável global para a view de clientes
static ClientesView clientes_view;

// Função para criar um card de cliente
GtkWidget* criar_card_cliente(const char *nome, const char *telefone, int id) {
    GtkWidget *card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_style_context_add_class(gtk_widget_get_style_context(card), "card");
    
    // Avatar do cliente
    GtkWidget *avatar = gtk_image_new_from_icon_name("avatar-default-symbolic",
                                                    GTK_ICON_SIZE_DIALOG);
    gtk_image_set_pixel_size(GTK_IMAGE(avatar), 64);
    
    // Nome do cliente
    GtkWidget *nome_label = gtk_label_new(NULL);
    char *markup = g_markup_printf_escaped("<span size='large' weight='bold'>%s</span>", nome);
    gtk_label_set_markup(GTK_LABEL(nome_label), markup);
    g_free(markup);
    
    // Telefone
    GtkWidget *telefone_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    GtkWidget *telefone_icon = gtk_image_new_from_icon_name("phone-symbolic",
                                                           GTK_ICON_SIZE_SMALL_TOOLBAR);
    GtkWidget *telefone_label = gtk_label_new(telefone);
    
    gtk_box_pack_start(GTK_BOX(telefone_box), telefone_icon, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(telefone_box), telefone_label, FALSE, FALSE, 0);
    
    // ID do cliente (oculto)
    g_object_set_data(G_OBJECT(card), "cliente-id", GINT_TO_POINTER(id));
    
    // Botões de ação
    GtkWidget *actions_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_widget_set_halign(actions_box, GTK_ALIGN_CENTER);
    
    GtkWidget *btn_editar = custom_button_new_with_icon(NULL, "document-edit-symbolic",
                                                       BUTTON_STYLE_PRIMARY);
    GtkWidget *btn_excluir = custom_button_new_with_icon(NULL, "user-trash-symbolic",
                                                        BUTTON_STYLE_DANGER);
    
    gtk_widget_set_tooltip_text(btn_editar, "Editar cliente");
    gtk_widget_set_tooltip_text(btn_excluir, "Excluir cliente");
    
    g_signal_connect(btn_editar, "clicked", G_CALLBACK(on_editar_cliente),
                    GINT_TO_POINTER(id));
    g_signal_connect(btn_excluir, "clicked", G_CALLBACK(on_excluir_cliente),
                    GINT_TO_POINTER(id));
    
    gtk_box_pack_start(GTK_BOX(actions_box), btn_editar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(actions_box), btn_excluir, FALSE, FALSE, 0);
    
    // Adicionar widgets ao card
    gtk_box_pack_start(GTK_BOX(card), avatar, FALSE, FALSE, 8);
    gtk_box_pack_start(GTK_BOX(card), nome_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(card), telefone_box, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(card), actions_box, FALSE, FALSE, 8);
    
    gtk_widget_set_margin_start(card, 8);
    gtk_widget_set_margin_end(card, 8);
    gtk_widget_set_margin_top(card, 8);
    gtk_widget_set_margin_bottom(card, 8);
    
    return card;
}

// Função de filtro para clientes
gboolean filtrar_clientes(GtkTreeModel *model, GtkTreeIter *iter, gpointer data) {
    char *nome, *telefone;
    const char *texto_busca = gtk_entry_get_text(GTK_ENTRY(clientes_view.search_entry));
    
    gtk_tree_model_get(model, iter,
                      COL_CLIENTE_NOME, &nome,
                      COL_CLIENTE_TELEFONE, &telefone,
                      -1);
    
    gboolean visivel = TRUE;
    
    if (texto_busca && *texto_busca != '\0') {
        char *nome_lower = g_utf8_strdown(nome, -1);
        char *telefone_lower = g_utf8_strdown(telefone, -1);
        char *busca_lower = g_utf8_strdown(texto_busca, -1);
        
        visivel = (strstr(nome_lower, busca_lower) != NULL ||
                  strstr(telefone_lower, busca_lower) != NULL);
        
        g_free(nome_lower);
        g_free(telefone_lower);
        g_free(busca_lower);
    }
    
    g_free(nome);
    g_free(telefone);
    
    return visivel;
}

// Callbacks
void on_search_cliente_changed(GtkSearchEntry *entry, gpointer user_data) {
    gtk_tree_model_filter_refilter(clientes_view.filter);
    atualizar_cards_clientes();
}

void on_sort_changed(GtkComboBox *combo_box, gpointer user_data) {
    int active = gtk_combo_box_get_active(combo_box);
    GtkTreeSortable *sortable = GTK_TREE_SORTABLE(clientes_view.sort);
    
    switch (active) {
        case SORT_NOME_ASC:
            gtk_tree_sortable_set_sort_column_id(sortable, COL_CLIENTE_NOME,
                                               GTK_SORT_ASCENDING);
            break;
        case SORT_NOME_DESC:
            gtk_tree_sortable_set_sort_column_id(sortable, COL_CLIENTE_NOME,
                                               GTK_SORT_DESCENDING);
            break;
        case SORT_ID_ASC:
            gtk_tree_sortable_set_sort_column_id(sortable, COL_CLIENTE_ID,
                                               GTK_SORT_ASCENDING);
            break;
        case SORT_ID_DESC:
            gtk_tree_sortable_set_sort_column_id(sortable, COL_CLIENTE_ID,
                                               GTK_SORT_DESCENDING);
            break;
    }
    
    atualizar_cards_clientes();
}

// Função para mostrar diálogo de cliente
void mostrar_dialogo_cliente(const char *titulo, Cliente *cliente, gboolean editando) {
    CustomDialog *dialog = custom_dialog_new(GTK_WINDOW(main_gui.window), titulo);
    
    // Campos do formulário
    custom_dialog_add_field(dialog, "Nome:", "Nome do cliente",
                          editando ? cliente->nome : NULL);
    custom_dialog_add_field(dialog, "Telefone:", "Telefone do cliente",
                          editando ? cliente->telefone : NULL);
    
    // Botões de ação
    custom_dialog_add_action_button(dialog, "Cancelar", "window-close-symbolic",
                                  G_CALLBACK(custom_dialog_destroy), dialog);
    
    GtkWidget *btn_salvar = custom_button_new_with_icon("Salvar",
                                                       "document-save-symbolic",
                                                       BUTTON_STYLE_SUCCESS);
    g_signal_connect(btn_salvar, "clicked", G_CALLBACK(custom_dialog_destroy), dialog);
    
    gtk_widget_show_all(dialog->dialog);
    
    int response = gtk_dialog_run(GTK_DIALOG(dialog->dialog));
    if (response == GTK_RESPONSE_ACCEPT) {
        Cliente novo_cliente;
        if (editando) {
            novo_cliente = *cliente;
        } else {
            novo_cliente.id = 0; // Será definido pela função de cadastro
        }
        
        // Obter dados dos campos
        const char *nome = gtk_entry_get_text(
            GTK_ENTRY(custom_dialog_get_entry(dialog, 0))
        );
        const char *telefone = gtk_entry_get_text(
            GTK_ENTRY(custom_dialog_get_entry(dialog, 1))
        );
        
        strncpy(novo_cliente.nome, nome, sizeof(novo_cliente.nome) - 1);
        strncpy(novo_cliente.telefone, telefone, sizeof(novo_cliente.telefone) - 1);
        
        if (editando) {
            if (editarCliente(cliente->id, novo_cliente) == 0) {
                mostrar_sucesso("Cliente atualizado com sucesso!");
                atualizar_lista_clientes();
            } else {
                mostrar_erro("Erro ao atualizar cliente!");
            }
        } else {
            if (cadastrarCliente(novo_cliente) == 0) {
                mostrar_sucesso("Cliente cadastrado com sucesso!");
                atualizar_lista_clientes();
            } else {
                mostrar_erro("Erro ao cadastrar cliente!");
            }
        }
    }
    
    custom_dialog_destroy(dialog);
}

// Callbacks de ações
void on_adicionar_cliente(GtkButton *button, gpointer user_data) {
    mostrar_dialogo_cliente("Novo Cliente", NULL, FALSE);
}

void on_editar_cliente(GtkButton *button, gpointer user_data) {
    int id = GPOINTER_TO_INT(user_data);
    
    // Encontrar cliente pelo ID
    Cliente *cliente = NULL;
    for (int i = 0; i < numClientes; i++) {
        if (clientes[i].id == id) {
            cliente = &clientes[i];
            break;
        }
    }
    
    if (cliente != NULL) {
        mostrar_dialogo_cliente("Editar Cliente", cliente, TRUE);
    }
}

void on_excluir_cliente(GtkButton *button, gpointer user_data) {
    int id = GPOINTER_TO_INT(user_data);
    
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(main_gui.window),
                                             GTK_DIALOG_MODAL,
                                             GTK_MESSAGE_QUESTION,
                                             GTK_BUTTONS_YES_NO,
                                             "Deseja realmente excluir este cliente?");
    
    int response = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    
    if (response == GTK_RESPONSE_YES) {
        if (excluirCliente(id) == 0) {
            mostrar_sucesso("Cliente excluído com sucesso!");
            atualizar_lista_clientes();
        } else {
            mostrar_erro("Erro ao excluir cliente!");
        }
    }
}

// Função para atualizar os cards de clientes
void atualizar_cards_clientes() {
    // Limpar container atual
    GList *children = gtk_container_get_children(GTK_CONTAINER(clientes_view.cards_container));
    GList *list_iter;
    for (list_iter = children; list_iter != NULL; list_iter = g_list_next(list_iter)) {
        gtk_widget_destroy(GTK_WIDGET(list_iter->data));
    }
    g_list_free(children);
    
    // Adicionar novos cards
    GtkTreeModel *model = GTK_TREE_MODEL(clientes_view.sort);
    GtkTreeIter tree_iter;
    
    if (gtk_tree_model_get_iter_first(model, &tree_iter)) {
        do {
            char *nome, *telefone;
            int id;
            
            gtk_tree_model_get(model, &tree_iter,
                             COL_CLIENTE_ID, &id,
                             COL_CLIENTE_NOME, &nome,
                             COL_CLIENTE_TELEFONE, &telefone,
                             -1);
            
            GtkWidget *card = criar_card_cliente(nome, telefone, id);
            gtk_container_add(GTK_CONTAINER(clientes_view.cards_container), card);
            
            g_free(nome);
            g_free(telefone);
        } while (gtk_tree_model_iter_next(model, &tree_iter));
    }
    
    gtk_widget_show_all(clientes_view.cards_container);
}

// Função para atualizar a lista de clientes
void atualizar_lista_clientes() {
    gtk_list_store_clear(clientes_view.store);
    
    for (int i = 0; i < numClientes; i++) {
        GtkTreeIter iter;
        gtk_list_store_append(clientes_view.store, &iter);
        gtk_list_store_set(clientes_view.store, &iter,
                          COL_CLIENTE_ID, clientes[i].id,
                          COL_CLIENTE_NOME, clientes[i].nome,
                          COL_CLIENTE_TELEFONE, clientes[i].telefone,
                          -1);
    }
    
    atualizar_cards_clientes();
}

// Função para criar a tela de clientes
GtkWidget* criar_tela_clientes() {
    // Container principal
    clientes_view.container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_container_set_border_width(GTK_CONTAINER(clientes_view.container), 16);
    
    // Barra de ferramentas
    GtkWidget *toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    
    // Botão adicionar
    GtkWidget *btn_adicionar = custom_button_new_with_icon("Novo Cliente",
                                                          "list-add-symbolic",
                                                          BUTTON_STYLE_SUCCESS);
    g_signal_connect(btn_adicionar, "clicked",
                    G_CALLBACK(on_adicionar_cliente), NULL);
    
    // Campo de busca
    clientes_view.search_entry = gtk_search_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(clientes_view.search_entry),
                                 "Pesquisar clientes...");
    g_signal_connect(clientes_view.search_entry, "search-changed",
                    G_CALLBACK(on_search_cliente_changed), NULL);
    
    // Combo de ordenação
    clientes_view.sort_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(clientes_view.sort_combo),
                                 "Nome (A-Z)");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(clientes_view.sort_combo),
                                 "Nome (Z-A)");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(clientes_view.sort_combo),
                                 "ID (Crescente)");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(clientes_view.sort_combo),
                                 "ID (Decrescente)");
    
    gtk_combo_box_set_active(GTK_COMBO_BOX(clientes_view.sort_combo), 0);
    g_signal_connect(clientes_view.sort_combo, "changed",
                    G_CALLBACK(on_sort_changed), NULL);
    
    // Adicionar widgets à toolbar
    gtk_box_pack_start(GTK_BOX(toolbar), btn_adicionar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(toolbar), clientes_view.search_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(toolbar), clientes_view.sort_combo, FALSE, FALSE, 0);
    
    // Criar modelo de dados
    clientes_view.store = gtk_list_store_new(NUM_COLS_CLIENTE,
                                           G_TYPE_INT,    // ID
                                           G_TYPE_STRING, // Nome
                                           G_TYPE_STRING);// Telefone
    
    // Criar filtro
    clientes_view.filter = GTK_TREE_MODEL_FILTER(
        gtk_tree_model_filter_new(GTK_TREE_MODEL(clientes_view.store), NULL)
    );
    gtk_tree_model_filter_set_visible_func(clientes_view.filter,
                                         filtrar_clientes, NULL, NULL);
    
    // Criar ordenação
    clientes_view.sort = GTK_TREE_MODEL_SORT(
        gtk_tree_model_sort_new_with_model(GTK_TREE_MODEL(clientes_view.filter))
    );
    
    // Container de cards
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                 GTK_POLICY_NEVER,
                                 GTK_POLICY_AUTOMATIC);
    
    clientes_view.cards_container = gtk_flow_box_new();
    gtk_flow_box_set_selection_mode(GTK_FLOW_BOX(clientes_view.cards_container),
                                  GTK_SELECTION_NONE);
    gtk_flow_box_set_max_children_per_line(GTK_FLOW_BOX(clientes_view.cards_container),
                                         4);
    gtk_flow_box_set_homogeneous(GTK_FLOW_BOX(clientes_view.cards_container), TRUE);
    
    gtk_container_add(GTK_CONTAINER(scroll), clientes_view.cards_container);
    
    // Adicionar widgets ao container principal
    gtk_box_pack_start(GTK_BOX(clientes_view.container), toolbar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(clientes_view.container), scroll, TRUE, TRUE, 0);
    
    // Carregar dados iniciais
    atualizar_lista_clientes();
    
    return clientes_view.container;
}
