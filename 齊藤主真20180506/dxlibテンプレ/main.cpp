#include "DxLib.h"
#include "Header.h"
// ------------------- パラメータ -------------------

#define WINDOW_W 640			// 画面の幅
#define WINDOW_H 480			// 画面の高さ
#define HDD_MAX 5200			// HDD回転数
#define HDD_MIN 0				// HDD最低回転数
#define HDD_PERMISSIBLE 60		// HDD回転許容範囲
#define OS_TIME 1280			// OSの起動に必要な時間
#define HDD_SPIN 60				// スペースキー１回分のスピン量


#define OS_STOP 0				//OS未起動
#define OS_START 1				//OS起動中

#define HDD_STOP 0				// HDD動いてない
#define HDD_START 1				// HDD動いてる
#define HDD_CAUTION 2			//回し過ぎ警告
#define DEAD 3					// 死亡

#define TITLE 0					// タイトル
#define EXPLANATION 1			//説明画面
#define GAME 2					// ゲーム
#define GAMEOVER 3				// ゲームオーバー
#define GAMECLEAR 4				// ゲームクリア


// ---------------------- HDD ----------------------

Charactor hdd;							// HDD
Charactor os;							// OS


// ---------------------- 画像 ----------------------

int hddGraph;					// hddの画像
int handGraph;					// osの画像
int bombGraph;					// 爆発画像
int gameoverGraph;				//ゲームオーバー画面
int titleGraph;					//普通にタイトル画像
int gamingGraph;				//ゲーム画像

//-----------------------音楽-----------------------

int mainbgm;                    //音楽

//-----------------------カウント-------------------

int rpm = 0, osstarting = 0;	//ゲーム内カウント


// ---------------------- 状態 ----------------------

int mainState;					// ゲームの状態
char beforeSpaceKey;			// 前のタイミングのSPACEキーの状態を格納
char beforeRKey;				// 前のタイミングのRキーの状態を格納
char keyBuffer[256];			// キーボードの状態を格納


// ---------------------------------------- main関数 ----------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	// ウィンドウモードで起動
	ChangeWindowMode(TRUE);

	// ウィンドウのタイトルを指定する
	SetMainWindowText("hddbreak");

	// DXライブラリの初期化を行う
	if (DxLib_Init() == -1) {
		return -1;
	}

	// 画面モードの変更
	SetGraphMode(WINDOW_W, WINDOW_H, 32);

	// 描画先を裏画面にする
	SetDrawScreen(DX_SCREEN_BACK);

	// ゲームの初期化処理
	init();

	// 1秒に約60回画面を更新
	int nextTime = GetNowCount() + 1000 / 60;

	// Escキーを押すまでループする
	while (ProcessMessage() == 0 && keyBuffer[KEY_INPUT_ESCAPE] == 0) {
		input(); //入力状態処理
		update();	// 更新処理
		draw();		// 描画処理

		// 時間が余った場合、sleepする
		if (nextTime > GetNowCount()) {
			Sleep(nextTime - GetNowCount());
		}
		nextTime = GetNowCount() + 1000 / 60;
	}

	// DXライブラリ使用の終了処理
	DxLib_End();

	// ソフトの終了
		return 0;
}
//--------------------------------------初期化処理--------------------------------------
void init(){
	mainState = TITLE;

	//上から順番に初期化処理を行う
	initGraphic();
	initSound();
	initHdd();
	initOs();

}

//画像の読み込み
void initGraphic(){
	hddGraph = LoadGraph("bmp/HDD.png");
	bombGraph = LoadGraph("bmp/fire.png");
	titleGraph = LoadGraph("bmp/HDD.png");
	handGraph = LoadGraph("bmp/hand.png");
	gameoverGraph = LoadGraph("bmp/diskcheck.png");
	gamingGraph = LoadGraph("bmp/bios.jpg");
}

//音声読み込み
void initSound(){
	mainbgm = LoadSoundMem("wav/hddbroken2.wav");
}

//HDDの初期化
void initHdd(){
	hdd.state = HDD_STOP;
	hdd.rect.x = 380;
	hdd.rect.y = 300;
	hdd.rect.width = 114;
	hdd.rect.height = 185;
}

//OSの初期化
void initOs(){
	os.state = OS_STOP;
	os.rect.x = 0;
	os.rect.y = 0;
	os.rect.width = 372;
	os.rect.height = 279;
}

//---------------------------------------入力処理---------------------------------------
void input() {
	beforeSpaceKey = keyBuffer[KEY_INPUT_SPACE];
	beforeRKey = keyBuffer[KEY_INPUT_R];
	GetHitKeyStateAll(keyBuffer);
}

//-------------------------------------更新処理----------------------------------------

void update() {
	switch (mainState) {
	case TITLE:
		// spaceキーを押した瞬間EXPLANATIONに移動
		if (beforeSpaceKey == 1 && keyBuffer[KEY_INPUT_SPACE] == 0) {
			mainState = EXPLANATION;
		}
		break;

	case EXPLANATION:
		// spaceキーを押した瞬間GAMEに移動
		if (beforeSpaceKey == 1 && keyBuffer[KEY_INPUT_SPACE] == 0) {
			mainState = GAME;
		}
		break;

	case GAME:
		// ゲームの更新処理を行う
		updateHdd();
		updateOs();
		break;

	case GAMEOVER:
		// Rキーを押した瞬間最初に戻る
		if (beforeRKey == 1 && keyBuffer[KEY_INPUT_R] == 0) {
			init();
		}
		break;

	case GAMECLEAR:
		// Rキーを押した瞬間最初に戻る
		if (beforeRKey == 1 && keyBuffer[KEY_INPUT_R] == 0) {
			init();
		}
		break;
	}
}

// HDDの更新処理
void updateHdd(){
	switch (hdd.state){
	case HDD_STOP:
		rpm = 0;
		//スペースを押したら回転数を上げてHDDを起動状態にする
		if (beforeSpaceKey == 1 && keyBuffer[KEY_INPUT_SPACE] == 0) {
			rpm += HDD_SPIN;
			hdd.state = HDD_START;
		}
		if (keyBuffer[KEY_INPUT_SPACE] == 1) {
			hdd.rect.x = 410;
			hdd.rect.y = 270;
		}
		if (keyBuffer[KEY_INPUT_SPACE] == 0) {
			hdd.rect.x = 380;
			hdd.rect.y = 300;
		}
		break;

	case HDD_START:
		rpm--;
		//摩擦でrpmは下がっていくがスペースを連打すれば回転数を上げられる
		if (beforeSpaceKey == 1 && keyBuffer[KEY_INPUT_SPACE] == 0) {
			rpm += HDD_SPIN;
		}
		if (keyBuffer[KEY_INPUT_SPACE] == 1) {
			hdd.rect.x = 410;
			hdd.rect.y = 270;

		}
		if (keyBuffer[KEY_INPUT_SPACE] == 0) {
			hdd.rect.x = 380;
			hdd.rect.y = 300;

		}

		//rpmが０になったらHDDがとまっている状態にする
		if (rpm <= HDD_MIN){
			hdd.state = HDD_STOP;
		}
		//5140~5260rpmになったらosが起動
		if (rpm < HDD_MAX + HDD_PERMISSIBLE && rpm > HDD_MAX - HDD_PERMISSIBLE){
			os.state = OS_START;
		}
		//5140以下になったらos停止
		if (rpm <= HDD_MAX - HDD_PERMISSIBLE){
			os.state = OS_STOP;
		}
		//5160以上になったらHDDを危機的状態にする
		if (rpm >= HDD_MAX + HDD_PERMISSIBLE){
			hdd.state = HDD_CAUTION;
			hdd.explosionCounter = 60;
		}
		break;

		//危機的状態の時カウント(hdd.explosionCounter)が0になったら爆発→ゲームオーバー
	case HDD_CAUTION:
		rpm--;
		if (beforeSpaceKey == 1 && keyBuffer[KEY_INPUT_SPACE] == 0) {
			rpm += HDD_SPIN;
		}

		if (keyBuffer[KEY_INPUT_SPACE] == 1) {
			hdd.rect.x = 410;
			hdd.rect.y = 270;
		}

		if (keyBuffer[KEY_INPUT_SPACE] == 0) {
			hdd.rect.x = 380;
			hdd.rect.y = 300;
		}

		hdd.explosionCounter--;
		if (hdd.explosionCounter <= 0){
			hdd.state = DEAD;
			mainState = GAMEOVER;
		}
		//カウンタが０になる前に5260以下になったらHDDを通常状態に戻す
		if (hdd.explosionCounter > 0 && rpm < HDD_MAX + HDD_PERMISSIBLE){
			hdd.state = HDD_START;
			hdd.explosionCounter = 60;

		}
		break;

	}
}

//OSの更新処理
void updateOs(){
	switch(os.state){
	case OS_STOP:
		osstarting = 0;
		//OSが止まっている時は何もしない
		break;

	case OS_START:
		//OSが動いている時はまずOSを動かす(ostimeが１分を超えたらゲームクリア)
		osstarting++;

		if (osstarting >= OS_TIME){
			mainState = GAMECLEAR;
		}
		break;
	}
}
//-------------------------------------描画処理----------------------------------------
void draw() {
	// 画面を消去する
	ClearDrawScreen();

	switch (mainState) {
	case TITLE:
		// タイトルと音楽の描画
		drawTitle();
		PlaySoundMem(mainbgm, DX_PLAYTYPE_LOOP, TRUE);
		break;

	case EXPLANATION:
		//説明画面の描画
		drawExplanation();
		break;

	case GAME:
		// ゲーム画面の描画
		drawHdd();							
		drawOs();							
		break;

	case GAMEOVER:
		// ゲームオーバー画面の描画
		drawGameOver();
		break;

	case GAMECLEAR:
		// ゲームクリア画面の描画
		drawGameClear();
		break;

	}


	// 画面に表示する
	ScreenFlip();
}

// タイトルの描画
void drawTitle() {
	DrawBox(0, 0, 372, 279, GetColor(255, 255, 255), FALSE);
	DrawGraph(360, 240, titleGraph, TRUE);
	DrawString(0, 100, "おい、お前のHDD、壊れてるぞ", GetColor(255, 255, 255));
	DrawString(0, 160, "SPACEキーで開始、Escで強制終了", GetColor(255, 255, 255));


}

//説明画面の描画
void drawExplanation(){
	DrawBox(0, 0, 372, 279, GetColor(255, 255, 255), FALSE);
	DrawBox(0, 0, 372, 279, GetColor(50, 50, 255), TRUE);
	DrawString(50, 5, "～　あらすじ　～", GetColor(255, 255, 255));
	DrawGraph(360, 240, titleGraph, TRUE);
	DrawString(0, 40, "「あれ、HDDから音がしない……」", GetColor(255, 255, 255));
	DrawString(0, 60, "どうやらOSが起動しないようです", GetColor(255, 255, 255));
	DrawString(0, 80, "しかしあなたは諦めていなかった!", GetColor(255, 255, 255));
	DrawString(0, 100, "「そうか……自分で回せばいいんだ!」", GetColor(255, 255, 255));
	DrawString(0, 120, "スペースキーでHDDを回転させます", GetColor(255, 255, 255));
	DrawString(0, 140, "HDDを約5200毎分回転(5200rpm)で回せば", GetColor(255, 255, 255));
	DrawString(0, 160, "OSが起動してくれます", GetColor(255, 255, 255));
	DrawString(0, 180, "5200rpmより早く回すと壊れるかも知れません", GetColor(255, 255, 255));
	DrawString(0, 200, "逆に遅いとOSが起動しません", GetColor(255, 255, 255));
	DrawString(0, 220, "がんばって5200rpmを維持してください", GetColor(255, 255, 255));
}

// ゲームオーバーの描画
void drawGameOver() {
	DrawGraph(0, 0, gameoverGraph, TRUE);
	DrawBox(0, 0, 372, 279, GetColor(255, 255, 255), FALSE);
	DrawGraph(360, 240, titleGraph, TRUE);
	DrawGraph(420, 200, bombGraph, TRUE);
	DrawString(0, 280, "うおっ!HDDから火がっ!! (GAMEOVER)", GetColor(255, 0, 0));
	DrawString(0, 300, "RキーでHDDを買ってくる(タイトル画面)", GetColor(255, 255, 255));
	StopSoundMem(mainbgm);
}

// ゲームクリアの描画
void drawGameClear() {
	DrawBox(0, 0, 372, 279, GetColor(50, 50, 255), TRUE);
	DrawBox(0, 0, 372, 279, GetColor(255, 255, 255), FALSE);
	DrawGraph(360, 240, titleGraph, TRUE);
	DrawString(186, 140, "ようこそ", GetColor(255, 255, 255));
	DrawString(0, 263, "このOSのコピーは正規品ではありません", GetColor(255, 255, 255));
	DrawString(0, 305, "やったー! 起動成功!", GetColor(0, 255, 0));
	DrawString(0, 325, "Rキーで正規のOSを買ってくる(タイトル画面)", GetColor(255, 255, 255));
	StopSoundMem(mainbgm);
}

//ゲーム画面の描画（rpm数と指)
void drawHdd(){
	DrawGraph(360, 240, hddGraph, TRUE);
	DrawGraph((int)hdd.rect.x, (int)hdd.rect.y, handGraph, TRUE);
	DrawFormatString(380, 220, GetColor(255, 255, 255), "現在%drpm", rpm);

	switch (hdd.state){
	case HDD_STOP:
		DrawString(0, 280, "スペースキーでHDDを回せ!", GetColor(255, 255, 255));
		break;

	case HDD_START:		
		DrawString(0, 280, "5140～5260rpmを目指せ!", GetColor(255, 255, 255));
		break;

	case HDD_CAUTION:
		DrawString(0, 280, "これ以上回すな!発火するぞ!", GetColor(255, 0, 0));
		break;
	}
}

//OSがどんな風に表示されるか(osが起動してから何秒後に何を表示するか)
void drawOs(){
	DrawBox(0, 0, 372, 279, GetColor(255, 255, 255), FALSE);
	DrawFormatString(0, 500, GetColor(255, 255, 255), "%d", osstarting);

	switch (os.state){
	case OS_STOP:
		break;

	case OS_START:

		if (osstarting <= 0)
			DrawString(2, 5, "", GetColor(100, 100, 100));

		else if (osstarting <= 300){
			DrawGraph(230, 4, gamingGraph, TRUE);
		}

		else if (osstarting <= 360)
			DrawString(2, 5, "-", GetColor(100, 100, 100));

		else if (osstarting <= 420)
			DrawString(2, 5, "", GetColor(100, 100, 100));

		else if (osstarting <= 480)
			DrawString(2, 5, "-", GetColor(100, 100, 100));

		else if (osstarting <= 540)
			DrawString(2, 5, "", GetColor(100, 100, 100));

		else if (osstarting <= 840)
			DrawString(126, 130, "起動中です", GetColor(255, 255, 255));

		else if (osstarting <= 960)
			DrawString(2, 5, "", GetColor(100, 100, 100));

		else if (osstarting <= 1020)
			DrawBox(0, 0, 372, 279, GetColor(50, 50, 255), TRUE);

		else if (osstarting <= 1260){
			DrawBox(0, 0, 372, 279, GetColor(50, 50, 255), TRUE);
			DrawString(120, 130, "ログオン準備中", GetColor(255, 255, 255));
		}

		break;
	}

		

}