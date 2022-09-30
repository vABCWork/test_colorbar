

// LCD�R���g���[�� Reset�p �o�̓|�[�g(PH3)�̒�`
#define LCD_RESET_PMR     (PORTH.PMR.BIT.B3)   //  �ėp���o�̓|�[�g
#define LCD_RESET_PDR     (PORTH.PDR.BIT.B3)   //  �o�̓|�[�g�Ɏw��
#define LCD_RESET_PODR    (PORTH.PODR.BIT.B3)  //  �o�̓f�[�^



// LCD�R���g���[�� data/command�@�ؑւ��p  �o�̓|�[�g(PC7)�̒�`
#define LCD_DC_PMR      (PORTC.PMR.BIT.B7)   //  �ėp���o�̓|�[�g
#define LCD_DC_PDR      (PORTC.PDR.BIT.B7)   //  �o�̓|�[�g�Ɏw��
#define LCD_DC_PODR     (PORTC.PODR.BIT.B7)  //  �o�̓f�[�^




extern	volatile uint8_t *spi_send_pt;		// ���M�f�[�^���i�[���Ă���ꏊ
extern  volatile uint32_t spi_send_num;		// ���M����o�C�g��



void Excep_RSPI0_SPTI0(void);

void RSPI_Init_Port(void);

void RSPI_Init_Reg(void);

void RSPI_SPCMD_0(void);

void spi_cmd_send( uint8_t cmd, uint32_t para_num, uint8_t *para_pt);

void spi_data_send( uint32_t para_num, uint8_t *data_pt);

