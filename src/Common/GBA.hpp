#ifndef GBA_HPP
#define GBA_HPP

#include <cstdint>

// Cart backup type can be detected by strings in the ROM image.
// The ID string must be located at a word-aligned memory location,
// the string length should be a multiple of 4 bytes (padded with zero's).
//   EEPROM_Vnnn    EEPROM 512 bytes or 8 Kbytes (4Kbit or 64Kbit)
//   SRAM_Vnnn      SRAM 32 Kbytes (256Kbit)
//   FLASH_Vnnn     FLASH 64 Kbytes (512Kbit) (ID used in older files)
//   FLASH512_Vnnn  FLASH 64 Kbytes (512Kbit) (ID used in newer files)
//   FLASH1M_Vnnn   FLASH 128 Kbytes (1Mbit)
enum SaveType
{
    SAVE_EEPROM512B = 0,
    SAVE EEPROM4KB  = 1,
    SAVE_SRAM32KB   = 2,
    SAVE_FLASH64KB  = 3,
    SAVE_FLASH128KB = 4
};

struct GBAHeader
{
    uint32_t EntryPoint;
    uint8_t Logo[156];
    char Title[12];
    char GameCode[4];
    char MakerCode[2];
    uint8_t Is96h;
    uint8_t UnitCode;
    uint8_t DeviceCode;
    uint8_t Reserved[7];
    uint8_t SoftwareVersion;
    uint8_t ChecksumComplement;
    uint8_t Unused2[2];
    // Multiboot data
    // uint32_t RamEntryPoint;
    // uint8_t BootMode;
    // uint8_t SlaveId;
    // uint8_t Unused3[26];
    // uint32_t JoyBusStart;

    bool VerifyHeader()
    {
        if (Is96h != 0x96)
            return false;

        return VerifyChecksum();
    }

    bool VerifyChecksum()
    {
        // The checksum is calculated using the bytes from the ROM header itself
        uint8_t check = 0;
        for (uint32_t i = 0xA0; i <= 0xBC; ++i)
            check -= *((uint8_t*)this + i);

        check = (check - 0x19) & 0xFF;

        return ChecksumComplement == check;
    }


};

#endif