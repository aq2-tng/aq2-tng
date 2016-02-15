//-----------------------------------------------------------------------------
// Matchmode related code
//
// $Id: a_match.c,v 1.18 2004/04/08 23:19:51 slicerdw Exp $
//
//-----------------------------------------------------------------------------
// $Log: a_match.c,v $
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
float realLtime = 0; //Need this because when paused level.time doenst inc

void SendScores(void)
{
	int mins, secs;

	mins = matchtime / 60;
	secs = (int)matchtime % 60;
	if(use_3teams->value) {
		gi.bprintf(PRINT_HIGH, "žžžžžžžžžžžžžžžžžžžžžžžžžžžžžžžžžžžžžžžžžžŸ\n");
		gi.bprintf(PRINT_HIGH, " Team 1 Score - Team 2 Score - Team 3 Score\n");
		gi.bprintf(PRINT_HIGH, "    [%d]           [%d]           [%d]\n", teams[TEAM1].score, teams[TEAM2].score, teams[TEAM3].score);
		gi.bprintf(PRINT_HIGH, " Total Played Time: %d:%02d\n", mins, secs);
		gi.bprintf(PRINT_HIGH, "žžžžžžžžžžžžžžžžžžžžžžžžžžžžžžžžžžžžžžžžžžŸ\n");
	} else {
		int team1score = 0, team2score = 0;

		if(ctf->value)
			GetCTFScores(&team1score, &team2score);
		else {
			team1score = teams[TEAM1].score;
			team2score = teams[TEAM2].score;
		}
		gi.bprintf(PRINT_HIGH, "žžžžžžžžžžžžžžžžžžžžžžžžžžžŸ\n");
		gi.bprintf(PRINT_HIGH, " Team 1 Score - Team 2 Score\n");
		gi.bprintf(PRINT_HIGH, "     [%d]           [%d]\n", team1score, team2score);
		gi.bprintf(PRINT_HIGH, " Total Played Time: %d:%02d\n", mins, secs);
		gi.bprintf(PRINT_HIGH, "žžžžžžžžžžžžžžžžžžžžžžžžžžžŸ\n");
	}
	gi.bprintf(PRINT_HIGH, "Match is over, waiting for next map, please vote a new one..\n");
}

void Cmd_Kill_f(edict_t * ent);	// Used for killing people when they sub off
void Cmd_Sub_f(edict_t * ent)
{
	if (!matchmode->value) {
		gi.cprintf(ent, PRINT_HIGH, "This command needs matchmode to be enabled\n");
		return;
	}

	if (ent->client->resp.team == NOTEAM) {
		gi.cprintf(ent, PRINT_HIGH, "You need to be on a team for that...\n");
		return;
	}
	if (!ent->client->resp.subteam) {
		Cmd_Kill_f(ent); // lets kill em.
		gi.bprintf(PRINT_HIGH, "%s is now a substitute for %s\n", ent->client->pers.netname, teams[ent->client->resp.team].name);
		ent->client->resp.subteam = ent->client->resp.team;
		return;
	}
	else {
		gi.bprintf(PRINT_HIGH, "%s is no longer a substitute for %s\n", ent->client->pers.netname, teams[ent->client->resp.team].name);
		ent->client->resp.subteam = 0;
		if(team_round_going && (teamdm->value || ctf->value))
		{
			ResetKills (ent);
			//AQ2:TNG Slicer Last Damage Location
			ent->client->resp.last_damaged_part = 0;
			ent->client->resp.last_damaged_players[0] = '\0';
			//AQ2:TNG END
			PutClientInServer (ent);
			AddToTransparentList (ent);
		}
		return;
	}

}

int TeamsReady(void)
{
	int i;
	
	for(i = TEAM1; i <= teamCount; i++) {
		if(!teams[i].ready)
			return 0;
	}
	return 1;
}

int CheckForCaptains(int cteam)
{
	edict_t *ent;
	int i;

	for (i = 1; i <= (int)(maxclients->value); i++) {
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
	int otherTeam;

	if (!matchmode->value) {
		gi.cprintf(ent, PRINT_HIGH, "This command needs matchmode to be enabled\n");
		return;
	}

	if (ent->client->resp.team == NOTEAM) {
		gi.cprintf(ent, PRINT_HIGH, "You need to be on a team for that...\n");
		return;
	}
	if (ent->client->resp.captain) {
		gi.bprintf(PRINT_HIGH, "%s is no longer %s's captain\n", ent->client->pers.netname, teams[ent->client->resp.captain].name);
		if(!team_round_going || (!teamdm->value && !ctf->value))
			teams[ent->client->resp.captain].ready = 0;
		ent->client->resp.captain = 0;
		return;
	}

	if (CheckForCaptains(ent->client->resp.team)) {
		gi.cprintf(ent, PRINT_HIGH, "Your team already has a captain\n");
		return;
	}
	else {
		gi.bprintf(PRINT_HIGH, "%s is now %s's captain\n", ent->client->pers.netname, teams[ent->client->resp.team].name);
		gi.sound(&g_edicts[0], CHAN_VOICE | CHAN_NO_PHS_ADD, gi.soundindex("misc/comp_up.wav"), 1.0, ATTN_NONE, 0.0);

		ent->client->resp.captain = ent->client->resp.team;
		if(ent->client->resp.team == TEAM1)
			otherTeam = TEAM2;
		else
			otherTeam = TEAM1;

		if(teams[otherTeam].wantReset)
			gi.cprintf(ent, PRINT_HIGH, "Team %i wants to reset scores, type 'resetscores' to accept\n", otherTeam);
	}
}

//extern int started; // AQ2:M - Matchmode - Used for ready command
void Cmd_Ready_f(edict_t * ent)
{
	char temp[128];

	if (!matchmode->value) {
		gi.cprintf(ent, PRINT_HIGH, "This command needs matchmode to be enabled\n");
		return;
	}

	if (!ent->client->resp.captain) {
		gi.cprintf(ent, PRINT_HIGH, "You need to be a captain for that\n");
		return;
	}

	if((teamdm->value || ctf->value) && team_round_going) {
		if(teamdm->value)
			gi.cprintf(ent, PRINT_HIGH, "You can't unready in teamdm, use 'pausegame' instead\n");
		else
			gi.cprintf(ent, PRINT_HIGH, "You can't unready in ctf, use 'pausegame' instead\n");
		return;
	}

	if (teams[ent->client->resp.captain].ready) {
		Com_sprintf(temp, sizeof(temp),"%s is no longer ready to play!", teams[ent->client->resp.captain].name);
		CenterPrintAll(temp);
		teams[ent->client->resp.captain].ready = 0;
	} else {
		Com_sprintf(temp, sizeof(temp), "%s is ready to play!", teams[ent->client->resp.captain].name);
		CenterPrintAll(temp);
		teams[ent->client->resp.captain].ready = 1;
	}

}

void Cmd_Teamname_f(edict_t * ent)
{
	int i, u, team;
	char temp[32];

	if (!matchmode->value) {
		gi.cprintf(ent, PRINT_HIGH, "This command needs matchmode to be enabled\n");
		return;
	}

	if(ctf->value) {
		gi.cprintf(ent, PRINT_HIGH, "You can't change teamnames in ctf mode\n");
		return;
	}

	if (!ent->client->resp.captain) {
		gi.cprintf(ent, PRINT_HIGH, "You need to be a captain for that\n");
		return;
	}
	team = ent->client->resp.team;
	if (teams[team].ready) {
		gi.cprintf(ent, PRINT_HIGH, "You can't use this while 'ready'\n");
		return;
	}
	if (team_round_going || team_game_going) {
		gi.cprintf(ent, PRINT_HIGH, "You can't use this while playing\n");
		return;
	}
	i = gi.argc();
	if (i < 2) {
		gi.cprintf(ent, PRINT_HIGH, "Your team name is %s\n", teams[team].name);
		return;
	}

	Q_strncpyz(temp, gi.argv(1), sizeof(temp));
	for (u = 2; u <= i; u++) {
		Q_strncatz(temp, " ", sizeof(temp));
		Q_strncatz(temp, gi.argv(u), sizeof(temp));
	}
	temp[18] = 0;

	gi.dprintf("%s (team %i) is now known as %s\n", teams[team].name, team, temp);
	IRC_printf(IRC_T_GAME, "%n (team %i) is now known as %n", teams[team].name, team, temp);
	strcpy(teams[team].name, temp);
	gi.cprintf(ent, PRINT_HIGH, "New team name: %s\n", teams[team].name);

}

void Cmd_Teamskin_f(edict_t * ent)
{
	int team;
	char *s;
/*	int i;
	edict_t *e;*/

	if (!matchmode->value) {
		gi.cprintf(ent, PRINT_HIGH, "This command needs matchmode to be enabled\n");
		return;
	}

	team = ent->client->resp.team;
	if(team == NOTEAM) {
		gi.cprintf(ent, PRINT_HIGH, "You need to be on a team for that...\n");
		return;
	}
	if (!ent->client->resp.captain) {
		gi.cprintf(ent, PRINT_HIGH, "You need to be a captain for that\n");
		return;
	}
	if (teams[team].ready) {
		gi.cprintf(ent, PRINT_HIGH, "You can't use this while 'ready'\n");
		return;
	}
	if (team_round_going || team_game_going) {
		gi.cprintf(ent, PRINT_HIGH, "You can't use this while playing\n");
		return;
	}
	if (gi.argc() < 2) {
		gi.cprintf(ent, PRINT_HIGH, "Your team skin is %s\n", teams[team].skin);
		return;
	}

	s = gi.argv(1);
	if(!strcmp(s, teams[team].skin)) {
		gi.cprintf(ent, PRINT_HIGH, "Your team skin is already %s\n", s);
		return;
	}

	Q_strncpyz(teams[team].skin, s, sizeof(teams[team].skin));
	if(ctf->value) {
		s = strchr(teams[team].skin, '/');
		if(s)
			s[1] = 0;
		else
			strcpy(teams[team].skin, "male/");
		Q_strncatz(teams[team].skin, team == 1 ? CTF_TEAM1_SKIN : CTF_TEAM2_SKIN, sizeof(teams[team].skin));
	}

	sprintf(teams[team].skin_index, "../players/%s_i", teams[team].skin);

/*	for (i = 1; i <= maxclients->value; i++) { //lets update players skin
		e = g_edicts + i;
		if (!e->inuse)
			continue;

		if(e->client->resp.team == team)
			AssignSkin(e, teams[team].skin, false);
	}*/
	gi.cprintf(ent, PRINT_HIGH, "New team skin: %s\n", teams[team].skin);
}

void Cmd_TeamLock_f(edict_t * ent, int a_switch)
{
	char msg[128];

	if (!matchmode->value) {
		gi.cprintf(ent, PRINT_HIGH, "This command needs matchmode to be enabled\n");
		return;
	}

	if (!mm_allowlock->value)
		gi.cprintf(ent, PRINT_HIGH, "Team locking is disabled on this server\n");
	else if (!ent->client->resp.team)
		gi.cprintf(ent, PRINT_HIGH, "You are not on a team\n");
	else if (!ent->client->resp.captain)
		gi.cprintf(ent, PRINT_HIGH, "You are not the captain of your team\n");
	else if (a_switch == 1 && teams[ent->client->resp.team].locked)
		gi.cprintf(ent, PRINT_HIGH, "Your team is already locked\n");
	else if (a_switch == 0 && !teams[ent->client->resp.team].locked)
		gi.cprintf(ent, PRINT_HIGH, "Your team isn't locked\n");
	else {
		teams[ent->client->resp.team].locked = a_switch;

		if (a_switch == 1)
			sprintf(msg, "%s locked", TeamName(ent->client->resp.team));
		else
			sprintf(msg, "%s unlocked", TeamName(ent->client->resp.team));

		CenterPrintAll(msg);
	}
}

void Cmd_SetAdmin_f (edict_t * ent)
{
	if(!matchmode->value) {
		gi.cprintf(ent, PRINT_HIGH, "Matchmode is not enabled on this server.\n");
		return;
	}

	if (gi.argc () < 2)
	{
		gi.cprintf (ent, PRINT_HIGH, "Usage:  matchadmin <password>\n");
		return;
	}

	if(strcmp (mm_adminpwd->string, "0") == 0)
	{
		gi.cprintf (ent, PRINT_HIGH, "Match admin mode is not enabled on this server..\n");
		return;
	}

	if (strcmp (mm_adminpwd->string, gi.argv (1)) == 0)
	{
		if (ent->client->resp.admin)
		{
			gi.cprintf (ent, PRINT_HIGH, "You are no longer a match admin.\n");
			gi.dprintf ("%s is no longer a match admin\n", ent->client->pers.netname);
			IRC_printf (IRC_T_GAME, "%n is no longer a match admin", ent->client->pers.netname);
			ent->client->resp.admin = 0;
		}
		else
		{
			gi.cprintf (ent, PRINT_HIGH, "You are now a match admin.\n");
			gi.dprintf ("%s is now a match admin\n", ent->client->pers.netname);
			IRC_printf (IRC_T_GAME, "%n is now a match admin", ent->client->pers.netname);
			ent->client->resp.admin = 1;
		}
	}
	else
		gi.cprintf (ent, PRINT_HIGH, "Wrong password\n");

}

void Cmd_ResetScores_f(edict_t * ent)
{
	int i, otherCaptain = 0;
	edict_t *e;

	if (!matchmode->value) {
		gi.cprintf(ent, PRINT_HIGH, "This command needs matchmode to be enabled\n");
		return;
	}

	if(ent->client->resp.admin) //Admins can resetscores
	{
		ResetScores(true);
		gi.bprintf(PRINT_HIGH, "Scores and time were reset by match admin %s\n", ent->client->pers.netname);
		return;
	}

	if (ent->client->resp.team == NOTEAM) {
		gi.cprintf(ent, PRINT_HIGH, "You need to be on a team for that...\n");
		return;
	}
	if (!ent->client->resp.captain) {
		gi.cprintf(ent, PRINT_HIGH, "You need to be a captain for that\n");
		return;
	}

	if(teams[ent->client->resp.team].wantReset)
	{
		teams[ent->client->resp.team].wantReset = 0;
		for (i = 1; i <= maxclients->value; i++) {
			e = g_edicts + i;
			if (!e->inuse || !e->client->resp.captain)
				continue;

			if(e->client->resp.team != NOTEAM && e->client->resp.team != ent->client->resp.team)
				gi.cprintf(e, PRINT_HIGH, "Team %i doesn't want to reset afterall", ent->client->resp.team);
		}
		gi.cprintf(ent, PRINT_HIGH, "Your score reset request cancelled\n");
		return;
	}

	teams[ent->client->resp.team].wantReset = 1;
	for(i = TEAM1; i<teamCount+1; i++) {
		if(!teams[i].wantReset)
			break;
	}
	if(i == teamCount+1)
	{
		ResetScores(true);
		gi.bprintf(PRINT_HIGH, "Scores and time were reset by request of captains\n");
		return;
	}

	for (i = 1; i <= maxclients->value; i++) {
		e = g_edicts + i;
		if (!e->inuse || !e->client->resp.captain)
			continue;

		if(e->client->resp.team != NOTEAM && e->client->resp.team != ent->client->resp.team) {
			gi.cprintf(e, PRINT_HIGH, "Team %i wants to reset scores, type 'resetscores' to accept\n", ent->client->resp.team);
			otherCaptain = 1;
		}
	}
	if(otherCaptain)
		gi.cprintf(ent, PRINT_HIGH, "Your score reset request was sent to the other team captain\n");
	else
		gi.cprintf(ent, PRINT_HIGH, "Other team needs a captain and his acceptance to reset the scores\n");

}

void Cmd_TogglePause_f(edict_t * ent, qboolean pause)
{
	static int lastPaused = 0;

	if (!matchmode->value) {
		gi.cprintf(ent, PRINT_HIGH, "This command needs matchmode to be enabled\n");
		return;
	}

	if ((int)mm_pausecount->value < 1) {
		gi.cprintf(ent, PRINT_HIGH, "Pause is disabled\n");
		return;
	}

	if (ent->client->resp.team == NOTEAM) {
		gi.cprintf(ent, PRINT_HIGH, "You need to be on a team for that...\n");
		return;
	}

	if (!team_round_going) {
		gi.cprintf(ent, PRINT_HIGH, "No match running, so why pause?\n");
		//return;
	}

	if (ent->client->resp.subteam) {
		gi.cprintf(ent, PRINT_HIGH, "You can't pause when substitute\n");
		return;
	}

	if(pause)
	{
		if(pause_time > 0)
		{
			gi.cprintf(ent, PRINT_HIGH, "Game is already paused you silly\n", time);
			return;
		}
		if (level.intermissiontime) {
			gi.cprintf(ent, PRINT_HIGH, "Can't pause in an intermission.\n");
			return;
		}
		if(teams[ent->client->resp.team].pauses_used >= (int)mm_pausecount->value)
		{
			gi.cprintf(ent, PRINT_HIGH, "Your team doesn't have any pauses left.\n");
			return;
		}
		teams[ent->client->resp.team].pauses_used++;

		CenterPrintAll (va("Game paused by %s\nTeam %i has %i pauses left", ent->client->pers.netname, ent->client->resp.team, (int)mm_pausecount->value - teams[ent->client->resp.team].pauses_used));
		pause_time = (int)mm_pausetime->value * 600;
		lastPaused = ent->client->resp.team;
	}
	else
	{
		if(!pause_time)
		{
			gi.cprintf(ent, PRINT_HIGH, "Game is not paused\n", time);
			return;
		}
		if(!lastPaused)
		{
			gi.cprintf(ent, PRINT_HIGH, "Already unpausing\n");
			return;
		}
		if(lastPaused != ent->client->resp.team)
		{
			gi.cprintf(ent, PRINT_HIGH, "You can't unpause when paused by the other team\n");
			return;
		}
		pause_time = 100;
		lastPaused = 0;
	}
}

