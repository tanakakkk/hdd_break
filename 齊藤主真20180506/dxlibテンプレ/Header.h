//---------------------- 構造体 ----------------------

// 矩形
struct Rect {
	double x;					// 左上のx座標
	double y;					// 左上のy座標
	double width;				// 幅
	double height;				// 高さ
};

// キャラクター
struct Charactor {
	Rect rect;					// 座標と幅
	int state;					// 状態
	int explosionCounter;		// hdd破損用カウンタ
};


// ------------- 関数のプロトタイプ宣言 -------------

// 初期化
void init();							// 初期化処理
void initSound();                       //曲初期化
void initGraphic();						// 画像の読み込み
void initHdd();							// HDDの初期化
void initOs();							// OSの初期化

// 入力
void input();							// 入力処理

// 更新
void update();							// 更新処理
void updateHdd();						// HDDの更新処理
void updateOs();						// OSの更新処理

// 描画
void draw();							// 描画処理
void drawTitle();						// タイトルの描画
void drawExplanation();					//説明の描画
void drawGameOver();					// ゲームオーバーの描画
void drawGameClear();					// ゲームクリアの描画
void drawHdd();							// HDDの描画
void drawOs();							// OSの描画
