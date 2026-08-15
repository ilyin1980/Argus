# Línea de comandos y automatización

El paquete incluye **dos ejecutables construidos sobre la misma biblioteca
central**:

| | |
|---|---|
| `imageworker-gui` | la ventana en la que está leyendo esto |
| `imageworker` | sin interfaz; todo lo que hace la GUI, más opciones de indexación que ella no ofrece |

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

Todos los comandos aceptan `--db <path>` para poner el índice en otro sitio que
no sea `<dir>/.imageworker`.

### Preparar una carpeta para la búsqueda de objetos

```
imageworker index D:/game/Assets --db D:/indexes/game/index.db --features
imageworker vocab D:/game/Assets --db D:/indexes/game/index.db
```

`index` por sí solo da la detección de duplicados. `--features` añade los
descriptores neuronales, y `vocab` entrena sobre ellos el índice de
preselección; hacen falta ambos para que `find` funcione. Vuelva a ejecutar
`vocab` tras añadir un lote grande de recursos, no después de cada archivo.

### Buscar

```
imageworker find D:/game/Assets --db D:/indexes/game/index.db --image shot.png --json
```

Pase `--roi x,y,w,h` cuando sepa dónde está el objeto. Es la opción más eficaz de
todas: sobre una región recuadrada la búsqueda es unas cinco veces más rápida y
encuentra aproximadamente el doble de puntos coincidentes.

### Encadenar

```
imageworker dupes D:/game/Assets --paths > groups.txt
imageworker find  D:/game/Assets --image shot.png --paths | clip
```

---

## Comprobar la instalación

```
imageworker doctor
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
cmake --preset msvc-release -DIMAGEWORKER_WITH_INFERENCE=ON
cmake --build --preset msvc-release
cmake --install build/msvc-release
```

`IMAGEWORKER_WITH_INFERENCE=OFF` compila sin ONNX Runtime ni OpenCV; el buscador
de duplicados sigue funcionando, la búsqueda de objetos no.

El paso de instalación arma una carpeta autosuficiente: todas las dependencias
junto a los ejecutables, nada leído del `PATH`. Un binario único no es posible:
ONNX Runtime con DirectML se distribuye solo como DLL, y `DirectML.dll` es un
redistribuible que no se puede enlazar en absoluto.
