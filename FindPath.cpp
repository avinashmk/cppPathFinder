#include "pch.h"
#include "FindPath.h"

#include <vector>
#include <cstdio>
#include <iostream>

class Node
{
	int _x;
	int _y;
	int _pos;
	bool _traversable;

public:
	Node(const unsigned char value, int position);
	Node* _left;
	Node* _right;
	Node* _up;
	Node* _down;
	bool isTraversable();
};

class Grid
{
	int _width;
	int _height;
	int _nrOfNodes;
	Node* _origin;
	std::vector<Node*> nodeList;

	void generateGrid(const unsigned char* pMap);
	int getAtPos(int x, int y);
	int getLeftPos(int x, int y);
	int getRightPos(int x, int y);
	int getUpPos(int x, int y);
	int getDownPos(int x, int y);


public:
	Grid(const int width, const int height, const unsigned char * pMap);
	int findPath(const int nStartX, const int nStartY,
		const int nTargetX, const int nTargetY,
		int* pOutBuffer, const int nOutBufferSize);
};

int FindPath(const int nStartX, const int nStartY,
	const int nTargetX, const int nTargetY,
	const unsigned char* pMap, const int nMapWidth, const int nMapHeight,
	int* pOutBuffer, const int nOutBufferSize)
{
	if ((nStartX == nTargetX) && (nStartY == nTargetY)) {
		return 0;
	}
	else {
		Grid grid(nMapWidth, nMapHeight, pMap);
		return grid.findPath(nStartX, nStartY, nTargetX, nTargetY, pOutBuffer, nOutBufferSize);
	}
}

Node::Node(const unsigned char value, int position) : _pos(position), _traversable(false)
{
	if (static_cast<unsigned char>(1) == value) {
		_traversable = true;
		//printf("Node::Node() traversable.\n");
	}
	else {
		//printf("Node::Node() NOT traversable.\n");
	}
}

bool Node::isTraversable()
{
	return _traversable;
}

Grid::Grid(const int width, const int height, const unsigned char* pMap) : _width(width), _height(height)
{
	_nrOfNodes = _width * _height;
	generateGrid(pMap);
}

void Grid::generateGrid(const unsigned char* pMap)
{
	for (int index = 0; index < _nrOfNodes; ++index) {
		Node* tmpNode = new Node(*(pMap + index), index);
		nodeList.push_back(tmpNode);
	}
	nodeList.shrink_to_fit();

	int index = 0;
	for (int y = 0; y < _height; y++) {
		for (int x = 0; x < _width; x++) {
			Node* tmpNode = nodeList.at(getAtPos(x, y));

			int leftPos = getLeftPos(x, y);
			if (leftPos > 0) {
				tmpNode->_left = nodeList.at(leftPos);
			}

			int rightPos = getRightPos(x, y);
			if (rightPos > 0) {
				tmpNode->_right = nodeList.at(rightPos);
			}

			int upPos = getUpPos(x, y);
			if (upPos > 0) {
				tmpNode->_up = nodeList.at(upPos);
			}

			int downPos = getDownPos(x, y);
			if (downPos > 0) {
				tmpNode->_down = nodeList.at(downPos);
			}
		}
	}
	printf("Grid::generateGrid() \t Grid Generated! nodeList.size(): %d\n", nodeList.size());

}

int Grid::getLeftPos(int x, int y)
{
	int pos = -1;

	if (--x) {
		pos = (4 * y) + x;
	}

	return pos;
}

int Grid::getRightPos(int x, int y)
{
	int pos = -1;

	if (++x < _width) {
		pos = (4 * y) + x;
	}

	return pos;
}

int Grid::getUpPos(int x, int y)
{
	int pos = -1;

	if (--y) {
		pos = (4 * y) + x;
	}

	return pos;
}

int Grid::getDownPos(int x, int y)
{
	int pos = -1;

	if (++y < _height) {
		pos = (4 * y) + x;
	}

	return pos;
}

int Grid::getAtPos(int x, int y)
{
	int pos = (4 * y) + x;
	return pos;
}

int Grid::findPath(const int nStartX, const int nStartY, const int nTargetX, const int nTargetY, int* pOutBuffer, const int nOutBufferSize)
{
	return -2;
}
