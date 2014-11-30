#include "utils.h"

namespace Apollo {

std::string toHexStr(const std::vector<unsigned char> & data, const std::string & separator)
{
    std::string hexStr;
    std::string chars = "0123456789ABCDEF";
    for(size_t i = 0; i < data.size(); i++)
    {
        unsigned char upper = (data[i] >> 4) & 0x0F;
        unsigned char lower = data[i] & 0x0F;
        hexStr.append(&chars[upper], 1);
        hexStr.append(&chars[lower], 1);
        if ((i + 1) != data.size())
        {
            hexStr.append(separator);
        }
    }
    return hexStr;
}

} // namespace Apollo
