//---------------------- �\���� ----------------------

// ��`
struct Rect {
	double x;					// �����x���W
	double y;					// �����y���W
	double width;				// ��
	double height;				// ����
};

// �L�����N�^�[
struct Charactor {
	Rect rect;					// ���W�ƕ�
	int state;					// ���
	int explosionCounter;		// hdd�j���p�J�E���^
};


// ------------- �֐��̃v���g�^�C�v�錾 -------------

// ������
void init();							// ����������
void initSound();                       //�ȏ�����
void initGraphic();						// �摜�̓ǂݍ���
void initHdd();							// HDD�̏�����
void initOs();							// OS�̏�����

// ����
void input();							// ���͏���

// �X�V
void update();							// �X�V����
void updateHdd();						// HDD�̍X�V����
void updateOs();						// OS�̍X�V����

// �`��
void draw();							// �`�揈��
void drawTitle();						// �^�C�g���̕`��
void drawExplanation();					//�����̕`��
void drawGameOver();					// �Q�[���I�[�o�[�̕`��
void drawGameClear();					// �Q�[���N���A�̕`��
void drawHdd();							// HDD�̕`��
void drawOs();							// OS�̕`��
