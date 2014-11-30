#include "tag.h"

namespace Apollo {

std::string Tag::getDataString(std::string key) {
    MapStdStr2StdStr genericKeyMap = getGenericKeyMap();

    if (genericKeyMap.end() == genericKeyMap.find(key)) {
        std::cerr << "Unknown generic key: " << key << std::endl;
        return "";
    } else {
        if (m_data.end() == m_data.find(genericKeyMap.at(key))) {
            return "";
        }
        return m_data.at(genericKeyMap.at(key));
    }
}

}
