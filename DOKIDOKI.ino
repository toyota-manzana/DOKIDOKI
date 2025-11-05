// Arduino言語では、コメントは//のあとに入れる（Pythonでは#）
// #include:「この名前のファイルの内容を、ここにまるごと読み込んでください」という命令
// ライブラリ: 他の誰かが書いた便利なプログラムを使わせてもらう
// 図書館（ライブラリ）で本を借りてきて、自分で描いている小説の中でまるまるパクっちゃうイメージ
// 〇〇.h: 〇〇は、本のタイトル. 〇〇.hファイルには、目次が書いてある。ヘッダーファイルと呼ぶ
// 目次に対応する本文は、〇〇.cppに書いてある
// #include <...>: 最初から付いているライブラリ

// M5StickC Plus2を操作するためのライブラリ
// /Users/user/Documents/Arduino/libraries/M5StickCPlus2
#include <M5StickCPlus2.h>

// 心拍センサーと通信を行うためのライブラリ
// /Users/user/Library/Arduino15/packages/m5stack/hardware/esp32/3.2.1/libraries/Wire/src/Wire.h
#include <Wire.h>

// ESP-NOW通信を行うためのライブラリ
// /Users/user/Library/Arduino15/packages/m5stack/tools/esp32-arduino-libs/idf-release_v5.4-2f7dcd86-v1/esp32s3/include/esp_wifi/include/esp_now.h
#include <esp_now.h>

// ESP-NOWを使うために必要なライブラリ（Wi-Fi機能を操作するライブラリ）
// /Users/user/Library/Arduino15/packages/m5stack/hardware/esp32/3.2.1/libraries/WiFi/src/WiFi.h
#include <WiFi.h>

// #include "...": ライブラリマネージャーで追加したライブラリ

// 心拍センサーを操作するためのライブラリ
// /Users/user/Documents/Arduino/libraries/MAX30100lib/src/MAX30100_PulseOximeter.h
#include "MAX30100_PulseOximeter.h"


// ここからは、データを入れる箱を作っていく
// ここからは、最後に「;」を付ける

// 上でincludeした"MAX30100_PulseOximeter.h"に書かれているPulseOximeterクラスのインスタンスを作る
// インスタンスに名前（pox）をつける
// 名前はなんでもいいけど、poxと付ける人が多いので、真似しておくと、他人が読む時にわかりやすい
PulseOximeter pox;

// 上でincludeしてないけど、Arduinoはいつも自動で""Arduino.h"をincludeしてくれる設定
// /Users/user/Library/Arduino15/packages/m5stack/hardware/esp32/3.2.1/cores/esp32/Arduino.h
// Arduinoが自動でincludeした"Arduino.h"は、さらに”Wstring.h”をincludeしている
// /Users/user/Library/Arduino15/packages/m5stack/hardware/esp32/3.2.1/cores/esp32/Wstring.h
// "Wstring.h"に書かれているStringクラスのインスタンスを作る
// nameにDOKDOKIという文字列を入れる？？: TODO
// String name; name = "DOKIDOKI";としても同じ意味
String name = "DOKIDOKI";

// 上でincludeしてないけど、Arduinoはいつも自動で "Arduino.h" をincludeしてくれる設定
// /Users/user/Library/Arduino15/packages/m5stack/hardware/esp32/3.2.1/cores/esp32/Arduino.h
// Arduinoが自動でincludeした "Arduino.h" は、さらに"stdint.h" をincludeしている
// /Users/user/Library/Arduino15/packages/m5stack/tools/esp-x32/2411/xtensa-esp-elf/include/stdint.h
// "stdint.h" には、uint32_t（32ビットの符号なし整数型）の型定義が書かれている？？: TODO
// String name = "DOKIDOKI";との違いがわからない: TODO
// uint32_t 型?の変数を作り、変数名を lastReport にする
// lastReport に、最初の値として 0 を入れておく
uint32_t lastReport = 0;

// uint8_t 型?の変数を作り、targetAddressという名前のリストにuint8_tの値を6個入れておく
// 0xFF: 16進数?の255
// targetAddressは、送信先のペンライトを指定するためのアドレス
// ふつうは、MACアドレスという「モノの住所」を指定するけど、
// 0xFFが6個入ると、送信先を指定せずに周りのみんなに送る設定になる
// 電波の届く範囲にいるM5stickたちが全員受け取れる
// ブロードキャストという
uint8_t targetAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// struct: いろんな種類のデータをひとまとめにできる箱。構造体という。
// 「ControlDataというデータの箱」をつくる。お弁当箱のイメージ。
// ただし、これは弁当箱に何を入れるかの設計図を作っているところ
// お弁当箱の中に、心拍数の値を入れるための「bpmというデータの箱」を更につくりますよという設計図。
// 「bpmというデータの箱」は、おかず入れのイメージ
// 実際のデータの弁当箱はループの中で毎回つくる
struct ControlData{
  uint16_t bpm;
};

// 書いてないけど、これもstruct
// esp_now.hの中にお弁当箱の内容が書いてあって、そこにstructが書いてある。
// peerは、送信先という意味。
// 送信先の情報を入れておくためのお弁当箱
// peerInfo: そのお弁当箱にpeerInfoという名前を付けましたよ、あとでその名前で呼びますよという意味
esp_now_peer_info_t peerInfo;

// パソコンのプログラムは、「起動して、閉じたら終わり」だけど、
// マイコンは 電源が入っているあいだ中ずっと動き続ける
// だからArduinoでは「プログラムを一度だけ動かす部分」と「ずっと繰り返す部分」が分かれている
// void setup()は、電源を入れたときに1回だけ実行する関数
// coid loop()は、setup()したあとにずっと実行される関数（何回もループして実行する）
// 電源を切ったら、loop()はおしまい
// 電源オン → setup() → loop() → loop() → loo0p()... → 電源オフ
// 1．電源ON
// 2．setup()で M5Stick の準備を整える
// 3．loop()で 1秒ごとに心拍を読み取り、ESP-NOWで送信
// 4．その動作を電源が切れるまでずっと繰り返す

void setup() {

  // 1. Wi-Fiモードを設定
  // Wi-FiモードをWIFI_STA（ステーションモード）に変更する
  // ESP-NOWで無線通信するために必要な設定
  // Wi-Fiの電波を使ってるけど、Wi-Fiのインターネット接続を使ってるわけじゃないから
  // Wifiがつながらないところでも使えるし、Wifiの設定もいらない
  WiFi.mode(WIFI_STA);

  // 2. EPS-Nowを初期化
  // 初期化＝init=はじめますという合図
  // esp_now_init()を実行すると、ESP-Nowの初期化ができる
  esp_now_init();

  // 3. 送信先（ピア）を登録
  // 心拍データを送る相手の情報を、peerInfoに詰めていく
  // 上で書いた、peerInfoというお弁当箱におかず（送信先の情報）をたくさん詰めていくイメージ
  // おかず１: peerInfo.peer_addr（送信先のアドレス）
  // memcpy(A, B, バイト数) : BをAにコピーする。バイト数？
  // 心拍データを送るアドレス（ブロードキャスト）を、peerInfo.peer_addrにコピーする
  // peerInfo.peer_addr = targetAddressだとなぜだめか: TODO
  memcpy(peerInfo.peer_addr, targetAddress, 6);
  // おかず２: peerInfo.channel（送信先にアクセスするチャンネル設定）
  // チャンネルに0を設定すると、チャンネルは特に指定せず、自動にまかせる設定になる
  // まかせたい
  peerInfo.channel = 0;
  // おかず３: peerInfo.encrypt（送信先へにアクセスを暗号化するか）
  // 内緒で送りたいときにはtrueにするけど、ブロードキャストではfalse（暗号化しない）しかない
  peerInfo.encrypt = false;
  // 以上、おかずを３つ詰めたpeerInfo弁当箱を、ESP-NOWに渡す
  // esp_now_add_peer(&お弁当箱)を実行すると、ESP-NOWにお弁当箱を渡せる
  // ＆はなんだ: TODO
  esp_now_add_peer(&peerInfo);

  // 4. M5Stickを動かす
  // M5: M5Stickを動かすための便利関数がたくさん詰まった弁当箱
  // M5.config: M5stickの設定がたくさん入ってる
  // たくさんの設定をcfgという変数に入れる
  // autoは、データ型（listとかdictみたいなもの）はそっちで自動で決めてね、という意味
  auto cfg = M5.config();
  
  // ついにM5stickを起動する！
  // そのときに、上で用意した設定情報を渡す
  StickCP2.begin(cfg);

  // デバッグを開始する
  // まだうまく使えないけど、将来のために書いておく
  Serial.begin(115200);

  // 5. 画面の設定
  // M5.Lcd: M5stickの画面の設定
  // M5.Lcd.setRotation: 画面の向き（横むき、縦向きなど）の設定
  // 0: 0° USBが左
  // 1:	90° USBが下 → これにする
  // 2:	180° USBが右
  // 3:	270° USBが上
  M5.Lcd.setRotation(1);
  // M5.Lcd.setTextFont: フォントの大きさの設定
  // 1:8px, 2:16px, 4:26px, 6:26px, 7:48px, 8:75px
  M5.Lcd.setTextFont(4);
  // M5.Lcd.println: 画面に表示して改行する
  // name = DOKIDOKI
  M5.Lcd.println(name);

  // 6. 心拍センサーを起動する！
  pox.begin();
}

// 大事なことだから、もう一回書く
// パソコンのプログラムは、「起動して、閉じたら終わり」だけど、
// マイコンは 電源が入っているあいだ中ずっと動き続ける
// だからArduinoでは「プログラムを一度だけ動かす部分」と「ずっと繰り返す部分」が分かれている
// void.setup()は、電源を入れたときに1回だけ実行する関数
// void.loop()は、setup()したあとにずっと実行される関数（何回もループして実行する）
// 電源を切ったら、loop()はおしまい
// 電源オン → setup() → loop() → loop() → loo0p()... → 電源オフ
// 1．電源ON
// 2．setup()で M5Stick の準備を整える
// 3．loop()で 1秒ごとに心拍を読み取り、ESP-NOWで送信
// 4．その動作を電源が切れるまでずっと繰り返す

// ずっと動き続けながら、心拍センサーの値を読み取って、
// 1秒ごとにESP-NOWで送信し、画面にも結果を表示する
void loop() {
  // M5.update: M5Stickのボタン・画面・電源状態を更新する関数
  // ・ボタンが押されたか
  // ・電池残量が変わったか
  // ・画面の明るさ調整は必要か
  // ループがまわるたびにチェックしてくれる
  // これがないと、ボタンが押されても反応しなくなるし
  // 電池が減っても表示が更新されない
  M5.update();

  // pox.update: 心拍センサーの情報を入れている弁当箱を更新する
  // 心拍センサーはすごく細かいタイミングでデータを取ってるけど、
  // M5stickから心拍データを更新してくださいとお願いしないと、
  // 弁当箱に入っている情報を更新してくれない
  // 弁当箱には、心拍数の値とか、心拍センサーの状態とか、センサーのいろんな情報のおかずが詰まってる
  pox.update();

  // millis(): M5stickが起動してから、どれくらい時間が経ったかを教えてくれる
  // ミリ秒で教えてくれる
  // 1秒 → 1000ミリ秒、5秒 → 5000ミリ秒
  // lastReportは、上で定義した変数
  // 最初は0が入っている
  // loop()の最後で、その時の経過時間を上書き保存する
  // if (millis() - lastReport > 1000): 前に心拍データを送信してから、もし1秒以上経っていたら
  if (millis() - lastReport > 1000) {
    // 心拍センサーの弁当箱に入っている心拍数のデータを、bpmという変数に受け取る
    float bpm = pox.getHeartRate();

    // BPMがゼロ以外のとき、という条件
    // 心拍センサーから指が離れちゃったときとかは、送信しない
    if (bpm > 0) {
      // M5stickの画面に表示するメッセージをつくる
      String message = "BPM: " + String(int(bpm));

      // 送信するデータの弁当箱を作る
      // 上で書いた弁当箱の設計図に沿った内容にする
      ControlData data;
      data.bpm = (uint16_t)bpm;

      // esp_now_send(A, B, C)
      // Aのアドレス（ブロードキャスト）に、Bを、Cのバイト数？で送る
      // これでESP-NOWが発射される！
      esp_now_send(targetAddress, (uint8_t*)&data, sizeof(data));

      // 黒い四角形を書く→黒で塗りつぶす
      // fillRect(x1, y1, x2, y2, 塗りつぶす色)
      // x1, y1: 四角形の左上の座標
      // x2, y2: 四角形の右下の座標
      M5.Lcd.fillRect(0, 53, 135, 80, BLACK);

      // M5stickの画面に目に見えないカーソルをもっていく
      // 画面の上の方にはDOKIDOKIって書いてあるから、その下の行
      M5.Lcd.setCursor(0, 53);  // 53: 26px*2+1px
      // 目に見えないカーソルの位置（DOKIDOKIの下）に
      // 残りのバッテリー数を表示する
      // 毎回黒い四角形で塗りつぶしてから表示する（前回の値を消して、表示する）
      // 例: 4100mV (92%)
      M5.Lcd.print(StickCP2.Power.getBatteryVoltage());
      M5.Lcd.print("mV (");
      M5.Lcd.print(StickCP2.Power.getBatteryLevel());
      M5.Lcd.println("%)");

      // さっきつくったメッセージを表示する
      M5.Lcd.print(message);
    }
    // 経過時間を保存する
    lastReport = millis();
  }
}
