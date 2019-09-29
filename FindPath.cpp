#include "pch.h"

#include "FindPath.h"

#include <iostream>
#include <cstdio>

#include <vector>
#include <queue>
#include <iterator>

constexpr int INVALID = -1;

class Node
{
public:
	const int position;
	bool traversable;
	bool visitedFromStart;
	bool visitedFromTarget;
	std::vector<int> pathFootprint;

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
	std::vector<int> pathFound;

	void generateNodeList(const unsigned char* pMap);

	int calculatePosAt(int x, int y) const;
	int calculateLeftPos(const int& pos) const;  //TODO: store calculated values to avoid redundant computation
	int calculateRightPos(const int& pos) const; //TODO: store calculated values to avoid redundant computation
	int calculateUpPos(const int& pos) const;    //TODO: store calculated values to avoid redundant computation
	int calculateDownPos(const int& pos) const;  //TODO: store calculated values to avoid redundant computation

	int getX(const int & pos) const;
	int getY(const int & pos) const;

	int getTraversablePos(int pos) const;
	void setPath(const int& fMatch, const int& bMatch, int* pOutBuffer);

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

Node::Node(const int& pos, const unsigned char& value) : position(pos), visitedFromStart(false), visitedFromTarget(false)
{
	if (static_cast<unsigned char>(1) == value) {
		traversable = true;
		//printf("Node::Node() traversable.\n");
	}
	else {
		traversable = false;
		//printf("Node::Node() NOT traversable.\n");
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
		nodeList.emplace_back(index, *(pMap + index)); // TODO: use emplace_back and stepsArray with stack Step template
		printf("Grid::generateNodeList() \t %d, %d\n", nodeList.back().position, nodeList.back().traversable);
	}
	nodeList.shrink_to_fit();
	nodeList.at(targetPos).traversable = true;
	printf("Grid::generate_nodeList() \t nodeList Generated! _nodeList.size(): %d\n", nodeList.size());
}

int Grid::plotSolution(int* pOutBuffer)
{
	printf("Grid::plotSolution() \t\t ENTER:\n");
	// Forward Search Prepare:
	std::queue<int> fwdSearchQ;
	fwdSearchQ.push(startPos);
	nodeList.at(startPos).visitedFromStart = true;
	printf("Grid::plotSolution() \t\t fwdSearchQ: %d\n", startPos);

	// Backward Search Prepare:
	std::queue<int> bckSearchQ;
	bckSearchQ.push(targetPos);
	nodeList.at(targetPos).visitedFromTarget = true;
	nodeList.at(targetPos).pathFootprint.push_back(targetPos);
	printf("Grid::plotSolution() \t\t bckSearchQ: %d\n", targetPos);

	printf("Grid::plotSolution() \t\t Plotting solution...\n");

	// Search start:
	while (!fwdSearchQ.empty()) {

		// Forward Search:
		int fPos = fwdSearchQ.front();
		printf("Grid::plotSolution() \t\t fwdSearchQ: %d\n", fPos);

		if (int left = calculateLeftPos(fPos); left != INVALID) {
			printf("Grid::plotSolution() \t\t           , left: %d\n", left);
			if (Node& tmpNode = nodeList.at(left); tmpNode.visitedFromTarget) {
				setPath(fPos, left, pOutBuffer);
				break; // solution found!
			}
			else if (nodeList.at(left).visitedFromStart) {
				// do nothing, ignore.
			}
			else {
				// new node.
				tmpNode.pathFootprint = nodeList.at(fPos).pathFootprint;
				tmpNode.pathFootprint.push_back(left);
				tmpNode.visitedFromStart = true;
				fwdSearchQ.push(left);
			}
		}
		if (int up = calculateUpPos(fPos); up != INVALID) {
			printf("Grid::plotSolution() \t\t           , up: %d\n", up);
			if (Node& tmpNode = nodeList.at(up); tmpNode.visitedFromTarget) {
				setPath(fPos, up, pOutBuffer);
				break; // solution found!
			}
			else if (nodeList.at(up).visitedFromStart) {
				// do nothing, ignore.
			}
			else {
				// new node.
				tmpNode.pathFootprint = nodeList.at(fPos).pathFootprint;
				tmpNode.pathFootprint.push_back(up);
				tmpNode.visitedFromStart = true;
				fwdSearchQ.push(up);
			}
		}
		if (int right = calculateRightPos(fPos); right != INVALID) {
			printf("Grid::plotSolution() \t\t           , right: %d\n", right);
			if (Node& tmpNode = nodeList.at(right); tmpNode.visitedFromTarget) {
				setPath(fPos, right, pOutBuffer);
				break; // solution found!
			}
			else if (nodeList.at(right).visitedFromStart) {
				// do nothing, ignore.
			}
			else {
				// new node.
				tmpNode.pathFootprint = nodeList.at(fPos).pathFootprint;
				tmpNode.pathFootprint.push_back(right);
				tmpNode.visitedFromStart = true;
				fwdSearchQ.push(right);
			}
		}
		if (int down = calculateDownPos(fPos); down != INVALID) {
			printf("Grid::plotSolution() \t\t           , down: %d\n", down);
			if (Node& tmpNode = nodeList.at(down); tmpNode.visitedFromTarget) {
				setPath(fPos, down, pOutBuffer);
				break; // solution found!
			}
			else if (nodeList.at(down).visitedFromStart) {
				// do nothing, ignore.
			}
			else {
				// new node.
				tmpNode.pathFootprint = nodeList.at(fPos).pathFootprint;
				tmpNode.pathFootprint.push_back(down);
				tmpNode.visitedFromStart = true;
				fwdSearchQ.push(down);
			}
		}
		fwdSearchQ.pop();
	}
	return pathFound.size();
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

void Grid::setPath(const int& fMatch, const int& bMatch, int* pOutBuffer)
{
	for (auto step = std::begin(nodeList.at(fMatch).pathFootprint); step != std::end(nodeList.at(fMatch).pathFootprint); step++) {
		pathFound.push_back(*step);
	}
	for (auto step = std::rbegin(nodeList.at(bMatch).pathFootprint); step != std::rend(nodeList.at(bMatch).pathFootprint); step++) {
		pathFound.push_back(*step);
	}
	printf("Grid::setPath() \t bMatch.size(): %d\n", nodeList.at(bMatch).pathFootprint.size());


	printf("Grid::setPath() \tPath: ");
	for (auto step = std::begin(pathFound); step != std::end(pathFound); pOutBuffer++, step++) {
		printf("%d, ", *step);
		*pOutBuffer = *step;
	}
	std::cout << std::endl;
}