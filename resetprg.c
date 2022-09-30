

#include	<machine.h>
#include	<_h_c_lib.h>

#include	 "iodefine.h"
#include	 "misratypes.h"


static void operating_frequency_set (void);

#pragma stacksize si=0x200	// 割り込みスタックの定義　(ユーザスタックサイズは使用しない。)
//#pragma stacksize su=0x100
#define PSW_init       0x00010000	// プロセッサステータスワード (割り込み許可)


#pragma section ResetPRG		// output PowerON_Reset to PResetPRG section

#pragma entry PowerON_Reset_PC


void PowerON_Reset_PC(void)
{
	set_extb(__sectop("EXCEPTVECT")); // CPUがRXV2アーキテクチャの場合、例外ベクタテーブルをEXTB(例外テーブルレジスタ)へセット
	set_intb(__sectop("C$VECT"));     // 可変ベクターテーブル(割り込み(タイマー等)の先頭アドレス)を、INTBレジスタにセット

	operating_frequency_set();    // システムクロック 等の設定
	
	_INITSCT();			// 静的変数の初期化 (初期値なしの静的変数=0, 初期値付きの静的変数=初期値)

	
	set_psw(PSW_init);		// 割り込み許可
	
	main();				// メインの起動

	brk();
}



//  クロックの設定
// 設定内容:
//    システムクロック(ICLK) = 32 MHz
//    周辺モジュールクロック(PCLKA) = 32 MHz (MTU2用)
//    周辺モジュールクロック(PCLKB) = 32 MHz (MTU2,S12AD以外の周辺モジュール用)
//    周辺モジュールクロック(PCLKD) = 32 MHz (S12AD A/Dコンバータ用)
//    フラッシュIFクロック(FCLK) = 32 MHz
//

static void operating_frequency_set (void)
{
	
	SYSTEM.PRCR.WORD = 0xA50F;	// クロック発生回路関連レジスタの書き込み許可
	
	SYSTEM.HOCOCR.BIT.HCSTP = 0;	// HOCO動作
	while((SYSTEM.OSCOVFSR.BIT.HCOVF) == 0){	//  高速オンチップオシレータ HOCO発振安定待ち
	}
	
					// 動作電力制御モード　中速動作モードから高速動作モードに設定
	SYSTEM.OPCCR.BIT.OPCM = 0;	// 高速動作モード
	while( SYSTEM.OPCCR.BIT.OPCMTSF == 1 ){  // 遷移待ち
	}
	
	 
	SYSTEM.SCKCR.LONG = 0x00000000;	    // システムクロックの設定 (FCLK=32MHz,ICLK=32MHz,  PCLKB=32MHz ,PCLKD=32MHz)					  
	while( SYSTEM.SCKCR.LONG != 0x00000000 ){
	}
	
	
	SYSTEM.SCKCR3.WORD = 0x0100;		// Clock sourceは、HOCO選択
	while( SYSTEM.SCKCR3.WORD != 0x0100){
	}
	
	
	
	SYSTEM.PRCR.WORD = 0xA500;	// クロック発生回路関連レジスタへの書き込み禁止
}


