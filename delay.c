

//   N [msec] 待つ関数 (ICLK = 32MHz)
//   命令語は、 RX V2 アーキテクチャ
//    コンパイル時　最適化レベル = 2   (-optimize = 2 )
//   n_msec=  1:  1msec
//      

#pragma instalign4 delay_msec

void  delay_msec( unsigned long  n_msec )
{
	unsigned long delay_cnt;

	while( n_msec-- ) {

	   delay_cnt = 10656;
		
  	   while(delay_cnt --) 
	   { 			 
	   }

	}
}


//   
//    5[usec}待つ   
//  E2データフラッシュ　STOP解除時間用 (高速動作モード時)
//

#pragma instalign4 delay_5usec

void  delay_5usec(void)
{
	unsigned long delay_cnt;

	   delay_cnt = 53;
		
	   while(delay_cnt --) 
	   { 			 
	   }

}
