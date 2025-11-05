// Arduino言語では、コメントは//のあとに入れる（Pythonでは#）
// #include:「この名前のファイルの内容を、ここにまるごと読み込んでください」という命令
// ライブラリ: 他の誰かが書いた便利なプログラムを使わせてもらう
// 図書館（ライブラリ）で本を借りてきて、自分で描いている小説の中でまるまるパクっちゃうイメージ
// 〇〇.h: 〇〇は、本のタイトル. 〇〇.hファイルには、目次が書いてある。ヘッダーファイルと呼ぶ
// 目次に対応する本文は、〇〇.cppに書いてある

// M5StickC Plus2を操作するためのライブラリ
// /Users/user/Documents/Arduino/libraries/M5StickCPlus2
#include <M5StickCPlus2.h>

// Groveと通信を行うためのライブラリ
// /Users/user/Library/Arduino15/packages/m5stack/hardware/esp32/3.2.1/libraries/Wire/src/Wire.h
#include <Wire.h>

// ESP-NOW通信を行うためのライブラリ
// /Users/user/Library/Arduino15/packages/m5stack/tools/esp32-arduino-libs/idf-release_v5.4-2f7dcd86-v1/esp32s3/include/esp_wifi/include/esp_now.h
#include <esp_now.h>

// ESP-NOWを使うために必要なライブラリ（Wi-Fi機能を操作するライブラリ）
// /Users/user/Library/Arduino15/packages/m5stack/hardware/esp32/3.2.1/libraries/WiFi/src/WiFi.h
#include <WiFi.h>

// LEDテープを操作するためのライブラリ
#include <FastLED.h>

// LED設定

// LEDテープが接続されている場所
// LEDテープはGroveポートで接続されている
// M5StickC Plus2のGroveは32と設定する
#define LED_PIN     32

// LEDテープについているLEDの粒の数
// [] ==== [] ◻︎1 ◻︎2 ◻︎3 ◻︎4 ◻︎5 ◻︎6 ◻︎7 ◻︎8 ◻︎9 ◻︎10 ◻︎11 ◻︎12 ◻︎13 ◻︎14 ◻︎15 [] <- groveポート
#define NUM_LEDS    15

// ここからは、データを入れる箱を作っていく
// ここからは、最後に「;」を付ける
// 上のdefineとは何が違うんだ: TODO

// LEDの状態を覚えておくためのリスト（FastLEDが使う）
// CRGB：FastLEDライブラリの「1つのLEDの色」を表す型。
// R（赤）, G（緑）, B（青）の3成分をセットで持つ。
// CRGB(255, 0,   0) → 赤
// CRGB(0,   0, 255) → 青
// CRGB(255, 0, 255) → ピンクっぽい紫: https://convertingcolors.com/rgb-color-255_0_255.html
// CRGB(180, 0, 255) → 紫らしい紫: https://convertingcolors.com/rgb-color-180_0_255.html
// CRGB(255, 0, 180) → ピンク: https://convertingcolors.com/rgb-color-255_0_180.html
// CRGB(255, 255, 250)→白
// CRGB::Blue→あらかじめ名前がつけられている色もある
// これに対し、CHSVは色相（Hue）、彩度（Saturation）、明度（Value）で色を表現する
// hue: 色相。0〜255の範囲で、赤(0)、緑(85)、青(170)などの色を表す
// saturation: 彩度。0(無彩色)〜255(鮮やか)の範囲
// value: 明度。0(黒)〜255(明るい)の範囲
// CHSVを使うと、虹色のグラデーションが作れる！: TODO
// leds: リストの名前
// leds[n]: n個のCRGBが入ったリストを作りますよという意味
// #define NUM_LEDS    15
// CRGB leds[15]: 15個のCRCBが入ったリストを作りますよという意味
// つまり、LEDの粒それぞれの色の状態をこのリストで覚えておくことができる
CRGB leds[NUM_LEDS];

// ペンライトにつけるM5stickの名前
// String: 文字列
// String name; name = "KIRAKIRA";と同じ意味
String name = "KIRAKIRA";

// float: 小数
// 受信した心拍数（beats per minute）
// あらかじめ0.0を入れておく
float bpm = 0.0;

// float: 小数
// 前回受診した心拍数（変化があったときだけ画面を更新する）
// あらかじめ0.0を入れておく
float lastBpm = 0.0;

//　unsigned: 正だけ　long: intより大きな整数
// uint32_tとかと何が違うんだろう: TODO
// 最後にLEDが点滅した時間
// あらかじめ0を入れておく
// 0.0じゃないのは、これは整数longだから
unsigned long lastBlink = 0;

// bool: trueかfalseの2値だけをとる型
// 現在LEDが点灯中かどうかをおぼえておく
// あらかじめfalseを入れておく
bool ledOn = false;                

// === 関数: BPMに応じたLEDの色を返す ===
// uint16_t, uint8_t
// u: unsigned→正の数だけ扱う
// int: integer→整数（Pythonと同じ）
// 16: 16ビット（2バイト）→ 2の16乗（0〜約6万）までの整数を扱える（1バイト🟰8ビット）
// 8: 8ビット（1バイト）→ 2の8乗（0〜255）までの整数を扱える（1バイト🟰8ビット）
// tは無視していいらしい
// ボードごとに整数の扱える範囲が違うので、ちゃんとサイズを指定するといい
// 最初にCRBGとなるのは、この関数はCRGBを返しますよという意味
// ちなみにvoidは何も返しませんよという意味
// bpmToColor: この関数の名前
// uint16_t bpm: この関数が受け取る数→uint16_t型のbpmという名前の変数を受け取りますよ〜
CRGB bpmToColor(uint16_t bpm) {
  // bpmが70より少ない時は、青を返しますよ〜
  if (bpm < 70) {
    return CRGB::Blue;
  // bpmが70より多くて100より少ない時は、bpmに応じて緑→黄色→赤の色を返しますよ〜
  } else if (bpm < 100) {
    // bpmが70のときratioは0.0、100のときratioは1.0
    // bpm70: (70 - 70) / 30.0 = 0 / 30.0 = 0.0
    // bpm85: (85 - 70) / 30.0 = 15 / 30.0 = 0.5
    // bpm100: (100 - 70) / 30.0 = 30 / 30.0 = 1.0
    // かしこい！こうやって0〜1に変換することを「正規化（normalization）」という: TODO
    float ratio = (bpm - 70) / 30.0;
    uint8_t r = ratio * 255;  // bpmが70のとき赤0、100のとき赤255、つまりだんだん赤が強く光る
    uint8_t g = 255;  // bpmによらず緑は精一杯光る
    uint8_t b = (1.0 - ratio) * 255;  // bpmが70のとき青255、100のとき青0、つまりだんだん青が弱くなる
    return CRGB(r, g, b);
  } else {
    // bpmが100以上のときは虹色を返しますよ〜
    return CHSV(millis() / 10 % 255, 255, 255);
  }
}

// struct: いろんな種類のデータをひとまとめにできる箱。構造体という。
// 「ControlDataというデータの箱」をつくる。お弁当箱のイメージ。
// ただし、これは弁当箱に何を入れるかの設計図を作っているところ
// お弁当箱の中に、心拍数の値を入れるための「bpmというデータの箱」を更につくりますよという設計図。
// 「bpmというデータの箱」は、おかず入れのイメージ
// 実際のデータの弁当箱はループの中で毎回つくる
// 心拍数は整数で送られてくる
struct ControlData {
  uint16_t bpm;
};

// === 受信データ格納用の変数 ===
// 初期値は0
// .bpmのドットは、pixel_settings.bpmという意味
// pixel_settings弁当箱のbpmおかず入れですよという意味
// ControlData pixel_settings; pixel_settings.bpm = 0;と同じ意味
// 同じならそう書けば良いのでは…？: TODO
ControlData pixel_settings = { .bpm = 0 };

// === ESP-NOW 受信時に呼び出される関数（コールバック） ===
// ESP-NOW は、データを受け取った瞬間に「ねえ今データ届いたよ！」と教えてくれる。
// esp_now_register_recv_cb(関数名);
// カッコの中にデータを受け取った瞬間に実行して欲しい関数を書いておくと自動でやってくれる
// この実行して欲しい関数を「コールバック関数」と呼ぶ。
// 自分で何度もチェックしに行かなくても、自動で呼んでもらえる
// const esp_now_recv_info_t *info  
//  const: 一度データを入れたらもう変更しないよという意味
//  esp_now_recv_info_t: 受信情報を表す構造体の型?: TODO
//  *: ポインタ?: TODO
// infoは使わないけどなぜかく… : TODO
// const uint8_t *data	受け取った実際のデータ（バイト列）
// const: 一度データを入れたらもう変更しないよという意味
// uint8_t: 8ビット（1バイト）→ 2の8乗（0〜255）までの整数を扱える（1バイト🟰8ビット）
// int len
// int: 整数（Pythonと同じ）
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  // 受け取ったデータの長さ (len) が、
  // 自分が想定している構造体のサイズ (sizeof(ControlData)) と同じか確認してる。
  // 違ったらどうなるのか: TODO
  if (len == sizeof(ControlData)) {
    // memcpy(A, B, バイト数) : BをAにコピーする。バイト数？: TODO
    // pixel_settingsを、dataにコピーする
    // pixel_settings=dataだとなぜだめか: TODO
    memcpy(&pixel_settings, data, len);
    // おかずのbpmとふつうのbpm?: TODO
    bpm = pixel_settings.bpm;
  }
}

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
  // M5stickを起動する
  // M5: M5Stickを動かすための便利関数がたくさん詰まった弁当箱
  // M5.config: M5stickの設定がたくさん入ってる
  // たくさんの設定をcfgという変数に入れる
  // autoは、データ型（listとかdictみたいなもの）はそっちで自動で決めてね、という意味
  auto cfg = M5.config();
  // ついにM5stickを起動する！
  // そのときに、上で用意した設定情報を渡す
  //M5.begin();だとうまくいかない
  StickCP2.begin(cfg);

  // デバッグを開始する
  // まだうまく使えないけど、将来のために書いておく
  Serial.begin(115200);

  // M5.Lcd: M5stickの画面の設定
  // M5.Lcd.setRotation: 画面の向き（横むき、縦向きなど）の設定
  // 0: 0° USBが左
  // 1:	90° USBが下
  // 2:	180° USBが右
  // 3:	270° USBが上 → これにする
  M5.Lcd.setRotation(3);
  // M5.Lcd.setTextFont: フォントの大きさの設定
  // 1:8px, 2:16px, 4:26px, 6:26px, 7:48px, 8:75px
  M5.Lcd.setTextFont(4);
  // M5.Lcd.println: 画面に表示して改行する
  // name = KIRAKIRA
  M5.Lcd.println(name);

  // LEDテープの初期化
  // 初期化＝init=はじめますという合図
  // WS2812Bという種類のLEDを、LED_PINに接続したNUM_LEDS個のLEDとして、leds[]リストに対応づけてね！
  // FastLED:ライブラリ名
  // FastLEDライブラリには、たくさんの便利関数が詰まってるけど、その中の addLeds<>() という関数を呼び出している。
  // .addLeds<...>(): テンプレート関数: TODO
  // addLedsで、「このLEDテープを使います！」という宣言をする
  // WS2812B: LEDの種類を指定
  // LED_PIN: 信号線のピンを指定
  // GRB: 色の順番（R/G/Bの並び）を指定する。LEDの種類によって「色の並び順（RGB / GRB / BRG）」が違う。
  // (leds, NUM_LEDS): ledsリストのデータと本物のLEDを対応させる
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  
  // 内部のLEDデータ配列（leds[]）を全部「黒（0,0,0）」にする
  FastLED.clear();

  // leds[] に今入っているデータを、実際のLEDテープに送る
  FastLED.show();

  // LED全体の明るさ（輝度）を0〜255の範囲で調整する
　// 電池がすぐになくなっちゃうので50に設定する
  FastLED.setBrightness(50);

  // Wi-Fiモードを設定
  // Wi-FiモードをWIFI_STA（ステーションモード）に変更する
  // ESP-NOWで無線通信するために必要な設定
  // Wi-Fiの電波を使ってるけど、Wi-Fiのインターネット接続を使ってるわけじゃないから
  // Wifiがつながらないところでも使えるし、Wifiの設定もいらない
  WiFi.mode(WIFI_STA);

  // EPS-Nowを初期化
  // 初期化＝init=はじめますという合図
  // esp_now_init()を実行すると、ESP-Nowの初期化ができる
  esp_now_init();

  // 心拍データを受信した後に実行されるコールバック登録
  esp_now_register_recv_cb(OnDataRecv);
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

// ずっと動き続けながら、心拍センサーの値を受け取って、
// 1秒ごとにLEDテープの設定を変更する
void loop() {
  // M5.update: M5Stickのボタン・画面・電源状態を更新する関数
  // ・ボタンが押されたか
  // ・電池残量が変わったか
  // ・画面の明るさ調整は必要か
  // ループがまわるたびにチェックしてくれる
  // これがないと、ボタンが押されても反応しなくなるし
  // 電池が減っても表示が更新されない
  M5.update();

  // bpmの値が0じゃなくて、かつ前回の値とちがうときだけ、画面を書き直す
  if (bpm > 0 && bpm != lastBpm) {
    // lastBpmには、最初0.0が入っている
    // ループが回りだしたら、lastBpmにはbpmの値を入れる
    // bpmには、最初0が入っている
    // ループが回りだしたら、bpmには受信した心拍数が入る
    lastBpm = bpm;

    // M5stickの画面に黒い四角形を書く→黒で塗りつぶす
    // fillRect(x1, y1, x2, y2, 塗りつぶす色)
    // x1, y1: 四角形の左上の座標
    // x2, y2: 四角形の右下の座標
    M5.Lcd.fillRect(0, 80, 135, 80, BLACK);

    // 上で塗りつぶした黒い四角形の左上に、目に見えないカーソルをもっていく
    // 画面の上の方にはKIRAKIRAって書いてあるから、その下の行
    M5.Lcd.setCursor(0, 80);

    // 目に見えないカーソルの位置（KIRAKIRAの下）に
    // 残りのバッテリー数を表示する
    // 毎回黒い四角形で塗りつぶしてから表示する（前回の値を消して、表示する）
    // 例: 4100mV (92%)
    M5.Lcd.print(StickCP2.Power.getBatteryVoltage());
    M5.Lcd.print("mV (");
    M5.Lcd.print(StickCP2.Power.getBatteryLevel());
    M5.Lcd.println("%)");
    M5.Lcd.printf("BPM: %3d", (int)bpm);     // 整数としてBPM表示（桁がずれないように）
  }

  // BPMがゼロ以外のとき、という条件
  // 心拍センサーから指が離れちゃったときとかは、送信しない
  if (bpm > 0) {
    // 点滅するタイミング（1拍あたりの時間）を計算
    // ミリビョウが基本
    // bpm: beat per minute: 1分あたりの拍数
    // 60bpm: 1分あたり60回心臓がドキドキする、つまり1秒あたり1回ドキドキする
    // interval = 60000 / bpm / 2
    // 60000: 1分＝60秒＝60000ミリ秒
    // 60000 / bpm: 1拍の長さ（ms）
    // 60bpmの場合、60000 / 60: 1拍あたり1000ミリ秒(1秒) ドキドキ(1+1=2秒)
    // 120bpmの場合、60000 / 120: 1拍あたり500ミリ秒(0.5秒)　ドキドキドキドキ！(0.5+0.5+0.5+0.5=2秒)
    // 2で割るのは、点滅ONと点滅OFFの両方を含むから
    // 60bpmの場合、60000 / 60 / 2: interval=500ミリ秒(0.5秒) on+off+on+off(0.5+0.5+0.5+0.5=2秒)
    // 120bpmの場合、60000 / 120 / 2: 1拍あたり250ミリ秒(0.25秒)　on+off+on+off+on+off+on+off+！(0.25+0.25+0.25+0.25+0.25+0.25+0.25+0.25=2秒)
    // まとめ
    // bpm:60 	1拍あたり:1000ミリ秒(1秒)	    休む時間:500ミリ秒(0.5秒)	  1秒に2回明滅
    // bpm:120	1拍あたり:500msミリ秒(0.5秒) 	休む時間:250ミリ秒(0.25秒)	0.25秒ごとに点滅
    // bpm:180	1拍あたり:333msミリ秒(0.3秒)  休む時間:166ミリ秒(0.166秒)	速く点滅
    unsigned long interval = 60000 / bpm / 2;
    
    // 前回点滅してから、もう intervalミリ秒たったかどうかををチェックする
    // intervalミリ秒以下だったら、何もせずに次のループへ
    if (millis() - lastBlink > interval) {
    
      // 点滅ON/OFFを切り替える
      // ledOnには最初falseが入っている
      // ledOnはbool型だから、trueかfalseしか入らない
      // 「!ledOn」は「ledOnの反対」
      // プログラミングは、右辺の値を左辺の変数にセットするのが基本
      // ledOnの値がfalseなら変数ledOnにtrueをセット、ledOnの値がtrueなら変数ledOnにfalseをセット
      // つまり、点灯中（true）なら消灯（false）、消灯中（false）なら点灯（true）に切り替える
      ledOn = !ledOn;

      // 現経過時刻を覚えておく
      lastBlink = millis();
      
      // 現在のbpmに応じた色を変数colorに入れる
      // bpmToColorはCRGBを返す関数だから、変数colorはCRGB型
      CRGB color = bpmToColor(bpm);

      // forの中に書かれていることを、光の粒の数（NUM_LEDS=15）だけ繰り返す
      // int i = 0: 0から始まって
      // i++: ひとつづつ増やして
      // i < NUM_LEDS: i < NUM_LEDSであるかぎり続ける（falseになったらやめる）
      // 1回目:  i=0 （i=0 < NUM_LEDS=15　→　true）
      // 2回目:  i=1 （i=1 < NUM_LEDS=15　→　true）
      // 3回目:  i=2 （i=1 < NUM_LEDS=15　→　true）
      // 4回目:  i=3 （i=1 < NUM_LEDS=15　→　true）
      // 5回目:  i=4 （i=1 < NUM_LEDS=15　→　true）
      // 6回目:  i=5 （i=1 < NUM_LEDS=15　→　true）
      // 7回目:  i=6 （i=1 < NUM_LEDS=15　→　true）
      // 8回目:  i=7 （i=1 < NUM_LEDS=15　→　true）
      // 9回目:  i=8 （i=1 < NUM_LEDS=15　→　true）
      // 10回目: i=9 （i=1 < NUM_LEDS=15　→　true）
      // 11回目: i=10（i=1 < NUM_LEDS=15　→　true）
      // 12回目: i=11（i=1 < NUM_LEDS=15　→　true）
      // 13回目: i=12（i=1 < NUM_LEDS=15　→　true）
      // 14回目: i=13（i=1 < NUM_LEDS=15　→　true） 
      // 15回目: i=14（i=1 < NUM_LEDS=15　→　true）
      // 16回目: i=15（i=1 < NUM_LEDS=15　→　false!）→ 繰り返し終了
      for (int i = 0; i < NUM_LEDS; i++) {
        
        // leds[i]: LEDテープの前からi番目の粒に「色」を設定する
        // ledOnがTrueならcolorに入ってる色、ledOnがfalseなら黒=消灯
        // 三項演算子（さんこうえんざんし）: TODO
        // 条件 ?　値A : 値B
        // 条件がtrueなら値Aを、falseなら値Bを返す
        // ledOnがtrueならcolorを、falseならCRGB::Black（黒=消灯）を返す
        // Python: leds[i] = color if ledOn else CRGB.Black
        // Python: leds[i] = color if ledOn == True else CRGB.Black?
        // Python: leds[i] = color if ledOn is True else CRGB.Black?
        leds[i] = ledOn ? color : CRGB::Black;
      }

      // FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
      // 上記でFastLEDとledsはつなげてある
      // FastLED.show()を実行すると、ledsリストの色にLEDテープを光らせることができる

      // leds: [赤, 赤, 赤, 赤, 赤, 赤, 赤, 赤, 赤, 赤, 赤, 赤, 赤, 赤, 赤]
      // ↓
      // FastLED.show()
      // ↓
      // LEDテープ: [] ==== [] 赤 赤 赤 赤 赤 赤 赤 赤　赤 赤 赤 赤 赤 赤 赤 []

      // leds: [黒, 黒, 黒, 黒, 黒, 黒, 黒, 黒, 黒, 黒, 黒, 黒, 黒, 黒, 黒]
      // ↓
      // FastLED.show()
      // ↓
      // LEDテープ: [] ==== [] 消 消 消 消 消 消 消 消 消 消 消 消 消 消 消 []
      FastLED.show();
    }
  }

  // 1ミリ秒（0.001秒）だけプログラムを止める
  // ループ処理（loop()）の中での動きが一時停止する
  // タイマーはちゃんと動き続ける
  // FastLED.show()をして、すぐに次のFastLED.show()をすると、
  // LEDテープの色を変えてる間に次の命令をおくっちゃうかもしれない
  // if (millis() - lastReport > 1000) {
  // DOKIDOKIの上記と何が違う？: TODO
  delay(1);
}
