#ifndef GPU_HPP
#define GPU_HPP

#include <cstdint>
#include <cstring>
#include <memory>

#include "LCD.hpp"
#include "Common/Utilities.hpp"

class CPU;

enum VideoMode
{
    // In this mode, four text background layers can be shown.
    // In this mode backgrounds 0 - 3 all count as "text" backgrounds,
    // and cannot be scaled or rotated.
    MODE_0 = 0,
    // This mode is similar in most respects to Mode 0, the main difference being that only 3 backgrounds are accessible -- 0, 1, and 2. Bgs 0 and 1 are text backgrounds, while bg 2 is a rotation/scaling background.
    MODE_1 = 1,
    MODE_2 = 2,
    MODE_3 = 3,
    MODE_4 = 4,
    MODE_5 = 5,
};

enum VideoData
{
    CYCLES_PER_PIXEL = 4,
    HORIZONTAL_PIXELS = 240,
    HDRAW_LENGTH = 1006,
    HBLANK_LENGTH = 226,
    HORIZONTAL_LENGTH = 1232,
    VERTICAL_PIXELS = 160,
    VBLANK_PIXELS = 68,
    VERTICAL_TOTAL_PIXELS = 228,
    TOTAL_LENGTH = 280896,

    VDRAW_DURATION = 160, // 160 scanlines
    VBLANK_DURATION = 68, // 68 scanlines
    HDRAW_DURATION = 1006, // 1006 scanlines
    HBLANK_DURATION = 228, // 228 scanlines (various sizes tell 226)
};

#define VCOUNT 0x4000006 // LY
#define DISPSTAT 0x4000004
#define DISPCNT 0x4000000

class GPU final
{
    public:
        GPU(CPU* cpu) : _cpu(cpu), _adapter(nullptr), _nextEvent(HDRAW_LENGTH), _lastHBlank(0), _nextHBlank(HDRAW_LENGTH)
        {
            memset(_vram, 0, sizeof(_vram) / sizeof(uint8_t));
            memset(_oam, 0, sizeof(_oam) / sizeof(uint8_t));
            memset(_obj, 0, sizeof(_obj) / sizeof(uint8_t));
        }

        /*
         * @description Main loop logic
         */
        void Step(uint32_t cycles);
        
        int32_t ReadInt32(uint32_t offset) { return int32_t(ReadUInt32(offset)); }
        uint32_t ReadUInt32(uint32_t offset);
        int16_t ReadInt16(uint32_t offset) { return int16_t(ReadUInt16(offset)); }
        uint16_t ReadUInt16(uint32_t offset);
        int8_t ReadInt8(uint32_t offset) { return int8_t(ReadUInt8(offset)); }
        uint8_t ReadUInt8(uint32_t offset);
        bool ReadBit(uint32_t offset, uint8_t bitIndex);

        void WriteUInt8(uint32_t offset, uint8_t value);
        void WriteUInt16(uint32_t offset, uint16_t value);
        void WriteUInt32(uint32_t offset, uint32_t value);
        void WriteBit(uint32_t offset, uint8_t bitIndex, bool isSet);
        
        /**
         * @description A helper function that reads color data from palette u16
         */
        void ExtractColorValues(uint16_t input, uint8_t& red, uint8_t& green, uint8_t& blue);

        void SetLCDAdapter(std::shared_ptr<LCDAdapter> adapter) { _adapter = adapter; }

        bool InHBlank();
        bool InVBlank();
        uint8_t GetCurrentLine();

        VideoMode GetVideoMode();
        bool IsBackgroundActive(uint8_t bg);

        void DrawHorizontal(uint8_t line);

    private:
        uint32_t _nextEvent;
        uint32_t _lastHBlank;
        uint32_t _nextHBlank;
        CPU* _cpu;
        uint8_t _vram[0x18000]; // VRAM (96KB)
        uint8_t _oam[0x400];    // OAM (1KB)
        uint8_t _obj[0x400];    // BG/OBJ Palette (1KB)
        std::shared_ptr<LCDAdapter> _adapter;
};

#endif
