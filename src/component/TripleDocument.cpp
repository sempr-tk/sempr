#include <Soprano/Soprano>
#include <Soprano/Parser>

#include <iostream>

#include "Exception.hpp"
#include "component/TripleDocument.hpp"
#include "component/TripleVector.hpp"

namespace sempr {

TripleDocument::TripleDocument(const std::string& file)
    : file_(file)
{
}

void TripleDocument::reload()
{
    // clear old data
    triples_.clear();

    // try different formats
    Soprano::RdfSerialization options[] = {
        Soprano::SerializationTurtle,
        Soprano::SerializationRdfXml,
//        Soprano::SerializationN3, // N3 parser successfully parses a non-existing file... :(
//        Soprano::SerializationNQuads, // same for NQuads...
        Soprano::SerializationNTriples
    };

    for (auto option : options)
    {
        std::cout << option << std::endl;
    }


    bool success = false;
    for (auto option : options)
    {
        const Soprano::Parser* p = Soprano::PluginManager::instance()->discoverParserForSerialization(option);
        if (!p) {
            std::cerr << "Soprano: No parser for option " << option << std::endl;
            continue;
        }

        auto statements = p->parseFile(QString::fromStdString(file_), QUrl(), option);
        if (!p->lastError())
        {
            std::cout << "able to parse file " << file_ << " with option " << option << std::endl;
            while (statements.next())
            {
                triples_.push_back({
                    (*statements).subject().toN3().toStdString(),
                    (*statements).predicate().toN3().toStdString(),
                    (*statements).object().toN3().toStdString()
                });
            }

            success = true;
            break;
        }
        else
        {
            std::cerr << p->lastError().message().toStdString() << std::endl;
        }
    }

    if (!success) throw sempr::Exception("TripleDocument could not load: " + file_);
}

void TripleDocument::setFile(const std::string& file)
{
    file_ = file;
    reload();
}

TripleIterator TripleDocument::begin() const
{
    // reuse the TripleVectorIterator, since we just use a vector<Triple>
    // internally, too.
    return TripleIterator(
               TripleVectorIterator::Ptr(
                   new TripleVectorIterator(triples_.begin())
               )
           );
}

TripleIterator TripleDocument::end() const
{
    return TripleIterator(
               TripleVectorIterator::Ptr(
                   new TripleVectorIterator(triples_.end())
               )
           );
}





}
