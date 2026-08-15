# ImageWorker nasıl kullanılır

ImageWorker, `png` ve `jpg` dosyaları bulunan bir klasör hakkında iki soruyu
yanıtlar:

- **Bu varlık ekran görüntümde nerede geçiyor?** — asıl iş.
- **Bu klasördeki hangi dosyalar birbirinin kopyası?** — ikincil iş.

---

## 1. Bir klasör gösterin

**Görseller**, taranacak klasördür. **Dizin konumu**, ImageWorker'ın kendi
verilerini tuttuğu yerdir: veritabanı, önizlemeler ve betimleyiciler.

Varsayılan olarak dizin, görsel klasörünün içindeki gizli `.imageworker`
klasörüne gider. Görseller salt okunur ya da ağ paylaşımındaysa, başkasının
deposuna hiçbir şey eklemek istemiyorsanız veya dizinin yeri daha hızlı bir
diskse **Dizin konumu**'nu değiştirin. **Varsayılan** eski yerine geri koyar.

**Dizinle**'ye basın. İlk geçiş her dosyayı okur; sonraki geçişler yalnızca boyutu
ya da değiştirilme zamanı değişen dosyaları yeniden okur, bu yüzden dizini güncel
tutmak ucuzdur.

> Toplam 400 MB tutan 4600 görselin dizinlenmesi, sinirsel betimleyiciler dâhil,
> kabaca üç dakika sürer.

---

## 2. Ekran görüntüsündeki bir varlığı bulun

**Örnekle arama**'yı açın, sonra ekran görüntüsünü içeri alın:

- panodan yapıştırmak için **Ctrl+V**'ye basın — her iki sekmeden de çalışır;
- bir görsel dosyasını pencerenin üzerine sürükleyin;
- ya da **Gözat…**'ı kullanın.

**Aradığınız nesnenin çevresine bir çerçeve çizin.** Bu, göründüğünden daha
önemlidir: tüm kare üzerinde arama, emeğinin çoğunu arka plana harcar. 4600
varlıklık bir kitaplıkta çerçevelenmiş bir nesne, tüm kare için 5,6 saniyeye
karşılık yaklaşık **2,6 saniye** sürer ve kabaca **iki katı eşleşen nokta**
bulur; çünkü bu noktaların her biri dekora değil nesnenin üstüne düşer. Çerçeveyi
silip yeniden tüm karede aramak için çerçevenin dışına tıklayın.

**Ara**'ya basın. En iyi eşleşme ekran görüntüsünde çerçevelenir ve tüm sonuçlar
sıralanmış hâlde aşağıda görünür.

### Sonuçları okumak

Her sonuç bir **puan** ve bir **iç nokta sayısı** gösterir. İç noktalar, tek bir
tutarlı yerleşim üzerinde uzlaşan eşleşmiş noktalardır; kanıt onlardır. Puan, bu
eşleşmelerin ne kadar *tutarlı* olduğunu tartar ve sayıları azaldıkça düşer.

**Sonuç, evet/hayır yanıtı değil, sıralanmış bir listedir.** Aynı karakterin başka
bir çizimi de doğrulamayı geçer, çünkü resmin büyük bölümü gerçekten eşleşir.
Doğru olan belirgin biçimde üstte yer alır; o yüzden tek bir kazanan aramak
yerine sıralamayı okuyun.

### Yöntem

- **Görseldeki nesneyi bul** — yukarıda anlatılan sinirsel arama. Betimleyici
  içeren bir dizin gerektirir; seçenek soluksa aşağıya bakın.
- **Tüm görsel benzerliği** — resimleri bütün olarak karşılaştıran algısal özet.
  *Aynı resmin* ölçeklenmiş ya da yeniden kodlanmış kopyasını bulmakta iyidir,
  ekran görüntüsündeki küçük bir nesneyi bulmakta işe yaramaz.

**Nesneyi bul** kapalıysa dizinde henüz betimleyici yoktur. Bunları komut
satırından bir kez oluşturun:

```
imageworker index "<görsel klasörü>" --db "<dizin klasörü>\index.db" --features
imageworker vocab "<görsel klasörü>" --db "<dizin klasörü>\index.db"
```

---

## 3. Kopyaları bulun

**Kopyalar**'ı açın, **Azami uzaklık**'ı ayarlayın ve **Kopyaları bul**'a basın.

- **Birebir** gruplar bayt bayt aynı dosyalardır. Bunlar üzerinde gönül rahatlığıyla
  işlem yapabilirsiniz.
- **Yakın** gruplar algısal özet gözüyle birbirine benzer. **Bunları gözden
  geçirin, körü körüne silmeyin.** Özet yerleşimi betimler, bu yüzden birkaç
  farklı ifadeyle çizilmiş aynı karakter, kullanılabilir her uzaklıkta tek bir
  gruba düşer.

Uzaklık 0 aynı parmak izleri demektir; 4, yeniden kodlamayı ve ölçeklemeyi
kaldırır. 8'in üzerinde gruplar yalnızca gözden geçirmek içindir.

Gruplar boşa harcanan alana göre, en büyükten başlayarak sıralanır. Bir grubun
içinde en büyük ve en yüksek çözünürlüklü dosya başta gelir — genellikle saklamak
isteyeceğiniz dosya.

---

## 4. Yolları dışarı almak

Her sonuç tam yolunu taşır:

- alttaki çubuk geçerli seçimin yolunu gösterir, **Kopyala** ise seçili tüm
  yolları satır satır kopyalar;
- dosya adının yanındaki küçük kopyalama simgesi yalnızca o yolu kopyalar;
- sağ tık; tam yolu, klasöre göreli yolu veya dosyayı dosya yöneticisinde
  göstermeyi sunar.

---

## 5. Diğer git dalları

Görsel klasörü bir git deposunun içindeyse **Dallar…** düğmesi etkinleşir.
İstediğiniz dalları işaretleyip **Dizinle**'ye basın: görselleri doğrudan
depodan okunur, yani hiçbir şey checkout edilmez ve çalışma ağacınıza
dokunulmaz. Üzerinde bulunduğunuz dal listede yoktur — o zaten çalışma ağacıdır.

Dal listesi, dizinin neyi içerdiğinin tam resmidir. Bir dalın işaretini kaldırıp
yeniden dizinlemek onu dizinden çıkarır.

Bir daldan gelen sonuçlar dal adını gösterir ve dosya yolu yerine `dal:yol`
biçiminde kendini tanıtır; çünkü diskte öyle bir dosya yoktur. **Aç**, o sürümü
geçici bir dosyaya çıkarır, böylece ona bakabilirsiniz; dosya yöneticisinde
gösterme ve onunla arama yalnızca çalışma ağacındaki dosyalar içindir.

Yeniden dizinleme ucuzdur: ucu kıpırdamamış bir dal bütünüyle atlanır ve bir dal
içinde yalnızca içeriği gerçekten değişen dosyalar yeniden okunur.

> **Git LFS**: görseller yerel LFS deponuzdan çözülür. Bir dal hiç çekilmemişse
> ImageWorker kaç nesnenin eksik olduğunu ve bunları `git lfs fetch --all`
> komutunun getireceğini söyler — kendiliğinden hiçbir şey indirmez.

---

## 6. Temalar ve dil

**Görünüm → Tema**, program çalışırken Sistem, Sci-fi, Material ve Çiçek arasında
geçiş yapar. Seçim hatırlanır.

**Görünüm → Dil** arayüzü değiştirir, bu da yeniden başlatma gerektirmez. Listede
bu sürümün içerdiği tüm diller bulunur; **Sistem dili** işletim sistemini izler.
Bu kılavuz da çevrilmiştir ve kılavuzu henüz hazır olmayan bir dil için İngilizceye
düşer.

---

## Yapamadıkları

- Algısal özet **aynı resmi** bulur, yalnızca benzer bir konuyu asla.
- Sinirsel arama, görselde **gerçekten bulunan** varlıkları bulur. Açıklamaya göre
  arama değildir.
- Hiç anahtar nokta üretmeyen bir varlık — düz bir renk, minicik bir simge —
  konumlandırılamaz. Tipik bir varlık kitaplığının yaklaşık %4'ü bu gruba girer.
