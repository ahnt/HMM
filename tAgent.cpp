/*
 *  tAgent.cpp
 *  HMMBrain
 *
 *  Created by Arend on 9/16/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "tAgent.h"

tAgent::tAgent(){
	int i;
	nrPointingAtMe=1;
	ancestor=NULL;
	for(i=0;i<maxNodes;i++){
		states[i]=0;
		newStates[i]=0;
	}
	bestSteps=-1;
	ID=masterID;
	masterID++;
	saved=false;
	hmmus.clear();
	nrOfOffspring=0;
	totalSteps=0;
	retired=false;
	food=0;
}

tAgent::~tAgent(){
	for(int i=0;i<hmmus.size();i++)
		delete hmmus[i];
	if(ancestor!=NULL){
		ancestor->nrPointingAtMe--;
		if(ancestor->nrPointingAtMe==0)
			delete ancestor;
	}
}

void tAgent::setupRandomAgent(int nucleotides){
	int i;
	genome.resize(nucleotides);
	for(i=0;i<nucleotides;i++)
		genome[i]=127;//rand()&255;
	ampUpStartCodons();
	setupPhenotype();
}
void tAgent::loadAgent(char* filename){
	FILE *f=fopen(filename,"r+t");
	int i;
	genome.clear();
	while(!(feof(f))){
		fscanf(f,"%i	",&i);
		genome.push_back((unsigned char)(i&255));
	}
	setupPhenotype();
}

void tAgent::ampUpStartCodons(void){
	int i,j;
	for(i=0;i<4;i++)
	{
		j=rand()%(genome.size()-100);
		genome[j]=42;
		genome[j+1]=(255-42);
		for(int k=2;k<20;k++)
			genome[j+k]=rand()&255;
	}
}

void tAgent::inherit(tAgent *from,double mutationRate,int theTime){
	int nucleotides=from->genome.size();
	int i,s,o,w;
	double localMutationRate=4.0/from->genome.size();
	vector<unsigned char> buffer;
	born=theTime;
	ancestor=from;
	from->nrPointingAtMe++;
	from->nrOfOffspring++;
	genome.clear();
	genome.resize(from->genome.size());
	for(i=0;i<nucleotides;i++)
		if(((double)rand()/(double)RAND_MAX)<localMutationRate)
			genome[i]=rand()&255;
		else
			genome[i]=from->genome[i];
	if((((double)rand()/(double)RAND_MAX)<0.05)&&(genome.size()<20000)){
		//duplication
		w=15+rand()&511;
		s=rand()%(genome.size()-w);
		o=rand()%genome.size();
		buffer.clear();
		buffer.insert(buffer.begin(),genome.begin()+s,genome.begin()+s+w);
		genome.insert(genome.begin()+o,buffer.begin(),buffer.end());
	}
	if((((double)rand()/(double)RAND_MAX)<0.02)&&(genome.size()>1000)){
		//deletion
		w=15+rand()&511;
		s=rand()%(genome.size()-w);
		genome.erase(genome.begin()+s,genome.begin()+s+w);
	}
	setupPhenotype();
	fitness=0.0;
}
void tAgent::setupPhenotype(void){
	int i;
	tHMMU *hmmu;
	if(hmmus.size()!=0)
		for(i=0;i<hmmus.size();i++)
			delete hmmus[i];
	hmmus.clear();
	for(i=0;i<genome.size();i++)
		if((genome[i]==42)&&(genome[(i+1)%genome.size()]==(255-42))){
			hmmu=new tHMMU;
			hmmu->setup(genome,i);
			hmmus.push_back(hmmu);
		}
}

void tAgent::retire(void){
	retired=true;
}

unsigned char * tAgent::getStatesPointer(void){
	return states;
}

void tAgent::resetBrain(void){
	for(int i=0;i<maxNodes;i++)
		states[i]=0;
}

void tAgent::updateStates(void){
	int i;
	for(i=0;i<hmmus.size();i++)
		hmmus[i]->update(&states[0],&newStates[0]);
	for(i=0;i<maxNodes;i++){
		states[i]=newStates[i];
		newStates[i]=0;
	}
	totalSteps++;
}

void tAgent::showBrain(void){
	for(int i=0;i<maxNodes;i++)
		cout<<(int)states[i];
	cout<<endl;
}

void tAgent::initialize(int x, int y, int d){
	int i,j;
	unsigned char dummy;
	xPos=x;
	yPos=y;
	direction=d;
	steps=0;
	/*
	if((rand()&1)==0){
		scramble[1]=2;
		scramble[2]=1;
	}
	*/
}

tAgent* tAgent::findLMRCA(void){
	tAgent *r,*d;
	if(ancestor==NULL)
		return NULL;
	else{
		r=ancestor;
		d=NULL;
		while(r->ancestor!=NULL){
			if(r->ancestor->nrPointingAtMe!=1)
				d=r;
			r=r->ancestor;
		}
		return d;
	}
}

void tAgent::saveFromLMRCAtoNULL(FILE *statsFile,FILE *genomeFile){
	if(ancestor!=NULL)
		ancestor->saveFromLMRCAtoNULL(statsFile,genomeFile);
	if(!saved){ 
		fprintf(statsFile,"%i	%i	%i	%f	%i	%f\n",ID,born,genome.size(),fitness,bestSteps,(float)totalSteps/(float)nrOfOffspring);
		fprintf(genomeFile,"%i	",ID);
		for(int i=0;i<genome.size();i++)
			fprintf(genomeFile,"	%i",genome[i]);
		fprintf(genomeFile,"\n");
		saved=true;
	}
	if((saved)&&(retired)) genome.clear();
}
void tAgent::saveLOD(FILE *statsFile,FILE *genomeFile){
	if(ancestor!=NULL)
		ancestor->saveLOD(statsFile,genomeFile);
	fprintf(statsFile,"%i	%i	%i	%f	%i	%f\n",ID,born,(int)genome.size(),fitness,bestSteps,(float)totalSteps/(float)nrOfOffspring);
	fprintf(genomeFile,"%i	",ID);
	for(int i=0;i<genome.size();i++)
		fprintf(genomeFile,"	%i",genome[i]);
	fprintf(genomeFile,"\n");
	
}

void tAgent::showPhenotype(void){
	for(int i=0;i<hmmus.size();i++)
		hmmus[i]->show();
	cout<<"------"<<endl;
}

void tAgent::saveToDot(char *filename){
	FILE *f=fopen(filename,"w+t");
	int i,j,k;
	fprintf(f,"digraph brain {\n");
	fprintf(f,"	ranksep=2.0;\n");
	for(i=0;i<9;i++)
		fprintf(f,"	%i [shape=invtriangle,style=filled,color=red];\n",i);
	for(i=9;i<13;i++)
		fprintf(f,"	%i [shape=circle,color=blue];\n",i);
	for(i=13;i<16;i++)
		fprintf(f,"	%i [shape=circle,style=filled,color=green];\n",i);
	for(i=0;i<hmmus.size();i++){
		for(j=0;j<hmmus[i]->ins.size();j++){
			for(k=0;k<hmmus[i]->outs.size();k++)
				fprintf(f,"	%i	->	%i;\n",hmmus[i]->ins[j],hmmus[i]->outs[k]);
		}
	}
	fprintf(f,"	{ rank=same; 0; 1; 2; 3; 4; 5; 6; 7; 8; }\n"); 
	fprintf(f,"	{ rank=same; 9; 10; 11; 12; }\n"); 
	fprintf(f,"	{ rank=same; 13; 14; 15; }\n"); 
	fprintf(f,"}\n");
	fclose(f);
}




