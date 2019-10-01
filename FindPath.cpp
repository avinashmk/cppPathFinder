#include "pch.h"

#include "FindPath.h"

#define DEBUG if (false)
#ifdef DEBUG
#include <iostream>
#include <cstdio>
#endif

#include <vector>
#include <queue>
#include <deque>
#include <iterator>

constexpr int INVALID = -1;
constexpr int CONTINUE_SEARCH = -2;


class Node
{
public:
	const int position;
	bool traversable;
	bool visitedFromStart;
	bool visitedFromTarget;
	int nextPosition;
	int previousPosition;

	Node(const int& pos, const unsigned char& value);
};

class Grid
{
	const int width;
	const int height;
	int startPos;  // TODO: make const
	int targetPos; // TODO: make const
	const int maxSteps;
	const int numberOfNodes;
	std::vector<Node> nodeList;
	std::deque<int> pathFound;

	void generateNodeList(const unsigned char* pMap);

	int calculatePosAt(int x, int y) const;
	int calculateLeftPos(const int& pos) const;
	int calculateRightPos(const int& pos) const;
	int calculateUpPos(const int& pos) const;
	int calculateDownPos(const int& pos) const;

	int getX(const int & pos) const;
	int getY(const int & pos) const;

	int getTraversablePos(int pos) const;

	int doNextFwdSearch(const int& fPos, const int& nextPos, std::queue<int>& bckSearchQ, int* pOutBuffer);
	int doNextBckSearch(const int & bPos, const int & nextPos, std::queue<int>& bckSearchQ, int * pOutBuffer);
	int setPath(const int& fMatch, const int& bMatch, int* pOutBuffer);


public:
	Grid(const int& nStartX, const int& nStartY,
		const int& nTargetX, const int& nTargetY,
		const unsigned char* pMap, const int& nMapWidth,
		const int& nMapHeight, const int& nOutBufferSize);

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
		Grid theMaze(nStartX, nStartY, nTargetX, nTargetY, pMap, nMapWidth, nMapHeight, nOutBufferSize);
		return theMaze.plotSolution(pOutBuffer);
	}
}

Node::Node(const int& pos, const unsigned char& value) : position(pos), visitedFromStart(false), visitedFromTarget(false), nextPosition(INVALID)
{
	if (static_cast<unsigned char>(1) == value) {
		traversable = true;
		DEBUG printf("Node::Node() traversable.\n");
	}
	else {
		traversable = false;
		DEBUG printf("Node::Node() NOT traversable.\n");
	}
}

Grid::Grid(const int& nStartX, const int& nStartY,
	const int& nTargetX, const int& nTargetY,
	const unsigned char* pMap, const int& nMapWidth,
	const int& nMapHeight, const int& nOutBufferSize) : width(nMapWidth), height(nMapHeight), maxSteps(nOutBufferSize), numberOfNodes(width * height)
{
	startPos = calculatePosAt(nStartX, nStartY);
	targetPos = calculatePosAt(nTargetX, nTargetY);

	generateNodeList(pMap);
}

int Grid::calculatePosAt(int x, int y) const
{
	int pos = (width * y) + x;
	return pos;
}

void Grid::generateNodeList(const unsigned char* pMap)
{
	for (int index = 0; index < numberOfNodes; ++index) {
		nodeList.emplace_back(index, *(pMap + index));
		DEBUG printf("Grid::generateNodeList() \t %d, %d\n", nodeList.back().position, nodeList.back().traversable);
	}
	nodeList.at(targetPos).traversable = true;
	DEBUG printf("Grid::generate_nodeList() \t nodeList Generated! _nodeList.size(): %d\n", static_cast<int>(nodeList.size()));
}

int Grid::plotSolution(int* pOutBuffer)
{
	DEBUG printf("Grid::plotSolution() \t\t ENTER:\n");
	// Forward Search Prepare:
	std::queue<int> fwdSearchQ;
	fwdSearchQ.push(startPos);
	nodeList.at(startPos).visitedFromStart = true;
	DEBUG printf("Grid::plotSolution() \t\t fwdSearchQ: %d\n", startPos);

	// Backward Search Prepare:
	std::queue<int> bckSearchQ;
	bckSearchQ.push(targetPos);
	nodeList.at(targetPos).visitedFromTarget = true;
	DEBUG printf("Grid::plotSolution() \t\t bckSearchQ: %d\n", targetPos);

	DEBUG printf("Grid::plotSolution() \t\t Plotting solution...\n");
	// Search start:
	while (!fwdSearchQ.empty() || !bckSearchQ.empty()) {

		// Forward Search:
		if (!fwdSearchQ.empty()) {
			int fPos = fwdSearchQ.front();
			DEBUG printf("Grid::plotSolution() \t\t fwdSearchQ: %d\n", fPos);
			if (int leftResult = doNextFwdSearch(fPos, calculateLeftPos(fPos), fwdSearchQ, pOutBuffer); leftResult != CONTINUE_SEARCH) {
				return leftResult;
			}
			if (int upResult = doNextFwdSearch(fPos, calculateUpPos(fPos), fwdSearchQ, pOutBuffer); upResult != CONTINUE_SEARCH) {
				return upResult;
			}
			if (int rightResult = doNextFwdSearch(fPos, calculateRightPos(fPos), fwdSearchQ, pOutBuffer); rightResult != CONTINUE_SEARCH) {
				return rightResult;
			}
			if (int downResult = doNextFwdSearch(fPos, calculateDownPos(fPos), fwdSearchQ, pOutBuffer); downResult != CONTINUE_SEARCH) {
				return downResult;
			}
			fwdSearchQ.pop();
		}

		// Backward Search:
		if (!bckSearchQ.empty()) {
			int bPos = bckSearchQ.front();
			DEBUG printf("Grid::plotSolution() \t\t bckSearchQ: %d\n", bPos);
			if (int leftResult = doNextBckSearch(bPos, calculateLeftPos(bPos), bckSearchQ, pOutBuffer); leftResult != CONTINUE_SEARCH) {
				return leftResult;
			}
			if (int upResult = doNextBckSearch(bPos, calculateUpPos(bPos), bckSearchQ, pOutBuffer); upResult != CONTINUE_SEARCH) {
				return upResult;
			}
			if (int rightResult = doNextBckSearch(bPos, calculateRightPos(bPos), bckSearchQ, pOutBuffer); rightResult != CONTINUE_SEARCH) {
				return rightResult;
			}
			if (int downResult = doNextBckSearch(bPos, calculateDownPos(bPos), bckSearchQ, pOutBuffer); downResult != CONTINUE_SEARCH) {
				return downResult;
			}
			bckSearchQ.pop();
		}
	}
	return INVALID;
}

int Grid::calculateLeftPos(const int& pos) const
{
	int leftPosition = INVALID;
	if (int x = getX(pos);  --x >= 0) {
		leftPosition = (width * getY(pos)) + x;
	}
	return getTraversablePos(leftPosition);
}

int Grid::calculateRightPos(const int& pos) const
{
	int rightPosition = INVALID;
	if (int x = getX(pos);  ++x < width) {
		rightPosition = (width * getY(pos)) + x;
	}
	return getTraversablePos(rightPosition);
}

int Grid::calculateUpPos(const int& pos) const
{
	int upPosition = INVALID;
	if (int y = getY(pos); --y >= 0) {
		upPosition = (width * y) + getX(pos);
	}
	return getTraversablePos(upPosition);
}

int Grid::calculateDownPos(const int& pos) const
{
	int downPosition = INVALID;
	if (int y = getY(pos);  ++y < height) {
		downPosition = (width * y) + getX(pos);
	}
	return getTraversablePos(downPosition);
}

int Grid::getX(const int& pos) const
{
	int x = pos % width;
	return x;
}

int Grid::getY(const int& pos) const
{
	int y = pos / width;
	return y;
}

int Grid::getTraversablePos(int pos) const
{
	if (pos >= 0) {
		if (nodeList.at(pos).traversable) {
			return pos;
		}
	}
	return INVALID;
}

int Grid::setPath(const int& fMatch, const int& bMatch, int* pOutBuffer)
{
	DEBUG printf("Grid::setPath() \tPath: ");
	for (int pos = fMatch; ((pos != INVALID) && (pos != startPos)); pos = nodeList.at(pos).previousPosition) {
		pathFound.push_front(pos);
	}
	for (int pos = bMatch; pos != INVALID; pos = nodeList.at(pos).nextPosition) {
		pathFound.push_back(pos);
	}
	DEBUG std::cout << std::endl;

	DEBUG printf("Grid::setPath() \tPath: ");
	if (static_cast<int>(pathFound.size()) <= maxSteps) {
		for (auto step = std::begin(pathFound); step != std::end(pathFound); pOutBuffer++, step++) {
			DEBUG printf("%d, ", *step);
			*pOutBuffer = *step;
		}
	}
	DEBUG std::cout << std::endl;
	DEBUG printf("Grid::setPath() \t bMatch.size(): %d\n", static_cast<int>(pathFound.size()));
	return static_cast<int>(pathFound.size());
}

int Grid::doNextFwdSearch(const int& fPos, const int& nextPos, std::queue<int>& fwdSearchQ, int* pOutBuffer) {
	if (nextPos != INVALID) {
		DEBUG printf("Grid::plotSolution() \t\t           , nextPos: %d\n", nextPos);
		if (Node& tmpNode = nodeList.at(nextPos); tmpNode.visitedFromTarget) {
			tmpNode.previousPosition = fPos;
			return setPath(fPos, nextPos, pOutBuffer); // solution found!
		}
		else if (nodeList.at(nextPos).visitedFromStart) {
			// do nothing, ignore.
		}
		else {
			// new node.
			tmpNode.previousPosition = fPos;
			tmpNode.visitedFromStart = true;
			fwdSearchQ.push(nextPos);
		}
	}
	return CONTINUE_SEARCH;
}

int Grid::doNextBckSearch(const int& bPos, const int& nextPos, std::queue<int>& bckSearchQ, int* pOutBuffer) {
	if (nextPos != INVALID) {
		DEBUG printf("Grid::plotSolution() \t\t           , nextPos: %d\n", nextPos);
		if (Node& tmpNode = nodeList.at(nextPos); tmpNode.visitedFromStart) {
			tmpNode.nextPosition = bPos;
			return setPath(nextPos, bPos, pOutBuffer); // solution found!
		}
		else if (nodeList.at(nextPos).visitedFromTarget) {
			// do nothing, ignore.
		}
		else {
			// new node.
			tmpNode.nextPosition = bPos;
			tmpNode.visitedFromTarget = true;
			bckSearchQ.push(nextPos);
		}
	}
	return CONTINUE_SEARCH;
}