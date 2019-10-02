#include "pch.h"

#include "FindPath.h"

#define DEBUG if (false)
#include <iostream>
#include <cstdio>

#include <vector>
#include <queue>
#include <deque>
#include <iterator>

constexpr int INVALID = -1;
constexpr int CONTINUE_SEARCH = -2;

struct Node
{
	const int position;     // The index of the Node in the pMap
	bool traversable;       // True if corresponding index's pMap value is '1' or is Target node.

	bool visitedFromStart;  // True if the Node has appeared in forward search operation.
	int previousPosition;   // Index of Parent node in pMap. Only used with forward search operations.

	bool visitedFromTarget; // True if the Node has appeared in backward search operation.
	int nextPosition;       // Index of Child node in pMap. Only used with backward search operations.

	Node(const int& pos, const unsigned char& value);
};

class Grid
{
	const int width;
	const int height;
	int startPos;               // Index of Start position in pMap
	int targetPos;              // Index of Target position in pMap
	const int maxSteps;         // Max number of steps allowed/allocated by caller.
	std::vector<Node> nodeList; // List of all pMap entites stored into a vector.
	int* pathBuffer;            // [output param] the list of indices denoting shortest path.

	int calculatePosAt(int x, int y) const;
	int calculateLeftPos(const int& pos) const;
	int calculateRightPos(const int& pos) const;
	int calculateUpPos(const int& pos) const;
	int calculateDownPos(const int& pos) const;
	int getX(const int & pos) const;
	int getY(const int & pos) const;
	int getTraversablePos(int pos) const;

	void generateNodeList(const unsigned char* pMap);
	int doNextFwdSearch(const int& fPos, const int& nextPos, std::queue<int>& bckSearchQ);
	int doNextBckSearch(const int & bPos, const int & nextPos, std::queue<int>& bckSearchQ);
	int setPath(const int& fMatch, const int& bMatch);

public:
	Grid(const int& nStartX, const int& nStartY,
		const int& nTargetX, const int& nTargetY,
		const unsigned char* pMap, const int& nMapWidth,
		const int& nMapHeight, const int& nOutBufferSize, int* pOutBuffer);

	int plotSolution();
};

// The entry point (API)
int FindPath(const int nStartX, const int nStartY,
	const int nTargetX, const int nTargetY,
	const unsigned char* pMap, const int nMapWidth, const int nMapHeight,
	int* pOutBuffer, const int nOutBufferSize)
{
	if ((nStartX == nTargetX) && (nStartY == nTargetY)) {
		return 0;
	}
	else {
		Grid theMaze(nStartX, nStartY, nTargetX, nTargetY, pMap, nMapWidth, nMapHeight, nOutBufferSize, pOutBuffer);
		return theMaze.plotSolution();
	}
}

// Node Constructor
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

// Grid Constructor
Grid::Grid(const int& nStartX, const int& nStartY,
	const int& nTargetX, const int& nTargetY,
	const unsigned char* pMap, const int& nMapWidth,
	const int& nMapHeight, const int& nOutBufferSize,
	int* pOutBuffer) : width(nMapWidth), height(nMapHeight), maxSteps(nOutBufferSize), pathBuffer(pOutBuffer)
{
	startPos = calculatePosAt(nStartX, nStartY);
	targetPos = calculatePosAt(nTargetX, nTargetY);
	generateNodeList(pMap);
}

// Calculates the index in the pMap w.r.t the coordinates.
int Grid::calculatePosAt(int x, int y) const
{
	int pos = (width * y) + x;
	return pos;
}

// Copies all the values from pMap into nodeList vector.
void Grid::generateNodeList(const unsigned char* pMap)
{
	const int numberOfNodes = width * height;
	for (int index = 0; index < numberOfNodes; ++index) {
		nodeList.emplace_back(index, *(pMap + index));
		DEBUG printf("Grid::generateNodeList() \t %d, %d\n", nodeList.back().position, nodeList.back().traversable);
	}
	nodeList.at(targetPos).traversable = true; // Target position is always traversable.
	DEBUG printf("Grid::generate_nodeList() \t nodeList Generated! _nodeList.size(): %d\n", static_cast<int>(nodeList.size()));
}

/* plotSolution()
   The main logic for plotting solution to find shortest route.

   A BFS algorithm is used -- Only, this one starts search from both
   Start position node and Target position node within a single iteration.
 */
int Grid::plotSolution()
{
	DEBUG printf("Grid::plotSolution() \t\t ENTER:\n");
	// Forward Search Queue Prepare:
	std::queue<int> fwdSearchQ;
	fwdSearchQ.push(startPos);
	nodeList.at(startPos).visitedFromStart = true;
	DEBUG printf("Grid::plotSolution() \t\t fwdSearchQ: %d\n", startPos);

	// Backward Search Queue Prepare:
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
			if (int leftResult = doNextFwdSearch(fPos, calculateLeftPos(fPos), fwdSearchQ); leftResult != CONTINUE_SEARCH) {
				return leftResult;
			}
			if (int upResult = doNextFwdSearch(fPos, calculateUpPos(fPos), fwdSearchQ); upResult != CONTINUE_SEARCH) {
				return upResult;
			}
			if (int rightResult = doNextFwdSearch(fPos, calculateRightPos(fPos), fwdSearchQ); rightResult != CONTINUE_SEARCH) {
				return rightResult;
			}
			if (int downResult = doNextFwdSearch(fPos, calculateDownPos(fPos), fwdSearchQ); downResult != CONTINUE_SEARCH) {
				return downResult;
			}
			fwdSearchQ.pop();
		}

		// Backward Search:
		if (!bckSearchQ.empty()) {
			int bPos = bckSearchQ.front();
			DEBUG printf("Grid::plotSolution() \t\t bckSearchQ: %d\n", bPos);
			if (int leftResult = doNextBckSearch(bPos, calculateLeftPos(bPos), bckSearchQ); leftResult != CONTINUE_SEARCH) {
				return leftResult;
			}
			if (int upResult = doNextBckSearch(bPos, calculateUpPos(bPos), bckSearchQ); upResult != CONTINUE_SEARCH) {
				return upResult;
			}
			if (int rightResult = doNextBckSearch(bPos, calculateRightPos(bPos), bckSearchQ); rightResult != CONTINUE_SEARCH) {
				return rightResult;
			}
			if (int downResult = doNextBckSearch(bPos, calculateDownPos(bPos), bckSearchQ); downResult != CONTINUE_SEARCH) {
				return downResult;
			}
			bckSearchQ.pop();
		}
	}
	return INVALID;
}

// Calculates the index in pMap that is to the left of pos(also w.r.t pMap)
int Grid::calculateLeftPos(const int& pos) const
{
	int leftPosition = INVALID;
	if (int x = getX(pos);  --x >= 0) {
		leftPosition = (width * getY(pos)) + x;
	}
	return getTraversablePos(leftPosition);
}

// Calculates the index in pMap that is to the right of pos(also w.r.t pMap)
int Grid::calculateRightPos(const int& pos) const
{
	int rightPosition = INVALID;
	if (int x = getX(pos);  ++x < width) {
		rightPosition = (width * getY(pos)) + x;
	}
	return getTraversablePos(rightPosition);
}

// Calculates the index in pMap that is to the top of pos(also w.r.t pMap)
int Grid::calculateUpPos(const int& pos) const
{
	int upPosition = INVALID;
	if (int y = getY(pos); --y >= 0) {
		upPosition = (width * y) + getX(pos);
	}
	return getTraversablePos(upPosition);
}

// Calculates the index in pMap that is to the below of pos(also w.r.t pMap)
int Grid::calculateDownPos(const int& pos) const
{
	int downPosition = INVALID;
	if (int y = getY(pos);  ++y < height) {
		downPosition = (width * y) + getX(pos);
	}
	return getTraversablePos(downPosition);
}

/* Calculates the X-coordinate w.r.t pMap visualised as a Grid of dimensions
   nMapWidth x nMapHeight.
   [input param]pos is the index of node in pMap.
 */
int Grid::getX(const int& pos) const
{
	int x = pos % width;
	return x;
}

/* Calculates the Y-coordinate w.r.t pMap visualised as a Grid of dimensions
   nMapWidth x nMapHeight.
   [input param]pos is the index of node in pMap.
 */
int Grid::getY(const int& pos) const
{
	int y = pos / width;
	return y;
}

// Returns the index w.r.t pMap if the node is traversable.
int Grid::getTraversablePos(int pos) const
{
	if (pos >= 0) {
		if (nodeList.at(pos).traversable) {
			return pos;
		}
	}
	return INVALID;
}

// Prepares extracted from both forward, backward searches and stores path to pathBuffer 
int Grid::setPath(const int& fMatch, const int& bMatch)
{
	std::deque<int> pathFound;
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
		for (auto step = std::begin(pathFound); step != std::end(pathFound); pathBuffer++, step++) {
			DEBUG printf("%d, ", *step);
			*pathBuffer = *step;
		}
	}
	DEBUG std::cout << std::endl;
	DEBUG printf("Grid::setPath() \t bMatch.size(): %d\n", static_cast<int>(pathFound.size()));
	return static_cast<int>(pathFound.size());
}

/* Forward search algorithm
	1. check if node is valid,
	2. check if node at nextPos is visited from Backward Search.
	   If true, generate path and return.
	3. check if node at nextPos is already visited as part of Forward Search.
	   If true, do nothing, return.
	4. Else, add node to the Forward search's Queue for later traversal.
	5. Return to indicate search continuation for the caller.
*/
int Grid::doNextFwdSearch(const int& fPos, const int& nextPos, std::queue<int>& fwdSearchQ) {
	if (nextPos != INVALID) {
		DEBUG printf("Grid::plotSolution() \t\t           , nextPos: %d\n", nextPos);
		if (Node& tmpNode = nodeList.at(nextPos); tmpNode.visitedFromTarget) {
			tmpNode.previousPosition = fPos;
			return setPath(fPos, nextPos); // solution found!
		}
		else if (tmpNode.visitedFromStart) {
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

/* Forward search algorithm
	1. check if node is valid,
	2. check if node at nextPos is visited from Forward Search.
	   If true, generate path and return.
	3. check if node at nextPos is already visited as part of Backward Search.
	   If true, do nothing, return.
	4. Else, add node to the Backward search's Queue for later traversal.
	5. Return to indicate search continuation for the caller.
*/
int Grid::doNextBckSearch(const int& bPos, const int& nextPos, std::queue<int>& bckSearchQ) {
	if (nextPos != INVALID) {
		DEBUG printf("Grid::plotSolution() \t\t           , nextPos: %d\n", nextPos);
		if (Node& tmpNode = nodeList.at(nextPos); tmpNode.visitedFromStart) {
			tmpNode.nextPosition = bPos;
			return setPath(nextPos, bPos); // solution found!
		}
		else if (tmpNode.visitedFromTarget) {
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