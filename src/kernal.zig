const c = @cImport({
    @cInclude("core/io.h");
    @cInclude("core/fb.h");
});

const std = @import("std");

export fn main() noreturn {
    c.uart_init();
    c.fb_init();

    c.uart_puts("Hello Kernel! (from zig-ish)\n");

    c.drawRect(100, 100, 200, 200, 0x03, 0x03);

    const text = "Hello Kernel!";
    c.drawString(120, 120, text, 0x03);
    c.drawLine(0, 0, 1080, 720, 2);

    var buff: [1024]u8 = undefined;
    var index: usize = 0;

    while (true) {
        const cr = c.uart_getc();
        c.uart_putc(cr);
        buff[index] = cr;
        buff[index + 1] = 0;
        index += 1;
        c.drawString(120, 130, &buff, 3);
    }
}
