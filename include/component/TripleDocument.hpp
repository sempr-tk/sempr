#ifndef SEMPR_COMPONENT_TRIPLEDOCUMENT_HPP_
#define SEMPR_COMPONENT_TRIPLEDOCUMENT_HPP_

#include "TripleContainer.hpp"

namespace sempr {

/**
    The TripleDocument is a component that mirrors the content of a file.
    It is not mutable other than through changing the contents of the file and
    calling reload(), or pointing it to a different file.
*/
class TripleDocument : public TripleContainer {
    std::string file_; // name of the file
    std::vector<Triple> triples_; // content of the file
public:
    /**
        Creates a TripleDocument pointing at the given file, but does not load
        the file, yet. Call reload() to initialize the component.
    */
    TripleDocument(const std::string& file);

    /**
        Clears the cached data an reloads the specified file.
        Throws an exception if the file cannot be parsed. The old content will
        be already gone by that point.
    */
    void reload();

    /**
        Clears the cached data and loads the contents of the (newly)
        specified file.
    */
    void setFile(const std::string& file);

    /**
        Allow iterating the the internal triple vector.
    */
    TripleIterator begin() const;
    TripleIterator end() const;
};


}


#endif /* include guard: SEMPR_COMPONENT_TRIPLEDOCUMENT_HPP_ */

