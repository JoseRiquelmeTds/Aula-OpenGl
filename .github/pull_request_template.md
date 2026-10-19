<!-- Título: [CG-<n>] <resumo da task>. Base: dev. Regras: docs/DEV-CONVENCOES.md § 8 -->

**Task:** CG-<n>

## O que mudou

-

## Como testar

1. `cmake --preset linux` (ou `windows`) e `cmake --build --preset linux --target run`
2.

## Checklist

- [ ] Compila sem warnings (`-Wall -Wextra -Wpedantic`)
- [ ] Testado rodando a janela, não só compilando
- [ ] Segue `docs/CONVENCOES.md` (metros, Y para cima, pivô, frente para +Z, posição + normal + UV)
- [ ] Arquivos no lugar indicado em `docs/ARQUITETURA.md`
- [ ] Tecla nova ou alterada registrada na tabela de atalhos (`docs/CONVENCOES.md` § 8) e no README § Comandos
- [ ] Textura nova registrada em `assets/textures/README.md`, com origem e licença
- [ ] Task no Jira em `Em Revisão`
