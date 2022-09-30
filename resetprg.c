

#include	<machine.h>
#include	<_h_c_lib.h>

#include	 "iodefine.h"
#include	 "misratypes.h"


static void operating_frequency_set (void);

#pragma stacksize si=0x200	// ���荞�݃X�^�b�N�̒�`�@(���[�U�X�^�b�N�T�C�Y�͎g�p���Ȃ��B)
//#pragma stacksize su=0x100
#define PSW_init       0x00010000	// �v���Z�b�T�X�e�[�^�X���[�h (���荞�݋���)


#pragma section ResetPRG		// output PowerON_Reset to PResetPRG section

#pragma entry PowerON_Reset_PC


void PowerON_Reset_PC(void)
{
	set_extb(__sectop("EXCEPTVECT")); // CPU��RXV2�A�[�L�e�N�`���̏ꍇ�A��O�x�N�^�e�[�u����EXTB(��O�e�[�u�����W�X�^)�փZ�b�g
	set_intb(__sectop("C$VECT"));     // �σx�N�^�[�e�[�u��(���荞��(�^�C�}�[��)�̐擪�A�h���X)���AINTB���W�X�^�ɃZ�b�g

	operating_frequency_set();    // �V�X�e���N���b�N ���̐ݒ�
	
	_INITSCT();			// �ÓI�ϐ��̏����� (�����l�Ȃ��̐ÓI�ϐ�=0, �����l�t���̐ÓI�ϐ�=�����l)

	
	set_psw(PSW_init);		// ���荞�݋���
	
	main();				// ���C���̋N��

	brk();
}



//  �N���b�N�̐ݒ�
// �ݒ���e:
//    �V�X�e���N���b�N(ICLK) = 32 MHz
//    ���Ӄ��W���[���N���b�N(PCLKA) = 32 MHz (MTU2�p)
//    ���Ӄ��W���[���N���b�N(PCLKB) = 32 MHz (MTU2,S12AD�ȊO�̎��Ӄ��W���[���p)
//    ���Ӄ��W���[���N���b�N(PCLKD) = 32 MHz (S12AD A/D�R���o�[�^�p)
//    �t���b�V��IF�N���b�N(FCLK) = 32 MHz
//

static void operating_frequency_set (void)
{
	
	SYSTEM.PRCR.WORD = 0xA50F;	// �N���b�N������H�֘A���W�X�^�̏������݋���
	
	SYSTEM.HOCOCR.BIT.HCSTP = 0;	// HOCO����
	while((SYSTEM.OSCOVFSR.BIT.HCOVF) == 0){	//  �����I���`�b�v�I�V���[�^ HOCO���U����҂�
	}
	
					// ����d�͐��䃂�[�h�@�������샂�[�h���獂�����샂�[�h�ɐݒ�
	SYSTEM.OPCCR.BIT.OPCM = 0;	// �������샂�[�h
	while( SYSTEM.OPCCR.BIT.OPCMTSF == 1 ){  // �J�ڑ҂�
	}
	
	 
	SYSTEM.SCKCR.LONG = 0x00000000;	    // �V�X�e���N���b�N�̐ݒ� (FCLK=32MHz,ICLK=32MHz,  PCLKB=32MHz ,PCLKD=32MHz)					  
	while( SYSTEM.SCKCR.LONG != 0x00000000 ){
	}
	
	
	SYSTEM.SCKCR3.WORD = 0x0100;		// Clock source�́AHOCO�I��
	while( SYSTEM.SCKCR3.WORD != 0x0100){
	}
	
	
	
	SYSTEM.PRCR.WORD = 0xA500;	// �N���b�N������H�֘A���W�X�^�ւ̏������݋֎~
}


