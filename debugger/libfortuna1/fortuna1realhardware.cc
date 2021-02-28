#include "fortuna1realhardware.hh"
#include "replyexception.hh"

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
    return serial_.request(req).freemem().amount();
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
    return serial_.request(req).testdma().response();
}

void Fortuna1RealHardware::ram_write_byte(uint16_t addr, uint8_t data)
{
    Request req;
    req.set_type(MessageType::RAM_WRITE_BYTE);
    auto ram_request = new RamRequest();
    ram_request->set_address(addr);
    ram_request->set_byte(data);
    req.set_allocated_ramrequest(ram_request);
    uint8_t response = serial_.request(req).ramresponse().byte();
    if (response != data)
        throw ReplyException("The byte sent to memory is not the same one returned by DMA.");
}

uint8_t Fortuna1RealHardware::ram_read_byte(uint16_t addr) const
{
    Request req;
    req.set_type(MessageType::RAM_READ_BYTE);
    auto ram_request = new RamRequest();
    ram_request->set_address(addr);
    req.set_allocated_ramrequest(ram_request);
    return serial_.request(req).ramresponse().byte();
}
