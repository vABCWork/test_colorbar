#include	<machine.h>
#include	 "iodefine.h"
#include	 "misratypes.h"
#include	"delay.h"
#include	 "spi.h"
#include	 "ILI9488.h"

void clear_module_stop(void);

void main(void)
{
	
	clear_module_stop();	//  モジュールストップの解除
	
	RSPI_Init_Port();	// RSPI ポートの初期化  (LCDコントローラ用)   
     	RSPI_Init_Reg();        // SPI レジスタの設定  

     	RSPI_SPCMD_0();	        // SPI 転送フォーマットを設定, SSLA0使用	
	
	ILI9488_Reset();	// LCD のリセット	
	 
	ILI9488_Init();		// LCDの初期化
	
	
	while(1){
			
		color_bar();	// 画面　8色表示(白,黄色,シアン,緑,マゼンタ,赤,青,黒 ) 86.5[msec]
	
		delay_msec(500);
	
		disp_black();		// 画面　黒  ( 80 [msec] at 16[MHz] ) 80[msecc]
	
		delay_msec(500);
	}
	
}





// モジュールストップの解除
// コンペアマッチタイマ(CMT) ユニット0(CMT0, CMT1) 
//   シリアルペリフェラルインタフェース0(RSPI)
//
void clear_module_stop(void)
{
	SYSTEM.PRCR.WORD = 0xA50F;	// クロック発生、消費電力低減機能関連レジスタの書き込み許可	
	
	MSTP(CMT0) = 0;			// コンペアマッチタイマ(CMT) ユニット0(CMT0, CMT1) モジュールストップの解除
	
	MSTP(RSPI0) = 0;		// シリアルペリフェラルインタフェース0 モジュールストップの解除
	
	SYSTEM.PRCR.WORD = 0xA500;	// クロック発生、消費電力低減機能関連レジスタ書き込み禁止
}

