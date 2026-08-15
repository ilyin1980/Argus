# 命令行与自动化

安装包提供**由同一个核心库构建出的两个可执行文件**：

| | |
|---|---|
| `argus-gui` | 你正在阅读本文的窗口 |
| `argus` | 无界面；GUI 能做的一切，外加 GUI 未开放的索引选项 |

两者共享全部逻辑，因此由其中一个建立的索引，另一个可以直接读取。它们互不包装 ——
都链接同一个静态核心。

---

## 输出约定

编写脚本时可以依赖这些约定，它们是刻意保持稳定的。

- `--json` 向 **stdout 输出以换行分隔的 JSON**，每行一个对象。每条结果同时带有
  `rel`（相对于被索引目录）和 `path`（绝对路径）。
- `--paths` 输出**纯粹的绝对路径**，每行一个，便于管道处理。在 `dupes` 中，空行
  用于分隔分组。该选项隐含 `--quiet`；与 `--json` 同时使用会报错，而不是悄悄在两
  者之间做选择。
- 进度、警告和错误始终写入 **stderr**，绝不写入 stdout。
- 结果的顺序是确定的。
- 退出码：**0** 有结果，**1** 无结果，**2** 出错。

---

## 命令

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

每条命令都接受 `--db <path>`，用于把索引放到 `<dir>/.argus` 以外的位置。

### 为对象搜索准备目录

```
argus index D:/game/Assets --db D:/indexes/game/index.db --features
argus vocab D:/game/Assets --db D:/indexes/game/index.db
```

只运行 `index` 即可获得重复检测。`--features` 会加上神经网络描述子，`vocab` 则在
其之上训练候选索引；`find` 需要这两步都完成才能工作。在批量添加大量素材之后重新
运行 `vocab`，而不是每加一个文件就运行一次。

### 搜索

```
argus find D:/game/Assets --db D:/indexes/game/index.db --image shot.png --json
```

当你知道对象大致在哪里时，请传入 `--roi x,y,w,h`。这是最有效的选项：在框定区域上
搜索大约快五倍，并且找到的匹配点大约多一倍。

### 其他 git 分支

```
argus index D:/game/Assets --branches all
argus index D:/game/Assets --branches release/1.4,feature/new-ui
```

图片从对象库中读取，因此不会检出任何内容，工作区保持原样。所列出的集合就是完整
集合：之前索引过、这次没有列出的分支会从索引中移除。`--branches all` 取所有本地
分支，`--remote-branches` 再加上远程跟踪分支。

重新建立索引的代价很低。顶端没有移动的分支会被跳过，连一棵树都不会列出；在分支
内部，由 blob 标识决定是否变化 —— 它是内容地址，因此比文件所用的大小与修改时间更
可靠。

来自分支的记录绝不会给出文件系统路径，因为并不存在。普通输出与 `--paths` 会写出
git 自己的 `分支:路径` 语法，`git show` 可以直接接受；`--json` 则以 `ref`、`blob`
和 `rev` 取代 `path`。

在 Git LFS 下，对象从本地 LFS 存储中解析。缺失的对象会被统计并只报告一次，并给出
`git lfs fetch --all` 作为解决办法；绝不会自行下载任何内容。

### 管道

```
argus dupes D:/game/Assets --paths > groups.txt
argus find  D:/game/Assets --image shot.png --paths | clip
```

---

## 检查安装

```
argus doctor
```

报告 ONNX Runtime 与 OpenCV 的版本、可用的执行提供程序，以及在当前驱动上是否真的
能创建 DirectML 会话 —— 这与提供程序只是出现在列表中并不是一回事。
`--extract <image>` 会对单个文件运行提取器，并报告找到多少关键点、耗时多久。

---

## 各文件的位置

```
<索引目录>/
  index.db              SQLite：文件、哈希、预览图、描述子偏移
  features/desc.f16     描述子，半精度
  features/kpts.f32     关键点
  features/vocab.bin    视觉词典
  features/bow.bin      候选索引
```

删除索引目录会重置一切，且不影响其他任何内容。数据库中的路径是**相对于被索引目录**
保存的，因此当同一存储被挂载到别处时，索引依然有效。

模型会先在可执行文件旁边的 `models/` 中查找，然后依次向上层目录查找 —— 这样打包好
的目录和构建目录都无需额外配置即可工作。

---

## 构建

```
cmake --preset msvc-release -DARGUS_WITH_INFERENCE=ON
cmake --build --preset msvc-release
cmake --install build/msvc-release
```

`ARGUS_WITH_INFERENCE=OFF` 表示不依赖 ONNX Runtime 和 OpenCV 进行构建；重复
查找仍然可用，对象搜索则不可用。

安装步骤会组装出一个自包含的目录 —— 所有依赖都在可执行文件旁边，不从 `PATH` 读取
任何东西。无法构建成单个文件：带 DirectML 的 ONNX Runtime 只以 DLL 形式提供，而
`DirectML.dll` 是一个根本无法静态链接的可再分发组件。
