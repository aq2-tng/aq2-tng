//-----------------------------------------------------------------------------
// Teamplay-related code for Action (formerly Axshun).  
// Some of this is borrowed from Zoid's CTF (thanks Zoid)
// -Fireblade
//
// $Id: a_team.c,v 1.88 2003/06/15 21:43:53 igor Exp $
//
//-----------------------------------------------------------------------------
// $Log: a_team.c,v $
// Revision 1.88  2003/06/15 21:43:53  igor
// added IRC client
//
// Revision 1.87  2002/04/01 14:00:08  freud
// After extensive checking I think I have found the spawn bug in the new
// system.
//
// Revision 1.86  2002/03/28 20:53:45  deathwatch
// updated credits (forgot QNI in the clan list)
//
// Revision 1.85  2002/03/28 13:34:01  deathwatch
// updated credits
//
// Revision 1.84  2002/03/28 12:10:11  freud
// Removed unused variables (compiler warnings).
// Added cvar mm_allowlock.
//
// Revision 1.83  2002/03/28 11:46:03  freud
// stat_mode 2 and timelimit 0 did not show stats at end of round.
// Added lock/unlock.
// A fix for use_oldspawns 1, crash bug.
//
// Revision 1.82  2002/03/27 15:16:56  freud
// Original 1.52 spawn code implemented for use_newspawns 0.
// Teamplay, when dropping bandolier, your drop the grenades.
// Teamplay, cannot pick up grenades unless wearing bandolier.
//
// Revision 1.81  2002/03/26 21:49:01  ra
// Bufferoverflow fixes
//
// Revision 1.80  2002/03/25 17:44:17  freud
// Small fix
//
// Revision 1.79  2002/03/24 22:45:53  freud
// New spawn code again, bad commit last time..
//
// Revision 1.77  2002/02/27 16:07:13  deathwatch
// Updated Credits menu
//
// Revision 1.76  2002/02/23 18:52:36  freud
// Added myself to the credits menu :)
//
// Revision 1.75  2002/02/23 18:33:52  freud
// Fixed newline bug with announcer (EXCELLENT.. 1 FRAG LEFT) for logfiles
//
// Revision 1.74  2002/02/23 18:12:14  freud
// Added newlines back to the CenterPrintAll for IMPRESSIVE, EXCELLENT,
// ACCURACY and X FRAGS Left, it was screwing up the logfile.
//
// Revision 1.73  2002/02/18 13:55:35  freud
// Added last damaged players %P
//
// Revision 1.72  2002/02/17 23:25:29  freud
// Fixed a small bug where stats were sent twice on votes and roundlimits
//
// Revision 1.71  2002/02/17 19:04:14  freud
// Possible bugfix for overflowing clients with stat_mode set.
//
// Revision 1.70  2002/02/03 01:07:28  freud
// more fixes with stats
//
// Revision 1.69  2002/02/01 12:54:08  ra
// messin with stat_mode
//
// Revision 1.68  2002/01/24 10:39:32  ra
// Removed an old debugging statement
//
// Revision 1.67  2002/01/24 02:24:56  deathwatch
// Major update to Stats code (thanks to Freud)
// new cvars:
// stats_afterround - will display the stats after a round ends or map ends
// stats_endmap - if on (1) will display the stats scoreboard when the map ends
//
// Revision 1.66  2001/12/30 04:00:09  ra
// Players that switch between teams before dying should also be punished.
//
// Revision 1.65  2001/12/30 03:39:52  ra
// Added to punishkills system if people do team none just before dieing
//
// Revision 1.64  2001/12/27 23:29:46  slicerdw
// Reset sub and captain vars when doing "team none"
//
// Revision 1.63  2001/12/05 15:27:35  igor_rock
// improved my english (actual -> current :)
//
// Revision 1.62  2001/12/02 16:41:52  igor_rock
// corrected the teamscores (they where switched)
//
// Revision 1.61  2001/12/02 16:16:16  igor_rock
// added "Actual Score" message after Round
//
// Revision 1.60  2001/11/25 19:09:25  slicerdw
// Fixed Matchtime
//
// Revision 1.59  2001/11/16 13:01:39  deathwatch
// Fixed 'no team wins' sound - it wont play now with use_warnings 0
// Precaching misc/flashlight.wav
//
// Revision 1.58  2001/11/03 17:33:06  ra
// Yes another warning gone
//
// Revision 1.57  2001/11/03 17:21:57  deathwatch
// Fixed something in the time command, removed the .. message from the voice command, fixed the vote spamming with mapvote, removed addpoint command (old pb command that wasnt being used). Some cleaning up of the source at a few points.
//
// Revision 1.56  2001/11/02 16:07:47  ra
// Changed teamplay spawn code so that teams dont spawn in the same place
// often in a row
//
// Revision 1.55  2001/09/30 03:09:34  ra
// Removed new stats at end of rounds and created a new command to
// do the same functionality.   Command is called "time"
//
// Revision 1.54  2001/09/29 20:18:26  ra
// Its boo boo day today
//
// Revision 1.53  2001/09/29 19:54:04  ra
// Made a CVAR to turn off extratimingstats
//
// Revision 1.52  2001/09/29 19:16:47  ra
// Made a boo boo in the timing stuff.
//
// Revision 1.51  2001/09/29 19:15:38  ra
// Added some more timing stuff
//
// Revision 1.50  2001/09/29 17:21:04  ra
// Fix a small 3teams bug
//
// Revision 1.49  2001/09/28 16:24:19  deathwatch
// use_rewards now silences the teamX wins sounds and added gibbing for the Shotgun
//
// Revision 1.48  2001/09/28 15:03:26  ra
// replacing itoa with a sprintf() call 'cause itoa is MIA on Linux
//
// Revision 1.47  2001/09/28 13:48:34  ra
// I ran indent over the sources. All .c and .h files reindented.
//
// Revision 1.46  2001/09/28 13:44:23  slicerdw
// Several Changes / improvements
//
// Revision 1.45  2001/09/26 18:13:48  slicerdw
// Fixed the roundtimelimit thingy which was ending the game ( confused with roundlimit )
//
// Revision 1.44  2001/08/08 12:42:22  slicerdw
// Ctf Should finnaly be fixed now, lets hope so
//
// Revision 1.43  2001/08/06 14:38:44  ra
// Adding UVtime for ctf
//
// Revision 1.42  2001/08/01 13:54:26  ra
// Hack to keep scoreboard from revealing whos alive during matches
//
// Revision 1.41  2001/07/30 10:17:59  igor_rock
// added some parenthesis in the 3 minutes warning clause
//
// Revision 1.40  2001/07/28 19:30:05  deathwatch
// Fixed the choose command (replaced weapon for item when it was working with items)
// and fixed some tabs on other documents to make it more readable
//
// Revision 1.39  2001/07/16 19:02:06  ra
// Fixed compilerwarnings (-g -Wall).  Only one remains.
//
// Revision 1.38  2001/07/15 02:08:40  slicerdw
// Added the "Team" section on scoreboard2 using matchmode
//
// Revision 1.37  2001/07/10 13:16:57  ra
// Fixed bug where the "3 MINUTES LEFT" warning gets printed at the begining
// of rounds that are only 2 minutes long.
//
// Revision 1.36  2001/07/09 17:55:50  slicerdw
// Small change on the Board
//
// Revision 1.35  2001/06/28 20:29:58  igor_rock
// changed the scoreboard to redruce length (and changed the debug output to report at 1023 lenght)
//
// Revision 1.34  2001/06/28 14:36:40  deathwatch
// Updated the Credits Menu a slight bit (added Kobra)
//
// Revision 1.33  2001/06/27 20:24:03  igor_rock
// changed the matchmode scoreboard completly (did a new one)
//
// Revision 1.32  2001/06/27 17:50:09  igor_rock
// fixed the vote reached bug in teamplay and matchmode
//
// Revision 1.31  2001/06/27 16:58:14  igor_rock
// corrected some limchasecam bugs
//
// Revision 1.30  2001/06/25 20:59:17  ra
//
// Adding my clantag.
//
// Revision 1.29  2001/06/22 18:37:01  igor_rock
// fixed than damn limchasecam bug - eentually :)
//
// Revision 1.28  2001/06/20 19:23:19  igor_rock
// added vcehckvotes for ctf mode "in game" ;)
//
// Revision 1.27  2001/06/20 07:21:21  igor_rock
// added use_warnings to enable/disable time/frags left msgs
// added use_rewards to enable/disable eimpressive, excellent and accuracy msgs
// change the configfile prefix for modes to "mode_" instead "../mode-" because
// they don't have to be in the q2 dir for doewnload protection (action dir is sufficient)
// and the "-" is bad in filenames because of linux command line parameters start with "-"
//
// Revision 1.26  2001/06/19 20:56:45  igor_rock
// fixed the matchmode scoreboard - finally :-)
//
// Revision 1.25  2001/06/19 18:56:38  deathwatch
// New Last killed target system
//
// Revision 1.24  2001/06/18 20:29:42  igor_rock
// subs don't respawn (says slicer :)
// splitted the scoreboard for matchmode (new if/else branch)
//
// Revision 1.23  2001/06/06 18:57:14  slicerdw
// Some tweaks on Ctf and related things
//
// Revision 1.21  2001/06/05 20:00:14  deathwatch
// Added ICE-M to credits, fixed some stuff
//
// Revision 1.20  2001/06/05 18:47:11  slicerdw
// Small tweaks to matchmode
//
// Revision 1.18  2001/06/01 19:18:42  slicerdw
// Added Matchmode Code
//
// Revision 1.17  2001/05/31 16:58:14  igor_rock
// conflicts resolved
//
// Revision 1.16.2.4  2001/05/27 11:47:53  igor_rock
// added .flg file support and timelimit bug fix
//
// Revision 1.16.2.3  2001/05/25 18:59:52  igor_rock
// Added CTF Mode completly :)
// Support for .flg files is still missing, but with "real" CTF maps like
// tq2gtd1 the ctf works fine.
// (I hope that all other modes still work, just tested DM and teamplay)
//
// Revision 1.16.2.2  2001/05/20 18:54:19  igor_rock
// added original ctf code snippets from zoid. lib compilesand runs but
// doesn't function the right way.
// Jsut committing these to have a base to return to if something wents
// awfully wrong.
//
// Revision 1.16.2.1  2001/05/20 15:17:31  igor_rock
// removed the old ctf code completly
//
// Revision 1.16  2001/05/20 12:54:18  igor_rock
// Removed newlines from Centered Messages like "Impressive"
//
// Revision 1.15  2001/05/17 16:18:13  igor_rock
// added wp_flags and did some itm_flags and other corrections
//
// Revision 1.14  2001/05/17 14:54:47  igor_rock
// added itm_flags for teamplay and ctf
//
// Revision 1.13  2001/05/13 01:23:01  deathwatch
// Added Single Barreled Handcannon mode, made the menus and scoreboards
// look nicer and made the voice command a bit less loud.
//
// Revision 1.12  2001/05/12 19:17:03  slicerdw
// Changed The Join and Weapon choosing Menus
//
// Revision 1.11  2001/05/12 18:38:27  deathwatch
// Tweaked MOTD and Menus some more
//
// Revision 1.10  2001/05/12 17:36:33  deathwatch
// Edited the version variables and updated the menus. Added variables:
// ACTION_VERSION, TNG_VERSION and TNG_VERSION2
//
// Revision 1.9  2001/05/12 14:52:47  mort
// Fixed bug of people being able to respawn when choosing a new item
//
// Revision 1.8  2001/05/12 13:48:58  mort
// Fixed CTF ForceSpawn bug
//
// Revision 1.7  2001/05/12 08:20:01  mort
// CTF bug fix, makes sure flags have actually spawned before certain functions attempt to use them
//
// Revision 1.6  2001/05/11 16:12:03  mort
// Updated path locations for CTF flag loading and CTF hacked spawns
//
// Revision 1.5  2001/05/11 16:07:25  mort
// Various CTF bits and pieces...
//
// Revision 1.4  2001/05/11 12:21:18  slicerdw
// Commented old Location support ( ADF ) With the ML/ETE Compatible one
//
// Revision 1.2  2001/05/07 20:06:45  igor_rock
// changed sound dir from sound/rock to sound/tng
//
// Revision 1.1.1.1  2001/05/06 17:24:38  igor_rock
// This is the PG Bund Edition V1.25 with all stuff laying around here...
//
//-----------------------------------------------------------------------------

#include "g_local.h"
#include "cgf_sfx_glass.h"

qboolean team_game_going = false;	// is a team game going right now?
qboolean team_round_going = false;	// is an actual round of a team game going right now?

int team_round_countdown = 0;	// countdown variable for start of a round
int rulecheckfrequency = 0;	// accumulator variable for checking rules every 1.5 secs
int lights_camera_action = 0;	// countdown variable for "lights...camera...action!" 
int timewarning = 0;		// countdown variable for "x Minutes left"
int fragwarning = 0;		// countdown variable for "x Frags left"
int holding_on_tie_check = 0;	// when a team "wins", countdown for a bit and wait...
int current_round_length = 0;	// frames that the current team round has lasted
int round_delay_time = 0;	// time gap between round end and new round

team_t teams[TEAM_TOP];
int	teamCount = 2;

#define MAX_SPAWNS 512		// max DM spawn points supported

edict_t *potential_spawns[MAX_SPAWNS];
int num_potential_spawns;
edict_t *teamplay_spawns[MAX_TEAMS];
trace_t trace_t_temp;		// used by our trace replace macro in ax_team.h

int num_teams = 3;		// teams in current game, fixed at 2 for now...

// <TNG:Freud New spawning variables>
int NS_num_used_farteamplay_spawns[MAX_TEAMS];
int NS_num_potential_spawns[MAX_TEAMS];
edict_t *NS_potential_spawns[MAX_TEAMS][MAX_SPAWNS];
edict_t *NS_used_farteamplay_spawns[MAX_TEAMS][MAX_SPAWNS];
int NS_randteam;
// </TNG:Freud>

transparent_list_t *transparent_list = NULL;

void CreditsMenu (edict_t * ent, pmenu_t * p);

void InitTransparentList ()
{
	if (transparent_list != NULL)
	{
		transparent_list_t *p, *q;

		p = transparent_list;
		while (p != NULL)
		{
			q = p->next;
			gi.TagFree(p);
			p = q;
		}
		transparent_list = NULL;
	}
}

void AddToTransparentList (edict_t * ent)
{
	transparent_list_t *p, *n;

	n =	(transparent_list_t *) gi.TagMalloc (sizeof (transparent_list_t), TAG_GAME);
	if (n == NULL)
	{
		gi.dprintf ("Out of memory\n");
		exit (1);
	}
	n->ent = ent;
	n->next = NULL;

	if (transparent_list == NULL)
	{
		transparent_list = n;
	}
	else
	{
		p = transparent_list;
		while (p->next != NULL)
		{
			p = p->next;
		}
		p->next = n;
	}
}

void RemoveFromTransparentList (edict_t * ent)
{
	transparent_list_t *p, *q, *r;

	if (transparent_list != NULL)
	{
		if (transparent_list->ent == ent)
		{
			q = transparent_list->next;
			gi.TagFree (transparent_list);
			transparent_list = q;
			return;
		}
		else
		{
			p = transparent_list;
			q = p->next;
			while (q != NULL)
			{
				if (q->ent == ent)
				{
					r = q->next;
					gi.TagFree (q);
					p->next = r;
					return;
				}
				p = p->next;
				q = p->next;
			}
		}
	}

	gi.dprintf("Warning: attempt to RemoveFromTransparentList when not in it\n");
}

void TransparentListSet (solid_t solid_type)
{
	transparent_list_t *p = transparent_list;

	while (p != NULL)
	{
		p->ent->solid = solid_type;
		gi.linkentity (p->ent);
		p = p->next;
	}
}

void ReprintMOTD (edict_t * ent, pmenu_t * p)
{
	PMenu_Close (ent);
	PrintMOTD (ent);
}

void JoinTeamAuto (edict_t * ent, pmenu_t * p)
{
	int i, team = TEAM1, num1 = 0, num2 = 0, num3 = 0, score1, score2, score3;

	for (i = 0; i < (int)maxclients->value; i++)
	{
		if (!g_edicts[i + 1].inuse)
			continue;
		if (game.clients[i].resp.team == TEAM1)
			num1++;
		else if (game.clients[i].resp.team == TEAM2)
			num2++;
		else if (game.clients[i].resp.team == TEAM3)
			num3++;
	}

	score1 = teams[TEAM1].score;
	score2 = teams[TEAM2].score;
	score3 = teams[TEAM3].score;

	if(ctf->value) {
		CTFCalcScores();
		GetCTFScores(&score1, &score2);
	}

	/* there are many different things to consider when selecting a team */
	if (num1 > num2 || (num1 == num2 && score1 > score2))
		team = TEAM2;

	if (use_3teams->value)
	{
		if (team == TEAM1)
		{
			if (num1 > num3 || (num1 == num3 && score1 > score3))
				team = TEAM3;
		}
		else
		{
			if (num2 > num3 || (num2 == num3 && score2 > score3))
				team = TEAM3;
		}
	}

	JoinTeam(ent, team, 0);
}

void JoinTeam1 (edict_t * ent, pmenu_t * p)
{
	JoinTeam (ent, TEAM1, 0);
}

void JoinTeam2 (edict_t * ent, pmenu_t * p)
{
	JoinTeam (ent, TEAM2, 0);
}

void JoinTeam3 (edict_t * ent, pmenu_t * p)
{
	if (use_3teams->value)
		JoinTeam (ent, TEAM3, 0);
}

void LeaveTeams (edict_t * ent, pmenu_t * p)
{
	LeaveTeam (ent);
	PMenu_Close (ent);
	OpenJoinMenu (ent);
}

void SelectWeapon2 (edict_t * ent, pmenu_t * p)
{
	ent->client->resp.weapon = GET_ITEM(MP5_NUM);
	PMenu_Close (ent);
	OpenItemMenu (ent);
	//PG BUND
	unicastSound(ent, gi.soundindex("weapons/mp5slide.wav"), 1.0);
}

void SelectWeapon3 (edict_t * ent, pmenu_t * p)
{
	ent->client->resp.weapon = GET_ITEM(M3_NUM);
	PMenu_Close (ent);
	OpenItemMenu (ent);
	//PG BUND
	unicastSound(ent, gi.soundindex("weapons/m3in.wav"), 1.0);
}

void SelectWeapon4 (edict_t * ent, pmenu_t * p)
{
	ent->client->resp.weapon = GET_ITEM(HC_NUM);
	PMenu_Close (ent);
	OpenItemMenu (ent);
	//PG BUND
	unicastSound(ent, gi.soundindex("weapons/cclose.wav"), 1.0);
}

void SelectWeapon5 (edict_t * ent, pmenu_t * p)
{
	ent->client->resp.weapon = GET_ITEM(SNIPER_NUM);
	PMenu_Close (ent);
	OpenItemMenu (ent);
	//PG BUND
	unicastSound(ent, gi.soundindex("weapons/ssgbolt.wav"), 1.0);
}

void SelectWeapon6 (edict_t * ent, pmenu_t * p)
{
	ent->client->resp.weapon = GET_ITEM(M4_NUM);
	PMenu_Close (ent);
	OpenItemMenu (ent);
	//PG BUND
	unicastSound(ent, gi.soundindex("weapons/m4a1slide.wav"), 1.0);
}

void SelectWeapon0 (edict_t * ent, pmenu_t * p)
{
	ent->client->resp.weapon = GET_ITEM(KNIFE_NUM);
	PMenu_Close (ent);
	OpenItemMenu (ent);
	//PG BUND
	unicastSound(ent, gi.soundindex("weapons/stab.wav"), 1.0);
}

void SelectWeapon9 (edict_t * ent, pmenu_t * p)
{
	ent->client->resp.weapon = GET_ITEM(DUAL_NUM);
	PMenu_Close (ent);
	OpenItemMenu (ent);
	//PG BUND
	unicastSound(ent, gi.soundindex("weapons/mk23slide.wav"), 1.0);
}

void SelectItem1 (edict_t * ent, pmenu_t * p)
{
	ent->client->resp.item = GET_ITEM(KEV_NUM);
	PMenu_Close (ent);
	//PG BUND
	unicastSound(ent, gi.soundindex("misc/veston.wav"), 1.0);
}

void SelectItem2 (edict_t * ent, pmenu_t * p)
{
	ent->client->resp.item = GET_ITEM(LASER_NUM);
	PMenu_Close (ent);
	//PG BUND
	unicastSound(ent, gi.soundindex("misc/lasersight.wav"), 1.0);
}

void SelectItem3 (edict_t * ent, pmenu_t * p)
{
	ent->client->resp.item = GET_ITEM(SLIP_NUM);
	PMenu_Close (ent);
	//PG BUND
	unicastSound(ent, gi.soundindex("misc/veston.wav"), 1.0);
}

void SelectItem4 (edict_t * ent, pmenu_t * p)
{
	ent->client->resp.item = GET_ITEM(SIL_NUM);
	PMenu_Close (ent);
	//PG BUND
	unicastSound(ent, gi.soundindex("misc/screw.wav"), 1.0);
}

void SelectItem5 (edict_t * ent, pmenu_t * p)
{
	ent->client->resp.item = GET_ITEM(BAND_NUM);
	PMenu_Close (ent);
	//PG BUND
	unicastSound(ent, gi.soundindex("misc/veston.wav"), 1.0);
}

void SelectItem6 (edict_t * ent, pmenu_t * p)
{
	ent->client->resp.item = GET_ITEM(HELM_NUM);
	PMenu_Close (ent);
	//PG BUND
	unicastSound(ent, gi.soundindex("misc/veston.wav"), 1.0);
}

void CreditsReturnToMain (edict_t * ent, pmenu_t * p)
{
	PMenu_Close (ent);
	OpenJoinMenu (ent);
}

//PG BUND BEGIN
void DoAGoodie (edict_t * ent, pmenu_t * p)
{
  //PG BUND
	unicastSound(ent, gi.soundindex("boss3/bs3idle1.wav"), 1.0);
	//stuffcmd (ent, "play boss3/bs3idle1.wav");
}

//PG BUND END

// AQ2:TNG - Igor adding the Rock-Sound ;-)
void RockClan (edict_t * ent, pmenu_t * p)
{
  gi.cprintf (ent, PRINT_HIGH, "Let's Rock! http://www.rock-clan.de/\n");
  //PG BUND
  unicastSound(ent, gi.soundindex("user/letsrock.wav"), 1.0);
  //stuffcmd (ent, "play user/letsrock.wav");
}
// AQ2:TNG - End Rock-Sound

// AQ2:TNG Deathwatch - Just for slicer :)
void SlicersCat (edict_t * ent, pmenu_t * p)
{
  gi.cprintf (ent, PRINT_HIGH, "sLiCeR [dW] couldn't have done it without his cat!\n");
  //PG BUND
  unicastSound(ent, gi.soundindex("makron/laf4.wav"), 1.0);
  //stuffcmd (ent, "play makron/laf4.wav");
}
// AQ2:TNG End

// AQ2:TNG Deathwatch - Just for QNI ppl
void QuakeNigguhz (edict_t * ent, pmenu_t * p)
{
  gi.cprintf (ent, PRINT_HIGH, "For all the homies!\n");
  //PG BUND
  unicastSound(ent, gi.soundindex("world/xian1.wav"), 1.0);
  //stuffcmd (ent, "play world/xian1.wav");
}

// AQ2:TNG Deathwatch - Editing all menus to show the correct credits, version, names, locations, urls, etc
pmenu_t creditsmenu[] = {
  {"*" TNG_VERSION, PMENU_ALIGN_CENTER, NULL, NULL},
  {"ùûûûûûûûûûûûûûûûûûûûûûûûûûü", PMENU_ALIGN_CENTER, NULL, NULL},
  {"*Design Team", PMENU_ALIGN_LEFT, NULL, NULL},
  {NULL, PMENU_ALIGN_LEFT, NULL, NULL},
  {"Deathwatch", PMENU_ALIGN_LEFT, NULL, DoAGoodie},
	{"Elviz", PMENU_ALIGN_LEFT, NULL, DoAGoodie},
	{"Freud [QNI]", PMENU_ALIGN_LEFT, NULL, QuakeNigguhz},
  {"Igor[Rock]", PMENU_ALIGN_LEFT, NULL, RockClan},
  {"JBravo[QNI]", PMENU_ALIGN_LEFT, NULL, QuakeNigguhz},
  {"sLiCeR [dW]", PMENU_ALIGN_LEFT, NULL, SlicersCat},
  {NULL, PMENU_ALIGN_LEFT, NULL, NULL},
  {"*Credits", PMENU_ALIGN_LEFT, NULL, NULL},
  {"(in no particular order)", PMENU_ALIGN_LEFT, NULL, NULL},
  {NULL, PMENU_ALIGN_LEFT, NULL, NULL},
  {"Clan Rock, dW, QNI & DP,", PMENU_ALIGN_LEFT, NULL, NULL},
  {"Kobra, Zarjazz,", PMENU_ALIGN_LEFT, NULL, NULL},
  {"Killerbee, Rookie[Rock],", PMENU_ALIGN_LEFT, NULL, NULL},
  {"PG Bund[Rock], Mort,", PMENU_ALIGN_LEFT, NULL, NULL},
  {"ICE-M, Palmtree,", PMENU_ALIGN_LEFT, NULL, NULL},
	{"Tempfile, Blackmonk,", PMENU_ALIGN_LEFT, NULL, NULL},
	{"Dome, Papst, Apr/ Maniac", PMENU_ALIGN_LEFT, NULL, NULL},
  {NULL, PMENU_ALIGN_LEFT, NULL, NULL},
  {"Return to main menu", PMENU_ALIGN_LEFT, NULL, CreditsReturnToMain},
  {"TAB to exit menu", PMENU_ALIGN_LEFT, NULL, NULL},
  {NULL, PMENU_ALIGN_LEFT, NULL, NULL},
  {"v" ACTION_VERSION, PMENU_ALIGN_RIGHT, NULL, NULL},
//PG BUND END
};

pmenu_t weapmenu[] = {
  {"*" TNG_VERSION, PMENU_ALIGN_CENTER, NULL, NULL},
  {"ùûûûûûûûûûûûûûûûûûûûûûûûûûü", PMENU_ALIGN_CENTER, NULL, NULL},
  {"Select your Weapon", PMENU_ALIGN_CENTER, NULL, NULL},
  {NULL, PMENU_ALIGN_LEFT, NULL, NULL},
  //AQ2:TNG - Igor adding wp_flags
  {NULL, PMENU_ALIGN_LEFT, NULL, NULL},	// "MP5/10 Submachinegun", SelectWeapon2
  {NULL, PMENU_ALIGN_LEFT, NULL, NULL},	// "M3 Super90 Assault Shotgun", SelectWeapon3
  {NULL, PMENU_ALIGN_LEFT, NULL, NULL},	// "Handcannon", SelectWeapon4
  {NULL, PMENU_ALIGN_LEFT, NULL, NULL},	// "SSG 3000 Sniper Rifle", SelectWeapon5
  {NULL, PMENU_ALIGN_LEFT, NULL, NULL},	// "M4 Assault Rifle", SelectWeapon6
  {NULL, PMENU_ALIGN_LEFT, NULL, NULL},	// "Combat Knives", SelectWeapon0
  {NULL, PMENU_ALIGN_LEFT, NULL, NULL},	// "Akimbo Pistols", SelectWeapon9
  //AQ2:TNG End adding wp_flags
  {NULL, PMENU_ALIGN_LEFT, NULL, NULL},
  //AQ2:TNG - Slicer: changing this
  //{"Leave Team", PMENU_ALIGN_LEFT, NULL, LeaveTeams},
  {"Return to Main Menu", PMENU_ALIGN_LEFT, NULL, CreditsReturnToMain},
  {NULL, PMENU_ALIGN_LEFT, NULL, NULL},
  //AQ2:TNG END
  {"Use [ and ] to move cursor", PMENU_ALIGN_LEFT, NULL, NULL},
  {"ENTER to select", PMENU_ALIGN_LEFT, NULL, NULL},
  {"TAB to exit menu", PMENU_ALIGN_LEFT, NULL, NULL},
  {NULL, PMENU_ALIGN_LEFT, NULL, NULL},
  {"v" ACTION_VERSION, PMENU_ALIGN_RIGHT, NULL, NULL},
};

pmenu_t itemmenu[] = {
  {"*" TNG_VERSION, PMENU_ALIGN_CENTER, NULL, NULL},
  {"ùûûûûûûûûûûûûûûûûûûûûûûûûûü", PMENU_ALIGN_CENTER, NULL, NULL},
  {"Select your Item", PMENU_ALIGN_CENTER, NULL, NULL},
  {NULL, PMENU_ALIGN_LEFT, NULL, NULL},
  //AQ2:TNG Igor adding itm_flags
  {NULL, PMENU_ALIGN_LEFT, NULL, NULL},	// "Kevlar Vest", SelectItem1
  {NULL, PMENU_ALIGN_LEFT, NULL, NULL},	// "Laser Sight", SelectItem2
  {NULL, PMENU_ALIGN_LEFT, NULL, NULL},	// "Stealth Slippers", SelectItem3
  {NULL, PMENU_ALIGN_LEFT, NULL, NULL},	// "Silencer", SelectItem4
  {NULL, PMENU_ALIGN_LEFT, NULL, NULL},	// "Bandolier", SelectItem5
  {NULL, PMENU_ALIGN_LEFT, NULL, NULL},	// "Kevlar Helmet", SelectItem6
  //AQ2:TNG end adding itm_flags
  {NULL, PMENU_ALIGN_LEFT, NULL, NULL},
  {"Use [ and ] to move cursor", PMENU_ALIGN_LEFT, NULL, NULL},
  {"ENTER to select", PMENU_ALIGN_LEFT, NULL, NULL},
  {"TAB to exit menu", PMENU_ALIGN_LEFT, NULL, NULL},
  {NULL, PMENU_ALIGN_LEFT, NULL, NULL},
  {"v" ACTION_VERSION, PMENU_ALIGN_RIGHT, NULL, NULL},
};
//AQ2:TNG - slicer
void VotingMenu (edict_t * ent, pmenu_t * p)
{
	PMenu_Close (ent);
	vShowMenu (ent, "");
}

//AQ2:TNG END
pmenu_t joinmenu[] = {
  {"*" TNG_VERSION, PMENU_ALIGN_CENTER, NULL, NULL},
  {"ùûûûûûûûûûûûûûûûûûûûûûûûûûü", PMENU_ALIGN_CENTER, NULL, NULL},
  {NULL /* lvl name */ , PMENU_ALIGN_CENTER, NULL, NULL},
  {NULL, PMENU_ALIGN_CENTER, NULL, NULL},
  {NULL /* team 1 */ , PMENU_ALIGN_LEFT, NULL, JoinTeam1},
  {NULL, PMENU_ALIGN_LEFT, NULL, NULL},
  {NULL /* team 2 */ , PMENU_ALIGN_LEFT, NULL, JoinTeam2},
  {NULL, PMENU_ALIGN_LEFT, NULL, NULL},
  {NULL /* team 3 */ , PMENU_ALIGN_LEFT, NULL, JoinTeam3},
  {NULL, PMENU_ALIGN_LEFT, NULL, NULL},
  {NULL, PMENU_ALIGN_LEFT, NULL, NULL},
  {NULL /* auto-join */ , PMENU_ALIGN_LEFT, NULL, JoinTeamAuto},
  {NULL, PMENU_ALIGN_LEFT, NULL, NULL},
  //AQ2:TNG - Slicer
  {"Voting & Ignoring Menus", PMENU_ALIGN_LEFT, NULL, VotingMenu},
  //AQ2:TNG END
  {"MOTD", PMENU_ALIGN_LEFT, NULL, ReprintMOTD},
  {"Credits", PMENU_ALIGN_LEFT, NULL, CreditsMenu},
  {NULL, PMENU_ALIGN_LEFT, NULL, NULL},
  {"Use [ and ] to move cursor", PMENU_ALIGN_LEFT, NULL, NULL},
  {"ENTER to select", PMENU_ALIGN_LEFT, NULL, NULL},
  {"TAB to exit menu", PMENU_ALIGN_LEFT, NULL, NULL},
  {"v" ACTION_VERSION, PMENU_ALIGN_RIGHT, NULL, NULL},
};
// AQ2:TNG End

void CreditsMenu (edict_t * ent, pmenu_t * p)
{
	PMenu_Close (ent);
	PMenu_Open (ent, creditsmenu, 4, sizeof (creditsmenu) / sizeof (pmenu_t));
	unicastSound(ent, gi.soundindex("world/elv.wav"), 1.0);
}


char *TeamName (int team)
{
	if (team >= TEAM1 && team <= TEAM3)
		return teams[team].name;
	else
		return "None";
}

void AssignSkin (edict_t * ent, const char *s, qboolean nickChanged)
{
	int playernum = ent - g_edicts - 1;
	char *p;
	char t[MAX_SKINLEN], skin[64] = "\0";

	if (ctf->value && !matchmode->value)
	{
		Q_strncpyz(t, s, sizeof(t));

		// forcing CTF model
		if(ctf_model->string[0]) {
			/* copy at most bytes that the skin name itself fits in with the delimieter and NULL */
			strncpy(t, ctf_model->string, MAX_SKINLEN-strlen(CTF_TEAM1_SKIN)-2);
			strcat(t, "/");
		}

		if ((p = strrchr (t, '/')) != NULL)
			p[1] = 0;
		else
			strcpy (t, "male/");

		switch (ent->client->resp.team)
		{
		case TEAM1:
			Com_sprintf(skin, sizeof(skin), "%s\\%s%s", ent->client->pers.netname, t, CTF_TEAM1_SKIN);
			break;
		case TEAM2:
			Com_sprintf(skin, sizeof(skin), "%s\\%s%s", ent->client->pers.netname, t, CTF_TEAM2_SKIN);
			break;
		default:
			Com_sprintf(skin, sizeof(skin), "%s\\%s", ent->client->pers.netname, s);
			break;
		}
	}
	else
	{
		switch (ent->client->resp.team)
		{
		case TEAM1:
		case TEAM2:
		case TEAM3:
			Com_sprintf(skin, sizeof(skin), "%s\\%s", ent->client->pers.netname, teams[ent->client->resp.team].skin);
			break;
		default:
			Com_sprintf(skin, sizeof(skin), "%s\\%s", ent->client->pers.netname, s);
			break;
		}
	}

	gi.configstring(CS_PLAYERSKINS + playernum, skin);
}


void Team_f (edict_t * ent)
{
	char *t;
	int desired_team = NOTEAM;
	char team[24];

	//PG BUND - BEGIN (Tourney extension)
	if (use_tourney->value)
	{
		gi.cprintf (ent, PRINT_MEDIUM,
		"Currently running tourney mod, team selection is disabled.");
		return;
	}
	//PG BUND - END (Tourney extension)        
	Q_strncpyz(team, gi.args(), sizeof(team));
	t = team;
	//  t = gi.args ();
	if (!*t)
	{
		if (ctf->value)
		{
			gi.cprintf (ent, PRINT_HIGH, "You are on %s.\n",
			CTFTeamName (ent->client->resp.team));
			return;
		}
		else
		{
			gi.cprintf (ent, PRINT_HIGH, "You are on %s.\n",
			TeamName (ent->client->resp.team));
			return;
		}
	}

	if ((int)(realLtime*10.0f) < (ent->client->resp.joined_team + 50))
	{
		gi.cprintf (ent, PRINT_HIGH, "You must wait 5 seconds before changing teams again.\n");
		return;
	}

	if (Q_stricmp (t, "none") == 0)
	{
		if (ent->client->resp.team == NOTEAM)
			gi.cprintf (ent, PRINT_HIGH, "You're not on a team.\n");
		else
			LeaveTeam (ent);

		return;
	}

	if (Q_stricmp (t, "1") == 0)
		desired_team = TEAM1;
	else if (Q_stricmp (t, "2") == 0)
		desired_team = TEAM2;
	else if (Q_stricmp (t, teams[TEAM1].name) == 0)
		desired_team = TEAM1;
	else if (Q_stricmp (t, teams[TEAM2].name) == 0)
		desired_team = TEAM2;
	else if (use_3teams->value)
	{
		if (Q_stricmp (t, "3") == 0)
			desired_team = TEAM3;
		else if (Q_stricmp (t, teams[TEAM3].name) == 0)
			desired_team = TEAM3;
	}
	else if (ctf->value)
	{
		if (Q_stricmp (t, "red") == 0)
			desired_team = TEAM1;
		else if (Q_stricmp (t, "blue") == 0)
			desired_team = TEAM2;
	}

	if(desired_team == NOTEAM)
	{
		gi.cprintf (ent, PRINT_HIGH, "Unknown team %s.\n", t);
		return;
	}

	if (ent->client->resp.team == desired_team)
	{
		gi.cprintf (ent, PRINT_HIGH, "You are already on %s.\n",
		TeamName (ent->client->resp.team));
		return;
	}

	JoinTeam (ent, desired_team, 1);
}

void JoinTeam (edict_t * ent, int desired_team, int skip_menuclose)
{
	char *s, *a;

	if (!skip_menuclose)
		PMenu_Close (ent);

	if (ent->client->resp.team == desired_team)
		return;

	if (matchmode->value && mm_allowlock->value && teams[desired_team].locked)
	{
		if (skip_menuclose)
			gi.cprintf(ent, PRINT_HIGH, "Cannot join %s (locked)\n", TeamName(desired_team));
		else
			gi.centerprintf(ent, "Cannot join %s (locked)", TeamName(desired_team));

		return;
	}

	if(!matchmode->value && eventeams->value && desired_team != NOTEAM) {
		if(!IsAllowedToJoin(ent, desired_team)) {
			gi.centerprintf(ent, "Cannot join %s (has too many players)", TeamName(desired_team));
			return;
		}
	}

	a = (ent->client->resp.team == NOTEAM) ? "joined" : "changed to";

	ent->client->resp.team = desired_team;
	s = Info_ValueForKey (ent->client->pers.userinfo, "skin");
	AssignSkin (ent, s, false);

	if (ent->solid != SOLID_NOT)	// alive, in game
	{
		if (punishkills->value)
		{
			if (ent->client->attacker && ent->client->attacker->client &&
			(ent->client->attacker->client != ent->client))
			{
				char deathmsg[64];
				Com_sprintf(deathmsg, sizeof(deathmsg), "%s ph34rs %s so much %s committed suicide! :)\n",
					ent->client->pers.netname, ent->client->attacker->client->pers.netname,
					ent->client->resp.radio_gender ? "she" : "he");
				PrintDeathMessage(deathmsg, ent);
				IRC_printf (IRC_T_DEATH, deathmsg);
				if(team_round_going || !OnSameTeam(ent, ent->client->attacker)) {
					Add_Frag (ent->client->attacker);
					Subtract_Frag (ent);
					ent->client->resp.deaths++;
				}
			}
		}
		ent->health = 0;
		player_die (ent, ent, ent, 100000, vec3_origin);
		ent->deadflag = DEAD_DEAD;
	}

	if (ctf->value)
	{
		ent->flags &= ~FL_GODMODE;
		ent->client->resp.ctf_state = CTF_STATE_START;
		gi.bprintf (PRINT_HIGH, "%s %s %s.\n",
		ent->client->pers.netname, a, CTFTeamName (desired_team));
		IRC_printf (IRC_T_GAME, "%n %s %n.",
		ent->client->pers.netname, a, CTFTeamName (desired_team));
	}
	else
	{
		if(teamdm->value)
			ent->flags &= ~FL_GODMODE;
		gi.bprintf (PRINT_HIGH, "%s %s %s.\n",
		ent->client->pers.netname, a, TeamName (desired_team));
		IRC_printf (IRC_T_GAME, "%n %s %n.",
		ent->client->pers.netname, a, TeamName (desired_team));
	}

	ent->client->resp.joined_team = (int)(realLtime*10.0f);

	//AQ2:TNG - Slicer added the ctf->value coz teamplay people were spawning....
	if ((ctf->value || teamdm->value) && team_round_going && (ent->inuse && ent->client->resp.team != NOTEAM))
	{
		//    ent->client->resp.last_killed_target = NULL;
		ResetKills (ent);
		//AQ2:TNG Slicer Last Damage Location
		ent->client->resp.last_damaged_part = 0;
		ent->client->resp.last_damaged_players[0] = '\0';
		//AQ2:TNG END
		PutClientInServer (ent);
		AddToTransparentList (ent);
	}
	//AQ2:TNG - Slicer Matchmode
	if (matchmode->value)
	{
		if (ent->client->resp.captain)
		{
			if(teamdm->value || ctf->value)
			{
				if(!team_round_going)
					teams[ent->client->resp.captain].ready = 0;
				teams[ent->client->resp.captain].locked = 0;
			}
			else
			{
				teams[ent->client->resp.captain].ready = 0;
			}
		}

		ent->client->resp.subteam = 0;	//SLICER: If a player joins or changes teams, the subteam resets....
		ent->client->resp.captain = 0;	//SLICER: Same here
	}
	//AQ2:TNG END
	if (!skip_menuclose && !teamdm->value && ctf->value != 2)
		OpenWeaponMenu (ent);
}

void LeaveTeam (edict_t * ent)
{
	char *g;

	if (ent->client->resp.team == NOTEAM)
		return;

	if (ent->solid != SOLID_NOT)	// alive, in game
	{
		if (punishkills->value)
		{
			if (ent->client->attacker && ent->client->attacker->client &&
			(ent->client->attacker->client != ent->client))
			{
				char deathmsg[64];
				Com_sprintf(deathmsg, sizeof(deathmsg), "%s ph34rs %s so much %s committed suicide! :)\n",
					ent->client->pers.netname, ent->client->attacker->client->pers.netname,
					ent->client->resp.radio_gender ? "she" : "he");
				PrintDeathMessage(deathmsg, ent);
				IRC_printf (IRC_T_DEATH, deathmsg);
				if(team_round_going || !OnSameTeam(ent, ent->client->attacker)) {
					Add_Frag (ent->client->attacker);
					Subtract_Frag (ent);
					ent->client->resp.deaths++;
				}
			}
		}
		ent->health = 0;
		player_die (ent, ent, ent, 100000, vec3_origin);
		ent->deadflag = DEAD_DEAD;
	}

	if (IsNeutral (ent))
		g = "its";
	else if (IsFemale (ent))
		g = "her";
	else
		g = "his";

	gi.bprintf (PRINT_HIGH, "%s left %s team.\n", ent->client->pers.netname, g);
	IRC_printf (IRC_T_GAME, "%n left %n team.", ent->client->pers.netname, g);

	ent->client->resp.joined_team = 0;
	ent->client->resp.team = NOTEAM;
	//AQ2:TNG Slicer 
	if (matchmode->value)
	{
		if (ent->client->resp.captain)
		{
			if(teamdm->value || ctf->value)
			{
				if(!team_round_going)
					teams[ent->client->resp.captain].ready = 0;
				teams[ent->client->resp.captain].locked = 0;
			}
			else
			{
				teams[ent->client->resp.captain].ready = 0;
			}
		}

		ent->client->resp.subteam = 0;	//SLICER: If a player joins or changes teams, the subteam resets....
		ent->client->resp.captain = 0;	//SLICER: Same here
	}
	//AQ2:TNG END
}

void ReturnToMain (edict_t * ent, pmenu_t * p)
{
	PMenu_Close (ent);
	OpenJoinMenu (ent);
}

void OpenItemMenu (edict_t * ent)
{
	//AQ2:TNG - Igor adding itm_flags
	static char *menu_itemnames[] = {
		"Kevlar Vest",
		"Laser Sight",
		"Stealth Slippers",
		"Silencer",
		"Bandolier",
		"Kevlar Helmet"
		};
	int pos;

	if (itm_flags->value != 0)
	{
		pos = 4;
		if ((int)itm_flags->value & ITF_KEV)
		{
			itemmenu[pos].text = menu_itemnames[0];
			itemmenu[pos].SelectFunc = SelectItem1;
			pos++;
		}

		if ((int)itm_flags->value & ITF_LASER)
		{
			itemmenu[pos].text = menu_itemnames[1];
			itemmenu[pos].SelectFunc = SelectItem2;
			pos++;
		}

		if ((int)itm_flags->value & ITF_SLIP)
		{
			itemmenu[pos].text = menu_itemnames[2];
			itemmenu[pos].SelectFunc = SelectItem3;
			pos++;
		}

		if ((int)itm_flags->value & ITF_SIL)
		{
			itemmenu[pos].text = menu_itemnames[3];
			itemmenu[pos].SelectFunc = SelectItem4;
			pos++;
		}

		if ((int)itm_flags->value & ITF_BAND)
		{
			itemmenu[pos].text = menu_itemnames[4];
			itemmenu[pos].SelectFunc = SelectItem5;
			pos++;
		}

		if ((int)itm_flags->value & ITF_HELM)
		{
			itemmenu[pos].text = menu_itemnames[5];
			itemmenu[pos].SelectFunc = SelectItem6;
			pos++;
		}

		for (; pos < 10; pos++)
		{
			itemmenu[pos].text = NULL;
			itemmenu[pos].SelectFunc = NULL;
		}

		//AQ2:TNG End adding itm_flags
		PMenu_Open (ent, itemmenu, 4, sizeof (itemmenu) / sizeof (pmenu_t));
	}
	else
	{
		PMenu_Close (ent);
	}
}

void OpenWeaponMenu (edict_t * ent)
{
	//AQ2:TNG - Igor adding wp_flags
	static char *menu_weapnames[] = {
		"MP5/10 Submachinegun",
		"M3 Super90 Assault Shotgun",
		"Handcannon",
		"SSG 3000 Sniper Rifle",
		"M4 Assault Rifle",
		"Combat Knives",
		"Akimbo Pistols"
		};
	int pos;

	if ((int) wp_flags->value & ~(WPF_MK23 | WPF_GRENADE))
	{
		pos = 4;
		if ((int) wp_flags->value & WPF_MP5)
		{
			weapmenu[pos].text = menu_weapnames[0];
			weapmenu[pos].SelectFunc = SelectWeapon2;
			pos++;
		}

		if ((int) wp_flags->value & WPF_M3)
		{
			weapmenu[pos].text = menu_weapnames[1];
			weapmenu[pos].SelectFunc = SelectWeapon3;
			pos++;
		}

		if ((int) wp_flags->value & WPF_HC)
		{
			weapmenu[pos].text = menu_weapnames[2];
			weapmenu[pos].SelectFunc = SelectWeapon4;
			pos++;
		}

		if ((int) wp_flags->value & WPF_SNIPER)
		{
			weapmenu[pos].text = menu_weapnames[3];
			weapmenu[pos].SelectFunc = SelectWeapon5;
			pos++;
		}

		if ((int) wp_flags->value & WPF_M4)
		{
			weapmenu[pos].text = menu_weapnames[4];
			weapmenu[pos].SelectFunc = SelectWeapon6;
			pos++;
		}

		if ((int) wp_flags->value & WPF_KNIFE)
		{
			weapmenu[pos].text = menu_weapnames[5];
			weapmenu[pos].SelectFunc = SelectWeapon0;
			pos++;
		}

		if ((int) wp_flags->value & WPF_DUAL)
		{
			weapmenu[pos].text = menu_weapnames[6];
			weapmenu[pos].SelectFunc = SelectWeapon9;
			pos++;
		}

		for (; pos < 11; pos++)
		{
			weapmenu[pos].text = NULL;
			weapmenu[pos].SelectFunc = NULL;
		}

		PMenu_Open (ent, weapmenu, 4, sizeof (weapmenu) / sizeof (pmenu_t));
	}
	else
	{
		OpenItemMenu (ent);
	}
	//AQ2:TNG End adding wp_flags
}

// AQ2:TNG Deathwatch - Updated this for the new menu
int UpdateJoinMenu (edict_t * ent)
{
	static char levelname[28];
	static char team1players[28];
	static char team2players[28];
	static char team3players[28];
	int num1 = 0, num2 = 0, num3 = 0, i;

	if (ctf->value)
	{
		joinmenu[4].text = "Join Red Team";
		joinmenu[4].SelectFunc = JoinTeam1;
		joinmenu[6].text = "Join Blue Team";
		joinmenu[6].SelectFunc = JoinTeam2;
		joinmenu[8].text = NULL;
		joinmenu[8].SelectFunc = NULL;
		if (ctf_forcejoin->string && *ctf_forcejoin->string)
		{
			if (Q_stricmp (ctf_forcejoin->string, "red") == 0)
			{
				joinmenu[6].text = NULL;
				joinmenu[6].SelectFunc = NULL;
			}
			else if (Q_stricmp (ctf_forcejoin->string, "blue") == 0)
			{
				joinmenu[4].text = NULL;
				joinmenu[4].SelectFunc = NULL;
			}
		}
	}
	else
	{
		joinmenu[4].text = teams[TEAM1].name;
		joinmenu[4].SelectFunc = JoinTeam1;
		joinmenu[6].text = teams[TEAM2].name;
		joinmenu[6].SelectFunc = JoinTeam2;
		if (use_3teams->value)
		{
			joinmenu[8].text = teams[TEAM3].name;
			joinmenu[8].SelectFunc = JoinTeam3;
		}
		else
		{
			joinmenu[8].text = NULL;
			joinmenu[8].SelectFunc = NULL;
		}
	}
	joinmenu[11].text = "Auto-join team";
	joinmenu[11].SelectFunc = JoinTeamAuto;

	levelname[0] = '*';
	if (g_edicts[0].message)
		Q_strncpyz(levelname + 1, g_edicts[0].message, sizeof(levelname) - 1);
	else
		Q_strncpyz(levelname + 1, level.mapname, sizeof(levelname) - 1);

	for (i = 0; i < (int)maxclients->value; i++)
	{
		if (!g_edicts[i + 1].inuse)
			continue;
		if (game.clients[i].resp.team == TEAM1)
			num1++;
		else if (game.clients[i].resp.team == TEAM2)
			num2++;
		else if (game.clients[i].resp.team == TEAM3)
			num3++;
	}

	sprintf (team1players, "  (%d players)", num1);
	sprintf (team2players, "  (%d players)", num2);
	sprintf (team3players, "  (%d players)", num3);

	joinmenu[2].text = levelname;
	if (joinmenu[4].text)
		joinmenu[5].text = team1players;
	else
		joinmenu[5].text = NULL;
	if (joinmenu[6].text)
		joinmenu[7].text = team2players;
	else
		joinmenu[7].text = NULL;
	if (joinmenu[8].text && use_3teams->value)
		joinmenu[9].text = team3players;
	else
		joinmenu[9].text = NULL;
}

// AQ2:TNG END

void OpenJoinMenu (edict_t * ent)
{
	//PG BUND - BEGIN (Tourney extension)
	if (use_tourney->value)
	{
		OpenWeaponMenu (ent);
		return;
	}
	//PG BUND - END (Tourney extension)

	UpdateJoinMenu (ent);

	PMenu_Open (ent, joinmenu, 11 /* magic for Auto-join menu item */, sizeof (joinmenu) / sizeof (pmenu_t));
}

int member_array (char *str, char *arr[], int num_elems)
{
	int l;

	for (l = 0; l < num_elems; l++)
	{
		if (!strcmp (str, arr[l]))
			return l;
	}
	return -1;
}

void CleanLevel ()
{
	char *remove_classnames[] = {
		"weapon_Mk23",
		"weapon_MP5",
		"weapon_M4",
		"weapon_M3",
		"weapon_HC",
		"weapon_Sniper",
		"weapon_Dual",
		"weapon_Knife",
		"weapon_Grenade",
		"ammo_sniper",
		"ammo_clip",
		"ammo_mag",
		"ammo_m4",
		"ammo_m3",
		"item_quiet",
		"item_slippers",
		"item_band",
		"item_lasersight",
		"item_vest",
		"thrown_knife",
		"hgrenade",
		"item_helmet"
	};
	int i;
	int base;
	edict_t *ent;

	base = 1 + maxclients->value + BODY_QUEUE_SIZE;
	ent = g_edicts + base;
	for (i = 1 + maxclients->value + BODY_QUEUE_SIZE; i < globals.num_edicts; i++, ent++)
	{
		if (!ent->classname)
			continue;
		if (member_array (ent->classname, remove_classnames,
			sizeof (remove_classnames) / sizeof (char *)) > -1)
		{
			G_FreeEdict (ent);
		}
	}

	CleanBodies ();

	// fix glass
	CGF_SFX_RebuildAllBrokenGlass ();
}

void MakeAllLivePlayersObservers(void);

void ResetScores (qboolean playerScores)
{
	int i;
	edict_t *ent;

	team_round_going = team_round_countdown = team_game_going = 0;
	current_round_length = matchtime = 0;
	pause_time = 0;
	num_ghost_players = 0;

	MakeAllLivePlayersObservers ();

	for(i = TEAM1; i < TEAM_TOP; i++)
	{
		teams[i].score = teams[i].total = 0;
		teams[i].ready = teams[i].locked = 0;
		teams[i].pauses_used = teams[i].wantReset = 0;
		gi.cvar_forceset(teams[i].teamscore->name, "0");
	}

	if(!playerScores)
		return;

	for (i = 0; i < game.maxclients; i++)
	{
		ent = g_edicts + 1 + i;
		if (!ent->inuse)
			continue;

		ent->client->resp.score = 0;
		ent->client->resp.kills = 0;
		ent->client->resp.damage_dealt = 0;
		ent->client->resp.hs_streak = 0;
		ent->client->resp.streak = 0;
		ent->client->resp.ctf_capstreak = 0;
		ent->client->resp.last_damaged_part = 0;
		ent->client->resp.last_damaged_players[0] = '\0';
		ent->client->resp.deaths = 0;
		ent->client->resp.killed_teammates = 0;
		ent->enemy = NULL;
		ResetKills(ent);
		ResetStats(ent);
	}
}

qboolean StartClient (edict_t * ent)
{
	if (ent->client->resp.team != NOTEAM)
		return false;

	// start as 'observer'
	ent->movetype = MOVETYPE_NOCLIP;
	ent->solid = SOLID_NOT;
	ent->svflags |= SVF_NOCLIENT;
	ent->client->resp.team = NOTEAM;
	ent->client->ps.gunindex = 0;
	gi.linkentity (ent);

	// Disabled so people can read the motd
	// OpenJoinMenu(ent);
	return true;
}

void CenterPrintAll (const char *msg)
{
	int i;
	edict_t *ent;

	gi.cprintf (NULL, PRINT_HIGH, "%s\n", msg);	// so it goes to the server console...

	for (i = 0; i < game.maxclients; i++)
	{
		ent = &g_edicts[1 + i];
		if (ent->inuse)
			gi.centerprintf (ent, "%s", msg);
	}
}

int TeamHasPlayers (int team)
{
	int i, players;
	edict_t *ent;

	players = 0;

	for (i = 0; i < game.maxclients; i++)
	{
		ent = &g_edicts[1 + i];
		if (!ent->inuse)
			continue;

		if (game.clients[i].resp.team == team)
			players++;
	}

	return players;
}

qboolean BothTeamsHavePlayers ()
{
	int players[TEAM_TOP] = {0}, i;
	edict_t *ent;

	//AQ2:TNG Slicer Matchmode
	if (matchmode->value && !TeamsReady())
		return false;

	//AQ2:TNG END
	if (use_tourney->value)
		return (LastOpponent > 1);

	for (i = 0; i < game.maxclients; i++)
	{
		ent = &g_edicts[1 + i];
		if (!ent->inuse || game.clients[i].resp.team == NOTEAM)
			continue;
		if (!game.clients[i].resp.subteam)
			players[game.clients[i].resp.team]++;
	}

	if (use_3teams->value)
		return ((players[1] && players[2]) || (players[1] && players[3]) || (players[2] && players[3]));
	
	return (players[1] && players[2]);
}

// CheckForWinner: Checks for a winner (or not).
int CheckForWinner ()
{
	int players[TEAM_TOP] = {0}, i;
	edict_t *ent;

	if(ctf->value || teamdm->value)
		return WINNER_NONE;

	for (i = 0; i < game.maxclients; i++)
	{
		ent = &g_edicts[1 + i];
		if (!ent->inuse || game.clients[i].resp.team == NOTEAM)
			continue;
		if (ent->solid != SOLID_NOT)
			players[game.clients[i].resp.team]++;
	}

	if (players[1] && players[2])
		return WINNER_NONE;

	if (use_3teams->value && ((players[1] && players[3]) || (players[2] && players[3])))
		return WINNER_NONE;
	
	for(i=TEAM1; i<=teamCount; i++)
		if(players[i])
			return i;

	return WINNER_TIE;

}

// CheckForForcedWinner: A winner is being forced, find who it is.
int CheckForForcedWinner()
{
	int onteam1 = 0, onteam2 = 0, onteam3 = 0, i;
	int health1 = 0, health2 = 0, health3 = 0;
	edict_t *ent;

	for (i = 0; i < game.maxclients; i++)
	{
		ent = &g_edicts[1 + i];
		if (!ent->inuse)
			continue;
		if (game.clients[i].resp.team == TEAM1 && ent->solid != SOLID_NOT)
		{
			onteam1++;
			health1 += ent->health;
		}
		else if (game.clients[i].resp.team == TEAM2 && ent->solid != SOLID_NOT)
		{
			onteam2++;
			health2 += ent->health;
		}
		else if (game.clients[i].resp.team == TEAM3 && ent->solid != SOLID_NOT)
		{
			onteam3++;
			health3 += ent->health;
		}
	}

	if (use_3teams->value)
	{
		if (onteam1 > onteam2)
		{
			if (onteam1 > onteam3)
				return WINNER_TEAM1;
			else if (onteam3 > onteam1)
				return WINNER_TEAM3;
			else if (health1 > health3)
				return WINNER_TEAM1;
			else if (health3 > health1)
				return WINNER_TEAM3;
			else
				return WINNER_TIE;
		}
		else if (onteam2 > onteam1)
		{
			if (onteam2 > onteam3)
				return WINNER_TEAM2;
			else if (onteam3 > onteam2)
				return WINNER_TEAM3;
			else if (health2 > health3)
				return WINNER_TEAM2;
			else if (health3 > health2)
				return WINNER_TEAM3;
			else
				return WINNER_TIE;
		}
		else if (onteam1 == onteam2 && onteam1 > onteam3)
		{
			if (health1 > health2)
				return WINNER_TEAM1;
			else if (health2 > health1)
				return WINNER_TEAM2;
			else
				return WINNER_TIE;
		}
		else if (onteam3 > onteam1)
			return WINNER_TEAM3;

		if (health1 > health2)
		{
			if (health1 > health3)
				return WINNER_TEAM1;
			else if (health3 > health1)
				return WINNER_TEAM3;
		}
		else if (health2 > health1)
		{
			if (health2 > health3)
				return WINNER_TEAM2;
			else if (health3 > health2)
				return WINNER_TEAM3;
		}
		else if (health3 > health1)
			return WINNER_TEAM3;
	}
	else
	{
		if (onteam1 > onteam2)
			return WINNER_TEAM1;
		else if (onteam2 > onteam1)
			return WINNER_TEAM2;

		if (health1 > health2)
			return WINNER_TEAM1;
		else if (health2 > health1)
			return WINNER_TEAM2;
	}
	return WINNER_TIE;
}

void SpawnPlayers ()
{
	int i;
	edict_t *ent;

	if(!ctf->value && !teamdm->value)
	{
		if (!use_oldspawns->value)
			NS_SetupTeamSpawnPoints ();
		else
			SetupTeamSpawnPoints ();
	}

	InitTransparentList ();
	for (i = 0; i < game.maxclients; i++)
	{
		ent = &g_edicts[1 + i];
		if (ent->inuse && ent->client->resp.team != NOTEAM && ent->client->resp.subteam == 0)
		{
			// ent->client->resp.last_killed_target = NULL;
			ResetKills (ent);
			//AQ2:TNG Slicer Last Damage Location
			ent->client->resp.last_damaged_part = 0;
			ent->client->resp.last_damaged_players[0] = '\0';
			//AQ2:TNG END
			PutClientInServer (ent);
			AddToTransparentList (ent);
		}
	}

	if(matchmode->value	&& limchasecam->value)
	{
		for (i = 0; i < game.maxclients; i++)
		{
			ent = &g_edicts[1 + i];
			if (ent->inuse && ent->client->resp.team != NOTEAM && ent->client->resp.subteam)
			{
				ent->client->chase_target = NULL;
				GetChaseTarget (ent);
				if (ent->client->chase_target != NULL)
				{
					if (limchasecam->value == 2)
					{
						ent->client->chase_mode = 1;
						UpdateChaseCam (ent);
						ent->client->chase_mode = 2;
					}
					else
					{
						ent->client->chase_mode = 1;
					}
					UpdateChaseCam (ent);
				}
			}
		}
	}
}

void StartRound ()
{
	team_round_going = 1;
	current_round_length = 0;
}

void StartLCA ()
{
	if(!teamdm->value && ctf->value != 2)
		CleanLevel ();

	if (use_tourney->value)
	{
		lights_camera_action = TourneySetTime (T_SPAWN);
		TourneyTimeEvent (T_SPAWN, lights_camera_action);
	}
	else
	{
		CenterPrintAll ("LIGHTS...");
		gi.sound (&g_edicts[0], CHAN_VOICE | CHAN_NO_PHS_ADD,
			gi.soundindex ("atl/lights.wav"), 1.0, ATTN_NONE, 0.0);
		lights_camera_action = 43;	// TempFile changed from 41
	}
	SpawnPlayers ();
}

// FindOverlap: Find the first (or next) overlapping player for ent.
edict_t *FindOverlap (edict_t * ent, edict_t * last_overlap)
{
  int i;
  edict_t *other;
  vec3_t diff;

  for (i = last_overlap ? last_overlap - g_edicts : 0; i < game.maxclients;
       i++)
    {
      other = &g_edicts[i + 1];

      if (!other->inuse || other->client->resp.team == NOTEAM
	  || other == ent
	  || other->solid == SOLID_NOT || other->deadflag == DEAD_DEAD)
	continue;

      VectorSubtract (ent->s.origin, other->s.origin, diff);

      if (diff[0] >= -33 && diff[0] <= 33 &&
	  diff[1] >= -33 && diff[1] <= 33 && diff[2] >= -65 && diff[2] <= 65)
	return other;
    }

  return NULL;
}

void ContinueLCA ()
{
	if (use_tourney->value)
	{
		TourneyTimeEvent (T_SPAWN, lights_camera_action);
		if (lights_camera_action == 1)
		{
			StartRound ();
		}
	}
	else
	{
		if (lights_camera_action == 23)
		{
			CenterPrintAll ("CAMERA...");
			gi.sound (&g_edicts[0], CHAN_VOICE | CHAN_NO_PHS_ADD,
			gi.soundindex ("atl/camera.wav"), 1.0, ATTN_NONE, 0.0);
		}
		else if (lights_camera_action == 3)
		{
			CenterPrintAll ("ACTION!");
			gi.sound (&g_edicts[0], CHAN_VOICE | CHAN_NO_PHS_ADD,
			gi.soundindex ("atl/action.wav"), 1.0, ATTN_NONE, 0.0);
		}
		else if (lights_camera_action == 1)
		{
			StartRound ();
		}
	}
	lights_camera_action--;
}

void MakeAllLivePlayersObservers ()
{
	edict_t *ent;
	int saveteam, i;

	/* if someone is carrying a flag it will disappear */
	if(ctf->value)
		CTFResetFlags();

	for (i = 0; i < game.maxclients; i++)
	{
		ent = &g_edicts[1 + i];
		if (!ent->inuse)
			continue;
		if(ent->solid == SOLID_NOT && ((!teamdm->value && !ctf->value) ||
			ent->client->resp.team == NOTEAM ||	ent->client->resp.subteam))
			continue;

		saveteam = ent->client->resp.team;
		ent->client->resp.team = NOTEAM;
		PutClientInServer (ent);
		ent->client->resp.team = saveteam;
	}
}

// PrintScores: Prints the current score on the console
void PrintScores (void)
{
	if (use_3teams->value) {
		gi.bprintf (PRINT_HIGH, "Current score is %s: %d to %s: %d to %s: %d\n", TeamName (TEAM1), teams[TEAM1].score, TeamName (TEAM2), teams[TEAM2].score, TeamName (TEAM3), teams[TEAM3].score);
		IRC_printf (IRC_T_TOPIC, "Current score on map %n is %n: %k to %n: %k to %n: %k", level.mapname, TeamName (TEAM1), teams[TEAM1].score, TeamName (TEAM2), teams[TEAM2].score, TeamName (TEAM3), teams[TEAM3].score);
	} else {
		gi.bprintf (PRINT_HIGH, "Current score is %s: %d to %s: %d\n", TeamName (TEAM1), teams[TEAM1].score, TeamName (TEAM2), teams[TEAM2].score);
		IRC_printf (IRC_T_TOPIC, "Current score on map %n is %n: %k to %n: %k", level.mapname, TeamName (TEAM1), teams[TEAM1].score, TeamName (TEAM2), teams[TEAM2].score);
	}
}

// WonGame: returns true if we're exiting the level.
int WonGame (int winner)
{
	edict_t *player, *cl_ent; // was: edict_t *player;
	int i;
	char arg[64];

	gi.bprintf (PRINT_HIGH, "The round is over:\n");
	IRC_printf (IRC_T_GAME, "The round is over:");
	if (winner == WINNER_TIE)
	{
		gi.bprintf (PRINT_HIGH, "It was a tie, no points awarded!\n");
		IRC_printf (IRC_T_GAME, "It was a tie, no points awarded!");

		if(use_warnings->value)
			gi.sound (&g_edicts[0], CHAN_VOICE | CHAN_NO_PHS_ADD,	gi.soundindex ("tng/no_team_wins.wav"), 1.0, ATTN_NONE, 0.0);
		PrintScores ();
	}
	else
	{

		if (use_tourney->value)
		{
			if(winner == TEAM1)
				player = TourneyFindPlayer(1);
			else
				player = TourneyFindPlayer(NextOpponent);

			if (player)
			{
				gi.bprintf (PRINT_HIGH, "%s was victorious!\n",
				player->client->pers.netname);
				IRC_printf (IRC_T_GAME, "%n was victorious!",
				player->client->pers.netname);
				TourneyWinner (player);
			}
		}
		else
		{
			gi.bprintf (PRINT_HIGH, "%s won!\n", TeamName(winner));
			IRC_printf (IRC_T_GAME, "%n won!", TeamName(winner));
			// AQ:TNG Igor[Rock] changing sound dir
			if(use_warnings->value)
				gi.sound (&g_edicts[0], CHAN_VOICE | CHAN_NO_PHS_ADD,	gi.soundindex (va("tng/team%i_wins.wav", winner)), 1.0, ATTN_NONE,	0.0);
			// end of changing sound dir
			teams[winner].score++;
			gi.cvar_forceset(teams[winner].teamscore->name, va("%i", teams[winner].score));

			PrintScores ();
		}
	}

	if (timelimit->value)
	{
		// AQ2:M - Matchmode
		if (matchmode->value)
		{
			if (matchtime >= timelimit->value * 60)
			{
				SendScores ();
				teams[TEAM1].ready = teams[TEAM2].ready = teams[TEAM3].ready = 0;
				team_round_going = team_round_countdown = team_game_going = matchtime = 0;
				MakeAllLivePlayersObservers ();
				return 1;
			}
		}
		else if (level.time >= timelimit->value * 60)
		{
			gi.bprintf (PRINT_HIGH, "Timelimit hit.\n");
			IRC_printf (IRC_T_GAME, "Timelimit hit.");
			EndDMLevel ();
			team_round_going = team_round_countdown = team_game_going = 0;
			return 1;
		}
	}
	
	if (roundlimit->value && !ctf->value)
	{
		if (teams[TEAM1].score >= roundlimit->value
			|| teams[TEAM2].score >= roundlimit->value
			|| teams[TEAM3].score >= roundlimit->value)
		{
			if (matchmode->value)
			{
				SendScores ();
				teams[TEAM1].ready = teams[TEAM2].ready = teams[TEAM3].ready = matchtime = 0;
				team_round_going = team_round_countdown = team_game_going = 0;
				MakeAllLivePlayersObservers ();
				return 1;
			}
			else
			{
				gi.bprintf (PRINT_HIGH, "Roundlimit hit.\n");
				IRC_printf (IRC_T_GAME, "Roundlimit hit.");
				EndDMLevel ();
				team_round_going = team_round_countdown = team_game_going = 0;
				return 1;
			}
		}
	}
	
	//PG BUND - BEGIN
	if (vCheckVote () == true)
	{
		EndDMLevel ();
		team_round_going = team_round_countdown = team_game_going = 0;
		return 1;
	}
	vNewRound ();
	//PG BUND - END

	if(teamplay->value  && (!timelimit->value || level.time <= ((timelimit->value * 60) - 5)))
	{
		arg[0] = '\0';
		for (i = 0; i < game.maxclients; i++)
		{
			cl_ent = &g_edicts[1 + i];

			if (cl_ent->inuse && cl_ent->client->resp.stat_mode == 2)
					Cmd_Stats_f(cl_ent, arg);
		}
	}

  return 0;
}


void CheckTeamRules (void)
{
	int winner, i;
	int checked_tie = 0;
	char buf[1024];

	if (round_delay_time && use_tourney->value)
	{
		TourneyTimeEvent (T_END, round_delay_time);
		round_delay_time--;
		if (!round_delay_time)
		{
			TourneyNewRound ();
			team_round_countdown = TourneySetTime (T_RSTART);
			TourneyTimeEvent (T_START, team_round_countdown);
		}
		return;
	}

// AQ2:TNG - JBravo adding UVtime
	if(ctf->value)
	{
		for (i = 0; i < maxclients->value; i++)
		{
			if (!g_edicts[i + 1].inuse)
				continue;
			if (game.clients[i].ctf_uvtime > 0)
			{
				game.clients[i].ctf_uvtime--;
				if (!game.clients[i].ctf_uvtime && team_round_going)
				{
					if(ctfgame.type == 2) {
						gi.centerprintf (&g_edicts[i + 1],
							"ACTION!\n"
							"\n"
							"You are %s the %s base!",
							(game.clients[i].resp.team == ctfgame.offence ?
								"ATTACKING" : "DEFENDING"),
							CTFOtherTeamName(ctfgame.offence));
					} else {
						gi.centerprintf (&g_edicts[i + 1], "ACTION!");
					}
				}
				else if (game.clients[i].ctf_uvtime % 10 == 0)
				{
					if(ctfgame.type == 2) {
						gi.centerprintf (&g_edicts[i + 1],
							"Shield %d\n"
							"\n"
							"You are %s the %s base!",
							game.clients[i].ctf_uvtime / 10,
							(game.clients[i].resp.team == ctfgame.offence ?
								"ATTACKING" : "DEFENDING"),
							CTFOtherTeamName(ctfgame.offence));
					} else {
						gi.centerprintf (&g_edicts[i + 1], "Shield %d",
							game.clients[i].ctf_uvtime / 10);
					}
				}
			}
		}
	}

	if (matchmode->value)
	{
		if(team_game_going)
			matchtime += 0.1f;

		if(mm_allowlock->value)
		{
			for(i=TEAM1; i <= teamCount; i++)
			{
				if (teams[i].locked && !TeamHasPlayers(i))
				{
					teams[i].locked = 0;
					sprintf(buf, "%s unlocked (no players)", TeamName(i));
					CenterPrintAll(buf);
				}
			}
		}
	}

	if (lights_camera_action)
	{
		ContinueLCA ();
		return;
	}

	if (team_round_going)
		current_round_length++;

	if (holding_on_tie_check)
	{
		holding_on_tie_check--;
		if (holding_on_tie_check > 0)
			return;
		holding_on_tie_check = 0;
		checked_tie = 1;
	}

	if (team_round_countdown)
	{
		team_round_countdown--;
		if(!team_round_countdown)
		{
			if (BothTeamsHavePlayers ())
			{
				team_game_going = 1;
				StartLCA ();
			}
			else
			{
				if (!matchmode->value || TeamsReady())
					CenterPrintAll ("Not enough players to play!");
				else
					CenterPrintAll ("Both Teams Must Be Ready!");

				team_round_going = team_round_countdown = team_game_going = 0;
				MakeAllLivePlayersObservers ();
			}
		}
		else if(use_tourney->value)
		{
			TourneyTimeEvent (T_START, team_round_countdown);
		}
		else
		{
			if (team_round_countdown == 101)
			{
				gi.sound (&g_edicts[0], CHAN_VOICE | CHAN_NO_PHS_ADD,
				gi.soundindex ("world/10_0.wav"), 1.0, ATTN_NONE, 0.0);
			}
		}
		if(team_round_countdown == 41 && !matchmode->value)
		{
			while(CheckForUnevenTeams(NULL));
		}
	}

	// check these rules every 1.5 seconds...
	if (++rulecheckfrequency % 15 && !checked_tie)
		return;

	if (!team_round_going)
	{
		if (timelimit->value)
		{
			// AQ2:TNG - Slicer : Matchmode
			if (matchmode->value)
			{
				if (matchtime >= timelimit->value * 60)
				{
					SendScores ();
					teams[TEAM1].ready = teams[TEAM2].ready = teams[TEAM3].ready = 0;
					team_round_going = team_round_countdown = team_game_going = matchtime = 0;
					MakeAllLivePlayersObservers ();
					return;
				}
			}
			//AQ2:TNG END
			else if (level.time >= timelimit->value * 60)
			{
				gi.bprintf (PRINT_HIGH, "Timelimit hit.\n");
				IRC_printf (IRC_T_GAME, "Timelimit hit.");
				if (ctf->value)
					ResetPlayers ();
				EndDMLevel ();
				team_round_going = team_round_countdown = team_game_going = 0;
				return;
			}
		}

//PG BUND - BEGIN
		if (vCheckVote () == true)
		{
			EndDMLevel ();
			team_round_going = team_round_countdown = team_game_going = 0;
			return;
		}
//PG BUND - END

		if (!team_round_countdown)
		{
			if (BothTeamsHavePlayers ())
			{
				if (use_tourney->value)
				{
					TourneyNewRound ();
					team_round_countdown = TourneySetTime (T_START);
					TourneyTimeEvent (T_START, team_round_countdown);
				}
				else
				{
					CenterPrintAll ("The round will begin in 20 seconds!");
					team_round_countdown = 201;
				}
			}
		}
    }
	else
	/* team_round_going */
	{
		if (ctf->value || teamdm->value)
		{
			if (vCheckVote () == true)
			{
				EndDMLevel ();
				team_round_going = team_round_countdown = team_game_going = 0;
				return;
			}
		}

		if ((winner = CheckForWinner ()) != WINNER_NONE)
		{
			if (!checked_tie)
			{
				holding_on_tie_check = 50;
				return;
			}
			if (WonGame(winner))
				return;

			team_round_going = 0;
			lights_camera_action = 0;
			holding_on_tie_check = 0;

			if (use_tourney->value)
				round_delay_time = TourneySetTime (T_END);
			else
				team_round_countdown = 71;
			return;
		}

		if (roundtimelimit->value && !ctf->value && !teamdm->value)
		{
			if (current_round_length > roundtimelimit->value * 600)
			{
				gi.bprintf (PRINT_HIGH, "Round timelimit hit.\n");
				IRC_printf (IRC_T_GAME, "Round timelimit hit.");
				winner = CheckForForcedWinner ();
				if (WonGame (winner))
					return;
				team_round_going = 0;
				timewarning = fragwarning = 0;
				lights_camera_action = 0;
				holding_on_tie_check = 0;
				team_round_countdown = 71;
				return;
			}
			// AQ:TNG Igor[Rock] changing sound dir

			if(use_warnings->value)
			{
				if (current_round_length > (roundtimelimit->value - 1) * 600)
				{
					if (timewarning < 2)
					{
						CenterPrintAll ("1 MINUTE LEFT...");
						gi.sound (&g_edicts[0], CHAN_VOICE | CHAN_NO_PHS_ADD,
						gi.soundindex ("tng/1_minute.wav"), 1.0,
						ATTN_NONE, 0.0);
						timewarning = 2;
					}
				}
				else if (current_round_length > (roundtimelimit->value - 3) * 600
					&& roundtimelimit->value > 3)
				{
					if (timewarning < 1)
					{
						CenterPrintAll ("3 MINUTES LEFT...");
						gi.sound (&g_edicts[0], CHAN_VOICE | CHAN_NO_PHS_ADD,
						gi.soundindex ("tng/3_minutes.wav"), 1.0,
						ATTN_NONE, 0.0);
						timewarning = 1;
					}
				}
			}
	  // end of changing sound dir
		}

		if (ctf->value || teamdm->value)
		{
			if(timelimit->value)
			{
				float gametime;

				if(matchmode->value)
					gametime = matchtime;
				else
					gametime = level.time;

				if (gametime >= timelimit->value * 60)
				{
					gi.bprintf (PRINT_HIGH, "Timelimit hit.\n");
					IRC_printf (IRC_T_GAME, "Timelimit hit.");
					teams[TEAM1].ready = teams[TEAM2].ready = teams[TEAM3].ready = 0;
					team_round_going = team_round_countdown =
					team_game_going = 0;
					if(matchmode->value) {
						SendScores ();
						MakeAllLivePlayersObservers ();
					}
					else {
						ResetPlayers ();
						EndDMLevel ();
					}
					matchtime = 0;
					return;
				}
			}
				
			if (!BothTeamsHavePlayers ())
			{
				if (!matchmode->value || TeamsReady())
					CenterPrintAll ("Not enough players to play!");
				else
					CenterPrintAll ("Both Teams Must Be Ready!");
	
				team_round_going = team_round_countdown = team_game_going = 0;
				MakeAllLivePlayersObservers ();

				/* try to restart the game */
				while(CheckForUnevenTeams(NULL));
			}

		}
	}
}


void A_Scoreboard (edict_t * ent)
{
  int wteam = 0;

	if (ent->client->showscores && ent->client->scoreboardnum == 1)
	{
		// blink header of the winning team during intermission
		if (level.intermissiontime && (level.framenum & 8))
		{			// blink 1/8th second
			if (teams[TEAM1].score > teams[TEAM2].score)
				wteam = TEAM1;
			else if (teams[TEAM2].score > teams[TEAM1].score)
				wteam = TEAM2;
			else if (teams[TEAM1].total > teams[TEAM2].total)	// frag tie breaker
				wteam = TEAM1;
			else if (teams[TEAM2].total > teams[TEAM1].total)
				wteam = TEAM2;

			if(use_3teams->value)
			{
				if(wteam) {
					if (teams[TEAM3].score > teams[wteam].score)
						wteam = TEAM3;
					else if (teams[TEAM3].score == teams[wteam].score) {
						if(teams[TEAM3].total > teams[wteam].total)
							wteam = TEAM3;
						else if (teams[TEAM3].total == teams[wteam].total)
							wteam = 0;
					}
				} else {
					if(teams[TEAM3].score > teams[TEAM1].score)
						wteam = TEAM3;
					else if (teams[TEAM3].total > teams[TEAM1].total)
						wteam = TEAM3;
				}
			}

			if (wteam == 1)
				ent->client->ps.stats[STAT_TEAM1_PIC] = 0;
			else if (wteam == 2)
				ent->client->ps.stats[STAT_TEAM2_PIC] = 0;
			else if (wteam == 3 && use_3teams->value)
				ent->client->ps.stats[STAT_TEAM3_PIC] = 0;
			else // tie game!
			{
				ent->client->ps.stats[STAT_TEAM1_PIC] = 0;
				ent->client->ps.stats[STAT_TEAM2_PIC] = 0;
				if(use_3teams->value)
					ent->client->ps.stats[STAT_TEAM3_PIC] = 0;
			}
		}
		else
		{
			ent->client->ps.stats[STAT_TEAM1_PIC] = gi.imageindex(teams[TEAM1].skin_index);
			ent->client->ps.stats[STAT_TEAM2_PIC] =	gi.imageindex(teams[TEAM2].skin_index);
			if (use_3teams->value)
				ent->client->ps.stats[STAT_TEAM3_PIC] = gi.imageindex (teams[TEAM3].skin_index);
		}

		ent->client->ps.stats[STAT_TEAM1_SCORE] = teams[TEAM1].score;
		ent->client->ps.stats[STAT_TEAM2_SCORE] = teams[TEAM2].score;
		if (use_3teams->value)
			ent->client->ps.stats[STAT_TEAM3_SCORE] = teams[TEAM3].score;
	}
}

// Maximum number of lines of scores to put under each team's header.
#define MAX_SCORES_PER_TEAM 9

//AQ2:TNG - Slicer Modified Scores for Match Mode
void A_ScoreboardMessage (edict_t * ent, edict_t * killer)
{
	char string[1400], damage[50];
	gclient_t *cl;
	edict_t *cl_ent;
	int maxsize = 1000, i, j, k;


	if (ent->client->scoreboardnum == 1)
	{
		int team, len, deadview;
		int sorted[TEAM_TOP][MAX_CLIENTS];
		int score;
		int total[TEAM_TOP] = {0,0,0,0};
		int totalsubs[TEAM_TOP] = {0,0,0,0}, subs[TEAM_TOP] = {0,0,0,0};
		int totalscore[TEAM_TOP] = {0,0,0,0};
		int totalalive[TEAM_TOP] = {0,0,0,0};
		int totalaliveprinted[TEAM_TOP] = {0,0,0,0};
		int stoppedat[TEAM_TOP] = {-1,-1,-1,-1};
		int name_pos[TEAM_TOP];
		int secs, mins;
		int offset[TEAM_TOP], tpic[TEAM_TOP][2] = {{0,0},{24,26},{25,27},{30,31}};
		char temp[16];
		int otherLines, scoreWidth = 3;

		if(use_3teams->value) {
			offset[TEAM1] = -80;
			offset[TEAM2] = 80;
			offset[TEAM3] = 240;
		} else {
			offset[TEAM1] = 0;
			offset[TEAM2] = 160;
			if(ctf->value)
			{
				tpic[TEAM1][0] = 30;
				tpic[TEAM2][0] = 31;
			}
			else if(teamdm->value)
			{
				scoreWidth = 3;
			}
		}

		deadview = (ent->solid == SOLID_NOT || ent->deadflag == DEAD_DEAD || !team_round_going);
		// AQ:TNG - Hack to keep scoreboard from revealing whos alive during matches - JBravo
		if (limchasecam->value != 0)
			deadview = 0;

		ent->client->ps.stats[STAT_TEAM_HEADER] = gi.imageindex ("tag3");

		for (i = 0; i < game.maxclients; i++)
		{
			cl_ent = g_edicts + 1 + i;
			if (!cl_ent->inuse)
				continue;

			if (game.clients[i].resp.team == NOTEAM)
				continue;

			team = game.clients[i].resp.team;

			if (!matchmode->value || !game.clients[i].resp.subteam)
			{
				score = game.clients[i].resp.score;
				if (noscore->value)
				{
					j = total[team];
				}
				else
				{
					for (j = 0; j < total[team]; j++)
					{
						if (score > game.clients[sorted[team][j]].resp.score)
							break;
						if (score == game.clients[sorted[team][j]].resp.score &&
							game.clients[i].resp.damage_dealt > game.clients[sorted[team][j]].resp.damage_dealt)
							break;
					}

					for (k = total[team]; k > j; k--)
						sorted[team][k] = sorted[team][k - 1];
				}
				sorted[team][j] = i;
				totalscore[team] += score;
				total[team]++;
			}
			else
			{
				totalsubs[team]++;
			}

			if (cl_ent->solid != SOLID_NOT && cl_ent->deadflag != DEAD_DEAD)
				totalalive[team]++;
		}

		// I've shifted the scoreboard position 8 pixels to the left in Axshun so it works
		// correctly in 320x240 (Action's does not)--any problems with this?  -FB
		// Also going to center the team names.

		for(i=TEAM1; i<= teamCount; i++)
		{
			name_pos[i] = ((20 - strlen (teams[i].name)) / 2) * 8;
			if (name_pos[i] < 0)
				name_pos[i] = 0;
		}

		len = 0;
		for(i=TEAM1; i<= teamCount; i++)
		{
			if(matchmode->value)
				Com_sprintf(temp, sizeof(temp), "%4i/%2i/%-2d", totalscore[i], total[i], totalsubs[i]);
			else
				Com_sprintf(temp, sizeof(temp), "%4i/%2i", totalscore[i], total[i]);

			if(ctf->value)
			{
				sprintf(string + len,
					"if %i xv %i yv 8 pic %i endif "
					"xv %i yv 28 string \"%s\" "
					"xv %i yv 12 num 2 %i ",
					tpic[i][0], offset[i]+8, tpic[i][0],
					offset[i]+40, temp,
					offset[i]+98, tpic[i][1]);

				offset[i]+=8;
			}
			else
			{
				sprintf (string + len,
					"if %i xv %i yv 8 pic %i endif "
					"if 22 xv %i yv 8 pic 22 endif "
					"xv %i yv 28 string \"%s\" "
					"xv %i yv 12 num %i %i "
					"xv %d yv 0 string \"%s\" ",
				tpic[i][0], offset[i], tpic[i][0], offset[i]+32, offset[i]+32,
				temp, offset[i]+96, scoreWidth, tpic[i][1],
				name_pos[i] + offset[i], teams[i].name);
			}

			len = strlen (string);
		}


		if(matchmode->value)
			otherLines = 3; //for subs
		else
			otherLines = 0;

		for (i = 0; i < (MAX_SCORES_PER_TEAM + 1 - otherLines); i++)
		{
			if (i >= total[TEAM1] && i >= total[TEAM2]) {
				if(teamCount == 2 || i >= total[TEAM3])
					break;
			}

			// ok, if we're approaching the "maxsize", then let's stop printing members of each
			// teams (if there's more than one member left to print in that team...)
			if (len > (maxsize - 100))
			{
				for(j=TEAM1; j<= teamCount; j++)
				{
					if (i < (total[j] - 1))
						stoppedat[j] = i;
				}
			}

			if (i == MAX_SCORES_PER_TEAM - 1 - otherLines)
			{
				for(j=TEAM1; j<= teamCount; j++)
				{
					if (total[j] > MAX_SCORES_PER_TEAM - otherLines)
						stoppedat[j] = i;
				}
			}

			for(j=TEAM1; j<= teamCount; j++)
			{
				if (i < total[j] && stoppedat[j] == -1)	// print next team member...
				{
					cl = &game.clients[sorted[j][i]];
					cl_ent = g_edicts + 1 + sorted[j][i];
					if (cl_ent->solid != SOLID_NOT && cl_ent->deadflag != DEAD_DEAD)
						totalaliveprinted[j]++;

					// AQ truncates names at 12, not sure why, except maybe to conserve scoreboard
					// string space?  skipping that "feature".  -FB

					sprintf (string + len,
					"xv %i yv %i string%s \"%s%s\" ",
					offset[j], 42 + i * 8,
					(deadview && cl_ent->solid != SOLID_NOT) ? "2" : "",
					(matchmode->value && game.clients[sorted[j][i]].resp.captain) ? "@" : "",
					game.clients[sorted[j][i]].pers.netname);

					len = strlen (string);
					if(ctf->value){
						if((j == TEAM1 && game.clients[sorted[j][i]].pers.inventory[ITEM_INDEX(flag2_item)])
						|| (j == TEAM2 && game.clients[sorted[j][i]].pers.inventory[ITEM_INDEX(flag1_item)])) {
							sprintf(string + len, "xv %i yv %i picn sbfctf%s ", offset[j]-8, 42 + i * 8, j == TEAM1 ? "2" : "1");
						len = strlen(string);
						}
					}
				}
			}
		}

		// Print remaining players if we ran out of room...
		if (!deadview)	// live player viewing scoreboard...
		{
			for(i=TEAM1; i<= teamCount; i++)
			{
				if (stoppedat[i] > -1)
				{
					sprintf (string + len,
						"xv %i yv %i string \"..and %i more\" ",
						offset[i], 42 + (stoppedat[i] * 8),
						total[i] - stoppedat[i]);
					len = strlen (string);
				}
			}
		}
		else			// dead player viewing scoreboard...
		{
			for(i=TEAM1; i<= teamCount; i++)
			{
				if (stoppedat[i] > -1)
				{
					sprintf (string + len,
						"xv %i yv %i string%s \"..and %i/%i more\" ",
						offset[i], 42 + (stoppedat[i] * 8),
						(totalalive[i] - totalaliveprinted[i]) ? "2" : "",
						totalalive[i] - totalaliveprinted[i],
						total[i] - stoppedat[i]);
					len = strlen (string);
				}
			}
		}
		
		if(matchmode->value) //Subs
		{

			for(i=TEAM1; i<= teamCount; i++)
			{
				sprintf (string + len, "xv %i yv 96 string2 \"Subs\" ",	offset[i]);
				len = strlen (string);
			}


			for (i = 0; i < game.maxclients; i++)
			{
				cl_ent = g_edicts + 1 + i;
				if (!cl_ent->inuse)
					continue;

				if (!game.clients[i].resp.subteam)
					continue;

				j = game.clients[i].resp.subteam;

				if (totalsubs[j] < 3 || !subs[j])
				{
					sprintf (string + strlen (string),
						"xv %i yv %d string \"%s%s\" ",
						offset[j], 104 + subs[j] * 8,
						game.clients[i].resp.captain ? "@" : "",
						game.clients[i].pers.netname);
				}
				else if (subs[j] < 2)
				{
					sprintf (string + strlen (string),
					"xv %i yv %i string \" + %i more\" ",
					offset[j], 104 + subs[j] * 8, totalsubs[j] - 1);
				}
				subs[j]++;
			}

			for(i=TEAM1; i<= teamCount; i++)
			{
				sprintf (string + strlen(string), "xv %i yv 128 string2 \"%s\" ",
					offset[i]+39, teams[i].ready ? "Ready" : "Not Ready");
			}

			mins = matchtime / 60;
			secs = matchtime - (mins * 60);
			sprintf (string + strlen (string), "xv 112 yv 144 string \"Time: %d:%02d\" ", mins, secs);
		}
	}
	else if (ent->client->scoreboardnum == 2)
	{
		int total = 0, score = 0, ping;
		int sorted[MAX_CLIENTS];

		for (i = 0; i < game.maxclients; i++)
		{
			cl_ent = g_edicts + 1 + i;
			if (!cl_ent->inuse)
				continue;

			score = game.clients[i].resp.score;
			if (noscore->value)
			{
				j = total;
			}
			else
			{
				for (j = 0; j < total; j++)
				{
					if (score > game.clients[sorted[j]].resp.score)
						break;
					if (score == game.clients[sorted[j]].resp.score &&
						game.clients[i].resp.damage_dealt > game.clients[sorted[j]].resp.damage_dealt)
							break;
				}
				for (k = total; k > j; k--)
					sorted[k] = sorted[k - 1];
			}
			sorted[j] = i;
			total++;
		}


		if (noscore->value)
		// AQ2:TNG Deathwatch - Nice little bar
		{
			strcpy (string,
			"xv 0 yv 32 string2 \"Player          Time Ping\" "
			"xv 0 yv 40 string2 \"ùûûûûûûûûûûûûûü ùûûü ùûûü\" ");
		}
		else if (teamdm->value)
		{
			if(matchmode->value) {
				strcpy(string,
					   "xv 0 yv 32 string2 \"Frags Player          Time Ping Deaths Team\" "
					   "xv 0 yv 40 string2 \"ùûûûü ùûûûûûûûûûûûûûü ùûûü ùûûü ùûûûûü ùûûü\" ");
			} else {
				strcpy(string,
					   "xv 0 yv 32 string2 \"Frags Player          Time Ping Deaths Kills\" "
					   "xv 0 yv 40 string2 \"ùûûûü ùûûûûûûûûûûûûûü ùûûü ùûûü ùûûûûü ùûûûü\" ");
			}
		}
		else if (matchmode->value)
		{
			strcpy (string,
			"xv 0 yv 32 string2 \"Frags Player          Time Ping Damage Team \" "
			"xv 0 yv 40 string2 \"ùûûûü ùûûûûûûûûûûûûûü ùûûü ùûûü ùûûûûü ùûûü\" ");

		}
		else
		{
			strcpy (string,
			"xv 0 yv 32 string2 \"Frags Player          Time Ping Damage Kills\" "
			"xv 0 yv 40 string2 \"ùûûûü ùûûûûûûûûûûûûûü ùûûü ùûûü ùûûûûü ùûûûü\" ");
		}
      /*
         {
         strcpy (string, "xv 0 yv 32 string2 \"Player          Time Ping\" "
         "xv 0 yv 40 string2 \"--------------- ---- ----\" ");
         }
         else
         {
         strcpy (string, "xv 0 yv 32 string2 \"Frags Player          Time Ping Damage Kills\" "
         "xv 0 yv 40 string2 \"----- --------------- ---- ---- ------ -----\" ");
         }
       */
      // AQ2:TNG END

		for (i = 0; i < total; i++)
		{
			ping = game.clients[sorted[i]].ping;
			if (ping > 999)
				ping = 999;

			if (noscore->value)
			{
				sprintf(string + strlen(string), "xv 0 yv %d string \"%-15s %4d %4d\" ",
					48 + i * 8,	game.clients[sorted[i]].pers.netname,
					(level.framenum - game.clients[sorted[i]].resp.enterframe) / 600,
					ping);
			}
			else
			{
				if(teamdm->value) {
					if (game.clients[sorted[i]].resp.deaths < 1000000)
						sprintf (damage, "%i", game.clients[sorted[i]].resp.deaths);
					else
						strcpy (damage, "999999");
				} else {
					if (game.clients[sorted[i]].resp.damage_dealt < 1000000)
						sprintf (damage, "%i", game.clients[sorted[i]].resp.damage_dealt);
					else
						strcpy (damage, "999999");
				}


				sprintf(string + strlen(string), "xv 0 yv %d string \"%5d %-15s %4d %4d %6s",
					48 + i * 8,	game.clients[sorted[i]].resp.score,
					game.clients[sorted[i]].pers.netname,
					(level.framenum - game.clients[sorted[i]].resp.enterframe) / 600,
					ping, damage);

				if(matchmode->value)
				{
					sprintf (string + strlen(string), " %i%s%s \" ",
						game.clients[sorted[i]].resp.team,
						game.clients[sorted[i]].resp.captain == 0 ? "" : "C",
						game.clients[sorted[i]].resp.subteam == 0 ? "" : "S");
				}
				else
				{
					sprintf(string + strlen(string), " %5i\" ",
						game.clients[sorted[i]].resp.kills);
				}
			}

			if (strlen(string) > (maxsize - 100) && i < (total - 2))
			{
				sprintf (string + strlen (string), "xv 0 yv %d string \"..and %d more\" ",
					48 + (i + 1) * 8, (total - i - 1));
				break;
			}
		}
	}


	if (strlen(string) > 1023)	// for debugging...
	{
		gi.dprintf
		("Warning: scoreboard string neared or exceeded max length\nDump:\n%s\n---\n",
		string);
		string[1023] = '\0';
	}

	gi.WriteByte (svc_layout);
	gi.WriteString (string);
}

// called when we enter the intermission
void TallyEndOfLevelTeamScores (void)
{
	int i;
	gi.sound (&g_edicts[0], CHAN_VOICE | CHAN_NO_PHS_ADD,
		gi.soundindex ("world/xian1.wav"), 1.0, ATTN_NONE, 0.0);

	teams[TEAM1].total = teams[TEAM2].total = teams[TEAM3].total = 0;
	for (i = 0; i < maxclients->value; i++)
	{
		if (!g_edicts[i + 1].inuse)
			continue;
		if (game.clients[i].resp.team == NOTEAM)
			continue;

		teams[game.clients[i].resp.team].total += game.clients[i].resp.score;
	}
}


/*
 * Teamplay spawning functions...
 */

edict_t *SelectTeamplaySpawnPoint (edict_t * ent)
{
  return teamplay_spawns[ent->client->resp.team - 1];
}

// SpawnPointDistance: 
// Returns the distance between two spawn points (or any entities, actually...)
float SpawnPointDistance (edict_t * spot1, edict_t * spot2)
{
	return Distance(spot1->s.origin, spot2->s.origin);
}

spawn_distances_t *spawn_distances;
// GetSpawnPoints:
// Put the spawn points into our potential_spawns array so we can work with them easily.
void GetSpawnPoints (void)
{
	edict_t *spot = NULL;

	num_potential_spawns = 0;

	if ((spot = G_Find (spot, FOFS (classname), "info_player_team1")) != NULL)
	{
		potential_spawns[num_potential_spawns] = spot;
		num_potential_spawns++;
	}

	if ((spot = G_Find (spot, FOFS (classname), "info_player_team2")) != NULL)
	{
		potential_spawns[num_potential_spawns] = spot;
		num_potential_spawns++;
	}

	spot = NULL;
	while ((spot = G_Find (spot, FOFS (classname), "info_player_deathmatch")) != NULL)
	{
		potential_spawns[num_potential_spawns] = spot;
		num_potential_spawns++;
		if (num_potential_spawns >= MAX_SPAWNS)
		{
			gi.dprintf ("Warning: MAX_SPAWNS exceeded\n");
			break;
		}
	}


	if(spawn_distances)
		gi.TagFree (spawn_distances);

	spawn_distances = (spawn_distances_t *)gi.TagMalloc (num_potential_spawns *
					sizeof (spawn_distances_t), TAG_GAME);
}

// newrand returns n, where 0 >= n < top
int newrand (int top)
{
	return (int) (random () * top);
}

// compare_spawn_distances is used by the qsort() call
int compare_spawn_distances (const void *sd1, const void *sd2)
{
	if (((spawn_distances_t *)sd1)->distance < ((spawn_distances_t *)sd2)->distance)
		return -1;
	else if (((spawn_distances_t *)sd1)->distance > ((spawn_distances_t *)sd2)->distance)
		return 1;
	else
		return 0;
}

void SelectRandomTeamplaySpawnPoint (int team, qboolean teams_assigned[])
{
	teamplay_spawns[team] = potential_spawns[newrand(num_potential_spawns)];
	teams_assigned[team] = true;
}

void SelectFarTeamplaySpawnPoint (int team, qboolean teams_assigned[])
{
	int x, y, spawn_to_use, preferred_spawn_points, num_already_used,
	total_good_spawn_points;
	float closest_spawn_distance, distance;


	num_already_used = 0;
	for (x = 0; x < num_potential_spawns; x++)
	{
		closest_spawn_distance = 2000000000;

		for (y = 0; y < teamCount; y++)
		{
			if (teams_assigned[y])
			{
				distance = SpawnPointDistance (potential_spawns[x], teamplay_spawns[y]);

				if (distance < closest_spawn_distance)
					closest_spawn_distance = distance;
			}
		}

		if (closest_spawn_distance == 0)
			num_already_used++;

		spawn_distances[x].s = potential_spawns[x];
		spawn_distances[x].distance = closest_spawn_distance;
	}

	qsort (spawn_distances, num_potential_spawns,
	sizeof (spawn_distances_t), compare_spawn_distances);

	total_good_spawn_points = num_potential_spawns - num_already_used;

	if (total_good_spawn_points <= 4)
		preferred_spawn_points = 1;
	else if (total_good_spawn_points <= 10)
		preferred_spawn_points = 2;
	else
		preferred_spawn_points = 3;

	//FB 6/1/99 - make DF_SPAWN_FARTHEST force far spawn points in TP
	if ((int) dmflags->value & DF_SPAWN_FARTHEST)
		preferred_spawn_points = 1;
	//FB 6/1/99

	spawn_to_use = newrand (preferred_spawn_points);

	if (team < 0 || team >= MAX_TEAMS)
	{
		gi.dprintf("Out-of-range teams value in SelectFarTeamplaySpawnPoint, skipping...\n");
	}
	else
	{
		teams_assigned[team] = true;
		teamplay_spawns[team] =
		spawn_distances[num_potential_spawns - spawn_to_use - 1].s;
	}

	
}

// SetupTeamSpawnPoints:
//
// Setup the points at which the teams will spawn.
//
void SetupTeamSpawnPoints ()
{
	qboolean teams_assigned[MAX_TEAMS];
	int i, l;

	for (l = 0; l < teamCount; l++)
	{
		teamplay_spawns[l] = NULL;
		teams_assigned[l] = false;
	}

	l = newrand(teamCount);

	SelectRandomTeamplaySpawnPoint(l, teams_assigned);

	for(i=l+1; i<teamCount+l; i++)
		SelectFarTeamplaySpawnPoint(i % teamCount, teams_assigned);

}


// TNG:Freud New Spawning system
// NS_GetSpawnPoints:
// Put the potential spawns into arrays for each team.
void NS_GetSpawnPoints ()
{
	int x, i;

	NS_randteam = newrand(2);

	for (x = 0; x < teamCount; x++)
	{
		NS_num_used_farteamplay_spawns[x] = 0;
		NS_num_potential_spawns[x] = num_potential_spawns;
		for(i = 0; i < num_potential_spawns; i++)
			NS_potential_spawns[x][i] = potential_spawns[i];
	}
}

// TNG:Freud
// NS_SelectRandomTeamplaySpawnPoint
// Select a random spawn point for the team from remaining spawns.
// returns false if no spawns left.
qboolean NS_SelectRandomTeamplaySpawnPoint (int team, qboolean teams_assigned[])
{
	int spawn_point, z;

	if (NS_num_potential_spawns[team] < 1) {
		gi.dprintf("New Spawncode: gone through all spawns, re-reading spawns\n");
		NS_GetSpawnPoints ();
		NS_SetupTeamSpawnPoints ();
		return false;
	}

	spawn_point = newrand (NS_num_potential_spawns[team]);
	NS_num_potential_spawns[team]--;

	teamplay_spawns[team] = NS_potential_spawns[team][spawn_point];
	teams_assigned[team] = true;

	for (z = spawn_point;z < NS_num_potential_spawns[team];z++) {
		NS_potential_spawns[team][z] = NS_potential_spawns[team][z + 1];
	}

	return true;
}

// TNG:Freud
// NS_SelectFarTeamplaySpawnPoint
// Selects farthest teamplay spawn point from available spawns.
qboolean NS_SelectFarTeamplaySpawnPoint (int team, qboolean teams_assigned[])
{
	int u, x, y, z, spawn_to_use, preferred_spawn_points, num_already_used,
	total_good_spawn_points;
	float closest_spawn_distance, distance;
	edict_t *usable_spawns[3];
	qboolean used;
	int num_usable;


	num_already_used = 0;
	for (x = 0; x < NS_num_potential_spawns[team]; x++)
	{
		closest_spawn_distance = 2000000000;

		for (y = 0; y < teamCount; y++)
		{
			if (teams_assigned[y])
			{
				distance =
				SpawnPointDistance (NS_potential_spawns[team][x], teamplay_spawns[y]);

				if (distance < closest_spawn_distance)
					closest_spawn_distance = distance;
			}
		}

		if (closest_spawn_distance == 0)
			num_already_used++;

		spawn_distances[x].s = NS_potential_spawns[team][x];
		spawn_distances[x].distance = closest_spawn_distance;
	}

	qsort (spawn_distances, NS_num_potential_spawns[team],
	sizeof (spawn_distances_t), compare_spawn_distances);

	total_good_spawn_points = NS_num_potential_spawns[team] - num_already_used;

	if (total_good_spawn_points <= 4)
		preferred_spawn_points = 1;
	else if (total_good_spawn_points <= 10)
		preferred_spawn_points = 2;
	else
		preferred_spawn_points = 3;

	//FB 6/1/99 - make DF_SPAWN_FARTHEST force far spawn points in TP
	if ((int) dmflags->value & DF_SPAWN_FARTHEST)
		preferred_spawn_points = 1;
	//FB 6/1/99

	num_usable = 0;
	for (z = 0;z < preferred_spawn_points;z++) {
		used = false;
		for (u = 0;u < NS_num_used_farteamplay_spawns[team];u++) {
			if (NS_used_farteamplay_spawns[team][u] ==
			spawn_distances[NS_num_potential_spawns[team] - z - 1].s) {
				used = true;
				break;
			}
		}
		if (used == false) {
			usable_spawns[num_usable] = spawn_distances[NS_num_potential_spawns[team] - z - 1].s;
			num_usable++;
		}
	}
	if (num_usable < 1) {
		NS_SetupTeamSpawnPoints ();
		return false;
	}

	spawn_to_use = newrand (num_usable);

	NS_used_farteamplay_spawns[team][NS_num_used_farteamplay_spawns[team]] = usable_spawns[spawn_to_use];
	NS_num_used_farteamplay_spawns[team]++;

	if (team < 0 || team >= MAX_TEAMS)
	{
		gi.dprintf("Out-of-range teams value in SelectFarTeamplaySpawnPoint, skipping...\n");
	}
	else
	{
		teams_assigned[team] = true;
		teamplay_spawns[team] = usable_spawns[spawn_to_use];
	}


	return true;
}

// TNG:Freud
// NS_SetupTeamSpawnPoints
// Finds and assigns spawn points to each team.
void NS_SetupTeamSpawnPoints ()
{
	qboolean teams_assigned[MAX_TEAMS];
	int l;


	for (l = 0; l < teamCount; l++)
	{
		teamplay_spawns[l] = NULL;
		teams_assigned[l] = false;
	}

	if (NS_SelectRandomTeamplaySpawnPoint (NS_randteam, teams_assigned) == false)
		return;

	for (l = 0;l < teamCount;l++) {
		// TNG:Freud disabled 3teams for new spawning system.
		if (l != NS_randteam &&
			NS_SelectFarTeamplaySpawnPoint (l, teams_assigned) == false)
				return;
	}
}

