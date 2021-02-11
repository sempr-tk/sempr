#include "Core.hpp"
#include "ECWME.hpp"
#include "Component.hpp"
#include "Exception.hpp"
#include "SimpleIncrementalIDGenerator.hpp"

#include <rete-reasoner/AssertedEvidence.hpp>
#include <QPluginLoader>
#include <QDir>
#include <cstdlib>

namespace sempr {

Core::Core(IDGenerator::Ptr idgen, Storage::Ptr storage)
    : idgen_(idgen), storage_(storage)
{
}

Core::Core(IDGenerator::Ptr idgen)
    : Core(idgen, nullptr)
{
}

Core::Core() : Core(std::make_shared<SimpleIncrementalIDGenerator>())
{
}

Core::~Core()
{
    for (auto entity : entities_)
    {
        entity->core_ = nullptr;
    }
}


void Core::loadPlugins()
{

    // if a SEMPR_PLUGINS_PATH is given, just use that.
    char* var = std::getenv("SEMPR_PLUGINS_PATH");
    if (var)
    {
        std::string path(var);
        std::cout << "Loading plugins from SEMPR_PLUGINS_PATH " << path << std::endl;
        loadPlugins(path);
        return;
    }
    else
    {
        std::cerr << "SEMPR_PLUGINS_PATH not set, searching LD_LIBRARY_PATH" << std::endl;
        // search for a sempr_plugins folder in the paths in the PATH variable
        var = std::getenv("LD_LIBRARY_PATH");
        if (var)
        {
            // split path by ':'
            auto parts = QString::fromStdString(std::string(var)).split(":", QString::SkipEmptyParts);

            for (auto path : parts)
            {
                std::cout << ".." << path.toStdString() << std::endl;
                QDir dir(path);
                if (dir.exists("sempr_plugins"))
                {
                    std::cout << "found sempr_plugins folder in " << dir.absolutePath().toStdString() << std::endl;
                    loadPlugins(dir.absoluteFilePath("sempr_plugins").toStdString());
                    return;
                }
            }

            std::cerr << "Couldn't find 'sempr_plugins' in any of the paths in the LD_LIBRARY_PATH variable." << std::endl;
        }
        else
        {
            std::cerr << "LD_LIBRARY_PATH variable not set?" << std::endl;
        }
    }

    // last resort: try standard installation path, /usr/lib/sempr_plugins
    std::cout << "Trying /usr/lib/sempr_plugins..." << std::endl;
    if (QDir("/usr/lib/sempr_plugins").exists())
        loadPlugins("/usr/lib/sempr_plugins");
    else
        throw std::runtime_error("Could not find a sempr_plugins folder");

}


void Core::loadPlugins(const std::string& path)
{
    QDir pluginDir(QString::fromStdString(path));

    auto list = pluginDir.entryList(QDir::Files | QDir::NoSymLinks);
    std::cout << "Loading plugins. " << list.size() << " candidates..." << std::endl;
    for (auto fname : list)
    {
        QPluginLoader loader(pluginDir.absoluteFilePath(fname));
        QObject* plugin = loader.instance();
        if (plugin)
        {
#ifdef QT4
            auto name = ""; // no loader.mataData() in Qt4 ...
#else
            auto name = loader.metaData()["className"].toString().toStdString();
#endif

            CapabilityInterface* cap = qobject_cast<CapabilityInterface*>(plugin);
            if (cap)
            {
                if (capabilities_.find(cap) != capabilities_.end())
                {
                    std::cout << "Plugin " << name
                              << " provided by " << loader.fileName().toStdString()
                              << " has already been loaded before." << std::endl;
                }
                else
                {
                    std::cout << "Loaded plugin " << name << " from "
                              << loader.fileName().toStdString() << std::endl;
                    capabilities_.insert(cap);
                    cap->setup(this);
                }
            }
            else
            {
                std::cout << loader.fileName().toStdString() << " is not a capability" << std::endl;
                loader.unload();
            }
        }
        else
        {
            // std::cout << fname.toStdString() << " is not a plugin" << std::endl;
            std::cout << loader.errorString().toStdString() << std::endl;
        }
    }
}


std::recursive_mutex& Core::reasonerMutex()
{
    return reasonerMutex_;
}

rete::Reasoner& Core::reasoner()
{
    return reasoner_;
}


rete::RuleParser& Core::parser()
{
    return ruleParser_;
}

std::vector<size_t> Core::addRules(const std::string& ruleString)
{
    std::lock_guard<std::recursive_mutex> lg(reasonerMutex_);
    auto newRules = ruleParser_.parseRules(ruleString, reasoner_.net());

    std::vector<size_t> ids;
    for (auto r : newRules)
    {
        // for the result
        ids.push_back(r->id());

        // but also, store the rule permanently
        rules_.push_back(r);
    }
    return ids;
}

void Core::removeRule(size_t id)
{
    auto it = std::find_if(rules_.begin(), rules_.end(),
        [id](rete::ParsedRule::Ptr& rule) -> bool
        {
            return rule->id() == id;
        });

    if (it != rules_.end())
    {
        (*it)->disconnect();
        rules_.erase(it);
    }
}

std::vector<rete::ParsedRule::Ptr> Core::rules()
{
    return rules_;
}


void Core::performInference()
{
    std::lock_guard<std::recursive_mutex> lock(reasonerMutex_);
    reasoner_.performInference();
}

Storage::Ptr Core::storage()
{
    return storage_;
}

void Core::addEntity(Entity::Ptr entity)
{
    // add it to the core if the entity wasn't added to one before
    if (entity->core_)
    {
        throw sempr::Exception("Entity already present in a Core!");
    }
    entity->core_ = this;
    this->entities_.insert(entity);


    // assign an ID if the entity does not have one, yet
    if (entity->id().empty())
    {
        entity->setId(idgen_->createIDFor(entity));
    }

    // Persist the entity
    if (storage_) storage_->save(entity);

    // add the WMEs to the reasoner
    auto components = entity->getComponents<Component>();
    for (auto& c : components)
    {
        addedComponent(entity, c);
    }
}


Entity::Ptr Core::getEntity(const std::string& id)
{
    for (auto e : entities_)
    {
        if (e->id() == id) return e;
    }
    return nullptr;
}


void Core::removeEntity(Entity::Ptr entity)
{
    if (entity->core_ != this)
    {
        throw sempr::Exception("Entity not part of this Core");
    }

    // Un-persist entity
    if (storage_) storage_->remove(entity);

    // remove corresponding WMEs
    auto evidence = std::make_shared<rete::AssertedEvidence>(entity->id());

    {
        std::lock_guard<std::recursive_mutex> lock(reasonerMutex_);
        reasoner_.removeEvidence(evidence);
    }

    this->entities_.erase(entity);
    entity->core_ = nullptr;
}

void Core::addedComponent(Entity::Ptr entity,
                          Component::Ptr component)
{
    // update storage
    if (storage_) storage_->save(entity, component);

    auto evidence = std::make_shared<rete::AssertedEvidence>(entity->id());
    auto wme = std::make_shared<ECWME>(entity, component);

    {
        std::lock_guard<std::recursive_mutex> lock(reasonerMutex_);
        reasoner_.addEvidence(wme, evidence);
    }
}

void Core::removedComponent(Entity::Ptr entity,
                          Component::Ptr component)
{
    // Un-persist component
    if (storage_) storage_->remove(entity, component);

    auto evidence = std::make_shared<rete::AssertedEvidence>(entity->id());
    auto wme = std::make_shared<ECWME>(entity, component);

    {
        std::lock_guard<std::recursive_mutex> lock(reasonerMutex_);
        reasoner_.removeEvidence(wme, evidence);
    }
}

void Core::changedComponent(Entity::Ptr entity,
                          Component::Ptr component)
{
    // update storage
    if (storage_) storage_->save(entity, component);

    // NOTE: You may have noticed that there are often new evidences and WMEs
    // created, even for "removedComponent" and "changedComponent". This is okay
    // since the WMEs evaluate to be identical (as they point to the same
    // entities and components), and the rete engine handles this case in the
    // AlphaMemories by propagating the pointer to the WME that was first
    // added, and discards the new wme. The reasoner also compares WMEs by value
    // to see if they are backed by evidence etc., so we should be good.

    auto wme = std::make_shared<ECWME>(entity, component);
    {
        std::lock_guard<std::recursive_mutex> lock(reasonerMutex_);
        reasoner_.net().getRoot()->activate(wme, rete::PropagationFlag::UPDATE);
    }
}

} /* sempr */
