#include <Soprano/Soprano>
#include <Soprano/Parser>

#include <iostream>

#include "Exception.hpp"
#include "component/TripleDocument.hpp"
#include "component/TripleVector.hpp"

namespace sempr {

TripleDocument::TripleDocument(const std::string& file)
    : file_(file)
{
}

void TripleDocument::setFile(const std::string& file)
{
    file_ = file;
}

std::string TripleDocument::getFile() const
{
    return file_;
}

}
