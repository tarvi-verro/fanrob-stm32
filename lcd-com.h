#include <stdint.h>
#include <stdbool.h>

#define LCD_WIDTH	84
#define LCD_HEIGHT	48

/**
 * lcd_send() - send data or control bytes to lcd
 * @data_mode:	if ture, @a will be intrepreted as pixel values by the lcd
 *		chip, if false, command mode will be selected
 * @a:		bytes to send
 * @l:		length of @a
 */
extern void lcd_send(bool data_mode /* false:command mode */,
		const uint8_t *a, int l);

/**
 * lcd_putc() - print out a character, advance caret
 * @c:		character to print, make sure that font-tiny.xcf has it
 */
extern void lcd_putc(char c);

/**
 * lcd_puts() - print out a string of characters, advancing caret
 * @s:		string of characters to spew onto the screen, make sure all
 *		characters are implemented in font-tiny.xcf and font-lookup.c
 */
extern void lcd_puts(const char *s);

/**
 * lcd_setcaret() - set caret location of lcd
 * @x:		x location, must be 0 <= x < 84
 * @y:		y location, must be 0 <= x < 6
 */
extern void lcd_setcaret(int x, int y);

/**
 * lcd_bgset() - set lcd background brightness
 * @b:		brightness, 0 for off, 255 for max brightness
 */
extern void lcd_bgset(uint8_t b);

/**
 * lcd_repeat() - repeats given data byte given times
 * @x:		byte to send
 * @n:		how many times to repeat
 */
extern void lcd_repeat(uint8_t x, int n);

/**
 * lcd_clear() - efficiently clears entire lcd
 */
extern void lcd_clear();

/**
 * setup_lcd() - initialize lcd and periperals
 */
extern void setup_lcd(void);

