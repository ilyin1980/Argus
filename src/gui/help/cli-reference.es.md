# Línea de comandos y automatización

El paquete incluye **dos ejecutables construidos sobre la misma biblioteca
central**:

| | |
|---|---|
| `argus-gui` | la ventana en la que está leyendo esto |
| `argus` | sin interfaz; todo lo que hace la GUI, más opciones de indexación que ella no ofrece |

Comparten toda la lógica, así que una carpeta indexada por uno la lee el otro.
Ninguno envuelve al otro: ambos enlazan el mismo núcleo estático.

---

## Contrato de salida

Confíe en esto al hacer scripts; es deliberadamente estable.

- `--json` escribe **JSON delimitado por líneas en stdout**, un objeto por línea.
  Cada resultado lleva tanto `rel` (relativa a la carpeta indexada) como `path`
  (absoluta).
- `--paths` escribe **rutas absolutas desnudas**, una por línea, para tuberías.
  En `dupes` una línea en blanco separa los grupos. Implica `--quiet`, y
  combinarla con `--json` es un error en lugar de una elección silenciosa entre
  las dos.
- El progreso, los avisos y los errores van siempre a **stderr**, nunca a stdout.
- Los resultados salen en un orden determinista.
- Códigos de salida: **0** se encontró algo, **1** no se encontró nada, **2** un
  error.

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

Todos los comandos aceptan `--db <path>` para poner el índice en otro sitio que
no sea `<dir>/.argus`.

### Preparar una carpeta para la búsqueda de objetos

```
argus index D:/game/Assets --db D:/indexes/game/index.db --features
argus vocab D:/game/Assets --db D:/indexes/game/index.db
```

`index` por sí solo da la detección de duplicados. `--features` añade los
descriptores neuronales, y `vocab` entrena sobre ellos el índice de
preselección; hacen falta ambos para que `find` funcione. Vuelva a ejecutar
`vocab` tras añadir un lote grande de recursos, no después de cada archivo.

### Buscar

```
argus find D:/game/Assets --db D:/indexes/game/index.db --image shot.png --json
```

Pase `--roi x,y,w,h` cuando sepa dónde está el objeto. Es la opción más eficaz de
todas: sobre una región recuadrada la búsqueda es unas cinco veces más rápida y
encuentra aproximadamente el doble de puntos coincidentes.

### Otras ramas de git

```
argus index D:/game/Assets --branches all
argus index D:/game/Assets --branches release/1.4,feature/new-ui
```

Las imágenes se leen del almacén de objetos, así que no se extrae nada y el árbol
de trabajo queda intacto. El conjunto indicado es el conjunto completo: una rama
indexada antes y no nombrada ahora sale del índice. `--branches all` toma todas
las ramas locales y `--remote-branches` añade las de seguimiento remoto.

Reindexar sale barato. Una rama cuya punta no se ha movido se omite sin listar
ningún árbol, y dentro de una rama decide el identificador del blob: una
dirección por contenido, o sea una comprobación más firme que el tamaño y la
fecha que se usan para los archivos.

Las filas que vienen de una rama nunca informan de una ruta del sistema de
archivos, porque no existe. La salida normal y `--paths` escriben la sintaxis
`rama:ruta` propia de git, que `git show` acepta; `--json` lleva `ref`, `blob` y
`rev` en lugar de `path`.

Con Git LFS los objetos se resuelven desde el almacén LFS local. Los que falten
se cuentan y se informan una sola vez, con `git lfs fetch --all` como solución;
nunca se descarga nada por iniciativa propia.

### Encadenar

```
argus dupes D:/game/Assets --paths > groups.txt
argus find  D:/game/Assets --image shot.png --paths | clip
```

---

## Comprobar la instalación

```
argus doctor
```

Informa de las versiones de ONNX Runtime y OpenCV, de los proveedores de
ejecución disponibles, y de si realmente se puede crear una sesión DirectML con
este controlador, que no es lo mismo que ver el proveedor simplemente en la
lista. `--extract <image>` pasa el extractor por un archivo e informa de cuántos
puntos característicos encontró y cuánto tardó.

---

## Dónde está cada cosa

```
<carpeta de índice>/
  index.db              SQLite: archivos, hashes, miniaturas, desplazamientos de descriptores
  features/desc.f16     descriptores, media precisión
  features/kpts.f32     puntos característicos
  features/vocab.bin    vocabulario visual
  features/bow.bin      índice de preselección
```

Borrar la carpeta del índice lo restablece todo y no toca nada más. Las rutas de
la base se guardan **relativas a la carpeta indexada**, de modo que un índice
sigue siendo válido cuando ese mismo almacenamiento se monta en otro sitio.

Los modelos se buscan en `models/` junto al ejecutable y después en los
directorios superiores, así que tanto una carpeta empaquetada como un árbol de
compilación funcionan sin configuración.

---

## Compilar

```
cmake --preset msvc-release -DARGUS_WITH_INFERENCE=ON
cmake --build --preset msvc-release
cmake --install build/msvc-release
```

`ARGUS_WITH_INFERENCE=OFF` compila sin ONNX Runtime ni OpenCV; el buscador
de duplicados sigue funcionando, la búsqueda de objetos no.

El paso de instalación arma una carpeta autosuficiente: todas las dependencias
junto a los ejecutables, nada leído del `PATH`. Un binario único no es posible:
ONNX Runtime con DirectML se distribuye solo como DLL, y `DirectML.dll` es un
redistribuible que no se puede enlazar en absoluto.
