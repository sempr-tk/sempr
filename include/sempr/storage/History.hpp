#ifndef SEMPR_STORAGE_HISTORY
#define SEMPR_STORAGE_HISTORY

#include <odb/core.hxx>

#include <sempr/storage/DBObject.hpp>
#include <ostream>
#include <vector>

namespace sempr { namespace storage {

/** Wrap a object-pointer with a history.
    E.g.:
    HistoryBase<Entity> e;
    e = Entity::Ptr(...); // push new reference, assume it is already persisted.
    e = Entity(...); // push new value -- TODO:
        This needs some care!
        How does persistence work? Need to persist the newly created object!
*/
template<typename T>
class History {
public:
    History() {
        init_ = false;
        values_.push_back(T()); // default value
    }
    
    void operator=(const T& other) {
        addValue(other);
    }
    
    T& operator[](size_t index) {
        return values_.at(index);
    }
    
    T& operator*() {
        return values_.back();
    }
    
    size_t size() const {
        return values_.size();
    }
        
    operator T() {
        return values_.back(); 
    }
    
        
protected:
    friend class odb::access;
    std::vector<T> values_;
    bool init_;
    
    void addValue(const T& val) {
        if (!init_) {
            // remove the default value that was added
            // in the ctor
            values_.clear();
            init_ = true;
        }
        
        values_.push_back( val );
    }
    
};

typedef History<std::string>    StringHistory;
typedef History<int>            IntHistory;
typedef History<long>           LongHistory;
typedef History<float>          FloatHistory;
typedef History<double>         DoubleHistory;
typedef History<unsigned int>   UIntHistory;
typedef History<unsigned long>  ULongHistory;

#pragma db value(StringHistory)
#pragma db value(IntHistory)
#pragma db value(LongHistory)
#pragma db value(FloatHistory)
#pragma db value(DoubleHistory)
#pragma db value(UIntHistory)
#pragma db value(ULongHistory)


} /* storage */
    
} /* sempr */

#endif /* end of include guard: SEMPR_STORAGE_HISTORY */
