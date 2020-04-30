#ifndef SEMPR_FILEMONITORNODEBUILDER_HPP_
#define SEMPR_FILEMONITORNODEBUILDER_HPP_

#include <rete-reasoner/NodeBuilder.hpp>
#include <mutex>

#include "FileMonitorNode.hpp"

namespace sempr {

class FileMonitorNodeBuilder : public rete::NodeBuilder {
    std::recursive_mutex& mutex_; // the mutex to sync the monitoring threads with
public:
    /**
        Builds the FileMonitorNodes.
        Takes a mutex by reference and hands it to the constructed nodes, in
        order to synchronize the access to the rete network.

        Be aware when using this: There will be threads triggering stuff inside
        the rete network depending of modifications of files on your system,
        so the reasoning process is not straight forward anymore! The created
        nodes need access to the rete network to work, but you need to lock it
        before calling performInference()! Hence, situations may appear where
        not all there is to infer could have been inferred in performInference
        due to these access restrictions, and you will need to repeatedly call
        performInference(), releasing the lock inbetween calls.
    */
    FileMonitorNodeBuilder(std::recursive_mutex& mutex);

    rete::Builtin::Ptr buildBuiltin(rete::ArgumentList& args) const override;


};

}

#endif /* include guard: SEMPR_FILEMONITORNODEBUILDER_HPP_ */
