#ifndef _LCD_H_
#define _LCD_H_

//#include "main.h"
#include "delay.h"

#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))


//LCD_CS: PB6  
#define LCD_CS_SET    GPIOB->ODR |= GPIO_ODR_6
#define LCD_CS_CLR    GPIOB->ODR &= ~GPIO_ODR_6
//LCD_BL: PC7  
#define LCD_BL_SET    GPIOC->ODR |= GPIO_ODR_7
#define LCD_BL_CLR    GPIOC->ODR &= ~GPIO_ODR_7
//LCD_DC: PA8
#define LCD_DC_SET    GPIOA->ODR |= GPIO_ODR_8
#define LCD_DC_CLR    GPIOA->ODR &= ~GPIO_ODR_8


#define LCD_CMD                0
#define LCD_DATA               1

#define LCD_WIDTH    240
#define LCD_HEIGHT   320

#define WHITE          0xFFFF
#define BLACK          0x0000	  
#define BLUE           0x001F  
#define BRED           0XF81F
#define GRED 		   0XFFE0
#define GBLUE		   0X07FF
#define RED            0xF800
#define MAGENTA        0xF81F
#define GREEN          0x07E0
#define CYAN           0x7FFF
#define YELLOW         0xFFE0
#define BROWN 		   0XBC40 
#define BRRED 		   0XFC07 
#define GRAY  		   0X8430 

#define FONT_1206    12
#define FONT_1608    16


extern void lcd_init(void);
extern void lcd_clear_screen(uint16_t hwColor);
extern void lcd_set_cursor(uint16_t hwXpos, uint16_t hwYpos);
extern void lcd_draw_point(uint16_t hwXpos, uint16_t hwYpos, uint16_t hwColor);
extern void lcd_display_char(uint16_t hwXpos, //specify x position.
                         uint16_t hwYpos, //specify y position.
                         uint8_t chChr,   //a char is display.
                         uint8_t chSize,  //specify the size of the char
                         uint16_t hwColor); //specify the color of the char
extern void lcd_display_num(uint16_t hwXpos,  //specify x position.
                          uint16_t hwYpos, //specify y position.
                          uint32_t chNum,  //a number is display.
                          uint8_t chLen,   //length ot the number
                          uint8_t chSize,  //specify the size of the number
                          uint16_t hwColor); //specify the color of the number
extern void lcd_display_string(uint16_t hwXpos, //specify x position.
                         uint16_t hwYpos,   //specify y position.
                         const uint8_t *pchString,  //a pointer to string
                         uint8_t chSize,    // the size of the string 
                         uint16_t hwColor);  // specify the color of the string 
void lcd_draw_line(uint16_t hwXpos0, //specify x0 position.
                      uint16_t hwYpos0, //specify y0 position.
                      uint16_t hwXpos1, //specify x1 position.
                      uint16_t hwYpos1, //specify y1 position.
                      uint16_t hwColor); //specify the color of the line
void lcd_draw_circle(uint16_t hwXpos,  //specify x position.
                        uint16_t hwYpos,  //specify y position.
                        uint16_t hwRadius, //specify the radius of the circle.
                        uint16_t hwColor);  //specify the color of the circle.
void lcd_fill_rect(uint16_t hwXpos,  //specify x position.
                   uint16_t hwYpos,  //specify y position.
                   uint16_t hwWidth, //specify the width of the rectangle.
                   uint16_t hwHeight, //specify the height of the rectangle.
                   uint16_t hwColor);  //specify the color of rectangle.
void lcd_draw_v_line(uint16_t hwXpos, //specify x position.
                        uint16_t hwYpos, //specify y position. 
                        uint16_t hwHeight, //specify the height of the vertical line.
                        uint16_t hwColor);  //specify the color of the vertical line.
void lcd_draw_h_line(uint16_t hwXpos, //specify x position.
                        uint16_t hwYpos,  //specify y position. 
                        uint16_t hwWidth, //specify the width of the horizonal line.
                        uint16_t hwColor); //specify the color of the horizonal line.
void lcd_draw_rect(uint16_t hwXpos,  //specify x position.
                      uint16_t hwYpos,  //specify y position.
                      uint16_t hwWidth, //specify the width of the rectangle.
                      uint16_t hwHeight, //specify the height of the rectangle.
                      uint16_t hwColor);  //specify the color of rectangle.
#endif
