// CTF Flag header file

void flagTouch(edict_t *self, edict_t *other, cplane_t *plane, csurface_t *surf);
void spawnFlags(char *mapname);
void checkForCap(edict_t *ent);
void DropFlag ( edict_t* ent );
void returnFlag(edict_t *self);
edict_t* SpawnFlag();
edict_t* SpawnRedFlag();
edict_t* SpawnBlueFlag();

extern int redFlagTaken;
extern int blueFlagTaken;

//extern vec3_t redFlagLocation;
//extern vec3_t blueFlagLocation;

extern edict_t *redFlag;
extern edict_t *blueFlag;

extern int started;
