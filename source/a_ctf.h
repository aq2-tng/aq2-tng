//-----------------------------------------------------------------------------
// a_ctf.h
// CTF Flag header file
//-----------------------------------------------------------------------------
//
// $Id: a_ctf.h,v 1.4 2001/05/13 11:26:38 igor_rock Exp $
//
//-----------------------------------------------------------------------------
// $Log: a_ctf.h,v $
// Revision 1.4  2001/05/13 11:26:38  igor_rock
// Added CVS Headers
//
//-----------------------------------------------------------------------------

void flagTouch(edict_t *self, edict_t *other, cplane_t *plane, csurface_t *surf);
void spawnFlags(char *mapname);
void checkForCap(edict_t *ent);
void DropFlag ( edict_t* ent );
void returnFlag(edict_t *self);
void ForceSpawn(edict_t *ent);
void SVCmd_MoveFlag();
void SVCmd_MoveSpawn();
void SVCmd_RemoveSpawns();
edict_t* SpawnFlag();
edict_t* SpawnRedFlag();
edict_t* SpawnBlueFlag();

extern int redFlagTaken;
extern int blueFlagTaken;

extern vec3_t redFlagLocation;
extern vec3_t blueFlagLocation;

extern edict_t *redFlag;
extern edict_t *blueFlag;

extern edict_t *closestRedSpawns[3]; // AQ2:M - CTF
extern edict_t *closestBlueSpawns[3]; // AQ2:M - CTF

extern int started;
extern int hackedSpawns;

extern int loadedFlags;
