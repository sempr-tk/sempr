#ifndef SEMPR_ECWMETOJSONCONVERTER_HPP_
#define SEMPR_ECWMETOJSONCONVERTER_HPP_

#include <rete-reasoner/WMEToJSONConverter.hpp>

namespace sempr {

class ECWMEToJSONConverter : public rete::WMEToJSONConverter {
public:
    bool convert(rete::WME::Ptr wme, std::string& json);
};

}


#endif /* SEMPR_ECWMETOJSONCONVERTER_HPP_ */