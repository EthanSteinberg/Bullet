#include "boost/date_time/posix_time/posix_time.hpp"
#include <cstdint>

struct t_Packet
{
   uint8_t type :1;
};

struct t_pingPacket : public t_Packet
{
   int64_t time;
};

