#include <grrlib.h>
#include <stdlib.h>
#include <wiiuse/wpad.h>
#include "GameboyTKP/gb_wiiwrapper.h"
#include "BMfont5_png.h"

int main() {
    u32 wpaddown;
    u32 wpadup;
    GRRLIB_Init();
    GRRLIB_2dMode();
    WPAD_Init();
    WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);
    GRRLIB_texImg* screen = GRRLIB_CreateEmptyTexture(160, 144);
    GRRLIB_texImg *tex_BMfont5 = GRRLIB_LoadTexture(BMfont5_png);
    GRRLIB_InitTileSet(tex_BMfont5, 8, 16, 0);
    GB_WiiWrapper gb;
    uint8_t* data;
    int ret = GRRLIB_LoadFile("roms/rom.gbc", &data);
    std::stringstream errorss;
    if (ret <= 0) {
        errorss << "Error loading rom: " << ret;
    }
    gb.LoadCartridge(data);
    gb.bus_.SoftReset();
    gb.cpu_.Reset(true);
    gb.timer_.Reset();
    gb.ppu_.Reset();
    std::string s = errorss.str();
    while(1) {
        WPAD_SetVRes(0, 640, 480);
        WPAD_ScanPads();
        wpaddown = WPAD_ButtonsDown(0);
        wpadup = WPAD_ButtonsUp(0);
        GRRLIB_FillScreen(0);
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
                GRRLIB_SetPixelTotexImg(x, y, screen, col);
                pix += 4;
            }
        }
        GRRLIB_FlushTex(screen);
        GRRLIB_DrawImg(200, 100, screen, 0, 2, 2, 0xFFFFFFFF);
        GRRLIB_Printf(0, 400, tex_BMfont5, 0xFFFFFFFF, 1, s.c_str());
        GRRLIB_Render();
    }
    GRRLIB_FreeTexture(screen);
    GRRLIB_Exit();
    return 0;
}