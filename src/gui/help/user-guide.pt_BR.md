# Como usar o Argus

> *Argos Panoptes, o gigante de cem olhos: metade dos olhos ficava aberta
> enquanto os outros dormiam, de modo que nada lhe escapava.*

O Argus responde a duas perguntas sobre uma pasta de arquivos `png` e
`jpg`:

- **Onde este asset aparece na minha captura de tela?** — a tarefa principal.
- **Quais arquivos desta pasta são duplicatas?** — a secundária.

---

## 1. Aponte para uma pasta

**Imagens** é a pasta a percorrer. **Índice em** é o lugar onde o Argus
guarda os próprios dados: o banco, as miniaturas e os descritores.

Por padrão o índice vai para uma pasta oculta `.argus` dentro da pasta de
imagens. Mude **Índice em** quando as imagens estiverem num compartilhamento de
rede ou somente leitura, quando você preferir não acrescentar nada ao repositório
de outra pessoa, ou quando o índice couber melhor num disco mais rápido.
**Padrão** devolve tudo ao lugar.

Pressione **Indexar**. A primeira passagem lê todos os arquivos; as seguintes só
releem aqueles cujo tamanho ou data de modificação mudou, de modo que manter o
índice em dia sai barato.

> Indexar 4600 imagens somando cerca de 400 MB leva aproximadamente três
> minutos, descritores neurais incluídos.

---

## 2. Encontrar um asset dentro de uma captura

Abra **Busca por exemplo** e traga a captura:

- pressione **Ctrl+V** para colá-la da área de transferência — funciona a partir
  de qualquer uma das abas;
- arraste um arquivo de imagem para a janela;
- ou use **Procurar…**.

**Desenhe um retângulo em volta do objeto procurado.** Isso importa mais do que
parece: no quadro inteiro a busca gasta a maior parte do esforço no fundo. Numa
biblioteca de 4600 assets, um objeto delimitado leva cerca de **2,6 segundos
contra 5,6** do quadro inteiro, e encontra aproximadamente **o dobro de pontos
correspondentes**, porque cada um deles cai sobre o objeto e não sobre o cenário.
Clique fora do retângulo para apagá-lo e voltar a buscar no quadro inteiro.

Pressione **Buscar**. O melhor resultado é contornado sobre a captura, e todos os
resultados aparecem abaixo, em ordem.

### Como ler os resultados

Cada resultado mostra uma **pontuação** e um **número de inliers**. Inliers são
os pontos casados que concordam com um único posicionamento coerente — é aí que
está a evidência. A pontuação pesa o quanto esses casamentos foram *coerentes*, e
cai quando são poucos.

**O resultado é uma lista ordenada, não uma resposta sim ou não.** Outro desenho
do mesmo personagem também passa na verificação, porque a maior parte da imagem
realmente coincide. O correto fica claramente acima: leia a ordem em vez de
procurar um único vencedor.

### Método

- **Localizar objeto na imagem** — a busca neural descrita acima. Exige um índice
  com descritores; se a entrada estiver esmaecida, veja abaixo.
- **Similaridade da imagem inteira** — um hash perceptual que compara imagens
  como um todo. Bom para achar uma cópia redimensionada ou recodificada da
  *mesma imagem*, inútil para achar um objeto pequeno dentro de uma captura.

Se **Localizar objeto** estiver desativado, o índice ainda não tem descritores.
Crie-os uma vez pela linha de comando:

```
argus index "<pasta de imagens>" --db "<pasta do índice>\index.db" --features
argus vocab "<pasta de imagens>" --db "<pasta do índice>\index.db"
```

---

## 3. Encontrar duplicatas

Abra **Duplicatas**, ajuste **Distância máx.** e pressione **Buscar duplicatas**.

- Grupos **exatos** são arquivos idênticos byte a byte. Sobre esses dá para agir
  com segurança.
- Grupos **próximos** se parecem aos olhos de um hash perceptual. **Revise-os,
  não apague às cegas.** O hash descreve a composição, então um mesmo personagem
  desenhado com várias expressões cai num único grupo em qualquer distância
  utilizável.

Distância 0 significa impressões idênticas; 4 tolera recodificação e
redimensionamento. Acima de 8 os grupos servem só para revisão.

Os grupos são listados pelo espaço que desperdiçam, do maior para o menor. Dentro
de um grupo, o arquivo maior e de resolução mais alta vem primeiro — normalmente
o que se quer manter.

---

## 4. Tirar os caminhos

Todo resultado carrega o caminho completo:

- a barra de baixo mostra o caminho da seleção atual, e **Copiar** copia todos os
  caminhos selecionados, um por linha;
- o pequeno ícone de cópia ao lado de um nome de arquivo copia só aquele;
- o botão direito oferece o caminho completo, o caminho relativo à pasta, ou
  mostrar o arquivo no gerenciador de arquivos.

---

## 5. Outros branches do git

Se a pasta de imagens estiver dentro de um repositório git, **Branches…** fica
disponível. Marque os branches que quiser e pressione **Indexar**: as imagens
deles são lidas direto do repositório, então nada é feito checkout e sua árvore
de trabalho não é tocada. O branch em que você está não aparece na lista — ele *é*
a árvore de trabalho.

A lista de branches é o retrato completo do que o índice contém. Desmarcar um
branch e indexar de novo o remove.

Resultados vindos de um branch mostram o nome dele e se identificam como
`branch:caminho` em vez de um caminho de arquivo, porque esse arquivo não existe
no disco. **Abrir** extrai essa versão para um arquivo temporário para você
poder vê-la; mostrar no gerenciador de arquivos e buscar com ela valem só para
arquivos da árvore de trabalho.

Reindexar sai barato: um branch cuja ponta não se moveu é pulado por inteiro, e
dentro de um branch só são relidos os arquivos cujo conteúdo realmente mudou.

> **Git LFS**: as imagens são resolvidas a partir do seu armazenamento LFS local.
> Se um branch nunca foi baixado, o Argus diz quantos objetos faltam e que
> `git lfs fetch --all` os traz — sozinho ele não baixa nada.

---

## 6. Temas e idioma

**Exibir → Tema** alterna entre Sistema, Sci-fi, Material e Floral com o programa
em execução. A escolha é lembrada.

**Exibir → Idioma** troca a interface, também sem reiniciar. A lista traz todos
os idiomas que esta versão carrega; **Idioma do sistema** acompanha o sistema
operacional. Este manual também é traduzido, e recorre ao inglês para um idioma
cujo manual ainda não está pronto.

---

## O que ele não faz

- O hash perceptual encontra **a mesma imagem**, nunca apenas um tema parecido.
- A busca neural encontra assets **realmente presentes** na imagem. Não é uma
  busca por descrição.
- Um asset que não produz nenhum ponto-chave — uma cor chapada, um ícone
  minúsculo — não pode ser localizado. Cerca de 4% de uma biblioteca de assets
  típica cai nessa categoria.
