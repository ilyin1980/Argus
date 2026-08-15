# Komut satırı ve otomasyon

Paket, **aynı çekirdek kitaplıktan üretilmiş iki çalıştırılabilir dosya**
içerir:

| | |
|---|---|
| `argus-gui` | bunu okuduğunuz pencere |
| `argus` | penceresiz; GUI'nin yaptığı her şey, artı GUI'nin sunmadığı dizinleme seçenekleri |

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

Her komut, dizini `<dir>/.argus` dışında bir yere koymak için `--db <path>`
alır.

### Bir klasörü nesne aramaya hazırlamak

```
argus index D:/game/Assets --db D:/indexes/game/index.db --features
argus vocab D:/game/Assets --db D:/indexes/game/index.db
```

Tek başına `index` kopya bulmayı sağlar. `--features` sinirsel betimleyicileri
ekler, `vocab` ise bunların üzerinde ön eleme dizinini eğitir; `find`'ın
çalışması için ikisi de gerekir. `vocab`'ı her dosyadan sonra değil, büyük bir
varlık partisi ekledikten sonra yeniden çalıştırın.

### Arama

```
argus find D:/game/Assets --db D:/indexes/game/index.db --image shot.png --json
```

Nesnenin nerede olduğunu biliyorsanız `--roi x,y,w,h` verin. En etkili seçenek
budur: çerçevelenmiş bir bölgede arama yaklaşık beş kat hızlanır ve kabaca iki
katı eşleşen nokta bulur.

### Diğer git dalları

```
argus index D:/game/Assets --branches all
argus index D:/game/Assets --branches release/1.4,feature/new-ui
```

Görseller nesne deposundan okunur; hiçbir şey checkout edilmez ve çalışma ağacına
dokunulmaz. Belirtilen küme tam kümedir: daha önce dizinlenmiş ve şimdi
adı geçmeyen bir dal dizinden düşer. `--branches all` bütün yerel dalları alır,
`--remote-branches` ise uzak izleme dallarını ekler.

Yeniden dizinleme ucuzdur. Ucu kıpırdamamış bir dal, tek bir ağaç bile
listelenmeden atlanır; bir dal içinde ise kararı blob kimliği verir — içerik
adresi olduğundan, dosyalar için kullanılan boyut ve tarihten daha güçlü bir
denetimdir.

Bir daldan gelen satırlar hiçbir zaman dosya sistemi yolu bildirmez, çünkü öyle
bir yol yoktur. Normal çıktı ve `--paths`, `git show`'un kabul ettiği kendi
`dal:yol` sözdizimini yazar; `--json` ise `path` yerine `ref`, `blob` ve `rev`
taşır.

Git LFS altında nesneler yerel LFS deposundan çözülür. Eksik olanlar sayılır ve
bir kez bildirilir; çaresi `git lfs fetch --all`'dır. Kendiliğinden hiçbir şey
indirilmez.

### Boru hattı

```
argus dupes D:/game/Assets --paths > groups.txt
argus find  D:/game/Assets --image shot.png --paths | clip
```

---

## Kurulumu denetlemek

```
argus doctor
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
cmake --preset msvc-release -DARGUS_WITH_INFERENCE=ON
cmake --build --preset msvc-release
cmake --install build/msvc-release
```

`ARGUS_WITH_INFERENCE=OFF`, ONNX Runtime ve OpenCV olmadan derler; kopya
bulma çalışmaya devam eder, nesne arama etmez.

Kurulum adımı kendi kendine yeten bir klasör oluşturur: tüm bağımlılıklar
çalıştırılabilir dosyaların yanında, hiçbiri `PATH`'ten okunmaz. Tek dosyalık bir
derleme mümkün değildir: DirectML'li ONNX Runtime yalnızca DLL olarak gelir ve
`DirectML.dll` hiçbir şekilde içeri bağlanamayan yeniden dağıtılabilir bir
bileşendir.
