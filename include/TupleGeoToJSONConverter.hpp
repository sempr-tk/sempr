#ifndef SEMPR_TUPLEGEOTOJSONCONVERTER_HPP_
#define SEMPR_TUPLEGEOTOJSONCONVERTER_HPP_

#include <rete-reasoner/WMEToJSONConverter.hpp>

namespace sempr {

/**
 * Adds conversion for TupleWME<GeosGeometry::Ptr>
 */
class TupleGeoToJSONConverter : public rete::WMEToJSONConverter {
public:
    bool convert(rete::WME::Ptr wme, std::string& json);
};

}


#endif /* SEMPR_TUPLEGEOTOJSONCONVERTER_HPP_ */