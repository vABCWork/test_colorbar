#include	<machine.h>
#include	 "iodefine.h"
#include	 "misratypes.h"
#include	"delay.h"
#include	 "spi.h"
#include	 "ILI9488.h"

void clear_module_stop(void);

void main(void)
{
	
	clear_module_stop();	//  ���W���[���X�g�b�v�̉���
	
	RSPI_Init_Port();	// RSPI �|�[�g�̏�����  (LCD�R���g���[���p)   
     	RSPI_Init_Reg();        // SPI ���W�X�^�̐ݒ�  

     	RSPI_SPCMD_0();	        // SPI �]���t�H�[�}�b�g��ݒ�, SSLA0�g�p	
	
	ILI9488_Reset();	// LCD �̃��Z�b�g	
	 
	ILI9488_Init();		// LCD�̏�����
	
	
	while(1){
			
		color_bar();	// ��ʁ@8�F�\��(��,���F,�V�A��,��,�}�[���^,��,��,�� ) 86.5[msec]
	
		delay_msec(500);
	
		disp_black();		// ��ʁ@��  ( 80 [msec] at 16[MHz] ) 80[msecc]
	
		delay_msec(500);
	}
	
}





// ���W���[���X�g�b�v�̉���
// �R���y�A�}�b�`�^�C�}(CMT) ���j�b�g0(CMT0, CMT1) 
//   �V���A���y���t�F�����C���^�t�F�[�X0(RSPI)
//
void clear_module_stop(void)
{
	SYSTEM.PRCR.WORD = 0xA50F;	// �N���b�N�����A����d�͒ጸ�@�\�֘A���W�X�^�̏������݋���	
	
	MSTP(CMT0) = 0;			// �R���y�A�}�b�`�^�C�}(CMT) ���j�b�g0(CMT0, CMT1) ���W���[���X�g�b�v�̉���
	
	MSTP(RSPI0) = 0;		// �V���A���y���t�F�����C���^�t�F�[�X0 ���W���[���X�g�b�v�̉���
	
	SYSTEM.PRCR.WORD = 0xA500;	// �N���b�N�����A����d�͒ጸ�@�\�֘A���W�X�^�������݋֎~
}

