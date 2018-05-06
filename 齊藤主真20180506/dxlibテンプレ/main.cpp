#include "DxLib.h"
#include "Header.h"
// ------------------- �p�����[�^ -------------------

#define WINDOW_W 640			// ��ʂ̕�
#define WINDOW_H 480			// ��ʂ̍���
#define HDD_MAX 5200			// HDD��]��
#define HDD_MIN 0				// HDD�Œ��]��
#define HDD_PERMISSIBLE 60		// HDD��]���e�͈�
#define OS_TIME 1280			// OS�̋N���ɕK�v�Ȏ���
#define HDD_SPIN 60				// �X�y�[�X�L�[�P�񕪂̃X�s����


#define OS_STOP 0				//OS���N��
#define OS_START 1				//OS�N����

#define HDD_STOP 0				// HDD�����ĂȂ�
#define HDD_START 1				// HDD�����Ă�
#define HDD_CAUTION 2			//�񂵉߂��x��
#define DEAD 3					// ���S

#define TITLE 0					// �^�C�g��
#define EXPLANATION 1			//�������
#define GAME 2					// �Q�[��
#define GAMEOVER 3				// �Q�[���I�[�o�[
#define GAMECLEAR 4				// �Q�[���N���A


// ---------------------- HDD ----------------------

Charactor hdd;							// HDD
Charactor os;							// OS


// ---------------------- �摜 ----------------------

int hddGraph;					// hdd�̉摜
int handGraph;					// os�̉摜
int bombGraph;					// �����摜
int gameoverGraph;				//�Q�[���I�[�o�[���
int titleGraph;					//���ʂɃ^�C�g���摜
int gamingGraph;				//�Q�[���摜

//-----------------------���y-----------------------

int mainbgm;                    //���y

//-----------------------�J�E���g-------------------

int rpm = 0, osstarting = 0;	//�Q�[�����J�E���g


// ---------------------- ��� ----------------------

int mainState;					// �Q�[���̏��
char beforeSpaceKey;			// �O�̃^�C�~���O��SPACE�L�[�̏�Ԃ��i�[
char beforeRKey;				// �O�̃^�C�~���O��R�L�[�̏�Ԃ��i�[
char keyBuffer[256];			// �L�[�{�[�h�̏�Ԃ��i�[


// ---------------------------------------- main�֐� ----------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	// �E�B���h�E���[�h�ŋN��
	ChangeWindowMode(TRUE);

	// �E�B���h�E�̃^�C�g�����w�肷��
	SetMainWindowText("hddbreak");

	// DX���C�u�����̏��������s��
	if (DxLib_Init() == -1) {
		return -1;
	}

	// ��ʃ��[�h�̕ύX
	SetGraphMode(WINDOW_W, WINDOW_H, 32);

	// �`���𗠉�ʂɂ���
	SetDrawScreen(DX_SCREEN_BACK);

	// �Q�[���̏���������
	init();

	// 1�b�ɖ�60���ʂ��X�V
	int nextTime = GetNowCount() + 1000 / 60;

	// Esc�L�[�������܂Ń��[�v����
	while (ProcessMessage() == 0 && keyBuffer[KEY_INPUT_ESCAPE] == 0) {
		input(); //���͏�ԏ���
		update();	// �X�V����
		draw();		// �`�揈��

		// ���Ԃ��]�����ꍇ�Asleep����
		if (nextTime > GetNowCount()) {
			Sleep(nextTime - GetNowCount());
		}
		nextTime = GetNowCount() + 1000 / 60;
	}

	// DX���C�u�����g�p�̏I������
	DxLib_End();

	// �\�t�g�̏I��
		return 0;
}
//--------------------------------------����������--------------------------------------
void init(){
	mainState = TITLE;

	//�ォ�珇�Ԃɏ������������s��
	initGraphic();
	initSound();
	initHdd();
	initOs();

}

//�摜�̓ǂݍ���
void initGraphic(){
	hddGraph = LoadGraph("bmp/HDD.png");
	bombGraph = LoadGraph("bmp/fire.png");
	titleGraph = LoadGraph("bmp/HDD.png");
	handGraph = LoadGraph("bmp/hand.png");
	gameoverGraph = LoadGraph("bmp/diskcheck.png");
	gamingGraph = LoadGraph("bmp/bios.jpg");
}

//�����ǂݍ���
void initSound(){
	mainbgm = LoadSoundMem("wav/hddbroken2.wav");
}

//HDD�̏�����
void initHdd(){
	hdd.state = HDD_STOP;
	hdd.rect.x = 380;
	hdd.rect.y = 300;
	hdd.rect.width = 114;
	hdd.rect.height = 185;
}

//OS�̏�����
void initOs(){
	os.state = OS_STOP;
	os.rect.x = 0;
	os.rect.y = 0;
	os.rect.width = 372;
	os.rect.height = 279;
}

//---------------------------------------���͏���---------------------------------------
void input() {
	beforeSpaceKey = keyBuffer[KEY_INPUT_SPACE];
	beforeRKey = keyBuffer[KEY_INPUT_R];
	GetHitKeyStateAll(keyBuffer);
}

//-------------------------------------�X�V����----------------------------------------

void update() {
	switch (mainState) {
	case TITLE:
		// space�L�[���������u��EXPLANATION�Ɉړ�
		if (beforeSpaceKey == 1 && keyBuffer[KEY_INPUT_SPACE] == 0) {
			mainState = EXPLANATION;
		}
		break;

	case EXPLANATION:
		// space�L�[���������u��GAME�Ɉړ�
		if (beforeSpaceKey == 1 && keyBuffer[KEY_INPUT_SPACE] == 0) {
			mainState = GAME;
		}
		break;

	case GAME:
		// �Q�[���̍X�V�������s��
		updateHdd();
		updateOs();
		break;

	case GAMEOVER:
		// R�L�[���������u�ԍŏ��ɖ߂�
		if (beforeRKey == 1 && keyBuffer[KEY_INPUT_R] == 0) {
			init();
		}
		break;

	case GAMECLEAR:
		// R�L�[���������u�ԍŏ��ɖ߂�
		if (beforeRKey == 1 && keyBuffer[KEY_INPUT_R] == 0) {
			init();
		}
		break;
	}
}

// HDD�̍X�V����
void updateHdd(){
	switch (hdd.state){
	case HDD_STOP:
		rpm = 0;
		//�X�y�[�X�����������]�����グ��HDD���N����Ԃɂ���
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
		//���C��rpm�͉������Ă������X�y�[�X��A�ł���Ή�]�����グ����
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

		//rpm���O�ɂȂ�����HDD���Ƃ܂��Ă����Ԃɂ���
		if (rpm <= HDD_MIN){
			hdd.state = HDD_STOP;
		}
		//5140~5260rpm�ɂȂ�����os���N��
		if (rpm < HDD_MAX + HDD_PERMISSIBLE && rpm > HDD_MAX - HDD_PERMISSIBLE){
			os.state = OS_START;
		}
		//5140�ȉ��ɂȂ�����os��~
		if (rpm <= HDD_MAX - HDD_PERMISSIBLE){
			os.state = OS_STOP;
		}
		//5160�ȏ�ɂȂ�����HDD����@�I��Ԃɂ���
		if (rpm >= HDD_MAX + HDD_PERMISSIBLE){
			hdd.state = HDD_CAUTION;
			hdd.explosionCounter = 60;
		}
		break;

		//��@�I��Ԃ̎��J�E���g(hdd.explosionCounter)��0�ɂȂ����甚�����Q�[���I�[�o�[
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
		//�J�E���^���O�ɂȂ�O��5260�ȉ��ɂȂ�����HDD��ʏ��Ԃɖ߂�
		if (hdd.explosionCounter > 0 && rpm < HDD_MAX + HDD_PERMISSIBLE){
			hdd.state = HDD_START;
			hdd.explosionCounter = 60;

		}
		break;

	}
}

//OS�̍X�V����
void updateOs(){
	switch(os.state){
	case OS_STOP:
		osstarting = 0;
		//OS���~�܂��Ă��鎞�͉������Ȃ�
		break;

	case OS_START:
		//OS�������Ă��鎞�͂܂�OS�𓮂���(ostime���P���𒴂�����Q�[���N���A)
		osstarting++;

		if (osstarting >= OS_TIME){
			mainState = GAMECLEAR;
		}
		break;
	}
}
//-------------------------------------�`�揈��----------------------------------------
void draw() {
	// ��ʂ���������
	ClearDrawScreen();

	switch (mainState) {
	case TITLE:
		// �^�C�g���Ɖ��y�̕`��
		drawTitle();
		PlaySoundMem(mainbgm, DX_PLAYTYPE_LOOP, TRUE);
		break;

	case EXPLANATION:
		//������ʂ̕`��
		drawExplanation();
		break;

	case GAME:
		// �Q�[����ʂ̕`��
		drawHdd();							
		drawOs();							
		break;

	case GAMEOVER:
		// �Q�[���I�[�o�[��ʂ̕`��
		drawGameOver();
		break;

	case GAMECLEAR:
		// �Q�[���N���A��ʂ̕`��
		drawGameClear();
		break;

	}


	// ��ʂɕ\������
	ScreenFlip();
}

// �^�C�g���̕`��
void drawTitle() {
	DrawBox(0, 0, 372, 279, GetColor(255, 255, 255), FALSE);
	DrawGraph(360, 240, titleGraph, TRUE);
	DrawString(0, 100, "�����A���O��HDD�A���Ă邼", GetColor(255, 255, 255));
	DrawString(0, 160, "SPACE�L�[�ŊJ�n�AEsc�ŋ����I��", GetColor(255, 255, 255));


}

//������ʂ̕`��
void drawExplanation(){
	DrawBox(0, 0, 372, 279, GetColor(255, 255, 255), FALSE);
	DrawBox(0, 0, 372, 279, GetColor(50, 50, 255), TRUE);
	DrawString(50, 5, "�`�@���炷���@�`", GetColor(255, 255, 255));
	DrawGraph(360, 240, titleGraph, TRUE);
	DrawString(0, 40, "�u����AHDD���特�����Ȃ��c�c�v", GetColor(255, 255, 255));
	DrawString(0, 60, "�ǂ����OS���N�����Ȃ��悤�ł�", GetColor(255, 255, 255));
	DrawString(0, 80, "���������Ȃ��͒��߂Ă��Ȃ�����!", GetColor(255, 255, 255));
	DrawString(0, 100, "�u�������c�c�����ŉ񂹂΂�����!�v", GetColor(255, 255, 255));
	DrawString(0, 120, "�X�y�[�X�L�[��HDD����]�����܂�", GetColor(255, 255, 255));
	DrawString(0, 140, "HDD���5200������](5200rpm)�ŉ񂹂�", GetColor(255, 255, 255));
	DrawString(0, 160, "OS���N�����Ă���܂�", GetColor(255, 255, 255));
	DrawString(0, 180, "5200rpm��葁���񂷂Ɖ��邩���m��܂���", GetColor(255, 255, 255));
	DrawString(0, 200, "�t�ɒx����OS���N�����܂���", GetColor(255, 255, 255));
	DrawString(0, 220, "����΂���5200rpm���ێ����Ă�������", GetColor(255, 255, 255));
}

// �Q�[���I�[�o�[�̕`��
void drawGameOver() {
	DrawGraph(0, 0, gameoverGraph, TRUE);
	DrawBox(0, 0, 372, 279, GetColor(255, 255, 255), FALSE);
	DrawGraph(360, 240, titleGraph, TRUE);
	DrawGraph(420, 200, bombGraph, TRUE);
	DrawString(0, 280, "������!HDD����΂���!! (GAMEOVER)", GetColor(255, 0, 0));
	DrawString(0, 300, "R�L�[��HDD�𔃂��Ă���(�^�C�g�����)", GetColor(255, 255, 255));
	StopSoundMem(mainbgm);
}

// �Q�[���N���A�̕`��
void drawGameClear() {
	DrawBox(0, 0, 372, 279, GetColor(50, 50, 255), TRUE);
	DrawBox(0, 0, 372, 279, GetColor(255, 255, 255), FALSE);
	DrawGraph(360, 240, titleGraph, TRUE);
	DrawString(186, 140, "�悤����", GetColor(255, 255, 255));
	DrawString(0, 263, "����OS�̃R�s�[�͐��K�i�ł͂���܂���", GetColor(255, 255, 255));
	DrawString(0, 305, "������[! �N������!", GetColor(0, 255, 0));
	DrawString(0, 325, "R�L�[�Ő��K��OS�𔃂��Ă���(�^�C�g�����)", GetColor(255, 255, 255));
	StopSoundMem(mainbgm);
}

//�Q�[����ʂ̕`��irpm���Ǝw)
void drawHdd(){
	DrawGraph(360, 240, hddGraph, TRUE);
	DrawGraph((int)hdd.rect.x, (int)hdd.rect.y, handGraph, TRUE);
	DrawFormatString(380, 220, GetColor(255, 255, 255), "����%drpm", rpm);

	switch (hdd.state){
	case HDD_STOP:
		DrawString(0, 280, "�X�y�[�X�L�[��HDD����!", GetColor(255, 255, 255));
		break;

	case HDD_START:		
		DrawString(0, 280, "5140�`5260rpm��ڎw��!", GetColor(255, 255, 255));
		break;

	case HDD_CAUTION:
		DrawString(0, 280, "����ȏ�񂷂�!���΂��邼!", GetColor(255, 0, 0));
		break;
	}
}

//OS���ǂ�ȕ��ɕ\������邩(os���N�����Ă��牽�b��ɉ���\�����邩)
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
			DrawString(126, 130, "�N�����ł�", GetColor(255, 255, 255));

		else if (osstarting <= 960)
			DrawString(2, 5, "", GetColor(100, 100, 100));

		else if (osstarting <= 1020)
			DrawBox(0, 0, 372, 279, GetColor(50, 50, 255), TRUE);

		else if (osstarting <= 1260){
			DrawBox(0, 0, 372, 279, GetColor(50, 50, 255), TRUE);
			DrawString(120, 130, "���O�I��������", GetColor(255, 255, 255));
		}

		break;
	}

		

}