#include "main.h"
#include "stm32f4xx_hal.h"
void lcd_clear (void);
void lcd_write_nibble(uint8_t nibble, uint8_t rs);

void lcd_send_cmd (uint8_t cmd);
void lcd_send_data (uint8_t data);

void lcd_put_cur(uint8_t row, uint8_t column);

void lcd_init ();

void lcd_send_string (char *str);

void lcd_backlight(uint8_t state) ;

