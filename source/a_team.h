//-----------------------------------------------------------------------------
// Include for Action team-related things
//
// $Id: a_team.h,v 1.6 2002/03/24 20:11:28 freud Exp $
//
//-----------------------------------------------------------------------------
// $Log: a_team.h,v $
// Revision 1.6  2002/03/24 20:11:28  freud
// New Spawn system introduced. cvar use_newspawns (default on). The system
// tries as best it can to stop teams from spawning in the same place more
// than once. The chances of a team spawning at the same place completely
// relies on how many spawns there are in the map.
//
// Revision 1.5  2001/11/27 23:23:40  igor_rock
// Bug fixed: day_cycle_at wasn't reset at mapchange
//
// Revision 1.4  2001/11/03 17:31:15  ra
// Compiler warning fix.
//
// Revision 1.3  2001/10/18 12:55:35  deathwatch
// Added roundtimeleft
//
// Revision 1.2  2001/09/28 13:48:34  ra
// I ran indent over the sources. All .c and .h files reindented.
//
// Revision 1.1.1.1  2001/05/06 17:24:27  igor_rock
// This is the PG Bund Edition V1.25 with all stuff laying around here...
//
//-----------------------------------------------------------------------------

#define NOTEAM          0
#define TEAM1           1
#define TEAM2           2
#define TEAM3           3

#define MAX_TEAMS       3
#define TEAM_TOP        (MAX_TEAMS+1)

#define WINNER_NONE     0
#define WINNER_TEAM1    1
#define WINNER_TEAM2    2
#define WINNER_TEAM3    3
#define WINNER_TIE      4

// Pre- and post-trace code for our teamplay anti-stick stuff.  If there are
// still "transparent" (SOLID_TRIGGER) players, they need to be set to
// SOLID_BBOX before a trace is performed, then changed back again
// afterwards.  PRETRACE() and POSTTRACE() should be called before and after
// traces in all places where combat is taking place (ie "transparent" players
// should be detected), ie shots being traced etc.  
// FB 6/1/99: Now crouching players will have their bounding box adjusted here
// too, for better shot areas. (there has to be a better way to do this?)

#define PRETRACE() \
        if (transparent_list && (int)teamplay->value && !lights_camera_action) \
                TransparentListSet(SOLID_BBOX)

#define POSTTRACE() \
        if (transparent_list && (int)teamplay->value && !lights_camera_action) \
                TransparentListSet(SOLID_TRIGGER)

edict_t *SelectTeamplaySpawnPoint (edict_t *);
edict_t *New_SelectTeamplaySpawnPoint (edict_t *);
qboolean FallingDamageAmnesty (edict_t * targ);
void OpenJoinMenu (edict_t *);
void OpenWeaponMenu (edict_t *);
void OpenItemMenu (edict_t * ent);
void JoinTeam (edict_t * ent, int desired_team, int skip_menuclose);
edict_t *FindOverlap (edict_t * ent, edict_t * last_overlap);
void CheckTeamRules (void);
void A_Scoreboard (edict_t * ent);
void Team_f (edict_t * ent);
qboolean StartClient (edict_t * ent);
void AssignSkin (edict_t *, char *);
void TallyEndOfLevelTeamScores (void);
void CheckForUnevenTeams (void);
void New_SetupTeamSpawnPoints ();
void SetupTeamSpawnPoints ();
int CheckTeamSpawnPoints ();
void GetSpawnPoints ();
void New_GetSpawnPoints ();
void CleanBodies ();		// from p_client.c, removes all current dead bodies from map
void New_SelectFarTeamplaySpawnPoint (int team, qboolean teams_assigned[]);

void LeaveTeam (edict_t *);
int newrand (int top);
void InitTransparentList ();
void AddToTransparentList (edict_t *);
void RemoveFromTransparentList (edict_t *);
void PrintTransparentList ();
void CenterPrintAll (char *msg);

typedef struct spawn_distances_s
{
  float distance;
  edict_t *s;
}
spawn_distances_t;

typedef struct transparent_list_s
{
  edict_t *ent;
  struct transparent_list_s *next;
}
transparent_list_t;


extern qboolean team_game_going;
extern qboolean team_round_going;
extern int team1_score;
extern int team2_score;
extern int team3_score;
extern int team1_total;
extern int team2_total;
extern int team3_total;
extern int lights_camera_action;
extern int holding_on_tie_check;
extern int team_round_countdown;
extern int timewarning;
extern int fragwarning;
extern transparent_list_t *transparent_list;
extern trace_t trace_t_temp;
extern int current_round_length; // For RoundTimeLeft
extern int day_cycle_at;
