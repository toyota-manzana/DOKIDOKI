# 💡 DOKIDOKI – 心拍と連動するスマートペンライト

![Arduino](https://img.shields.io/badge/Code-Arduino-blue?logo=arduino)
![ESP-NOW](https://img.shields.io/badge/Wireless-ESP--NOW-orange?logo=espressif)
![M5StickC](https://img.shields.io/badge/Hardware-M5Stack-red?logo=espressif)
![MAX30100](https://img.shields.io/badge/Sensor-MAX30100-green)
![WS2812B](https://img.shields.io/badge/LED-WS2812B-yellow)
![Fusion 360](https://img.shields.io/badge/3D-Fusion%20360-orange?logo=autodesk)
![FastLED](https://img.shields.io/badge/Library-FastLED-lightblue)

**DOKIDOKI** は、M5StickC Plus2 と MAX30100 心拍センサを使って「演者と観客を光でつなぐ」スマートペンライトです。


https://github.com/user-attachments/assets/17568819-5b7a-4fd7-aafc-20940c4dec21


---

## ✨ Features / 機能

- ❤️ 心拍数（BPM）に応じてLEDの色と明滅を制御  
- 📡 ESP-NOW通信で心拍データをリアルタイム共有  
- 🎭 ステージ上の演者と観客が“同じ鼓動で光る”体験を演出  
- 🔧 3Dプリントした筐体にLEDを内蔵  

---

## 🧠 Concept / コンセプト

ライブで感じる「一体感」を、テクノロジーの力で可視化したいという思いから生まれました。

本体はペンライト用「KIRAKIRA」と、心拍センサーデバイス「DOKIDOKI」で構成されています。

DOKIDOKI (Sensor) → ESP-NOW → KIRAKIRA (LED)

- 演者の心拍に応じて、観客のペンライトが光る
    - 演者は、ステージが巨大な胎内に感じられる
    - 観客は、演者のリアルな心境が感じられる
- 観客の心拍に応じて、観客のペンライトが光る
    - 演者は、観客の興奮を色で実感することができる
    - 観客は、自分たちの興奮を演者に伝えることができる

---

## 🛠 Hardware / 使用パーツ

| 部品 | 数 | 用途 |
|------|------|------|
| M5StickC Plus2 | 2 | 送信・受信用 |
| MAX30100 | 1 | 心拍センサ |
| WS2812B LEDテープ | 1 | 発光部 |
| 消臭ビーズ | - | 拡散素材 |
| 消臭ビーズ・[ダブルパック](https://hands.net/goods/4571253490987/?srsltid=AfmBOoqlqhc-WfwXnskZxjAxS-qVK5IdbzFM0zz7NS32AF7wHfgBxlVs) | 1 | 拡散素材 |
| 3Dプリント筐体 | - | 本体 |

---

## 🎨 Color Logic / カラーロジック

| BPM範囲 | 色の変化 |
|----------|-----------|
| ≤ 70 | 青（安静） |
| 70〜100 | 緑〜黄〜赤（緊張） |
| ≥ 100 | レインボー（MAX） |

---

## 🧩 ファイル構成

~~~text
DOKIDOKI/
├── KIRAKIRA.ino # LEDテープがついたM5stickC Plus2
├── DOKIDOKI.ino　# 心拍センサーがついたM5stickC Plus2
├── TOYOLIGHT v7.stl # 本体のデータ 
└── README.md # このファイル
~~~

## 環境開発構築

https://docs.m5stack.com/ja/arduino/m5stickc_plus2/program

1. Arduino IDEをインストールする
   1. プログラムを書いたり、M5stickC Plus2に書き込んだりするために、Ardiono IDEをインストールします
   2. インストローラーをダウンロードする: https://www.arduino.cc/en/software/#ide
   3. ダウンロードしたファイルを開いてインストールする
   4. 設定→言語を日本語に設定
2. USBシリアルドライバのインストール
    1. MacとM5stickC Plus2を繋ぐためのドライバをインストールします
    2. インストーラをダウンロードする（USB Driver & Open source Library）: https://docs.m5stack.com/en/download
    3. ダウンロードしたファイルを開いてインストールする
    4. セキュリティ設定を変更して再起動する
3. ポートを選択する
    1. M5stickC plus2をMacと接続する
    2. ツール→ポート（ボードと見分ける）
    3. 接続したM5stickC Plus2は「cu.usbserial-XXXXXXX」と表示されるはずなので、選択する（DOKIDOKIとKIRAKIRAを区別するために、それぞれのXXXXXXをメモする）
4. ボードをインストールする
   1. 設定→追加のボードマネージャのURLに「https://static-cdn.m5stack.com/resource/arduino/package_m5stack_index.json」を記入
   2. ツール→ボード（ポートと見分ける）→ボードマネージャを開く
   3. 「m5stack」を検索してインストールする
   4. ツール→ボード→M5stickC Plus2を選択する
5. ライブラリをインストールする
   1. ツール→ライブラリを管理..を選択する
   2. 以下をインストールする
        1. M5StickCPlus2
        2. FastLED
        3. MAX30100lib
6. ファイル→開く→*.inoファイルを開く
7. ボードとポートを確認する
8. スケッチ→書き込み

## 🛠　ペンライト本体にM5stickCplus2を取り付ける

- ペンライトの底に付属の取り付け部品をビス留めします
- 取り付け部品にM5stickC plus2を取り付けます
- M5stickC plus2にLEDテープを取り付けます
- ペンライト本体の穴の中にLEDテープを入れます
- 3DデータにはLEDテープを通すための穴を開けましたが、十分ではありませんでした
- 仕方がないので、ペンライト本体の背面をライターで炙って、プラスドライバーで穴を開けました
- 再現する時は気をつけてください！

## 🫶 Special Thanks

- ChatGPT 🤝 
