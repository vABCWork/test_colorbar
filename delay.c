

//   N [msec] �҂֐� (ICLK = 32MHz)
//   ���ߌ�́A RX V2 �A�[�L�e�N�`��
//    �R���p�C�����@�œK�����x�� = 2   (-optimize = 2 )
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
//    5[usec}�҂�   
//  E2�f�[�^�t���b�V���@STOP�������ԗp (�������샂�[�h��)
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
