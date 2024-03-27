#include <print.h>

/* forward declaration */
static void print_char(fmt_callback_t, void *, char, int, int);
static void print_str(fmt_callback_t, void *, const char *, int, int);
static void print_num(fmt_callback_t, void *, unsigned long, int, int, int, int, char, int);

void vprintfmt(fmt_callback_t out, void *data, const char *fmt, va_list ap) {
	char c;
	const char *s;
	long num;
	long x, y, z;

	int width;
	int long_flag; // output is long (rather than int)
	int neg_flag;  // output is negative
	int ladjust;   // output is left-aligned
	char padc;     // padding char

	for (;;) {
		/* scan for the next '%' */
		/* Exercise 1.4: Your code here. (1/8) */

		const char *ptr = fmt;
		while (*ptr != '%' && *ptr != '\0') {
			ptr++;
		}
 
		/* flush the string found so far */
		/* Exercise 1.4: Your code here. (2/8) */
		
		out(data, fmt, ptr - fmt); // now ptr points to the '%', the char between ptr and fmt are waiting for output.
		fmt = ptr; 	// Allowed, const pointer only means the content it points to was unmutable, but the pointer can point
				// to somewhere else as long as it is const as well.

		/* check "are we hitting the end?" */
		/* Exercise 1.4: Your code here. (3/8) */
		
		if (*fmt == '\0') {
			break;
		}

		/* we found a '%' */
		/* Exercise 1.4: Your code here. (4/8) */

		fmt++;
		
		/* Format pattern:
		 * 	%[flags][width][length]<specifier>
		 * 
		 * - flags: '-': left aligned, default right; 
		 *          '0': 0 padding when short than specified width;
		 * - width: a number, represents the min width of the output. When bigger than the actual length, padding 
		 *   	    with ' '(spaces), else do nothing.
		 * - length: 'l', %ld and %lD -> long int, others (%lx, %lo, %lb, %lu, ...) -> unsigned long int;
		 * - specifier: 'b' unsigned binary
		 *   		'd', 'D' decimal
		 *   		'o', 'O' unsigned octal
		 *   		'u'. 'U' unsigned decimal
		 *   		'x', 'X' unsigned hexadecimal, lowercase/uppercase
		 *   		'c' char
		 *   		's' string
		 *
		 */

		/* check format flag */
		/* Exercise 1.4: Your code here. (5/8) */
		
		ladjust = 0;  // default: Right aligned
		padc = ' ';
		if (*fmt == '-') {
			ladjust = 1; // '-' enables left aligned
			fmt++;
		} else if (*fmt == '0') {
			padc = '0';  // '0' enables 0 padding
			fmt++;
		}

		/* get width */
		/* Exercise 1.4: Your code here. (6/8) */
		
		width = 0;
		while (*fmt >= '0' && *fmt <= '9') {
			width *= 10;
			width += (*fmt - '0');
			fmt++;	
		}
		
		/* check for long */
		/* Exercise 1.4: Your code here. (7/8) */
		
		long_flag = 0;
		if (*fmt == 'l') {
			long_flag = 1;
			fmt++;
		}
		
		neg_flag = 0;
		switch (*fmt) {
		case 'P':
			if (long_flag) {
				x = va_arg(ap, long int);
			} else {
				x = va_arg(ap, int);
			}
			
			out(data, "(", 1);
			
			if (x < 0) {
				x = -x;	
				print_num(out, data, x, 10, 1, width, ladjust, padc, 0);
			} else {
				print_num(out, data, x, 10, 0, width, ladjust, padc, 0);
			}

			if (long_flag) {
				y = va_arg(ap, long int);
			} else {
				y = va_arg(ap, int);
			}
			
			out(data, ",", 1);
			
			if (y < 0) {
				y = -y;
				print_num(out, data, y, 10, 1, width, ladjust, padc, 0);
			} else {
				print_num(out, data, y, 10, 0, width, ladjust, padc, 0);
			}
			
			out(data, ",", 1);

			if (long_flag) {
				z = (long int)((x + y) * (x - y));
				z = z < 0 ? -z : z;
			} else {
				z = (int)((x + y) * (x - y));
				z = z < 0 ? -z : z;
			}
			print_num(out, data, z, 10, 0, width, ladjust, padc, 0);
			
			out(data, ")", 1);

			break;
		case 'b':
			if (long_flag) {
				num = va_arg(ap, long int);
			} else {
				num = va_arg(ap, int);
			}
			print_num(out, data, num, 2, 0, width, ladjust, padc, 0);
			break;

		case 'd':
		case 'D':
			if (long_flag) {
				num = va_arg(ap, long int);
			} else {
				num = va_arg(ap, int);
			}

			/*
			 * Refer to other parts (case 'b', case 'o', etc.) and func 'print_num' to
			 * complete this part. Think the differences between case 'd' and the
			 * others. (hint: 'neg_flag').
			 */
			/* Exercise 1.4: Your code here. (8/8) */
			
			// The third parameter of 'print_num' is type of unsigned long, make sure 'num' is positive!
			if (num < 0) {
				neg_flag = 1;
				num = -num;
			}
			print_num(out, data, num, 10, neg_flag, width, ladjust, padc, 0);	
			break;

		case 'o':
		case 'O':
			if (long_flag) {
				num = va_arg(ap, long int);
			} else {
				num = va_arg(ap, int);
			}
			print_num(out, data, num, 8, 0, width, ladjust, padc, 0);
			break;

		case 'u':
		case 'U':
			if (long_flag) {
				num = va_arg(ap, long int);
			} else {
				num = va_arg(ap, int);
			}
			print_num(out, data, num, 10, 0, width, ladjust, padc, 0);
			break;

		case 'x':
			if (long_flag) {
				num = va_arg(ap, long int);
			} else {
				num = va_arg(ap, int);
			}
			print_num(out, data, num, 16, 0, width, ladjust, padc, 0);
			break;

		case 'X':
			if (long_flag) {
				num = va_arg(ap, long int);
			} else {
				num = va_arg(ap, int);
			}
			print_num(out, data, num, 16, 0, width, ladjust, padc, 1);
			break;

		case 'c':
			c = (char)va_arg(ap, int);
			print_char(out, data, c, width, ladjust);
			break;

		case 's':
			s = (char *)va_arg(ap, char *);
			print_str(out, data, s, width, ladjust);
			break;

		case '\0':
			fmt--;
			break;

		default:
			/* output this char as it is */
			out(data, fmt, 1);
		}
		fmt++;
	}
}

/* --------------- local help functions --------------------- */
void print_char(fmt_callback_t out, void *data, char c, int length, int ladjust) {
	int i;

	if (length < 1) {
		length = 1;
	}
	const char space = ' ';
	if (ladjust) {
		out(data, &c, 1);
		for (i = 1; i < length; i++) {
			out(data, &space, 1);
		}
	} else {
		for (i = 0; i < length - 1; i++) {
			out(data, &space, 1);
		}
		out(data, &c, 1);
	}
}

void print_str(fmt_callback_t out, void *data, const char *s, int length, int ladjust) {
	int i;
	int len = 0;
	const char *s1 = s;
	while (*s1++) {
		len++;
	}
	if (length < len) {
		length = len;
	}

	if (ladjust) {
		out(data, s, len);
		for (i = len; i < length; i++) {
			out(data, " ", 1);
		}
	} else {
		for (i = 0; i < length - len; i++) {
			out(data, " ", 1);
		}
		out(data, s, len);
	}
}

void print_num(fmt_callback_t out, void *data, unsigned long u, int base, int neg_flag, int length,
	       int ladjust, char padc, int upcase) {
	/* algorithm :
	 *  1. prints the number from left to right in reverse form.
	 *  2. fill the remaining spaces with padc if length is longer than
	 *     the actual length
	 *     TRICKY : if left adjusted, no "0" padding.
	 *		    if negtive, insert  "0" padding between "0" and number.
	 *  3. if (!ladjust) we reverse the whole string including paddings
	 *  4. otherwise we only reverse the actual string representing the num.
	 */

	int actualLength = 0;
	char buf[length + 70];
	char *p = buf;
	int i;

	do {
		int tmp = u % base; // Be careful! Mod nagetive number is different from mod positive
		if (tmp <= 9) {
			*p++ = '0' + tmp;
		} else if (upcase) {
			*p++ = 'A' + tmp - 10;
		} else {
			*p++ = 'a' + tmp - 10;
		}
		u /= base;
	} while (u != 0);

	if (neg_flag) {
		*p++ = '-';
	}

	/* figure out actual length and adjust the maximum length */
	actualLength = p - buf;
	if (length < actualLength) {
		length = actualLength;
	}

	/* add padding */
	if (ladjust) {
		padc = ' ';
	}
	if (neg_flag && !ladjust && (padc == '0')) {
		for (i = actualLength - 1; i < length - 1; i++) {
			buf[i] = padc;
		}
		buf[length - 1] = '-';
	} else {
		for (i = actualLength; i < length; i++) {
			buf[i] = padc;
		}
	}

	/* prepare to reverse the string */
	int begin = 0;
	int end;
	if (ladjust) {
		end = actualLength - 1;
	} else {
		end = length - 1;
	}

	/* adjust the string pointer */
	while (end > begin) {
		char tmp = buf[begin];
		buf[begin] = buf[end];
		buf[end] = tmp;
		begin++;
		end--;
	}

	out(data, buf, length);
}
