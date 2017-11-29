#include <sempr/storage/DBObject.hpp>
#include <DBObject_odb.h>

namespace sempr { namespace storage {

DBObject::DBObject(DBObject::Ptr parent)
    : parent_(parent), persisted_(false)
{
    // use a default id generation-parameterization
    id_ = core::IDGen<DBObject>().generate();
    setDiscriminator<DBObject>();
}

DBObject::DBObject(const core::IDGenBase& idgen, DBObject::Ptr parent)
    : parent_(parent), persisted_(false)
{
    id_ = idgen.generate();
    setDiscriminator<DBObject>();
}


void DBObject::dbcallback(odb::callback_event e, odb::database &db)
{
    switch (e) {
        case odb::callback_event::pre_load:
            preLoad(db);
            break;
        case odb::callback_event::post_load:
            postLoad(db);
            break;
    }

}

const std::string DBObject::discriminator() const {
    return discriminator_;
}


void DBObject::dbcallback(odb::callback_event e, odb::database &db) const
{
    switch (e) {
        case odb::callback_event::pre_persist:
            prePersist(db);
            break;
        case odb::callback_event::post_persist:
            postPersist(db);
            break;
        case odb::callback_event::pre_update:
            preUpdate(db);
            break;
        case odb::callback_event::post_update:
            postUpdate(db);
            break;
        case odb::callback_event::pre_erase:
            preErase(db);
            break;
        case odb::callback_event::post_erase:
            postErase(db);
            break;
    }
}


void DBObject::prePersist(odb::database& db) const {}
void DBObject::postPersist(odb::database& db) const { persisted_ = true; }
void DBObject::preUpdate(odb::database& db) const {}
void DBObject::postUpdate(odb::database& db) const {}
void DBObject::preErase(odb::database& db) const {}
void DBObject::postErase(odb::database& db) const {}
void DBObject::preLoad(odb::database& db) { persisted_ = true; }

void DBObject::postLoad(odb::database& db)
{
    // query for the discriminator
    DBObject_type t = db.query_value<DBObject_type>(odb::query<DBObject_type>::id == id_);
    discriminator_ = t.discriminator_;
}

} /* storage */

} /* sempr */
