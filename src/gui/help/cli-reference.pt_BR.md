# Linha de comando e automação

O pacote traz **dois executáveis construídos sobre a mesma biblioteca central**:

| | |
|---|---|
| `imageworker-gui` | a janela em que você está lendo isto |
| `imageworker` | sem interface; tudo o que a GUI faz, mais opções de indexação que ela não expõe |

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
imageworker index  <dir> [--features] [--jobs N] [--ext png,jpg] [--force]
imageworker vocab  <dir> [--words 2048] [--sample 120000]
imageworker find   <dir> --image shot.png [--roi x,y,w,h] [--shortlist N] [--top N]
imageworker dupes  <dir> [--distance 4] [--exact-only] [--near-only]
imageworker query  <dir> --image ref.png [--top 20]
imageworker match  --query q.png --asset a.png
imageworker stats  <dir>
imageworker doctor [--extract img.png]
imageworker formats
```

Todo comando aceita `--db <path>` para colocar o índice em outro lugar que não
`<dir>/.imageworker`.

### Preparar uma pasta para a busca de objetos

```
imageworker index D:/game/Assets --db D:/indexes/game/index.db --features
imageworker vocab D:/game/Assets --db D:/indexes/game/index.db
```

`index` sozinho dá a detecção de duplicatas. `--features` acrescenta os
descritores neurais, e `vocab` treina sobre eles o índice de pré-seleção; os dois
são necessários antes que `find` funcione. Rode `vocab` de novo depois de
acrescentar um lote grande de assets, não a cada arquivo.

### Buscar

```
imageworker find D:/game/Assets --db D:/indexes/game/index.db --image shot.png --json
```

Passe `--roi x,y,w,h` quando souber onde está o objeto. É a opção mais eficaz de
todas: numa região delimitada a busca fica cerca de cinco vezes mais rápida e
encontra aproximadamente o dobro de pontos correspondentes.

### Encadeamento

```
imageworker dupes D:/game/Assets --paths > groups.txt
imageworker find  D:/game/Assets --image shot.png --paths | clip
```

---

## Conferir a instalação

```
imageworker doctor
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
cmake --preset msvc-release -DIMAGEWORKER_WITH_INFERENCE=ON
cmake --build --preset msvc-release
cmake --install build/msvc-release
```

`IMAGEWORKER_WITH_INFERENCE=OFF` compila sem ONNX Runtime e OpenCV; o localizador
de duplicatas continua funcionando, a busca de objetos não.

A etapa de instalação monta uma pasta autossuficiente — todas as dependências ao
lado dos executáveis, nada lido do `PATH`. Um binário único não é possível: o
ONNX Runtime com DirectML só existe como DLL, e `DirectML.dll` é um
redistribuível que não pode ser ligado de jeito nenhum.
