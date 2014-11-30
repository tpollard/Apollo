#ifndef APOLLO_TAGCREATOR_H
#define APOLLO_TAGCREATOR_H

#include <QDebug>
#include "tag.h"
#include "utils.h"
#include <string>
#include <iostream>
#include <fstream>

namespace Apollo {

class TagCreator
{
public:
    TagCreator();
    virtual Tag* createTag(std::string);
};

} // namespace Apollo

#endif // APOLLO_TAGCREATOR_H
