#include <sempr/core/Observable.hpp>
#include <iostream>

namespace sempr { namespace core {

std::ostream& operator << (std::ostream& os, const Observable& observable)
{
    os << "Observable " << &observable << std::endl;
    for (auto t : observable.types_)
    {
        os << "    is a " << t.name() << std::endl;
    }

    return os;
}


} /* core */

} /* sempr */
