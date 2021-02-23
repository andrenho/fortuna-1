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
    Reply reply = serial_.request(req);
    return reply.free_mem();
}
