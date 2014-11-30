#include "tagcreator.h"
#include "id3v2tagcreator.h"

namespace Apollo {

const unsigned int HEADER_READ_SIZE = 10;

TagCreator::TagCreator() {
}

Tag* TagCreator::createTag(std::string filename) {
    Tag * tag = NULL;

    // Open file for binary read
    std::ifstream song;
    song.open(filename.c_str(), std::ios::in | std::ios::binary);

    if (song.is_open()) {
        // Read first 10 (arbitrary) bytes
        song.seekg(0, std::ios::beg);
        std::vector<unsigned char> header(HEADER_READ_SIZE, 0);
        song.read((char *) header.data(), HEADER_READ_SIZE);
        size_t sizeRead = song.tellg();
        song.close();

        if (sizeRead != HEADER_READ_SIZE) {
            std::cerr << "Error reading file header" << std::endl;
        } else {
            std::string fileId((char*) &header[0], 3);
            if (fileId == "ID3") {
                ID3v2TagCreator id3v2TagCreator = ID3v2TagCreator();
                tag = id3v2TagCreator.createTag(filename);
            } else {
                std::cerr << "Unsupported tag type. Header was: 0x" << Apollo::toHexStr(header) << " (" << filename << ")" << std::endl;
            }
        }
    } else {
        std::cerr << "Error opening file: " << filename << std::endl;
    }

    return tag;
}

} // namespace Apollo
