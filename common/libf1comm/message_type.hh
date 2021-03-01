#ifndef LIBF1COMM_MESSAGE_TYPE_HH
#define LIBF1COMM_MESSAGE_TYPE_HH

enum MessageType {
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
    
    
    
    Undefined       = 99999,
};

enum DeserializationError {
    NoErrors, InvalidMessageClass, ChecksumDoesNotMatch,
};

#endif //LIBF1COMM_MESSAGE_TYPE_HH
