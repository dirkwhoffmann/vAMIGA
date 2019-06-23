// -----------------------------------------------------------------------------
// This file is part of vAmiga
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "KickRom.h"

// AROS Kickstart replacement
const uint8_t KickRom::magicBytes1[] = { 0x11, 0x14, 0x4E, 0xF9, 0x00, 0xF8, 0x00 };
const uint8_t KickRom::magicBytes1a[] = { 0x11, 0x11, 0x4E, 0xF9, 0x00, 0xF8, 0x00 };

// Kickstart 1.2 and 1.3
const uint8_t KickRom::magicBytes2[] = { 0x11, 0x11, 0x4E, 0xF9, 0x00, 0xFC, 0x00 };

// Kickstart 2.04
const uint8_t KickRom::magicBytes3[] = { 0x11, 0x14, 0x4E, 0xF9, 0x00, 0xF8, 0x00 };

// Diagnostic v2.0 (Logica)
const uint8_t KickRom::magicBytes4[] = { 0x11, 0x11, 0x4E, 0xF9, 0x00, 0xF8, 0x04 };

KickRom::KickRom()
{
    setDescription("KickRom");
}

bool
KickRom::isKickRomBuffer(const uint8_t *buffer, size_t length)
{
    if (length != KB(256) && length != KB(512)) return false;
    
    return
    matchingBufferHeader(buffer, magicBytes1, sizeof(magicBytes1)) ||
    matchingBufferHeader(buffer, magicBytes1a, sizeof(magicBytes1a)) ||
    matchingBufferHeader(buffer, magicBytes2, sizeof(magicBytes2)) ||
    matchingBufferHeader(buffer, magicBytes3, sizeof(magicBytes3)) ||
    matchingBufferHeader(buffer, magicBytes4, sizeof(magicBytes4));
}

bool
KickRom::isKickRomFile(const char *path)
{
    if (!checkFileSize(path, KB(256)) &&
        !checkFileSize(path, KB(512))) return false;
    
    return
    matchingFileHeader(path, magicBytes1, sizeof(magicBytes1)) ||
    matchingFileHeader(path, magicBytes1a, sizeof(magicBytes1a)) ||
    matchingFileHeader(path, magicBytes2, sizeof(magicBytes2)) ||
    matchingFileHeader(path, magicBytes3, sizeof(magicBytes3)) ||
    matchingFileHeader(path, magicBytes4, sizeof(magicBytes4));
}

KickRom *
KickRom::makeWithBuffer(const uint8_t *buffer, size_t length)
{
    KickRom *rom = new KickRom();
    
    if (!rom->readFromBuffer(buffer, length)) {
        delete rom;
        return NULL;
    }
    
    return rom;
}

KickRom *
KickRom::makeWithFile(const char *path)
{
    KickRom *rom = new KickRom();
    
    if (!rom->readFromFile(path)) {
        delete rom;
        return NULL;
    }
    
    return rom;
}

bool
KickRom::readFromBuffer(const uint8_t *buffer, size_t length)
{
    if (!AmigaFile::readFromBuffer(buffer, length))
        return false;
    
    return isKickRomBuffer(buffer, length);
}
