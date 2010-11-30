/*
 *  tGame.h
 *  HMMBrain
 *
 *  Created by Arend on 9/23/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
 
#ifndef _tGame_h_included_
#define _tGame_h_included_

#include "globalConst.h"
#include "tAgent.h"
#include <vector>
#include <map>

using namespace std;

#define xDim 256
#define yDim 16
#define startMazes 1
#define cPI 3.14159265

class tGame{
public:
	double runGameWithAgent(tAgent* agent,int mazes,int steps,int maxStepsOfThought,bool show);
	void executeAgentStepInMaze(tAgent* agent,int mazeToUse);
	void evaluateFitnessInMaze(tAgent* agent,int mazeToUse);
	int cX,cY;
	tGame();
	~tGame();
};
#endif