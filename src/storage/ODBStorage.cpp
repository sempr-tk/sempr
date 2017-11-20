#include <sempr/storage/ODBStorage.hpp>
#include <DBObject_odb.h>
#include <odb/core.hxx>
#include <odb/schema-catalog.hxx>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

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


void ODBStorage::save( DBObject::Ptr o ) {
    odb::transaction t(db_->begin());

    if (!o->persisted()) {
        setID(o, boost::uuids::random_generator()());
        db_->persist(o);
    } else {
        db_->update(o);
    }

    t.commit();
}


DBObject::Ptr ODBStorage::load( const boost::uuids::uuid& id ) {
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
    odb::transaction t( db_->begin() );
    db_->erase(data);
    t.commit();
}

}}
