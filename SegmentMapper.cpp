#include "provided.h"
#include "MyMap.h"
#include <vector>
using namespace std;

class SegmentMapperImpl
{
public:
	SegmentMapperImpl();
	~SegmentMapperImpl();
	void init(const MapLoader& ml);
	vector<StreetSegment> getSegments(const GeoCoord& gc) const;
private:
    MyMap<GeoCoord, vector<StreetSegment>> m_segs;
    bool findStreetSegment(vector<StreetSegment> v, StreetSegment seg)
    {
        for (int i=0; i < v.size(); ++i)
        {
            if (seg.streetName == v[i].streetName &&
                seg.segment.start.latitude == v[i].segment.start.latitude &&
                seg.segment.start.longitude == v[i].segment.start.longitude &&
                seg.segment.end.latitude == v[i].segment.end.latitude &&
                seg.segment.end.longitude == v[i].segment.end.longitude
                )
            {
                return true;
            }
        }
        return false;
    }
    void addSegment(GeoCoord gc, StreetSegment seg)
    {
        vector<StreetSegment>* segptr = m_segs.find(gc);
        if(segptr == nullptr)
        {
            vector<StreetSegment> segs;
            segs.push_back(seg);
            m_segs.associate(gc, segs);
        }
        if (!findStreetSegment(*segptr, seg))
        {
            segptr->push_back(seg);
        }
    }
};

SegmentMapperImpl::SegmentMapperImpl()
{
}

SegmentMapperImpl::~SegmentMapperImpl()
{
}

void SegmentMapperImpl::init(const MapLoader& ml)
{
    for(int i = 0; i < ml.getNumSegments(); i++)
    {
        StreetSegment seg;
        ml.getSegment(i, seg);
        addSegment(seg.segment.start, seg);
        addSegment(seg.segment.end, seg);
        for(int i = 0; i < seg.attractions.size(); i++)
        {
            addSegment(seg.attractions[i].geocoordinates, seg);
        }
    }
}

vector<StreetSegment> SegmentMapperImpl::getSegments(const GeoCoord& gc) const
{
    vector<StreetSegment> segments = *m_segs.find(gc);
    if(segments.empty())
    {
        vector<StreetSegment> segs;
        return segs;
    }
	return segments;  // This compiles, but may not be correct
}

//******************** SegmentMapper functions ********************************

// These functions simply delegate to SegmentMapperImpl's functions.
// You probably don't want to change any of this code.

SegmentMapper::SegmentMapper()
{
	m_impl = new SegmentMapperImpl;
}

SegmentMapper::~SegmentMapper()
{
	delete m_impl;
}

void SegmentMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

vector<StreetSegment> SegmentMapper::getSegments(const GeoCoord& gc) const
{
	return m_impl->getSegments(gc);
}
