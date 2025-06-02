const c = @cImport({
    @cInclude("core/io.h");
});

export fn main() noreturn {
    c.uart_init();
    c.uart_puts("Hello, kernel Wordl! (from zig-ish)\n");
    while (true) {
        c.uart_putc(c.uart_getc());
    }
}
