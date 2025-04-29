#ifndef BACKEND_H
#define BACKEND_H

#define MAX_PRODUTOS 100
#define MAX_CLIENTES 100
#define MAX_PEDIDOS 100

#define TAXA_IVA 14.0f  // IVA em porcentagem

typedef struct {
    int id;
    char nome[50];
    char descricao[100];
    float preco;
} Produto;

typedef struct {
    int id;
    char nome[50];
    char telefone[15];
} Cliente;

typedef struct {
    int id;
    int idCliente;
    int idProduto[MAX_PRODUTOS];
    int quantidade[MAX_PRODUTOS];
    int numProdutos;
    float total;
    float valorPago;
    float troco;
    char metodoPagamento[20];
} Pedido;

// Variáveis globais para armazenar os dados em memória
extern Produto produtos[MAX_PRODUTOS];
extern Cliente clientes[MAX_CLIENTES];
extern Pedido pedidos[MAX_PEDIDOS];
extern int numProdutos;
extern int numClientes;
extern int numPedidos;

// Funções para carregar e salvar dados
void carregarProdutos();
void salvarProdutos();
void carregarClientes();
void salvarClientes();
void carregarPedidos();
void salvarPedidos();

// Funções para manipulação de produtos
int cadastrarProduto(Produto p);
int editarProduto(int id, Produto p);
int excluirProduto(int id);
int obterMaiorPrecoProduto(float *precoMaximo);

// Funções para manipulação de clientes
int cadastrarCliente(Cliente c);
int editarCliente(int id, Cliente c);
int excluirCliente(int id);

// Funções para manipulação de pedidos
int fazerPedido(Pedido p);
void salvarPedidoEmArquivo(Pedido p);

// Funções auxiliares
void corrigirIDs();

#endif // BACKEND_H
