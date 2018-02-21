#ifndef SEMPR_STORAGE_DBOBJECT_H_
#define SEMPR_STORAGE_DBOBJECT_H_

#include <odb/core.hxx>
#include <odb/database.hxx>
#include <odb/callback.hxx>
#include <memory>

#include <sempr/storage/History.hpp>
// #include <sempr/core/IDGenerator.hpp>
#include <sempr/core/IDGenUtil.hpp>


namespace sempr { namespace storage {
    class Storage;

#pragma db object polymorphic pointer(std::shared_ptr) callback(dbcallback)
class DBObject {
public:
    using Ptr = std::shared_ptr<DBObject>;

    /**
        Assigning a parent to the object transfers data-ownership. If the
        parent is removed from the database, this object will be removed as well.
        Careful: This is a database-internal mechanism, no event will be fired,
        except if done so explicitly from the parent...

        Default ctor: Implicitely set the IDGenBase to IDGen<DBObject>().
    */
    DBObject(DBObject::Ptr parent = NULL);

    /**
        Takes an IDGenBase& to allow derived classes to specify
        their own ID-generation-method, or at least specialize the global
        strategy for the most derived type.
        (--> Chair_1 instead of DBObject_37)
    */
    DBObject(const core::IDGenBase* idgen, DBObject::Ptr parent = NULL);

    /**
        Copy constructors are too confusing, so I disabled them for all entities.
        It would be possible to define them for DBObject, Entity and Geometries (which is where I
        need them): New id using the same id-generator, same parent, no child-entities (shallow copy),
        and so on. But for more complex entities? Think about anything with an RDFEntity! You would
        need to create another RDFEntity, change its internals (subject!), and so on. No, instead:
        Implement clone-methods where you need them, and know exactly what you are doing.
    */
    DBObject(const DBObject&) = delete;


    virtual ~DBObject(){}

    /*
        Returns true if the object is known to be persisted (if it has been
        saved or loaded before).
    */
    bool persisted() const { return persisted_; }


    std::string id() const { return id_; }

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
    // database-callbacks.
    virtual void prePersist(odb::database& db) const;
    virtual void postPersist(odb::database& db) const;
    virtual void preUpdate(odb::database& db) const;
    virtual void postUpdate(odb::database& db) const;
    virtual void preErase(odb::database& db) const;
    virtual void postErase(odb::database& db) const;
    virtual void preLoad(odb::database& db);
    virtual void postLoad(odb::database& db);

    /** set the discriminator to the value used by odb. */
    template <class T>
    void setDiscriminator() {
        discriminator_ = odb::object_traits_impl<T, odb::id_common>::info.discriminator;
    }

    /**
        Set the parent of this object. If the parent is deleted from the database,
        its children are deleted, too.
    */
    void setParent(std::shared_ptr<const DBObject> parent) {
        parent_ = parent;
    }

private:
    friend class odb::access;
    friend class storage::Storage;

    void dbcallback(odb::callback_event e, odb::database& db);
    void dbcallback(odb::callback_event e, odb::database& db) const;

    #pragma db id
    std::string id_;

    #pragma db transient
    std::string discriminator_;

    /// A flag that states if the object has already been persisted.
    /// Is set to "false" in the ctor and to "true" in persist/load-callbacks
    #pragma db transient
    mutable bool persisted_;

protected:
    /// The wrapper from which the given id was generated.
    /// this is used to revoke the id in preLoad.
    /// It's only protected since the Entity-class needs access
    /// to release IDs of its newChildren_ in preLoad.
    #pragma db transient
    std::unique_ptr<const core::IDGenBase> idgenerator_;

private:

    // test: does this collide with the discriminator?
    // std::string typeid_; // yes.

    #pragma db on_delete(cascade)
    std::weak_ptr<const DBObject> parent_;
};

#pragma db view object(DBObject)
struct DBObject_type {
    #pragma db column("typeid")
    std::string discriminator_;
};


} /* storage */
} /* sempr */

#endif /* end of include guard: SEMPR_STORAGE_DBOBJECT_H_ */
