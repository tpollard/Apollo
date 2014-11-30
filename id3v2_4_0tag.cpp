#include "id3v2_4_0tag.h"

namespace Apollo {

Tag::MapStdStr2StdStr ID3v2_4_0Tag::m_genericKeyMap = {
    {"ARTIST", "TPE1"},
    {"TITLE", "TIT2"},
    {"ALBUM", "TALB"},
    {"TRACK_NUMBER", "TRCK"},
    {"ALBUM_ART", "APIC"},
};

Tag::MapStdStr2StdStr ID3v2_4_0Tag::m_frameMap = {
    {"AENC", "Audio encryption"},
    {"APIC", "Attached picture"},
    {"ASPI", "Audio seek point index"},
    {"COMM", "Comments"},
    {"COMR", "Commercial frame"},
    {"ENCR", "Encryption method registration"},
    {"EQU2", "Equalisation (2)"},
    {"ETCO", "Event timing codes"},
    {"GEOB", "General encapsulated object"},
    {"GRID", "Group identification registration"},
    {"LINK", "Linked information"},
    {"MCDI", "Music CD identifier"},
    {"MLLT", "MPEG location lookup table"},
    {"OWNE", "Ownership frame"},
    {"PRIV", "Private frame"},
    {"PCNT", "Play counter"},
    {"POPM", "Popularimeter"},
    {"POSS", "Position synchronisation frame"},
    {"RBUF", "Recommended buffer size"},
    {"RVA2", "Relative volume adjustment (2)"},
    {"RVRB", "Reverb"},
    {"SEEK", "Seek frame"},
    {"SIGN", "Signature frame"},
    {"SYLT", "Synchronised lyric/text"},
    {"SYTC", "Synchronised tempo codes"},
    {"TALB", "Album/Movie/Show title"},
    {"TBPM", "BPM (beats per minute)"},
    {"TCMP", "Part of a compilation"}, // unofficial
    {"TCOM", "Composer"},
    {"TCON", "Content type"},
    {"TCOP", "Copyright message"},
    {"TCP ", "Part of a compilation"}, // unofficial
    {"TDEN", "Encoding time"},
    {"TDLY", "Playlist delay"},
    {"TDOR", "Original release time"},
    {"TDRC", "Recording time"},
    {"TDRL", "Release time"},
    {"TDTG", "Tagging time"},
    {"TENC", "Encoded by"},
    {"TEXT", "Lyricist/Text writer"},
    {"TFLT", "File type"},
    {"TIPL", "Involved people list"},
    {"TIT1", "Content group description"},
    {"TIT2", "Title/songname/content description"},
    {"TIT3", "Subtitle/Description refinement"},
    {"TKEY", "Initial key"},
    {"TLAN", "Language(s)"},
    {"TLEN", "Length"},
    {"TMCL", "Musician credits list"},
    {"TMED", "Media type"},
    {"TMOO", "Mood"},
    {"TOAL", "Original album/movie/show title"},
    {"TOFN", "Original filename"},
    {"TOLY", "Original lyricist(s)/text writer(s)"},
    {"TOPE", "Original artist(s)/performer(s)"},
    {"TOWN", "File owner/licensee"},
    {"TPE1", "Lead performer(s)/Soloist(s)"},
    {"TPE2", "Band/orchestra/accompaniment"},
    {"TPE3", "Conductor/performer refinement"},
    {"TPE4", "Interpreted, remixed, or otherwise modified by"},
    {"TPOS", "Part of a set"},
    {"TPRO", "Produced notice"},
    {"TPUB", "Publisher"},
    {"TRCK", "Track number/Position in set"},
    {"TRSN", "Internet radio station name"},
    {"TRSO", "Internet radio station owner"},
    {"TSOA", "Album sort order"},
    {"TSOP", "Performer sort order"},
    {"TSOT", "Title sort order"},
    {"TSRC", "ISRC (international standard recording code)"},
    {"TSSE", "Software/Hardware and settings used for encoding"},
    {"TSST", "Set subtitle"},
    {"TXXX", "User defined text information frame"},
    {"UFID", "Unique file identifier"},
    {"USER", "Terms of use"},
    {"USLT", "Unsynchronised lyric/text transcription"},
    {"WCOM", "Commercial information"},
    {"WCOP", "Copyright/Legal information"},
    {"WOAF", "Official audio file webpage"},
    {"WOAR", "Official artist/performer webpage"},
    {"WOAS", "Official audio source webpage"},
    {"WORS", "Official Internet radio station homepage"},
    {"WPAY", "Payment"},
    {"WPUB", "Publishers official webpage"},
    {"WXXX", "User defined URL link frame"},
};

QPixmap ID3v2_4_0Tag::getAlbumArt() {
    QPixmap art;
    std::string dataString = getDataString("ALBUM_ART");
    if (dataString.size() != 0) {
        std::vector<unsigned char> data(dataString.begin(), dataString.end());
        unsigned long offset = 0;
        unsigned char textEncoding = data[offset++];
        std::string MIMEtype((char*) &data[offset]);
        offset += MIMEtype.size() + 1;
        unsigned char pictureType = data[offset++];
        // Picture type:
        // $00     Other
        // $01     32x32 pixels 'file icon' (PNG only)
        // $02     Other file icon
        // $03     Cover (front)
        // $04     Cover (back)
        // $05     Leaflet page
        // $06     Media (e.g. lable side of CD)
        // $07     Lead artist/lead performer/soloist
        // $08     Artist/performer
        // $09     Conductor
        // $0A     Band/Orchestra
        // $0B     Composer
        // $0C     Lyricist/text writer
        // $0D     Recording Location
        // $0E     During recording
        // $0F     During performance
        // $10     Movie/video screen capture
        // $11     A bright coloured fish
        // $12     Illustration
        // $13     Band/artist logotype
        // $14     Publisher/Studio logotype

        std::string description((char*) &data[offset]);
        offset += description.size() + 1;
        art.loadFromData(&data[offset], data.size()-offset);
    }

    return art;
}

ID3v2_4_0Tag::ID3v2_4_0Tag(unsigned char flags, std::vector<unsigned char> & data):
    ID3v2Tag()
{
    // FIXME do something with flags

    m_frameIdSize = 4;
    m_frameSizeSize = 4;
    m_frameFlagsSize = 2;
    m_valid = true;
    m_syncSafeSize = true;
    parse(data);
}

const Tag::MapStdStr2StdStr& ID3v2_4_0Tag::getFrameMap() const {
    return m_frameMap;
}

const Tag::MapStdStr2StdStr& ID3v2_4_0Tag::getGenericKeyMap() const {
    return m_genericKeyMap;
}

void ID3v2_4_0Tag::parse(std::vector<unsigned char> data) {
    unsigned long offset = 0;

    while (offset < data.size()) {
        ID3v2Frame frame(data, offset, m_frameIdSize, m_frameSizeSize, m_frameFlagsSize, m_syncSafeSize);
        if (frame.isValid()) {
            if (m_frameMap.end() == m_frameMap.find(frame.getFrameId())) {
                if ((frame.getFrameId()[0] != 0) || (frame.getFrameId()[1] != 0) || (frame.getFrameId()[2] != 0) || (frame.getFrameId()[3] != 0)) {
                    std::cerr << "Unknown frame id: " << frame.getFrameId() << std::endl;
                }
                offset = data.size();
            } else {
                m_data.insert(std::pair<std::string, std::string>(frame.getFrameId(), frame.getDataString()));
            }
        } else {
            std::cerr << "Frame not valid" << std::endl;
            offset = data.size();
            m_valid = false;
        }
    }
}

bool ID3v2_4_0Tag::isValid() {
    return m_valid;
}

} // namespace Apollo
