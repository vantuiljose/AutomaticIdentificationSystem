
#include "stdafx.h"
#include "List.h"

void addTraj(TrajetoriasAtivas* tAtivas, Trajetoria* traj){
	//Celula* c = (Celula*) malloc(sizeof(Celula));
	traj->prox = NULL;
	tAtivas->fin->prox = traj;
	tAtivas->fin = traj;
	tAtivas->size++;
}

void createListTraj(TrajetoriasAtivas** tAtivas){
	(*tAtivas) = (TrajetoriasAtivas*) malloc(sizeof(TrajetoriasAtivas));
	(*tAtivas)->size=0;
	(*tAtivas)->ini = (Trajetoria*) malloc(sizeof(Trajetoria));
	(*tAtivas)->ini->prox = NULL;
	(*tAtivas)->fin = (*tAtivas)->ini;
}