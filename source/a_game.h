//-----------------------------------------------------------------------------
// Include for base Action game-related things
//
// $Id: a_game.h,v 1.12 2001/09/28 13:48:34 ra Exp $
//
//-----------------------------------------------------------------------------
// $Log: a_game.h,v $
// Revision 1.12  2001/09/28 13:48:34  ra
// I ran indent over the sources. All .c and .h files reindented.
//
// Revision 1.11  2001/09/02 20:33:34  deathwatch
// Added use_classic and fixed an issue with ff_afterround, also updated version
// nr and cleaned up some commands.
//
// Updated the VC Project to output the release build correctly.
//
// Revision 1.10  2001/08/15 14:50:48  slicerdw
// Added Flood protections to Radio & Voice, Fixed the sniper bug AGAIN
//
// Revision 1.9  2001/08/08 12:54:02  ra
// Increasing version number to 2.0b2
//
// Revision 1.8  2001/07/15 20:54:20  slicerdw
// Added a function to clean bodies "cleanbodies" and a "entcount" for test porpuses
//
// Revision 1.7  2001/06/21 00:05:30  slicerdw
// New Video Check System done -  might need some revision but works..
//
// Revision 1.4  2001/06/19 18:56:38  deathwatch
// New Last killed target system
//
// Revision 1.3  2001/06/01 19:18:42  slicerdw
// Added Matchmode Code
//
// Revision 1.2  2001/05/12 17:36:33  deathwatch
// Edited the version variables and updated the menus. Added variables:
// ACTION_VERSION, TNG_VERSION and TNG_VERSION2
//
// Revision 1.1.1.1  2001/05/06 17:24:24  igor_rock
// This is the PG Bund Edition V1.25 with all stuff laying around here...
//
//-----------------------------------------------------------------------------


// AQ2:TNG Deathwatch - Updated the Version variables to show TNG Stuff
#define ACTION_VERSION  "1.52 TNG 2.1"
#define TNG_VERSION		"AQ2: The Next Generation"
#define TNG_VERSION2	"AQ2: The Next Generation v2.1"
// AQ2:TNG Deathwatch End
//AQ2:TNG Slicer This is the max players writen on last killed target
#define MAX_LAST_KILLED 5
//AQ2:TNG END

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
//AQ2:TNG SLICER  New Last killed enemy support
void ResetKills (edict_t * ent);
int ReadKilledPlayers (edict_t * ent);
void AddKilledPlayer (edict_t * self, edict_t * ent);
void GetLastKilledTarget (edict_t * self, char *buf);
void VideoCheckClient (edict_t * ent);
//AQ2:TNG END
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
