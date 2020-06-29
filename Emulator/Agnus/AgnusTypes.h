// -----------------------------------------------------------------------------
// This file is part of vAmiga
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

// This file must conform to standard ANSI-C to be compatible with Swift.

#ifndef _AGNUS_T_INC
#define _AGNUS_T_INC

typedef enum : long
{
    AGNUS_8367, // OCS Agnus
    AGNUS_8372, // ECS Agnus (up to 1MB Chip Ram)
    AGNUS_8375, // ECS Agnus (up to 2MB Chip Ram)
    AGNUS_CNT
}
AgnusRevision;

inline bool isAgnusRevision(long value)
{
    return value >= 0 && value < AGNUS_CNT;
}

inline const char *agnusRevisionName(AgnusRevision type)
{
    assert(isAgnusRevision(type));

    switch (type) {
        case AGNUS_8367: return "AGNUS_8367";
        case AGNUS_8372: return "AGNUS_8372";
        case AGNUS_8375: return "AGNUS_8375";
        default:         return "???";
    }
}

typedef struct
{
    AgnusRevision revision;
}
AgnusConfig;

// Register change identifiers
typedef enum : i32
{
    REG_NONE = 0,
    REG_BLTSIZE,
    REG_BLTSIZV,
    REG_BLTCON0,
    REG_BLTCON0L,
    REG_BLTCON1,
    REG_INTREQ,
    REG_INTENA,
    REG_BPLCON0_AGNUS,
    REG_BPLCON0_DENISE,
    REG_BPLCON1_AGNUS,
    REG_BPLCON1_DENISE,
    REG_BPLCON2,
    REG_DMACON,
    REG_DIWSTRT,
    REG_DIWSTOP,
    REG_DDFSTRT,
    REG_DDFSTOP,
    REG_BPL1MOD,
    REG_BPL2MOD,
    REG_BPL1PTH,
    REG_BPL1PTL,
    REG_BPL2PTH,
    REG_BPL2PTL,
    REG_BPL3PTH,
    REG_BPL3PTL,
    REG_BPL4PTH,
    REG_BPL4PTL,
    REG_BPL5PTH,
    REG_BPL5PTL,
    REG_BPL6PTH,
    REG_BPL6PTL,

    REG_SPR0DATA,
    REG_SPR1DATA,
    REG_SPR0DATB,
    REG_SPR1DATB,
    REG_SPR0POS,
    REG_SPR1POS,
    REG_SPR0CTL,
    REG_SPR1CTL,

    REG_SPR2DATA,
    REG_SPR3DATA,
    REG_SPR2DATB,
    REG_SPR3DATB,
    REG_SPR2POS,
    REG_SPR3POS,
    REG_SPR2CTL,
    REG_SPR3CTL,

    REG_SPR4DATA,
    REG_SPR5DATA,
    REG_SPR4DATB,
    REG_SPR5DATB,
    REG_SPR4POS,
    REG_SPR5POS,
    REG_SPR4CTL,
    REG_SPR5CTL,

    REG_SPR6DATA,
    REG_SPR7DATA,
    REG_SPR6DATB,
    REG_SPR7DATB,
    REG_SPR6POS,
    REG_SPR7POS,
    REG_SPR6CTL,
    REG_SPR7CTL,
    
    REG_COUNT
}
RegChangeID;

static inline bool isRegChangeID(long value)
{
    return value >= 0 && value < REG_COUNT;
}

typedef enum : i8
{
    BUS_NONE,
    BUS_CPU,
    BUS_REFRESH,
    BUS_DISK,
    BUS_AUDIO,
    BUS_BITPLANE,
    BUS_SPRITE,
    BUS_COPPER,
    BUS_BLITTER,
    BUS_OWNER_COUNT
}
BusOwner;

static inline bool isBusOwner(long value)
{
    return value >= 0 && value < BUS_OWNER_COUNT;
}

typedef enum
{
    DDF_OFF,
    DDF_READY,
    DDF_ON
}
DDFState;

typedef enum
{
    SPR_DMA_IDLE,
    SPR_DMA_ACTIVE
}
SprDMAState;

typedef enum : long
{
    MODULATE_FG_LAYER,
    MODULATE_BG_LAYER,
    MODULATE_ODD_EVEN_LAYERS
}
DmaDebuggerDisplayMode;

typedef struct
{
    bool enabled;
    bool visualize[BUS_OWNER_COUNT];
    double colorRGB[BUS_OWNER_COUNT][3];
    double opacity;
    DmaDebuggerDisplayMode displayMode;
}
DMADebuggerInfo;

typedef struct
{
    i16 vpos;
    i16 hpos;

    u16 dmacon;
    u16 bplcon0;
    u8  bpu;
    u16 ddfstrt;
    u16 ddfstop;
    u16 diwstrt;
    u16 diwstop;

    u16 bpl1mod;
    u16 bpl2mod;
    u16 bltamod;
    u16 bltbmod;
    u16 bltcmod;
    u16 bltdmod;
    u16 bltcon0;
    
    u32 coppc;
    u32 dskpt;
    u32 bplpt[6];
    u32 audpt[4];
    u32 audlc[4];
    u32 bltpt[4];
    u32 sprpt[8];

    bool bls;
}
AgnusInfo;

typedef struct
{
    struct {
        long raw[BUS_OWNER_COUNT];
        double accumulated[BUS_OWNER_COUNT];
    } bus;
}
AgnusStats;

typedef struct
{
    u8   copList;
    bool active;
    bool cdang;
    u32  coppc;
    u32  cop1lc;
    u32  cop2lc;
    u16  cop1ins;
    u16  cop2ins;
    i16  length1;
    i16  length2;
}
CopperInfo;

typedef struct
{
    int accuracy;
}
BlitterConfig;

typedef struct
{
    // bool active;
    u16 bltcon0;
    u16 bltcon1;
    u16 ash;
    u16 bsh;
    u16 minterm;
    u16 bltapt;
    u16 bltbpt;
    u16 bltcpt;
    u16 bltdpt;
    u16 bltafwm;
    u16 bltalwm;
    i16 bltamod;
    i16 bltbmod;
    i16 bltcmod;
    i16 bltdmod;
    u16 aold;
    u16 bold;
    u16 anew;
    u16 bnew;
    u16 ahold;
    u16 bhold;
    u16 chold;
    u16 dhold;
    bool bbusy;
    bool bzero;
    bool firstIteration;
    bool lastIteration;
    bool fci;
    bool fco;
    bool fillEnable;
    bool storeToDest;
}
BlitterInfo;

#endif
