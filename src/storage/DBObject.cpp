#include <sempr/storage/DBObject.hpp>
#include <DBObject_odb.h>

namespace sempr { namespace storage {

DBObject::DBObject()
    : DBObject(new core::IDGen<DBObject>())
{
}

DBObject::DBObject(const core::IDGenBase* idgen, bool temporary)
    : persisted_(false), temporary_(temporary), idgenerator_(idgen)
{
    id_ = idgen->generate();
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
        default:
            // nothing
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
        default:
            // nothing
            break;
    }
}


void DBObject::prePersist(odb::database& db) const {}
void DBObject::postPersist(odb::database& db) const { persisted_ = true; }
void DBObject::preUpdate(odb::database& db) const {}
void DBObject::postUpdate(odb::database& db) const {}
void DBObject::preErase(odb::database& db) const {}
void DBObject::postErase(odb::database& db) const {}
void DBObject::preLoad(odb::database& db)
{
    persisted_ = true;

    // an ID has been generated, but we already have one (that's going to be
    // assinged between pre- and postLoad). Hence, revoke the current one, to
    // "free" it.
    idgenerator_->revoke(this->id_);
}

void DBObject::postLoad(odb::database& db)
{
    // query for the discriminator
    DBObject_type t = db.query_value<DBObject_type>(odb::query<DBObject_type>::id == id_);
    discriminator_ = t.discriminator_;
}

} /* storage */

} /* sempr */
