#ifndef SEMPR_ENTITY_RDFDOCUMENT_HPP_
#define SEMPR_ENTITY_RDFDOCUMENT_HPP_

#include <sempr/entity/RDFEntity.hpp>

namespace sempr { namespace entity {

/**
    RDFDocument loads all triples from a file into itself.
*/
#pragma db object
class RDFDocument : public RDFEntity {
    SEMPR_ENTITY
private:
    friend class odb::access;
    std::string filename_;

    RDFDocument();
public:
    using Ptr = std::shared_ptr<RDFDocument>;
    ~RDFDocument();

    std::string filename() const;

    /**
        Create an RDFDocument from a file.
    */
    static RDFDocument::Ptr FromFile(const std::string& file);

};

}}


#endif /* end of include guard SEMPR_ENTITY_RDFDOCUMENT_HPP_ */
