#include "DxLib.h"
#include "mouse.h"

/*===== 定数定義 =====*/
#define PuzzleXOne 160
#define PuzzleYOne 210
#define PuzzleXTwo 820
#define PuzzleYTwo 210
#define MOVE 5
#define TimeSet 30


/*===== 列挙体 =====*/
typedef enum{
	TitleLeady,
	Title,
	CountDown,
	PuzzleLeady,
	Puzzle,
	SoloLeady,
	Solo,
	ResaltLeady,
	Resalt,
	Debag,
}eScene;

int Scene = TitleLeady;


/*===== マウス関連用変数 =====*/
int mx; //マウスx座標取得
int my; //マウスy座標取得
int Mouse;
int cflag = 0; //クリックフラグ

/*===== DirectInputゲームパッド関連用変数 =====*/
int input1 = GetJoypadInputState(DX_INPUT_KEY_PAD1);
int input2 = GetJoypadInputState(DX_INPUT_PAD2);
int bflag = 0; //ボタンフラグ
int gx[2]; //ゲームパッドx座標取得
int gy[2]; //ゲームパッドy座標取得

/*===== 画像用変数 =====*/
int end_graph;
int debag_graph;
int GameMode_Button[2];
int point_graph[2];
int TBack;
int PBack;
int RBack;
int number[100];
int puzzle[3][9];
int about;
int Clear[3];
int player[3];
int gflag = 0; //画像用フラグ

/*===== 色用変数=====*/
int White = GetColor(255, 255, 255);

/*===== 処理用変数 =====*/
int eflag = 1; //エンドフラグ
int pflag[2] = { 1, 1 }; //パネルフラグ
int panel[2][9];
int place[2]; //スタート位置

/*===== タイマー用関数 =====*/
int Timer = 0;
int tflag = 0; //タイマー用フラグ

/*===== 勝敗判定用変数 =====*/
double SaveTime[2];
int PanelHit[2];
int win = NULL;

/*===== 関数プロトタイプ宣言 =====*/
void Graph();
void KeyPulse(int Change, int Button);
void Change(int player, int x, int y);


/*===== メイン文 =====*/
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int){

	SetGraphMode(1280, 720, 32), ChangeWindowMode(FALSE), DxLib_Init(), SetDrawScreen(DX_SCREEN_BACK);

	/*----- 画像読み込み -----*/
	Graph();


	while (ScreenFlip() == 0 && ProcessMessage() == 0 && ClearDrawScreen() == 0 && !CheckHitKey(KEY_INPUT_ESCAPE) && eflag){

		/*----- マウス座標取得 -----*/
		Mouse = GetMouseInput();
		GetMousePoint(&mx, &my);

		/*----- ゲームパッド入力状態取得 -----*/
		input1 = GetJoypadInputState(DX_INPUT_KEY_PAD1);
		input2 = GetJoypadInputState(DX_INPUT_PAD2);


		switch(Scene){

		case TitleLeady:{
			/*----- 音楽停止 -----*/
			StopSoundFile();

			/*----- フラグ初期化 -----*/
			bflag = 2;
			gflag = 0;
			tflag = 0;
			pflag[0] = 1;
			pflag[1] = 1;
			place[0] = 0;
			place[1] = 0;

			/*----- タイマー初期化 -----*/
			Timer = 0;

			/*----- 勝敗判定用変数初期化 -----*/
			SaveTime[0] = 30;
			SaveTime[1] = 30;
			win = NULL;

			/*----- シーン遷移 -----*/
			Scene = Title;

			/*----- 音楽再生開始 -----*/
			PlaySoundFile("Music/Title.mp3", DX_PLAYTYPE_LOOP);

			break;
		}

		case Title:{
			/*----- 画像表示 -----*/
			DrawGraph(0, 0, TBack, FALSE);
			if (0 <= mx && mx < 100 && 0 <= my && my < 100){
				DrawGraph(0, 0, debag_graph, FALSE); //Debag
			}
			if (1180 <= mx && mx < 1280 && 0 <= my && my < 100){
				DrawGraph(1180, 0, end_graph, FALSE); //End
			}
			DrawGraph(160, 570, GameMode_Button[0], TRUE); //Solo
			DrawGraph(820, 570, GameMode_Button[1], TRUE); //Multi
			DrawGraph(mx - 5, my - 5, point_graph[0], TRUE); //MousePointer

			/*----- マウス座標 -----*/
			MouseXY(mx, my, 1180, 0, 1280, 100, &eflag, 0, Mouse, &cflag); //end
			MouseXY(mx, my, 0, 0, 100, 100, &Scene, Debag, Mouse, &cflag); //debag
			MouseXY(mx, my, 160, 570, 460, 670, &Scene, SoloLeady, Mouse, &cflag); //solo
			MouseXY(mx, my, 820, 570, 1120, 670, &Scene, CountDown, Mouse, &cflag); //multi
			MouseEND(Mouse, &cflag);

			break;
		}

		case CountDown:{
			/*----- 音楽停止 -----*/
			StopSoundFile();

			/*----- 動画再生 -----*/
			PlayMovie("Movie/countdown.mpg", 1, DX_MOVIEPLAYTYPE_NORMAL);

			/*----- シーン遷移 -----*/
			Scene = PuzzleLeady;

			break;
		}

		case PuzzleLeady:{
			/*----- 音楽停止 -----*/
			StopSoundFile();

			/*----- パネルシャッフル -----*/
			if (gflag == 0){
				for (int j = 0; j < 2; j++){
					for (int i = 0; i < 9; i++) {
						panel[j][i] = i;
					}
					for (int i = 0; i < 1000; i++) {
						Change(j, GetRand(2), GetRand(2));
					}
				}

				gflag = 1;
			}

			/*----- 空きパネル探索 -----*/
			for (int j = 0; j < 2; j++){
				for (int i = 0; i < 9; i++){
					if (panel[j][i] == 8){
						place[j] = i;
					}
				}
			}

			/*----- ゲームパッドXY初期位置設定 -----*/
			/*----- 1P -----*/
			switch (place[0]){
			case 0:gx[0] = 45 + PuzzleXOne; gy[0] = 45 + PuzzleYOne; break;
			case 1:gx[0] = 145 + PuzzleXOne; gy[0] = 45 + PuzzleYOne; break;
			case 2:gx[0] = 245 + PuzzleXOne; gy[0] = 45 + PuzzleYOne; break;
			case 3:gx[0] = 45 + PuzzleXOne; gy[0] = 145 + PuzzleYOne; break;
			case 4:gx[0] = 145 + PuzzleXOne; gy[0] = 145 + PuzzleYOne; break;
			case 5:gx[0] = 245 + PuzzleXOne; gy[0] = 145 + PuzzleYOne; break;
			case 6:gx[0] = 45 + PuzzleXOne; gy[0] = 245 + PuzzleYOne; break;
			case 7:gx[0] = 145 + PuzzleXOne; gy[0] = 245 + PuzzleYOne; break;
			case 8:gx[0] = 245 + PuzzleXOne; gy[0] = 245 + PuzzleYOne; break;
			}
			/*----- 2P -----*/
			switch (place[1]){
			case 0:gx[1] = 45 + PuzzleXTwo; gy[1] = 45 + PuzzleYTwo; break;
			case 1:gx[1] = 145 + PuzzleXTwo; gy[1] = 45 + PuzzleYTwo; break;
			case 2:gx[1] = 245 + PuzzleXTwo; gy[1] = 45 + PuzzleYTwo; break;
			case 3:gx[1] = 45 + PuzzleXTwo; gy[1] = 145 + PuzzleYTwo; break;
			case 4:gx[1] = 145 + PuzzleXTwo; gy[1] = 145 + PuzzleYTwo; break;
			case 5:gx[1] = 245 + PuzzleXTwo; gy[1] = 145 + PuzzleYTwo; break;
			case 6:gx[1] = 45 + PuzzleXTwo; gy[1] = 245 + PuzzleYTwo; break;
			case 7:gx[1] = 145 + PuzzleXTwo; gy[1] = 245 + PuzzleYTwo; break;
			case 8:gx[1] = 245 + PuzzleXTwo; gy[1] = 245 + PuzzleYTwo; break;
			}

			/*----- 各種変数初期化 -----*/
			bflag = 0;

			/*----- シーン遷移 -----*/
			Scene = Puzzle;

			/*----- 音楽再生開始 -----*/
			PlaySoundFile("Music/Puzzle.wav", DX_PLAYTYPE_BACK);

			break;
		}

		case Puzzle:{
			/*----- タイマーカウント -----*/
			Timer++;

			/*----- タイムアップ判定 -----*/
			if ((TimeSet - (Timer / 60)) == 0){
				tflag = 1;
			}

			/*----- 背景表示 -----*/
			DrawGraph(0, 0, PBack, FALSE);

			/*----- 数字表示 -----*/
			DrawGraph(590, 20, number[TimeSet - (Timer / 60)], TRUE);

			/*----- パネル表示1P -----*/
			for (int i = 0; i < 9; i++) {
				if (panel[0][i] < 8) {
					DrawGraph(((i % 3) * 100) + PuzzleXOne, ((i / 3) * 100) + PuzzleYOne, puzzle[0][panel[0][i]], FALSE);
				}
			}
			if (!pflag[0]){
				DrawGraph(PuzzleXOne, PuzzleYOne, Clear[0], FALSE);
			}

			/*----- パネル表示2P -----*/
			for (int i = 0; i < 9; i++) {
				if (panel[1][i] < 8) {
					DrawGraph(((i % 3) * 100) + PuzzleXTwo, ((i / 3) * 100) + PuzzleYTwo, puzzle[1][panel[1][i]], FALSE);
				}
			}
			if (!pflag[1]){
				DrawGraph(PuzzleXTwo, PuzzleYTwo, Clear[1], FALSE);
			}

			/*----- ポインター表示 -----*/
			DrawGraph(gx[0], gy[0], point_graph[0], TRUE); //1P
			DrawGraph(gx[1], gy[1], point_graph[1], TRUE); //2P

			/*----- ゲームパッド入力1P -----*/
			if ((input1 & PAD_INPUT_UP) && 5 < (gy[0] - PuzzleYOne)){
				gy[0] -= MOVE;
			}
			if ((input1 & PAD_INPUT_DOWN) && (gy[0] - PuzzleYOne) < 295){
				gy[0] += MOVE;
			}
			if ((input1 & PAD_INPUT_LEFT) && 5 < (gx[0] - PuzzleXOne)){
				gx[0] -= MOVE;
			}
			if ((input1 & PAD_INPUT_RIGHT) && (gx[0] - PuzzleXOne) < 295){
				gx[0] += MOVE;
			}

			/*----- ゲームパッド入力2P -----*/
			if ((input2 & PAD_INPUT_UP) && 5 < (gy[1] - PuzzleYTwo)){
				gy[1] -= MOVE;
			}
			if ((input2 & PAD_INPUT_DOWN) && (gy[1] - PuzzleYTwo) < 295){
				gy[1] += MOVE;
			}
			if ((input2 & PAD_INPUT_LEFT) && 5 < (gx[1] - PuzzleXTwo)){
				gx[1] -= MOVE;
			}
			if ((input2 & PAD_INPUT_RIGHT) && (gx[1] - PuzzleXTwo) < 295){
				gx[1] += MOVE;
			}

			/*----- 入れ替え関数 -----*/
			if (pflag[0]){
				Change(0, (gx[0] - PuzzleXOne) / 100, (gy[0] - PuzzleYOne) / 100);
			}
			if (pflag[1]){
				Change(1, (gx[1] - PuzzleXTwo) / 100, (gy[1] - PuzzleYTwo) / 100);
			}

			/*----- 終了判定 -----*/
			PanelHit[0] = 9; //正答率1P
			PanelHit[1] = 9; //正答率2P
			bool clear[2] = { true, true }; //クリア判定
			for (int j = 0; j < 2; j++){
				for (int i = 0; i < 9; i++) {
					if (panel[j][i] != i){
						clear[j] = false;
						PanelHit[j]--;
					}
				}
			}
			for (int i = 0; i < 2; i++){
				if (clear[i]){
					int nowflag = pflag[i];
					pflag[i] = 0;
					if (nowflag != pflag[i]){
						SaveTime[i] = Timer / 60;
					}
				}
			}
			if ((clear[0] && clear[1]) || tflag){
				Scene = ResaltLeady;
			}

			break;
		}

		case SoloLeady:{
			/*----- 音楽停止 -----*/
			StopSoundFile();

			/*----- パネルシャッフル -----*/
			if (gflag == 0){
				for (int i = 0; i < 9; i++) {
					panel[0][i] = i;
				}
				for (int i = 0; i < 1000; i++) {
					Change(0, GetRand(2), GetRand(2));
				}

				gflag = 1;
			}

			/*----- 空きパネル探索 -----*/
			for (int i = 0; i < 9; i++){
				if (panel[0][i] == 8){
					place[0] = i;
				}
			}

			/*----- ゲームパッドXY初期位置設定 -----*/
			switch (place[0]){
			case 0:gx[0] = 45 + PuzzleXOne; gy[0] = 45 + PuzzleYOne; break;
			case 1:gx[0] = 145 + PuzzleXOne; gy[0] = 45 + PuzzleYOne; break;
			case 2:gx[0] = 245 + PuzzleXOne; gy[0] = 45 + PuzzleYOne; break;
			case 3:gx[0] = 45 + PuzzleXOne; gy[0] = 145 + PuzzleYOne; break;
			case 4:gx[0] = 145 + PuzzleXOne; gy[0] = 145 + PuzzleYOne; break;
			case 5:gx[0] = 245 + PuzzleXOne; gy[0] = 145 + PuzzleYOne; break;
			case 6:gx[0] = 45 + PuzzleXOne; gy[0] = 245 + PuzzleYOne; break;
			case 7:gx[0] = 145 + PuzzleXOne; gy[0] = 245 + PuzzleYOne; break;
			case 8:gx[0] = 245 + PuzzleXOne; gy[0] = 245 + PuzzleYOne; break;
			}

			/*----- 各種変数初期化 -----*/
			bflag = 0;

			/*----- シーン遷移 -----*/
			Scene = Solo;

			/*----- 音楽再生開始 -----*/
			PlaySoundFile("Music/Puzzle.wav", DX_PLAYTYPE_LOOP);

			break;
		}

		case Solo:{
			/*----- 背景表示 -----*/
			DrawGraph(0, 0, PBack, FALSE);

			/*----- 数字表示 -----*/
			DrawGraph(590, 20, number[99], TRUE);

			/*----- パネル表示1P -----*/
			for (int i = 0; i < 9; i++) {
				if (panel[0][i] < 8) {
					DrawGraph(((i % 3) * 100) + PuzzleXOne, ((i / 3) * 100) + PuzzleYOne, puzzle[2][panel[0][i]], FALSE);
				}
			}
			if (!pflag[0]){
				DrawGraph(PuzzleXOne, PuzzleYOne, Clear[2], FALSE);
			}

			/*----- 概要表示 -----*/
			DrawGraph(PuzzleXTwo, PuzzleYTwo, about, FALSE);

			/*----- ポインター表示 -----*/
			DrawGraph(gx[0], gy[0], point_graph[0], TRUE); //1P

			/*----- ゲームパッド入力1P -----*/
			if ((input1 & PAD_INPUT_UP) && 5 < (gy[0] - PuzzleYOne)){
				gy[0] -= MOVE;
			}
			if ((input1 & PAD_INPUT_DOWN) && (gy[0] - PuzzleYOne) < 295){
				gy[0] += MOVE;
			}
			if ((input1 & PAD_INPUT_LEFT) && 5 < (gx[0] - PuzzleXOne)){
				gx[0] -= MOVE;
			}
			if ((input1 & PAD_INPUT_RIGHT) && (gx[0] - PuzzleXOne) < 295){
				gx[0] += MOVE;
			}

			/*----- 入れ替え関数 -----*/
			if (pflag[0]){
				Change(0, (gx[0] - PuzzleXOne) / 100, (gy[0] - PuzzleYOne) / 100);
			}

			/*----- 終了判定 -----*/
			PanelHit[0] = 9; //正答率1P
			bool clear[2] = { true, true }; //クリア判定
			for (int i = 0; i < 9; i++) {
				if (panel[0][i] != i){
					clear[0] = false;
					PanelHit[0]--;
				}
			}
			for (int i = 0; i < 2; i++){
				if (clear[i]){
					int nowflag = pflag[i];
					pflag[i] = 0;
					if (nowflag != pflag[i]){
						SaveTime[i] = Timer / 60;
					}
				}
			}

			/*----- シーン遷移 -----*/
			KeyPulse(TitleLeady, PAD_INPUT_4);

			break;
		}

		case ResaltLeady:{
			/*----- 音楽停止 -----*/
			StopSoundFile();

			/*----- シーン遷移 -----*/
			Scene = Resalt;

			/*----- 音楽再生開始 -----*/
			PlaySoundFile("Music/Resalt.mp3", DX_PLAYTYPE_LOOP);

			break;
		}

		case Resalt:{
			/*----- 画像表示 -----*/
			DrawGraph(0, 0, RBack, FALSE);
			DrawGraph(400, 250, number[PanelHit[0]], TRUE);
			DrawGraph(400, 350, number[(int)SaveTime[0]], TRUE);
			DrawGraph(1080, 250, number[PanelHit[1]], TRUE);
			DrawGraph(1080, 350, number[(int)SaveTime[1]], TRUE);

			/*----- 勝敗判定 -----*/
			if (PanelHit[0] > PanelHit[1]){
				win = 1;
			}
			else if (PanelHit[0] < PanelHit[1]){
				win = 2;
			}
			else if (PanelHit[0] == PanelHit[1]){
				if (SaveTime[0] > SaveTime[1]){
					win = 1;
				}
				else if (SaveTime[0] < SaveTime[1]){
					win = 2;
				}
				else if (SaveTime[0] == SaveTime[1]){
					win = 0;
				}
			}

			/*----- 結果表示 -----*/
			DrawGraph(490, 520, player[win], TRUE);

			/*----- シーン遷移 -----*/
			KeyPulse(TitleLeady, PAD_INPUT_4);

			break;
		}

		case Debag:{
			/*----- 画像表示 -----*/
			if (0 <= mx && mx < 100 && 0 <= my && my < 100){
				DrawGraph(0, 0, debag_graph, FALSE); //Debag
			}
			if (0 <= mx && mx < 1280 && 0 <= my && my < 100){
				DrawGraph(mx, my, point_graph[0], TRUE); //Debag
			}

			/*----- 文字表示 -----*/
			DrawFormatString(0, 110, White, "(input1 & PAD_INPUT_4):%d", (input1 & PAD_INPUT_4));
			DrawFormatString(240, 110, White, "(input1 & PAD_INPUT_3):%d", (input1 & PAD_INPUT_3));
			DrawFormatString(0, 130, White, "(input2 & PAD_INPUT_4):%d", (input2 & PAD_INPUT_4));
			DrawFormatString(0, 150, White, "(input1 & PAD_INPUT_UP):%d", (input1 & PAD_INPUT_UP));
			DrawFormatString(0, 170, White, "(input1 & PAD_INPUT_DOWN):%d", (input1 & PAD_INPUT_DOWN));
			DrawFormatString(0, 190, White, "(input1 & PAD_INPUT_LEFT):%d", (input1 & PAD_INPUT_LEFT));
			DrawFormatString(0, 210, White, "(input1 & PAD_INPUT_RIGHT):%d", (input1 & PAD_INPUT_RIGHT));

			/*----- マウス座標 -----*/
			MouseXY(mx, my, 0, 0, 100, 100, &Scene, Title, Mouse, &cflag);
			MouseEND(Mouse, &cflag);

			break;
		}

		}

	}

	DxLib_End(); // DXライブラリ終了処理
	return 0;

}


void Graph(){
	end_graph = LoadGraph("Graph/end.png");
	debag_graph = LoadGraph("Graph/debag.png");

	GameMode_Button[0] = LoadGraph("Graph/Solo.png");
	GameMode_Button[1] = LoadGraph("Graph/Multi.png");

	point_graph[0] = LoadGraph("Graph/point1.png");
	point_graph[1] = LoadGraph("Graph/point2.png");

	TBack = LoadGraph("Graph/title.png");
	PBack = LoadGraph("Graph/puzzle_back.png");
	RBack = LoadGraph("Graph/Resalt.png");

	LoadDivGraph("Graph/number.png", 100, 10, 10, 100, 100, number);
	LoadDivGraph("Graph/Puzzle1.png", 9, 3, 3, 100, 100, puzzle[0]);
	LoadDivGraph("Graph/Puzzle2.png", 9, 3, 3, 100, 100, puzzle[1]);
	LoadDivGraph("Graph/TestPuzzle.png", 9, 3, 3, 100, 100, puzzle[2]);

	about = LoadGraph("Graph/about.png");

	Clear[0] = LoadGraph("Graph/Puzzle1.png");
	Clear[1] = LoadGraph("Graph/Puzzle2.png");
	Clear[2] = LoadGraph("Graph/TestPuzzle.png");

	player[0] = LoadGraph("Graph/draw.png");
	player[1] = LoadGraph("Graph/1pwin.png");
	player[2] = LoadGraph("Graph/2pwin.png");
}

void KeyPulse(int Change, int Button){
	if (((input1 & Button) || (input2 & Button))){
		bflag++;
	}

	if (bflag == 1){
		Scene = Change;
	}
	else if (1 < bflag){
		if ((!(input1 & Button) && !(input2 & Button))){
			bflag = 0;
		}
	}
}

void Change(int player, int x, int y) {
	int p1 = y * 3 + x;
	int p2 = -1;

	if (x > 0 && panel[player][p1 - 1] == 8) {
		p2 = p1 - 1;
	}
	if (x < 2 && panel[player][p1 + 1] == 8) {
		p2 = p1 + 1;
	}
	if (y > 0 && panel[player][p1 - 3] == 8) {
		p2 = p1 - 3;
	}
	if (y < 2 && panel[player][p1 + 3] == 8) {
		p2 = p1 + 3;
	}

	if (p2 != -1) {
		panel[player][p2] = panel[player][p1];
		panel[player][p1] = 8;
	}
}