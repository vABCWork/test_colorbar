
extern uint8_t rgb111_data_buf[9800];
	

void ILI9488_Init(void);

void ILI9488_Reset(void);

void lcd_adrs_set( uint16_t col, uint16_t page, uint16_t col2, uint16_t page2);

void color_bar(void);
void disp_black(void);


