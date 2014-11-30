#ifndef APOLLO_ID3V2TAG_H
#define APOLLO_ID3V2TAG_H

#include <string>
#include <vector>
#include "tag.h"

namespace Apollo {

class ID3v2Frame
{
public:
    ID3v2Frame(std::vector<unsigned char> & data, unsigned long & offset, unsigned long frameIdSize, unsigned long frameSizeSize, unsigned long frameFlagsSize, bool syncSafeSize);
    unsigned long getSize();
    std::string getFrameId();
    std::string getDataString();
    bool isValid();

protected:
    std::string m_frameId;
    unsigned char m_textFormat;
    std::vector<unsigned char> * m_data;
    std::vector<unsigned char> * m_flags;
    bool m_valid;
};


class ID3v2Tag : public Apollo::Tag
{
public:
    ID3v2Tag();
    ~ID3v2Tag();
    static MapStdStr2StdStr m_versionMap_3_0_to_4_0;
    virtual bool isValid() = 0;

protected:
    unsigned long m_frameIdSize;
    unsigned long m_frameSizeSize;
    unsigned long m_frameFlagsSize;
    bool m_syncSafeSize;
};

} // namespace Apollo

#endif // APOLLO_ID3V2TAG_H
