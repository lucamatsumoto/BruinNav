#include "provided.h"
#include <string>
#include "MyMap.h"
#include <cctype>
using namespace std;

class AttractionMapperImpl
{
public:
	AttractionMapperImpl();
	~AttractionMapperImpl();
	void init(const MapLoader& ml);
	bool getGeoCoord(string attraction, GeoCoord& gc) const;
private:
    MyMap<string, GeoCoord> m_attractions;
    string toLower(string& orig)
    {
        string lc;
        lc.resize(orig.size());
        transform(orig.cbegin(), orig.cend(), lc.begin(), tolower);
        return lc;
    }
};

AttractionMapperImpl::AttractionMapperImpl()
{
}

AttractionMapperImpl::~AttractionMapperImpl()
{
}

void AttractionMapperImpl::init(const MapLoader& ml)
{
    StreetSegment seg;
    for(int i = 0; i < ml.getNumSegments(); i++)
    {
        ml.getSegment(i, seg);
        vector<Attraction> attractions = seg.attractions;
        for(int i = 0; i < attractions.size(); i++)
        {
            m_attractions.associate(toLower(attractions[i].name), attractions[i].geocoordinates);
        }   
    }
    //iterate through all segments from the MapLoader object
}

bool AttractionMapperImpl::getGeoCoord(string attraction, GeoCoord& gc) const
{
    const GeoCoord* g = m_attractions.find(attraction);
    if(g != nullptr)
    {
        gc = *g;
        return true;
    }
	return false;  // This compiles, but may not be correct
}

//******************** AttractionMapper functions *****************************

// These functions simply delegate to AttractionMapperImpl's functions.

AttractionMapper::AttractionMapper()
{
	m_impl = new AttractionMapperImpl;
}

AttractionMapper::~AttractionMapper()
{
	delete m_impl;
}

void AttractionMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

bool AttractionMapper::getGeoCoord(string attraction, GeoCoord& gc) const
{
	return m_impl->getGeoCoord(attraction, gc);
}
