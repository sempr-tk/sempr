#ifndef SEMPR_STORAGE_DBOBJECT_H_
#define SEMPR_STORAGE_DBOBJECT_H_

#include <odb/core.hxx>
#include <odb/database.hxx>
#include <odb/callback.hxx>
#include <memory>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <sempr/storage/History.hpp>

namespace sempr { namespace storage {
    class Storage;

#pragma db object polymorphic pointer(std::shared_ptr) callback(dbcallback)
class DBObject {
public:
    using Ptr = std::shared_ptr<DBObject>;

    DBObject();// : id_(boost::uuids::nil_generator()()) , parent_() {}
    /*  Assigning a parent to the object transfers data-ownership. If the
        parent is removed from the database, this object will be removed as well.
        Careful: This is a database-internal mechanism, no event will be fired,
        except if done so explicitly from the parent...
    */
    DBObject(DBObject::Ptr parent);
        //: id_(boost::uuids::nil_generator()()), parent_(parent) {}
    virtual ~DBObject(){}

    /*  returns true if the object has an id pointing into the database.
        used to check if "update" or "persist" is needed. */
    bool persisted() const { return !id_.is_nil(); }


    virtual const boost::uuids::uuid& uuid() const { return id_; }


    // database-callbacks.
    virtual void prePersist(odb::database& db) const;
    virtual void postPersist(odb::database& db) const;
    virtual void preUpdate(odb::database& db) const;
    virtual void postUpdate(odb::database& db) const;
    virtual void preErase(odb::database& db) const;
    virtual void postErase(odb::database& db) const;
    virtual void preLoad(odb::database& db);
    virtual void postLoad(odb::database& db);

    /**
        Returns the auto-assigned (odb) discriminator to distinguish between
        different entities. To always return the discriminator of the _dynamic_
        type, you need to call setDiscriminator<MyClass>() in every ctor (for
        which you will need to #include <MyClass_odb.h> --> only in *.cpp!).
        This is only necessary for "fresh" objects. Upon loading an object from
        the database, it's discriminator is automatically loaded. But be aware
        that the session might prevent that!
    */
    const std::string discriminator() const;

protected:

    /** set the discriminator to the value used by odb. */
    template <class T>
    void setDiscriminator() {
        discriminator_ = odb::object_traits_impl<T, odb::id_common>::info.discriminator;
    }


private:
    friend class odb::access;
    friend class storage::Storage;

    void dbcallback(odb::callback_event e, odb::database& db);
    void dbcallback(odb::callback_event e, odb::database& db) const;

    #pragma db id type("TEXT")
    boost::uuids::uuid id_;

    #pragma db transient
    std::string discriminator_;

    // test: does this collide with the discriminator?
    // std::string typeid_; // yes.

    #pragma db on_delete(cascade)
    std::weak_ptr<DBObject> parent_;

    static boost::uuids::random_generator uuid_gen;

};

#pragma db view object(DBObject)
struct DBObject_type {
    #pragma db column("typeid")
    std::string discriminator_;
};


} /* storage */
} /* sempr */

#endif /* end of include guard: SEMPR_STORAGE_DBOBJECT_H_ */
