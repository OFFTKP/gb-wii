#include <grrlib.h>
#include <stdlib.h>
#include <wiiuse/wpad.h>
#include "GameboyTKP/gb_wiiwrapper.h"
#include "Tetris_gbc.h";
#include "BMfont5_png.h"

// Tile stuff
#define TILE_DELAY  10
#define TILE_UP     12 * 0
#define TILE_RIGHT  12 * 1
#define TILE_DOWN   12 * 2
#define TILE_LEFT   12 * 3
#define TILE_UP2    12 * 4 + 9
#define TILE_RIGHT2 12 * 5 + 9
#define TILE_DOWN2  12 * 6 + 9
#define TILE_LEFT2  12 * 7 + 9

// RGBA Colors
#define GRRLIB_BLACK   0x000000FF
#define GRRLIB_MAROON  0x800000FF
#define GRRLIB_GREEN   0x008000FF
#define GRRLIB_OLIVE   0x808000FF
#define GRRLIB_NAVY    0x000080FF
#define GRRLIB_PURPLE  0x800080FF
#define GRRLIB_TEAL    0x008080FF
#define GRRLIB_GRAY    0x808080FF
#define GRRLIB_SILVER  0xC0C0C0FF
#define GRRLIB_RED     0xFF0000FF
#define GRRLIB_LIME    0x00FF00FF
#define GRRLIB_YELLOW  0xFFFF00FF
#define GRRLIB_BLUE    0x0000FFFF
#define GRRLIB_FUCHSIA 0xFF00FFFF
#define GRRLIB_AQUA    0x00FFFFFF
#define GRRLIB_WHITE   0xFFFFFFFF

void YUVfromRGB(unsigned char& Y, unsigned char& U, unsigned char& V, unsigned char R, unsigned char G, unsigned char B) {
    Y = 0.257 * R + 0.504 * G + 0.098 * B + 16;
    U = -0.148 * R - 0.291 * G + 0.439 * B + 128;
    V = 0.439 * R - 0.368 * G - 0.071 * B + 128;
}

void YfromRGB(unsigned char& Y, unsigned char R, unsigned char G, unsigned char B) {
    Y = 0.257 * R + 0.504 * G + 0.098 * B + 16;
}

#define RGB2YUV(r, g, b, y, u, v) {\
  y = (306*r + 601*g + 117*b)  >> 10;\
  u = ((-172*r - 340*g + 512*b) >> 10)  + 128;\
  v = ((512*r - 429*g - 83*b) >> 10) + 128;\
  y = y < 0 ? 0 : y;\
  u = u < 0 ? 0 : u;\
  v = v < 0 ? 0 : v;\
  y = y > 255 ? 255 : y;\
  u = u > 255 ? 255 : u;\
  v = v > 255 ? 255 : v; }

int main() {
    u32 wpaddown;
    u32 wpadup;
    GRRLIB_Init();
    GRRLIB_2dMode();
    WPAD_Init();
    WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);
    GRRLIB_texImg* tex_sprite_png = GRRLIB_CreateEmptyTexture(160, 144);
    GRRLIB_texImg *tex_BMfont5 = GRRLIB_LoadTexture(BMfont5_png);
    GRRLIB_InitTileSet(tex_BMfont5, 8, 16, 0);
    GB_WiiWrapper gb;
    gb.LoadCartridge((void*)Tetris_gbc);
    while(1) {
        WPAD_SetVRes(0, 640, 480);
        WPAD_ScanPads();
        wpaddown = WPAD_ButtonsDown(0);
        wpadup = WPAD_ButtonsUp(0);
        GRRLIB_FillScreen(GRRLIB_BLACK);
        if(wpaddown & WPAD_BUTTON_HOME) {
            break;
        } else if (wpaddown != 0) {
            gb.HandleKeyDown(wpaddown);
        }
        if (wpadup) {
            gb.HandleKeyUp(wpadup);
        }
        for (int i = 0; i < 70000; i++) {
            gb.Update();
        }
        uint8_t* data = (uint8_t*)gb.GetScreenData();
        int pix = 0;
        for (int y = 0; y < 144; y++) {
            for (int x = 0; x < 160; x++) {
                uint32_t col = *(uint32_t*)(data + pix);
                GRRLIB_SetPixelTotexImg(x, y, tex_sprite_png, col);
                pix += 4;
            }
        }
        GRRLIB_FlushTex(tex_sprite_png);
        GRRLIB_DrawImg(0, 0, tex_sprite_png, 0, 1, 1, GRRLIB_WHITE);
        std::stringstream ss;
        ss << std::hex << gb.cpu_.Instructions[gb.cpu_.last_instr_].name << " " << (int)gb.cpu_.PC << " A:" << (int)gb.cpu_.A << " B:" << (int)gb.cpu_.B << " C:" << (int)gb.cpu_.D << " D:" << (int)gb.cpu_.D << " E:" << (int)gb.cpu_.E << " H:" << (int)gb.cpu_.H << " L:" << (int)gb.cpu_.L;
        std::string s = ss.str();
        GRRLIB_Printf(200, 27, tex_BMfont5, GRRLIB_WHITE, 1, s.c_str());
        GRRLIB_Render();
    }
    GRRLIB_FreeTexture(tex_sprite_png);
    GRRLIB_Exit();
    return 0;
}