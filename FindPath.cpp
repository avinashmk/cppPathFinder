#include "pch.h"
#include "FindPath.h"

#include <vector>
#include <algorithm>
#include <iterator>
#include <iostream>

#include <cstdio>
#include <cmath>

#define NOT_EXISTS -1
#define NOT_TRAVERSABLE -1
#define MAX_STEPS_REACHED -2
#define ALREADY_TRAVERSED -3

class Node
{
public:
	int _pos;
	bool _traversable;
	int _left;
	int _right;
	int _up;
	int _down;

	Node(const unsigned char value, int position);
};

class Grid
{
	int _width;
	int _height;

	int _nrOfNodes;
	int _startPos;
	int _targetPos;

	int _maxSteps;
	int _minSteps;

	bool _pathFound;
	std::vector<int> _route;
	std::vector<Node*> _nodeList;

	void generateNodeList(const unsigned char* pMap);

	int getAtPos(int x, int y);
	int getLeftPos(int x, int y);
	int getRightPos(int x, int y);
	int getUpPos(int x, int y);
	int getDownPos(int x, int y);

	int getTraversablePos(int pos);

	int getX(int pos); // still needed??
	int getY(int pos); // still needed??

	int plotRoute(const int presentPos, std::vector<int> route);
	void setPathFound(std::vector<int> route);

public:
	Grid(const int nStartX, const int nStartY,
		const int nTargetX, const int nTargetY,
		const unsigned char* pMap, const int nMapWidth,
		const int nMapHeight, const int nOutBufferSize);

	int plotSolution(int* pOutBuffer);
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
		Grid grid(nStartX, nStartY, nTargetX, nTargetY, pMap, nMapWidth, nMapHeight, nOutBufferSize);
		return grid.plotSolution(pOutBuffer);
	}
}

Node::Node(const unsigned char traversable, int position) : _pos(position), _traversable(false)
{
	if (static_cast<unsigned char>(1) == traversable) {
		_traversable = true;
		////printf("Node::Node() traversable.\n");
	}
	else {
		////printf("Node::Node() NOT traversable.\n");
	}
}

Grid::Grid(const int nStartX, const int nStartY,
	const int nTargetX, const int nTargetY,
	const unsigned char* pMap, const int nMapWidth,
	const int nMapHeight, const int nOutBufferSize) : _width(nMapWidth), _height(nMapHeight), _maxSteps(nOutBufferSize), _pathFound(false)
{
	_nrOfNodes = _width * _height;
	_startPos = getAtPos(nStartX, nStartY);
	_targetPos = getAtPos(nTargetX, nTargetY);
	_minSteps = (std::abs(nStartX - nTargetX) + std::abs(nStartY - nTargetY));

	generateNodeList(pMap);
}

void Grid::generateNodeList(const unsigned char* pMap)
{
	for (int index = 0; index < _nrOfNodes; ++index) {
		_nodeList.push_back(new Node(*(pMap + index), index));
	}
	_nodeList.shrink_to_fit();

	for (int y = 0; y < _height; y++) {
		for (int x = 0; x < _width; x++) {
			Node* tmpNode = _nodeList.at(getAtPos(x, y));
			tmpNode->_left = getTraversablePos(getLeftPos(x, y));
			tmpNode->_right = getTraversablePos(getRightPos(x, y));
			tmpNode->_up = getTraversablePos(getUpPos(x, y));
			tmpNode->_down = getTraversablePos(getDownPos(x, y));

			//printf("Grid::generate_nodeList() \tNode: %d, left: %d, right: %d, up: %d, down: %d\n", 
			//	tmpNode->_pos, tmpNode->_left, tmpNode->_right, tmpNode->_up, tmpNode->_down);
		}
	}
	//printf("Grid::generate_nodeList() \t _nodeList Generated! _nodeList.size(): %d\n", _nodeList.size());
}

int Grid::getLeftPos(int x, int y)
{
	int pos = NOT_EXISTS;

	if (--x >= 0) {
		pos = (_width * y) + x;
	}

	return pos;
}

int Grid::getRightPos(int x, int y)
{
	int pos = NOT_EXISTS;

	if (++x < _width) {
		pos = (_width * y) + x;
	}

	return pos;
}

int Grid::getUpPos(int x, int y)
{
	int pos = NOT_EXISTS;

	if (--y >= 0) {
		pos = (_width * y) + x;
	}

	return pos;
}

int Grid::getDownPos(int x, int y)
{
	int pos = NOT_EXISTS;

	if (++y < _height) {
		pos = (_width * y) + x;
	}

	return pos;
}

int Grid::getTraversablePos(int pos)
{
	if (pos >= 0) {
		if ((_nodeList.at(pos)->_traversable) || (pos == _targetPos)) {
			return pos;
		}
	}
	return NOT_TRAVERSABLE;
}

int Grid::getX(int pos)
{
	int x = pos % _width;
	return x;
}

int Grid::getY(int pos)
{
	int y = pos / _width;
	return y;
}

int Grid::getAtPos(int x, int y)
{
	int pos = (_width * y) + x;
	return pos;
}

int Grid::plotSolution(int* pOutBuffer)
{
	//printf("Grid::findPath() \t\t Plotting route from %d to %d\n", _startPos, _targetPos);
	int result = plotRoute(_startPos, _route);
	
	if (_pathFound) {
		_route.erase(std::begin(_route));
		//printf("Grid::findPath() \t\t route.size(): %d\n", _route.size());
		for (auto step = std::begin(_route); step != std::end(_route); pOutBuffer++, step++) {
			*pOutBuffer = *step;
		}
		return static_cast<int>(_route.size());
	}
	return -1;
}

int Grid::plotRoute(const int presentPos, std::vector<int> route) //member function const so that class varibals remain unaffected.
{
	//printf("Grid::plotRoute() \t\t ENTER: %d\n", presentPos);

	if (auto found = std::find(std::begin(route), std::end(route), presentPos); found != std::end(route)) {
		//printf("Grid::plotRoute() \t\t Already visited: %d\n", presentPos);
		return ALREADY_TRAVERSED;
	}
	route.push_back(presentPos);

	if (presentPos == _targetPos) { // always false for the first iteration.
		//printf("Grid::plotRoute() \t\t Route Plotted! %d\n", route.size());
		if (!(_pathFound && (route.size() > _route.size()))) { // Update in all cases except: Present path is longer than found path.
			setPathFound(route);
		}
		return static_cast<int>(route.size()); // POSITIVE EXIT CONDITION
	}

	if (route.size() == _maxSteps) {
		return MAX_STEPS_REACHED; // NEGATIVE EXIT CONDITION
	}

	if (int leftPos = _nodeList.at(presentPos)->_left; leftPos >= 0) {
		//printf("Grid::plotRoute() \t\t leftPos: %d\n", leftPos);
		if (int result = plotRoute(leftPos, route); result > 0) {
			//printf("Grid::plotRoute() \t\t Route Plotted! Left: %d\n", route.size());
			if ((route.size() - 1) == _minSteps) {
				return result; // POSITIVE EXIT CONDITION
			}
		}
		else {
			//printf("Grid::plotRoute() \t\t pop_back leftPos\n");
		}

	}

	if (int rightPos = _nodeList.at(presentPos)->_right; rightPos >= 0) {
		//printf("Grid::plotRoute() \t\t rightPos: %d\n", rightPos);
		if (int result = plotRoute(rightPos, route); result > 0) {
			//printf("Grid::plotRoute() \t\t Route Plotted! Right: %d\n", route.size());
			if ((route.size() - 1) == _minSteps) {
				return result; // POSITIVE EXIT CONDITION
			}
		}
		else {
			//printf("Grid::plotRoute() \t\t pop_back rightPos\n");
		}
	}

	if (int upPos = _nodeList.at(presentPos)->_up; upPos >= 0) {
		//printf("Grid::plotRoute() \t\t upPos: %d\n", upPos);
		if (int result = plotRoute(upPos, route); result > 0) {
			//printf("Grid::plotRoute() \t\t Route Plotted! Up: %d\n", route.size());
			if ((route.size() - 1) == _minSteps) {
				return result; // POSITIVE EXIT CONDITION
			}
		}
		else {
			//printf("Grid::plotRoute() \t\t pop_back upPos\n");
		}
	}

	if (int downPos = _nodeList.at(presentPos)->_down; downPos >= 0) {
		//printf("Grid::plotRoute() \t\t downPos: %d\n", downPos);
		if (int result = plotRoute(downPos, route); result > 0) {
			//printf("Grid::plotRoute() \t\t Route Plotted! Down: %d\n", route.size());
			if ((route.size() - 1) == _minSteps) {
				return result; // POSITIVE EXIT CONDITION
			}
		}
		else {
			//printf("Grid::plotRoute() \t\t pop_back downPos\n");
		}
	}

	return NOT_EXISTS; // NEGATIVE EXIT CONDITION
}

void Grid::setPathFound(std::vector<int> route)
{
	_pathFound = true;
	_route = std::move(route);
}
