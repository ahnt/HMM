/*
 *  tGame.cpp
 *  HMMBrain
 *
 *  Created by Arend on 9/23/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "tGame.h"
#include "math.h"



tGame::tGame(){
	int i,x,y,j;
}

tGame::~tGame(){
}

void tGame::executeAgentStepInMaze(tAgent* agent,int mazeToUse){
	int c,action,i;
	double angle;
	double d=sqrt((agent->xPos*agent->xPos)+(agent->yPos*agent->yPos));
	/*
	if(agent->xPos>0) 
		agent->states[0]=0;
	else 
		agent->states[0]=1;
	if(agent->yPos>0) 
		agent->states[1]=0;
	else 
		agent->states[1]=1;
	agent->updateStates();
	action=(agent->states[maxNodes-1]&1)+((agent->states[maxNodes-2]&1)<<1);
	switch(action){
		case 0:
			agent->xPos+=1.0;
			agent->yPos+=1.0;
			break;
		case 1:
			agent->xPos+=1.0;
			agent->yPos-=1.0;
			break;
		case 2:
			agent->xPos-=1.0;
			agent->yPos+=1.0;
			break;
		case 3:
			agent->xPos-=1.0;
			agent->yPos-=1.0;
			break;
	}			
	*/
	angle=atan2(-1.0*agent->yPos,-1.0*agent->xPos)*180.0/cPI;
	agent->states[maxNodes-1]=0;
	agent->states[maxNodes-2]=0;
	agent->states[maxNodes-3]=0;
	angle=agent->direction-angle;
	for(i=0;i<5;i++)
		agent->states[i]=0;
	if(abs((int)angle)<90){
		agent->states[2+(int)(angle/36.0)]=1;
	}
//	printf("%f	%f	%i	",angle,agent->direction,4+(int)(angle/10.0));
//	for(i=0;i<9;i++)
//		printf("%i",agent->states[i]);
//	printf("\n");
//	c=0;
//	do{
//		c++;
		agent->updateStates();
		action=(agent->states[maxNodes-1]&1)+((agent->states[maxNodes-2]&1)<<1)+((agent->states[maxNodes-3]&1)<<1);
//	}while ((c<3)&&(action==0));
	
	switch(action){
		case 0:
		case 4:break;
		case 1:
			agent->direction-=5.0;
			while(agent->direction>360.0) agent->direction-=360.0;
			while(agent->direction<0.0) agent->direction+=360.0;
			break;
		case 2:
			agent->direction+=5.0;
			while(agent->direction>360.0) agent->direction-=360.0;
			while(agent->direction<0.0) agent->direction+=360.0;
			break;
		case 3:
			agent->xPos+=cos(agent->direction*(cPI/180.0));
			agent->yPos+=sin(agent->direction*(cPI/180.0));
			break;
		case 5:
			agent->direction-=15.0;
			while(agent->direction>360.0) agent->direction-=360.0;
			while(agent->direction<0.0) agent->direction+=360.0;
			break;
		case 6:
			agent->direction+=15.0;
			while(agent->direction>360.0) agent->direction-=360.0;
			while(agent->direction<0.0) agent->direction+=360.0;
			break;
		case 7:
			agent->xPos+=cos(agent->direction*(cPI/180.0))*2.0;
			agent->yPos+=sin(agent->direction*(cPI/180.0))*2.0;
			break;
	}
	double nd=sqrt((agent->xPos*agent->xPos)+(agent->yPos*agent->yPos));
	if(nd<1.0) 
		nd=1.0;
	agent->fitness+=1.0/(nd*nd*nd);
}




