# Komut satırı ve otomasyon

Paket, **aynı çekirdek kitaplıktan üretilmiş iki çalıştırılabilir dosya**
içerir:

| | |
|---|---|
| `imageworker-gui` | bunu okuduğunuz pencere |
| `imageworker` | penceresiz; GUI'nin yaptığı her şey, artı GUI'nin sunmadığı dizinleme seçenekleri |

Tüm mantığı paylaşırlar; biriyle dizinlenen klasörü öteki okur. Hiçbiri
diğerinin sarmalayıcısı değildir — ikisi de aynı statik çekirdeğe bağlanır.

---

## Çıktı sözleşmesi

Betik yazarken buna güvenebilirsiniz; bilerek kararlıdır.

- `--json`, **stdout'a satırlarla ayrılmış JSON** yazar; satır başına bir nesne.
  Her sonuç hem `rel` (dizinlenen klasöre göre) hem de `path` (mutlak) taşır.
- `--paths`, boru hattı için **yalnızca mutlak yolları** satır satır yazar.
  `dupes` içinde grupları boş bir satır ayırır. `--quiet` anlamına gelir ve
  `--json` ile birleştirilmesi, ikisi arasında sessiz bir seçim yerine hata
  sayılır.
- İlerleme, uyarı ve hatalar her zaman **stderr**'e gider, asla stdout'a değil.
- Sonuçlar belirlenimci bir sırayla çıkar.
- Çıkış kodları: **0** bir şey bulundu, **1** hiçbir şey bulunamadı, **2** hata.

---

## Komutlar

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

Her komut, dizini `<dir>/.imageworker` dışında bir yere koymak için `--db <path>`
alır.

### Bir klasörü nesne aramaya hazırlamak

```
imageworker index D:/game/Assets --db D:/indexes/game/index.db --features
imageworker vocab D:/game/Assets --db D:/indexes/game/index.db
```

Tek başına `index` kopya bulmayı sağlar. `--features` sinirsel betimleyicileri
ekler, `vocab` ise bunların üzerinde ön eleme dizinini eğitir; `find`'ın
çalışması için ikisi de gerekir. `vocab`'ı her dosyadan sonra değil, büyük bir
varlık partisi ekledikten sonra yeniden çalıştırın.

### Arama

```
imageworker find D:/game/Assets --db D:/indexes/game/index.db --image shot.png --json
```

Nesnenin nerede olduğunu biliyorsanız `--roi x,y,w,h` verin. En etkili seçenek
budur: çerçevelenmiş bir bölgede arama yaklaşık beş kat hızlanır ve kabaca iki
katı eşleşen nokta bulur.

### Boru hattı

```
imageworker dupes D:/game/Assets --paths > groups.txt
imageworker find  D:/game/Assets --image shot.png --paths | clip
```

---

## Kurulumu denetlemek

```
imageworker doctor
```

ONNX Runtime ile OpenCV sürümlerini, kullanılabilir yürütme sağlayıcılarını ve bu
sürücüde bir DirectML oturumunun gerçekten oluşturulup oluşturulamadığını bildirir
— ki bu, sağlayıcının yalnızca listede görünmesiyle aynı şey değildir.
`--extract <image>`, çıkarıcıyı tek bir dosya üzerinde çalıştırır ve kaç anahtar
nokta bulduğunu, ne kadar sürdüğünü bildirir.

---

## Ne nerede durur

```
<dizin klasörü>/
  index.db              SQLite: dosyalar, özetler, önizlemeler, betimleyici konumları
  features/desc.f16     betimleyiciler, yarım duyarlık
  features/kpts.f32     anahtar noktalar
  features/vocab.bin    görsel sözlük
  features/bow.bin      ön eleme dizini
```

Dizin klasörünü silmek her şeyi sıfırlar ve başka hiçbir şeye dokunmaz.
Veritabanındaki yollar **dizinlenen klasöre göre** saklanır, böylece aynı depolama
başka bir yere bağlandığında dizin geçerli kalır.

Modeller önce çalıştırılabilir dosyanın yanındaki `models/` içinde, sonra üstteki
klasörlerde aranır — böylece hem paketlenmiş bir klasör hem de bir derleme ağacı
ayar gerektirmeden çalışır.

---

## Derleme

```
cmake --preset msvc-release -DIMAGEWORKER_WITH_INFERENCE=ON
cmake --build --preset msvc-release
cmake --install build/msvc-release
```

`IMAGEWORKER_WITH_INFERENCE=OFF`, ONNX Runtime ve OpenCV olmadan derler; kopya
bulma çalışmaya devam eder, nesne arama etmez.

Kurulum adımı kendi kendine yeten bir klasör oluşturur: tüm bağımlılıklar
çalıştırılabilir dosyaların yanında, hiçbiri `PATH`'ten okunmaz. Tek dosyalık bir
derleme mümkün değildir: DirectML'li ONNX Runtime yalnızca DLL olarak gelir ve
`DirectML.dll` hiçbir şekilde içeri bağlanamayan yeniden dağıtılabilir bir
bileşendir.
