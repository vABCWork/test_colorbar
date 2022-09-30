#include "iodefine.h"
#include  "misratypes.h"
#include "spi.h"

#include "ILI9488.h"



uint8_t rgb111_data_buf[9800];	// �\���p�o�b�t�@ RGB-111�̃f�[�^���i�[ (2pixel��1�o�C�g)
				//�@�J���[�o�[�\���e�X�g�̏ꍇ�́A8�F���@480Wx320H�͈̔͂֕\���@1�F=60Wx320H
                                //  (2dot = 1byte, 1col=320dot= 160byte,  160x60col=9600byte ) 
	
// ILI9488�̏�����
//
// 1) 8�F�Ŏg�p : Idle Mode ON (39h). Interface Pixel Format (3Ah)
// 2) �c�^, �������֏�������: Memory Access Control (36h) ,�p�����[�^=48H
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
//             �B �B�@       �B �B�@       
//�@�@�@�@�@�@VCC GND          T_IRQ
//
// 
// Memory Access Control (36h)
// �p�����[�^ (MADCTL)
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
	 
	 spi_cmd_send(0x13,0,&para[0] );  // Normal Display ON (13h) , �p�����[�^��=0	
	 
	 spi_cmd_send(0x39,0,&para[0] );  // Idle Mode ON (39h) , �p�����[�^��=0 
	 
	 para[0] = 0x51;		  // Interface Pixel Format (3Ah)�̃p�����[�^ , 8�F  (3 bits/pixel) (MCU interface format (SPI)) 
	 spi_cmd_send(0x3A,1,&para[0] );  // Interface Pixel Format (3Ah), �p�����[�^��=1

	 
	para[0] = 0x48;	   		   //  �R�}���h���W�X�^ 0x36�p�p�����[�^ (MY=0,MX=1,MV=0)
	spi_cmd_send( 0x36,1,&para[0]);    // Memory access control  �R�}���h���W�X�^ 0x36 , �p�����[�^��=1
	 
	
				          // Frame Rate Control (In Idle Mode/8 Colors) (B2h)
	 para[0] = 0x00;    		  //  �R�}���h���W�X�^ 0xB1�p�p�����[�^
	 para[1] = 0x10;	 
	 spi_cmd_send(0xB2 ,2,&para[0] );      // �R�}���h���W�X�^ 0xB2, �p�����[�^��=2
	 
	 
	 			   // (Extend Command)Display Function Control (B6h) 
	para[0] = 0x02;    	   //  �R�}���h���W�X�^ 0xB6�p�p�����[�^
	para[1] = 0x02;            //
	para[2] = 0x3b;             // 
	spi_cmd_send(0xB6,3,&para[0]);      //  �R�}���h���W�X�^ 0xB6 , �p�����[�^��=2
	 
	 
				    // Display ON (29h)
	spi_cmd_send(0x29,0,&para[0]);       // Display On �R�}���h���W�X�^ 0x29 , �p�����[�^��=0
	
	spi_cmd_send(0x11,0,&para[0]);      // Sleep OUT (11h) , �p�����[�^��=0
	 
	delay_msec(5);	    	    	   //  5msec�҂�
	  
}



// LCD �̃��Z�b�g
// 
void ILI9488_Reset(void)
{

     	
	LCD_RESET_PODR = 0;              // LCD ���Z�b�g��Ԃɂ���
	delay_msec(1);		        // 1[msec] �҂� 
	
	LCD_RESET_PODR = 1;             // LCD �񃊃Z�b�g���
	delay_msec(200);	        // 200[msec] �҂� 
	
	
}





//  �\���͈͂̐ݒ�
// ����:
//  col: �J�n�J����(x), page(row):�J�n�y�[�W(y)
//  col2:�I���J����, page2(row2): �I���y�[�W
//
void lcd_adrs_set( uint16_t col, uint16_t page, uint16_t col2, uint16_t page2)
{
	uint8_t para[8];
	
	 para[0] = (uint8_t) ((0xff00 & col) >> 8);     //  SC[15:8]�@�X�^�[�g�J����(16bit)�̏�ʃo�C�g
	 para[1] = (uint8_t) (0x00ff & col);            //  SC[7:0]         :�@�@�@�@�@�@�@�@���ʃo�C�g 
	 para[2] = (uint8_t) ((0xff00 & col2) >> 8);    //  EC[15:8]�@�I���J����(16bit)�̏�ʃo�C�g
	 para[3] = (uint8_t) (0x00ff & col2);           //  EC[7:0]         :�@�@�@�@�@�@�@�@���ʃo�C�g 
	 	 
	 spi_cmd_send(0x2a,4,&para[0]);      // Column Address Set �R�}���h���W�X�^ 0x2a , �p�����[�^��=4
	
	
	 para[0] = (uint8_t) ((0xff00 & page ) >> 8);    //  SP[15:8]�@�X�^�[�g�y�[�W(16bit)�̏�ʃo�C�g
	 para[1] = (uint8_t) (0x00ff & page);            //  SP[7:0]         :�@�@�@�@�@�@�@�@���ʃo�C�g 
	 para[2] = (uint8_t) ((0xff00 & page2 ) >> 8);    // EP[15:8]�@�I���y�[�W(16bit)�̏�ʃo�C�g
	 para[3] = (uint8_t) (0x00ff & page2);            // EP[7:0]         :�@�@�@�@�@�@�@�@���ʃo�C�g 
	 
	 spi_cmd_send(0x2b,4,&para[0]);      // Page Address Set �R�}���h���W�X�^ 0x2b , �p�����[�^��=4
		 
}

	
//  ILI9488  LCD �J���[�o�[(8�F) 
//   (320x480)
//
//   1byte �� 2pixel����RGB���  
//   1byte = **RG BRGB

// �F       RGB     2pixel���
// ��       111 ,  0011 1111(=0x3f)
// ���F     110 ,  0011 0110(=0x36)
// �V�A��   011 ,  0001 1011(=0x1b)
// ��       010 ,  0001 0010(=0x12)
// �}�[���^ 101 ,  0010 1101(=0x2d)
// ��       100 ,  0010 0100(=0x24)
// ��       001 ,  0000 1001(=0x09)
// ��       000 ,  0000 0000(=0x00)
//


void color_bar(void)
{
	uint32_t i;
	uint32_t num;
	
	uint8_t para[4];
	
	lcd_adrs_set(0,0, 319,479);	  // �������ݔ͈͎w��(�R�}���h 2a�ƃR�}���h 2b) (�J�n�J����=0, �J�n�y�[�W=0, �I���J����=319, �I���y�[�W=479)
        
	num = (320 / 2 ) * 60; 		// 2pixel/1byte, 480/8�F=60 page
	
	
         for ( i = 0; i < num ; i++)	// �s�N�Z���f�[�^�𗬂����� (60�y�[�W��) 	
         {
	     rgb111_data_buf[i] = 0x3f;        //  �� (2pixel ��)
         }
	
	 spi_cmd_send(0x2c,0,&para[0]);		// Memory Write (2Ch)  �擪�ʒu(�R�}���h2a,2b�Ŏw�肵���ʒu)����f�[�^��������
	 spi_data_send(num, &rgb111_data_buf[0]);  // �f�[�^���M

	 delay_5usec();		// 5 usec�҂�
	 
	 for ( i = 0; i < num ; i++)	// �s�N�Z���f�[�^�𗬂�����	
         {
	     rgb111_data_buf[i] = 0x36;        // ���F (2pixel ��)
         }
	 
	  spi_cmd_send(0x3c,0,&para[0]);	// Memory Write Continue (3Ch)
	  spi_data_send(num, &rgb111_data_buf[0]);  // �f�[�^���M
	 
	  delay_5usec();		// 5 usec�҂�
		
          for ( i = 0; i < num ; i++)	// �s�N�Z���f�[�^�𗬂�����	
         {
	     rgb111_data_buf[i] = 0x1b;        // �V�A��  (2pixel ��)
         }
	 spi_cmd_send(0x3c,0,&para[0]);	// Memory Write Continue (3Ch)
	 spi_data_send(num, &rgb111_data_buf[0]);  // �f�[�^���M
	
	 delay_5usec();		// 5 usec�҂�
	  
         for ( i = 0; i < num ;i++)	// �s�N�Z���f�[�^�𗬂�����	
         {
	     rgb111_data_buf[i] = 0x12;        // ��
         }
	 spi_cmd_send(0x3c,0,&para[0]);	// Memory Write Continue (3Ch)
	 spi_data_send(num, &rgb111_data_buf[0]);  // �f�[�^���M
	
	 delay_5usec();		// 5 usec�҂�
	 
         for ( i = 0; i < num; i++)	// �s�N�Z���f�[�^�𗬂�����	
         {
	     rgb111_data_buf[i] = 0x2d;        // �}�[���_
         }
	  spi_cmd_send(0x3c,0,&para[0]);	// Memory Write Continue (3Ch)
	  spi_data_send(num, &rgb111_data_buf[0]);  // �f�[�^���M
	
	 delay_5usec();		// 5 usec�҂�
	 
	 for ( i = 0; i < num; i++)	// �s�N�Z���f�[�^�𗬂�����	
         {
	     rgb111_data_buf[i] = 0x24;        // ��
         }
	  spi_cmd_send(0x3c,0,&para[0]);	// Memory Write Continue (3Ch)
	  spi_data_send(num, &rgb111_data_buf[0]);  // �f�[�^���M
	  
	  delay_5usec();		// 5 usec�҂�
	
         for ( i = 0; i < num; i++)	// �s�N�Z���f�[�^�𗬂�����	
         {
	     rgb111_data_buf[i] = 0x09;        // ��
         }
	  spi_cmd_send(0x3c,0,&para[0]);	// Memory Write Continue (3Ch)
	  spi_data_send(num, &rgb111_data_buf[0]);  // �f�[�^���M
	 
	  delay_5usec();		// 5 usec�҂�
	   
         for ( i = 0; i < num; i++)	// �s�N�Z���f�[�^�𗬂�����	
         {
	     rgb111_data_buf[i] = 0x00;        // ��
         }
	  spi_cmd_send(0x3c,0,&para[0]);	// Memory Write Continue (3Ch)
	  spi_data_send(num, &rgb111_data_buf[0]);  // �f�[�^���M
	
}


//
// ��ʂ����ɂ���
//
void disp_black(void)
{
	uint32_t i;
	uint8_t para[4];
	
	lcd_adrs_set(0,0, 319,479);	  // �������ݔ͈͎w��(�R�}���h 2a�ƃR�}���h 2b) (�J�n�J����=0, �J�n�y�[�W=0, �I���J����=319, �I���y�[�W=479)

         for ( i = 0; i < 9600 ; i++)	// �s�N�Z���f�[�^�𗬂����� (40�y�[�W��)  (1page�����̂� 480 / 2 = 240byte�K�v , (1 pixel=2 byte) ) 	
         {
	     rgb111_data_buf[i] = 0x00;        //  �� (2pixel ��)
         }
	
	 spi_cmd_send(0x2c,0,&para[0]);		// Memory Write (2Ch)  �擪�ʒu(�R�}���h2a,2b�Ŏw�肵���ʒu)����f�[�^��������
	 spi_data_send(9600, &rgb111_data_buf[0]);  // �f�[�^���M
	 
	 spi_cmd_send(0x3c,0,&para[0]);	// Memory Write Continue (3Ch)
	 spi_data_send(9600, &rgb111_data_buf[0]);  // �f�[�^���M
	 
	 spi_cmd_send(0x3c,0,&para[0]);	// Memory Write Continue (3Ch)
	 spi_data_send(9600, &rgb111_data_buf[0]);  // �f�[�^���M
	 
	 spi_cmd_send(0x3c,0,&para[0]);	// Memory Write Continue (3Ch)
	 spi_data_send(9600, &rgb111_data_buf[0]);  // �f�[�^���M
	 
	 spi_cmd_send(0x3c,0,&para[0]);	// Memory Write Continue (3Ch)
	 spi_data_send(9600, &rgb111_data_buf[0]);  // �f�[�^���M
	 
	 spi_cmd_send(0x3c,0,&para[0]);	// Memory Write Continue (3Ch)
	 spi_data_send(9600, &rgb111_data_buf[0]);  // �f�[�^���M
	 
	 spi_cmd_send(0x3c,0,&para[0]);	// Memory Write Continue (3Ch)
	 spi_data_send(9600, &rgb111_data_buf[0]);  // �f�[�^���M
	 
	 spi_cmd_send(0x3c,0,&para[0]);	// Memory Write Continue (3Ch)
	 spi_data_send(9600, &rgb111_data_buf[0]);  // �f�[�^���M
	 
}



