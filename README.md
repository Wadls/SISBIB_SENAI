# SISBIB - Sistema de Gestão e Controle de Biblioteca

Sistema de console em **Linguagem C** para automatizar o controle do acervo bibliográfico e o fluxo de circulação de livros de uma Instituição de Ensino Superior.

> **SENAI Regional Metropolitana – Lauro de Freitas/Bahia**
> Curso Técnico em Desenvolvimento de Sistemas · Módulo 1
> Unidade Curricular: Lógica de Programação · Docente: Jorge Luiz dos Santos

## Objetivo

Hoje o controle de empréstimos, devoluções, cadastro de acervo e aplicação de sanções (multas, suspensões e ressarcimentos) é feito de forma muito manual, o que gera divergências de estoque, inconsistências nos dados de usuários e atrasos na aplicação das regras. O SISBIB busca resolver esses problemas automatizando todo o processo.

## Funcionalidades

- **CRUD** de Usuários, Autores e Livros (com exclusão condicional)
- **Empréstimos** com atualização automática de estoque
- **Devoluções** com cálculo de atraso, multa diária e suspensão
- **Ressarcimento** financeiro integral em caso de perda ou dano
- **Persistência** em arquivos locais (carga na inicialização e gravação antes de encerrar)
- **Menus numerados** com tratamento de entradas inválidas

## Regras de Negócio (resumo)

| Tema | Regra |
|------|-------|
| Exclusão | Não é possível excluir usuários, autores ou livros com vínculos/empréstimos ativos |
| Status do usuário | `ATIVO`, `INATIVO` ou `SUSPENSO`; usuários não ativos ficam bloqueados para novos empréstimos |
| Empréstimo | Exige usuário `ATIVO` e exemplar fisicamente disponível |
| Atraso | Devolução fora do prazo gera multa diária e pode gerar suspensão |
| Perda/dano | Registro de ressarcimento financeiro integral |

## Estrutura Planejada

```
sisbib/
├── main.c
├── interface.h / interface.c      # menus e captura de dados
├── negocio.h / negocio.c          # regras de empréstimo, multas e validações
├── persistencia.h / persistencia.c # leitura e gravação em arquivos
├── utils.h / utils.c              # datas, limpeza de buffer e validações gerais
└── dados/                         # arquivos de persistência
```

**Entidades (structs):** `Usuario`, `Autor`, `Livro`, `Emprestimo`

## Como compilar e executar

```bash
gcc -std=c99 -Wall -o sisbib main.c interface.c negocio.c persistencia.c utils.c
./sisbib
```

## Restrições do Projeto

- Desenvolvido estritamente em **C** com bibliotecas padrão
- **Sem** banco de dados (SQL) e **sem** interface gráfica (GUI)
- Persistência por manipulação direta de arquivos locais
- Gerenciamento correto de memória e limpeza do buffer do teclado a cada leitura
- Mensagens claras, numeradas e orientativas ao usuário

## Etapas do Projeto

| Desafio | Fase | Entrega |
|---------|------|---------|
| 1 | Análise de Requisitos | Objetivo, fluxograma e lista de regras de negócio |
| 2 | Projeto | Modelo de dados (structs) e organização dos módulos `.h`/`.c` |
| 3 | Implementação | Código-fonte em C (CRUDs, movimentação, persistência, interface) |
| 4 | Teste e Validação | Cenários de teste, relatório e correção de bugs |
| 5 | Apresentação Final | Demonstração, documentação e relatório de testes |
