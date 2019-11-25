#include <sempr/storage/ODBStorage.hpp>
#include <DBObject_odb.h>
#include <odb/core.hxx>
#include <odb/schema-catalog.hxx>

namespace sempr { namespace storage {

ODBStorage::ODBStorage(const std::string& db_name, bool clearDatabase)
{
    db_.reset( new odb::sqlite::database(db_name,
                    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE ));
    session_.reset( new odb::session() );
    try {
        odb::transaction t(db_->begin());
        // db_->execute("PRAGMA foreign_keys=OFF");
        // odb::schema_catalog::create_schema(*db_, "", true);
        odb::schema_catalog::create_schema(*db_, "", clearDatabase);
        // db_->execute("PRAGMA foreign_keys=ON");
        t.commit();
        // odb::schema_catalog::create_schema(*db_, "", true); // drop existing
    } catch (const odb::exception& e) {
        std::cerr << e.what() << '\n';
    }
}

ODBStorage::~ODBStorage()
{
}

void ODBStorage::save( std::vector<DBObject::Ptr>& in ) {

//    std::cout << "got bulk data: " << in.size() << " entries" << std::endl;
    odb::transaction t(db_->begin());
    for(auto o : in){

        // continue if o is only an temporary object
        if(o->temporary())
            continue;

        // otherwise persist it
        try {
            // std::cout << "save " << o->id() << '\n';
            if (!o->persisted()) {
                //std::cout << "persist: " << o->id() << std::endl;
                db_->persist(o);
            } else {
                //std::cout << "update: " << o->id() << std::endl;
                db_->update(o);
            }
        } catch (odb::object_already_persistent& ex) {
            std::cout << "ODBStorage::save: object " << o->id() << " already persistent. (batch!)" << "\n";
            throw ex;
        }

    }
    t.commit();
}

void ODBStorage::save( DBObject::Ptr o ) {

    if(o->temporary())
            return;

    // std::cout << "single-save: " << o->id() << '\n';
    try {
        odb::transaction t(db_->begin());
        if (!o->persisted()) {
            // std::cout << "persist: " << o->id() << std::endl;
            db_->persist(o);
        } else {
            // std::cout << "update: " << o->id() << std::endl;
            db_->update(o);
        }
        t.commit();
    } catch (odb::object_already_persistent& ex) {
        std::cout << "ODBStorage::save: object " << o->id() << " already persistent. (single save) " << std::endl;
        throw ex;
    }
}


DBObject::Ptr ODBStorage::load( const std::string& id ) {
    odb::transaction t( db_->begin() );
    DBObject::Ptr o( db_->load<DBObject>(id) );
    t.commit();
    return o;
}

void ODBStorage::loadAll(std::vector<DBObject::Ptr> &data) {
    odb::transaction t( db_->begin() );
    odb::result<DBObject> r = db_->query<DBObject>();
    for (auto it = r.begin(); it != r.end(); ++it) {
        data.push_back(it.load());
    }
    t.commit();
}

void ODBStorage::remove(DBObject::Ptr data) {

    if(!data->persisted())
        return;

    odb::transaction t( db_->begin() );
    db_->erase(data);
    t.commit();
}

void ODBStorage::remove(std::vector<DBObject::Ptr>& data)
{
    odb::transaction t( db_->begin() );
    for (auto o : data) {
        // std::cout << "remove: " << o->id() << '\n';
        if(o->persisted())
            db_->erase(o);
    }
    t.commit();
}

}}
