#include "song.h"

Apollo::TagCreator * Apollo::Song::m_tagCreator = new Apollo::TagCreator();

Apollo::Song::Song(std::string filename) :
    m_filename(filename)
{
    m_tag = m_tagCreator->createTag(filename);
}

Apollo::Song::~Song() {
    //delete(m_tag);
}

Apollo::Tag * Apollo::Song::getTag() {
    return m_tag;
}

bool Apollo::Song::isTagValid() {
    return (m_tag == NULL) ? false : m_tag->isValid();
}
