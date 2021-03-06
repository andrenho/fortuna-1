#ifndef LIBF1COMM_DEFINES_HH
#define LIBF1COMM_DEFINES_HH

static constexpr uint8_t FinalByte      = 0xe4;
static constexpr uint8_t InvalidCommand = 0xef;

enum MessageClass : uint8_t {
    MC_Request          = 0xf0,
    MC_Reply            = 0xf1,
    MC_DebugInformation = 0xf2,
};

#define MESSAGE_TYPES             \
    X(Nop,           0x00)        \
                                  \
    /* tests */                   \
    X(TestDebug,     0x01)        \
    X(TestDMA,       0x02)        \
                                  \
    /* general */                 \
    X(FreeMem,       0x0a)        \
    X(SoftReset,     0x0b)        \
    X(HardReset,     0x0c)        \
                                  \
    /* RAM */                     \
    X(RamReadByte,   0x10)        \
    X(RamWriteByte,  0x11)        \
    X(RamReadBlock,  0x12)        \
    X(RamWriteBlock, 0x13)        \
                                  \
    /* SDCARD */                  \
    X(SDCard_Status, 0x20)        \
    X(SDCard_Read,   0x21)        \
                                  \
    /* Z80 */                     \
    X(Z80_CpuInfo,   0x30)        \
    X(Z80_Step,      0x31)        \
    X(Z80_NMI,       0x32)        \
    X(Z80_Run,       0x33)        \
    X(Z80_Next,      0x34)        \
    X(Z80_Stop,      0x35)        \
    X(Z80_Events,    0x36)        \
                                  \
    /* Terminal */                \
    X(Keypress,      0x40)        \
                                  \
    /* Breakpoints */             \
    X(BreakpointsList,      0x50) \
    X(BreakpointsAdd,       0x51) \
    X(BreakpointsRemove,    0x52) \
    X(BreakpointsRemoveAll, 0x53) \
                                  \
    X(Undefined,     0xff)

enum MessageType : uint8_t {
#define X(name, value) name = value,
    MESSAGE_TYPES
#undef X
};

enum class DeserializationError {
    NoErrors, InvalidMessageClass, ChecksumDoesNotMatch, FinalByteNotReceived, BufferDataTooLarge,
};

enum Result : uint8_t {
    OK                               = 0,
    InvalidRequest                   = 1,
    WrongChecksumDMA                 = 2,
    DeserializationErrorInController = 3,
    SDCardError                      = 4,
    TooManyBreakpoints               = 5,
};

#define RESET_STATUS           \
    X(Ok,                0xa0) \
    X(SDCardInitError,   0xa1) \
    X(SDCardReadError,   0xa2) \
    X(SDCardNotBootable, 0xa3)

enum ResetStatus : uint8_t {
#define X(name, value) name = value,
    RESET_STATUS
#undef X
};

#define EVENTS \
    X(NoEvents,          0xb0) \
    X(CharPrinted,       0xb1) \
    X(Stopped,           0xb2) \
    X(BreakpointReached, 0xb3)

enum EventType : uint8_t {
#define X(name, value) name = value,
    EVENTS
#undef X
};

#ifndef EMBEDDED
#include <unordered_map>

    inline const std::unordered_map<uint8_t, std::string> message_type_names = {
    #define X(name, value) { value, #name },
            MESSAGE_TYPES
    #undef X
    };

    inline const std::unordered_map<uint8_t, std::string> reset_status_names = {
    #define X(name, value) { value, #name },
            RESET_STATUS
    #undef X
    };

    inline const std::unordered_map<uint8_t, std::string> event_names = {
    #define X(name, value) { value, #name },
            EVENTS
    #undef X
    };

#endif

#define SYSTEM_RESET 0xdd
#define MAX_BREAKPOINTS 32

#endif //LIBF1COMM_DEFINES_HH
