//-----------------------------------------------------------------------------
// Include for base Action game-related things
//
// $Id: a_game.h,v 1.2 2001/05/12 17:36:33 deathwatch Exp $
//
//-----------------------------------------------------------------------------
// $Log: a_game.h,v $
// Revision 1.2  2001/05/12 17:36:33  deathwatch
// Edited the version variables and updated the menus. Added variables:
// ACTION_VERSION, TNG_VERSION and TNG_VERSION2
//
// Revision 1.1.1.1  2001/05/06 17:24:24  igor_rock
// This is the PG Bund Edition V1.25 with all stuff laying around here...
//
//-----------------------------------------------------------------------------


// AQ2:TNG Deathwatch - Updated the Version variables to show TNG Stuff
#define ACTION_VERSION  "1.52 TNG 2.0"
#define TNG_VERSION		"AQ2: The Next Generation"
#define TNG_VERSION2	"AQ2: The Next Generation v2.0"
// AQ2:TNG Deathwatch End

extern char team1_name[];
extern char team2_name[];
extern char team3_name[];
extern char team1_skin[];
extern char team2_skin[];
extern char team3_skin[];
extern char team1_skin_index[];
extern char team2_skin_index[];
extern char team3_skin_index[];
extern char *map_rotation[];
extern int num_maps, cur_map, num_allvotes;	// num_allvotes added by Igor[Rock]

extern char *tnames[];
extern int *took_damage;

void ReadConfigFile ();
void ReadMOTDFile ();
void PrintMOTD (edict_t *);
void stuffcmd (edict_t *, char *);
int KickDoor (trace_t * tr_old, edict_t * ent, vec3_t forward);

// Prototypes of base Q2 functions that weren't included in any Q2 header
qboolean loc_CanSee (edict_t *, edict_t *);
qboolean IsNeutral (edict_t *);
qboolean IsFemale (edict_t *);
void ParseSayText (edict_t *, char *);

//PG BUND - BEGIN
//void ParseSayText(edict_t *, char *);
void GetWeaponName (edict_t * ent, char *buf);
void GetItemName (edict_t * ent, char *buf);
void GetHealth (edict_t * ent, char *buf);
void GetAmmo (edict_t * ent, char *buf);
void GetNearbyTeammates (edict_t * self, char *buf);
//PG BUND - END

//TempFile
void GetLastLoss (edict_t * self, char *buf, char team);

// Firing styles (where shots originate from)
#define ACTION_FIRING_CENTER		0
#define ACTION_FIRING_CLASSIC		1
#define ACTION_FIRING_CLASSIC_HIGH	2

// maxs[2] of a player when crouching (we modify it from the normal 4)
// ...also the modified viewheight -FB 7/18/99
#define CROUCHING_MAXS2                 16
#define CROUCHING_VIEWHEIGHT		8
