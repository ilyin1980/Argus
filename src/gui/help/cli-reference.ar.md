# سطر الأوامر والأتمتة

تضم الحزمة **برنامجين مبنيين على المكتبة الأساسية نفسها**:

| | |
|---|---|
| `imageworker-gui` | النافذة التي تقرأ فيها هذا النص |
| `imageworker` | بلا واجهة؛ كل ما تفعله الواجهة الرسومية، وزيادة خيارات فهرسة لا تعرضها |

يتقاسمان المنطق كله، فالمجلد الذي يفهرسه أحدهما يقرأه الآخر. وليس أحدهما غلافًا
حول الآخر، بل يرتبط كلاهما بالنواة الساكنة نفسها.

---

## عقد المخرجات

اعتمد عليه في البرمجة النصية؛ فهو ثابت عن قصد.

- `--json` يكتب **JSON مفصولًا بأسطر إلى stdout**، كائنًا واحدًا في كل سطر. وتحمل
  كل نتيجة `rel` (نسبةً إلى المجلد المفهرس) و`path` (المسار المطلق) معًا.
- `--paths` يكتب **مسارات مطلقة مجردة**، واحدًا في كل سطر، لتمريرها في أنبوب. وفي
  `dupes` يفصل سطر فارغ بين المجموعات. ويستلزم الخيار `--quiet`، وجمعه مع `--json`
  خطأ لا اختيارًا صامتًا بينهما.
- التقدّم والتحذيرات والأخطاء تذهب دائمًا إلى **stderr**، ولا تذهب إلى stdout أبدًا.
- تخرج النتائج بترتيب حتمي.
- رموز الخروج: **٠** وُجد شيء، **١** لم يوجد شيء، **٢** خطأ.

---

## الأوامر

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

يقبل كل أمر `--db <path>` لوضع الفهرس في موضع غير `<dir>/.imageworker`.

### تجهيز مجلد للبحث عن الأجسام

```
imageworker index D:/game/Assets --db D:/indexes/game/index.db --features
imageworker vocab D:/game/Assets --db D:/indexes/game/index.db
```

`index` وحده يمنحك كشف التكرارات. و`--features` يضيف الواصفات العصبية، و`vocab`
يدرّب فوقها فهرس القائمة المختصرة؛ وكلاهما لازم قبل أن يعمل `find`. أعد تشغيل
`vocab` بعد إضافة دفعة كبيرة من العناصر، لا بعد كل ملف.

### البحث

```
imageworker find D:/game/Assets --db D:/indexes/game/index.db --image shot.png --json
```

مرّر `--roi x,y,w,h` حين تعرف موضع الجسم. وهو أنفع الخيارات على الإطلاق: على
منطقة محددة يصير البحث أسرع بنحو خمس مرات، ويجد تقريبًا ضعف عدد النقاط المتطابقة.

### التمرير في أنبوب

```
imageworker dupes D:/game/Assets --paths > groups.txt
imageworker find  D:/game/Assets --image shot.png --paths | clip
```

---

## فحص التثبيت

```
imageworker doctor
```

يذكر إصداري ONNX Runtime وOpenCV، ومزوّدي التنفيذ المتاحين، وما إذا كان إنشاء
جلسة DirectML ممكنًا فعلًا على هذا المشغّل — وهذا غير مجرد ظهور المزوّد في القائمة.
و`--extract <image>` يشغّل المستخرج على ملف واحد ويذكر كم نقطة مميزة وجد وكم
استغرق.

---

## أين يوجد كل شيء

```
<مجلد الفهرس>/
  index.db              SQLite: الملفات والبصمات والمعاينات وإزاحات الواصفات
  features/desc.f16     الواصفات بدقة نصفية
  features/kpts.f32     النقاط المميزة
  features/vocab.bin    المعجم البصري
  features/bow.bin      فهرس القائمة المختصرة
```

حذف مجلد الفهرس يعيد كل شيء إلى حاله ولا يمس أي شيء آخر. وتُحفظ المسارات في قاعدة
البيانات **نسبةً إلى المجلد المفهرس**، فيبقى الفهرس صالحًا حين يُركَّب التخزين نفسه
في موضع آخر.

يُبحث عن النماذج في `models/` بجوار البرنامج، ثم في المجلدات الأعلى منه — وبذلك
يعمل المجلد المحزوم وشجرة البناء كلاهما بلا إعداد.

---

## البناء

```
cmake --preset msvc-release -DIMAGEWORKER_WITH_INFERENCE=ON
cmake --build --preset msvc-release
cmake --install build/msvc-release
```

`IMAGEWORKER_WITH_INFERENCE=OFF` يبني من دون ONNX Runtime وOpenCV؛ فيظل كشف
التكرارات عاملًا، ولا يعمل البحث عن الأجسام.

خطوة التثبيت تجمع مجلدًا مكتفيًا بذاته: كل اعتمادية بجوار البرنامجين، ولا شيء
يُقرأ من `PATH`. أما البناء في ملف واحد فغير ممكن: إذ لا يأتي ONNX Runtime مع
DirectML إلا كمكتبة DLL، و`DirectML.dll` مكوّن قابل لإعادة التوزيع لا يمكن ربطه
داخليًا البتة.
