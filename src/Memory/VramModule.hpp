#ifndef VRAM_MODULE_HPP
#define VRAM_MODULE_HPP

class VRAM final : public MemoryModule
{
    private:
        VRAM(uint32_t size) : MemoryModule(size) { }

    public:
        static std::unique_ptr<VRAM>& Instance(uint32_t size)
        {
            static std::unique_ptr<VRAM> _instance(new VRAM(size));
            return _instance;
        }
};

#endif