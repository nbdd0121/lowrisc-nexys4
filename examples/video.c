#include <stdio.h>
#include "diskio.h"
#include "ff.h"
#include "uart.h"
#include "memory.h"
#include "rgb.h"

#undef putchar

volatile uint32_t *mem_base;

volatile uint32_t * get_videomem_base() {
    return (volatile uint32_t *)(DEV_MAP__io_ext_videomem__BASE);
}

#define CR_BASE  0x8000
#define CR_DEPTH 0x8001

#define DEPTH_32 0
#define DEPTH_16 1
#define DEPTH_8  2
#define DEPTH_GRAYSCALE 3

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
            to[idx] = grayscale_to32(rgb32_tograyscale(from[idx]));
        }
    }
}

void copyImg16(volatile uint32_t* from, volatile uint16_t* to) {
    for(int y = 0; y < 128; y++) {
        for(int x = 0; x < 128; x++) {
            int idx = (y << 7) | x;
            to[idx] = rgb32_to16(from[idx]);
        }
    }
}

void copyImg8(volatile uint32_t* from, volatile uint8_t* to) {
    for(int y = 0; y < 128; y++) {
        for(int x = 0; x < 128; x++) {
            int idx = (y << 7) | x;
            to[idx] = rgb32_to8(from[idx]);
        }
    }
}

void copyImgGrayscale(volatile uint32_t* from, volatile uint8_t* to) {
    for(int y = 0; y < 128; y++) {
        for(int x = 0; x < 128; x += 2) {
            int idx = (y << 7) | x;
            to[idx >> 1] = (rgb32_tograyscale(from[idx + 1]) << 4) | rgb32_tograyscale(from[idx]);
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

    uint32_t mode = DEPTH_GRAYSCALE;
    uint32_t step;

    if(mode == DEPTH_32) {
        writeCr(CR_BASE, mem_backup - mem_base);
        copyImg(ddr_base, mem);
        writeCr(CR_BASE, mem - mem_base);
        copyImg(ddr_base + 0x4000, mem + 0x4000);
        step = 128;
    }else if(mode==DEPTH_16){
        writeCr(CR_BASE, mem_backup - mem_base);
        copyImg16(ddr_base, (volatile uint16_t*)mem);
        copyImg16(ddr_base + 0x4000, (volatile uint16_t*)mem + 0x4000);
        writeCr(CR_BASE, mem - mem_base);
        copyImg16(ddr_base, (volatile uint16_t*)mem + 0x8000);
        copyImg16(ddr_base + 0x4000, (volatile uint16_t*)mem + 0xC000);
        step = 64;
    }else if(mode==DEPTH_8){
        writeCr(CR_BASE, mem_backup - mem_base);
        copyImg8(ddr_base, (volatile uint8_t*)mem);
        copyImg8(ddr_base + 0x4000, (volatile uint8_t*)mem + 0x4000);
        copyImg8(ddr_base, (volatile uint8_t*)mem + 0x8000);
        copyImg8(ddr_base + 0x4000, (volatile uint8_t*)mem + 0xC000);
        writeCr(CR_BASE, mem - mem_base);
        copyImg8(ddr_base, (volatile uint8_t*)mem + 0x10000);
        copyImg8(ddr_base + 0x4000, (volatile uint8_t*)mem + 0x14000);
        copyImg8(ddr_base, (volatile uint8_t*)mem + 0x18000);
        copyImg8(ddr_base + 0x4000, (volatile uint8_t*)mem + 0x1C000);
        step = 32;
    }else{
        writeCr(CR_BASE, mem_backup - mem_base);
        copyImgGrayscale(ddr_base, (volatile uint8_t*)mem);
        copyImgGrayscale(ddr_base + 0x4000, (volatile uint8_t*)mem + 0x2000);
        copyImgGrayscale(ddr_base, (volatile uint8_t*)mem + 0x4000);
        copyImgGrayscale(ddr_base + 0x4000, (volatile uint8_t*)mem + 0x6000);
        copyImgGrayscale(ddr_base, (volatile uint8_t*)mem + 0x8000);
        copyImgGrayscale(ddr_base + 0x4000, (volatile uint8_t*)mem + 0xA000);
        copyImgGrayscale(ddr_base, (volatile uint8_t*)mem + 0xC000);
        copyImgGrayscale(ddr_base + 0x4000, (volatile uint8_t*)mem + 0xE000);
        writeCr(CR_BASE, mem - mem_base);
        copyImgGrayscale(ddr_base, (volatile uint8_t*)mem + 0x10000);
        copyImgGrayscale(ddr_base + 0x4000, (volatile uint8_t*)mem + 0x12000);
        copyImgGrayscale(ddr_base, (volatile uint8_t*)mem + 0x14000);
        copyImgGrayscale(ddr_base + 0x4000, (volatile uint8_t*)mem + 0x16000);
        copyImgGrayscale(ddr_base, (volatile uint8_t*)mem + 0x18000);
        copyImgGrayscale(ddr_base + 0x4000, (volatile uint8_t*)mem + 0x1A000);
        copyImgGrayscale(ddr_base, (volatile uint8_t*)mem + 0x1C000);
        copyImgGrayscale(ddr_base + 0x4000, (volatile uint8_t*)mem + 0x1E000);
        step = 16;
    }

    writeCr(CR_DEPTH, mode);

    for(uint32_t s = 0;;s += step) {
        writeCr(CR_BASE, s & 0x7FFF);
        sleep(10);
    }

    return 0;
}

