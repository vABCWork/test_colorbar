#include "iodefine.h"
#include "misratypes.h"

#include "spi.h"


volatile uint8_t *spi_send_pt;		// ���M�f�[�^���i�[���Ă���ꏊ

volatile uint32_t spi_send_num;	// ���M����o�C�g��



// RSPI0 SPTI0
// ���M�o�b�t�@�G���v�e�B���荞��
//
//    RSPI0.SPDR.LONG�ɂ́A32bit�����Z�b�g�����B
//   (���M�́A8bit���Ɏ��{�����B)
//   32bit = 4byte , 1byte ��2pixel�̃f�[�^�̂��߁A1��̑��M�ŁA8pixel���̃f�[�^�����M�����B
//

#pragma interrupt (Excep_RSPI0_SPTI0(vect=46))
void Excep_RSPI0_SPTI0(void){

	RSPI0.SPDR.LONG = (uint32_t)(*spi_send_pt);        // ���M�f�[�^�Z�b�g
	
	spi_send_num = spi_send_num  - 1;	// ���M�񐔂��f�N�������g
	spi_send_pt++;				// ���M�ʒu�̍X�V
	
	if ( spi_send_num ==  0) {             //  �ŏI���M�f�[�^�̑��M����
		RSPI0.SPCR.BIT.SPTIE = 0;       // ���M�o�b�t�@�G���v�e�B���荞�ݗv���̔������֎~
		RSPI0.SPCR.BIT.SPE = 0;        // RSPI�@�\�͖���
	}
	
}




// RSPI �|�[�g�̏�����  (LCD�R���g���[���p)
// �@�}�X�^: �}�C�R��
//   �X���[�u:  LCD�R���g���[��(ILI9488)
//
// PC6 : MOSI     ,SPI_MOSI ,�}�X�^�o�� �X���[�u����   
// PC5 : RSPCKA   ,SPI_CLK  ,�N���b�N�o��  
// PC4 : SSLA0    ,DISP_CS  ,�X���[�u�Z���N�g 
// PH3 : port�o�� ,DISP_RST, LCD�R���g���[��(ILI9488) Reset�p 
// PC7 : port�o�� ,DISP_DC ,LCD�R���g���[��(ILI9488)  �R�}���h/�f�[�^���ʗp�@(�R�}���h=low,�f�[�^=high)  
//

void RSPI_Init_Port(void)
{
	
	MPC.PWPR.BIT.B0WI = 0;  	 // �}���`�t�@���N�V�����s���R���g���[���@�v���e�N�g����
        MPC.PWPR.BIT.PFSWE = 1;  	// PmnPFS ���C�g�v���e�N�g����
    
	MPC.PC6PFS.BYTE = 0x0d;		// PC6 = MOSIA   Master Out slave in
	MPC.PC5PFS.BYTE = 0x0d;	  	// PC5 = RSPCKA
	MPC.PC4PFS.BYTE = 0x0d;		// PC4 = SSLA0
	
        MPC.PWPR.BYTE = 0x80;      	//  PmnPFS ���C�g�v���e�N�g �ݒ�	
	
        PORTC.PMR.BIT.B6 = 1;     	// PC6  ���Ӌ@�\�Ƃ��Ďg�p
	PORTC.PMR.BIT.B5 = 1;     	// PC5  
	PORTC.PMR.BIT.B4 = 1;     	// PC4
	
	PORTC.DSCR.BIT.B6 = 1;     	// PC6 ���쓮�o��
	PORTC.DSCR.BIT.B5 = 1;     	// PC5  
	PORTC.DSCR.BIT.B4 = 1;     	// PC4
	
	
	LCD_RESET_PMR  = 0;     	// PH3:�ėp���o�̓|�[�g ,LED�R���g���[�� Reset�p 
	LCD_RESET_PDR = 1;      	// PH3: �o�̓|�[�g�ɐݒ� LED�R���g���[���ւ�Reset
	
        LCD_DC_PMR = 0;        		// PC7:�ėp���o�̓|�[�g, LED�R���g���[�� data/command�؂�ւ�
	LCD_DC_PDR = 1;        		// PC7: �o�̓|�[�g�ɐݒ� LED�R���g���[�� data/command�؂�ւ�     �@  �@
	 
}




// RSPI ���W�X�^�̏����ݒ� 
//�r�b�g���[�g:
//  RSPI �R�}���h���W�X�^0 �` 7�iSPCMD0�`7)��BRDV[1:0] �r�b�g��SPBR ���W�X�^�̐ݒ�l(n)�̑g�ݍ��킹�ł��܂�B
// 
//  ���݁ARSPI0.SPCMD0��BRDV=0 ,   PCLKB= 32MHz�Ƃ��Ă���B
//   �r�b�g���[�g = PCLKB / 2*(SPBR+1)
//   
//
//     SPBR = 0 �ŁA32/2 = 16 [MHz]
//     SPBR = 1 �ŁA32/4 = 8 [MHz]
//

void RSPI_Init_Reg(void)
{

	RSPI0.SPCR.BIT.SPMS = 0;        // SPI���� (4����)	
	RSPI0.SPCR.BIT.TXMD = 1;        // ���M����̂ݍs���B��M�Ȃ�
	RSPI0.SPCR.BIT.MODFEN = 0;      // ���[�h�t�H���g�G���[���o���֎~
	
	RSPI0.SPCR.BIT.MSTR = 1;	//  �}�X�^���[�h
	
	
	RSPI0.SPCR.BIT.SPEIE =0;       // �G���[���荞�ݗv���̔������֎~
	RSPI0.SPCR.BIT.SPTIE =0;       // ���M�o�b�t�@�G���v�e�B���荞�ݗv���̔������֎~
	RSPI0.SPCR.BIT.SPE = 0;        // RSPI�@�\�͖���
	RSPI0.SPCR.BIT.SPRIE = 0;      // RSPI��M�o�b�t�@�t�����荞�ݗv���̔������֎~
	
	RSPI0.SSLP.BYTE = 0;                // �X���[�u�Z���N�g�ɐ� SSL0�`SSL3 �A�N�e�B�uLow
	RSPI0.SPPCR.BYTE = 0;               //  �ʏ탂�[�h
	
	RSPI0.SPSCR.BYTE = 0;               // �]���t�H�[�}�b�g�́A���SPCMD0 ���W�X�^���Q��

	
	// ILI9488 17.4.3. DBI Type C Option 3 (4-Line SPI System) Timing Characteristics ���ASerial clock cycle (Write) = 50nsec (min)
	// �������ݎ��ɂ́A20MHz���ō�
	RSPI0.SPBR = 0x0;		   // 16 MHz
	
	
	RSPI0.SPDCR.BIT.SPFC =0x00;    // SPDR ���W�X�^�Ɋi�[�ł���i1 ��̓]���N���j�t���[���� 1 =(32bit, 4byte)
	RSPI0.SPDCR.BIT.SPRDTD = 0;     // 0�FSPDR�͎�M�o�b�t�@��ǂݏo��
	RSPI0.SPDCR.BIT.SPLW = 1;	// RSPI�f�[�^���W�X�^(SPDR)�̓����O���[�h(32bit)�̃A�N�Z�X

                                       // SPCMD0.BIT.SCKDEN = 1�̏ꍇ�́A
	RSPI0.SPCKD.BYTE = 0;               // �Z���N�g�M������N���b�N�����܂ł̒x�����ԁ@1RSPCK
				       // SPCMD0.BIT.SCKDEN = 0�̏ꍇ�A�Z���N�g�M������N���b�N�����܂ł̒x�����Ԃ́A1RSPCK
			
	RSPI0.SSLND.BYTE = 0;          // SPCMD0.BIT.SLNDEN = 1�̏ꍇ�́A
	                              // �N���b�N�ŏI�G�b�W����A�Z���N�g�����܂ł̒x������
	                              // SPCMD0.BIT.SLNDEN = 0�̏ꍇ�A1RSPCK
				      
	RSPI0.SPND.BYTE = 0;	     // SPCMD0.BIT.SPNDEN = 1�̏ꍇ�́A�V���A���]���I����́A�Z���N�g�M����A�N�e�B�u���ԁi���A�N�Z�X�x��)�@1RSPCK�{2PCLK	      
				     // SPCMD0.BIT.SPNDEN = 0�̏ꍇ�A1RSPCK�{2PCLK
	
        RSPI0.SPCR2.BYTE = 0;        // �p���e�B�Ȃ��@�A�C�h�����荞�ݗv���̔������֎~�@�A�C�h�����荞�ݗv���̔������֎~

					// ���M�f�[�^�G���v�e�B���荞��
	IPR(RSPI0,SPTI0) = 0x06;	// ���荞�݃��x�� = 6�@�@�i15���ō����x��)
	IR(RSPI0,SPTI0) = 0;		// ���荞�ݗv���̃N���A
	IEN(RSPI0,SPTI0) = 1;		// ���荞�݋���	
	
				      
}






//     SPCMD 0 �̐ݒ�
//   
//     SPI ���[�h3: �N���b�N�A�C�h���� High
//                :  (�N���b�N�����オ�莞�Ƀf�[�^�T���v��)

//�@�@�@SSL�M��   : SSLA0

void RSPI_SPCMD_0(void)
{

					// SPI ���[�h 3 : �\�� OK
	RSPI0.SPCMD0.BIT.CPOL = 1;	// CPOL= 1 ���p���X(�N���b�N�A�C�h���� High)				
	RSPI0.SPCMD0.BIT.CPHA = 1;	// CPHA= 1 ��G�b�W�Ńf�[�^�ω��A�����G�b�W�Ńf�[�^�T���v�� (�G�b�W��1���琔����j

	
	RSPI0.SPCMD0.BIT.BRDV = 0;	// BRDV= 0 �x�[�X�̃r�b�g���[�g�g�p (�����Ȃ�)
	RSPI0.SPCMD0.BIT.SSLA = 0;	// SSLA= 0 SSL0���g�p
	
	RSPI0.SPCMD0.BIT.SSLKP = 0;	// �]���I����(���̃v���O�����̏ꍇ�A1byte�]��)�SSSL�M�����l�Q�[�g
	
	RSPI0.SPCMD0.BIT.SPB = 0x07;	//  SPB= 7  ���M �f�[�^�� 8 bit
	RSPI0.SPCMD0.BIT.LSBF = 0;	// LSBF= 0  MSB first
	
	RSPI0.SPCMD0.BIT.SPNDEN = 0;	// SPNDEN=0 ���A�N�Z�X�x���́A1RSPCK + 2PCLK
	RSPI0.SPCMD0.BIT.SLNDEN = 0;	// SLNDEN=0 SSL�l�Q�[�g�x���� 1RSPCK
	RSPI0.SPCMD0.BIT.SCKDEN = 0;	//  SCKDEN=0 RSPCK�x���� 1RSPCK
	
}


// LCD�R���g���[��(ILI9488)�ւ̕\���f�[�^�𑗐M �@(���M���荞�ݎg�p)
//
void spi_data_send( uint32_t para_num, uint8_t *data_pt)
{
					   
        LCD_DC_PODR = 1;     			// �f�[�^�w�� (D/C�|�[�g= High)
	
	spi_send_pt = data_pt;   		// ���M ���荞�݂ő��M����f�[�^�̐擪�ʒu
	spi_send_num = para_num;		// ���M���荞�݂ł̑��M�� (1��̑��M���荞�݂ŁA32bit���Z�b�g) 
	
	
						// ���M�J�n���̑��M�o�b�t�@�G���v�e�B���荞�ݗv���́A
						// SPTIE �r�b�g�Ɠ����܂��͌�ɁASPE �r�b�g���g1�h �ɂ��邱�ƂŔ������܂��B(page 1010)
	
	RSPI0.SPCR.BIT.SPTIE = 1;       	// ���M�o�b�t�@�G���v�e�B���荞�ݗv���̔���������
	RSPI0.SPCR.BIT.SPE = 1;        		// RSPI �@�\�L��
						
	
	
	
}



// LCD�R���g���[��(ILI9488)�ւ̃R�}���h�ƃp�����[�^(�f�[�^)���M 
// ���M�o�b�t�@�G���v�e�B�t���O�̃|�[�����O�ŁA�R�}���h�A�p�����[�^���M
//
// �R�}���h�ɑ��M�p�����[�^�������ꍇ�Acmd�������M�Bpara_num = 0 �ݒ�B
// �R�}���h�ɑ��M�p�����[�^������ꍇ�Acmd���M��Apara[]��para_num�����M�B
//
//�@�@����: cmd �R�}���h(=���W�X�^ID) ( 8 bit)
//          sd_num :���M�p�����[�^�� 
//          *para_pt: ���M�p�����[�^�̊i�[�ʒu�B���M�p�����[�^�́Apara[]�Ɋi�[����Ă���B
//   
//
void spi_cmd_send( uint8_t cmd, uint32_t para_num, uint8_t *para_pt)
{
        uint32_t i;
	uint32_t temp;
	
        RSPI0.SPCR.BIT.SPE = 1;        // RSPI �@�\�L��	
	
	temp =  RSPI0.SPSR.BIT.IDLNF;		//�@��ǂ�  (0=RSPI�A�C�h�����, 1=RSP�]����)  (page= 1069)
	while( RSPI0.SPSR.BIT.IDLNF ==1 );	// �]�����̓��[�v (���荞�݂ő��M�����f�[�^�́A�S�f�[�^�̓]���I���҂�)
	
	
	while( RSPI0.SPSR.BIT.SPTEF !=1 );       // ���M�o�b�t�@���G���v�e�B�łȂ�(=���M�o�b�t�@�ɗL���ȃf�[�^������)�ꍇ�́Awhile���[�v 
	                                         // ���M�o�b�t�@����V�t�g���W�X�^�֑��M�f�[�^���]�����ꂽ���� SPTEF=1�ɂȂ�B�i�N���b�N����1�r�b�g���ɑ��M��) 
	LCD_DC_PODR = 0;	   		// D/C�|�[�g = Low (�R�}���h)
	
	RSPI0.SPDR.LONG = (uint32_t)cmd;        // �R�}���h(���W�X�^ID)���M�J�n
	
	
	temp =  RSPI0.SPSR.BIT.IDLNF;		//�@��ǂ�  (0=RSPI�A�C�h�����, 1=RSP�]����)  (page= 1069) 
	while( RSPI0.SPSR.BIT.IDLNF ==1 );	// �]�����̓��[�v  
	  
	
	if ( para_num >  0 ) {    // ���M�p�����[�^�t���̏ꍇ
	
	  LCD_DC_PODR = 1;     			// D/C�|�[�g= High (�f�[�^)
	
	  for ( i = 0 ;  i  < para_num ; i++ ) {
	      while( RSPI0.SPSR.BIT.SPTEF !=1 );      // ���M�o�b�t�@���G���v�e�B�łȂ�(=���M�o�b�t�@�ɗL���ȃf�[�^������)�ꍇ�́Awhile���[�v 
	      RSPI0.SPDR.LONG = (uint32_t)(*para_pt);        // �p�����[�^���M�J�n	
	      para_pt ++;			// �p�����[�^�ʒu�̍X�V
	  }
	  
	  temp =  RSPI0.SPSR.BIT.IDLNF;		//�@��ǂ�  (0=RSPI�A�C�h�����, 1=RSP�]����)  (page= 1069) 
	  while( RSPI0.SPSR.BIT.IDLNF ==1 );	// �]�����̓��[�v  
	
	}
	
	RSPI0.SPCR.BIT.SPE = 0;        // RSPI �@�\����	
}



