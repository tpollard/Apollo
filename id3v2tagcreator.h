#ifndef APOLLO_ID3V2TAGCREATOR_H
#define APOLLO_ID3V2TAGCREATOR_H

#include "tagcreator.h"
#include "id3v2_3_0tag.h"
#include "id3v2_4_0tag.h"

namespace Apollo {

class ID3v2TagCreator : public Apollo::TagCreator
{
public:
    ID3v2TagCreator();
    virtual Tag * createTag(std::string);
};

} // namespace Apollo

#endif // APOLLO_ID3V2TAGCREATOR_H
