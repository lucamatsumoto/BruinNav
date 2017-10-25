#include "provided.h"
#include "SegmentMapper.cpp"
#include "AttractionMapper.cpp"
#include <string>
#include <vector>
#include <queue>
using namespace std;

class NavigatorImpl
{
public:
    NavigatorImpl();
    ~NavigatorImpl();
    bool loadMapData(string mapFile);
    NavResult navigate(string start, string end, vector<NavSegment>& directions) const;
private:
    AttractionMapper m_attractionMapper;
    SegmentMapper m_segmentMapper;
    MapLoader m_mapLoader;
    class AStarNode
    {
    public:
        GeoCoord m_gc;
        double m_g;
        double m_h;
        double m_f;
        AStarNode(AStarNode* prev, StreetSegment seg, GeoCoord gc)
        {
            m_prev = prev;
            m_seg = seg;
            m_gc = gc;
            m_f = 0;
            m_g = 0;
            m_h = 0;
        }
    private:
        AStarNode* m_prev;
        StreetSegment m_seg;
    };
    struct AStarNodeGreaterThan
    {
        bool operator()(AStarNode a, AStarNode b)
        {
            return(a.m_g + a.m_h) < (b.m_g + b.m_h);
        }
    };
    bool gcEquals(const GeoCoord& g1, const GeoCoord& g2) const
    {
        if(g1.latitudeText == g2.latitudeText &&  g1.longitudeText == g2.longitudeText)
        {
            return true;
        }
        return false;
    }
    static bool lessCost (const AStarNode& lhs, const AStarNode& rhs)
    {
        return (lhs.m_f < rhs.m_f);
    }
    void insertAStarNode(vector<AStarNode>* vec, AStarNode node) const
    {
        vector<AStarNode>::iterator it = lower_bound(vec->begin(), vec->end(), node, lessCost);
        vec->insert(it, node);
    }
};

NavigatorImpl::NavigatorImpl()
{
}

NavigatorImpl::~NavigatorImpl()
{
    
}

bool NavigatorImpl::loadMapData(string mapFile)
{
    if(m_mapLoader.load(mapFile))
    {
        m_attractionMapper.init(m_mapLoader);
        m_segmentMapper.init(m_mapLoader);
        return true;
    }
	return false;  // This compiles, but may not be correct
}

NavResult NavigatorImpl::navigate(string start, string end, vector<NavSegment> &directions) const
{
    vector<AStarNode> openList;
    vector<AStarNode> closedList;
    GeoCoord gc;
    bool goodSource = m_attractionMapper.getGeoCoord(start, gc);
    if(goodSource == false)
    {
        return NAV_BAD_SOURCE;
    }
    vector<StreetSegment> startSegments = m_segmentMapper.getSegments(gc);
    if(startSegments.empty())
    {
        return NAV_BAD_SOURCE;
    }
    bool goodDest = m_attractionMapper.getGeoCoord(end, gc);
    if(goodDest == false)
    {
        return NAV_BAD_DESTINATION;
    }
    vector<StreetSegment> endSegments = m_segmentMapper.getSegments(gc);
    if(endSegments.empty())
    {
        return NAV_BAD_DESTINATION;
    }
    //TODO:: CHECK SIZE?
    
    AStarNode node1(nullptr, startSegments[0], gc);
    insertAStarNode(&openList, node1);
    while(!openList.empty())
    {
        AStarNode q = openList[0];
        openList.erase(openList.begin());
        vector<StreetSegment> connectedSegments = m_segmentMapper.getSegments(q.m_gc);
        vector<AStarNode> successors;
        for(int i = 0; connectedSegments.size(); i++)
        {
            StreetSegment seg = connectedSegments[i];
            if(gcEquals(seg.segment.start, q.m_gc))
            {
                AStarNode end(nullptr, seg, seg.segment.end);
                successors.push_back(end);
            }
            else if(gcEquals(seg.segment.end, q.m_gc))
            {
                AStarNode start(nullptr, seg, seg.segment.start);
                successors.push_back(start);
            }
            else
            {
                AStarNode start(nullptr, seg, seg.segment.start);
                AStarNode end(nullptr, seg, seg.segment.end);
                successors.push_back(start);
                successors.push_back(end);
            }
        }
        for(int i = 0; i < successors.size(); i++)
        {
            AStarNode successor = successors[i];
            if(gcEquals(successor.m_gc, gc))
            {
                //STORE RESULTS IN VECTOR FOR NAVIGATION
                return NAV_SUCCESS;
            }
            successor.m_g = q.m_g + distanceEarthKM(q.m_gc, successor.m_gc);
            successor.m_h = distanceEarthKM(gc, successor.m_gc);
            successor.m_f = successor.m_g + successor.m_h;
            bool skipSuccessor = false;
            for(int i = 0; i < openList.size(); i++)
            {
                if(gcEquals(successor.m_gc, openList[i].m_gc) && openList[i].m_f < successor.m_f)
                {
                    skipSuccessor = true;
                    break;
                }
                for(int i = 0; i < closedList.size(); i++)
                {
                    if(gcEquals(successor.m_gc, closedList[i].m_gc) && closedList[i].m_f < successor.m_f)
                    {
                        skipSuccessor = true;
                        break;
                    }
                }
            }
            if(skipSuccessor)
            {
                continue;
            }
            insertAStarNode(&openList, successor);
        }
        closedList.push_back(q);
    }
	return NAV_NO_ROUTE;  // This compiles, but may not be correct
}

//******************** Navigator functions ************************************

// These functions simply delegate to NavigatorImpl's functions.
// You probably don't want to change any of this code.

Navigator::Navigator()
{
    m_impl = new NavigatorImpl;
}

Navigator::~Navigator()
{
    delete m_impl;
}

bool Navigator::loadMapData(string mapFile)
{
    return m_impl->loadMapData(mapFile);
}

NavResult Navigator::navigate(string start, string end, vector<NavSegment>& directions) const
{
    return m_impl->navigate(start, end, directions);
}
