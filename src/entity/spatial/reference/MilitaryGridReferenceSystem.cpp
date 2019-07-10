#include <sempr/entity/spatial/reference/MilitaryGridReferenceSystem.hpp>
#include <MilitaryGridReferenceSystem_odb.h>
#include <GeographicLib/Constants.hpp>

#include <memory>
#include <cmath>
#include <limits>

#include <boost/algorithm/string.hpp>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(MilitaryGridReferenceSystem)

MilitaryGridReferenceSystem::MilitaryGridReferenceSystem() :
    zone_(-1),
    designator_(' '),
    squareID_("")
{
}

MilitaryGridReferenceSystem::MilitaryGridReferenceSystem(const std::string& GZDSquareID) :
    MilitaryGridReferenceSystem(zoneFromGZD(GZDSquareID), designatorFromGZD(GZDSquareID), squareIDFromGZD(GZDSquareID))
{
}

MilitaryGridReferenceSystem::MilitaryGridReferenceSystem(int zone, char designator, const std::string& squareID) : 
    MilitaryGridReferenceSystem(zone, designator, squareID, new core::IDGen<MilitaryGridReferenceSystem>())
{
}

MilitaryGridReferenceSystem::MilitaryGridReferenceSystem(int zone, char designator, const std::string& squareID, const core::IDGenBase* idgen) : 
    ProjectionCS(idgen),
    zone_(zone),
    designator_(designator),
    squareID_(squareID)
{
    this->setDiscriminator<MilitaryGridReferenceSystem>();

    if ( !checkGZD(getGZDSquareID()) )
        throw ProjectionException("No valid GZDSquareID in MGRS!");
}

int MilitaryGridReferenceSystem::getZone() const
{
    return zone_;
}

bool MilitaryGridReferenceSystem::isNorth() const
{
    return (designator_ >= 'N' && designator_ <= 'Z');
}

std::string MilitaryGridReferenceSystem::getGZDSquareID() const
{
    std::string zoneStr = zone_ != 0 ? std::to_string(zone_) : "";  //0 zone for ups coordinate near polars
    return zoneStr + designator_ + squareID_;
}

bool MilitaryGridReferenceSystem::isEqual(const SpatialReference::Ptr other) const
{
    auto otherMGRS = std::dynamic_pointer_cast<MilitaryGridReferenceSystem>(other);

    if(otherMGRS)
    {
        return this->zone_ == otherMGRS->zone_ && this->designator_ == otherMGRS->designator_ && this->squareID_ == otherMGRS->squareID_;
    }
    else
    {
        return false;
    }
}

FilterPtr MilitaryGridReferenceSystem::forward() const
{
    return FilterPtr(new MGRSForwardFilter(getGZDSquareID()) );
}

FilterPtr MilitaryGridReferenceSystem::reverse() const
{
    return FilterPtr(new MGRSReversFilter(getGZDSquareID()) );
}

std::string MilitaryGridReferenceSystem::buildMGRS(const std::string& GZDSquareID, double x, double y, const std::string& separator)
{
    return GZDSquareID + separator + std::to_string(int(x)) + separator + std::to_string(int(y));
}

void MilitaryGridReferenceSystem::splitMGRS(const std::string& mgrs, std::string& GZDSquareID, double& x, double& y)
{
    std::string mgrsStr = mgrs;    //copy and hide
    boost::trim(mgrsStr);

    std::vector<std::string> results;

    // check for whitespace as seperator 
    if (mgrsStr.find(' ') != std::string::npos)
    {
        boost::split(results, mgrsStr, [](char c){return c == ' ';});
    }
    else    
    {
        // no seperator found - assume that the string is not seperated:

        // find last letter of squareID
        size_t lastLetter = std::numeric_limits<size_t>::max();

        for (size_t i = 0; i < mgrsStr.size(); i++)
        {
            if ( isLetter(mgrsStr[i]) )
            {
                lastLetter = i;
            }
        }

        if (lastLetter == std::numeric_limits<size_t>::max())
            throw ProjectionException("No valid GZD of MGRS string!");

        // split of the GZD with squareID
        std::string gzdID = mgrsStr.substr(0, lastLetter+1);
        results.push_back(gzdID);

        // split coordinates (x/y) from string
        std::string numbers =  mgrsStr.substr(lastLetter+1, mgrsStr.size());

        if (numbers.size() % 2 != 0)
            throw ProjectionException("No valid MGRS string precision!");

        // split coordinate to x and y
        if (numbers.size() > 0)
        {
            auto splitIdx = numbers.size()/2;
            results.push_back(numbers.substr(0, splitIdx));
            results.push_back(numbers.substr(splitIdx, numbers.size()));
        }
        
    }

    // check if mgrs string is splitted and valid
    if( !(results.size() == 1 || results.size() == 3) || !checkGZD(results[0]))
        throw ProjectionException("No valid MGRS string!");

    // set GZD with squareID as return value
    GZDSquareID = results[0];

    // if there are coordinates set the x and y return values, otherwise to zero
    if (results.size() == 3)
    {
        if(results[1].size() != results[2].size())
            throw ProjectionException("No valid MGRS string precision!");

        size_t prec = results[1].size();
        int prec_correct = std::pow(10, 8 - prec);    //to get a precision of 8 digitis (1mm)

        // this will fill up non high precision positions with zero.
        // ToDo: its possible the user like to get the mid point of this square and not the corner.

        x = std::stoi(results[1]) * prec_correct;
        y = std::stoi(results[2]) * prec_correct;
    }
    else
    {
        x = 0;
        y = 0;
    }
}


bool MilitaryGridReferenceSystem::checkGZD(const std::string& GZDSquareID)
{
    if (GZDSquareID.size() < 1 || GZDSquareID.size() > 5)
        return false;

    int zone = zoneFromGZD(GZDSquareID);
    if (zone < 0 || zone > 60)
        return false;

    char designator = designatorFromGZD(GZDSquareID);
    if ( !isLetter(designator) )
        return false;

    std::string squareID = squareIDFromGZD(GZDSquareID);
    if (squareID.size() > 2)
        return false;

    return true;

}

int MilitaryGridReferenceSystem::zoneFromGZD(const std::string& GZDSquareID)
{
    size_t lastNumber = std::numeric_limits<size_t>::max();

    for (size_t i = 0; i < GZDSquareID.size(); i++)
    {
        if ( isNumber(GZDSquareID[i]) )
        {
            lastNumber = i;
            continue;
        }
        else
        {
            break;
        }
    }
    
    if (lastNumber != std::numeric_limits<size_t>::max())
        return std::stoi(GZDSquareID);
    
    return 0; // there is no zone number - so it only could be a UPS position near the polars
}

char MilitaryGridReferenceSystem::designatorFromGZD(const std::string& GZDSquareID)
{
    for (size_t i = 0; i < GZDSquareID.size(); i++)
    {
        if ( isLetter(GZDSquareID[i]) )
            return GZDSquareID[i];
    }

    return ' ';
}

std::string MilitaryGridReferenceSystem::squareIDFromGZD(const std::string& GZDSquareID)
{
    size_t firstLetter = std::numeric_limits<size_t>::max();

    for (size_t i = 0; i < GZDSquareID.size(); i++)
    {
        if ( isLetter(GZDSquareID[i]) )
        {
            firstLetter = i;
            break;
        }
    }

    return GZDSquareID.substr(firstLetter+1, GZDSquareID.size());
}

bool MilitaryGridReferenceSystem::isLetter(char c)
{
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

bool MilitaryGridReferenceSystem::isNumber(char c)
{
    return (c >= '0' && c <= '9');
}



}}
