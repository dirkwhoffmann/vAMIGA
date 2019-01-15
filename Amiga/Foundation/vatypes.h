// -----------------------------------------------------------------------------
// This file is part of vAmiga
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _VATYPES_H
#define _VATYPES_H

//
// Amiga hardware
//

typedef enum : long
{
    A500,
    A1000,
    A2000
}
AmigaModel;

inline bool isAmigaModel(AmigaModel model)
{
    return model >= A500 && model <= A2000;
}

inline const char *modelName(AmigaModel model)
{
    return
    model == A500 ? "Amiga 500" :
    model == A1000 ? "Amiga 1000" :
    model == A2000 ? "Amiga 2000" : "???";
}

typedef enum : long
{
    A1010_ORIG, // Amiga 3,5" drive, emulated with original speed
    A1010_2X,   // Amiga 3,5" drive, emulated 2x faster
    A1010_4X,   // Amiga 3,5" drive, emulated 4x faster
    A1010_8X,   // Amiga 3,5" drive, emulated 8x faster
    A1010_WARP  // Amiga 3,5" drive, emulated as fast as possible
}
DriveType;

inline bool isDriveType(DriveType model)
{
    return model >= A1010_ORIG && model <= A1010_WARP;
}

inline const char *driveTypeName(DriveType type)
{
    return
    type == A1010_ORIG ? "A1010 (original speed)" :
    type == A1010_2X   ? "A1010 (2x faster)" :
    type == A1010_4X   ? "A1010 (4x faster)" :
    type == A1010_8X   ? "A1010 (8x faster)" :
    type == A1010_WARP ? "A1010 (warp speed)" : "???";
}


//
// Amiga configuration
//

/* Amiga configuration
 * This is a full description of the computer we are going to emulate.
 */
typedef struct
{
    DriveType type;
    bool connected;
}
DriveConfiguration;

typedef struct
{
    AmigaModel model;
    long chipRamSize; // size in KB
    long slowRamSize; // size in KB
    long fastRamSize; // size in KB
    bool realTimeClock;
    DriveConfiguration df0;
    DriveConfiguration df1;
}
AmigaConfiguration;


//
// Memory
//

/* Memory source identifiers
 * The identifiers are used in the mem source lookup table to specify the
 * source and target of a peek or poke operation, respectively.
 */
typedef enum
{
    MEM_UNMAPPED,
    MEM_ROM,
    MEM_WOM,
    MEM_CHIP,
    MEM_SLOW,
    MEM_FAST,
    MEM_CIA,
    MEM_RTC,
    MEM_CUSTOM
} MemorySource;


//
// External files (snapshots, disk images, etc.)
//

typedef enum
{
    FILETYPE_UKNOWN = 0,
    FILETYPE_SNAPSHOT,
    FILETYPE_ADF,
    FILETYPE_BOOT_ROM,
    FILETYPE_KICK_ROM
    
} VAFileType;

inline bool isVAFileType(VAFileType model) {
    return model >= FILETYPE_UKNOWN && model <= FILETYPE_KICK_ROM;
}

//
// Notification messages (GUI communication)
//

// List of all known message id's
typedef enum
{
    MSG_NONE = 0,
    
    // Emulator state
    MSG_READY_TO_POWER_ON,
    MSG_POWER_ON,
    MSG_POWER_OFF,
    MSG_RUN,
    MSG_PAUSE,
    MSG_RESET,
    MSG_ROM_MISSING,

    // Floppy drives
    MSG_DRIVE_CONNECT,
    MSG_DRIVE_DISCONNECT,
    MSG_DRIVE_LED_ON,
    MSG_DRIVE_LED_OFF,
    MSG_DRIVE_DISK_INSERT,
    MSG_DRIVE_DISK_EJECT,
    MSG_DRIVE_DISK_SAVED,
    MSG_DRIVE_DISK_UNSAVED,
    MSG_DRIVE_DMA_ON,
    MSG_DRIVE_DMA_OFF,
    MSG_DRIVE_HEAD_UP,
    MSG_DRIVE_HEAD_DOWN,
    
    
    
    //
    // DEPRECATED C64 related messages ...
    //
    
    // ROM and snapshot handling
    MSG_BASIC_ROM_LOADED,
    MSG_CHAR_ROM_LOADED,
    MSG_KERNAL_ROM_LOADED,
    MSG_VC1541_ROM_LOADED,
    MSG_SNAPSHOT_TAKEN,
    
    // CPU related messages
    MSG_CPU_OK,
    MSG_CPU_SOFT_BREAKPOINT_REACHED,
    MSG_CPU_HARD_BREAKPOINT_REACHED,
    MSG_CPU_ILLEGAL_INSTRUCTION,
    MSG_WARP_ON,
    MSG_WARP_OFF,
    MSG_ALWAYS_WARP_ON,
    MSG_ALWAYS_WARP_OFF,
    
    // VIC related messages
    MSG_PAL,
    MSG_NTSC,
    
    // IEC Bus
    MSG_IEC_BUS_BUSY,
    MSG_IEC_BUS_IDLE,
    
    // Keyboard
    MSG_KEYMATRIX,
    MSG_CHARSET,
    
    // Peripherals (Disk drive)
    MSG_VC1541_ATTACHED,
    MSG_VC1541_ATTACHED_SOUND,
    MSG_VC1541_DETACHED,
    MSG_VC1541_DETACHED_SOUND,
    MSG_VC1541_DISK,
    MSG_VC1541_DISK_SOUND,
    MSG_VC1541_NO_DISK,
    MSG_VC1541_NO_DISK_SOUND,
    MSG_VC1541_RED_LED_ON,
    MSG_VC1541_RED_LED_OFF,
    MSG_VC1541_MOTOR_ON,
    MSG_VC1541_MOTOR_OFF,
    MSG_VC1541_HEAD_UP,
    MSG_VC1541_HEAD_UP_SOUND,
    MSG_VC1541_HEAD_DOWN,
    MSG_VC1541_HEAD_DOWN_SOUND,
    
    // Peripherals (Disk)
    MSG_DISK_SAVED,
    MSG_DISK_UNSAVED,
    
    // Peripherals (Datasette)
    MSG_VC1530_TAPE,
    MSG_VC1530_NO_TAPE,
    MSG_VC1530_PROGRESS,
    
    // Peripherals (Expansion port)
    MSG_CARTRIDGE,
    MSG_NO_CARTRIDGE,
    MSG_CART_SWITCH
    
} MessageType;

/* A single message
 * Only a very messages utilize the data file. E.g., the drive related message
 * use it to code the drive number (0 = df0 etc.).
 */
typedef struct {
    MessageType type;
    long data;
} Message;

// Callback function signature
typedef void Callback(const void *, int, long);

#endif
