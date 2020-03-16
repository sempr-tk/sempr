#ifndef SEMPR_COMPONENT_TRIPLEDOCUMENT_HPP_
#define SEMPR_COMPONENT_TRIPLEDOCUMENT_HPP_

#include "Component.hpp"

namespace sempr {

/**
    The TripleDocument is a component that simply wraps a filename. It is
    merely an annotation to say "this is a file that should be loaded to
    obtain more information in the form of triples".
*/
class TripleDocument : public Component {
    std::string file_; // name of the file
public:
    using Ptr = std::shared_ptr<TripleDocument>;
    /**
        Creates a TripleDocument pointing at the specified file.
    */
    TripleDocument(const std::string& file);

    /**
        Sets a new filename.
    */
    void setFile(const std::string& file);

    std::string getFile() const;
};


template <>
struct ComponentName<TripleDocument> {
    static constexpr const char* value = "TripleDocument";
};

}

#endif /* include guard: SEMPR_COMPONENT_TRIPLEDOCUMENT_HPP_ */

