#include "iodefine.h"
#include  "misratypes.h"
#include "spi.h"

#include "ILI9488.h"



uint8_t rgb111_data_buf[9800];	// 表示用バッファ RGB-111のデータを格納 (2pixelで1バイト)
				//　カラーバー表示テストの場合は、8色を　480Wx320Hの範囲へ表示　1色=60Wx320H
                                //  (2dot = 1byte, 1col=320dot= 160byte,  160x60col=9600byte ) 
	
// ILI9488の初期化
//
// 1) 8色で使用 : Idle Mode ON (39h). Interface Pixel Format (3Ah)
// 2) 縦型, →方向へ書き込み: Memory Access Control (36h) ,パラメータ=48H
//
//                  Clolumn
//            0                 319
//          0 +------------------+
// Page(Row)  |                  |
//            |                  |
//            |                  |
//            |                  |
//            |                  |
//            |                  |
//            |                  |    
//            |                  |
//            |                  |
//            |                  |
//            |                  |
//       479  |                  |
//            +------------------+
//             。 。　       。 。　       
//　　　　　　VCC GND          T_IRQ
//
// 
// Memory Access Control (36h)
// パラメータ (MADCTL)
//  b7 : MY (Row address order)    0=Top to bottomm, 1= Bottom to top	
//  b6 : MX (Column address order) 0=Left to right,  1= Right to Left
//  b5 : MV (Row/Column exchange)
//  b4 : Vertical Refresh Order
//  b3 : RGB-BGR Order 0=RGB color filter panel, 1=BGR color filter panel
//  b2 : Horizontal Refresh ORDER
//  b1 : Reserved
//  b0 : Reserved
//	

void ILI9488_Init(void)
{
	uint8_t para[4];
	 
	 spi_cmd_send(0x13,0,&para[0] );  // Normal Display ON (13h) , パラメータ数=0	
	 
	 spi_cmd_send(0x39,0,&para[0] );  // Idle Mode ON (39h) , パラメータ数=0 
	 
	 para[0] = 0x51;		  // Interface Pixel Format (3Ah)のパラメータ , 8色  (3 bits/pixel) (MCU interface format (SPI)) 
	 spi_cmd_send(0x3A,1,&para[0] );  // Interface Pixel Format (3Ah), パラメータ数=1

	 
	para[0] = 0x48;	   		   //  コマンドレジスタ 0x36用パラメータ (MY=0,MX=1,MV=0)
	spi_cmd_send( 0x36,1,&para[0]);    // Memory access control  コマンドレジスタ 0x36 , パラメータ数=1
	 
	
				          // Frame Rate Control (In Idle Mode/8 Colors) (B2h)
	 para[0] = 0x00;    		  //  コマンドレジスタ 0xB1用パラメータ
	 para[1] = 0x10;	 
	 spi_cmd_send(0xB2 ,2,&para[0] );      // コマンドレジスタ 0xB2, パラメータ数=2
	 
	 
	 			   // (Extend Command)Display Function Control (B6h) 
	para[0] = 0x02;    	   //  コマンドレジスタ 0xB6用パラメータ
	para[1] = 0x02;            //
	para[2] = 0x3b;             // 
	spi_cmd_send(0xB6,3,&para[0]);      //  コマンドレジスタ 0xB6 , パラメータ数=2
	 
	 
				    // Display ON (29h)
	spi_cmd_send(0x29,0,&para[0]);       // Display On コマンドレジスタ 0x29 , パラメータ数=0
	
	spi_cmd_send(0x11,0,&para[0]);      // Sleep OUT (11h) , パラメータ数=0
	 
	delay_msec(5);	    	    	   //  5msec待ち
	  
}



// LCD のリセット
// 
void ILI9488_Reset(void)
{

     	
	LCD_RESET_PODR = 0;              // LCD リセット状態にする
	delay_msec(1);		        // 1[msec] 待つ 
	
	LCD_RESET_PODR = 1;             // LCD 非リセット状態
	delay_msec(200);	        // 200[msec] 待つ 
	
	
}





//  表示範囲の設定
// 入力:
//  col: 開始カラム(x), page(row):開始ページ(y)
//  col2:終了カラム, page2(row2): 終了ページ
//
void lcd_adrs_set( uint16_t col, uint16_t page, uint16_t col2, uint16_t page2)
{
	uint8_t para[8];
	
	 para[0] = (uint8_t) ((0xff00 & col) >> 8);     //  SC[15:8]　スタートカラム(16bit)の上位バイト
	 para[1] = (uint8_t) (0x00ff & col);            //  SC[7:0]         :　　　　　　　　下位バイト 
	 para[2] = (uint8_t) ((0xff00 & col2) >> 8);    //  EC[15:8]　終了カラム(16bit)の上位バイト
	 para[3] = (uint8_t) (0x00ff & col2);           //  EC[7:0]         :　　　　　　　　下位バイト 
	 	 
	 spi_cmd_send(0x2a,4,&para[0]);      // Column Address Set コマンドレジスタ 0x2a , パラメータ数=4
	
	
	 para[0] = (uint8_t) ((0xff00 & page ) >> 8);    //  SP[15:8]　スタートページ(16bit)の上位バイト
	 para[1] = (uint8_t) (0x00ff & page);            //  SP[7:0]         :　　　　　　　　下位バイト 
	 para[2] = (uint8_t) ((0xff00 & page2 ) >> 8);    // EP[15:8]　終了ページ(16bit)の上位バイト
	 para[3] = (uint8_t) (0x00ff & page2);            // EP[7:0]         :　　　　　　　　下位バイト 
	 
	 spi_cmd_send(0x2b,4,&para[0]);      // Page Address Set コマンドレジスタ 0x2b , パラメータ数=4
		 
}

	
//  ILI9488  LCD カラーバー(8色) 
//   (320x480)
//
//   1byte に 2pixel分のRGB情報  
//   1byte = **RG BRGB

// 色       RGB     2pixel情報
// 白       111 ,  0011 1111(=0x3f)
// 黄色     110 ,  0011 0110(=0x36)
// シアン   011 ,  0001 1011(=0x1b)
// 緑       010 ,  0001 0010(=0x12)
// マゼンタ 101 ,  0010 1101(=0x2d)
// 赤       100 ,  0010 0100(=0x24)
// 青       001 ,  0000 1001(=0x09)
// 黒       000 ,  0000 0000(=0x00)
//


void color_bar(void)
{
	uint32_t i;
	uint32_t num;
	
	uint8_t para[4];
	
	lcd_adrs_set(0,0, 319,479);	  // 書き込み範囲指定(コマンド 2aとコマンド 2b) (開始カラム=0, 開始ページ=0, 終了カラム=319, 終了ページ=479)
        
	num = (320 / 2 ) * 60; 		// 2pixel/1byte, 480/8色=60 page
	
	
         for ( i = 0; i < num ; i++)	// ピクセルデータを流し込む (60ページ分) 	
         {
	     rgb111_data_buf[i] = 0x3f;        //  白 (2pixel 分)
         }
	
	 spi_cmd_send(0x2c,0,&para[0]);		// Memory Write (2Ch)  先頭位置(コマンド2a,2bで指定した位置)からデータ書き込み
	 spi_data_send(num, &rgb111_data_buf[0]);  // データ送信

	 delay_5usec();		// 5 usec待ち
	 
	 for ( i = 0; i < num ; i++)	// ピクセルデータを流し込む	
         {
	     rgb111_data_buf[i] = 0x36;        // 黄色 (2pixel 分)
         }
	 
	  spi_cmd_send(0x3c,0,&para[0]);	// Memory Write Continue (3Ch)
	  spi_data_send(num, &rgb111_data_buf[0]);  // データ送信
	 
	  delay_5usec();		// 5 usec待ち
		
          for ( i = 0; i < num ; i++)	// ピクセルデータを流し込む	
         {
	     rgb111_data_buf[i] = 0x1b;        // シアン  (2pixel 分)
         }
	 spi_cmd_send(0x3c,0,&para[0]);	// Memory Write Continue (3Ch)
	 spi_data_send(num, &rgb111_data_buf[0]);  // データ送信
	
	 delay_5usec();		// 5 usec待ち
	  
         for ( i = 0; i < num ;i++)	// ピクセルデータを流し込む	
         {
	     rgb111_data_buf[i] = 0x12;        // 緑
         }
	 spi_cmd_send(0x3c,0,&para[0]);	// Memory Write Continue (3Ch)
	 spi_data_send(num, &rgb111_data_buf[0]);  // データ送信
	
	 delay_5usec();		// 5 usec待ち
	 
         for ( i = 0; i < num; i++)	// ピクセルデータを流し込む	
         {
	     rgb111_data_buf[i] = 0x2d;        // マゼンダ
         }
	  spi_cmd_send(0x3c,0,&para[0]);	// Memory Write Continue (3Ch)
	  spi_data_send(num, &rgb111_data_buf[0]);  // データ送信
	
	 delay_5usec();		// 5 usec待ち
	 
	 for ( i = 0; i < num; i++)	// ピクセルデータを流し込む	
         {
	     rgb111_data_buf[i] = 0x24;        // 赤
         }
	  spi_cmd_send(0x3c,0,&para[0]);	// Memory Write Continue (3Ch)
	  spi_data_send(num, &rgb111_data_buf[0]);  // データ送信
	  
	  delay_5usec();		// 5 usec待ち
	
         for ( i = 0; i < num; i++)	// ピクセルデータを流し込む	
         {
	     rgb111_data_buf[i] = 0x09;        // 青
         }
	  spi_cmd_send(0x3c,0,&para[0]);	// Memory Write Continue (3Ch)
	  spi_data_send(num, &rgb111_data_buf[0]);  // データ送信
	 
	  delay_5usec();		// 5 usec待ち
	   
         for ( i = 0; i < num; i++)	// ピクセルデータを流し込む	
         {
	     rgb111_data_buf[i] = 0x00;        // 黒
         }
	  spi_cmd_send(0x3c,0,&para[0]);	// Memory Write Continue (3Ch)
	  spi_data_send(num, &rgb111_data_buf[0]);  // データ送信
	
}


//
// 画面を黒にする
//
void disp_black(void)
{
	uint32_t i;
	uint8_t para[4];
	
	lcd_adrs_set(0,0, 319,479);	  // 書き込み範囲指定(コマンド 2aとコマンド 2b) (開始カラム=0, 開始ページ=0, 終了カラム=319, 終了ページ=479)

         for ( i = 0; i < 9600 ; i++)	// ピクセルデータを流し込む (40ページ分)  (1page書くのに 480 / 2 = 240byte必要 , (1 pixel=2 byte) ) 	
         {
	     rgb111_data_buf[i] = 0x00;        //  黒 (2pixel 分)
         }
	
	 spi_cmd_send(0x2c,0,&para[0]);		// Memory Write (2Ch)  先頭位置(コマンド2a,2bで指定した位置)からデータ書き込み
	 spi_data_send(9600, &rgb111_data_buf[0]);  // データ送信
	 
	 spi_cmd_send(0x3c,0,&para[0]);	// Memory Write Continue (3Ch)
	 spi_data_send(9600, &rgb111_data_buf[0]);  // データ送信
	 
	 spi_cmd_send(0x3c,0,&para[0]);	// Memory Write Continue (3Ch)
	 spi_data_send(9600, &rgb111_data_buf[0]);  // データ送信
	 
	 spi_cmd_send(0x3c,0,&para[0]);	// Memory Write Continue (3Ch)
	 spi_data_send(9600, &rgb111_data_buf[0]);  // データ送信
	 
	 spi_cmd_send(0x3c,0,&para[0]);	// Memory Write Continue (3Ch)
	 spi_data_send(9600, &rgb111_data_buf[0]);  // データ送信
	 
	 spi_cmd_send(0x3c,0,&para[0]);	// Memory Write Continue (3Ch)
	 spi_data_send(9600, &rgb111_data_buf[0]);  // データ送信
	 
	 spi_cmd_send(0x3c,0,&para[0]);	// Memory Write Continue (3Ch)
	 spi_data_send(9600, &rgb111_data_buf[0]);  // データ送信
	 
	 spi_cmd_send(0x3c,0,&para[0]);	// Memory Write Continue (3Ch)
	 spi_data_send(9600, &rgb111_data_buf[0]);  // データ送信
	 
}



