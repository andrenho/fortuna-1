#ifndef LIBF1COMM_DEFINES_HH
#define LIBF1COMM_DEFINES_HH

static constexpr uint8_t FinalByte      = 0xe4;
static constexpr uint8_t InvalidCommand = 0xef;

enum MessageClass : uint8_t {
    MC_Request          = 0xf0,
    MC_Reply            = 0xf1,
    MC_DebugInformation = 0xf2,
};

enum MessageType : uint8_t {
    Nop             = 0x00,
    
    // tests
    TestDebug       = 0x01,
    TestDMA         = 0x02,
    
    // general
    FreeMem         = 0x0a,
    Reset           = 0x0b,
    
    // RAM
    RamReadByte     = 0x10,
    RamWriteByte    = 0x11,
    RamReadBlock    = 0x12,
    RamWriteBlock   = 0x13,
    DataReadBus     = 0x14,
    DataWriteBus    = 0x15,
    
    // SDCard
    SDCard_Status   = 0x20,
    SDCard_Read     = 0x21,
    
    Undefined       = 0xff,
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
};

#endif //LIBF1COMM_DEFINES_HH
