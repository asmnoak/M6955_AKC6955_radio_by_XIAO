<p><H3>M6955(AKC6955)によるオールバンドラジオ</H3></p>
<p>
M6955(AKC6955)は、FM/TV/MW/SW/LWオールバンド対応のDSPラジオICである。I2Cインターフェースでコントロールできる。Arduinoとの組み合わせが一般的であるが<br>
ここでは、Seeeduino XIAOを使用した。データシートは、<a href="https://www.aitendo.com/product/7012">こちら（DSPラジオモジュール）</a>を参考にするとよい。<br>
なお、チップ自体はAliExpressから入手可能である（SOP-24パッケージなので、かろうじてハンダ付け可能）。<br>
</p>

<p><strong>特長</strong><br>
 ・タクトスイッチ１にてFM/MW/SW/LWのバンド切り替えが可能（TVは未サポート）。<br>
 ・タクトスイッチ２にて各バンド内の受信局（周波数をハードコードしておく）切り替えが可能。<br>
 ・ロータリーエンコーダにて受信周波数の変更が可能。<br>
 ・ボリュームによる音量調節が可能。<br>
 ・受信バンドと周波数をOLEDに表示可能。<br>
</p>
<p><strong>H/W構成</strong><br>
 ・Seeeduino XIAO - コントローラ<br>
 ・SD1306 128x64 OLED表示装置（オプション）<br>
 ・タクトスイッチ１、タクトスイッチ２<br>
 ・ボリューム（50KΩ）<br>
 ・ロータリーエンコーダ<br>
 ・Xtal発振器（32768Hz）、コンデンサ、抵抗類（データシート参照）
</p>
<p><strong>Seeeduino XIAOについて</strong><br>
500円玉サイズのコンパクトなArduinoである。以下を参考にArduino IDEに設定を行うことで利用できる。<br>
<a href="https://wiki.seeedstudio.com/jp/Seeeduino-XIAO/">XIAOの設定方法</a>
</p>
<p>
<img src="./blob/main/m6955_radio1.jpg" width="480" height="360">
FM 80.4MHzを受信中
</p>
<p><strong>操作</strong><br>
 ・FM/MW/SW/LWのバンド切り替え（タクトスイッチ）。<br>
 ・プリセットしたラジオ局又は受信周波数の切り替え（タクトスイッチ）。<br>
 ・音量調整（ボリューム）。<br>
 ・受信周波数の変更（ロータリーエンコーダ）。なお、変更のステップはバンドごとに決まっている。<br>

</p>
<p><strong>接続</strong><br>
<p>各コンポーネントの接続は以下の通り。<br>
</p>
<p>
I2C接続&nbsp; M6955と表示装置（マルチドロップで接続）
※XIAO内蔵のプルアップ機能を利用しているのでプルアップ抵抗は不要
<table> 
<tr>
<td>I2C&nbsp;</td><td>XIAO</td>
</tr>
<tr>
<td>VCC</td><td>3V3</td>
</tr>
<tr>
<td>GND</td><td>GND</td>
</tr>
<tr>
<td>SCK</td><td>D5</td>
<tr>
<tr>
<td>SDA</td><td>D4</td>
<tr>
</table>
</p>
<p>
タクトスイッチ（ボタン）
<table> 
<tr>
<td>ボタン&nbsp;</td><td>XIAO&nbsp;</td><td>機能</td>
</tr>
<tr>
<td>BAND</td><td>D2</td><td>バンドを切り替え</td>
<tr>
<tr>
<td>STATION</td><td>D3</td><td>ラジオ局を「次へ」切り替え</td>
<tr>
<tr>
<td>予備</td><td>D1</td></td><td>現状はホーム局へ移動</td>
</tr>
</table>
</p>
<p>
ロータリーエンコーダ
<table> 
<tr>
<td>ロータリーエンコーダ&nbsp;</td><td>XIAO</td>
</tr>
<tr>
<td>+</td><td>3V3</td>
</tr>
<tr>
<td>GND</td><td>GND</td>
</tr>
<tr>
<td>DT</td><td>D6</td>
<tr>
<tr>
<td>CLK</td><td>D7</td>
<tr>
</table>
</p>
</p>
<p><strong>インストール</strong><br>
<ol>
<li>コードを、ZIP形式でダウンロード</li>
<li>ArduinoIDEにおいてライブラリを登録</li>
<li>ArduinoIDEからm6955_rot_oled_xiao_master.inoを開く</li>
<li>「検証・コンパイル」に成功したら、一旦、「名前を付けて保存」を行う<br>
（ライブラリが不足のエラーが出たら、追加する。例えば、SD1306 OLED表示装置）
</li>
<li>上に述べたH/W構成を用意し、配線を接続する</li>
<li>PCとUSBケーブルで接続する。適当なCOM番号で認識される（USBのドライバが必要な場合あり）</li>
<li>「マイコンボードに書き込む」を行う</li>
<li>書き込みが成功すれば、表示装置にFM局受信が表示される（正しく受信するためには、FM局の受信周波数を変更する必要がある）</li>
<li>なお、動作の状況がシリアルモニタに表示される</li>
</li>
</ol>
</p>
<p>
<p><strong>若干の解説</strong><br>
・受信バンドmode_setについては、Reg1に設定する。<br>
・受信周波数listen_freqはチャンネル番号に変換して、Reg2に設定する。バンドごとの計算式がデータシートに示されている。<br>
・各タクトスイッチについては、割り込み機能を利用している。例&nbsp;void mode_setting()<br>
</p>
<p>
<img src="./m6955_radio2.jpg" width="400" height="400">
基板上の配置の様子。右がXIAO、左がM6955。電源はAMS1117-3.3により3.3Vに変換している。
</p>
<p><strong>注意事項</strong><br>
・「アプリケーションノート」がないので、スケッチには試行錯誤が含まれている。<br>
・AMの受信にはバーアンテナを接続する必要があるが、XIAOとの位置関係で電波干渉の影響がかなりあるので要注意（同じケースに入れるのは無理）。<br>
・音声は「ステレオ」に設定しているので、Lチャネル、Rチャネル夫々にスピーカー（イヤホン）を接続するが<strong>100μFから200μF程度のコンデンサを介して</strong><br>
　GNDに落とす（スピーカーをGNDに直接接続するとM6955が壊れるので注意）。<br>
　モノラルの場合は、スケッチを「逆相」（Reg6）に変更する必要がある。この時はLチャネル、Rチャネル間にスピーカーを接続する。<br>
・利用の際は、自己責任でお楽しみください。</p>
</p>
