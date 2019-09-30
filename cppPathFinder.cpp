// cppPathFinder.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "FindPath.h"
#include <iostream>

int main()
{
	std::cout << "Hello World!\n";
	
	{
		unsigned char pMap[] = { 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1};
		int pOutBuffer[12];
		int length = FindPath(0, 0, 1, 2, pMap, 4, 3, pOutBuffer, 12);
		std::cout << "\n\nRESULT: steps: " << length << std::endl;
		for (int i = 0; i < length; i++) {
			std::cout << pOutBuffer[i] << std::endl;
		}
	}

	{/*
		unsigned char pMap[] = { 0, 0, 1, 0, 1, 1, 1, 0, 1 };
		int pOutBuffer[7];
		int length = FindPath(2, 0, 0, 2, pMap, 3, 3, pOutBuffer, 7); 
		std::cout << "\n\nRESULT: steps: " << length << std::endl;
		for (int i = 0; i < length; i++) {
			std::cout << pOutBuffer[i] << std::endl;
		}
	}

	{
		unsigned char pMap[] = { 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1 };
		int pOutBuffer[12];
		int length = FindPath(0, 0, 1, 2, pMap, 4, 3, pOutBuffer, 12);
		std::cout << "\n\nRESULT: steps: " << length << std::endl;
		for (int i = 0; i < length; i++) {
			std::cout << pOutBuffer[i] << std::endl;
		}*/
	}
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
