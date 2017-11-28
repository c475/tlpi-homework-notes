#ifndef BINARY_SEMS_H
#define BINARY_SEMS_H

#include "../../lib/tlpi_hdr.h"

// variables controlling the operation of the functions below

extern Boolean bsUseSemUndo; // use SEM_UNDO during semop()?
extern Boolean bsRetryOnEintr; // retry semop() if interrupted by a signal handler?


int initSemAvailable(int semId, int semNum);

int initSemInUse(int semId, int semNum);

int reserveSem(int semId, int semNum);

int releaseSem(int semId, int semNum);

#endif
