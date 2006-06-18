//-----------------------------------------------------------------------------
// Matchmode related code
//
// $Id: a_match.c,v 1.20 2006/06/18 09:07:15 igor_rock Exp $
//
//-----------------------------------------------------------------------------
// $Log: a_match.c,v $
// Revision 1.20  2006/06/18 09:07:15  igor_rock
// - corrected some indexes from [1] to [TEAM1] and so on
//
// Revision 1.19  2006/06/17 11:32:49  igor_rock
// Some code cleanup:
// - moved team related variables to a single struct variable
// - some minor changes to reduced possible error sources
//
// Revision 1.18  2004/04/08 23:19:51  slicerdw
// Optimized some code, added a couple of features and fixed minor bugs
//
// Revision 1.17  2003/06/15 21:43:53  igor
// added IRC client
//
// Revision 1.16  2003/02/10 02:12:25  ra
// Zcam fixes, kick crashbug in CTF fixed and some code cleanup.
//
// Revision 1.15  2002/03/28 12:10:11  freud
// Removed unused variables (compiler warnings).
// Added cvar mm_allowlock.
//
// Revision 1.14  2002/03/28 11:46:03  freud
// stat_mode 2 and timelimit 0 did not show stats at end of round.
// Added lock/unlock.
// A fix for use_oldspawns 1, crash bug.
//
// Revision 1.13  2002/03/25 23:34:06  slicerdw
// Small tweak on var handling ( prevent overflows )
//
// Revision 1.12  2001/12/05 15:27:35  igor_rock
// improved my english (actual -> current :)
//
// Revision 1.11  2001/12/02 16:15:32  igor_rock
// added console messages (for the IRC-Bot) to matchmode
//
// Revision 1.10  2001/11/25 19:09:25  slicerdw
// Fixed Matchtime
//
// Revision 1.9  2001/09/28 15:44:29  ra
// Removing Bill Gate's fingers from a_match.c
//
// Revision 1.8  2001/09/28 13:48:34  ra
// I ran indent over the sources. All .c and .h files reindented.
//
// Revision 1.7  2001/06/16 16:47:06  deathwatch
// Matchmode Fixed
//
// Revision 1.6  2001/06/13 15:36:31  deathwatch
// Small fix
//
// Revision 1.5  2001/06/13 07:55:17  igor_rock
// Re-Added a_match.h and a_match.c
// Added CTF Header for a_ctf.h and a_ctf.c
//
//-----------------------------------------------------------------------------

#include "g_local.h"
#include "a_match.h"

float matchtime = 0;
int ingame = 0;

void SendWorldMsg(char *s, int sound, int center)
{
	int i;
	edict_t *e;

	for (i = 1; i <= maxclients->value; i++) {
		e = g_edicts + i;
		if (e->inuse) {
			if (center) {
				gi.centerprintf(e, "%s", s);
			} else {
				gi.cprintf(e, PRINT_HIGH, "%s", s);
				if (sound)
					stuffcmd(e, "play misc/comp_up.wav");
			}
		}
	}
	gi.dprintf(s);
}

void SendScores()
{
	int mins, secs;

	mins = matchtime / 60;
	secs = matchtime - (mins * 60);
	gi.bprintf(PRINT_HIGH, "žžžžžžžžžžžžžžžžžžžžžžžžžžžžžžžžŸ\n");
	gi.bprintf(PRINT_HIGH, "Team 1 Score - Team 2 Score\n");
	gi.bprintf(PRINT_HIGH, "    [%d]           [%d]\n", team_data[TEAM1].score, team_data[TEAM2].score);
	gi.bprintf(PRINT_HIGH, "Total Played Time: %d:%02d\n", mins, secs);
	gi.bprintf(PRINT_HIGH, "žžžžžžžžžžžžžžžžžžžžžžžžžžžžžžžžŸ\n");
	gi.bprintf(PRINT_HIGH, "Match is over, waiting for next map, please vote a new one..\n");
}

void Cmd_Kill_f(edict_t * ent);	// Used for killing people when they sub off
void Cmd_Sub_f(edict_t * ent)
{
	char temp[256];

	if (ent->client->resp.team == NOTEAM || !teamplay->value) {
		gi.cprintf(ent, PRINT_HIGH, "You need to be on a team for that...\n");
		return;
	}
	if (ent->client->resp.subteam == 0) {
		ent->client->resp.subteam = ent->client->resp.team;

		// lets kill em.
		Cmd_Kill_f(ent);
		if (ent->client->resp.subteam == 1) {
			sprintf(temp, "%s is now a substitute for %s\n", ent->client->pers.netname, team_data[TEAM1].name);
			SendWorldMsg(temp, 0, 0);
		} else {
			sprintf(temp, "%s is now a substitute for %s\n", ent->client->pers.netname, team_data[TEAM2].name);
			SendWorldMsg(temp, 0, 0);
		}
		return;
	}
	if (ent->client->resp.subteam == TEAM1) {
		sprintf(temp, "%s is no longer a substitute for %s\n", ent->client->pers.netname, team_data[TEAM1].name);
		SendWorldMsg(temp, 0, 0);
		ent->client->resp.subteam = 0;
		return;
	}
	if (ent->client->resp.subteam == TEAM2) {
		sprintf(temp, "%s is no longer a substitute for %s\n", ent->client->pers.netname, team_data[TEAM2].name);
		SendWorldMsg(temp, 0, 0);
		ent->client->resp.subteam = 0;
		return;
	}
}
int CheckForCaptains(int cteam)
{
	edict_t *ent;
	int i;

	for (i = 1; i <= (int) (maxclients->value); i++) {
		ent = getEnt(i);
		if (ent->inuse) {
			if (ent->client->resp.captain == cteam)
				return 1;
		}
	}
	return 0;
}

void Cmd_Captain_f(edict_t * ent)
{
	int i;
	char temp[256];

	if (ent->client->resp.team == NOTEAM || !teamplay->value) {
		gi.cprintf(ent, PRINT_HIGH, "You need to be on a team for that...\n");
		return;
	}
	if (ent->client->resp.captain == TEAM1) {
		sprintf(temp, "%s is no longer %s's Captain\n", ent->client->pers.netname, team_data[TEAM1].name);
		SendWorldMsg(temp, 0, 0);
		team_data[TEAM1].ready = 0;
		ent->client->resp.captain = 0;
		return;
	}
	if (ent->client->resp.captain == TEAM2) {
		sprintf(temp, "%s is no longer %s's Captain\n", ent->client->pers.netname, team_data[TEAM2].name);
		SendWorldMsg(temp, 0, 0);
		team_data[TEAM2].ready = 0;
		ent->client->resp.captain = 0;
		return;
	}
	i = CheckForCaptains(ent->client->resp.team);
	if (i == ent->client->resp.team) {
		gi.cprintf(ent, PRINT_HIGH, "Your team already has a Captain\n");
		return;
	}
	if (i == 0) {
		if (ent->client->resp.team == 1) {
			sprintf(temp, "%s is now %s's Captain\n", ent->client->pers.netname, team_data[TEAM1].name);
			SendWorldMsg(temp, 1, 0);
			ent->client->resp.captain = 1;
		} else if (ent->client->resp.team == 2) {
			sprintf(temp, "%s is now %s's Captain\n", ent->client->pers.netname, team_data[TEAM2].name);
			SendWorldMsg(temp, 1, 0);
			ent->client->resp.captain = 2;
		}
	}
}

//extern int started; // AQ2:M - Matchmode - Used for ready command
void Cmd_Ready_f(edict_t * ent)
{
	char temp[256];

	if (ent->client->resp.captain == 0) {
		gi.cprintf(ent, PRINT_HIGH, "You need to be a captain for that\n");
		return;
	}

	if (ent->client->resp.captain == 1) {
		if (team_data[TEAM1].ready) {
			sprintf(temp, "%s is no longer ready to play!\n", team_data[TEAM1].name);
			SendWorldMsg(temp, 0, 1);
			team_data[TEAM1].ready = 0;
			return;
		} else {
			sprintf(temp, "%s is ready to play!\n", team_data[TEAM1].name);
			SendWorldMsg(temp, 1, 1);
			team_data[TEAM1].ready = 1;
			return;
		}
	}
	if (ent->client->resp.captain == 2) {
		if (team_data[TEAM2].ready) {
			sprintf(temp, "The %s is no longer ready to play!\n", team_data[TEAM2].name);
			SendWorldMsg(temp, 0, 1);
			team_data[TEAM2].ready = 0;
			return;
		} else {
			sprintf(temp, "%s is ready to play!\n", team_data[TEAM2].name);
			SendWorldMsg(temp, 1, 1);
			team_data[TEAM2].ready = 1;
			return;
		}
	}
}

void Cmd_Teamname_f(edict_t * ent)
{
	int i, u, team;
	char temp[128];
	team = ent->client->resp.team;
	if (ent->client->resp.captain == 0) {
		gi.cprintf(ent, PRINT_HIGH, "You need to be a captain for that\n");
		return;
	}
	if (team == 1 && team_data[TEAM1].ready == 1) {
		gi.cprintf(ent, PRINT_HIGH, "You cannot use this while 'Ready'\n");
		return;
	}
	if (team == 2 && team_data[TEAM2].ready == 1) {
		gi.cprintf(ent, PRINT_HIGH, "You cannot use this while 'Ready'\n");
		return;
	}
	if (team_round_going || team_game_going) {
		gi.cprintf(ent, PRINT_HIGH, "You cannot use this while playing\n");
		return;
	}
	i = gi.argc();
	if (gi.argc() < 2) {
		if (team == 1)
			gi.cprintf(ent, PRINT_HIGH, "Your Team Name is %s\n", team_data[TEAM1].name);

		else if (team == 2)
			gi.cprintf(ent, PRINT_HIGH, "Your Team Name is %s\n", team_data[TEAM2].name);
		return;
	}
	strncpy(temp, gi.argv(1), sizeof(temp));
	for (u = 2; u <= i; u++) {
		strncat(temp, " ", sizeof(temp));
		strncat(temp, gi.argv(u), sizeof(temp));
	}
	temp[18] = 0;
	if (team == 1) {
		gi.dprintf("%s (Team 1) is now known as %s\n", team_data[TEAM1].name, temp);
		IRC_printf(IRC_T_GAME, "%n (Team 1) is now known as %n", team_data[TEAM1].name, temp);
		strcpy(team_data[TEAM1].name, temp);
		gi.cprintf(ent, PRINT_HIGH, "New Team Name: %s\n", team_data[TEAM1].name);
		return;
	}
	if (team == 2) {
		gi.dprintf("%s (Team 2) is now known as %s\n", team_data[TEAM2].name, temp);
		IRC_printf(IRC_T_GAME, "%n (Team 2) is now known as %n", team_data[TEAM2].name, temp);
		strcpy(team_data[TEAM2].name, temp);
		gi.cprintf(ent, PRINT_HIGH, "New Team Name: %s\n", team_data[TEAM2].name);
		return;
	}
}

void Cmd_Teamskin_f(edict_t * ent)
{
	char *s;
	int i, team;
	char temp[128];

	team = ent->client->resp.team;
	if (ent->client->resp.captain == 0) {
		gi.cprintf(ent, PRINT_HIGH, "You need to be a captain for that\n");
		return;
	}
	if (team == 1 && team_data[TEAM1].ready == 1) {
		gi.cprintf(ent, PRINT_HIGH, "You cannot use this while 'Ready'\n");
		return;
	}
	if (team == 2 && team_data[TEAM2].ready == 1) {
		gi.cprintf(ent, PRINT_HIGH, "You cannot use this while 'Ready'\n");
		return;
	}
	if (team_round_going || team_game_going) {
		gi.cprintf(ent, PRINT_HIGH, "You cannot use this while playing\n");
		return;
	}
	i = gi.argc();
	if (gi.argc() < 2) {
		s = Info_ValueForKey(ent->client->pers.userinfo, "skin");
		if (ent->client->resp.team == 1)
			gi.cprintf(ent, PRINT_HIGH, "Your Team Skin is %s\n", team_data[TEAM1].skin);
		else
			gi.cprintf(ent, PRINT_HIGH, "Your Team Skin is %s\n", team_data[TEAM2].skin);
		return;
	}
	strncpy(temp, gi.argv(1), sizeof(temp));
	temp[127] = 0;
	if (team == 1) {
		strcpy(team_data[TEAM1].skin, temp);
		sprintf(team_data[TEAM1].skin_index, "../players/%s_i", team_data[TEAM1].skin);
		gi.cprintf(ent, PRINT_HIGH, "New Team Skin: %s\n", team_data[TEAM1].skin);
		return;
	}
	if (team == 2) {
		strcpy(team_data[TEAM2].skin, temp);
		sprintf(team_data[TEAM2].skin_index, "../players/%s_i", team_data[TEAM2].skin);
		gi.cprintf(ent, PRINT_HIGH, "New Team Skin: %s\n", team_data[TEAM2].skin);
		return;
	}
}

void Cmd_TeamLock_f(edict_t * ent, int a_switch)
{
	char msg[128];
	if (!mm_allowlock->value)
		gi.cprintf(ent, PRINT_HIGH, "Team locking is disabled on this server\n");
	else if (!ent->client->resp.team)
		gi.cprintf(ent, PRINT_HIGH, "You are not on a team\n");
	else if (!ent->client->resp.captain)
		gi.cprintf(ent, PRINT_HIGH, "You are not the captain of your team\n");
	else if (a_switch == 1 && team_data[ent->client->resp.team].locked)
		gi.cprintf(ent, PRINT_HIGH, "Your team is already locked\n");
	else if (a_switch == 0 && !team_data[ent->client->resp.team].locked)
		gi.cprintf(ent, PRINT_HIGH, "Your team isn't locked\n");
	else {
		team_data[ent->client->resp.team].locked = a_switch;

		if (a_switch == 1)
			sprintf(msg, "%s locked\n", TeamName(ent->client->resp.team));
		else
			sprintf(msg, "%s unlocked\n", TeamName(ent->client->resp.team));

		CenterPrintAll(msg);
	}
}
