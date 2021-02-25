#include "fortuna1realhardware.hh"

Fortuna1RealHardware::Fortuna1RealHardware(std::string const& serial_port)
    : serial_(serial_port.c_str())
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

}

Fortuna1RealHardware::~Fortuna1RealHardware()
{
    google::protobuf::ShutdownProtobufLibrary();
}

size_t Fortuna1RealHardware::free_mem() const
{
    Request req;
    req.set_type(MessageType::FREE_MEM);
    return serial_.request(req).free_mem();
}

void Fortuna1RealHardware::test_debug_messages() const
{
    Request req;
    req.set_type(MessageType::TEST_DEBUG);
    serial_.request(req);
}

std::string Fortuna1RealHardware::test_dma() const
{
    Request req;
    req.set_type(MessageType::TEST_DMA);
    return serial_.request(req).buffer();
}
