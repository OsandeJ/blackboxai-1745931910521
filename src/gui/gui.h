#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>
#include "widgets/custom_header.h"
#include "widgets/custom_button.h"
#include "widgets/custom_dialog.h"

// Tipos de notificação
typedef enum {
    NOTIFICATION_INFO,
    NOTIFICATION_SUCCESS,
    NOTIFICATION_WARNING,
    NOTIFICATION_ERROR
} NotificationType;

// Estrutura para armazenar widgets principais da interface
typedef struct {
    GtkWidget *window;           // Janela principal
    GtkWidget *main_box;         // Container principal
    GtkWidget *sidebar;          // Barra lateral
    GtkWidget *content_area;     // Área de conteúdo
    CustomHeader *header;        // Cabeçalho personalizado
    GtkWidget *stack;           // Stack para alternar entre telas
    GtkWidget *stack_sidebar;   // Sidebar para navegação do stack
    GtkWidget *theme_switch;    // Switch para alternar tema claro/escuro
    GtkWidget *notification_area; // Área para notificações
} MainGui;

// Funções principais
void iniciar_gui(int argc, char *argv[]);
void carregar_css();
void aplicar_tema(gboolean tema_escuro);

// Funções de navegação
void mostrar_tela_produtos();
void mostrar_tela_clientes();
void mostrar_tela_pedidos();

// Funções de notificação
void mostrar_notificacao(const char *mensagem, const char *tipo);
void mostrar_erro(const char *mensagem);
void mostrar_sucesso(const char *mensagem);
void mostrar_aviso(const char *mensagem);
void mostrar_info(const char *mensagem);

// Funções de diálogo
void mostrar_dialogo_confirmacao(const char *titulo, const char *mensagem, 
                               GCallback callback_sim, gpointer user_data);
void mostrar_dialogo_erro(const char *titulo, const char *mensagem);
void mostrar_dialogo_info(const char *titulo, const char *mensagem);

// Funções de validação
gboolean validar_numero(const char *texto);
gboolean validar_texto_vazio(const char *texto);
gboolean validar_preco(const char *texto);
gboolean validar_telefone(const char *texto);

// Funções de formatação
char* formatar_moeda(float valor);
char* formatar_telefone(const char *telefone);
char* formatar_data(const char *data);

// Variável global para acesso à interface
extern MainGui main_gui;

#endif // GUI_H
