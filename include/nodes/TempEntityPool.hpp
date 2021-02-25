#ifndef SEMPR_TEMPENTITYPOOL_HPP_
#define SEMPR_TEMPENTITYPOOL_HPP_

#include "Entity.hpp"
#include <map>
#include <string>

namespace sempr {

/**
    The TempEntityPool is a singleton :( used by the CreateEntityNode in order
    to not create multiple entities with the same id, even if the multiple
    CreateEntityNodes are used.
*/
class TempEntityPool {
    TempEntityPool() = default;
    TempEntityPool(const TempEntityPool&) = delete;
    TempEntityPool(TempEntityPool&&) = delete;
    TempEntityPool& operator = (const TempEntityPool&) = delete;
    TempEntityPool& operator = (TempEntityPool&&) = delete;

public:
    static TempEntityPool& instance()
    {
        static TempEntityPool pool;
        return pool;
    }

    Entity::Ptr get(const std::string& id, bool isURI)
    {
        auto it = entities_.find({id, isURI});
        if (it != entities_.end())
        {
            return it->second;
        }
        else
        {
            auto e = Entity::create();
            if (isURI)
                e->setURI(id);
            else
                e->setId(id);

            entities_[{id, isURI}] = e;
            return e;
        }
    }

private:
    std::map<
        std::pair<std::string, bool>,
        Entity::Ptr> entities_;

};

}


#endif /* include guard: SEMPR_TEMPENTITYPOOL_HPP_ */
