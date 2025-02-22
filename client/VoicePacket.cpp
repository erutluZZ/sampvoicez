/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "VoicePacket.h"

static DWORD CalcCrc32cHash(LPCBYTE buffer, DWORD length, DWORD crc = 0) noexcept
{
    crc = ~crc;

    while (length-- != 0)
    {
        crc ^= *buffer++;

        for (int k = 0; k < 8; ++k)
        {
            crc = crc & 1 ? (crc >> 1) ^ 0x82f63b78 : crc >> 1;
        }
    }

    return ~crc;
}

DWORD VoicePacket::GetFullSize() const noexcept
{
    return sizeof(*this) + length;
}

bool VoicePacket::CheckHeader() const noexcept
{
    return hash == CalcCrc32cHash((PBYTE)(this) + sizeof(hash), sizeof(*this) - sizeof(hash));
}

void VoicePacket::CalcHash() noexcept
{
    hash = CalcCrc32cHash((PBYTE)(this) + sizeof(hash), sizeof(*this) - sizeof(hash));
}
