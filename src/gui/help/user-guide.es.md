# Cómo usar ImageWorker

ImageWorker responde a dos preguntas sobre una carpeta de archivos `png` y
`jpg`:

- **¿Dónde aparece este recurso en mi captura de pantalla?** — la tarea
  principal.
- **¿Qué archivos de esta carpeta están duplicados?** — la secundaria.

---

## 1. Apuntar a una carpeta

**Imágenes** es la carpeta en la que buscar. **Índice en** es el lugar donde
ImageWorker guarda sus propios datos: la base de datos, las miniaturas y los
descriptores.

De forma predeterminada el índice va a una carpeta oculta `.imageworker` dentro
de la carpeta de imágenes. Cambie **Índice en** cuando las imágenes estén en un
recurso de red o de solo lectura, cuando prefiera no añadir nada al repositorio
de otra persona, o cuando el índice deba ir en un disco más rápido.
**Predeterminado** lo devuelve a su sitio.

Pulse **Indexar**. La primera pasada lee todos los archivos; las siguientes solo
releen aquellos cuyo tamaño o fecha de modificación cambió, así que mantener el
índice al día sale barato.

> Indexar 4600 imágenes de unos 400 MB lleva aproximadamente tres minutos,
> incluidos los descriptores neuronales.

---

## 2. Encontrar un recurso dentro de una captura

Abra **Búsqueda por ejemplo** y meta la captura:

- pulse **Ctrl+V** para pegarla desde el portapapeles — funciona desde cualquiera
  de las dos pestañas;
- arrastre un archivo de imagen a la ventana;
- o use **Examinar…**.

**Dibuje un recuadro alrededor del objeto que busca.** Importa más de lo que
parece: sobre el fotograma completo la búsqueda gasta casi todo su esfuerzo en el
fondo. En una biblioteca de 4600 recursos, un objeto recuadrado tarda unos
**2,6 segundos frente a 5,6** del fotograma entero, y encuentra
aproximadamente **el doble de puntos coincidentes**, porque todos ellos caen
sobre el objeto y no sobre el decorado. Haga clic fuera del recuadro para
borrarlo y volver a buscar en todo el fotograma.

Pulse **Buscar**. El mejor resultado queda perfilado sobre la captura, y todos
los resultados aparecen debajo, ordenados.

### Cómo leer los resultados

Cada resultado muestra una **puntuación** y un **número de inliers**. Los inliers
son los puntos emparejados que coinciden en una misma colocación coherente: esa
es la evidencia. La puntuación pondera lo *coherentes* que fueron esos
emparejamientos, y baja cuando son pocos.

**El resultado es una lista ordenada, no un sí o un no.** Otro dibujo del mismo
personaje también supera la verificación, porque la mayor parte de la imagen
coincide de verdad. El correcto queda claramente por encima: lea el orden en
lugar de buscar un único ganador.

### Método

- **Localizar un objeto en la imagen** — la búsqueda neuronal descrita arriba.
  Necesita un índice con descriptores; si la entrada está en gris, vea más abajo.
- **Similitud de la imagen completa** — un hash perceptual que compara imágenes
  en conjunto. Bueno para encontrar una copia reescalada o recodificada de *la
  misma imagen*, inútil para encontrar un objeto pequeño dentro de una captura.

Si **Localizar un objeto** está desactivado, el índice todavía no tiene
descriptores. Créelos una vez desde la línea de comandos:

```
imageworker index "<carpeta de imágenes>" --db "<carpeta de índice>\index.db" --features
imageworker vocab "<carpeta de imágenes>" --db "<carpeta de índice>\index.db"
```

---

## 3. Buscar duplicados

Abra **Duplicados**, ajuste **Distancia máx.** y pulse **Buscar duplicados**.

- Los grupos **exactos** son archivos idénticos byte a byte. Con ellos se puede
  actuar con confianza.
- Los grupos **cercanos** se parecen a ojos de un hash perceptual. **Revíselos,
  no los borre a ciegas.** El hash describe la composición, así que un mismo
  personaje dibujado con varias expresiones cae en un solo grupo a cualquier
  distancia utilizable.

Distancia 0 significa huellas idénticas; 4 tolera la recodificación y el
reescalado. Por encima de 8 los grupos son solo para revisar.

Los grupos se listan por el espacio que desperdician, de mayor a menor. Dentro de
un grupo, el archivo más grande y de mayor resolución va primero: normalmente el
que conviene conservar.

---

## 4. Sacar las rutas

Cada resultado lleva su ruta completa:

- la barra inferior muestra la ruta de la selección actual, y **Copiar** copia
  todas las rutas seleccionadas, una por línea;
- el pequeño icono de copiar junto a un nombre de archivo copia solo esa;
- el botón derecho ofrece la ruta completa, la ruta relativa a la carpeta, o
  mostrar el archivo en el explorador.

---

## 5. Otras ramas de git

Si la carpeta de imágenes está dentro de un repositorio git, se habilita
**Ramas…**. Marque las ramas que quiera y pulse **Indexar**: sus imágenes se leen
directamente del repositorio, así que no se extrae nada y su árbol de trabajo no
se toca. La rama que tiene activa no aparece en la lista: ella *es* el árbol de
trabajo.

La lista de ramas describe exactamente lo que contiene el índice. Desmarcar una
rama y volver a indexar la elimina.

Los resultados de una rama muestran su nombre y se identifican como
`rama:ruta` en lugar de como una ruta de archivo, porque ese archivo no existe en
el disco. **Abrir** extrae esa versión a un archivo temporal para que pueda
verla; mostrarla en el gestor de archivos y buscar con ella son solo para
archivos del árbol de trabajo.

Reindexar sale barato: una rama cuya punta no se ha movido se omite por completo,
y dentro de una rama solo se releen los archivos cuyo contenido cambió de verdad.

> **Git LFS**: las imágenes se resuelven desde su almacén LFS local. Si una rama
> nunca se descargó, ImageWorker dice cuántos objetos faltan y que
> `git lfs fetch --all` los trae; por su cuenta no descarga nada.

---

## 6. Temas e idioma

**Ver → Tema** alterna entre Sistema, Sci-fi, Material y Floral mientras el
programa está en marcha. La elección se recuerda.

**Ver → Idioma** cambia la interfaz, también sin reiniciar. La lista contiene
todos los idiomas que trae esta versión; **Idioma del sistema** sigue al sistema
operativo. Este manual también está traducido, y recurre al inglés para un idioma
cuyo manual todavía no está listo.

---

## Lo que no puede hacer

- El hash perceptual encuentra **la misma imagen**, nunca simplemente un motivo
  parecido.
- La búsqueda neuronal encuentra recursos **realmente presentes** en la imagen.
  No es una búsqueda por descripción.
- Un recurso que no produce ningún punto característico — un color plano, un
  icono diminuto — no se puede localizar. Alrededor del 4 % de una biblioteca de
  recursos típica cae en esta categoría.
