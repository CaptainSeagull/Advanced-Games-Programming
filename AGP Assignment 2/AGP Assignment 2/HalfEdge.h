#pragma once

class HalfEdge
{
public:
	HalfEdge *previous = nullptr, *next = nullptr, *opposite = nullptr;
	unsigned int first, second;

	HalfEdge(unsigned int first, unsigned int second) : first(first), second(second){}
	~HalfEdge(){}
};

