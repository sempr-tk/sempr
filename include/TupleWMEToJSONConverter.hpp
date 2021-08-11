#ifndef SEMPR_TUPLEWMETOJSONCONVERTER_HPP_
#define SEMPR_TUPLEWMETOJSONCONVERTER_HPP_

#include <rete-reasoner/WMEToJSONConverter.hpp>

namespace sempr {

/**
 * Adds conversions for basic tuple wmes like
 * EmptyWME, TupleWME<int>, TupleWME<std::string>, ...
 */
class TupleWMEToJSONConverter : public rete::WMEToJSONConverter {
public:
    bool convert(rete::WME::Ptr wme, std::string& json);
};

}


#endif /* SEMPR_TUPLEWMETOJSONCONVERTER_HPP_ */