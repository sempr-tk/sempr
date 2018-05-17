#ifndef SEMPR_QUERY_QUERY_H_
#define SEMPR_QUERY_QUERY_H_

#include <string>
#include <memory>

#include <sempr/core/Observable.hpp>

namespace sempr { namespace query {

    class Query : public core::Observable, public core::OType<Query> {
    public:
        virtual std::string type() const = 0;
        virtual ~Query(){}
    };

}}

#endif /* end of include guard: SEMPR_QUERY_QUERY_H_ */
