/****************************************************************
*	piclib.c						*
*	ライブラリ関数一覧　(関数本体）				*
*			 last update 2014/06/12 by ecstaff	*
*****************************************************************/

#include	"pic.h"		/* ヘッダ・ファイル		*/
#include	"def.h"		/* define宣言一覧		*/
#include	"global.h"	/* グローバル変数		*/
#include	"piclib.h"	/* 関数一覧(プロトタイプ宣言）	*/

/********************************************************
*	関数名： void init(void)			*
*	機能　： PICのI/O等の初期化ルーチン		*
*********************************************************/
void init(void)
{
        OPTION_REG=0xd7;/* タイマ0のクロックは内部クロック、1:256プリスケーラ */
        CMCON = 0x07;   /* アナログコンパレータを無効(デジタル入力) */
        TMR0=0;		/* タイマ0に0を書き込む			*/
	T0IF=0;		/* タイマ0割り込みフラグを0にする	*/
	T0IE=1;		/* タイマ0割り込みを使う		*/
	GIE=1;		/* 割り込みを使う			*/
	TRISA=0x1f;	/* ポートAをすべて入力ピンにする	*/
	TRISB=0;	/* ポートBをすべて出力ピンにする	*/

	PORTB = 0xff;	/* ポートBを全消灯			*/
}
/********************************************************
*	関数名： void led_on(char bit1)			*
*	機能　： bit1で指定したビットのLEDを点灯	*
*********************************************************/
void led_on(char bit1)
{
	char data;

	data = PORTB;	/* 現在の状況を確認	*/
	data &= ~bit1;
	PORTB = data;
}
/********************************************************
*	関数名： void led_off(char bit1)		*
*	機能　： bit1で指定したビットのLEDを消灯	*
*********************************************************/
void led_off(char bit1)
{
	char data;

	data = PORTB;	/* 現在の状況を確認	*/
	data |= bit1;
	PORTB = data;
}
/********************************************************
*	関数名： char sw_read(void)			*
*	機能　： スイッチ入力状態を戻り値として返す	*
*********************************************************/
char sw_read(void)
{
	char	data, data2;
	int	t;

	data = PORTA;
	data = ~data & 0x1f;
	if (!(data == CoinA 
	   || data == CoinB
	   || data == CoinC
	   || data == Buy
	   || data == Cancel
	   || data == Exit)) return 0;	/* 単独SWとExit以外はNO PUSHとする*/
	if (data != 0x00) {		/* チャタリング防止用		*/
		for (t = 0; t < 100; t++);
		data2 = PORTA;
		data2 = ~data2 & 0x1f;
		if (data == data2) {
			return data;
		}
	}
	return 0x00;			/* NO PUSH	*/
}

/********************************************************
*	関数名： void DispStatus(char bit1)		*
*	機能　： bit1で指定したステータスのLEDを点灯	*
*********************************************************/
void DispStatus(
char	bit1	/* ONするビット */
){
	led_off(ALL_LED);
	led_on(bit1);
}
/********************************************************
*	関数名： void StampOut(void)			*
*	機能　： STAMP用LEDを点灯			*
*********************************************************/
void  StampOut(int num) {
    if (num >= 3) {
        led_on(LED100);
    }
    if (num >= 2) {
        led_on(STAMP2);
    }
    if (num >= 1) {
        led_on(STAMP1);
    }
}
/********************************************************
*	関数名： int DispCoinA(int num)			*
*	機能　： numで指定した枚数の赤LEDを点灯	(10円用)*
*********************************************************/
int DispCoinA(	/* 正常終了->1, その他->0 を返す */
  int num	/* コインの枚数(1〜MX_DCOIN) */
) {
	/* コインの枚数が範囲内か判定 */
	if (num <= 0 || num > MX_NCOIN) return 0;
	outcoin |= red_led[num];
	return 1;
}
/********************************************************
*	関数名： int DispCoinB(int num)			*
*	機能　： numで指定した枚数のLEDを点灯	(50円用)*
*********************************************************/
int DispCoinB(	/* 正常終了->1, その他->0 を返す */
  int num	/* コインの枚数(1〜MX_DCOIN) */
) {
	/* コインの枚数が範囲内か判定 */
	if (num <= 0 || num > MX_NCOIN) return 0;
	outcoin |= LED50;
	return 1;
}
/********************************************************
*	関数名： void interrrupt isr(void)		*
*	機能　： タイマ割り込み処理ルーチン		*
*********************************************************/
void interrupt isr(void)		/* 割り込み関数	*/
{
	char	i;

	if (T0IF){			/* タイマ0割り込み？		*/
		T0IF=0;			/* タイマ0割り込みフラグを0にする*/
		if(--cnt == 0){		/* cntを-1して結果が0？		*/
			if (outfg) {
				i = PORTB;
				PORTB = (i ^ outcoin);
			}
			cnt=UPDATE;	/* cntにLEDの更新周期を書き込む	*/
		}
	}
}
/********************************************************
*	関数名： void timer(char i)			*
*	機能　： タイマー				*
*********************************************************/
void timer(char i) 
{
	char	j;
	int	t;
	for (j = 0; j < i; j++){
		for (t = 0; t < 1000;t++)
			;
	}
}
/********************************************************
*	関数名： void halt(void)			*
*	機能　： LEDを全点灯し無限ループ		*
*********************************************************/
void halt(void)
{
	PORTB = 0x00;	/* LED all on!!	*/
	for(;;);	/* endless	*/
}
