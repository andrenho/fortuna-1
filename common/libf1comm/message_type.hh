#ifndef LIBF1COMM_MESSAGE_TYPE_HH
#define LIBF1COMM_MESSAGE_TYPE_HH

static constexpr uint8_t FinalByte = 0xe4;

enum MessageClass : uint8_t {
    MC_Request          = 0xf0,
    MC_Reply            = 0xf1,
    MC_DebugInformation = 0xf2,
};

enum MessageType : uint8_t {
    Nop             = 0,
    
    // tests
    TestDebug       = 1,
    TestDMA         = 2,
    
    // internal
    FreeMem         = 9,
    
    // RAM
    RamReadByte     = 10,
    RamWriteByte    = 11,
    RamReadBlock    = 12,
    RamWriteBlock   = 13,
    DataReadBus     = 14,
    DataWriteBus    = 15,
    
    
    
    Undefined       = 0xff,
};

enum class DeserializationError {
    NoErrors, InvalidMessageClass, ChecksumDoesNotMatch, FinalByteNotReceived, BufferDataTooLarge,
};

#endif //LIBF1COMM_MESSAGE_TYPE_HH
