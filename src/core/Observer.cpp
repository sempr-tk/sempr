#include <sempr/core/Observer.hpp>
// #include <iostream>

namespace sempr { namespace core {

Observer::Observer()
{
}

Observer::~Observer()
{
}

// void Observer::notify(Observable::Ptr o)
// {
//     if (types_.find(typeid(*o)) != types_.end()) {
//         types_[typeid(*o)](o);
//     } else {
//         // std::cout << typeid(*this).name() << " couldnt find " << typeid(*o).name() << '\n';
//     }
// }

#include <iostream>

void Observer::notify(Observable::Ptr o)
{
    doProcess(o);
}

void Observer::doProcess(Observable::Ptr o)
{
    for (auto pair : processPairs_)
    {
        if (pair.checkFunction(o)) { pair.processFunction(o); }
    }
}

bool Observer::canProcess(Observable::Ptr o)
{
    for (auto pair : processPairs_)
    {
        if (pair.checkFunction(o)) return true;
    }
    return false;
}

void Observer::addProcessPair(ProcessPair p)
{
    processPairs_.push_back(p);
}

void Observer::addProcessPair(ProcessPair::Check_f check, ProcessPair::Process_f process)
{
    ProcessPair p;
    p.checkFunction = check;
    p.processFunction = process;

    addProcessPair(p);
}

} /* core */

} /* sempr */
