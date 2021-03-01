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
    return serial_.request(req).buffer();
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

void Fortuna1RealHardware::ram_write_buffer(uint16_t addr, std::vector<uint8_t> const& bytes)
{
    /*
    if (bytes.size() > 512)
        throw std::runtime_error("There's a 512 byte limit to buffer size when writing to RAM.");
    Request req;
    req.set_type(MessageType::RAM_WRITE_BLOCK);
    auto ram_request = new RamRequest();
    ram_request->set_address(addr);
    auto buffer = new std::string();
    for (uint8_t b: bytes)
        *buffer += b;
    ram_request->set_allocated_buffer(buffer);
    req.set_allocated_ramrequest(ram_request);
    serial_.request(req);
     */
}

std::vector<uint8_t> Fortuna1RealHardware::ram_read_buffer(uint16_t addr, uint16_t sz) const
{
    /*
    if (sz > 512)
        throw std::runtime_error("There's a 512 byte limit to buffer size when writing to RAM.");
    Request req;
    req.set_type(MessageType::RAM_READ_BLOCK);
    auto ram_request = new RamRequest();
    ram_request->set_address(addr);
    ram_request->set_size(sz);
    auto& str = serial_.request(req).ramresponse().buffer();
    return std::vector<uint8_t>(str.begin(), str.end());
     */
}
