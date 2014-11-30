#include "id3v2_3_0tag.h"

//#define FRAME_DEBUG

namespace Apollo {

Tag::MapStdStr2StdStr ID3v2_3_0Tag::m_genericKeyMap = {
    {"ARTIST", "TPE1"},
    {"TITLE", "TIT2"},
    {"ALBUM", "TALB"},
    {"TRACK_NUMBER", "TRCK"},
    {"ALBUM_ART", "APIC"},
};

Tag::MapStdStr2StdStr ID3v2_3_0Tag::m_frameMap = {
    {"AENC", "Audio encryption"},
    {"APIC", "Attached picture"},
    {"COMM", "Comments"},
    {"COMR", "Commercial frame"},
    {"ENCR", "Encryption method registration"},
    {"EQUA", "Equalization"},
    {"ETCO", "Event timing codes"},
    {"GEOB", "General encapsulated object"},
    {"GRID", "Group identification registration"},
    {"IPLS", "Involved people list"},
    {"LINK", "Linked information"},
    {"MCDI", "Music CD identifier"},
    {"MLLT", "MPEG location lookup table"},
    {"OWNE", "Ownership frame"},
    {"PRIV", "Private frame"},
    {"PCNT", "Play counter"},
    {"POPM", "Popularimeter"},
    {"POSS", "Position synchronisation frame"},
    {"RBUF", "Recommended buffer size"},
    {"RVAD", "Relative volume adjustment"},
    {"RVRB", "Reverb"},
    {"SYLT", "Synchronized lyric/text"},
    {"SYTC", "Synchronized tempo codes"},
    {"TALB", "Album/Movie/Show title"},
    {"TBPM", "BPM (beats per minute)"},
    {"TCMP", "Part of a compilation"}, // unofficial
    {"TCOM", "Composer"},
    {"TCON", "Content type"},
    {"TCOP", "Copyright message"},
    {"TCP ", "Part of a compilation"}, // unofficial
    {"TDAT", "Date"},
    {"TDLY", "Playlist delay"},
    {"TDRC", "Recording time"}, // unofficial
    {"TENC", "Encoded by"},
    {"TEXT", "Lyricist/Text writer"},
    {"TFLT", "File type"},
    {"TIME", "Time"},
    {"TIT1", "Content group description"},
    {"TIT2", "Title/songname/content description"},
    {"TIT3", "Subtitle/Description refinement"},
    {"TKEY", "Initial key"},
    {"TLAN", "Language(s)"},
    {"TLEN", "Length"},
    {"TMED", "Media type"},
    {"TOAL", "Original album/movie/show title"},
    {"TOFN", "Original filename"},
    {"TOLY", "Original lyricist(s)/text writer(s)"},
    {"TOPE", "Original artist(s)/performer(s)"},
    {"TORY", "Original release year"},
    {"TOWN", "File owner/licensee"},
    {"TPE1", "Lead performer(s)/Soloist(s)"},
    {"TPE2", "Band/orchestra/accompaniment"},
    {"TPE3", "Conductor/performer refinement"},
    {"TPE4", "Interpreted, remixed, or otherwise modified by"},
    {"TPOS", "Part of a set"},
    {"TPUB", "Publisher"},
    {"TRCK", "Track number/Position in set"},
    {"TRDA", "Recording dates"},
    {"TRSN", "Internet radio station name"},
    {"TRSO", "Internet radio station owner"},
    {"TSIZ", "Size"},
    {"TSOA", "Album sort order"}, // unofficial
    {"TSOP", "Performer sort order"}, // unofficial
    {"TSOT", "Title sort order"}, // unofficial
    {"TSRC", "ISRC (international standard recording code)"},
    {"TSSE", "Software/Hardware and settings used for encoding"},
    {"TYER", "Year"},
    {"TXXX", "User defined text information frame"},
    {"UFID", "Unique file identifier"},
    {"USER", "Terms of use"},
    {"USLT", "Unsychronized lyric/text transcription"},
    {"WCOM", "Commercial information"},
    {"WCOP", "Copyright/Legal information"},
    {"WOAF", "Official audio file webpage"},
    {"WOAR", "Official artist/performer webpage"},
    {"WOAS", "Official audio source webpage"},
    {"WORS", "Official internet radio station homepage"},
    {"WPAY", "Payment"},
    {"WPUB", "Publishers official webpage"},
    {"WXXX", "User defined URL link frame"},
};

QPixmap ID3v2_3_0Tag::getAlbumArt() {
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

ID3v2_3_0Tag::ID3v2_3_0Tag(unsigned char flags, std::vector<unsigned char> & data):
    ID3v2Tag()
{
    // FIXME do something with flags

    m_frameIdSize = 4;
    m_frameSizeSize = 4;
    m_frameFlagsSize = 2;
    m_valid = true;
    m_syncSafeSize = false;
    parse(data);
}

const Tag::MapStdStr2StdStr& ID3v2_3_0Tag::getFrameMap() const {
    return m_frameMap;
}

const Tag::MapStdStr2StdStr& ID3v2_3_0Tag::getGenericKeyMap() const {
    return m_genericKeyMap;
}

void ID3v2_3_0Tag::parse(std::vector<unsigned char> data) {
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

bool ID3v2_3_0Tag::isValid() {
    return m_valid;
}

} // namespace Apollo
