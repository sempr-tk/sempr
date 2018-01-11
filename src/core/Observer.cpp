#include <sempr/core/Observer.hpp>
// #include <iostream>

namespace sempr { namespace core {

Observer::Observer()
{
}

Observer::~Observer()
{
}


void Observer::notify(Observable::Ptr o)
{
    if (types_.find(typeid(*o)) != types_.end()) {
        types_[typeid(*o)](o);
    } else {
        // std::cout << typeid(*this).name() << " couldnt find " << typeid(*o).name() << '\n';
    }
}

} /* core */

} /* sempr */
