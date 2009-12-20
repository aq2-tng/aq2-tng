//-----------------------------------------------------------------------------
// PG BUND
// a_xgame.h
//
// header file for a_xgame.c
//
// $Id: a_xgame.h,v 1.9 2002/02/18 13:55:35 freud Exp $
//
//-----------------------------------------------------------------------------
// $Log: a_xgame.h,v $
// Revision 1.9  2002/02/18 13:55:35  freud
// Added last damaged players %P
//
// Revision 1.8  2001/09/28 13:48:34  ra
// I ran indent over the sources. All .c and .h files reindented.
//
// Revision 1.7  2001/06/25 11:44:47  slicerdw
// New Video Check System - video_check and video_check_lockpvs no longer latched
//
// Revision 1.6  2001/06/21 00:05:30  slicerdw
// New Video Check System done -  might need some revision but works..
//
// Revision 1.5  2001/05/31 16:58:14  igor_rock
// conflicts resolved
//
// Revision 1.4.2.1  2001/05/31 06:47:51  igor_rock
// - removed crash bug with non exisitng flag files
// - added new commands "setflag1", "setflag2" and "saveflags" to create
//   .flg files
//
// Revision 1.4  2001/05/11 12:21:18  slicerdw
// Commented old Location support ( ADF ) With the ML/ETE Compatible one
//
// Revision 1.2  2001/05/07 21:18:34  slicerdw
// Added Video Checking System
//
// Revision 1.1.1.1  2001/05/06 17:25:24  igor_rock
// This is the PG Bund Edition V1.25 with all stuff laying around here...
//
//-----------------------------------------------------------------------------


//relative position directions
#define RP_NORTH 1
#define RP_SOUTH 2
#define RP_EAST  4
#define RP_WEST  8

//TempFile punch delay
#define PUNCH_DELAY	5	// 5 frames, that's .5 seconds

//maximum size for location description
#define LOC_STR_LEN 128
//maximum amount of location points on a map
#define LOC_MAX_POINTS 300

//TempFile BEGIN
/*
   at first, i wanted to keep support for classic .pg files,
   but that would be inconvenient to code. So I sent all the .pg
   support to hell and completely rebuilt the location detection.
   I used different variable and type names so that it doesn't get confusing. -TF

   //structure of one point
   typedef struct
   {
   vec3_t pos;
   char desc[LOC_STR_LEN];
   } locpoint_t;

   //type declaration of point list
   typedef locpoint_t mapdesc_t[LOC_MAX_POINTS];

   //the whole map description
   extern mapdesc_t mapdesc;

 */

// TempFile cube support
//AQ2:TNG - Slicer Old Location support
/*
typedef struct
  {
    vec3_t lowerleft;
    vec3_t upperright;
    char desc[LOC_STR_LEN];
  }
loccube_t;

typedef loccube_t mapdescex_t[LOC_MAX_POINTS];
extern mapdescex_t mapdescex;
extern loccube_t *setcube;	// this one is used for creating new cubes

extern int num_loccubes;

void FixCubeData (loccube_t *);

// TempFile END


// --- forward declarations

//initializes mapdesc, read a location file if available
void DescListInit (char *mapname);
qboolean GetPositionText (vec3_t, char *);	//TempFile we need this public (HUD!)
*/
//AQ2:TNG END
void ParseSayText (edict_t *, char *, size_t size);


//AQ2:TNG - Slicer 
qboolean GetPlayerLocation (edict_t * self, char *buf);
void GetSightedLocation (edict_t * self, char *buf);
void AntiCheat_CheckClient (edict_t * ent);
void GetLastDamagedPart (edict_t * self, char *buf);
void GetLastDamagedPlayers (edict_t * self, char *buf);
void GetEnemyPosition (edict_t * self, char *buf);
//AQ2:TNG END

void Cmd_SetFlag1_f (edict_t * self);
void Cmd_SetFlag2_f (edict_t * self);
void Cmd_SaveFlags_f (edict_t * self);
