#include <stdio.h>
#include "diskio.h"
#include "ff.h"
#include "uart.h"
#include "memory.h"

#undef putchar

volatile uint32_t *mem_base;

volatile uint32_t * get_videomem_base() {
	return (volatile uint32_t *)(DEV_MAP__io_ext_videomem__BASE);
}

#define CR_BASE 0x8000

/* File reading routine */
int readFile(const char* name, void* dest) {
	static int initialized = 0;
	static FATFS FatFs;
	if (!initialized) {
		if(f_mount(&FatFs, "", 1)) {
			printf("Fail to mount SD driver!\n");
			return -1;
		}
		initialized = 1;
	}

	FIL fil;                /* File object */
    uint8_t* buffer = dest; /* File copy buffer */
	FRESULT fr;             /* FatFs return code */
	uint32_t br;            /* Read count */
	uint32_t i;

    fr = f_open(&fil, name, FA_READ);
    if (fr) {
        printf("Failed to open %s!\n", name);
        return -1;
    }

    uint32_t fsize = 0;
    for (;;) {
        fr = f_read(&fil, &buffer[fsize], 64, &br);  /* Read a chunk of source file */
        if (fr || br == 0) break; /* error or eof */
        fsize += br;
    }

    if(f_close(&fil)) {
        printf("Fail to close file!");
        return -1;
    }
}

/* Fill the whole framebuffer */
void fillColor(volatile uint32_t* mem, uint32_t color) {
	for(int y = 0; y < 128; y++) {
		for(int x = 0; x < 128; x++) {
			int idx = (y << 7) | x;
			mem[idx] = color;
		}
	}
}

/* Fill the framebuffer using black&white grids */
void fillGrid(volatile uint32_t* mem) {
	for(int y = 0; y < 128; y++) {
		for(int x = 0; x < 128; x++) {
			int idx = (y << 7) | x;
			mem[idx] = (x + y) & 1 ? 0xFFFFFF : 0;
		}
	}
}

void changeColor(uint32_t color) {
	static uint32_t prev = 0xFFFFFFFF;
	if (color == prev) return;
	prev = color;
	if (color == 0xFFFFFFFF)
		printf("\x1b[0m");
	else
		printf("\x1b[48;2;%d;%d;%dm", (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
}

void sleep(int t) {
	for(;t>=0;t--)
        asm volatile("li a4, 12500; .1: addiw a4, a4, -1; ble a0, a4, .1;":::"a4");
}

void writeCr(ptrdiff_t reg, ptrdiff_t val) {
	mem_base[reg] = val;
	while(mem_base[reg] != val);
}

void dump(volatile uint32_t* mem) {
	for(int y = 0; y < 128; y++) {
		for(int x = 0; x < 128; x++) {
			int idx = (y << 7) | x;
			changeColor(mem[idx]);
			putchar(' ');
		}
		changeColor(0xFFFFFFFF);
		putchar('\n');
	}
}

void copyImg(volatile uint32_t* from, volatile uint32_t* to) {
    for(int y = 0; y < 128; y++) {
		for(int x = 0; x < 128; x++) {
			int idx = (y << 7) | x;
            to[idx] = from[idx];
		}
	}
}

#define SWAP(a, b) do{typeof(a) tmp = a; a = b; b = tmp;}while(0)

int main() {
	uart_init();
	printf("----------\n");

	mem_base = get_videomem_base();
	volatile uint32_t *mem = mem_base;
	volatile uint32_t *mem_backup = mem_base + 0x4000;

	uint32_t colors[] = {0x000000, 0xFF0000, 0x00FF00, 0x0000FF, 0xFFFFFF};

    uint32_t* ddr_base = (uint32_t*)(void*)get_ddr_base();
    readFile("img.raw", ddr_base);
    readFile("img2.raw", ddr_base + 0x4000);

	writeCr(CR_BASE, mem_backup - mem_base);
	copyImg(ddr_base, mem);
    // fillColor(mem, 0);
	writeCr(CR_BASE, mem - mem_base);
    copyImg(ddr_base + 0x4000, mem_backup);
    // fillColor(mem_backup, 0xFFFFFF);

	for(uint32_t s = 0;;s+=128) {
		writeCr(CR_BASE, s & 0x7FFF);
		sleep(10);
	}

	return 0;
}

