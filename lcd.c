
/** Put this in the src folder **/

#include "lcd.h"
extern I2C_HandleTypeDef hi2c1;  // change your handler here accordingly

uint8_t backlight_state = 1;
#define I2C_ADDR 0x3F // I2C address of the PCF8574
#define RS_BIT 0 // Register select bit
#define EN_BIT 2 // Enable bit
#define BL_BIT 3 // Back light bit
#define D4_BIT 4 // Data 4 bit
#define D5_BIT 5 // Data 5 bit
#define D6_BIT 6 // Data 6 bit
#define D7_BIT 7 // Data 7 bit
// P7 : D7, P6 : D6 P5 : D5, P4 : D4
// P3 : BLK, P2 : E , P1 : RW , P0 : RS

#define LCD_ROWS 2 // Number of rows on the LCD
#define LCD_COLS 16 // Number of columns on the LCD

// LCD 모듈을 제어하기 위해 사용되는 함수로, 주어진 4비트 데이터를 LCD에 전송
// nibble : 전송할 4비트 데이터 , rs : 0(CMD MODE) 1(Data MODE) , rw : 0(Write) 1(Read) -> Always Write
void lcd_write_nibble(uint8_t nibble, uint8_t rs) {
  // 전송할 4비트 왼쪽으로 4칸 Shift ( 상위 4비트로 nibble 이동)
  uint8_t data = nibble << D4_BIT;
  // data OR 00000010(Data) 또는 00000000(Cmd) -> RS비트 설정
  data |= rs << RS_BIT;
  // data  OR 0001000(On) 또는 00000000(Off) -> BL 비트 설정
  data |= backlight_state << BL_BIT; // Include backlight state in data
  // data OR 00000100(Enable) -> E 비트 설정
  data |= 1 << EN_BIT;
  // I2C_ADDR << 1 => Write Mode
  // HAL_I2C_Master_Transmit(I2C 통신을 위한 핸들러 ,I2C 장치의 주소, 전송할 nibble, size, Timeout);
  // 마스터 장치에서 슬레이브 장치로 전송하는 함수
  HAL_I2C_Master_Transmit(&hi2c1, I2C_ADDR << 1, &data, 1, 100);
  HAL_Delay(1);
  // data AND 11111011 -> E 비트 비활성화
  data &= ~(1 << EN_BIT);
  // 다시 슬레이브 기기에 보내서 LCD에서 nibble을 받고 처리하지 못하게
  HAL_I2C_Master_Transmit(&hi2c1, I2C_ADDR << 1, &data, 1, 100);
}

//LCD에 명령을 전송,  I2C 데이터 패킷(data_t 배열)으로 만든 후 LCD 모듈로 전송
void lcd_send_cmd (uint8_t cmd)
{

	  uint8_t upper_nibble = cmd >> 4;
	  uint8_t lower_nibble = cmd & 0x0F;
	  lcd_write_nibble(upper_nibble, 0);
	  lcd_write_nibble(lower_nibble, 0);
	  if (cmd == 0x01 || cmd == 0x02) {
	    HAL_Delay(2);
	  }

}

//LCD에 표시할 데이터 (예: 문자)를 전송
void lcd_send_data (uint8_t data)
{
	  uint8_t upper_nibble = data >> 4;
	  uint8_t lower_nibble = data & 0x0F;
	  lcd_write_nibble(upper_nibble, 1);
	  lcd_write_nibble(lower_nibble, 1);
}
// LCD 디스플레이를 지웁
void lcd_clear (void)
{
	lcd_send_cmd(0x01);
	    HAL_Delay(2);
}
//지정된 행 (0 또는 1)과 열 (0에서 15까지)에 커서.
//행을 기반으로 커서 위치(col)를 계산하고 이에 맞는 명령을 LCD에 전송
void lcd_put_cur(uint8_t row, uint8_t column)
{
    uint8_t address;
    switch (row) {
        case 0:
            address = 0x00;
            break;
        case 1:
            address = 0x40;
            break;
        default:
            address = 0x00;
    }
    address += column;
    lcd_send_cmd(0x80 | address);
}

// LCD를 초기화
//4비트 모드로 LCD를 설정하는 초기화 명령
void lcd_init ()
{
	HAL_Delay(50);
	  lcd_write_nibble(0x03, 0);
	  HAL_Delay(5);
	  lcd_write_nibble(0x03, 0);
	  HAL_Delay(1);
	  lcd_write_nibble(0x03, 0);
	  HAL_Delay(1);
	  lcd_write_nibble(0x02, 0);
	  lcd_send_cmd(0x28);
	  lcd_send_cmd(0x0C);
	  lcd_send_cmd(0x06);
	  lcd_send_cmd(0x01);
	  HAL_Delay(2);
}

//문자열(char *str)을 LCD에 전송
void lcd_send_string (char *str)
{
	 while (*str) {
	    lcd_send_data(*str++);
	  }
}
void lcd_backlight(uint8_t state) {
  if (state) {
    backlight_state = 1;
  } else {
    backlight_state = 0;
  }
}
