#ifndef APOLLO_ID3V2_3_0TAG_H
#define APOLLO_ID3V2_3_0TAG_H

#include "id3v2tag.h"

namespace Apollo {

class ID3v2_3_0Tag : public Apollo::ID3v2Tag
{
public:
    ID3v2_3_0Tag(unsigned char flags, std::vector<unsigned char> & data);
    virtual QPixmap getAlbumArt();
    static MapStdStr2StdStr m_frameMap;
    static MapStdStr2StdStr m_genericKeyMap;
    virtual const MapStdStr2StdStr& getFrameMap() const;
    virtual const MapStdStr2StdStr& getGenericKeyMap() const;
    bool isValid();

private:
    void parse(std::vector<unsigned char> data);
    bool m_valid;
};

} // namespace Apollo

#endif // APOLLO_ID3V2_3_0TAG_H
