#include "core/io.h"

void main()
{
    uart_init();
	uart_puts("Hello, kernel World!\r\n");
	while (1)
		uart_putc(uart_getc());
}
