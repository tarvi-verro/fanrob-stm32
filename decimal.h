
extern int decimal_length(unsigned int i);

extern void print_decimal(unsigned int d, int len);

/**
 * print_decimal_fixpt() - print out a fixed base-10 point decimal
 * @d:		decimal to print
 * @len:	length of &d when printed out in base 10, without comma
 * @exp:	exponent of base 10
 * @base_to_exp:the base raised to &exp
 */
extern void print_decimal_fixpt(unsigned int d, int len, int exp,
		int base_to_exp);

