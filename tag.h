#ifndef TAG_H
#define TAG_H

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <QPixmap>

namespace Apollo
{

class Tag
{
public:
    typedef std::map<std::string, std::string> MapStdStr2StdStr;
    virtual const MapStdStr2StdStr& getFrameMap() const = 0;
    virtual const MapStdStr2StdStr& getGenericKeyMap() const = 0;
    std::string getDataString(std::string key);
    virtual QPixmap getAlbumArt() = 0;
    virtual bool isValid() = 0;

protected:
    std::string m_filename;
    std::string m_format;
    MapStdStr2StdStr m_data;          // Contains all key, data pairs from tag
    //MapStdStr2StdStr m_genericKeyMap; // Maps things like ARTIST => tag specific artist key

    virtual void parse(std::vector<unsigned char> data) = 0;

};

} // namespace Apollo

#endif // TAG_H
