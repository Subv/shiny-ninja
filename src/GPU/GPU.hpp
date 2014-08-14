#ifndef GPU_HPP
#define GPU_HPP

enum VideoMode
{
    MODE_0 = 0,
    MODE_1 = 1,
    MODE_2 = 2,
    MODE_3 = 3,
    MODE_4 = 4,
    MODE_5 = 5,
};

class GPU final
{
    private:
        GPU()
        {
            memset(_vram, 0, sizeof(_vram) / sizeof(uint8_t));
            memset(_vram, 0, sizeof(_oam) / sizeof(uint8_t));
            memset(_vram, 0, sizeof(_obj) / sizeof(uint8_t));
        }

    public:
        static std::unique_ptr<GPU>& Instance()
        {
            static std::unique_ptr<GPU> _instance(new GPU());
            return _instance;
        }
        
        uint32_t ReadInt32(uint32_t offset) final;
        uint16_t ReadInt16(uint32_t offset) final;
        
        /**
         * @description A Helper function that reads color data from palette u16
         */
        void ExtractColorValues(uint16_t input, uint8_t& red, uint8_t& green, uint8_t& blue) final;
    
    private:
        uint8_t _vram[0x18000]; // VRAM (96KB)
        uint8_t _oam[0x400];    // OAM (1KB)
        uint8_t _obj[0x400];    // BG/OBJ Palette (1KB)
};

#define sGPU GPU::Instance()
#endif
