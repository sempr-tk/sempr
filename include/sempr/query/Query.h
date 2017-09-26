#ifndef SEMPR_QUERY_QUERY_H_
#define SEMPR_QUERY_QUERY_H_

#include <string>
#include <memory>

class Query {
public:
    virtual std::string type() const = 0;
    virtual ~Query(){}
};

#endif /* end of include guard: SEMPR_QUERY_QUERY_H_ */
