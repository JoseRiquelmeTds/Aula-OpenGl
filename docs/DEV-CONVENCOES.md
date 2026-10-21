# Convenções de desenvolvimento

Regras do repositório: como escrever o código, como nomear branches, commits e PRs, e o fluxo até o merge.
Onde cada coisa fica: [ARQUITETURA.md](ARQUITETURA.md). Modelagem (unidades, pivô, vértice): [CONVENCOES.md](CONVENCOES.md).

## Sumário

1. [Idioma](#1-idioma)
2. [Nomes (cases)](#2-nomes-cases)
3. [Formatação](#3-formatação)
4. [Headers e includes](#4-headers-e-includes)
5. [Regras de C++ e OpenGL](#5-regras-de-c-e-opengl)
6. [Branches](#6-branches)
7. [Commits](#7-commits)
8. [Pull requests](#8-pull-requests)
9. [Fluxo de uma task](#9-fluxo-de-uma-task)

---

## 1. Idioma

| O quê | Idioma | Exemplo |
| --- | --- | --- |
| Identificadores (classes, funções, variáveis, arquivos `.hpp/.cpp`) | inglês | `ProjectionScreen`, `setOpen()`, `projection_screen.cpp` |
| Comentários, mensagens de log, documentação | português | `// pivô no topo da tela` |
| Branches, commits, PRs | português (sem acento na branch) | `CG-66/feat-tela-retratil` |

## 2. Nomes (cases)

| Elemento | Case | Exemplo |
| --- | --- | --- |
| Arquivo C++ | `snake_case`, `.hpp` / `.cpp` com o mesmo nome | `scene_node.hpp`, `scene_node.cpp` |
| Classe, struct, enum | `PascalCase` | `SceneNode`, `MeshData`, `Texture::Wrap` |
| Valor de `enum class` | `PascalCase` | `Wrap::ClampToEdge` |
| Função, método | `camelCase`, verbo | `beginFrame()`, `addChild()`, `configureVertexAttributes()` |
| Variável local, parâmetro | `camelCase` | `deltaTime`, `textureSize` |
| Membro **privado** | `camelCase_` (sublinhado no fim) | `position_`, `indexCount_` |
| Membro **público** de classe/struct | `camelCase` | `movementSpeed`, `transform`, `visible` |
| Constante (`constexpr`, `const` de arquivo) | `kPascalCase` | `kWindowWidth`, `kMaxPitch` |
| Namespace | minúsculas | `renderer`, `primitives`, `objects` |
| Header guard | `AULA_OPENGL_<PASTA>_<ARQUIVO>_HPP` | `AULA_OPENGL_OBJECTS_FAN_HPP` |
| Macro | evite; se precisar, `UPPER_SNAKE_CASE` | — |

**Shaders (GLSL):**

| Elemento | Case | Exemplo |
| --- | --- | --- |
| Arquivo | `snake_case.vert` / `.frag`, par com o mesmo nome | `basic.vert`, `basic.frag` |
| Atributo de entrada do vertex shader | `aPascalCase` | `aPosition`, `aNormal`, `aUV` |
| Saída do vertex / entrada do fragment | `vPascalCase` | `vWorldPos`, `vNormal` |
| Uniform | `uPascalCase` | `uModel`, `uView`, `uTexture` |
| Saída do fragment shader | `FragColor` | — |

**Outros arquivos:** texturas em `snake_case` sem acento (`madeira_carteira.png`); documentação em `MAIUSCULAS.md` sem
acento (`DEV-CONVENCOES.md`), porque acento em nome de arquivo quebra links e scripts no Windows.

## 3. Formatação

Não há `clang-format` no repositório; siga o estilo do código existente:

- **4 espaços**, sem tab. **Até 120 colunas.**
- Chave na mesma linha (`if (...) {`, `class X {`, `void f() {`), e **sempre** com chaves, mesmo em `if` de uma linha.
- `public:` / `private:` no nível da classe; `case` indentado dentro do `switch`.
- `T* ptr` e `const T& ref` (o símbolo junto do tipo).
- Literais `float` com `f`: `0.5f`, `1.0f`.
- Comentário explica **por quê** ou a convenção (pivô, unidade, sentido); não repete o que o código diz.
- Arquivos em UTF-8 com quebra de linha LF (o `.gitattributes` força LF).

## 4. Headers e includes

- Header guard `#ifndef/#define/#endif` com o nome da [tabela acima](#2-nomes-cases), comentado no `#endif`.
- Header só com declarações e comentário de uso; implementação no `.cpp`. Exceção: getters de uma linha.
- Ordem dos includes, separados por linha em branco:
  1. no `.cpp`, o próprio header (`#include "gfx/mesh.hpp"`);
  2. terceiros com `<>`, **`<glad/gl.h>` sempre antes do GLFW** e de outros headers de OpenGL;
  3. biblioteca padrão (`<vector>`, `<memory>`);
  4. headers do projeto com `""`, caminho a partir de `include/` (`"core/camera.hpp"`, `"gfx/mesh.hpp"`).
- Nada de `using namespace` em header.
- Funções e constantes usadas só num `.cpp` ficam num **namespace anônimo**.
- Declaração antecipada (`class Camera;`) quando o header só usa ponteiro ou referência.

## 5. Regras de C++ e OpenGL

- **C++17**, compila **sem warnings** com `-Wall -Wextra -Wpedantic` no Windows e no Linux.
- **OpenGL 3.3 core apenas:** nada de `glBegin`/`glEnd`, `glMatrixMode`, GLU, GLUT ou GLEW. Matrizes com GLM.
- **Recurso de GPU tem dono RAII:** a classe cria no construtor, apaga no destrutor e **proíbe cópia**
  (`X(const X&) = delete;`), como `Shader`, `Mesh` e `Texture`. Recursos de GPU só existem com o contexto ativo:
  crie depois do `gladLoadGL` e destrua antes do `glfwDestroyWindow`.
- **Erros não derrubam o programa:** vão para `stderr` com o prefixo do módulo (`[Texture] ...`) e o objeto fica
  inválido (`isValid() == false`). Nada de exceção nem `exit()` fora do `main.cpp`.
- **Caminhos relativos à raiz do repositório** (`"shaders/basic.vert"`, `"assets/textures/..."`); o alvo `run` já executa
  da raiz.
- **Ponteiro cru não é dono** (ex.: `SceneNode::mesh`). Quem é dono usa membro direto ou `std::unique_ptr`.
- `const` sempre que der (métodos que não alteram estado, parâmetros por `const&`).
- Sem variáveis globais mutáveis em código novo; estado fica em membros (`Classroom`, objetos).

## 6. Branches

`main` é a versão entregue, `dev` é a branch padrão de integração. As duas são protegidas: **não aceitam push direto** e
o merge exige PR com **1 aprovação de outra pessoa** (CG-86).

**Formato:**

```
CG-<n>/<tipo>-<descricao-em-kebab-case>
```

- `CG-<n>`: a chave da task no Jira, **em maiúsculas**.
- `<tipo>`: um dos tipos de [commit](#7-commits) (`feat`, `fix`, `refactor`, `docs`, `build`, `chore`...).
- `<descricao>`: 2 a 5 palavras em português, minúsculas, sem acento, separadas por hífen.
- Sempre criada a partir de `dev` atualizado: `git switch dev && git pull && git switch -c CG-59/feat-ventilador-parede`.

| ✅ | ❌ |
| --- | --- |
| `CG-59/feat-ventilador-parede` | `ventilador` (sem task e sem tipo) |
| `CG-75/feat-tela-retratil-escala` | `CG-75/feat-Tela_Retrátil` (maiúscula, sublinhado, acento) |
| `CG-90/fix-normal-tampa-cilindro` | `feat/CG-90-fix` (ordem trocada) |
| `CG-53/docs-convencoes` | `cg-53/docs-convencoes` (chave em minúsculas) |

O padrão vale para as branches **novas**. As que já existem (`cg-54/...`, `cg-55/...`, `feat/...`) ficam como estão,
não precisam ser renomeadas.

> [!WARNING]
> **Windows não diferencia maiúsculas de minúsculas em nomes de branch.** `cg-55/...` e `CG-55/...` viram a mesma pasta
> em `.git/refs/heads/` e o Git recusa ou mistura as duas. Não crie uma branch `CG-<n>/...` para uma task que já tem
> branch `cg-<n>/...`: continue na existente.

Mudança sem task no Jira (raro: ajuste de CI, typo) usa `<tipo>/<descricao>`, por exemplo `chore/atualiza-gitignore`.
Se a mudança for maior que isso, crie a task antes.

## 7. Commits

[Conventional Commits](https://www.conventionalcommits.org/pt-br/), com a task no escopo:

```
<tipo>(CG-<n>): <descrição no presente, minúscula, sem ponto final>

<corpo opcional: o porquê da mudança, quebrado em ~72 colunas>
```

| Tipo | Quando | Exemplo |
| --- | --- | --- |
| `feat` | funcionalidade nova (objeto, interação, controle) | `feat(CG-59): modela o corpo e as hélices do ventilador` |
| `fix` | correção de bug | `fix(CG-58): valida a unidade de textura antes do bind` |
| `refactor` | muda a estrutura sem mudar o comportamento | `refactor(CG-55): separa os atalhos de inspeção do update` |
| `docs` | só documentação ou comentários | `docs(CG-53): adiciona convenções de modelagem e de repositório` |
| `build` | CMake, presets, bibliotecas em `lib/` | `build(CG-52): embute o stb_image` |
| `chore` | manutenção sem código de produção (gitignore, templates) | `chore(CG-51): ignora a pasta out/` |
| `perf` | só desempenho | `perf(CG-71): calcula a normal matrix na CPU` |
| `style` | só formatação, sem mudar lógica | `style(CG-62): ajusta indentação da carteira` |

- **Um commit = uma mudança que compila.** Não misture refactor e feature no mesmo commit.
- Textura nova vai no mesmo commit que a linha dela em `assets/textures/README.md`.
- Configure seu nome e e-mail antes do primeiro commit em cada máquina (há commits no histórico como "Your Name"):
  `git config --global user.name "Nome Sobrenome"` e `git config --global user.email "voce@exemplo.com"`.

## 8. Pull requests

- **Base:** `dev`. PR para `main` só na entrega, a partir de `dev`.
- **Título:** `[CG-<n>] <resumo da task>`, normalmente o título da task no Jira.
  Ex.: `[CG-55] Câmera livre básica para inspeção dos modelos`.
- **Descrição:** o template (`.github/pull_request_template.md`) já vem preenchido no GitHub: o que mudou, como testar,
  checklist.
- **Um PR por task.** Se a task B depende da A ainda aberta, abra o PR de B com base na branch de A e avise na
  descrição; quando A for mergeado, troque a base de B para `dev`.
- **Revisão:** 1 aprovação de **outra pessoa**. Quem revisa compila e roda (`--target run`), não só lê o diff.
- **Merge:** pelo botão do GitHub (merge commit, como no histórico atual), feito por quem abriu o PR depois da aprovação.
  Depois apague a branch remota.
- Antes de pedir revisão, atualize com `dev` (`git pull origin dev` na sua branch) e resolva os conflitos você mesmo.

## 9. Fluxo de uma task

Status do Jira (projeto CG): `Backlog` → `A fazer` → `Em Progresso` → `Em Revisão` → `Finalizado`, e `Bloqueado` quando
depender de algo fora do seu controle.

| Momento | Git | Jira |
| --- | --- | --- |
| Começou | branch `CG-<n>/<tipo>-...` a partir de `dev` | `Em Progresso`, responsável = você |
| Travou em outra task/pessoa | — | `Bloqueado` + comentário dizendo o quê |
| Abriu o PR | `[CG-<n>] ...` com base `dev` | `Em Revisão` |
| PR mergeado em `dev` | apaga a branch | `Finalizado` |

Se a task foi feita por outra pessoa, troque o responsável no Jira antes de finalizar: a divisão do trabalho entra no
relatório (CG-80).
