#include "id3v2tagcreator.h"

namespace Apollo {

const unsigned int ID3V2_HEADER_SIZE = 10;

ID3v2TagCreator::ID3v2TagCreator() {
}

Tag * ID3v2TagCreator::createTag(std::string filename) {
    Tag * tag = NULL;       // Tag that will be returned
    size_t sizeRead = 0;    // Temporary used to verify file reads

    std::ifstream song;
    song.open(filename.c_str(), std::ios::in | std::ios::binary);

    if (song.is_open()) {
        // Read entire header
        song.seekg(0, std::ios::end);
        //int songLength = song.tellg();
        song.seekg(0, std::ios::beg);
        std::vector<unsigned char> header(ID3V2_HEADER_SIZE, 0);
        song.read((char *) header.data(), ID3V2_HEADER_SIZE);
        sizeRead = song.tellg();

        if (sizeRead != ID3V2_HEADER_SIZE) {
            std::cerr << "Error reading file header. Could not read the entire header" << std::endl;
            return tag;
        }

        std::string fileId((char*) &header[0], 3);
        if (fileId != "ID3") {
            std::cerr << "ID3v2TagCreator called on a non-ID3v2 tag. Something is very wrong..." << std::endl;
            return tag;
        }

        unsigned char majorVersion = header[3];
        unsigned char minorVersion = header[4];

        if (((majorVersion == 2) || (majorVersion == 3) || (majorVersion == 4)) && (minorVersion == 0)) {
            std::string format = "ID3v2." + std::to_string((unsigned int) majorVersion) + "." + std::to_string((unsigned int) minorVersion);

            if ((0x80 <= header[6]) || (0x80 <= header[7]) || (0x80 <= header[8]) || (0x80 <= header[9])) {
                std::cerr << "Invalid size: 0x" << Apollo::toHexStr({header[6], header[7], header[8], header[9]}) << " (" << filename << ")" << std::endl;
            } else {
                unsigned long size  = (header[6] << 21)
                                    | (header[7] << 14)
                                    | (header[8] << 7)
                                    | header[9];
                unsigned char flags = header[5];

                std::vector<unsigned char> data(size, 0);
                song.read((char *) data.data(), size);
                sizeRead = (size_t) song.tellg() - ID3V2_HEADER_SIZE;
                song.close();

                if (size == sizeRead) {
                    switch (majorVersion) {
                        case 3:
                            if ((flags & 0x80) != 0) {
                                std::cerr << "Syncronization not supported (" << filename << ")" << std::endl;
                            } else {
                                tag = new ID3v2_3_0Tag(flags, data);
                            }
                            break;
                        case 4:
                            if ((flags & 0x80) != 0) {
                                std::cerr << "Syncronization not supported (" << filename << ")" << std::endl;
                                //tag = new ID3v2_4_0Tag(flags, data);
                            } else {
                                tag = new ID3v2_4_0Tag(flags, data);
                            }
                            break;
                        default:
                            std::cerr << "Unsupported version: " << format << " (" << filename << ")" << std::endl;
                    }
                } else {
                    std::cerr << "Unable to read entire tag data (" << filename << ")" << std::endl;
                }
            }
        } else {
            std::cerr << "Unsupported ID3v2 version: " << (unsigned int) majorVersion << "." << (unsigned int) minorVersion << " (" << filename << ")" << std::endl;
        }

    } else {
        std::cerr << "Error opening file: " << filename << std::endl;
    }

    return tag;
}

} // namespace Apollo
