#ifndef MEMORY_MODULE_HPP
#define MEMORY_MODULE_HPP

class MemoryModule
{
    public:
        MemoryModule(uint32_t memorySize)
        {
            _capacity = memorySize;
            _data = new uint8[memorySize];
        }

        virtual ~MemoryModule()
        {
            delete[] _data;
        }

        virtual uint8_t ReadUint8(uint32_t offset)
        {
            if (offset > _capacity)
                return 0;
            return _data[offset];
        }

        virtual uint16_t ReadUint16(uint32_t offset)
        {
            if (offset + 1 > _capacity)
                return 0;
            return *(uint16_t*)(&_data[offset]);
        }

        virtual uint32_t ReadUint32(uint32_t offset)
        {
            if (offset + 4 > _capacity)
                return 0;
            return *(uint32_t*)(&_data[offset]);
        }

        virtual void Reset()
        {
            memset(_data, 0, _capacity);
        }

    protected:
        uint8_t* _data;
        uint32_t _capacity;
};

#endif // MEMORY_MODULE_HPP