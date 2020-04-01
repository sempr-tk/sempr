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
    SEMPR_COMPONENT

    std::string file_; // name of the file
public:
    using Ptr = std::shared_ptr<TripleDocument>;
    /**
        Creates a TripleDocument pointing at the specified file.
    */
    TripleDocument(const std::string& file);
    TripleDocument() = default; // mainly for cereal.

    /**
        Sets a new filename.
    */
    void setFile(const std::string& file);

    std::string getFile() const;

    /**
        Serialization with cereal
    */
    template <class Archive>
    void save(Archive& ar) const
    {
        ar( cereal::make_nvp<Archive>("base", cereal::base_class<Component>(this)),
            cereal::make_nvp<Archive>("file", file_) );
    }

    template <class Archive>
    void load(Archive& ar)
    {
        ar( cereal::make_nvp<Archive>("base", cereal::base_class<Component>(this)),
            cereal::make_nvp<Archive>("file", file_) );
    }
};


template <>
struct ComponentName<TripleDocument> {
    static constexpr const char* value = "TripleDocument";
};

}

CEREAL_REGISTER_TYPE(sempr::TripleDocument)

#endif /* include guard: SEMPR_COMPONENT_TRIPLEDOCUMENT_HPP_ */

