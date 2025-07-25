#ifndef Z64_H
#define Z64_H

#include "z64hdr.h"
#include "ultra64.h"
// #include "ultra64/gs2dex.h"
#include "z64save.h"
#include "z64light.h"
#include "z64bgcheck.h"
#include "z64actor.h"
#include "z64player.h"
#include "z64audio.h"
#include "z64object.h"
#include "z64ocarina.h"
#include "z64camera.h"
#include "z64environment.h"
#include "z64cutscene.h"
#include "z64collision_check.h"
#include "z64curve.h"
#include "z64scene.h"
#include "z64effect.h"
#include "z64item.h"
#include "z64animation.h"
#include "z64dma.h"
#include "z64math.h"
#include "z64map_mark.h"
#include "z64skin.h"
#include "z64transition.h"
#include "z64interface.h"
#include "alignment.h"
#include "sequence.h"
#include "sfx.h"
#include "color.h"
#include "ichain.h"
#include "regs.h"
#include "irqmgr.h"
#include "padmgr.h"
#include "fault.h"
#include "sched.h"
#include "rumble.h"

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240

#define REGION_NULL 0
#define REGION_JP 1
#define REGION_US 2
#define REGION_EU 3

#define THREAD_PRI_IDLE_INIT    10
#define THREAD_PRI_MAIN_INIT    10
#define THREAD_PRI_DMAMGR_LOW   10  // Used when decompressing files
#define THREAD_PRI_GRAPH        11
#define THREAD_PRI_AUDIOMGR     12
#define THREAD_PRI_PADMGR       14
#define THREAD_PRI_MAIN         15
#define THREAD_PRI_SCHED        15
#define THREAD_PRI_DMAMGR       16
#define THREAD_PRI_IRQMGR       17
#define THREAD_PRI_FAULT_CLIENT (OS_PRIORITY_APPMAX - 1)
#define THREAD_PRI_FAULT        OS_PRIORITY_APPMAX

#define THREAD_ID_IDLE        1
#define THREAD_ID_FAULT       2
#define THREAD_ID_MAIN        3
#define THREAD_ID_GRAPH       4
#define THREAD_ID_SCHED       5
#define THREAD_ID_PADMGR      7
#define THREAD_ID_AUDIOMGR   10
#define THREAD_ID_DMAMGR     18
#define THREAD_ID_IRQMGR     19

#define STACK(stack, size) \
    u64 stack[ALIGN8(size) / sizeof(u64)]

#define STACK_TOP(stack) \
    ((u8*)(stack) + sizeof(stack))

// Texture memory size, 4 KiB
#define TMEM_SIZE 0x1000

// NOTE: Once we start supporting other builds, this can be changed with an ifdef
#define REGION_NATIVE REGION_EU

typedef struct{
    /* 0x00 */ char unk[0x4];
    /* 0x04 */ MtxF mf;
} HorseStruct;

typedef struct {
    /* 0x00 */ s32  regPage; // 0: no page selected (reg editor is not active); 1: first page; `REG_PAGES`: last page
    /* 0x04 */ s32  regGroup; // Indexed from 0 to `REG_GROUPS`-1. Each group has its own character to identify it.
    /* 0x08 */ s32  regCur; // Selected reg, indexed from 0 as the page start
    /* 0x0C */ s32  dPadInputPrev;
    /* 0x10 */ s32  inputRepeatTimer;
    /* 0x14 */ s16  data[REG_GROUPS * REGS_PER_GROUP]; // Accessed through *REG macros, see regs.h
} GameInfo; // size = 0x15D4

typedef struct {
    /* 0x00000 */ u16 headMagic; // GFXPOOL_HEAD_MAGIC
    /* 0x00008 */ Gfx polyOpaBuffer[0x17E0];
    /* 0x0BF08 */ Gfx polyXluBuffer[0x800];
    /* 0x0FF08 */ Gfx overlayBuffer[0x400];
    /* 0x11F08 */ Gfx workBuffer[0x80];
    /* 0x11308 */ Gfx unusedBuffer[0x20];
    /* 0x12408 */ u16 tailMagic; // GFXPOOL_TAIL_MAGIC
} GfxPool; // size = 0x12410

typedef struct {
    /* 0x0000 */ u32    size;
    /* 0x0004 */ void*    bufp;
    /* 0x0008 */ void*    head;
    /* 0x000C */ void*    tail;
} TwoHeadArena; // size = 0x10

typedef struct {
    /* 0x0000 */ u32    size;
    /* 0x0004 */ Gfx*   bufp;
    /* 0x0008 */ Gfx*   p;
    /* 0x000C */ Gfx*   d;
} TwoHeadGfxArena; // size = 0x10

typedef struct GraphicsContext {
    /* 0x0000 */ Gfx* polyOpaBuffer; // Pointer to "Zelda 0"
    /* 0x0004 */ Gfx* polyXluBuffer; // Pointer to "Zelda 1"
    /* 0x0008 */ char unk_008[0x08]; // Unused, could this be pointers to "Zelda 2" / "Zelda 3"
    /* 0x0010 */ Gfx* overlayBuffer; // Pointer to "Zelda 4"
    /* 0x0014 */ u32 unk_014;
    /* 0x0018 */ char unk_018[0x20];
    /* 0x0038 */ OSMesg msgBuff[0x08];
    /* 0x0058 */ OSMesgQueue* schedMsgQueue;
    /* 0x005C */ OSMesgQueue queue;
    /* 0x0078 */ OSScTask task;
    /* 0x00E0 */ char unk_0E0[0xD0];
    /* 0x01B0 */ Gfx* workBuffer;
    /* 0x01B4 */ TwoHeadGfxArena work;
    /* 0x01C4 */ char unk_01C4[0xC0];
    /* 0x0284 */ OSViMode* viMode;
    /* 0x0288 */ char unk_0288[0x20]; // Unused, could this be Zelda 2/3 ?
    /* 0x02A8 */ TwoHeadGfxArena overlay; // "Zelda 4"
    /* 0x02B8 */ TwoHeadGfxArena polyOpa; // "Zelda 0"
    /* 0x02C8 */ TwoHeadGfxArena polyXlu; // "Zelda 1"
    /* 0x02D8 */ u32 gfxPoolIdx;
    /* 0x02DC */ u16* curFrameBuffer;
    /* 0x02E0 */ char unk_2E0[0x04];
    /* 0x02E4 */ u32 viFeatures;
    /* 0x02E8 */ s32 fbIdx;
    /* 0x02EC */ void (*callback)(struct GraphicsContext*, void*);
    /* 0x02F0 */ void* callbackParam;
    /* 0x02F4 */ f32 xScale;
    /* 0x02F8 */ f32 yScale;
    /* 0x02FC */ char unk_2FC[0x04];
} GraphicsContext; // size = 0x300

typedef struct {
   /* 0x0000 */ s32 topY;    // uly (upper left y)
   /* 0x0004 */ s32 bottomY; // lry (lower right y)
   /* 0x0008 */ s32 leftX;   // ulx (upper left x)
   /* 0x000C */ s32 rightX;  // lrx (lower right x)
} Viewport; // size = 0x10

typedef struct {
    /* 0x0000 */ s32    magic; // string literal "VIEW" / 0x56494557
    /* 0x0004 */ GraphicsContext* gfxCtx;
    /* 0x0008 */ Viewport viewport;
    /* 0x0018 */ f32    fovy;  // vertical field of view in degrees
    /* 0x001C */ f32    zNear; // distance to near clipping plane
    /* 0x0020 */ f32    zFar;  // distance to far clipping plane
    /* 0x0024 */ f32    scale; // scale for matrix elements
    /* 0x0028 */ Vec3f  eye;
    /* 0x0034 */ Vec3f  at;
    /* 0x0040 */ Vec3f  up;
    /* 0x0050 */ Vp     vp;
    /* 0x0060 */ Mtx    projection;
    /* 0x00A0 */ Mtx    viewing;
    /* 0x00E0 */ Mtx*   projectionPtr;
    /* 0x00E4 */ Mtx*   viewingPtr;
    /* 0x00E8 */ Vec3f  distortionOrientation;
    /* 0x00F4 */ Vec3f  distortionScale;
    /* 0x0100 */ f32    distortionSpeed;
    /* 0x0104 */ Vec3f  curDistortionOrientation;
    /* 0x0110 */ Vec3f  curDistortionScale;
    /* 0x011C */ u16    normal; // used to normalize the projection matrix
    /* 0x0120 */ s32    flags;
    /* 0x0124 */ s32    unk_124;
} View; // size = 0x128

#define VIEW_VIEWING (1 << 0)
#define VIEW_VIEWPORT (1 << 1)
#define VIEW_PROJECTION_PERSPECTIVE (1 << 2)
#define VIEW_PROJECTION_ORTHO (1 << 3)
#define VIEW_ALL (VIEW_VIEWING | VIEW_VIEWPORT | VIEW_PROJECTION_PERSPECTIVE | VIEW_PROJECTION_ORTHO)

#define VIEW_FORCE_VIEWING (VIEW_VIEWING << 4)
#define VIEW_FORCE_VIEWPORT (VIEW_VIEWPORT << 4)
#define VIEW_FORCE_PROJECTION_PERSPECTIVE (VIEW_PROJECTION_PERSPECTIVE << 4)
#define VIEW_FORCE_PROJECTION_ORTHO (VIEW_PROJECTION_ORTHO << 4)

typedef enum {
    /*  0 */ SETUPDL_0,
    /*  1 */ SETUPDL_1,
    /*  2 */ SETUPDL_2,
    /*  3 */ SETUPDL_3,
    /*  4 */ SETUPDL_4,
    /*  5 */ SETUPDL_5,
    /*  6 */ SETUPDL_6,
    /*  7 */ SETUPDL_7,
    /*  8 */ SETUPDL_8,
    /*  9 */ SETUPDL_9,
    /* 10 */ SETUPDL_10,
    /* 11 */ SETUPDL_11,
    /* 12 */ SETUPDL_12,
    /* 13 */ SETUPDL_13,
    /* 14 */ SETUPDL_14,
    /* 15 */ SETUPDL_15,
    /* 16 */ SETUPDL_16,
    /* 17 */ SETUPDL_17,
    /* 18 */ SETUPDL_18,
    /* 19 */ SETUPDL_19,
    /* 20 */ SETUPDL_20,
    /* 21 */ SETUPDL_21,
    /* 22 */ SETUPDL_22,
    /* 23 */ SETUPDL_23,
    /* 24 */ SETUPDL_24,
    /* 25 */ SETUPDL_25,
    /* 26 */ SETUPDL_26,
    /* 27 */ SETUPDL_27,
    /* 28 */ SETUPDL_28,
    /* 29 */ SETUPDL_29,
    /* 30 */ SETUPDL_30,
    /* 31 */ SETUPDL_31,
    /* 32 */ SETUPDL_32,
    /* 33 */ SETUPDL_33,
    /* 34 */ SETUPDL_34,
    /* 35 */ SETUPDL_35,
    /* 36 */ SETUPDL_36,
    /* 37 */ SETUPDL_37,
    /* 38 */ SETUPDL_38,
    /* 39 */ SETUPDL_39,
    /* 40 */ SETUPDL_40,
    /* 41 */ SETUPDL_41,
    /* 42 */ SETUPDL_42,
    /* 43 */ SETUPDL_43,
    /* 44 */ SETUPDL_44,
    /* 45 */ SETUPDL_45,
    /* 46 */ SETUPDL_46,
    /* 47 */ SETUPDL_47,
    /* 48 */ SETUPDL_48,
    /* 49 */ SETUPDL_49,
    /* 50 */ SETUPDL_50,
    /* 51 */ SETUPDL_51,
    /* 52 */ SETUPDL_52,
    /* 53 */ SETUPDL_53,
    /* 54 */ SETUPDL_54,
    /* 55 */ SETUPDL_55,
    /* 56 */ SETUPDL_56,
    /* 57 */ SETUPDL_57,
    /* 58 */ SETUPDL_58,
    /* 59 */ SETUPDL_59,
    /* 60 */ SETUPDL_60,
    /* 61 */ SETUPDL_61,
    /* 62 */ SETUPDL_62,
    /* 63 */ SETUPDL_63,
    /* 64 */ SETUPDL_64,
    /* 65 */ SETUPDL_65,
    /* 66 */ SETUPDL_66,
    /* 67 */ SETUPDL_67,
    /* 68 */ SETUPDL_68,
    /* 69 */ SETUPDL_69,
    /* 70 */ SETUPDL_70,
    /* 71 */ SETUPDL_MAX
} SetupDL;

typedef struct {
    /* 0x00 */ u8   seqId;
    /* 0x01 */ u8   natureAmbienceId;
} SequenceContext; // size = 0x2

typedef struct {
    /* 0x00 */ s32 enabled;
    /* 0x04 */ s32 timer;
} FrameAdvanceContext; // size = 0x8

typedef struct {
    /* 0x00 */ Vec3f    pos;
    /* 0x0C */ f32      unk_0C; // radius?
    /* 0x10 */ Color_RGB8 color;
} TargetContextEntry; // size = 0x14

typedef struct {
    /* 0x00 */ Vec3f    naviRefPos; // possibly wrong
    /* 0x0C */ Vec3f    targetCenterPos;
    /* 0x18 */ Color_RGBAf naviInner;
    /* 0x28 */ Color_RGBAf naviOuter;
    /* 0x38 */ Actor*   arrowPointedActor;
    /* 0x3C */ Actor*   targetedActor;
    /* 0x40 */ f32      unk_40;
    /* 0x44 */ f32      unk_44;
    /* 0x48 */ s16      unk_48;
    /* 0x4A */ u8       activeCategory;
    /* 0x4B */ u8       unk_4B;
    /* 0x4C */ s8       unk_4C;
    /* 0x4D */ char     unk_4D[0x03];
    /* 0x50 */ TargetContextEntry arr_50[3];
    /* 0x8C */ Actor*   unk_8C;
    /* 0x90 */ Actor*   bgmEnemy; // The nearest enemy to player with the right flags that will trigger NA_BGM_ENEMY
    /* 0x94 */ Actor*   unk_94;
} TargetContext; // size = 0x98

typedef struct {
    /* 0x00 */ void*      texture;
    /* 0x04 */ s16      x;
    /* 0x06 */ s16      y;
    /* 0x08 */ u8       width;
    /* 0x09 */ u8       height;
    /* 0x0A */ u8       durationTimer; // how long the title card appears for before fading
    /* 0x0B */ u8       delayTimer; // how long the title card waits to appear
    /* 0x0C */ s16      alpha;
    /* 0x0E */ s16      intensity;
} TitleCardContext; // size = 0x10

typedef struct {
    /* 0x00 */ s32    length; // number of actors loaded of this category
    /* 0x04 */ Actor* head; // pointer to head of the linked list of this category (most recent actor added)
} ActorListEntry; // size = 0x08

typedef struct {
    /* 0x0000 */ u8     freezeFlashTimer;
    /* 0x0001 */ char   unk_01[0x01];
    /* 0x0002 */ u8     unk_02;
    /* 0x0003 */ u8     lensActive;
    /* 0x0004 */ char   unk_04[0x04];
    /* 0x0008 */ u8     total; // total number of actors loaded
    /* 0x000C */ ActorListEntry actorLists[ACTORCAT_MAX];
    /* 0x006C */ TargetContext targetCtx;
    struct {
        /* 0x0104 */ u32    swch;
        /* 0x0108 */ u32    tempSwch;
        /* 0x010C */ u32    unk0;
        /* 0x0110 */ u32    unk1;
        /* 0x0114 */ u32    chest;
        /* 0x0118 */ u32    clear;
        /* 0x011C */ u32    tempClear;
        /* 0x0120 */ u32    collect;
        /* 0x0124 */ u32    tempCollect;
    }                   flags;
    /* 0x0128 */ TitleCardContext titleCtx;
    /* 0x0138 */ char   unk_138[0x04];
    /* 0x013C */ void*  absoluteSpace; // Space used to allocate actor overlays with alloc type ACTOROVL_ALLOC_ABSOLUTE
} ActorContext; // size = 0x140

typedef struct {
    /* 0x00 */ char  unk_00[0x4];
    /* 0x04 */ void* segment;
    /* 0x08 */ u8    state;
    /* 0x0C */ f32   unk_0C;
    /* 0x10 */ u16   frames;
    /* 0x12 */ u16   unk_12;
    /* 0x14 */ s32   subCamId;
    /* 0x18 */ u16   unk_18;
    /* 0x1A */ u8    unk_1A;
    /* 0x1B */ u8    unk_1B;
    /* 0x1C */ CutsceneCameraPoint* subCamLookAtPoints;
    /* 0x20 */ CutsceneCameraPoint* subCamEyePoints;
    /* 0x24 */ CsCmdActorAction* linkAction;
    /* 0x28 */ CsCmdActorAction* npcActions[10]; // "npcdemopnt"
} CutsceneContext; // size = 0x50

typedef struct {
    /* 0x00 */ u16 countdown;
    /* 0x04 */ Vec3f worldPos;
    /* 0x10 */ Vec3f projectedPos;
} SfxSource; // size = 0x1C

typedef enum {
    /* 0x00 */ SKYBOX_NONE,
    /* 0x01 */ SKYBOX_NORMAL_SKY,
    /* 0x02 */ SKYBOX_BAZAAR,
    /* 0x03 */ SKYBOX_OVERCAST_SUNSET,
    /* 0x04 */ SKYBOX_MARKET_ADULT,
    /* 0x05 */ SKYBOX_CUTSCENE_MAP,
    /* 0x07 */ SKYBOX_HOUSE_LINK = 7,
    /* 0x09 */ SKYBOX_MARKET_CHILD_DAY = 9,
    /* 0x0A */ SKYBOX_MARKET_CHILD_NIGHT,
    /* 0x0B */ SKYBOX_HAPPY_MASK_SHOP,
    /* 0x0C */ SKYBOX_HOUSE_KNOW_IT_ALL_BROTHERS,
    /* 0x0E */ SKYBOX_HOUSE_OF_TWINS = 14,
    /* 0x0F */ SKYBOX_STABLES,
    /* 0x10 */ SKYBOX_HOUSE_KAKARIKO,
    /* 0x11 */ SKYBOX_KOKIRI_SHOP,
    /* 0x13 */ SKYBOX_GORON_SHOP = 19,
    /* 0x14 */ SKYBOX_ZORA_SHOP,
    /* 0x16 */ SKYBOX_POTION_SHOP_KAKARIKO = 22,
    /* 0x17 */ SKYBOX_POTION_SHOP_MARKET,
    /* 0x18 */ SKYBOX_BOMBCHU_SHOP,
    /* 0x1A */ SKYBOX_HOUSE_RICHARD = 26,
    /* 0x1B */ SKYBOX_HOUSE_IMPA,
    /* 0x1C */ SKYBOX_TENT,
    /* 0x1D */ SKYBOX_UNSET_1D,
    /* 0x20 */ SKYBOX_HOUSE_MIDO = 32,
    /* 0x21 */ SKYBOX_HOUSE_SARIA,
    /* 0x22 */ SKYBOX_HOUSE_ALLEY,
    /* 0x27 */ SKYBOX_UNSET_27 = 39
} SkyboxId;

typedef struct {
    /* 0x000 */ char unk_00[0x128];
    /* 0x128 */ void* staticSegments[2];
    /* 0x130 */ u16 (*palettes)[256];
    /* 0x134 */ Gfx (*dListBuf)[150];
    /* 0x138 */ Gfx* unk_138;
    /* 0x13C */ Vtx* roomVtx;
    /* 0x140 */ s16  unk_140;
    /* 0x144 */ Vec3f rot;
    /* 0x150 */ char unk_150[0x10];
} SkyboxContext; // size = 0x160

typedef enum {
    TEXTBOX_ICON_TRIANGLE,
    TEXTBOX_ICON_SQUARE,
    TEXTBOX_ICON_ARROW
} TextBoxIcon;

typedef enum {
    LANGUAGE_ENG,
    LANGUAGE_GER,
    LANGUAGE_FRA,
    LANGUAGE_MAX
} Language;

// TODO get these properties from the textures themselves
#define FONT_CHAR_TEX_WIDTH  16
#define FONT_CHAR_TEX_HEIGHT 16
#define FONT_CHAR_TEX_SIZE ((16 * 16) / 2) // 16x16 I4 texture

// TODO get these properties from the textures themselves
#define MESSAGE_STATIC_TEX_SIZE 0x1000

typedef enum {
    /* 0x00 */ MSGMODE_NONE,
    /* 0x01 */ MSGMODE_TEXT_START,
    /* 0x02 */ MSGMODE_TEXT_BOX_GROWING,
    /* 0x03 */ MSGMODE_TEXT_STARTING,
    /* 0x04 */ MSGMODE_TEXT_NEXT_MSG,
    /* 0x05 */ MSGMODE_TEXT_CONTINUING,
    /* 0x06 */ MSGMODE_TEXT_DISPLAYING,
    /* 0x07 */ MSGMODE_TEXT_AWAIT_INPUT,
    /* 0x08 */ MSGMODE_TEXT_DELAYED_BREAK,
    /* 0x09 */ MSGMODE_OCARINA_STARTING,
    /* 0x0A */ MSGMODE_SONG_DEMONSTRATION_STARTING,
    /* 0x0B */ MSGMODE_SONG_PLAYBACK_STARTING,
    /* 0x0C */ MSGMODE_OCARINA_PLAYING,
    /* 0x0D */ MSGMODE_OCARINA_CORRECT_PLAYBACK,
    /* 0x0E */ MSGMODE_OCARINA_FAIL, // Failed to play a valid song after entering 8 notes
    /* 0x0F */ MSGMODE_OCARINA_FAIL_NO_TEXT, // Never set, only compared against
    /* 0x10 */ MSGMODE_OCARINA_NOTES_DROP,
    /* 0x11 */ MSGMODE_SONG_PLAYED, // Played a full named song correctly
    /* 0x12 */ MSGMODE_SETUP_DISPLAY_SONG_PLAYED,
    /* 0x13 */ MSGMODE_DISPLAY_SONG_PLAYED,
    /* 0x14 */ MSGMODE_DISPLAY_SONG_PLAYED_TEXT_BEGIN,
    /* 0x15 */ MSGMODE_DISPLAY_SONG_PLAYED_TEXT,
    /* 0x16 */ MSGMODE_SONG_PLAYED_ACT_BEGIN,
    /* 0x17 */ MSGMODE_SONG_PLAYED_ACT, // Act on a played song
    /* 0x18 */ MSGMODE_SONG_DEMONSTRATION_SELECT_INSTRUMENT,
    /* 0x19 */ MSGMODE_SONG_DEMONSTRATION,
    /* 0x1A */ MSGMODE_SONG_DEMONSTRATION_DONE,
    /* 0x1B */ MSGMODE_SONG_PLAYBACK,
    /* 0x1C */ MSGMODE_SONG_PLAYBACK_SUCCESS,
    /* 0x1D */ MSGMODE_SONG_PLAYBACK_FAIL,
    /* 0x1E */ MSGMODE_SONG_PLAYBACK_NOTES_DROP,
    /* 0x1F */ MSGMODE_OCARINA_AWAIT_INPUT,
    /* 0x20 */ MSGMODE_UNK_20, // Never set and does nothing
    /* 0x21 */ MSGMODE_SCARECROW_LONG_RECORDING_START,
    /* 0x22 */ MSGMODE_SCARECROW_LONG_RECORDING_ONGOING,
    /* 0x23 */ MSGMODE_SCARECROW_LONG_PLAYBACK,
    /* 0x24 */ MSGMODE_SCARECROW_SPAWN_RECORDING_START,
    /* 0x25 */ MSGMODE_SCARECROW_SPAWN_RECORDING_ONGOING,
    /* 0x26 */ MSGMODE_SCARECROW_SPAWN_RECORDING_FAILED,
    /* 0x27 */ MSGMODE_SCARECROW_SPAWN_RECORDING_DONE,
    /* 0x28 */ MSGMODE_SCARECROW_SPAWN_PLAYBACK,
    /* 0x29 */ MSGMODE_MEMORY_GAME_START,
    /* 0x2A */ MSGMODE_MEMORY_GAME_LEFT_SKULLKID_PLAYING,
    /* 0x2B */ MSGMODE_MEMORY_GAME_LEFT_SKULLKID_WAIT,
    /* 0x2C */ MSGMODE_MEMORY_GAME_RIGHT_SKULLKID_PLAYING,
    /* 0x2D */ MSGMODE_MEMORY_GAME_RIGHT_SKULLKID_WAIT,
    /* 0x2E */ MSGMODE_MEMORY_GAME_PLAYER_PLAYING,
    /* 0x2F */ MSGMODE_MEMORY_GAME_ROUND_SUCCESS,
    /* 0x30 */ MSGMODE_MEMORY_GAME_START_NEXT_ROUND,
    /* 0x31 */ MSGMODE_FROGS_START,
    /* 0x32 */ MSGMODE_FROGS_PLAYING,
    /* 0x33 */ MSGMODE_FROGS_WAITING,
    /* 0x34 */ MSGMODE_TEXT_AWAIT_NEXT,
    /* 0x35 */ MSGMODE_TEXT_DONE,
    /* 0x36 */ MSGMODE_TEXT_CLOSING,
    /* 0x37 */ MSGMODE_PAUSED // Causes the message system to do nothing until external code sets a new message mode or calls a public function
} MessageMode;

typedef enum {
    /*  0 */ TEXT_STATE_NONE,
    /*  1 */ TEXT_STATE_DONE_HAS_NEXT,
    /*  2 */ TEXT_STATE_CLOSING,
    /*  3 */ TEXT_STATE_DONE_FADING,
    /*  4 */ TEXT_STATE_CHOICE,
    /*  5 */ TEXT_STATE_EVENT,
    /*  6 */ TEXT_STATE_DONE,
    /*  7 */ TEXT_STATE_SONG_DEMO_DONE,
    /*  8 */ TEXT_STATE_8,
    /*  9 */ TEXT_STATE_9,
    /* 10 */ TEXT_STATE_AWAITING_NEXT
} TextState;

typedef struct {
    /* 0x0000 */ u32    msgOffset;
    /* 0x0004 */ u32    msgLength;
    union {
    /* 0x0008 */ u8     charTexBuf[FONT_CHAR_TEX_SIZE * 120];
    /* 0x0008 */ u64    force_structure_alignment_charTex;
    };
    union {
    /* 0x3C08 */ u8     iconBuf[FONT_CHAR_TEX_SIZE];
    /* 0x3C08 */ u64    force_structure_alignment_icon;
    };
    union {
    /* 0x3C88 */ u8     fontBuf[FONT_CHAR_TEX_SIZE * 320];
    /* 0x3C88 */ u64    force_structure_alignment_font;
    };
    union {
    /* 0xDC88 */ char   msgBuf[1280];
    /* 0xDC88 */ u16    msgBufWide[640];
    /* 0xDC88 */ u64    force_structure_alignment_msg;
    };
} Font; // size = 0xE188

#define TEXTBOX_ENDTYPE_DEFAULT     0x00
#define TEXTBOX_ENDTYPE_2_CHOICE    0x10
#define TEXTBOX_ENDTYPE_3_CHOICE    0x20
#define TEXTBOX_ENDTYPE_HAS_NEXT    0x30
#define TEXTBOX_ENDTYPE_PERSISTENT  0x40
#define TEXTBOX_ENDTYPE_EVENT       0x50
#define TEXTBOX_ENDTYPE_FADING      0x60

typedef struct {
    /* 0x0000 */ View   view;
    /* 0x0128 */ Font   font;
    /* 0xE2B0 */ u8*    textboxSegment; // original name: "fukidashiSegment"
    /* 0xE2B4 */ char   unk_E2B4[0x4];
    /* 0xE2B8 */ OcarinaStaff* ocarinaStaff; // original name : "info"
    /* 0xE2BC */ char   unk_E2BC[0x3C];
    /* 0xE2F8 */ u16    textId;
    /* 0xE2FA */ u16    choiceTextId;
    /* 0xE2FC */ u8     textBoxProperties; // original name : "msg_disp_type"
    /* 0xE2FD */ u8     textBoxType; // "Text Box Type"
    /* 0xE2FE */ u8     textBoxPos; // text box position
    /* 0xE300 */ s32    msgLength; // original name : "msg_data"
    /* 0xE304 */ u8     msgMode; // original name: "msg_mode"
    /* 0xE305 */ char   unk_E305[0x1];
    /* 0xE306 */ u8     msgBufDecoded[200]; // decoded message buffer, may be smaller than this
    /* 0xE3CE */ u16    msgBufPos; // original name : "rdp"
    /* 0xE3D0 */ u16    unk_E3D0; // unused, only ever set to 0
    /* 0xE3D2 */ u16    textDrawPos; // draw all decoded characters up to this buffer position
    /* 0xE3D4 */ u16    decodedTextLen; // decoded message buffer length
    /* 0xE3D6 */ u16    textUnskippable;
    /* 0xE3D8 */ s16    textPosX;
    /* 0xE3DA */ s16    textPosY;
    /* 0xE3DC */ s16    textColorR;
    /* 0xE3DE */ s16    textColorG;
    /* 0xE3E0 */ s16    textColorB;
    /* 0xE3E2 */ s16    textColorAlpha;
    /* 0xE3E4 */ u8     textboxEndType; // original name : "select"
    /* 0xE3E5 */ u8     choiceIndex;
    /* 0xE3E6 */ u8     choiceNum; // textboxes that are not choice textboxes have a choiceNum of 1
    /* 0xE3E7 */ u8     stateTimer;
    /* 0xE3E8 */ u16    textDelayTimer;
    /* 0xE3EA */ u16    textDelay;
    /* 0xE3EA */ u16    lastPlayedSong; // original references : "Ocarina_Flog" , "Ocarina_Free"
    /* 0xE3EE */ u16    ocarinaMode; // original name : "ocarina_mode"
    /* 0xE3F0 */ u16    ocarinaAction; // original name : "ocarina_no"
    /* 0xE3F2 */ u16    unk_E3F2; // this is like "lastPlayedSong" but set less often, original name : "chk_ocarina_no"
    /* 0xE3F4 */ u16    unk_E3F4; // unused, only set to 0 in z_actor
    /* 0xE3F6 */ u16    textboxBackgroundIdx;
    /* 0xE3F8 */ u8     textboxBackgroundForeColorIdx;
    /* 0xE3F8 */ u8     textboxBackgroundBackColorIdx;
    /* 0xE3F8 */ u8     textboxBackgroundYOffsetIdx;
    /* 0xE3F8 */ u8     textboxBackgroundUnkArg; // unused, set by the textbox background control character arguments
    /* 0xE3FC */ char   unk_E3FC[0x2];
    /* 0xE3FE */ s16    textboxColorRed;
    /* 0xE400 */ s16    textboxColorGreen;
    /* 0xE402 */ s16    textboxColorBlue;
    /* 0xE404 */ s16    textboxColorAlphaTarget;
    /* 0xE406 */ s16    textboxColorAlphaCurrent;
    /* 0xE408 */ Actor* talkActor;
    /* 0xE40C */ s16    disableWarpSongs; // warp song flag set by scene commands
    /* 0xE40E */ s16    unk_E40E; // ocarina related
    /* 0xE410 */ u8     lastOcarinaButtonIndex;
} MessageContext; // size = 0xE418

typedef enum {
    /* 0x00 */ DO_ACTION_ATTACK,
    /* 0x01 */ DO_ACTION_CHECK,
    /* 0x02 */ DO_ACTION_ENTER,
    /* 0x03 */ DO_ACTION_RETURN,
    /* 0x04 */ DO_ACTION_OPEN,
    /* 0x05 */ DO_ACTION_JUMP,
    /* 0x06 */ DO_ACTION_DECIDE,
    /* 0x07 */ DO_ACTION_DIVE,
    /* 0x08 */ DO_ACTION_FASTER,
    /* 0x09 */ DO_ACTION_THROW,
    /* 0x0A */ DO_ACTION_NONE, // in do_action_static, the texture at this position is NAVI, however this value is in practice the "No Action" value
    /* 0x0B */ DO_ACTION_CLIMB,
    /* 0x0C */ DO_ACTION_DROP,
    /* 0x0D */ DO_ACTION_DOWN,
    /* 0x0E */ DO_ACTION_SAVE,
    /* 0x0F */ DO_ACTION_SPEAK,
    /* 0x10 */ DO_ACTION_NEXT,
    /* 0x11 */ DO_ACTION_GRAB,
    /* 0x12 */ DO_ACTION_STOP,
    /* 0x13 */ DO_ACTION_PUTAWAY,
    /* 0x14 */ DO_ACTION_REEL,
    /* 0x15 */ DO_ACTION_1,
    /* 0x16 */ DO_ACTION_2,
    /* 0x17 */ DO_ACTION_3,
    /* 0x18 */ DO_ACTION_4,
    /* 0x19 */ DO_ACTION_5,
    /* 0x1A */ DO_ACTION_6,
    /* 0x1B */ DO_ACTION_7,
    /* 0x1C */ DO_ACTION_8,
    /* 0x1D */ DO_ACTION_MAX
} DoAction;

typedef struct {
    /* 0x0000 */ View   view;
    /* 0x0128 */ Vtx*   actionVtx;
    /* 0x012C */ Vtx*   beatingHeartVtx;
    /* 0x0130 */ u8*    parameterSegment;
    /* 0x0134 */ u8*    doActionSegment;
    /* 0x0138 */ u8*    iconItemSegment;
    /* 0x013C */ u8*    mapSegment;
    /* 0x0140 */ u8     mapPalette[32];
    /* 0x0160 */ DmaRequest dmaRequest_160;
    /* 0x0180 */ DmaRequest dmaRequest_180;
    /* 0x01A0 */ char   unk_1A0[0x20];
    /* 0x01C0 */ OSMesgQueue loadQueue;
    /* 0x01D8 */ OSMesg loadMsg;
    /* 0x01DC */ Viewport viewport;
    /* 0x01EC */ s16    unk_1EC;
    /* 0x01EE */ u16    unk_1EE;
    /* 0x01F0 */ u16    unk_1F0;
    /* 0x01F4 */ f32    unk_1F4;
    /* 0x01F8 */ s16    naviCalling;
    /* 0x01FA */ s16    unk_1FA;
    /* 0x01FC */ s16    unk_1FC;
    /* 0x01FE */ s16    heartColorOscillator;
    /* 0x0200 */ s16    heartColorOscillatorDirection;
    /* 0x0202 */ s16    beatingHeartPrim[3];
    /* 0x0208 */ s16    beatingHeartEnv[3];
    /* 0x020E */ s16    heartsPrimR[2];
    /* 0x0212 */ s16    heartsPrimG[2];
    /* 0x0216 */ s16    heartsPrimB[2];
    /* 0x021A */ s16    heartsEnvR[2];
    /* 0x021E */ s16    heartsEnvG[2];
    /* 0x0222 */ s16    heartsEnvB[2];
    /* 0x0226 */ s16    unk_226; // Used only in unused functions
    /* 0x0228 */ s16    unk_228; // Used only in unused functions
    /* 0x022A */ s16    beatingHeartOscillator;
    /* 0x022C */ s16    beatingHeartOscillatorDirection;
    /* 0x022E */ s16    unk_22E;
    /* 0x0230 */ s16    lensMagicConsumptionTimer; // When lens is active, 1 unit of magic is consumed every time the timer reaches 0
    /* 0x0232 */ s16    counterDigits[4]; // used for key and rupee counters
    /* 0x023A */ u8     numHorseBoosts;
    /* 0x023C */ u16    unk_23C;
    /* 0x023E */ u16    hbaAmmo; // ammo while playing the horseback archery minigame
    /* 0x0240 */ u16    unk_240;
    /* 0x0242 */ u16    unk_242;
    /* 0x0224 */ u16    unk_244; // screen fill alpha?
    /* 0x0246 */ u16    aAlpha; // also carrots alpha
    /* 0x0248 */ u16    bAlpha; // also HBA score alpha
    /* 0x024A */ u16    cLeftAlpha;
    /* 0x024C */ u16    cDownAlpha;
    /* 0x024E */ u16    cRightAlpha;
    /* 0x0250 */ u16    healthAlpha; // also max C-Up alpha
    /* 0x0252 */ u16    magicAlpha; // also Rupee and Key counters alpha
    /* 0x0254 */ u16    minimapAlpha;
    /* 0x0256 */ s16    startAlpha;
    /* 0x0258 */ s16    unk_258;
    /* 0x025A */ s16    unk_25A;
    /* 0x025C */ s16    mapRoomNum;
    /* 0x025E */ s16    mapPaletteIndex; // "map_palete_no"
    /* 0x0260 */ u8     unk_260;
    /* 0x0261 */ u8     unk_261;
    struct {
        /* 0x0262 */ u8    hGauge;     // "h_gage"; unknown?
        /* 0x0263 */ u8    bButton;    // "b_button"
        /* 0x0264 */ u8    aButton;    // "a_button"
        /* 0x0265 */ u8    bottles;    // "c_bottle"
        /* 0x0266 */ u8    tradeItems; // "c_warasibe"
        /* 0x0267 */ u8    hookshot;   // "c_hook"
        /* 0x0268 */ u8    ocarina;    // "c_ocarina"
        /* 0x0269 */ u8    warpSongs;  // "c_warp"
        /* 0x026A */ u8    sunsSong;   // "m_sunmoon"
        /* 0x026B */ u8    farores;    // "m_wind"
        /* 0x026C */ u8    dinsNayrus; // "m_magic"; din's fire and nayru's love
        /* 0x026D */ u8    all;        // "another"; enables all item restrictions
    }                   restrictions;
} InterfaceContext; // size = 0x270

typedef struct {
    /* 0x00 */ void* loadedRamAddr;
    /* 0x04 */ uintptr_t vromStart;
    /* 0x08 */ uintptr_t vromEnd;
    /* 0x0C */ void* vramStart;
    /* 0x10 */ void* vramEnd;
    /* 0x14 */ u32 offset; // loadedRamAddr - vramStart
    /* 0x18 */ const char* name;
} KaleidoMgrOverlay; // size = 0x1C

typedef enum {
    /* 0x00 */ KALEIDO_OVL_KALEIDO_SCOPE,
    /* 0x01 */ KALEIDO_OVL_PLAYER_ACTOR,
    /* 0x02 */ KALEIDO_OVL_MAX
} KaleidoOverlayType;

#define PAUSE_ITEM_NONE 999

#define PAUSE_CURSOR_PAGE_LEFT 10
#define PAUSE_CURSOR_PAGE_RIGHT 11

typedef enum {
    /* 0x00 */ PAUSE_ITEM,
    /* 0x01 */ PAUSE_MAP,
    /* 0x02 */ PAUSE_QUEST,
    /* 0x03 */ PAUSE_EQUIP,
    /* 0x04 */ PAUSE_WORLD_MAP
} PauseMenuPage;

#define PAUSE_EQUIP_PLAYER_WIDTH 64
#define PAUSE_EQUIP_PLAYER_HEIGHT 112

#define PAUSE_EQUIP_BUFFER_SIZE sizeof(u16[PAUSE_EQUIP_PLAYER_HEIGHT][PAUSE_EQUIP_PLAYER_WIDTH])
#define PAUSE_PLAYER_SEGMENT_GAMEPLAY_KEEP_BUFFER_SIZE 0x5000

typedef struct {
    /* 0x0000 */ View   view;
    /* 0x0128 */ u8*    iconItemSegment;
    /* 0x012C */ u8*    iconItem24Segment;
    /* 0x0130 */ u8*    iconItemAltSegment;
    /* 0x0134 */ u8*    iconItemLangSegment;
    /* 0x0138 */ u8*    nameSegment;
    /* 0x013C */ u8*    playerSegment;
    /* 0x0140 */ char   unk_140[0x04];
    /* 0x0144 */ Vtx*   itemPageVtx;
    /* 0x0148 */ Vtx*   equipPageVtx;
    /* 0x014C */ Vtx*   mapPageVtx;
    /* 0x0150 */ Vtx*   questPageVtx;
    /* 0x0154 */ Vtx*   infoPanelVtx;
    /* 0x0158 */ Vtx*   itemVtx;
    /* 0x015C */ Vtx*   equipVtx;
    /* 0x0160 */ char   unk_160[0x04];
    /* 0x0164 */ Vtx*   questVtx;
    /* 0x0168 */ Vtx*   cursorVtx;
    /* 0x016C */ Vtx*   saveVtx;
    /* 0x0170 */ char   unk_170[0x24];
    /* 0x0194 */ OcarinaStaff* ocarinaStaff;
    /* 0x0198 */ char   unk_198[0x20];
    /* 0x01B8 */ OSMesgQueue loadQueue;
    /* 0x01D0 */ OSMesg loadMsg;
    /* 0x01D4 */ u16    state;
    /* 0x01D6 */ u16    debugState;
    /* 0x01D8 */ Vec3f  eye;
    /* 0x01E4 */ u16    unk_1E4;
    /* 0x01E6 */ u16    mode;
    /* 0x01E8 */ u16    pageIndex; // "kscp_pos"
    /* 0x01EA */ u16    unk_1EA;
    /* 0x01EC */ u16    unk_1EC;
    /* 0x01F0 */ f32    unk_1F0;
    /* 0x01F4 */ f32    unk_1F4;
    /* 0x01F8 */ f32    unk_1F8;
    /* 0x01FC */ f32    unk_1FC;
    /* 0x0200 */ f32    unk_200;
    /* 0x0204 */ f32    unk_204; // "angle_s"
    /* 0x0208 */ u16    alpha;
    /* 0x020A */ s16    offsetY;
    /* 0x020C */ char   unk_20C[0x08];
    /* 0x0214 */ s16    stickAdjX;
    /* 0x0216 */ s16    stickAdjY;
    /* 0x0218 */ s16    cursorPoint[5]; // "cursor_point"
    /* 0x0222 */ s16    cursorX[5]; // "cur_xpt"
    /* 0x022C */ s16    cursorY[5]; // "cur_ypt"
    /* 0x0236 */ s16    dungeonMapSlot;
    /* 0x0238 */ s16    cursorSpecialPos; // "key_angle"
    /* 0x023A */ s16    pageSwitchTimer;
    /* 0x023C */ u16    namedItem; // "zoom_name"
    /* 0x023E */ u16    cursorItem[4]; // "select_name"
    /* 0x0246 */ u16    cursorSlot[4];
    /* 0x024E */ u16    equipTargetItem; // "sl_item_no"
    /* 0x0250 */ u16    equipTargetSlot; // "sl_number"
    /* 0x0252 */ u16    equipTargetCBtn;
    /* 0x0254 */ s16    equipAnimX;
    /* 0x0256 */ s16    equipAnimY;
    /* 0x0258 */ s16    equipAnimAlpha;
    /* 0x025A */ s16    infoPanelOffsetY;
    /* 0x025C */ u16    nameDisplayTimer;
    /* 0x025E */ u16    nameColorSet; // 0 = white; 1 = grey
    /* 0x0260 */ s16    cursorColorSet; // 0 = white; 4 = yellow; 8 = green
    /* 0x0262 */ s16    promptChoice; // save/continue choice: 0 = yes; 4 = no
    /* 0x0264 */ s16    ocarinaSongIdx;
    /* 0x0266 */ u8     worldMapPoints[20]; // 0 = hidden; 1 = displayed; 2 = highlighted
    /* 0x027A */ u8     tradeQuestLocation;
    /* 0x027C */ SkelAnime playerSkelAnime;
} PauseContext; // size = 0x2C0

typedef enum {
    /* 00 */ GAMEOVER_INACTIVE,
    /* 01 */ GAMEOVER_DEATH_START,
    /* 02 */ GAMEOVER_DEATH_WAIT_GROUND, // wait for link to fall and hit the ground
    /* 03 */ GAMEOVER_DEATH_DELAY_MENU, // wait for 1 second before showing the game over menu
    /* 04 */ GAMEOVER_DEATH_MENU, // do nothing while kaleidoscope handles the game over menu
    /* 20 */ GAMEOVER_REVIVE_START = 20,
    /* 21 */ GAMEOVER_REVIVE_RUMBLE,
    /* 22 */ GAMEOVER_REVIVE_WAIT_GROUND, // wait for link to fall and hit the ground
    /* 23 */ GAMEOVER_REVIVE_WAIT_FAIRY, // wait for the fairy to rise all the way up out of links body
    /* 24 */ GAMEOVER_REVIVE_FADE_OUT // fade out the game over lights as link is revived and gets back up
} GameOverState;

typedef struct {
    /* 0x00 */ u16 state;
} GameOverContext; // size = 0x2

typedef struct {
    /* 0x00 */ s16      id;
    /* 0x04 */ void*    segment;
    /* 0x08 */ DmaRequest  dmaRequest;
    /* 0x28 */ OSMesgQueue loadQueue;
    /* 0x40 */ OSMesg   loadMsg;
} ObjectStatus; // size = 0x44

typedef struct {
    /* 0x0000 */ void*  spaceStart;
    /* 0x0004 */ void*  spaceEnd; // original name: "endSegment"
    /* 0x0008 */ u8     num; // number of objects in bank
    /* 0x0009 */ u8     unk_09;
    /* 0x000A */ u8     mainKeepIndex; // "gameplay_keep" index in bank
    /* 0x000B */ u8     subKeepIndex; // "gameplay_field_keep" or "gameplay_dangeon_keep" index in bank
#if Z64ROM_VERSION >= 1007000 && OBJECT_EXCHANGE_BANK_MAX > 19
    /* 0x000C */ ObjectStatus *status;
    /* 0x0010 */ u8            unused[0x508];
#else
    /* 0x000C */ ObjectStatus status[OBJECT_EXCHANGE_BANK_MAX];
#endif
} ObjectContext; // size = 0x518

typedef enum {
    /* 0 */ LENS_MODE_HIDE_ACTORS, // lens actors are visible by default, and hidden by using lens (for example, fake walls)
    /* 1 */ LENS_MODE_SHOW_ACTORS // lens actors are invisible by default, and shown by using lens (for example, invisible enemies)
} LensMode;

typedef enum {
    /* 0 */ ROOM_BEHAVIOR_TYPE1_0,
    /* 1 */ ROOM_BEHAVIOR_TYPE1_1,
    /* 2 */ ROOM_BEHAVIOR_TYPE1_2,
    /* 3 */ ROOM_BEHAVIOR_TYPE1_3, // unused
    /* 4 */ ROOM_BEHAVIOR_TYPE1_4, // unused
    /* 5 */ ROOM_BEHAVIOR_TYPE1_5
} RoomBehaviorType1;

typedef enum {
    /* 0 */ ROOM_BEHAVIOR_TYPE2_0,
    /* 1 */ ROOM_BEHAVIOR_TYPE2_1,
    /* 2 */ ROOM_BEHAVIOR_TYPE2_2,
    /* 3 */ ROOM_BEHAVIOR_TYPE2_3,
    /* 4 */ ROOM_BEHAVIOR_TYPE2_4,
    /* 5 */ ROOM_BEHAVIOR_TYPE2_5,
    /* 6 */ ROOM_BEHAVIOR_TYPE2_6
} RoomBehaviorType2;

typedef struct {
    /* 0x00 */ s8   num;
    /* 0x01 */ u8   unk_01;
    /* 0x02 */ u8   behaviorType2;
    /* 0x03 */ u8   behaviorType1;
    /* 0x04 */ s8   echo;
    /* 0x05 */ u8   lensMode;
    /* 0x08 */ RoomShape* roomShape; // original name: "ground_shape"
    /* 0x0C */ void* segment;
    /* 0x10 */ char unk_10[0x4];
} Room; // size = 0x14

typedef struct {
    /* 0x00 */ Room  curRoom;
    /* 0x14 */ Room  prevRoom;
    /* 0x28 */ void* bufPtrs[2];
    /* 0x30 */ u8    unk_30;
    /* 0x31 */ s8    status;
    /* 0x34 */ void* unk_34;
    /* 0x38 */ DmaRequest dmaRequest;
    /* 0x58 */ OSMesgQueue loadQueue;
    /* 0x70 */ OSMesg loadMsg;
    /* 0x74 */ s16 unk_74[2]; // context-specific data used by the current scene draw config
} RoomContext; // size = 0x78

typedef struct {
    /* 0x000 */ s16 colATCount;
    /* 0x002 */ u16 sacFlags;
    /* 0x004 */ Collider* colAT[COLLISION_CHECK_AT_MAX];
    /* 0x0CC */ s32 colACCount;
    /* 0x0D0 */ Collider* colAC[COLLISION_CHECK_AC_MAX];
    /* 0x1C0 */ s32 colOCCount;
    /* 0x1C4 */ Collider* colOC[COLLISION_CHECK_OC_MAX];
    /* 0x28C */ s32 colLineCount;
    /* 0x290 */ OcLine* colLine[COLLISION_CHECK_OC_LINE_MAX];
} CollisionCheckContext; // size = 0x29C

typedef struct ListAlloc {
    /* 0x00 */ struct ListAlloc* prev;
    /* 0x04 */ struct ListAlloc* next;
} ListAlloc; // size = 0x8

typedef struct {
    /* 0x00 */ s32 width;
    /* 0x04 */ s32 height;
    /* 0x08 */ s32 widthSave;
    /* 0x0C */ s32 heightSave;
    /* 0x10 */ u16* fbuf;
    /* 0x14 */ u16* fbufSave;
    /* 0x18 */ u8* cvgSave;
    /* 0x1C */ u16* zbuf;
    /* 0x20 */ u16* zbufSave;
    /* 0x24 */ s32 ulxSave;
    /* 0x28 */ s32 ulySave;
    /* 0x2C */ s32 lrxSave;
    /* 0x30 */ s32 lrySave;
    /* 0x34 */ s32 ulx;
    /* 0x38 */ s32 uly;
    /* 0x3C */ s32 lrx;
    /* 0x40 */ s32 lry;
    /* 0x44 */ ListAlloc alloc;
    /* 0x4C */ u32 unk_4C;
} PreRender; // size = 0x50

#define TRANS_TRIGGER_OFF 0 // transition is not active
#define TRANS_TRIGGER_START 20 // start transition (exiting an area)
#define TRANS_TRIGGER_END -20 // transition is ending (arriving in a new area)

typedef enum {
    /*  0 */ TRANS_MODE_OFF,
    /*  1 */ TRANS_MODE_SETUP,
    /*  2 */ TRANS_MODE_INSTANCE_INIT,
    /*  3 */ TRANS_MODE_INSTANCE_RUNNING,
    /*  4 */ TRANS_MODE_FILL_WHITE_INIT,
    /*  5 */ TRANS_MODE_FILL_IN,
    /*  6 */ TRANS_MODE_FILL_OUT,
    /*  7 */ TRANS_MODE_FILL_BROWN_INIT,
    /*  8 */ TRANS_MODE_08, // unused
    /*  9 */ TRANS_MODE_09, // unused
    /* 10 */ TRANS_MODE_INSTANT,
    /* 11 */ TRANS_MODE_INSTANCE_WAIT,
    /* 12 */ TRANS_MODE_SANDSTORM_INIT,
    /* 13 */ TRANS_MODE_SANDSTORM,
    /* 14 */ TRANS_MODE_SANDSTORM_END_INIT,
    /* 15 */ TRANS_MODE_SANDSTORM_END,
    /* 16 */ TRANS_MODE_CS_BLACK_FILL_INIT,
    /* 17 */ TRANS_MODE_CS_BLACK_FILL
} TransitionMode;

typedef enum {
    /*  0 */ TRANS_TYPE_WIPE,
    /*  1 */ TRANS_TYPE_TRIFORCE,
    /*  2 */ TRANS_TYPE_FADE_BLACK,
    /*  3 */ TRANS_TYPE_FADE_WHITE,
    /*  4 */ TRANS_TYPE_FADE_BLACK_FAST,
    /*  5 */ TRANS_TYPE_FADE_WHITE_FAST,
    /*  6 */ TRANS_TYPE_FADE_BLACK_SLOW,
    /*  7 */ TRANS_TYPE_FADE_WHITE_SLOW,
    /*  8 */ TRANS_TYPE_WIPE_FAST,
    /*  9 */ TRANS_TYPE_FILL_WHITE2,
    /* 10 */ TRANS_TYPE_FILL_WHITE,
    /* 11 */ TRANS_TYPE_INSTANT,
    /* 12 */ TRANS_TYPE_FILL_BROWN,
    /* 13 */ TRANS_TYPE_FADE_WHITE_CS_DELAYED,
    /* 14 */ TRANS_TYPE_SANDSTORM_PERSIST,
    /* 15 */ TRANS_TYPE_SANDSTORM_END,
    /* 16 */ TRANS_TYPE_CS_BLACK_FILL,
    /* 17 */ TRANS_TYPE_FADE_WHITE_INSTANT,
    /* 18 */ TRANS_TYPE_FADE_GREEN,
    /* 19 */ TRANS_TYPE_FADE_BLUE,
    // transition types 20 - 31 are unused
    // transition types 32 - 55 are constructed using the TRANS_TYPE_CIRCLE macro
    /* 56 */ TRANS_TYPE_MAX = 56
} TransitionType;

#define TRANS_NEXT_TYPE_DEFAULT 0xFF // when `nextTransitionType` is set to default, the type will be taken from the entrance table for the ending transition

typedef enum {
    /* 0 */ TCA_NORMAL,
    /* 1 */ TCA_WAVE,
    /* 2 */ TCA_RIPPLE,
    /* 3 */ TCA_STARBURST
} TransitionCircleAppearance;

typedef enum {
    /* 0 */ TCC_BLACK,
    /* 1 */ TCC_WHITE,
    /* 2 */ TCC_GRAY,
    /* 3 */ TCC_SPECIAL // color varies depending on appearance. unused and appears broken
} TransitionCircleColor;

typedef enum {
    /* 0 */ TCS_FAST,
    /* 1 */ TCS_SLOW
} TransitionCircleSpeed;

#define TC_SET_PARAMS (1 << 7)

#define TRANS_TYPE_CIRCLE(appearance, color, speed) ((1 << 5) | ((color & 3) << 3) | ((appearance & 3) << 1) | (speed & 1))

typedef struct {
    union {
        TransitionFade fade;
        TransitionCircle circle;
        TransitionTriforce triforce;
        TransitionWipe wipe;
    } instanceData;
    /* 0x228 */ s32   transitionType;
    /* 0x22C */ void* (*init)(void* transition);
    /* 0x230 */ void  (*destroy)(void* transition);
    /* 0x234 */ void  (*update)(void* transition, s32 updateRate);
    /* 0x238 */ void  (*draw)(void* transition, Gfx** gfxP);
    /* 0x23C */ void  (*start)(void* transition);
    /* 0x240 */ void  (*setType)(void* transition, s32 type);
    /* 0x244 */ void  (*setColor)(void* transition, u32 color);
    /* 0x248 */ void  (*setUnkColor)(void* transition, u32 color);
    /* 0x24C */ s32   (*isDone)(void* transition);
} TransitionContext; // size = 0x250

typedef struct {
    /* 0x00 */ u8* readBuff;
} SramContext; // size = 0x4

#define SRAM_SIZE 0x8000
#define SRAM_HEADER_SIZE 0x10

typedef enum {
    /* 0x00 */ SRAM_HEADER_SOUND,
    /* 0x01 */ SRAM_HEADER_ZTARGET,
    /* 0x02 */ SRAM_HEADER_LANGUAGE,
    /* 0x03 */ SRAM_HEADER_MAGIC // must be the value of `sZeldaMagic` for save to be considered valid
} SramHeaderField;

typedef struct GameAllocEntry {
    /* 0x00 */ struct GameAllocEntry* next;
    /* 0x04 */ struct GameAllocEntry* prev;
    /* 0x08 */ u32 size;
    /* 0x0C */ u32 unk_0C;
} GameAllocEntry; // size = 0x10

typedef struct {
    /* 0x00 */ GameAllocEntry base;
    /* 0x10 */ GameAllocEntry* head;
} GameAlloc; // size = 0x14

struct GameState;

typedef void (*GameStateFunc)(struct GameState* gameState);

typedef struct GameState {
    /* 0x00 */ GraphicsContext* gfxCtx;
    /* 0x04 */ GameStateFunc main;
    /* 0x08 */ GameStateFunc destroy; // "cleanup"
    /* 0x0C */ GameStateFunc init;
    /* 0x10 */ u32 size;
    /* 0x14 */ Input input[MAXCONTROLLERS];
    /* 0x74 */ TwoHeadArena tha;
    /* 0x84 */ GameAlloc alloc;
    /* 0x98 */ u32 running;
    /* 0x9C */ u32 frames;
    /* 0xA0 */ u32 unk_A0;
} GameState; // size = 0xA4

typedef struct {
    /* 0x00 */ GameState state;
} SetupState; // size = 0xA4

typedef struct {
    /* 0x0000 */ GameState state;
    /* 0x00A4 */ u8* staticSegment;
    /* 0x00A8 */ View view;
    /* 0x01D0 */ SramContext sramCtx;
    /* 0x01D4 */ u16 unk_1D4; // not used in mq dbg (some sort of timer that doesn't seem to affect anything)
    /* 0x01D6 */ s16 coverAlpha;
    /* 0x01D8 */ s16 addAlpha; // not used in mq dbg
    /* 0x01DA */ u16 visibleDuration; // not used in mq dbg
    /* 0x01DC */ s16 ult;
    /* 0x01DE */ s16 uls;
    /* 0x01E0 */ char unk_1E0[0x01];
    /* 0x01E1 */ u8 exit;
    /* 0x01E2 */ char unk_1E2[0x06];
} ConsoleLogoState; // size = 0x1E8

struct MapSelectState;

typedef struct {
    /* 0x00 */ char* name;
    /* 0x04 */ void (*loadFunc)(struct MapSelectState*, s32);
    /* 0x08 */ s32 entranceIndex;
} SceneSelectEntry; // size = 0xC

typedef struct MapSelectState {
    /* 0x0000 */ GameState state;
    /* 0x00A8 */ View view;
    /* 0x01D0 */ s32 count;
    /* 0x01D4 */ SceneSelectEntry* scenes;
    /* 0x01D8 */ s32 currentScene;
    /* 0x01DC */ s32 pageDownIndex; // Index of pageDownStops
    /* 0x01E0 */ s32 pageDownStops[7];
    /* 0x01FC */ char unk_1FC[0x0C];
    /* 0x0208 */ s32 opt;
    /* 0x020C */ s32 topDisplayedScene; // The scene which is currently at the top of the screen
    /* 0x0210 */ char unk_210[0x0C];
    /* 0x021C */ s32 verticalInputAccumulator;
    /* 0x0220 */ s32 verticalInput;
    /* 0x0224 */ s32 timerUp;
    /* 0x0228 */ s32 timerDown;
    /* 0x022C */ s32 lockUp;
    /* 0x0230 */ s32 lockDown;
    /* 0x0234 */ s32 unk_234; // unused
    /* 0x0238 */ u8* staticSegment;
} MapSelectState; // size = 0x240

typedef struct {
    /* 0x0000 */ GameState state;
    /* 0x00A4 */ u8* staticSegment;
    /* 0x00A8 */ View view;
} SampleState; // size = 0x1D0

typedef struct {
    /* 0x00 */ u8 byte0;
    /* 0x01 */ u8 byte1;
    /* 0x02 */ u8 byte2;
    /* 0x03 */ u8 byte3;
} ElfMessage; // size = 0x4

typedef struct {
    /* 0x00 */ u8 numActors;
    /* 0x04 */ TransitionActorEntry* list;
} TransitionActorContext;

typedef struct PlayState {
    /* 0x00000 */ GameState state;
    /* 0x000A4 */ s16 sceneId;
    /* 0x000A6 */ u8 sceneDrawConfig;
    /* 0x000A7 */ char unk_A7[0x9];
    /* 0x000B0 */ void* sceneSegment;
    /* 0x000B8 */ View view;
    /* 0x001E0 */ Camera mainCamera;
    /* 0x0034C */ Camera subCameras[NUM_CAMS - CAM_ID_SUB_FIRST];
    /* 0x00790 */ Camera* cameraPtrs[NUM_CAMS];
    /* 0x007A0 */ s16 activeCamId;
    /* 0x007A2 */ s16 nextCamId;
    /* 0x007A4 */ SequenceContext sequenceCtx;
    /* 0x007A8 */ LightContext lightCtx;
    /* 0x007B8 */ FrameAdvanceContext frameAdvCtx;
    /* 0x007C0 */ CollisionContext colCtx;
    /* 0x01C24 */ ActorContext actorCtx;
    /* 0x01D64 */ CutsceneContext csCtx; // "demo_play"
    /* 0x01DB4 */ SfxSource sfxSources[16];
    /* 0x01F74 */ SramContext sramCtx;
    /* 0x01F78 */ SkyboxContext skyboxCtx;
    /* 0x020D8 */ MessageContext msgCtx; // "message"
    /* 0x104F0 */ InterfaceContext interfaceCtx; // "parameter"
    /* 0x10760 */ PauseContext pauseCtx;
    /* 0x10A20 */ GameOverContext gameOverCtx;
    /* 0x10A24 */ EnvironmentContext envCtx;
    /* 0x10B20 */ AnimationContext animationCtx;
    /* 0x117A4 */ ObjectContext objectCtx;
    /* 0x11CBC */ RoomContext roomCtx;
    /* 0x11D34 */ TransitionActorContext transiActorCtx;
    /* 0x11D3C */ void (*playerInit)(Player* player, struct PlayState* play, FlexSkeletonHeader* skelHeader);
    /* 0x11D40 */ void (*playerUpdate)(Player* player, struct PlayState* play, Input* input);
    /* 0x11D44 */ s32 (*isPlayerDroppingFish)(struct PlayState* play);
    /* 0x11D48 */ s32 (*startPlayerFishing)(struct PlayState* play);
    /* 0x11D4C */ s32 (*grabPlayer)(struct PlayState* play, Player* player);
    /* 0x11D50 */ s32 (*startPlayerCutscene)(struct PlayState* play, Actor* actor, s32 mode);
    /* 0x11D54 */ void (*func_11D54)(Player* player, struct PlayState* play);
    /* 0x11D58 */ s32 (*damagePlayer)(struct PlayState* play, s32 damage);
    /* 0x11D5C */ void (*talkWithPlayer)(struct PlayState* play, Actor* actor);
    /* 0x11D60 */ MtxF viewProjectionMtxF;
    /* 0x11DA0 */ MtxF billboardMtxF;
    /* 0x11DE0 */ Mtx* billboardMtx;
    /* 0x11DE4 */ u32 gameplayFrames;
    /* 0x11DE8 */ u8 linkAgeOnLoad;
    /* 0x11DE9 */ u8 unk_11DE9;
    /* 0x11DEA */ u8 curSpawn;
    /* 0x11DEB */ u8 numSetupActors;
    /* 0x11DEC */ u8 numRooms;
    /* 0x11DF0 */ RomFile* roomList;
    /* 0x11DF4 */ ActorEntry* linkActorEntry;
    /* 0x11DF8 */ ActorEntry* setupActorList;
    /* 0x11DFC */ void* unk_11DFC;
    /* 0x11E00 */ EntranceEntry* setupEntranceList;
    /* 0x11E04 */ s16* setupExitList;
    /* 0x11E08 */ Path* setupPathList;
    /* 0x11E0C */ ElfMessage* cUpElfMsgs;
    /* 0x11E10 */ void* specialEffects;
    /* 0x11E14 */ u8 skyboxId;
    /* 0x11E15 */ s8 transitionTrigger; // "fade_direction"
    /* 0x11E16 */ s16 unk_11E16;
    /* 0x11E18 */ s16 unk_11E18;
    /* 0x11E1A */ s16 nextEntranceIndex;
    /* 0x11E1C */ char unk_11E1C[0x40];
    /* 0x11E5C */ s8 shootingGalleryStatus;
    /* 0x11E5D */ s8 bombchuBowlingStatus; // "bombchu_game_flag"
    /* 0x11E5E */ u8 transitionType;
    /* 0x11E60 */ CollisionCheckContext colChkCtx;
    /* 0x120FC */ u16 envFlags[20];
    /* 0x12124 */ PreRender pauseBgPreRender;
    /* 0x12174 */ char unk_12174[0x53];
    /* 0x121C7 */ s8 unk_121C7;
    /* 0x121C8 */ TransitionContext transitionCtx;
    /* 0x12418 */ char unk_12418[0x3];
    /* 0x1241B */ u8 transitionMode; // "fbdemo_wipe_modem"
    /* 0x1241C */ TransitionFade transitionFade;
    /* 0x12428 */ char unk_12428[0x3];
    /* 0x1242B */ u8 viewpoint; // toggleable camera setting by shops or player. Is also equal to the bgCamIndex + 1
    /* 0x1242C */ SceneTableEntry* loadedScene;
    /* 0x12430 */ char unk_12430[0xE8];
} PlayState; // size = 0x12518

typedef struct {
    /* 0x0000 */ GameState state;
    /* 0x00A8 */ View view;
} TitleSetupState; // size = 0x1D0

typedef struct {
    /* 0x00000 */ GameState state;
    /* 0x000A4 */ Vtx* windowVtx;
    /* 0x000A8 */ u8* staticSegment;
    /* 0x000AC */ u8* parameterSegment;
    /* 0x000B0 */ char unk_B0[0x8];
    /* 0x000B8 */ View view;
    /* 0x001E0 */ SramContext sramCtx;
    /* 0x001E4 */ char unk_1E4[0x4];
    /* 0x001E8 */ SkyboxContext skyboxCtx;
    /* 0x00348 */ MessageContext msgCtx;
    /* 0x0E760 */ Font font;
    /* 0x1C8E8 */ EnvironmentContext envCtx;
    /* 0x1C9E4 */ char unk_1C9E4[0x4];
    /* 0x1C9E8 */ Vtx* windowContentVtx;
    /* 0x1C9EC */ Vtx* keyboardVtx;
    /* 0x1C9F0 */ Vtx* nameEntryVtx;
    /* 0x1C9F4 */ u8 n64ddFlag;
    /* 0x1C9F6 */ u16 deaths[3];
    /* 0x1C9FC */ u8 fileNames[3][8];
    /* 0x1CA14 */ u16 healthCapacities[3];
    /* 0x1CA1C */ u32 questItems[3];
    /* 0x1CA28 */ s16 n64ddFlags[3];
    /* 0x1CA2E */ s8 defense[3];
    /* 0x1CA32 */ u16 health[3];
    /* 0x1CA38 */ s16 buttonIndex;
    /* 0x1CA3A */ s16 confirmButtonIndex; // 0: yes, 1: quit
    /* 0x1CA3C */ s16 menuMode;
    /* 0x1CA3E */ s16 configMode;
    /* 0x1CA40 */ s16 prevConfigMode;
    /* 0x1CA42 */ s16 nextConfigMode;
    /* 0x1CA44 */ s16 selectMode;
    /* 0x1CA46 */ s16 selectedFileIndex;
    /* 0x1CA48 */ char unk_1CA48[0x2];
    /* 0x1CA4A */ s16 fileNamesY[3];
    /* 0x1CA50 */ s16 actionTimer;
    /* 0x1CA52 */ s16 buttonYOffsets[6];
    /* 0x1CA5E */ s16 copyDestFileIndex;
    /* 0x1CA60 */ s16 warningLabel;
    /* 0x1CA62 */ s16 warningButtonIndex;
    /* 0x1CA64 */ s16 titleLabel;
    /* 0x1CA66 */ s16 nextTitleLabel;
    /* 0x1CA68 */ s16 windowColor[3];
    /* 0x1CA6E */ s16 titleAlpha[2];
    /* 0x1CA72 */ s16 windowAlpha;
    /* 0x1CA74 */ s16 fileButtonAlpha[3];
    /* 0x1CA7A */ s16 nameBoxAlpha[3];
    /* 0x1CA80 */ s16 nameAlpha[3];
    /* 0x1CA86 */ s16 connectorAlpha[3];
    /* 0x1CA8C */ s16 fileInfoAlpha[3];
    /* 0x1CA92 */ s16 actionButtonAlpha[2];
    /* 0x1CA96 */ s16 confirmButtonAlpha[2];
    /* 0x1CA9A */ s16 optionButtonAlpha;
    /* 0x1CA9C */ s16 nameEntryBoxAlpha;
    /* 0x1CA9E */ s16 controlsAlpha;
    /* 0x1CAA0 */ s16 emptyFileTextAlpha;
    /* 0x1CAA2 */ s16 highlightColor[4];
    /* 0x1CAAA */ s16 highlightPulseDir; // 0 fade out, 1 fade in
    /* 0x1CAAC */ s16 unk_1CAAC; // initialized but never used
    /* 0x1CAAE */ s16 confirmButtonTexIndices[2];
    /* 0x1CAB2 */ s16 inputTimerX;
    /* 0x1CAB4 */ s16 inputTimerY;
    /* 0x1CAB6 */ s16 stickXDir;
    /* 0x1CAB8 */ s16 stickYDir;
    /* 0x1CABA */ s16 stickAdjX;
    /* 0x1CABC */ s16 stickAdjY;
    /* 0x1CABE */ s16 nameEntryBoxPosX;
    /* 0x1CAC0 */ s16 windowPosX;
    /* 0x1CAC4 */ f32 windowRot;
    /* 0x1CAC8 */ s16 kbdButton; // only for buttons, not characters
    /* 0x1CACA */ s16 charPage; // 0: hiragana, 1: katakana, 2: alphabet
    /* 0x1CACC */ s16 charBgAlpha; // square shape the letter sits in
    /* 0x1CACE */ s16 charIndex; // 0 - 64, top left to bottom right
    /* 0x1CAD0 */ s16 kbdX; // (0, 0) is top left character
    /* 0x1CAD2 */ s16 kbdY;
    /* 0x1CAD4 */ s16 newFileNameCharCount;
    /* 0x1CAD6 */ s16 unk_1CAD6[5];
} FileSelectState; // size = 0x1CAE0

typedef enum {
    DPM_UNK = 0,
    DPM_PLAYER = 1,
    DPM_ENEMY = 2,
    DPM_UNK3 = 3
} DynaPolyMoveFlag;

typedef struct {
    /* 0x00 */ AnimationHeader* animation;
    /* 0x04 */ f32              playSpeed;
    /* 0x08 */ f32              startFrame;
    /* 0x0C */ f32              frameCount;
    /* 0x10 */ u8               mode;
    /* 0x14 */ f32              morphFrames;
} AnimationInfo; // size = 0x18

typedef struct {
    /* 0x00 */ AnimationHeader* animation;
    /* 0x04 */ f32              frameCount;
    /* 0x08 */ u8               mode;
    /* 0x0C */ f32              morphFrames;
} AnimationFrameCountInfo; // size = 0x10

typedef struct {
    /* 0x00 */ AnimationHeader* animation;
    /* 0x04 */ f32 playSpeed;
    /* 0x08 */ u8 mode;
    /* 0x0C */ f32 morphFrames;
} AnimationSpeedInfo; // size = 0x10

typedef struct {
    /* 0x00 */ AnimationHeader* animation;
    /* 0x04 */ u8 mode;
    /* 0x08 */ f32 morphFrames;
} AnimationMinimalInfo; // size = 0xC

typedef struct {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ Vec3s unk_08;
    /* 0x0E */ Vec3s unk_0E;
    /* 0x14 */ f32 unk_14;
    /* 0x18 */ Vec3f unk_18;
    /* 0x24 */ s16 unk_24;
} struct_80034A14_arg1; // size = 0x28

// Macros for `EntranceInfo.field`
#define ENTRANCE_INFO_CONTINUE_BGM_FLAG (1 << 15)
#define ENTRANCE_INFO_DISPLAY_TITLE_CARD_FLAG (1 << 14)
#define ENTRANCE_INFO_END_TRANS_TYPE_MASK 0x3F80
#define ENTRANCE_INFO_END_TRANS_TYPE_SHIFT 7
#define ENTRANCE_INFO_END_TRANS_TYPE(field)          \
    (((field) >> ENTRANCE_INFO_END_TRANS_TYPE_SHIFT) \
     & (ENTRANCE_INFO_END_TRANS_TYPE_MASK >> ENTRANCE_INFO_END_TRANS_TYPE_SHIFT))
#define ENTRANCE_INFO_START_TRANS_TYPE_MASK 0x7F
#define ENTRANCE_INFO_START_TRANS_TYPE_SHIFT 0
#define ENTRANCE_INFO_START_TRANS_TYPE(field)          \
    (((field) >> ENTRANCE_INFO_START_TRANS_TYPE_SHIFT) \
     & (ENTRANCE_INFO_START_TRANS_TYPE_MASK >> ENTRANCE_INFO_START_TRANS_TYPE_SHIFT))

typedef struct {
    /* 0x00 */ s8  sceneId;
    /* 0x01 */ s8  spawn;
    /* 0x02 */ u16 field;
} EntranceInfo; // size = 0x4

typedef struct {
    /* 0x00 */ void*     loadedRamAddr;
    /* 0x04 */ uintptr_t vromStart; // if applicable
    /* 0x08 */ uintptr_t vromEnd;   // if applicable
    /* 0x0C */ void*     vramStart; // if applicable
    /* 0x10 */ void*     vramEnd;   // if applicable
    /* 0x14 */ void*     unk_14;
    /* 0x18 */ void*     init;    // initializes and executes the given context
    /* 0x1C */ void*     destroy; // deconstructs the context, and sets the next context to load
    /* 0x20 */ void*     unk_20;
    /* 0x24 */ void*     unk_24;
    /* 0x28 */ UNK_TYPE4 unk_28;
    /* 0x2C */ u32       instanceSize;
} GameStateOverlay; // size = 0x30

typedef struct {
    /* 0x00 */ GameState state;
    /* 0xA4 */ u32       timer;
    /* 0xA8 */ UNK_TYPE4 unk_A8;
} PreNMIState; // size = 0xAC

typedef enum {
    /*  1 */ F_8F = 1,
    /*  2 */ F_7F,
    /*  3 */ F_6F,
    /*  4 */ F_5F,
    /*  5 */ F_4F,
    /*  6 */ F_3F,
    /*  7 */ F_2F,
    /*  8 */ F_1F,
    /*  9 */ F_B1,
    /* 10 */ F_B2,
    /* 11 */ F_B3,
    /* 12 */ F_B4,
    /* 13 */ F_B5,
    /* 14 */ F_B6,
    /* 15 */ F_B7,
    /* 16 */ F_B8
} FloorID;

// All arrays pointed in this struct are indexed by "map indices"
// In dungeons, the map index corresponds to the dungeon index (which also indexes keys, items, etc)
// In overworld areas, the map index corresponds to the overworld area index (spot 00, 01, etc)
typedef struct {
    /* 0x00 */ s16 (*floorTexIndexOffset)[8]; // dungeon texture index offset by floor
    /* 0x04 */ s16*  bossFloor; // floor the boss is on
    /* 0x08 */ s16 (*roomPalette)[32]; // map palette by room
    /* 0x0C */ s16*  maxPaletteCount; // max number of palettes in a same floor
    /* 0x10 */ s16 (*paletteRoom)[8][14]; // room by palette by floor
    /* 0x14 */ s16 (*roomCompassOffsetX)[44]; // dungeon compass icon X offset by room
    /* 0x18 */ s16 (*roomCompassOffsetY)[44]; // dungeon compass icon Y offset by room
    /* 0x1C */ u8*   dgnMinimapCount; // number of room minimaps
    /* 0x20 */ u16*  dgnMinimapTexIndexOffset; // dungeon minimap texture index offset
    /* 0x24 */ u16*  owMinimapTexSize;
    /* 0x28 */ u16*  owMinimapTexOffset;
    /* 0x2C */ s16*  owMinimapPosX;
    /* 0x30 */ s16*  owMinimapPosY;
    /* 0x34 */ s16 (*owCompassInfo)[4]; // [X scale, Y scale, X offset, Y offset]
    /* 0x38 */ s16*  dgnTexIndexBase; // dungeon texture index base
    /* 0x3C */ s16 (*dgnCompassInfo)[4]; // [X scale, Y scale, X offset, Y offset]
    /* 0x40 */ s16*  owMinimapWidth;
    /* 0x44 */ s16*  owMinimapHeight;
    /* 0x48 */ s16*  owEntranceIconPosX; // "dungeon entrance" icon X pos
    /* 0x4C */ s16*  owEntranceIconPosY; // "dungeon entrance" icon Y pos
    /* 0x50 */ u16*  owEntranceFlag; // flag in inf_table[26] based on which entrance icons are shown (0xFFFF = always shown)
    /* 0x54 */ f32 (*floorCoordY)[8]; // Y coordinate of each floor
    /* 0x58 */ u16*  switchEntryCount; // number of "room switch" entries, which correspond to the next 3 arrays
    /* 0x5C */ u8  (*switchFromRoom)[51]; // room to come from
    /* 0x60 */ u8  (*switchFromFloor)[51]; // floor to come from
    /* 0x64 */ u8  (*switchToRoom)[51]; // room to go to
    /* 0x68 */ u8  (*floorID)[8];
    /* 0x6C */ s16* skullFloorIconY; // dungeon big skull icon Y pos
} MapData; // size = 0x70

#define PAUSE_MAP_MARK_NONE -1
#define PAUSE_MAP_MARK_CHEST 0
#define PAUSE_MAP_MARK_BOSS 1

typedef struct {
    /* 0x00 */ s16 chestFlag; // chest icon is only displayed if this flag is not set for the current room, -1 for no flag
    /* 0x04 */ f32 x, y; // coordinates to place the icon (top-left corner)
} PauseMapMarkPoint; // size = 0x0C

typedef struct {
    /* 0x00 */ s16 markType; // 0 for the chest icon, 1 for the boss skull icon, -1 for none
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ const Vtx* vtx;
    /* 0x0C */ s32 vtxCount;
    /* 0x10 */ s32 count; // number of icons to display
    /* 0x14 */ PauseMapMarkPoint points[12];
} PauseMapMarkData; // size = 0xA4

typedef PauseMapMarkData PauseMapMarksData[3];

typedef struct DebugDispObject {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ Vec3s rot;
    /* 0x14 */ Vec3f scale;
    /* 0x20 */ Color_RGBA8 color;
    /* 0x24 */ s16   type;
    /* 0x28 */ struct DebugDispObject* next;
} DebugDispObject; // size = 0x2C

typedef enum {
    MTXMODE_NEW,  // generates a new matrix
    MTXMODE_APPLY // applies transformation to the current matrix
} MatrixMode;

typedef struct {
    /* 0x00 */ PrintCallback callback;
    /* 0x04 */ Gfx* dList;
    /* 0x08 */ u16 posX;
    /* 0x0A */ u16 posY;
    /* 0x0C */ u16 baseX;
    /* 0x0E */ u8 baseY;
    /* 0x0F */ u8 flags;
    /* 0x10 */ Color_RGBA8_u32 color;
    /* 0x14 */ char unk_14[0x1C]; // unused
} GfxPrint; // size = 0x30

#define GFX_CHAR_X_SPACING    8
#define GFX_CHAR_Y_SPACING    8

#define GFXP_UNUSED "\x8E"
#define GFXP_UNUSED_CHAR 0x8E
#define GFXP_HIRAGANA "\x8D"
#define GFXP_HIRAGANA_CHAR 0x8D
#define GFXP_KATAKANA "\x8C"
#define GFXP_KATAKANA_CHAR 0x8C
#define GFXP_RAINBOW_ON "\x8B"
#define GFXP_RAINBOW_ON_CHAR 0x8B
#define GFXP_RAINBOW_OFF "\x8A"
#define GFXP_RAINBOW_OFF_CHAR 0x8A

#define GFXP_FLAG_HIRAGANA (1 << 0)
#define GFXP_FLAG_RAINBOW  (1 << 1)
#define GFXP_FLAG_SHADOW   (1 << 2)
#define GFXP_FLAG_UPDATE   (1 << 3)
#define GFXP_FLAG_ENLARGE  (1 << 6)
#define GFXP_FLAG_OPEN     (1 << 7)

typedef struct StackEntry {
    /* 0x00 */ struct StackEntry* next;
    /* 0x04 */ struct StackEntry* prev;
    /* 0x08 */ u32* head;
    /* 0x0C */ u32* tail;
    /* 0x10 */ u32 initValue;
    /* 0x14 */ s32 minSpace;
    /* 0x18 */ const char* name;
} StackEntry;

typedef enum {
    STACK_STATUS_OK = 0,
    STACK_STATUS_WARNING = 1,
    STACK_STATUS_OVERFLOW = 2
} StackStatus;

typedef struct {
    /* 0x00 */ u32 magic; // IS64
    /* 0x04 */ u32 get;
    /* 0x08 */ u8 unk_08[0x14-0x08];
    /* 0x14 */ u32 put;
    /* 0x18 */ u8 unk_18[0x20-0x18];
    /* 0x20 */ u8 data[0x10000-0x20];
} ISVDbg;

typedef struct {
    /* 0x00 */ char name[0x18];
    /* 0x18 */ u32 mediaFormat;
    /* 0x1C */ union {
        struct {
            u16 cartId;
            u8 countryCode;
            u8 version;
        };
        u32 regionInfo;
    };
} LocaleCartInfo; // size = 0x20

typedef struct {
    /* 0x00 */ char magic[4]; // Yaz0
    /* 0x04 */ u32 decSize;
    /* 0x08 */ u32 compInfoOffset; // only used in mio0
    /* 0x0C */ u32 uncompDataOffset; // only used in mio0
    /* 0x10 */ u8 data[1];
} Yaz0Header; // size = 0x10 ("data" is not part of the header)

typedef struct {
    /* 0x0000 */ IrqMgr*     irqMgr;
    /* 0x0004 */ Scheduler*  sched;
    /* 0x0008 */ OSScTask    audioTask;
    /* 0x0070 */ AudioTask*  rspTask;
    /* 0x0074 */ OSMesgQueue interruptQueue;
    /* 0x008C */ OSMesg      interruptMsgBuf[8];
    /* 0x00AC */ OSMesgQueue taskQueue;
    /* 0x00C4 */ OSMesg      taskMsgBuf[1];
    /* 0x00C8 */ OSMesgQueue lockQueue;
    /* 0x00E0 */ OSMesg      lockMsgBuf[1];
    /* 0x00E8 */ OSThread    thread;
} AudioMgr; // size = 0x298

struct ArenaNode;

typedef struct Arena {
    /* 0x00 */ struct ArenaNode* head;
    /* 0x04 */ void* start;
    /* 0x08 */ OSMesgQueue lockQueue;
    /* 0x20 */ u8 unk_20;
    /* 0x21 */ u8 isInit;
    /* 0x22 */ u8 flag;
} Arena; // size = 0x24

typedef struct ArenaNode {
    /* 0x00 */ s16 magic;
    /* 0x02 */ s16 isFree;
    /* 0x04 */ u32 size;
    /* 0x08 */ struct ArenaNode* next;
    /* 0x0C */ struct ArenaNode* prev;
    /* 0x10 */ const char* filename;
    /* 0x14 */ s32 line;
    /* 0x18 */ OSId threadId;
    /* 0x1C */ Arena* arena;
    /* 0x20 */ OSTime time;
    /* 0x28 */ u8 unk_28[0x30-0x28]; // probably padding
} ArenaNode; // size = 0x30

#define RELOC_SECTION(reloc) ((reloc) >> 30)
#define RELOC_OFFSET(reloc) ((reloc) & 0xFFFFFF)
#define RELOC_TYPE_MASK(reloc) ((reloc) & 0x3F000000)
#define RELOC_TYPE_SHIFT 24

/* MIPS Relocation Types */
#define R_MIPS_32 2
#define R_MIPS_26 4
#define R_MIPS_HI16 5
#define R_MIPS_LO16 6

typedef struct OverlayRelocationSection {
    /* 0x00 */ u32 textSize;
    /* 0x04 */ u32 dataSize;
    /* 0x08 */ u32 rodataSize;
    /* 0x0C */ u32 bssSize;
    /* 0x10 */ u32 nRelocations;
    /* 0x14 */ u32 relocations[1];
} OverlayRelocationSection; // size >= 0x18

typedef struct {
    /* 0x00 */ u32 resetting;
    /* 0x04 */ u32 resetCount;
    /* 0x08 */ OSTime duration;
    /* 0x10 */ OSTime resetTime;
} PreNmiBuff; // size = 0x18 (actually osAppNMIBuffer is 0x40 bytes large but the rest is unused)

typedef struct {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 unk_04;
} SubQuakeRequest14;

typedef struct {
    /* 0x00 */ s16 randIdx;
    /* 0x02 */ s16 countdownMax;
    /* 0x04 */ Camera* cam;
    /* 0x08 */ u32 callbackIdx;
    /* 0x0C */ s16 y;
    /* 0x0E */ s16 x;
    /* 0x10 */ s16 zoom;
    /* 0x12 */ s16 rotZ;
    /* 0x14 */ SubQuakeRequest14 unk_14;
    /* 0x1A */ s16 speed;
    /* 0x1C */ s16 unk_1C;
    /* 0x1E */ s16 countdown;
    /* 0x20 */ s16 camPtrIdx;
} QuakeRequest; // size = 0x24

typedef struct {
    /* 0x00 */ Vec3f vec1;
    /* 0x0C */ Vec3f vec2;
    /* 0x18 */ s16 rotZ;
    /* 0x1A */ s16 unk_1A;
    /* 0x1C */ s16 zoom;
} ShakeInfo; // size = 0x1E

typedef struct {
    /* 0x00 */ Vec3f atOffset;
    /* 0x0C */ Vec3f eyeOffset;
    /* 0x18 */ s16 rotZ;
    /* 0x1A */ s16 unk_1A;
    /* 0x1C */ s16 zoom;
    /* 0x20 */ f32 unk_20;
} QuakeCamCalc; // size = 0x24


#define UCODE_NULL      0
#define UCODE_F3DZEX    1
#define UCODE_UNK       2
#define UCODE_S2DEX     3

typedef struct {
    /* 0x00 */ u32 type;
    /* 0x04 */ void* ptr;
} UCodeInfo; // size = 0x8

typedef struct {
    /* 0x00 */ u32 segments[NUM_SEGMENTS];
    /* 0x40 */ Gfx* dlStack[18];
    /* 0x88 */ s32 dlDepth;
    /* 0x8C */ u32 dlCnt;
    /* 0x90 */ u32 vtxCnt;
    /* 0x94 */ u32 spvtxCnt;
    /* 0x98 */ u32 tri1Cnt;
    /* 0x9C */ u32 tri2Cnt;
    /* 0xA0 */ u32 quadCnt;
    /* 0xA4 */ u32 lineCnt;
    /* 0xA8 */ u32 loaducodeCnt;
    /* 0xAC */ u32 pipeSyncRequired;
    /* 0xB0 */ u32 tileSyncRequired;
    /* 0xB4 */ u32 loadSyncRequired;
    /* 0xB8 */ u32 syncErr;
    /* 0xBC */ s32 enableLog;
    /* 0xC0 */ s32 ucodeType;
    /* 0xC4 */ s32 ucodeInfoCount;
    /* 0xC8 */ UCodeInfo* ucodeInfo;
    /* 0xCC */ u32 modeH;
    /* 0xD0 */ u32 modeL;
    /* 0xD4 */ u32 geometryMode;
} UCodeDisas; // size = 0xD8

typedef struct {
    /* 0x00 */ u16 table[8*8];
} JpegQuantizationTable; // size = 0x80

typedef struct {
    /* 0x00 */ u8 codeOffs[16];
    /* 0x10 */ u16 codesA[16];
    /* 0x30 */ u16 codesB[16];
    /* 0x50 */ u8* symbols;
} JpegHuffmanTable; // size = 0x54

// this struct might be inaccurate but it's not used outside jpegutils.c anyways
typedef struct {
    /* 0x000 */ u8 codeOffs[16];
    /* 0x010 */ u16 dcCodes[120];
    /* 0x100 */ u16 acCodes[256];
} JpegHuffmanTableOld; // size = 0x300

typedef union {
    struct {
    /* 0x00 */ u32 address;
    /* 0x04 */ u32 mbCount;
    /* 0x08 */ u32 mode;
    /* 0x0C */ u32 qTableYPtr;
    /* 0x10 */ u32 qTableUPtr;
    /* 0x14 */ u32 qTableVPtr;
    /* 0x18 */ u32 mbSize; // This field is used by the microcode to save the macroblock size during a yield
    };
    long long int force_structure_alignment;
} JpegTaskData; // size = 0x20

typedef struct {
    /* 0x000 */ JpegTaskData taskData;
    /* 0x020 */ u64 yieldData[0x200 / sizeof(u64)];
    /* 0x220 */ JpegQuantizationTable qTableY;
    /* 0x2A0 */ JpegQuantizationTable qTableU;
    /* 0x320 */ JpegQuantizationTable qTableV;
    /* 0x3A0 */ u8 codesLengths[0x110];
    /* 0x4B0 */ u16 codes[0x108];
    /* 0x6C0 */ u16 data[4][0x180];
} JpegWork; // size = 0x12C0

typedef struct {
    /* 0x00 */ void* imageData;
    /* 0x04 */ u8 mode;
    /* 0x05 */ u8 unk_05;
    /* 0x08 */ JpegHuffmanTable* hTablePtrs[4];
    /* 0x18 */ u8 unk_18;
} JpegDecoder; // size = 0x1C

typedef struct {
    /* 0x00 */ u8 dqtCount;
    /* 0x04 */ u8* dqtPtr[3];
    /* 0x10 */ u8 dhtCount;
    /* 0x14 */ u8* dhtPtr[4];
    /* 0x24 */ void* imageData;
    /* 0x28 */ u32 mode; // 0 if Y V0 is 1 and 2 if Y V0 is 2
    /* 0x30 */ OSScTask scTask;
    /* 0x98 */ OSMesgQueue mq;
    /* 0xB0 */ OSMesg msg;
    /* 0xB4 */ JpegWork* workBuf;
} JpegContext; // size = 0xB8

typedef struct {
    /* 0x00 */ u32 byteIdx;
    /* 0x04 */ u8 bitIdx;
    /* 0x05 */ u8 dontSkip;
    /* 0x08 */ u32 curWord;
    /* 0x0C */ s16 unk_0C;
    /* 0x0E */ s16 unk_0E;
    /* 0x10 */ s16 unk_10;
} JpegDecoderState; // size = 0x14

typedef struct {
    /* 0x0000 */ OSViMode customViMode;
    /* 0x0050 */ s32 viHeight;
    /* 0x0054 */ s32 viWidth;
    /* 0x0058 */ s32 unk_58; // Right adjustment?
    /* 0x005C */ s32 unk_5C; // Left adjustment?
    /* 0x0060 */ s32 unk_60; // Bottom adjustment?
    /* 0x0064 */ s32 unk_64; // Top adjustment?
    /* 0x0068 */ s32 viModeBase; // enum: {0, 1, 2, 3}
    /* 0x006C */ s32 viTvType;
    /* 0x0070 */ u32 unk_70; // bool
    /* 0x0074 */ u32 unk_74; // bool
    /* 0x0078 */ u32 unk_78; // bool
    /* 0x007C */ u32 unk_7C; // bool
    /* 0x0080 */ u32 viFeatures;
    /* 0x0084 */ u32 unk_84;
} ViMode;

// Vis...
typedef struct {
    /* 0x00 */ u32 type;
    /* 0x04 */ u32 setScissor;
    /* 0x08 */ Color_RGBA8_u32 color;
    /* 0x0C */ Color_RGBA8_u32 envColor;
} struct_801664F0; // size = 0x10

typedef struct {
    /* 0x00 */ u32 unk_00;
    /* 0x04 */ u32 setScissor;
    /* 0x08 */ Color_RGBA8_u32 primColor;
    /* 0x0C */ Color_RGBA8_u32 envColor;
    /* 0x10 */ u16* tlut;
    /* 0x14 */ Gfx* dList;
} VisMono; // size = 0x18

// Vis...
typedef struct {
    /* 0x00 */ u32 useRgba;
    /* 0x04 */ u32 setScissor;
    /* 0x08 */ Color_RGBA8_u32 primColor;
    /* 0x08 */ Color_RGBA8_u32 envColor;
} struct_80166500; // size = 0x10

typedef struct {
    /* 0x00 */ char unk_00[0x18];
    /* 0x18 */ s32 unk_18;
    /* 0x1C */ s32 y;
} SpeedMeter; // size = 0x20

typedef struct {
    /* 0x00 */ s32 maxval;
    /* 0x04 */ s32 val;
    /* 0x08 */ u16 backColor;
    /* 0x0A */ u16 foreColor;
    /* 0x0C */ s32 ulx;
    /* 0x10 */ s32 lrx;
    /* 0x14 */ s32 uly;
    /* 0x18 */ s32 lry;
} SpeedMeterAllocEntry; // size = 0x1C

typedef struct {
    /* 0x00 */ volatile OSTime* time;
    /* 0x04 */ u8 x;
    /* 0x05 */ u8 y;
    /* 0x06 */ u16 color;
} SpeedMeterTimeEntry; // size = 0x08

typedef struct {
    /* 0x00 */ u32 value;
    /* 0x04 */ const char* name;
} F3dzexConst; // size = 0x8

typedef struct {
    /* 0x00 */ u32 value;
    /* 0x04 */ const char* setName;
    /* 0x08 */ const char* unsetName;
} F3dzexFlag; // size = 0x0C

typedef struct {
    /* 0x00 */ const char* name;
    /* 0x04 */ u32 value;
    /* 0x08 */ u32 mask;
} F3dzexRenderMode; // size = 0x0C

typedef struct {
    /* 0x00 */ const char* name;
    /* 0x04 */ u32 value;
} F3dzexSetModeMacroValue; // size = 0x8

typedef struct {
    /* 0x00 */ const char* name;
    /* 0x04 */ u32 shift;
    /* 0x08 */ u32 len;
    /* 0x0C */ F3dzexSetModeMacroValue values[4];
} F3dzexSetModeMacro; // size = 0x2C

typedef struct {
    /* 0x00 */ u16* value;
    /* 0x04 */ const char* name;
} FlagSetEntry; // size = 0x08

typedef struct {
    /* 0x00 */ RomFile file;
    /* 0x08 */ RomFile palette;
} SkyboxFile; // size = 0x10

#define ROM_FILE(name) \
    { (uintptr_t)_##name##SegmentRomStart, (uintptr_t)_##name##SegmentRomEnd }
#define ROM_FILE_EMPTY(name) \
    { (uintptr_t)_##name##SegmentRomStart, (uintptr_t)_##name##SegmentRomStart }
#define ROM_FILE_UNSET \
    { 0 }

#endif
