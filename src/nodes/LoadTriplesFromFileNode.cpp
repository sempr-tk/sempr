#include "nodes/LoadTriplesFromFileNode.hpp"

#include <iostream>
#include <Soprano/Soprano>
#include <Soprano/Parser>
#include <rete-rdf/Triple.hpp>

namespace sempr {


LoadTriplesFromFileNode::LoadTriplesFromFileNode(std::unique_ptr<rete::StringAccessor> filename)
    : filename_(std::move(filename))
{
}

void LoadTriplesFromFileNode::execute(
        rete::Token::Ptr token,
        rete::PropagationFlag flag,
        std::vector<rete::WME::Ptr>& inferred)
{
    if (flag == rete::PropagationFlag::ASSERT || flag == rete::PropagationFlag::UPDATE)
    {
        Soprano::RdfSerialization options[] = {
            Soprano::SerializationTurtle,
            Soprano::SerializationRdfXml,
//            Soprano::SerializationN3, // N3 parser successfully parses a non-existing file... :(
//            Soprano::SerializationNQuads, // same for NQuads...
            Soprano::SerializationNTriples
        };

        bool success = false;
        for (auto option : options)
        {
            const Soprano::Parser* p = Soprano::PluginManager::instance()->discoverParserForSerialization(option);
            if (!p) {
                std::cerr << "Soprano: No parser for option " << option << std::endl;
                continue;
            }
            auto statements = p->parseFile(
                    QString::fromStdString(filename_->getString(token)),
                    QUrl(), option);
            if (!p->lastError())
            {
                while (statements.next())
                {
                    inferred.push_back(
                        std::make_shared<rete::Triple>(
                            (*statements).subject().toN3().toStdString(),
                            (*statements).predicate().toN3().toStdString(),
                            (*statements).object().toN3().toStdString()
                        )
                    );
                }
                success = true;
                break;
            }
        } // end for: trying options

        if (!success)
        {
            std::cerr << "Soprano: Failed to parse file: " << filename_->getString(token) << std::endl;
        }
    }
}

std::string LoadTriplesFromFileNode::toString() const
{
    return "LoadTriplesFromFile(" + filename_->toString() + ")";
}


}
