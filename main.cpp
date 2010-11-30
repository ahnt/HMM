
#include <stdio.h>

#include <vector>
#include <map>
#include <math.h>
#include <time.h>
#include <iostream>
#include "globalConst.h"
#include "tHMM.h"
#include "tAgent.h"
#include "tGame.h"

#define randDouble ((double)rand()/(double)RAND_MAX)

using namespace std;

double replacementRate=0.1;
double perSiteMutationRate=0.01;
int update=0;
int allowedSteps=100;
int huntRepeats=32;

void init(void);
void display(void);
void reshape(int, int);
void mouse(int, int, int, int);
void myIdleFunction(void);
void renderScene(void);

int main(int argc, char *argv[])
{
	vector<tAgent*>population;
	tAgent *masterAgent;
	int i,j,k,who=0;
	tGame *game;
	double maxFitness;
	int newBorn=0;
	FILE *resFile;
	FILE *LOD;
	FILE *genomeFile;
	
	resFile=fopen("result.txt","w+t");
	LOD=fopen(argv[1],"w+t");
	genomeFile=fopen(argv[2],"w+t");	
	srand(time(NULL));
	population.resize(100);
	masterAgent=new tAgent;
//	masterAgent->setupRandomAgent(1000);
//	for(i=0;i<masterAgent->genome.size();i++)
//		masterAgent->genome[i]=rand()&255;
	masterAgent->loadAgent((char*)"startGenome.txt");
	printf("genome length:%i\n",masterAgent->genome.size());
	masterAgent->setupPhenotype();
	masterAgent->showPhenotype();
	masterAgent->saveToDot("brain.dot");
	for(i=0;i<population.size();i++){
		population[i]=new tAgent;
		population[i]->inherit(masterAgent,perSiteMutationRate,0);
//		population[i]->setupRandomAgent(1000);
		population[i]->setupPhenotype();
		population[i]->initialize(xDim/4,yDim/4,rand()&3);
	}
	masterAgent->nrPointingAtMe--;

	game=new tGame;
//	game->maze[0]->setupMaze();
//	game->maze[0]->showMaze();
//	game->maze[0]->showDist();
//	cout<<"max fit maze 0: "<<game->maze[0]->maxFit(allowedSteps)<<endl;
//	game->maze[0]->showPath();
//	cout<<"shortest Path"<<game->maze[0]->longestPath<<endl;
	//game->runGameWithAgent(population[0],10,20,1,true);
	cout<<"setup complete"<<endl;
	while(update<100000){
		int h;
		//*
		for(i=0;i<population.size();i++){
			population[i]->fitness=0.0;
			population[i]->fitnesses.clear();
		}
		for(k=0;k<huntRepeats;k++){
			for(i=0;i<population.size();i++){
				population[i]->fitness=0.0;
//				population[i]->xPos=-4.0;
//				population[i]->yPos=-10.0;
//				population[i]->direction=90.0;
				population[i]->xPos=sin(360.0*((double)(k>>2)/(double)(huntRepeats>>2))*(cPI/180.0))*10.0;
				population[i]->yPos=cos(360.0*((double)(k>>2)/(double)(huntRepeats>>2))*(cPI/180.0))*10.0;
				//population[i]->xPos=sin(360.0*((double)k/(double)huntRepeats)*(cPI/180.0))*20.0;
				//				population[i]->xPos=(randDouble*100.0)-50.0;
//				population[i]->yPos=(randDouble*100.0)-50.0;
				population[i]->direction=90.0*((double)(k&3));
				population[i]->resetBrain();
			}
			for(j=0;j<allowedSteps;j++)
				for(i=0;i<population.size();i++){
					game->executeAgentStepInMaze(population[i],0);
					if((i==who)&&((update&255)==0)){
						fprintf(resFile,"%i	%f	%f\n",j,population[i]->xPos,population[i]->yPos);
						fflush(resFile);
					}
				}
			for(i=0;i<population.size();i++)
				population[i]->fitnesses.push_back(population[i]->fitness);
		}
		// */
		maxFitness=0.0;
		for(i=0;i<population.size();i++){
			population[i]->fitness=1.0;
			//population[i]->fitness=(double)rand()/(double)RAND_MAX;
			for(j=0;j<population[i]->fitnesses.size();j++)
				population[i]->fitness*=population[i]->fitnesses[j];
			population[i]->fitness=pow(population[i]->fitness,1.0/(double)population[i]->fitnesses.size());
			population[i]->fitness/=(double)population[i]->fitnesses.size();
			if(population[i]->fitness>maxFitness){
				maxFitness=population[i]->fitness;
				who=i;
			}
		}
		if((update&31)==31) population[who]->showPhenotype();
		for(i=0;i<population.size();i++)
			if((randDouble<replacementRate)||(population[i]->fitness==0.0))
			{
				do{ j=rand()%population.size(); } while((j==i)||(randDouble>(population[j]->fitness/maxFitness)));
				population[i]->retire();
				population[i]->nrPointingAtMe--;										
				if(population[i]->nrPointingAtMe==0)
					delete population[i];
				population[i]=new tAgent;
				population[i]->inherit(population[j],perSiteMutationRate,update);
				population[i]->fitness=0.0;
			}
		update++;
		//	if((update&4095)==4095)
		{
			cout<< update<< " "<<maxFitness<<" "<< population[0]->xPos<<" "<<population[0]->genome.size()<<" "<<population[0]->yPos<<" "<<newBorn<<endl;
			newBorn=0;
		}

			//	if((update&65535)==65535){
		if((update&31)==31){
			cout<<"---"<<endl;
			tAgent *a;
			a=population[0]->findLMRCA();
			if((a!=NULL)&&(a!=population[0])){
				if(a->ancestor!=NULL){
					cout <<"saving"<<endl;
					a->ancestor->saveFromLMRCAtoNULL(LOD,genomeFile);
				}
			}
			fflush(LOD);
			fflush(genomeFile);

		}

	}
	population[0]->ancestor->saveFromLMRCAtoNULL(LOD,genomeFile);
//	population[0]->saveLOD(LOD,genomeFile);
    return 0;
}

