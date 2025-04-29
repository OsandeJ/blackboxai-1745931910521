#include <stdio.h>
#include <string.h>
#include "backend.h"

// Variáveis globais
Produto produtos[MAX_PRODUTOS];
Cliente clientes[MAX_CLIENTES];
Pedido pedidos[MAX_PEDIDOS];
int numProdutos = 0;
int numClientes = 0;
int numPedidos = 0;

// Funções para carregar dados
void carregarProdutos() {
    FILE *arquivo = fopen("produtos.dat", "rb");
    if (arquivo != NULL) {
        numProdutos = fread(produtos, sizeof(Produto), MAX_PRODUTOS, arquivo);
        fclose(arquivo);
    }
}

void carregarClientes() {
    FILE *arquivo = fopen("clientes.dat", "rb");
    if (arquivo != NULL) {
        numClientes = fread(clientes, sizeof(Cliente), MAX_CLIENTES, arquivo);
        fclose(arquivo);
    }
}

void carregarPedidos() {
    FILE *arquivo = fopen("pedidos.dat", "rb");
    if (arquivo != NULL) {
        numPedidos = fread(pedidos, sizeof(Pedido), MAX_PEDIDOS, arquivo);
        fclose(arquivo);
    }
}

// Funções para salvar dados
void salvarProdutos() {
    FILE *arquivo = fopen("produtos.dat", "wb");
    if (arquivo != NULL) {
        fwrite(produtos, sizeof(Produto), numProdutos, arquivo);
        fclose(arquivo);
    }
}

void salvarClientes() {
    FILE *arquivo = fopen("clientes.dat", "wb");
    if (arquivo != NULL) {
        fwrite(clientes, sizeof(Cliente), numClientes, arquivo);
        fclose(arquivo);
    }
}

void salvarPedidos() {
    FILE *arquivo = fopen("pedidos.dat", "wb");
    if (arquivo != NULL) {
        fwrite(pedidos, sizeof(Pedido), numPedidos, arquivo);
        fclose(arquivo);
    }
}

// Funções para manipulação de produtos
int cadastrarProduto(Produto p) {
    if (numProdutos >= MAX_PRODUTOS) {
        return -1;
    }
    
    // Gerar ID
    if (numProdutos > 0) {
        p.id = produtos[numProdutos - 1].id + 1;
    } else {
        p.id = 1;
    }
    
    produtos[numProdutos] = p;
    numProdutos++;
    salvarProdutos();
    return 0;
}

int editarProduto(int id, Produto p) {
    for (int i = 0; i < numProdutos; i++) {
        if (produtos[i].id == id) {
            p.id = id;
            produtos[i] = p;
            salvarProdutos();
            return 0;
        }
    }
    return -1;
}

int excluirProduto(int id) {
    int encontrado = 0;
    for (int i = 0; i < numProdutos; i++) {
        if (produtos[i].id == id) {
            encontrado = 1;
        }
        if (encontrado && i < numProdutos - 1) {
            produtos[i] = produtos[i + 1];
        }
    }
    if (encontrado) {
        numProdutos--;
        salvarProdutos();
        return 0;
    }
    return -1;
}

int obterMaiorPrecoProduto(float *precoMaximo) {
    if (numProdutos == 0) {
        return -1;
    }
    
    *precoMaximo = produtos[0].preco;
    for (int i = 1; i < numProdutos; i++) {
        if (produtos[i].preco > *precoMaximo) {
            *precoMaximo = produtos[i].preco;
        }
    }
    return 0;
}

// Funções para manipulação de clientes
int cadastrarCliente(Cliente c) {
    if (numClientes >= MAX_CLIENTES) {
        return -1;
    }
    
    // Gerar ID
    if (numClientes > 0) {
        c.id = clientes[numClientes - 1].id + 1;
    } else {
        c.id = 1;
    }
    
    clientes[numClientes] = c;
    numClientes++;
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
        }
        if (encontrado && i < numClientes - 1) {
            clientes[i] = clientes[i + 1];
        }
    }
    if (encontrado) {
        numClientes--;
        salvarClientes();
        return 0;
    }
    return -1;
}

// Funções para manipulação de pedidos
int fazerPedido(Pedido p) {
    if (numPedidos >= MAX_PEDIDOS) {
        return -1;
    }
    
    // Gerar ID
    if (numPedidos > 0) {
        p.id = pedidos[numPedidos - 1].id + 1;
    } else {
        p.id = 1;
    }
    
    pedidos[numPedidos] = p;
    numPedidos++;
    salvarPedidos();
    return 0;
}

void salvarPedidoEmArquivo(Pedido p) {
    char filename[100];
    snprintf(filename, sizeof(filename), "pedido_%d.txt", p.id);
    
    FILE *arquivo = fopen(filename, "w");
    if (arquivo != NULL) {
        fprintf(arquivo, "Pedido ID: %d\n", p.id);
        fprintf(arquivo, "Cliente ID: %d\n", p.idCliente);
        fprintf(arquivo, "Produtos:\n");
        for (int i = 0; i < p.numProdutos; i++) {
            fprintf(arquivo, "  Produto ID: %d, Quantidade: %d\n", p.idProduto[i], p.quantidade[i]);
        }
        fprintf(arquivo, "Total: %.2f\n", p.total);
        fprintf(arquivo, "Valor Pago: %.2f\n", p.valorPago);
        fprintf(arquivo, "Troco: %.2f\n", p.troco);
        fprintf(arquivo, "Método de Pagamento: %s\n", p.metodoPagamento);
        fclose(arquivo);
    }
}

void corrigirIDs() {
    // Implementar se necessário
}
