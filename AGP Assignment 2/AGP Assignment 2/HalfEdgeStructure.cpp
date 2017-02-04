#include "HalfEdgeStructure.h"


HalfEdgeStructure::HalfEdgeStructure()
{
}

void HalfEdgeStructure::addFace(unsigned int a, unsigned int b, unsigned int c)
{
	//Create the three half edges of the triangle
	HalfEdge *firstHalfEdge = new HalfEdge(a, b);
	HalfEdge *secondHalfEdge = new HalfEdge(b, c);
	HalfEdge *thirdHalfEdge = new HalfEdge(c, a);

	//Set each one's next and previous half edge
	firstHalfEdge->next = secondHalfEdge;
	firstHalfEdge->previous = thirdHalfEdge;

	secondHalfEdge->next = thirdHalfEdge;
	secondHalfEdge->previous = firstHalfEdge;

	thirdHalfEdge->next = firstHalfEdge;
	thirdHalfEdge->previous = secondHalfEdge;

	//Add the half edges to the map along with the indices representing their edge for this face.
	m_halfEdgeMap[VertexEdgePair(a, b)] = firstHalfEdge;
	m_halfEdgeMap[VertexEdgePair(b, c)] = secondHalfEdge;
	m_halfEdgeMap[VertexEdgePair(c, a)] = thirdHalfEdge;

	/*Check to see if the edges we are currently adding have already been added but in the reverse order, 
	meaning it is the opposite half edge in an adjacent triangle. */
	if (m_halfEdgeMap.find(VertexEdgePair(b, a)) != m_halfEdgeMap.end())
	{
		HalfEdge *halfEdge = m_halfEdgeMap[VertexEdgePair(b, a)];
		halfEdge->opposite = firstHalfEdge;
		firstHalfEdge->opposite = halfEdge;
	}

	if (m_halfEdgeMap.find(VertexEdgePair(c, b)) != m_halfEdgeMap.end())
	{
		HalfEdge *halfEdge = m_halfEdgeMap[VertexEdgePair(c, b)];
		halfEdge->opposite = secondHalfEdge;
		secondHalfEdge->opposite = halfEdge;
	}

	if (m_halfEdgeMap.find(VertexEdgePair(a, c)) != m_halfEdgeMap.end())
	{
		HalfEdge *halfEdge = m_halfEdgeMap[VertexEdgePair(a, c)];
		halfEdge->opposite = thirdHalfEdge;
		thirdHalfEdge->opposite = halfEdge;
	}
}

HalfEdge* HalfEdgeStructure::getHalfEdge(const unsigned int a, const unsigned int b)
{
	VertexEdgePair pair(a, b);

	if (m_halfEdgeMap.find(pair) == m_halfEdgeMap.end())
		return nullptr;
	else
		return m_halfEdgeMap[pair];
}


HalfEdgeStructure::~HalfEdgeStructure()
{
}
