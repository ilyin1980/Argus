# Linha de comando e automação

O pacote traz **dois executáveis construídos sobre a mesma biblioteca central**:

| | |
|---|---|
| `argus-gui` | a janela em que você está lendo isto |
| `argus` | sem interface; tudo o que a GUI faz, mais opções de indexação que ela não expõe |

Os dois compartilham toda a lógica, então uma pasta indexada por um é lida pelo
outro. Nenhum é invólucro do outro — ambos ligam o mesmo núcleo estático.

---

## Contrato de saída

Conte com isto ao escrever scripts; ele é deliberadamente estável.

- `--json` escreve **JSON delimitado por linhas em stdout**, um objeto por linha.
  Cada resultado carrega tanto `rel` (relativo à pasta indexada) quanto `path`
  (absoluto).
- `--paths` escreve **apenas caminhos absolutos**, um por linha, para
  encadeamento. Em `dupes` uma linha em branco separa os grupos. Implica
  `--quiet`, e combiná-lo com `--json` é um erro, não uma escolha silenciosa
  entre os dois.
- Progresso, avisos e erros vão sempre para o **stderr**, nunca para o stdout.
- Os resultados saem numa ordem determinística.
- Códigos de saída: **0** achou algo, **1** não achou nada, **2** erro.

---

## Comandos

```
argus index  <dir> [--features] [--jobs N] [--ext png,jpg] [--force]
argus vocab  <dir> [--words 2048] [--sample 120000]
argus find   <dir> --image shot.png [--roi x,y,w,h] [--shortlist N] [--top N]
argus dupes  <dir> [--distance 4] [--exact-only] [--near-only]
argus query  <dir> --image ref.png [--top 20]
argus match  --query q.png --asset a.png
argus stats  <dir>
argus doctor [--extract img.png]
argus formats
```

Todo comando aceita `--db <path>` para colocar o índice em outro lugar que não
`<dir>/.argus`.

### Preparar uma pasta para a busca de objetos

```
argus index D:/game/Assets --db D:/indexes/game/index.db --features
argus vocab D:/game/Assets --db D:/indexes/game/index.db
```

`index` sozinho dá a detecção de duplicatas. `--features` acrescenta os
descritores neurais, e `vocab` treina sobre eles o índice de pré-seleção; os dois
são necessários antes que `find` funcione. Rode `vocab` de novo depois de
acrescentar um lote grande de assets, não a cada arquivo.

### Buscar

```
argus find D:/game/Assets --db D:/indexes/game/index.db --image shot.png --json
```

Passe `--roi x,y,w,h` quando souber onde está o objeto. É a opção mais eficaz de
todas: numa região delimitada a busca fica cerca de cinco vezes mais rápida e
encontra aproximadamente o dobro de pontos correspondentes.

### Outros branches do git

```
argus index D:/game/Assets --branches all
argus index D:/game/Assets --branches release/1.4,feature/new-ui
```

As imagens são lidas do armazenamento de objetos, então nada é feito checkout e a
árvore de trabalho fica intacta. O conjunto informado é o conjunto completo: um
branch indexado antes e não citado agora sai do índice. `--branches all` pega
todos os branches locais, e `--remote-branches` acrescenta os de rastreamento
remoto.

Reindexar sai barato. Um branch com a ponta parada é pulado sem listar nenhuma
árvore, e dentro de um branch quem decide é o id do blob: um endereço por
conteúdo, portanto uma verificação mais firme do que o tamanho e a data usados
para arquivos.

Linhas vindas de um branch nunca informam um caminho do sistema de arquivos,
porque ele não existe. A saída normal e `--paths` escrevem a sintaxe
`branch:caminho` do próprio git, que o `git show` aceita; `--json` carrega `ref`,
`blob` e `rev` no lugar de `path`.

Com Git LFS os objetos são resolvidos do armazenamento LFS local. Os que faltam
são contados e informados uma única vez, com `git lfs fetch --all` como solução;
nada é baixado por conta própria.

### Encadeamento

```
argus dupes D:/game/Assets --paths > groups.txt
argus find  D:/game/Assets --image shot.png --paths | clip
```

---

## Conferir a instalação

```
argus doctor
```

Informa as versões do ONNX Runtime e do OpenCV, os provedores de execução
disponíveis, e se uma sessão DirectML realmente pode ser criada neste driver — o
que não é a mesma coisa que o provedor apenas aparecer na lista.
`--extract <image>` roda o extrator sobre um arquivo e informa quantos pontos-chave
encontrou e quanto tempo levou.

---

## Onde fica cada coisa

```
<pasta do índice>/
  index.db              SQLite: arquivos, hashes, miniaturas, deslocamentos dos descritores
  features/desc.f16     descritores, meia precisão
  features/kpts.f32     pontos-chave
  features/vocab.bin    vocabulário visual
  features/bow.bin      índice de pré-seleção
```

Apagar a pasta do índice zera tudo e não mexe em mais nada. Os caminhos no banco
são guardados **relativos à pasta indexada**, então um índice continua válido
quando o mesmo armazenamento é montado em outro lugar.

Os modelos são procurados em `models/` ao lado do executável e depois nos
diretórios acima — assim tanto uma pasta empacotada quanto uma árvore de
compilação funcionam sem configuração.

---

## Compilando

```
cmake --preset msvc-release -DARGUS_WITH_INFERENCE=ON
cmake --build --preset msvc-release
cmake --install build/msvc-release
```

`ARGUS_WITH_INFERENCE=OFF` compila sem ONNX Runtime e OpenCV; o localizador
de duplicatas continua funcionando, a busca de objetos não.

A etapa de instalação monta uma pasta autossuficiente — todas as dependências ao
lado dos executáveis, nada lido do `PATH`. Um binário único não é possível: o
ONNX Runtime com DirectML só existe como DLL, e `DirectML.dll` é um
redistribuível que não pode ser ligado de jeito nenhum.
