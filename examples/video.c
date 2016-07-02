// A hello world program

#include <stdio.h>
#include "uart.h"

#undef putchar

volatile uint32_t * get_videomem_base() {
	return (volatile uint32_t *)(DEV_MAP__io_ext_videomem__BASE);
}

void changeColor(uint32_t color) {
	printf("\x1b[48;2;%d;%d;%dm", (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
}

int main() {
	uart_init();
	printf("----------\n");

	volatile uint32_t *mem = get_videomem_base();

	// 160x80 image
	// y += 2 here as height of a character is the double of its width
	for(int y = 88; y < 168; y += 2) {
		uint32_t prevColor = 0xFFFFFFFF;

		for(int x = 48; x < 208; x++) {
			int idx = (y << 8) | x;
			uint32_t color = mem[idx];
			// Optimization to increase speed
			if(color != prevColor) changeColor(color);
			putchar(' ');
			prevColor = color;
		}
		printf("\x1b[0m\n");
	}

	return 0;
}

