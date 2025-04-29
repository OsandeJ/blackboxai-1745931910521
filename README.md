
Built by https://www.blackbox.ai

---

```markdown
# Sistema de Gestão de Vendas

## Project Overview
O Sistema de Gestão de Vendas é um aplicativo desenvolvido em C que permite gerenciar produtos, clientes e pedidos de forma simplificada. O sistema contém funcionalidades para cadastrar, editar e excluir produtos e clientes, além de realizar pedidos e visualizar relatórios das transações realizadas.

## Installation

### Requirements
- GCC ou qualquer outro compilador C compatível
- GTK+ 3.0 ou superior para a interface gráfica

### Building the Project
1. **Clone o repositório:**
   ```bash
   git clone <URL_DO_REPOSITORIO>
   cd <PASTA_DO_PROJETO>
   ```

2. **Compile os arquivos:**
   Se você estiver utilizando o GCC, pode compilar o projeto da seguinte maneira:
   ```bash
   gcc -o sistema_gestao main.c backend.c gui.c produtos_gui.c clientes_gui.c pedidos_gui.c -lgtk-3 -lgdk-3 -lgio-2.0
   ```

3. **Execute o projeto:**
   ```bash
   ./sistema_gestao
   ```

## Usage

Ao abrir o sistema, você será apresentado a um menu principal onde poderá escolher entre as seguintes opções:
- Cadastrar Produto
- Cadastrar Cliente
- Fazer Pedido
- Visualizar Pedidos
- Pesquisar Cliente por Nome
- Listar Produtos
- Sair

Siga os prompts para interagir com as funcionalidades oferecidas.

## Features
- Cadastro e gerenciamento de produtos e clientes.
- Realização de pedidos com registro detalhado.
- Persistência de dados através do armazenamento em arquivos binários.
- Interface gráfica utilizando GTK+ 3 para melhor usabilidade.
- Visualização de pedidos e histórico de transações.

## Dependencies
O projeto utiliza as seguintes bibliotecas e pacotes:
- `gtk-3`: Para a interface gráfica
- `glib`: Para manipulação de strings e dados

Instale as dependências no seu sistema com os seguintes comandos (para sistemas baseados em Debian):
```bash
sudo apt-get install libgtk-3-dev
sudo apt-get install build-essential
```

## Project Structure

```
.
├── src
│   ├── gui
│   │   ├── gui.c
│   │   ├── produtos_gui.c
│   │   ├── clientes_gui.c
│   │   └── pedidos_gui.c
│   ├── backend
│   │   ├── backend.c
│   └── main.c
├── config.css
├── Makefile (se aplicável)
└── README.md
```

- **main.c**: Ponto de entrada da aplicação, onde a interface gráfica é inicializada e os dados são carregados.
- **backend.c**: Contém as funções responsáveis pela manipulação de dados de produtos, clientes e pedidos.
- **gui.c**: Contém a lógica e a criação da interface gráfica da aplicação.
- **produtos_gui.c**, **clientes_gui.c**, **pedidos_gui.c**: Módulos para a interface gráfica de cada entidade, permitindo adicionar, editar e excluir informações.

## Contributing
Contribuições são bem-vindas! Sinta-se à vontade para abrir uma issue ou pull request no repositório.

## License
Este projeto é licenciado sob a MIT License - veja o arquivo [LICENSE](LICENSE) para mais detalhes.
```

**Note:** Replace `<URL_DO_REPOSITORIO>` with the actual repository URL if applicable. Make sure that the project details are accurately represented according to your specific project structure and requirements.