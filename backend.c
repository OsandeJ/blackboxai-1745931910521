#include "backend.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h> // Para mkdir e stat

Produto produtos[MAX_PRODUTOS];
Cliente clientes[MAX_CLIENTES];
Pedido pedidos[MAX_PEDIDOS];
int numProdutos = 0;
int numClientes = 0;
int numPedidos = 0;

// Funções para carregar e salvar dados

void carregarProdutos() {
    FILE *arquivo = fopen("produtos.dat", "rb");
    if (!arquivo) {
        numProdutos = 0;
        return;
    }

    if (fread(&numProdutos, sizeof(int), 1, arquivo) != 1) {
        numProdutos = 0;
    } else {
        fread(produtos, sizeof(Produto), numProdutos, arquivo);
    }
    fclose(arquivo);
}

void salvarProdutos() {
    FILE *arquivo = fopen("produtos.dat", "wb");
    if (!arquivo) {
        return;
    }
    fwrite(&numProdutos, sizeof(int), 1, arquivo);
    fwrite(produtos, sizeof(Produto), numProdutos, arquivo);
    fclose(arquivo);
}

void carregarClientes() {
    FILE *arquivo = fopen("clientes.dat", "rb");
    if (!arquivo) {
        numClientes = 0;
        return;
    }

    if (fread(&numClientes, sizeof(int), 1, arquivo) != 1) {
        numClientes = 0;
    } else {
        fread(clientes, sizeof(Cliente), numClientes, arquivo);
    }
    fclose(arquivo);
}

void salvarClientes() {
    FILE *arquivo = fopen("clientes.dat", "wb");
    if (!arquivo) {
        return;
    }
    fwrite(&numClientes, sizeof(int), 1, arquivo);
    fwrite(clientes, sizeof(Cliente), numClientes, arquivo);
    fclose(arquivo);
}

void carregarPedidos() {
    FILE *arquivo = fopen("pedidos.dat", "rb");
    if (!arquivo) {
        numPedidos = 0;
        return;
    }

    if (fread(&numPedidos, sizeof(int), 1, arquivo) != 1) {
        numPedidos = 0;
    } else {
        fread(pedidos, sizeof(Pedido), numPedidos, arquivo);
    }
    fclose(arquivo);
}

void salvarPedidos() {
    FILE *arquivo = fopen("pedidos.dat", "wb");
    if (!arquivo) {
        return;
    }
    fwrite(&numPedidos, sizeof(int), 1, arquivo);
    fwrite(pedidos, sizeof(Pedido), numPedidos, arquivo);
    fclose(arquivo);
}

// Funções para manipulação de produtos

int cadastrarProduto(Produto p) {
    if (numProdutos >= MAX_PRODUTOS) {
        return -1; // Limite atingido
    }
    p.id = numProdutos + 1;
    produtos[numProdutos++] = p;
    salvarProdutos();
    return 0; // Sucesso
}

int editarProduto(int id, Produto p) {
    for (int i = 0; i < numProdutos; i++) {
        if (produtos[i].id == id) {
            p.id = id; // manter o id original
            produtos[i] = p;
            salvarProdutos();
            return 0; // Sucesso
        }
    }
    return -1; // Produto não encontrado
}

int excluirProduto(int id) {
    int encontrado = 0;
    for (int i = 0; i < numProdutos; i++) {
        if (produtos[i].id == id) {
            encontrado = 1;
            for (int j = i; j < numProdutos - 1; j++) {
                produtos[j] = produtos[j + 1];
            }
            numProdutos--;
            salvarProdutos();
            break;
        }
    }
    return encontrado ? 0 : -1;
}

int obterMaiorPrecoProduto(float *precoMaximo) {
    if (numProdutos == 0) {
        *precoMaximo = 0.0f;
        return -1;
    }
    float max = produtos[0].preco;
    for (int i = 1; i < numProdutos; i++) {
        if (produtos[i].preco > max) {
            max = produtos[i].preco;
        }
    }
    *precoMaximo = max;
    return 0;
}

// Funções para manipulação de clientes

int cadastrarCliente(Cliente c) {
    if (numClientes >= MAX_CLIENTES) {
        return -1;
    }
    c.id = numClientes + 1;
    clientes[numClientes++] = c;
    salvarClientes();
    return 0;
}

int editarCliente(int id, Cliente c) {
    for (int i = 0; i < numClientes; i++) {
        if (clientes[i].id == id) {
            c.id = id;
            clientes[i] = c;
            salvarClientes();
            return 0;
        }
    }
    return -1;
}

int excluirCliente(int id) {
    int encontrado = 0;
    for (int i = 0; i < numClientes; i++) {
        if (clientes[i].id == id) {
            encontrado = 1;
            for (int j = i; j < numClientes - 1; j++) {
                clientes[j] = clientes[j + 1];
            }
            numClientes--;
            salvarClientes();
            break;
        }
    }
    return encontrado ? 0 : -1;
}

// Funções para manipulação de pedidos

int fazerPedido(Pedido p) {
    if (numPedidos >= MAX_PEDIDOS) {
        return -1;
    }
    p.id = numPedidos + 1;
    pedidos[numPedidos++] = p;
    salvarPedidos();
    return 0;
}

void salvarPedidoEmArquivo(Pedido p) {
    // Obter data e hora atuais
    time_t agora = time(NULL);
    struct tm *tempo = localtime(&agora);

    // Criar nome da pasta com a data (ex.: "2025-01-08")
    char nomePasta[20];
    strftime(nomePasta, sizeof(nomePasta), "%Y-%m-%d", tempo);

    // Verificar se a pasta já existe antes de criá-la
    struct stat st = {0};
    if (stat(nomePasta, &st) == -1) {
#ifdef _WIN32
        _mkdir(nomePasta);
#else
        mkdir(nomePasta, 0700);
#endif
    }

    // Criar nome do arquivo com data e hora (ex.: "2025-01-08_14-30-45.txt")
    char nomeArquivo[50];
    strftime(nomeArquivo, sizeof(nomeArquivo), "%Y-%m-%d_%H-%M-%S.txt", tempo);

    // Montar o caminho completo (ex.: "2025-01-08/2025-01-08_14-30-45.txt")
    char caminhoCompleto[100];
    snprintf(caminhoCompleto, sizeof(caminhoCompleto), "%s/%s", nomePasta, nomeArquivo);

    // Abrir o arquivo para escrita
    FILE *arquivo = fopen(caminhoCompleto, "w");
    if (!arquivo) {
        return;
    }

    // Escrever o cabeçalho do pedido no arquivo
    fprintf(arquivo, "--- Resumo do Pedido ---\n");
    fprintf(arquivo, "Pedido ID: %d\n", p.id);
    fprintf(arquivo, "Cliente: %s\n", clientes[p.idCliente - 1].nome);
    fprintf(arquivo, "Telefone: %s\n\n", clientes[p.idCliente - 1].telefone);

    // Escrever os produtos em formato de tabela
    fprintf(arquivo, "%-15s %-12s %-15s %-15s\n", "Produto", "Quantidade", "Preço Unitário", "Total");
    fprintf(arquivo, "----------------------------------------------------------\n");

    for (int i = 0; i < p.numProdutos; i++) {
        Produto prod = produtos[p.idProduto[i] - 1];
        fprintf(arquivo, "%-15s %-12d %-15.2f %-15.2f\n",
                prod.nome,
                p.quantidade[i],
                prod.preco,
                prod.preco * p.quantidade[i]);
    }

    fprintf(arquivo, "----------------------------------------------------------\n");
    fprintf(arquivo, "TOTAL: %.2f Kz\n", p.total);
    fprintf(arquivo, "Método de Pagamento: %s\n", p.metodoPagamento);
    fprintf(arquivo, "Valor Pago: %.2f Kz\n", p.valorPago);
    fprintf(arquivo, "Troco: %.2f Kz\n", p.troco);

    fclose(arquivo);
}

// Corrigir IDs (exemplo para clientes)
void corrigirIDs() {
    for (int i = 0; i < numClientes; i++) {
        clientes[i].id = i + 1;
    }
    salvarClientes();
}
