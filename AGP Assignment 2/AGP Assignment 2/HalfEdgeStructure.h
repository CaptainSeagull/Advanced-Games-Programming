#pragma once

#include "HalfEdge.h"

#include <map>

typedef std::pair<unsigned int, unsigned int> VertexEdgePair;

class HalfEdgeStructure
{
private:
	std::map<VertexEdgePair, HalfEdge*> m_halfEdgeMap;
public:
	HalfEdgeStructure();
	void addFace(unsigned int a, unsigned int b, unsigned int c);
	HalfEdge* getHalfEdge(const unsigned int a, const unsigned int b);
	~HalfEdgeStructure();
};

