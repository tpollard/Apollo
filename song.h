#ifndef SONG_H
#define SONG_H

#include "tag.h"
#include "tagcreator.h"
#include <string>

namespace Apollo {

class Song {
public:
    Song(std::string);
    ~Song();

    static TagCreator * m_tagCreator;
    Apollo::Tag * getTag();
    bool isTagValid();

private:
    Apollo::Tag * m_tag;
    std::string m_filename;
};

} //namespace Apollo

#endif // SONG_H
