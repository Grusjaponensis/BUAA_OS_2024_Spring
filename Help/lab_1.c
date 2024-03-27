#include <stdarg.h>

// extra answer 1
static int str_idx = 0;
void stroutputk(void *data, const char *buf, size_t len) {
	char *str = (char *)data;
	for (int i = 0; i < len; str_idx++, i++) {
		str[str_idx] = buf[i];
	}
	str[str_idx] = '\0';
}		

int sprintf(char *buf, const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	str_idx = 0;
	vprintfmt(stroutputk, buf, fmt, ap);
	va_end(ap);
	return strlen(buf);
}

// extra answer 2
void outputknew(void *data, const char *buf, size_t len) {
	for (int i = 0; i < len; i++) {
        (*data) = buf[i];
        data++;
    }
}

int sprintf(char *buf, const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
    
    char *content = buf;
	vprintfmt(outputknew, &context, fmt, ap);
    *context = '\0'; //为了在后面用context - buf 
    
	va_end(ap);
	return context - buf; 
}

// exam answer
long num1, num2;
case 'R':
    if (long_flag) {
        num1 = va_arg(ap, long int);
        num2 = va_arg(ap, long int);
    } else {
        num1 = va_arg(ap, int);
        num2 = va_arg(ap, int);
    }

    out(data, "(", 1);
    
    if (num1 < 0) {
        neg_flag = 1;
        num1 = -num1;
    }

    print_num(out, data, num1, 10, neg_flag, width, ladjust, padc, 0);

    out(data, ",", 1);