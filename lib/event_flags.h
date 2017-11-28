#ifndef EVENT_FLAG_H
#define EVENT_FLAG_H

int destroyEventFlag(int flagId);
int setEventFlag(int flagId);
int clearEventFlag(int flagId);
int waitForEventFlag(int flagId);
int createEventFlag(const char *key);
int getFlagState(int flagId);
int getEventFlag(const char *key);

#endif