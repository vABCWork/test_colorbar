#include "iodefine.h"
#include "misratypes.h"

#include "spi.h"


volatile uint8_t *spi_send_pt;		// 送信データを格納している場所

volatile uint32_t spi_send_num;	// 送信するバイト数



// RSPI0 SPTI0
// 送信バッファエンプティ割り込み
//
//    RSPI0.SPDR.LONGには、32bit分がセットされる。
//   (送信は、8bit毎に実施される。)
//   32bit = 4byte , 1byte は2pixelのデータのため、1回の送信で、8pixel分のデータが送信される。
//

#pragma interrupt (Excep_RSPI0_SPTI0(vect=46))
void Excep_RSPI0_SPTI0(void){

	RSPI0.SPDR.LONG = (uint32_t)(*spi_send_pt);        // 送信データセット
	
	spi_send_num = spi_send_num  - 1;	// 送信回数をデクリメント
	spi_send_pt++;				// 送信位置の更新
	
	if ( spi_send_num ==  0) {             //  最終送信データの送信完了
		RSPI0.SPCR.BIT.SPTIE = 0;       // 送信バッファエンプティ割り込み要求の発生を禁止
		RSPI0.SPCR.BIT.SPE = 0;        // RSPI機能は無効
	}
	
}




// RSPI ポートの初期化  (LCDコントローラ用)
// 　マスタ: マイコン
//   スレーブ:  LCDコントローラ(ILI9488)
//
// PC6 : MOSI     ,SPI_MOSI ,マスタ出力 スレーブ入力   
// PC5 : RSPCKA   ,SPI_CLK  ,クロック出力  
// PC4 : SSLA0    ,DISP_CS  ,スレーブセレクト 
// PH3 : port出力 ,DISP_RST, LCDコントローラ(ILI9488) Reset用 
// PC7 : port出力 ,DISP_DC ,LCDコントローラ(ILI9488)  コマンド/データ判別用　(コマンド=low,データ=high)  
//

void RSPI_Init_Port(void)
{
	
	MPC.PWPR.BIT.B0WI = 0;  	 // マルチファンクションピンコントローラ　プロテクト解除
        MPC.PWPR.BIT.PFSWE = 1;  	// PmnPFS ライトプロテクト解除
    
	MPC.PC6PFS.BYTE = 0x0d;		// PC6 = MOSIA   Master Out slave in
	MPC.PC5PFS.BYTE = 0x0d;	  	// PC5 = RSPCKA
	MPC.PC4PFS.BYTE = 0x0d;		// PC4 = SSLA0
	
        MPC.PWPR.BYTE = 0x80;      	//  PmnPFS ライトプロテクト 設定	
	
        PORTC.PMR.BIT.B6 = 1;     	// PC6  周辺機能として使用
	PORTC.PMR.BIT.B5 = 1;     	// PC5  
	PORTC.PMR.BIT.B4 = 1;     	// PC4
	
	PORTC.DSCR.BIT.B6 = 1;     	// PC6 高駆動出力
	PORTC.DSCR.BIT.B5 = 1;     	// PC5  
	PORTC.DSCR.BIT.B4 = 1;     	// PC4
	
	
	LCD_RESET_PMR  = 0;     	// PH3:汎用入出力ポート ,LEDコントローラ Reset用 
	LCD_RESET_PDR = 1;      	// PH3: 出力ポートに設定 LEDコントローラへのReset
	
        LCD_DC_PMR = 0;        		// PC7:汎用入出力ポート, LEDコントローラ data/command切り替え
	LCD_DC_PDR = 1;        		// PC7: 出力ポートに設定 LEDコントローラ data/command切り替え     　  　
	 
}




// RSPI レジスタの初期設定 
//ビットレート:
//  RSPI コマンドレジスタ0 ～ 7（SPCMD0～7)のBRDV[1:0] ビットとSPBR レジスタの設定値(n)の組み合わせできまる。
// 
//  現在、RSPI0.SPCMD0のBRDV=0 ,   PCLKB= 32MHzとしている。
//   ビットレート = PCLKB / 2*(SPBR+1)
//   
//
//     SPBR = 0 で、32/2 = 16 [MHz]
//     SPBR = 1 で、32/4 = 8 [MHz]
//

void RSPI_Init_Reg(void)
{

	RSPI0.SPCR.BIT.SPMS = 0;        // SPI動作 (4線式)	
	RSPI0.SPCR.BIT.TXMD = 1;        // 送信動作のみ行う。受信なし
	RSPI0.SPCR.BIT.MODFEN = 0;      // モードフォルトエラー検出を禁止
	
	RSPI0.SPCR.BIT.MSTR = 1;	//  マスタモード
	
	
	RSPI0.SPCR.BIT.SPEIE =0;       // エラー割り込み要求の発生を禁止
	RSPI0.SPCR.BIT.SPTIE =0;       // 送信バッファエンプティ割り込み要求の発生を禁止
	RSPI0.SPCR.BIT.SPE = 0;        // RSPI機能は無効
	RSPI0.SPCR.BIT.SPRIE = 0;      // RSPI受信バッファフル割り込み要求の発生を禁止
	
	RSPI0.SSLP.BYTE = 0;                // スレーブセレクト極性 SSL0～SSL3 アクティブLow
	RSPI0.SPPCR.BYTE = 0;               //  通常モード
	
	RSPI0.SPSCR.BYTE = 0;               // 転送フォーマットは、常にSPCMD0 レジスタを参照

	
	// ILI9488 17.4.3. DBI Type C Option 3 (4-Line SPI System) Timing Characteristics より、Serial clock cycle (Write) = 50nsec (min)
	// 書き込み時には、20MHzが最高
	RSPI0.SPBR = 0x0;		   // 16 MHz
	
	
	RSPI0.SPDCR.BIT.SPFC =0x00;    // SPDR レジスタに格納できる（1 回の転送起動）フレーム数 1 =(32bit, 4byte)
	RSPI0.SPDCR.BIT.SPRDTD = 0;     // 0：SPDRは受信バッファを読み出す
	RSPI0.SPDCR.BIT.SPLW = 1;	// RSPIデータレジスタ(SPDR)はロングワード(32bit)のアクセス

                                       // SPCMD0.BIT.SCKDEN = 1の場合の、
	RSPI0.SPCKD.BYTE = 0;               // セレクト信号からクロック発生までの遅延時間　1RSPCK
				       // SPCMD0.BIT.SCKDEN = 0の場合、セレクト信号からクロック発生までの遅延時間は、1RSPCK
			
	RSPI0.SSLND.BYTE = 0;          // SPCMD0.BIT.SLNDEN = 1の場合の、
	                              // クロック最終エッジから、セレクト解除までの遅延時間
	                              // SPCMD0.BIT.SLNDEN = 0の場合、1RSPCK
				      
	RSPI0.SPND.BYTE = 0;	     // SPCMD0.BIT.SPNDEN = 1の場合の、シリアル転送終了後の、セレクト信号非アクティブ期間（次アクセス遅延)　1RSPCK＋2PCLK	      
				     // SPCMD0.BIT.SPNDEN = 0の場合、1RSPCK＋2PCLK
	
        RSPI0.SPCR2.BYTE = 0;        // パリティなし　アイドル割り込み要求の発生を禁止　アイドル割り込み要求の発生を禁止

					// 送信データエンプティ割り込み
	IPR(RSPI0,SPTI0) = 0x06;	// 割り込みレベル = 6　　（15が最高レベル)
	IR(RSPI0,SPTI0) = 0;		// 割り込み要求のクリア
	IEN(RSPI0,SPTI0) = 1;		// 割り込み許可	
	
				      
}






//     SPCMD 0 の設定
//   
//     SPI モード3: クロックアイドル時 High
//                :  (クロック立ち上がり時にデータサンプル)

//　　　SSL信号   : SSLA0

void RSPI_SPCMD_0(void)
{

					// SPI モード 3 : 表示 OK
	RSPI0.SPCMD0.BIT.CPOL = 1;	// CPOL= 1 負パルス(クロックアイドル時 High)				
	RSPI0.SPCMD0.BIT.CPHA = 1;	// CPHA= 1 奇数エッジでデータ変化、偶数エッジでデータサンプル (エッジは1から数える）

	
	RSPI0.SPCMD0.BIT.BRDV = 0;	// BRDV= 0 ベースのビットレート使用 (分周なし)
	RSPI0.SPCMD0.BIT.SSLA = 0;	// SSLA= 0 SSL0を使用
	
	RSPI0.SPCMD0.BIT.SSLKP = 0;	// 転送終了後(このプログラムの場合、1byte転送)全SSL信号をネゲート
	
	RSPI0.SPCMD0.BIT.SPB = 0x07;	//  SPB= 7  送信 データは 8 bit
	RSPI0.SPCMD0.BIT.LSBF = 0;	// LSBF= 0  MSB first
	
	RSPI0.SPCMD0.BIT.SPNDEN = 0;	// SPNDEN=0 次アクセス遅延は、1RSPCK + 2PCLK
	RSPI0.SPCMD0.BIT.SLNDEN = 0;	// SLNDEN=0 SSLネゲート遅延は 1RSPCK
	RSPI0.SPCMD0.BIT.SCKDEN = 0;	//  SCKDEN=0 RSPCK遅延は 1RSPCK
	
}


// LCDコントローラ(ILI9488)への表示データを送信 　(送信割り込み使用)
//
void spi_data_send( uint32_t para_num, uint8_t *data_pt)
{
					   
        LCD_DC_PODR = 1;     			// データ指定 (D/Cポート= High)
	
	spi_send_pt = data_pt;   		// 送信 割り込みで送信するデータの先頭位置
	spi_send_num = para_num;		// 送信割り込みでの送信数 (1回の送信割り込みで、32bit分セット) 
	
	
						// 送信開始時の送信バッファエンプティ割り込み要求は、
						// SPTIE ビットと同時または後に、SPE ビットを“1” にすることで発生します。(page 1010)
	
	RSPI0.SPCR.BIT.SPTIE = 1;       	// 送信バッファエンプティ割り込み要求の発生を許可
	RSPI0.SPCR.BIT.SPE = 1;        		// RSPI 機能有効
						
	
	
	
}



// LCDコントローラ(ILI9488)へのコマンドとパラメータ(データ)送信 
// 送信バッファエンプティフラグのポーリングで、コマンド、パラメータ送信
//
// コマンドに送信パラメータが無い場合、cmdだけ送信。para_num = 0 設定。
// コマンドに送信パラメータがある場合、cmd送信後、para[]をpara_num分送信。
//
//　　入力: cmd コマンド(=レジスタID) ( 8 bit)
//          sd_num :送信パラメータ数 
//          *para_pt: 送信パラメータの格納位置。送信パラメータは、para[]に格納されている。
//   
//
void spi_cmd_send( uint8_t cmd, uint32_t para_num, uint8_t *para_pt)
{
        uint32_t i;
	uint32_t temp;
	
        RSPI0.SPCR.BIT.SPE = 1;        // RSPI 機能有効	
	
	temp =  RSPI0.SPSR.BIT.IDLNF;		//　空読み  (0=RSPIアイドル状態, 1=RSP転送中)  (page= 1069)
	while( RSPI0.SPSR.BIT.IDLNF ==1 );	// 転送中はループ (割り込みで送信したデータの、全データの転送終了待ち)
	
	
	while( RSPI0.SPSR.BIT.SPTEF !=1 );       // 送信バッファがエンプティでない(=送信バッファに有効なデータがある)場合は、whileループ 
	                                         // 送信バッファからシフトレジスタへ送信データが転送された時に SPTEF=1になる。（クロック毎に1ビット毎に送信中) 
	LCD_DC_PODR = 0;	   		// D/Cポート = Low (コマンド)
	
	RSPI0.SPDR.LONG = (uint32_t)cmd;        // コマンド(レジスタID)送信開始
	
	
	temp =  RSPI0.SPSR.BIT.IDLNF;		//　空読み  (0=RSPIアイドル状態, 1=RSP転送中)  (page= 1069) 
	while( RSPI0.SPSR.BIT.IDLNF ==1 );	// 転送中はループ  
	  
	
	if ( para_num >  0 ) {    // 送信パラメータ付きの場合
	
	  LCD_DC_PODR = 1;     			// D/Cポート= High (データ)
	
	  for ( i = 0 ;  i  < para_num ; i++ ) {
	      while( RSPI0.SPSR.BIT.SPTEF !=1 );      // 送信バッファがエンプティでない(=送信バッファに有効なデータがある)場合は、whileループ 
	      RSPI0.SPDR.LONG = (uint32_t)(*para_pt);        // パラメータ送信開始	
	      para_pt ++;			// パラメータ位置の更新
	  }
	  
	  temp =  RSPI0.SPSR.BIT.IDLNF;		//　空読み  (0=RSPIアイドル状態, 1=RSP転送中)  (page= 1069) 
	  while( RSPI0.SPSR.BIT.IDLNF ==1 );	// 転送中はループ  
	
	}
	
	RSPI0.SPCR.BIT.SPE = 0;        // RSPI 機能無効	
}



