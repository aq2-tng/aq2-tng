//-----------------------------------------------------------------------------
// Matchmode related definitions
//
// $Id: a_match.h,v 1.6 2001/11/25 19:09:25 slicerdw Exp $
//
//-----------------------------------------------------------------------------
// $Log: a_match.h,v $
// Revision 1.6  2001/11/25 19:09:25  slicerdw
// Fixed Matchtime
//
// Revision 1.5  2001/09/28 13:48:34  ra
// I ran indent over the sources. All .c and .h files reindented.
//
// Revision 1.4  2001/06/13 07:55:17  igor_rock
// Re-Added a_match.h and a_match.c
// Added CTF Header for a_ctf.h and a_ctf.c
//
//-----------------------------------------------------------------------------

void SendScores ();
void Cmd_Captain_f (edict_t * ent);
void Cmd_Ready_f (edict_t * ent);
void Cmd_Sub_f (edict_t * ent);
void Cmd_Teamname_f (edict_t * ent);
void Cmd_Teamskin_f (edict_t * ent);
void SendWorldMsg (char *s, int sound, int center);
int CheckForCaptains (int cteam);
extern int team1ready;
extern int team2ready;
extern int ingame;
extern float matchtime;
