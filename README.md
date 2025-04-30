# Sistema de Gestão

Sistema de gestão moderno desenvolvido em C com interface gráfica GTK+3, adaptado para o mercado angolano.

## Funcionalidades

- Gestão completa de produtos
  - Cadastro, edição e exclusão de produtos
  - Visualização em lista e grid
  - Filtro dinâmico por preço (slider)
  - Cálculo automático de IVA (14%)
  - Exportação de dados
  - Visualização de estatísticas

- Gestão de clientes
  - Cadastro e manutenção de clientes
  - Histórico de pedidos por cliente
  - Formatação de telefone no padrão angolano

- Gestão de pedidos
  - Registro de pedidos
  - Cálculo automático de valores
  - Impressão de comprovantes
  - Histórico de vendas

## Interface Moderna

- Design responsivo e intuitivo
- Temas claro e escuro
- Animações suaves
- Notificações toast
- Ícones modernos
- Validações em tempo real

## Requisitos

- Sistema operacional Linux
- GTK+ 3.0 ou superior
- GCC (GNU Compiler Collection)
- Make

## Instalação

1. Instale as dependências:
```bash
make deps
```

2. Configure o ambiente:
```bash
make setup
```

3. Compile o projeto:
```bash
make
```

4. Execute o sistema:
```bash
make run
```

## Estrutura do Projeto

```
sistema-gestao/
├── src/
│   ├── backend/       # Lógica de negócios
│   │   ├── backend.c
│   │   └── backend.h
│   └── gui/          # Interface gráfica
│       ├── styles/   # Estilos CSS
│       ├── views/    # Telas do sistema
│       ├── widgets/  # Componentes reutilizáveis
│       ├── gui.c
│       └── gui.h
├── main.c            # Ponto de entrada
└── Makefile         # Script de compilação
```

## Características Técnicas

- Desenvolvido em C
- Interface gráfica com GTK+3
- Armazenamento em arquivos binários
- Suporte a localização (pt_AO)
- Moeda: Kwanza (Kz)
- IVA: 14%

## Desenvolvimento

Para contribuir com o projeto:

1. Clone o repositório
2. Crie uma branch para sua feature
3. Faça suas alterações
4. Envie um pull request

## Licença

Este projeto está sob a licença MIT.

## Suporte

Para suporte ou dúvidas, abra uma issue no repositório do projeto.
