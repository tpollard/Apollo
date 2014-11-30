#include "id3v2tag.h"

//#define FRAME_DEBUG

namespace Apollo {

Tag::MapStdStr2StdStr ID3v2Tag::m_versionMap_3_0_to_4_0 = {
    {"EQUA", "EQU2"},
    {"IPLS", "TIPL"},
    {"RVAD", "RVA2"},
    {"TDAT", "TDRC"},
    {"TIME", "TDRC"},
    {"TORY", "TDOR"},
    {"TRDA", "TDRC"},
    {"TSIZ", ""},
    {"TYER", "TDRC"},
};

ID3v2Frame::ID3v2Frame(std::vector<unsigned char> &data, unsigned long & offset, unsigned long frameIdSize, unsigned long frameSizeSize, unsigned long frameFlagsSize, bool syncSafeSize) {
    m_valid = true;

    // Check that the header can be read
    if ((offset + frameIdSize + frameSizeSize + frameFlagsSize) < data.size()) {
        m_frameId = std::string((char *) &data[offset], frameIdSize);
#ifdef FRAME_DEBUG
        std::cout << "Frame id: " << m_frameId << " @ " << std::hex << offset << std::endl;
#endif
        if (m_frameId == std::string("\0\0\0\0", 4)) {
            offset = data.size();
            return;
        }
        offset += frameIdSize;
        unsigned long size = 0;
        for (unsigned int i = 0; i < frameSizeSize; i++) {
            if (syncSafeSize) {
                size = (size << 7) | data[offset + i];
            } else {
                size = (size << 8) | data[offset + i];
            }
        }
        offset += frameSizeSize;
#ifdef FRAME_DEBUG
        std::cout << "Size: 0x" << std::hex << size << std::endl;
#endif
        m_flags = NULL;
        offset += frameFlagsSize;
        m_textFormat = 0;
        if (m_frameId[0] == 'T') {
            m_textFormat = data[offset];
            offset++;
            size--;
        }
        if (data.begin()+offset+size > data.end()) {
            std::cerr << "Size too big at offset " << offset << std::endl;
            m_valid = false;
        } else {
            m_data = new std::vector<unsigned char>(data.begin()+offset, data.begin()+offset+size);
            m_valid = true;
            offset += size;
    #ifdef FRAME_DEBUG
            std::cout << "Data: " << std::string((char *) m_data->data(), m_data->size()) << std::endl;
    #endif
        }
    } else {
        std::cerr << "Requested size too big for data: requested=" << offset + frameIdSize + frameSizeSize + frameFlagsSize << " data=" << data.size() << std::endl;
    }

    return;
}

bool ID3v2Frame::isValid() {
    return m_valid;
}

unsigned long ID3v2Frame::getSize() {
    return m_data->size();
}

std::string ID3v2Frame::getFrameId() {
    return m_frameId;
}

std::string ID3v2Frame::getDataString() {
    return std::string((char *) m_data->data(), m_data->size());
}

ID3v2Tag::ID3v2Tag() {
}

ID3v2Tag::~ID3v2Tag() {
}

} // namespace Apollo
