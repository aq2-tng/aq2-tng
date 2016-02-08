//-----------------------------------------------------------------------------
// p_client.c
//
// $Id: p_client.c,v 1.90 2004/09/23 00:09:44 slicerdw Exp $
//
//-----------------------------------------------------------------------------
// $Log: p_client.c,v $
// Revision 1.90  2004/09/23 00:09:44  slicerdw
// Radio kill count was missing for falling death
//
// Revision 1.89  2004/04/08 23:19:51  slicerdw
// Optimized some code, added a couple of features and fixed minor bugs
//
// Revision 1.88  2003/10/01 19:24:14  igor_rock
// corrected a smaller bug (thanks to nopcode for the bug report)
//
// Revision 1.87  2003/06/15 21:43:53  igor
// added IRC client
//
// Revision 1.86  2003/06/15 15:34:32  igor
// - removed the zcam code from this branch (see other branch)
// - added fixes from 2.72 (source only) version
// - resetted version number to 2.72
// - This version should be exactly like the release 2.72 - just with a few
//   more fixes (which whoever did the source only variant didn't get because
//   he didn't use the CVS as he should. Shame on him.
//
// Revision 1.85  2003/02/10 02:12:25  ra
// Zcam fixes, kick crashbug in CTF fixed and some code cleanup.
//
// Revision 1.84  2002/12/31 17:07:22  igor_rock
// - corrected the Add_Ammo function to regard wp_flags
//
// Revision 1.83  2002/12/30 12:58:16  igor_rock
// - Corrected some comments (now it looks better)
// - allweapon mode now recognizes wp_flags
//
// Revision 1.82  2002/09/04 11:23:10  ra
// Added zcam to TNG and bumped version to 3.0
//
// Revision 1.81  2002/04/01 15:16:06  freud
// Stats code redone, tng_stats now much more smarter. Removed a few global
// variables regarding stats code and added kevlar hits to stats.
//
// Revision 1.80  2002/03/28 13:30:36  freud
// Included time played in ghost.
//
// Revision 1.79  2002/03/28 12:10:11  freud
// Removed unused variables (compiler warnings).
// Added cvar mm_allowlock.
//
// Revision 1.78  2002/03/25 23:35:19  freud
// Ghost code, use_ghosts and more stuff..
//
// Revision 1.77  2002/03/25 18:57:36  freud
// Added maximum number of stored player sessions (ghosts)
//
// Revision 1.76  2002/03/25 18:32:11  freud
// I'm being too productive.. New ghost command needs testing.
//
// Revision 1.75  2002/02/23 18:33:52  freud
// Fixed newline bug with announcer (EXCELLENT.. 1 FRAG LEFT) for logfiles
//
// Revision 1.74  2002/02/23 18:12:14  freud
// Added newlines back to the CenterPrintAll for IMPRESSIVE, EXCELLENT,
// ACCURACY and X FRAGS Left, it was screwing up the logfile.
//
// Revision 1.73  2002/02/19 09:32:47  freud
// Removed PING PONGs from CVS, not fit for release.
//
// Revision 1.72  2002/02/18 23:38:05  freud
// PING PONG..
//
// Revision 1.71  2002/02/18 23:25:42  freud
// More tweaks
//
// Revision 1.70  2002/02/18 20:21:36  freud
// Added PING PONG mechanism for timely disconnection of clients. This is
// based on a similar scheme as the scheme used by IRC. The client has
// cvar ping_timeout seconds to reply or will be disconnected.
//
// Revision 1.69  2002/02/18 18:25:51  ra
// Bumped version to 2.6, fixed ctf falling and kicking of players in ctf
// uvtime
//
// Revision 1.68  2002/02/18 17:17:21  freud
// Fixed the CTF leaving team bug. Also made the shield more efficient,
// No falling damage.
//
// Revision 1.67  2002/02/18 13:55:35  freud
// Added last damaged players %P
//
// Revision 1.66  2002/02/17 20:10:09  freud
// Better naming of auto_items is auto_equip, requested by Deathwatch.
//
// Revision 1.65  2002/02/17 20:01:32  freud
// Fixed stat_mode overflows, finally.
// Added 2 new cvars:
//      auto_join (0|1), enables auto joining teams from previous map.
//      auto_items (0|1), enables weapon and items caching between maps.
//
// Revision 1.64  2002/02/17 19:04:15  freud
// Possible bugfix for overflowing clients with stat_mode set.
//
// Revision 1.63  2002/02/01 16:09:49  freud
// Fixed the Taught how to fly bug
//
// Revision 1.62  2002/02/01 14:29:18  ra
// Attempting to fix tought how to fly no frag bug
//
// Revision 1.61  2002/01/31 11:15:06  freud
// Fix for crashes with stat_mode, not sure it works.
//
// Revision 1.60  2002/01/24 11:38:01  ra
// Cleanups
//
// Revision 1.59  2002/01/23 13:08:32  ra
// fixing tought how to fly bug
//
// Revision 1.58  2001/12/24 18:06:05  slicerdw
// changed dynamic check for darkmatch only
//
// Revision 1.56  2001/12/23 16:30:50  ra
// 2.5 ready. New stats from Freud. HC and shotgun gibbing seperated.
//
// Revision 1.55  2001/12/09 14:02:11  slicerdw
// Added gl_clear check -> video_check_glclear cvar
//
// Revision 1.54  2001/11/29 17:58:31  igor_rock
// TNG IRC Bot - First Version
//
// Revision 1.53  2001/11/08 20:56:24  igor_rock
// - changed some things related to wp_flags
// - corrected use_punch bug when player only has an empty weapon left
//
// Revision 1.52  2001/09/28 16:24:20  deathwatch
// use_rewards now silences the teamX wins sounds and added gibbing for the Shotgun
//
// Revision 1.51  2001/09/28 13:48:35  ra
// I ran indent over the sources. All .c and .h files reindented.
//
// Revision 1.50  2001/09/28 13:44:23  slicerdw
// Several Changes / improvements
//
// Revision 1.49  2001/09/05 14:33:57  slicerdw
// Added Fix's from the 2.1 release
//
// Revision 1.48  2001/09/03 14:25:00  deathwatch
// Added gibbing with HC (only happens rarely) when sv_gib is on and cleaned up
// the player_die function and made sure the flashlight gets turned off when someone
// is dead.
//
// Revision 1.47  2001/09/02 20:33:34  deathwatch
// Added use_classic and fixed an issue with ff_afterround, also updated version
// nr and cleaned up some commands.
//
// Updated the VC Project to output the release build correctly.
//
// Revision 1.46  2001/08/19 01:22:25  deathwatch
// cleaned the formatting of some files
//
// Revision 1.45  2001/08/17 21:31:37  deathwatch
// Added support for stats
//
// Revision 1.44  2001/08/15 14:50:48  slicerdw
// Added Flood protections to Radio & Voice, Fixed the sniper bug AGAIN
//
// Revision 1.43  2001/08/08 12:42:22  slicerdw
// Ctf Should finnaly be fixed now, lets hope so
//
// Revision 1.42  2001/08/06 23:35:31  ra
// Fixed an uvtime bug when clients join server while CTF rounds are already
// going.
//
// Revision 1.41  2001/08/06 14:38:45  ra
// Adding UVtime for ctf
//
// Revision 1.40  2001/08/06 13:41:41  slicerdw
// Added a fix for ctf..
//
// Revision 1.39  2001/08/06 03:00:49  ra
// Added FF after rounds. Please someone look at the EVIL if statments for me :)
//
// Revision 1.38  2001/08/03 15:08:32  ra
// Fix small bug in %K related to "tought how to fly" deaths.
//
// Revision 1.37  2001/07/20 11:56:04  slicerdw
// Added a check for the players spawning during countdown on ctf ( lets hope it works )
//
// Revision 1.36  2001/06/27 16:58:14  igor_rock
// corrected some limchasecam bugs
//
// Revision 1.35  2001/06/26 18:47:30  igor_rock
// added ctf_respawn cvar
//
// Revision 1.34  2001/06/25 11:44:47  slicerdw
// New Video Check System - video_check and video_check_lockpvs no longer latched
//
// Revision 1.33  2001/06/22 20:35:07  igor_rock
// fixed the flying corpse bug
//
// Revision 1.32  2001/06/22 18:37:01  igor_rock
// fixed than damn limchasecam bug - eentually :)
//
// Revision 1.31  2001/06/21 07:37:10  igor_rock
// fixed some limchasecam bugs
//
// Revision 1.30  2001/06/21 00:05:30  slicerdw
// New Video Check System done -  might need some revision but works..
//
// Revision 1.27  2001/06/20 07:21:21  igor_rock
// added use_warnings to enable/disable time/frags left msgs
// added use_rewards to enable/disable eimpressive, excellent and accuracy msgs
// change the configfile prefix for modes to "mode_" instead "../mode-" because
// they don't have to be in the q2 dir for doewnload protection (action dir is sufficient)
// and the "-" is bad in filenames because of linux command line parameters start with "-"
//
// Revision 1.26  2001/06/19 21:35:54  igor_rock
// If you select Sniper, sniper is your startweapon now.
//
// Revision 1.25  2001/06/19 21:10:05  igor_rock
// changed the "is now known" message to the normal namelimit
//
// Revision 1.24  2001/06/19 18:56:38  deathwatch
// New Last killed target system
//
// Revision 1.23  2001/06/18 18:14:09  igor_rock
// corrected bug with team none players shooting and flying around
//
// Revision 1.22  2001/06/01 19:18:42  slicerdw
// Added Matchmode Code
//
// Revision 1.21  2001/05/31 16:58:14  igor_rock
// conflicts resolved
//
// Revision 1.20  2001/05/20 15:00:19  slicerdw
// Some minor fixes and changings on Video Checking system
//
// Revision 1.19.2.3  2001/05/25 18:59:52  igor_rock
// Added CTF Mode completly :)
// Support for .flg files is still missing, but with "real" CTF maps like
// tq2gtd1 the ctf works fine.
// (I hope that all other modes still work, just tested DM and teamplay)
//
// Revision 1.19.2.2  2001/05/20 18:54:19  igor_rock
// added original ctf code snippets from zoid. lib compilesand runs but
// doesn't function the right way.
// Jsut committing these to have a base to return to if something wents
// awfully wrong.
//
// Revision 1.19.2.1  2001/05/20 15:17:31  igor_rock
// removed the old ctf code completly
//
// Revision 1.19  2001/05/20 12:54:18  igor_rock
// Removed newlines from Centered Messages like "Impressive"
//
// Revision 1.18  2001/05/16 13:26:38  slicerdw
// Too Many Userinfo Cvars( commented some) & Enabled death messages on CTF
//
// Revision 1.17  2001/05/13 15:01:45  ra
//
//
// In teamplay mode it is OK to teamkill as soon as the rounds are over. Changed
// things so that when the rounds are over it is also OK to plummet.
//
// Revision 1.16  2001/05/13 01:23:01  deathwatch
// Added Single Barreled Handcannon mode, made the menus and scoreboards
// look nicer and made the voice command a bit less loud.
//
// Revision 1.14  2001/05/12 13:51:20  mort
// Fixed ClientObituary Add_Frag bug in CTF
//
// Revision 1.13  2001/05/12 13:48:58  mort
// Fixed CTF ForceSpawn bug
//
// Revision 1.12  2001/05/12 13:37:38  mort
// Fixed CTF bug, god mode is now on when players spawn
//
// Revision 1.11  2001/05/12 10:50:16  slicerdw
// Fixed That Transparent List Thingy
//
// Revision 1.10  2001/05/11 16:07:26  mort
// Various CTF bits and pieces...
//
// Revision 1.9  2001/05/11 12:21:19  slicerdw
// Commented old Location support ( ADF ) With the ML/ETE Compatible one
//
// Revision 1.7  2001/05/07 21:18:35  slicerdw
// Added Video Checking System
//
// Revision 1.6  2001/05/07 20:06:45  igor_rock
// changed sound dir from sound/rock to sound/tng
//
// Revision 1.5  2001/05/07 02:05:36  ra
//
//
// Added tkok command to forgive teamkills.
//
// Revision 1.4  2001/05/07 01:44:07  ra
//
//
// Add a fix for the $$ skin crashing the server.
//
// Revision 1.3  2001/05/06 20:29:21  ra
//
//
// Adding comments to the limchasecam fix.
//
// Revision 1.2  2001/05/06 20:20:49  ra
//
//
// Fixing limchasecam.
//
// Revision 1.1.1.1  2001/05/06 17:29:49  igor_rock
// This is the PG Bund Edition V1.25 with all stuff laying around here...
//
//-----------------------------------------------------------------------------

#include "g_local.h"
#include "m_player.h"
#include "cgf_sfx_glass.h"

void ClientUserinfoChanged(edict_t * ent, char *userinfo);
void ClientDisconnect(edict_t * ent);
void SP_misc_teleporter_dest(edict_t * ent);
void CopyToBodyQue(edict_t * ent);

void Add_Frag(edict_t * ent, int mod)
{
	char buf[256];
	int frags = 0;

	if (in_warmup)
		return;

	ent->client->resp.kills++;
	if (mod > 0 && mod < MAX_GUNSTAT) {
		ent->client->resp.gunstats[mod].kills++;
	}

	if (teamplay->value && teamdm->value != 2)
	{
		ent->client->resp.score++;	// just 1 normal kill

		if (ent->solid != SOLID_NOT && ent->deadflag != DEAD_DEAD)
		{
			ent->client->resp.streakKills++;
			if (ent->client->resp.streakKills % 5 == 0 && use_rewards->value)
			{
				sprintf(buf, "IMPRESSIVE %s!", ent->client->pers.netname);
				CenterPrintAll(buf);
				gi.sound(&g_edicts[0], CHAN_VOICE | CHAN_NO_PHS_ADD,
					 gi.soundindex("tng/impressive.wav"), 1.0, ATTN_NONE, 0.0);
			}
			else if (ent->client->resp.streakKills % 12 == 0 && use_rewards->value)
			{
				sprintf(buf, "EXCELLENT %s (%dx)!", ent->client->pers.netname,ent->client->resp.streakKills/12);
				CenterPrintAll(buf);
				gi.sound(&g_edicts[0], CHAN_VOICE | CHAN_NO_PHS_ADD,
					 gi.soundindex("tng/excellent.wav"), 1.0, ATTN_NONE, 0.0);
			}
		}

		if(teamdm->value)
			teams[ent->client->resp.team].score++;
		// end changing sound dir
	} else { //Deathmatch

		if (ent->solid != SOLID_NOT && ent->deadflag != DEAD_DEAD)
			ent->client->resp.streakKills++;

		if (ent->client->resp.streakKills < 4)
			frags = 1;
		else if (ent->client->resp.streakKills < 8)
			frags = 2;
		else if (ent->client->resp.streakKills < 16)
			frags = 4;
		else if (ent->client->resp.streakKills < 32)
			frags = 8;
		else
			frags = 16;

		if(frags > 1)
		{
			gi.bprintf(PRINT_MEDIUM,
				"%s has %d kills in a row and receives %d frags for the kill!\n",
				ent->client->pers.netname, ent->client->resp.streakKills, frags );
			IRC_printf(IRC_T_GAME,
				"%n has %k kills in a row and receives %k frags for the kill!",
				ent->client->pers.netname, ent->client->resp.streakKills, frags );
		}
		ent->client->resp.score += frags;

		if(ent->client->resp.streakKills)
			gi.cprintf(ent, PRINT_HIGH, "Kill count: %d\n", ent->client->resp.streakKills);

		if(teamdm->value)
			teams[ent->client->resp.team].score += frags;
	}

	// AQ:TNG Igor[Rock] changing sound dir
	if (fraglimit->value && use_warnings->value) {
		if (ent->client->resp.score == fraglimit->value - 1) {
			if (fragwarning < 3) {
				CenterPrintAll("1 FRAG LEFT...");
				gi.sound(&g_edicts[0], CHAN_VOICE | CHAN_NO_PHS_ADD,
					 gi.soundindex("tng/1_frag.wav"), 1.0, ATTN_NONE, 0.0);
				fragwarning = 3;
			}
		} else if (ent->client->resp.score == fraglimit->value - 2) {
			if (fragwarning < 2) {
				CenterPrintAll("2 FRAGS LEFT...");
				gi.sound(&g_edicts[0], CHAN_VOICE | CHAN_NO_PHS_ADD,
					 gi.soundindex("tng/2_frags.wav"), 1.0, ATTN_NONE, 0.0);
				fragwarning = 2;
			}
		} else if (ent->client->resp.score == fraglimit->value - 3) {
			if (fragwarning < 1) {
				CenterPrintAll("3 FRAGS LEFT...");
				gi.sound(&g_edicts[0], CHAN_VOICE | CHAN_NO_PHS_ADD,
					 gi.soundindex("tng/3_frags.wav"), 1.0, ATTN_NONE, 0.0);
				fragwarning = 1;
			}
		}
	}
	// end of changing sound dir
}

void Subtract_Frag(edict_t * ent)
{
	ent->client->resp.score--;
	ent->client->resp.streakKills = 0;
	if(teamdm->value)
		teams[ent->client->resp.team].score--;
}

// FRIENDLY FIRE functions

void Add_TeamWound(edict_t * attacker, edict_t * victim, int mod)
{
	if (!teamplay->value || !attacker->client || !victim->client) {
		return;
	}

	attacker->client->team_wounds++;

	// Warn both parties that they are teammates. Since shotguns are pellet based,
	// make sure we don't overflow the client when using MOD_HC or MOD_SHOTGUN. The
	// ff_warning flag should have been reset before each attack.
	if (attacker->client->ff_warning == 0) {
		attacker->client->ff_warning++;
		gi.cprintf(victim, PRINT_HIGH, "You were hit by %s, your TEAMMATE!\n", attacker->client->pers.netname);
		gi.cprintf(attacker, PRINT_HIGH, "You hit your TEAMMATE %s!\n", victim->client->pers.netname);
	}
	// We want team_wounds to increment by one for each ATTACK, not after each 
	// bullet or pellet does damage. With the HAND CANNON this means 2 attacks
	// since it is double barreled and we don't want to go into p_weapon.c...
	attacker->client->team_wounds = (attacker->client->team_wounds_before + 1);

	// If count is less than MAX_TEAMKILLS*3, return. If count is greater than
	// MAX_TEAMKILLS*3 but less than MAX_TEAMKILLS*4, print off a ban warning. If
	// count equal (or greater than) MAX_TEAMKILLS*4, ban and kick the client.
	if ((int) maxteamkills->value < 1)	//FB
		return;
	if (attacker->client->team_wounds < ((int) maxteamkills->value * 3)) {
		return;
	} else if (attacker->client->team_wounds < ((int) maxteamkills->value * 4)) {
		// Print a note to console, and issue a warning to the player.
		gi.cprintf(NULL, PRINT_MEDIUM,
			   "%s is in danger of being banned for wounding teammates\n", attacker->client->pers.netname);
		gi.cprintf(attacker, PRINT_HIGH,
			   "WARNING: You'll be temporarily banned if you continue wounding teammates!\n");
		return;
	} else {
		if (attacker->client->ipaddr) {
			if (Ban_TeamKiller(attacker, (int) twbanrounds->value)) {
				gi.cprintf(NULL, PRINT_MEDIUM,
					   "Banning %s@%s for team wounding\n",
					   attacker->client->pers.netname, attacker->client->ipaddr);

				gi.cprintf(attacker, PRINT_HIGH,
					   "You've wounded teammates too many times, and are banned for %d %s.\n",
					   (int) twbanrounds->value,
					   (((int) twbanrounds->value > 1) ? "games" : "game"));
			} else {
				gi.cprintf(NULL, PRINT_MEDIUM,
					   "Error banning %s: unable to get ipaddr\n", attacker->client->pers.netname);
			}
			Kick_Client(attacker);
		}
	}

	return;
}

void Add_TeamKill(edict_t * attacker)
{
	if (!teamplay->value || !attacker->client || !team_round_going) {
		return;
	}

	attacker->client->team_kills++;
	// Because the stricter team kill was incremented, lower team_wounds
	// by amount inflicted in last attack (i.e., no double penalty).
	if (attacker->client->team_wounds > attacker->client->team_wounds_before) {
		attacker->client->team_wounds = attacker->client->team_wounds_before;
	}
	// If count is less than 1/2 MAX_TEAMKILLS, print off simple warning. If
	// count is greater than 1/2 MAX_TEAMKILLS but less than MAX_TEAMKILLS,
	// print off a ban warning. If count equal or greater than MAX_TEAMKILLS,
	// ban and kick the client.
	if (((int) maxteamkills->value < 1) ||
	    (attacker->client->team_kills < (((int) maxteamkills->value % 2) + (int) maxteamkills->value / 2))) {
		gi.cprintf(attacker, PRINT_HIGH, "You killed your TEAMMATE!\n");
		return;
	} else if (attacker->client->team_kills < (int) maxteamkills->value) {
		// Show this on the console
		gi.cprintf(NULL, PRINT_MEDIUM,
			   "%s is in danger of being banned for killing teammates\n", attacker->client->pers.netname);
		// Issue a warning to the player
		gi.cprintf(attacker, PRINT_HIGH, "WARNING: You'll be banned if you continue killing teammates!\n");
		return;
	} else {
		// They've killed too many teammates this game - kick 'em for a while
		if (attacker->client->ipaddr) {
			if (Ban_TeamKiller(attacker, (int) tkbanrounds->value)) {
				gi.cprintf(NULL, PRINT_MEDIUM,
					   "Banning %s@%s for team killing\n",
					   attacker->client->pers.netname, attacker->client->ipaddr);
				gi.cprintf(attacker, PRINT_HIGH,
					   "You've killed too many teammates, and are banned for %d %s.\n",
					   (int) tkbanrounds->value,
					   (((int) tkbanrounds->value > 1) ? "games" : "game"));
			} else {
				gi.cprintf(NULL, PRINT_MEDIUM,
					   "Error banning %s: unable to get ipaddr\n", attacker->client->pers.netname);
			}
		}
		Kick_Client(attacker);
	}
}

// FRIENDLY FIRE

//
// Gross, ugly, disgustuing hack section
//

/*QUAKED info_player_start (1 0 0) (-16 -16 -24) (16 16 32)
The normal starting point for a level.
*/
void SP_info_player_start( edict_t * self )
{
}

/*QUAKED info_player_deathmatch (1 0 1) (-16 -16 -24) (16 16 32)
potential spawning position for deathmatch games
*/
void SP_info_player_deathmatch(edict_t * self)
{
	SP_misc_teleporter_dest(self);
}

/*QUAKED info_player_intermission (1 0 1) (-16 -16 -24) (16 16 32)
The deathmatch intermission point will be at one of these
Use 'angles' instead of 'angle', so you can set pitch or roll as well as yaw.  'pitch yaw roll'
*/
void SP_info_player_intermission(void)
{
}

//=======================================================================

void player_pain(edict_t * self, edict_t * other, float kick, int damage)
{
	// player pain is handled at the end of the frame in P_DamageFeedback
}

// ^^^

// PrintDeathMessage: moved the actual printing of the death messages to here, to handle
//  the fact that live players shouldn't receive them in teamplay.  -FB
void PrintDeathMessage(char *msg, edict_t * gibee)
{
	int j;
	edict_t *other;

	if (!teamplay->value) {
		gi.bprintf(PRINT_MEDIUM, "%s", msg);
		return;
	}

	if (dedicated->value)
		gi.cprintf(NULL, PRINT_MEDIUM, "%s", msg);

	// First, let's print the message for gibee and its attacker. -TempFile
	gi.cprintf(gibee, PRINT_MEDIUM, "%s", msg);
	if (gibee->client->attacker && gibee->client->attacker != gibee)
		gi.cprintf(gibee->client->attacker, PRINT_MEDIUM, "%s", msg);

	if(!team_round_going)
		return;

	for (j = 1; j <= game.maxclients; j++) {
		other = &g_edicts[j];
		if (!other->inuse || !other->client)
			continue;

		// only print if he's NOT gibee, NOT attacker, and NOT alive! -TempFile
		if (other != gibee && other != gibee->client->attacker && other->solid == SOLID_NOT)
			gi.cprintf(other, PRINT_MEDIUM, "%s", msg);
	}
}

void ClientObituary(edict_t * self, edict_t * inflictor, edict_t * attacker)
{
	int mod;
	int loc;
	char *message;
	char *message2;
	char death_msg[1024];	// enough in all situations? -FB
	qboolean friendlyFire;
	int special = 0;
	int n;

	self->client->resp.ctf_capstreak = 0;

	friendlyFire = meansOfDeath & MOD_FRIENDLY_FIRE;
	mod = meansOfDeath & ~MOD_FRIENDLY_FIRE;
	loc = locOfDeath;	// useful for location based hits
	message = NULL;
	message2 = "";

	if (attacker == self)
	{
		switch (mod) {
		case MOD_HELD_GRENADE:
			message = "tried to put the pin back in";
			break;
		case MOD_HG_SPLASH:
			if (self->client->pers.gender == GENDER_MALE)
				message = "didn't throw his grenade far enough";
			else if (self->client->pers.gender == GENDER_FEMALE)
				message = "didn't throw her grenade far enough";
			else
				message = "didn't throw its grenade far enough";
			break;
		case MOD_G_SPLASH:
			if (self->client->pers.gender == GENDER_MALE)
				message = "tripped on his own grenade";
			else if (self->client->pers.gender == GENDER_FEMALE)
				message = "tripped on her own grenade";
			else
				message = "tripped on its own grenade";
			break;
		default:
			if (self->client->pers.gender == GENDER_MALE)
				message = "killed himself";
			else if (self->client->pers.gender == GENDER_FEMALE)
				message = "killed herself";
			else
				message = "killed itself";
			break;
		}
	}

	if (!message) {
		switch (mod) {
		case MOD_BREAKINGGLASS:
			message = "ate too much glass";
			break;
		case MOD_SUICIDE:
			message = "is done with the world";
			break;
		case MOD_FALLING:
			// moved falling to the end
			if (self->client->push_timeout)
				special = 1;
			//message = "hit the ground hard, real hard";
			if (self->client->pers.gender == GENDER_MALE)
				message = "plummets to his death";
			else if (self->client->pers.gender == GENDER_FEMALE)
				message = "plummets to her death";
			else
				message = "plummets to its death";
			break;
		case MOD_CRUSH:
			message = "was flattened";
			break;
		case MOD_WATER:
			message = "sank like a rock";
			break;
		case MOD_SLIME:
			message = "melted";
			break;
		case MOD_LAVA:
			message = "does a back flip into the lava";
			break;
		case MOD_EXPLOSIVE:
		case MOD_BARREL:
			message = "blew up";
			break;
		case MOD_EXIT:
			message = "found a way out";
			break;
		case MOD_TARGET_LASER:
			message = "saw the light";
			break;
		case MOD_TARGET_BLASTER:
			message = "got blasted";
			break;
		case MOD_BOMB:
		case MOD_SPLASH:
		case MOD_TRIGGER_HURT:
			message = "was in the wrong place";
			break;
		}
	}

	if (message)
	{
		// handle falling with an attacker set
		if (special && self->client->attacker && self->client->attacker->client
		&& (self->client->attacker->client != self->client))
		{
			sprintf(death_msg, "%s was taught how to fly by %s\n",
				self->client->pers.netname, self->client->attacker->client->pers.netname);
			PrintDeathMessage(death_msg, self);
			IRC_printf(IRC_T_KILL, death_msg);
			AddKilledPlayer(self->client->attacker, self);
			self->client->attacker->client->pers.num_kills++;

			//MODIFIED FOR FF -FB
			if (OnSameTeam(self, self->client->attacker))
			{
				if (!DMFLAGS(DF_NO_FRIENDLY_FIRE) && (!teamplay->value || team_round_going || !ff_afterround->value)) {
					self->enemy = self->client->attacker;
					Add_TeamKill(self->client->attacker);
					Subtract_Frag(self->client->attacker);	//attacker->client->resp.score--;
					self->client->resp.deaths++;
				}
			}
			else
			{
				self->client->resp.streakKills = 0;
				Add_Frag(self->client->attacker, MOD_UNKNOWN);
				self->client->resp.deaths++;
			}

		}
		else
		{
			sprintf( death_msg, "%s %s\n", self->client->pers.netname, message );
			PrintDeathMessage( death_msg, self );
			IRC_printf( IRC_T_DEATH, death_msg );

			if (!teamplay->value || team_round_going || !ff_afterround->value)  {
				Subtract_Frag( self );
				self->client->resp.deaths++;
			}

			self->enemy = NULL;
		}
		return;
	}
#if 0
		// handle bleeding, not used because bleeding doesn't get set
		if (mod == MOD_BLEEDING) {
			sprintf(death_msg, "%s bleeds to death\n", self->client->pers.netname);
			PrintDeathMessage(death_msg, self);
			return;
		}
#endif

	self->enemy = attacker;
	if (attacker && attacker->client)
	{
		switch (mod) {
		case MOD_MK23:	// zucc
			switch (loc) {
			case LOC_HDAM:
				if (self->client->pers.gender == GENDER_MALE)
					message = " has a hole in his head from";
				else if (self->client->pers.gender == GENDER_FEMALE)
					message = " has a hole in her head from";
				else
					message = " has a hole in its head from";
				message2 = "'s Mark 23 pistol";
				break;
			case LOC_CDAM:
				message = " loses a vital chest organ thanks to";
				message2 = "'s Mark 23 pistol";
				break;
			case LOC_SDAM:
				if (self->client->pers.gender == GENDER_MALE)
					message = " loses his lunch to";
				else if (self->client->pers.gender == GENDER_FEMALE)
					message = " loses her lunch to";
				else
					message = " loses its lunch to";
				message2 = "'s .45 caliber pistol round";
				break;
			case LOC_LDAM:
				message = " is legless because of";
				message2 = "'s .45 caliber pistol round";
				break;
			default:
				message = " was shot by";
				message2 = "'s Mark 23 Pistol";
			}
			break;
		case MOD_MP5:
			switch (loc) {
			case LOC_HDAM:
				message = "'s brains are on the wall thanks to";
				message2 = "'s 10mm MP5/10 round";
				break;
			case LOC_CDAM:
				message = " feels some chest pain via";
				message2 = "'s MP5/10 Submachinegun";
				break;
			case LOC_SDAM:
				message = " needs some Pepto Bismol after";
				message2 = "'s 10mm MP5 round";
				break;
			case LOC_LDAM:
				if (self->client->pers.gender == GENDER_MALE)
					message = " had his legs blown off thanks to";
				else if (self->client->pers.gender == GENDER_FEMALE)
					message = " had her legs blown off thanks to";
				else
					message = " had its legs blown off thanks to";
				message2 = "'s MP5/10 Submachinegun";
				break;
			default:
				message = " was shot by";
				message2 = "'s MP5/10 Submachinegun";
			}
			break;
		case MOD_M4:
			switch (loc) {
			case LOC_HDAM:
				message = " had a makeover by";
				message2 = "'s M4 Assault Rifle";
				break;
			case LOC_CDAM:
				message = " feels some heart burn thanks to";
				message2 = "'s M4 Assault Rifle";
				break;
			case LOC_SDAM:
				message = " has an upset stomach thanks to";
				message2 = "'s M4 Assault Rifle";
				break;
			case LOC_LDAM:
				message = " is now shorter thanks to";
				message2 = "'s M4 Assault Rifle";
				break;
			default:
				message = " was shot by";
				message2 = "'s M4 Assault Rifle";
			}
			break;
		case MOD_M3:
			n = rand() % 2 + 1;
			if (n == 1) {
				message = " accepts";
				message2 = "'s M3 Super 90 Assault Shotgun in hole-y matrimony";
			} else {
				message = " is full of buckshot from";
				message2 = "'s M3 Super 90 Assault Shotgun";
			}
			break;
		case MOD_HC:
			n = rand() % 2 + 1;
			if (n == 1) {
				if (attacker->client->resp.hc_mode)	// AQ2:TNG Deathwatch - Single Barreled HC Death Messages
				{
					message = " underestimated";
					message2 = "'s single barreled handcannon shot";
				} else {
					message = " ate";
					message2 = "'s sawed-off 12 gauge";
				}
			} else {
				if (attacker->client->resp.hc_mode)	// AQ2:TNG Deathwatch - Single Barreled HC Death Messages
				{
					message = " won't be able to pass a metal detector anymore thanks to";
					message2 = "'s single barreled handcannon shot";
				} else {
					message = " is full of buckshot from";
					message2 = "'s sawed off shotgun";
				}
			}
			break;
		case MOD_SNIPER:
			switch (loc) {
			case LOC_HDAM:
				if (self->client->ps.fov < 90) {
					if (self->client->pers.gender == GENDER_MALE)
						message = " saw the sniper bullet go through his scope thanks to";
					else if (self->client->pers.gender == GENDER_FEMALE)
						message = " saw the sniper bullet go through her scope thanks to";
					else
						message = " saw the sniper bullet go through its scope thanks to";
				} else
					message = " caught a sniper bullet between the eyes from";
				break;
			case LOC_CDAM:
				message = " was picked off by";
				break;
			case LOC_SDAM:
				message = " was sniped in the stomach by";
				break;
			case LOC_LDAM:
				message = " was shot in the legs by";
				break;
			default:
				message = " was sniped by";
				//message2 = "'s Sniper Rifle";
			}
			break;
		case MOD_DUAL:
			switch (loc) {
			case LOC_HDAM:
				message = " was trepanned by";
				message2 = "'s akimbo Mark 23 pistols";
				break;
			case LOC_CDAM:
				message = " was John Woo'd by";
				//message2 = "'s .45 caliber pistol round";
				break;
			case LOC_SDAM:
				message = " needs some new kidneys thanks to";
				message2 = "'s akimbo Mark 23 pistols";
				break;
			case LOC_LDAM:
				message = " was shot in the legs by";
				message2 = "'s akimbo Mark 23 pistols";
				break;
			default:
				message = " was shot by";
				message2 = "'s pair of Mark 23 Pistols";
			}
			break;
		case MOD_KNIFE:
			switch (loc) {
			case LOC_HDAM:
				if (self->client->pers.gender == GENDER_MALE)
					message = " had his throat slit by";
				else if (self->client->pers.gender == GENDER_FEMALE)
					message = " had her throat slit by";
				else
					message = " had its throat slit by";
				break;
			case LOC_CDAM:
				message = " had open heart surgery, compliments of";
				break;
			case LOC_SDAM:
				message = " was gutted by";
				break;
			case LOC_LDAM:
				message = " was stabbed repeatedly in the legs by";
				break;
			default:
				message = " was slashed apart by";
				message2 = "'s Combat Knife";
			}
			break;
		case MOD_KNIFE_THROWN:
			switch (loc) {
				case LOC_HDAM:
				message = " caught";
				if (self->client->pers.gender == GENDER_MALE)
					message2 = "'s flying knife with his forehead";
				else if (self->client->pers.gender == GENDER_FEMALE)
					message2 = "'s flying knife with her forehead";
				else
					message2 = "'s flying knife with its forehead";
				break;
			case LOC_CDAM:
				message = "'s ribs don't help against";
				message2 = "'s flying knife";
				break;
			case LOC_SDAM:
				if (self->client->pers.gender == GENDER_MALE)
					message = " sees the contents of his own stomach thanks to";
				else if (self->client->pers.gender == GENDER_FEMALE)
					message = " sees the contents of her own stomach thanks to";
				else
					message = " sees the contents of its own stomach thanks to";
				message2 = "'s flying knife";
				break;
			case LOC_LDAM:
				if (self->client->pers.gender == GENDER_MALE)
					message = " had his legs cut off thanks to";
				else if (self->client->pers.gender == GENDER_FEMALE)
					message = " had her legs cut off thanks to";
				else
					message = " had its legs cut off thanks to";
				message2 = "'s flying knife";
				break;
			default:
				message = " was hit by";
				message2 = "'s flying Combat Knife";
			}
			break;
		case MOD_KICK:
			n = rand() % 3 + 1;
			if (n == 1) {
				if (self->client->pers.gender == GENDER_MALE)
					message = " got his ass kicked by";
				else if (self->client->pers.gender == GENDER_FEMALE)
					message = " got her ass kicked by";
				else
					message = " got its ass kicked by";
			} else if (n == 2) {
				if (self->client->pers.gender == GENDER_MALE) {
					message = " couldn't remove";
					message2 = "'s boot from his ass";
				} else if (self->client->pers.gender == GENDER_FEMALE) {
					message = " couldn't remove";
					message2 = "'s boot from her ass";
				} else {
					message = " couldn't remove";
					message2 = "'s boot from its ass";
				}
			} else {
				if (self->client->pers.gender == GENDER_MALE) {
					message = " had a Bruce Lee put on him by";
					message2 = ", with a quickness";
				} else if (self->client->pers.gender == GENDER_FEMALE) {
					message = " had a Bruce Lee put on her by";
					message2 = ", with a quickness";
				} else {
					message = " had a Bruce Lee put on it by";
					message2 = ", with a quickness";
				}
			}
			break;
		case MOD_PUNCH:
			n = rand() % 3 + 1;
			if (n == 1) {
				message = " got a free facelift by";
			} else if (n == 2) {
				message = " was knocked out by";
			} else {
				message = " caught";
				message2 = "'s iron fist";
			}
			break;
		case MOD_BLASTER:
			message = "was blasted by";
			break;
		case MOD_GRENADE:
			message = "was popped by";
			message2 = "'s grenade";
			break;
		case MOD_G_SPLASH:
			message = "was shredded by";
			message2 = "'s shrapnel";
			break;
		case MOD_HYPERBLASTER:
			message = "was melted by";
			message2 = "'s hyperblaster";
			break;
		case MOD_HANDGRENADE:
			message = " caught";
			message2 = "'s handgrenade";
			break;
		case MOD_HG_SPLASH:
			message = " didn't see";
			message2 = "'s handgrenade";
			break;
		case MOD_HELD_GRENADE:
			message = " feels";
			message2 = "'s pain";
			break;
		case MOD_TELEFRAG:
			message = " tried to invade";
			message2 = "'s personal space";
			break;
		case MOD_GRAPPLE:
			message = " was caught by";
			message2 = "'s grapple";
			break;
		}	//end of case (mod)

		if (message)
		{
			sprintf(death_msg, "%s%s %s%s\n", self->client->pers.netname,
			message, attacker->client->pers.netname, message2);
			PrintDeathMessage(death_msg, self);
			IRC_printf(IRC_T_KILL, death_msg);
			AddKilledPlayer(attacker, self);

			if (friendlyFire) {
				if (!teamplay->value || team_round_going || !ff_afterround->value)
				{
					self->enemy = attacker; //tkok
					Add_TeamKill(attacker);
					Subtract_Frag(attacker);	//attacker->client->resp.score--;
					self->client->resp.deaths++;
				}
			} else {
				if (!teamplay->value || mod != MOD_TELEFRAG) {
					Add_Frag(attacker, mod);
					attacker->client->pers.num_kills++;
					self->client->resp.streakKills = 0;
					self->client->resp.deaths++;
				}
			}

			return;
		}	// if(message)
	}

	sprintf(death_msg, "%s died\n", self->client->pers.netname);
	PrintDeathMessage(death_msg, self);
	IRC_printf(IRC_T_DEATH, death_msg);

	Subtract_Frag(self);	//self->client->resp.score--;
	self->client->resp.deaths++;
}

void Touch_Item(edict_t * ent, edict_t * other, cplane_t * plane, csurface_t * surf);

// zucc used to toss an item on death
void EjectItem(edict_t * ent, gitem_t * item)
{
	edict_t *drop;
	float spread;

	if (item) {
		spread = 300.0 * crandom();
		ent->client->v_angle[YAW] -= spread;
		drop = Drop_Item(ent, item);
		ent->client->v_angle[YAW] += spread;
		drop->spawnflags = DROPPED_PLAYER_ITEM;
	}

}

// unique weapons need to be specially treated so they respawn properly
void EjectWeapon(edict_t * ent, gitem_t * item)
{
	edict_t *drop;
	float spread;

	if (item) {
		spread = 300.0 * crandom();
		ent->client->v_angle[YAW] -= spread;
		drop = Drop_Item(ent, item);
		ent->client->v_angle[YAW] += spread;
		drop->spawnflags = DROPPED_PLAYER_ITEM;
		if (!in_warmup)
			drop->think = temp_think_specweap;
	}

}

//zucc toss items on death
void TossItemsOnDeath(edict_t * ent)
{
	gitem_t *item;
	qboolean quad;
	int i;

	// don't bother dropping stuff when allweapons/items is active
	if (allitem->value) {
		// remove the lasersight because then the observer might have it
		item = GET_ITEM(LASER_NUM);
		ent->client->pers.inventory[ITEM_INDEX(item)] = 0;
		if(allweapon->value)
			return;

		SP_LaserSight(ent, item);
	} else {
		DeadDropSpec(ent);
		if (allweapon->value)// don't drop weapons if allweapons is on
			return;
	}

	if (WPF_ALLOWED(MK23_NUM) && WPF_ALLOWED(DUAL_NUM)) {
		// give the player a dual pistol so they can be sure to drop one
		item = GET_ITEM(DUAL_NUM);
		ent->client->pers.inventory[ITEM_INDEX(item)]++;
		EjectItem(ent, item);
	}

	// check for every item we want to drop when a player dies
	for (i = MP5_NUM; i < DUAL_NUM; i++) {
		item = GET_ITEM( i );
		while (ent->client->pers.inventory[ITEM_INDEX( item )] > 0) {
			ent->client->pers.inventory[ITEM_INDEX( item )]--;
			EjectWeapon( ent, item );
		}
	}

	item = GET_ITEM(KNIFE_NUM);
	if (ent->client->pers.inventory[ITEM_INDEX(item)] > 0) {
		EjectItem(ent, item);
	}
// special items

	if (!DMFLAGS(DF_QUAD_DROP))
		quad = false;
	else
		quad = (ent->client->quad_framenum > (level.framenum + HZ));

	if (quad) {
		edict_t *drop;
		float spread;

		spread = 300.0 * crandom();
		ent->client->v_angle[YAW] += spread;
		drop = Drop_Item(ent, FindItemByClassname("item_quad"));
		ent->client->v_angle[YAW] -= spread;
		drop->spawnflags |= DROPPED_PLAYER_ITEM;

		drop->touch = Touch_Item;
		drop->nextthink = ent->client->quad_framenum;
		drop->think = G_FreeEdict;
	}
}

void TossClientWeapon(edict_t * self)
{
	gitem_t *item;
	edict_t *drop;
	qboolean quad;
	float spread;

	item = self->client->pers.weapon;
	if (!self->client->pers.inventory[self->client->ammo_index])
		item = NULL;
	if (item && (strcmp(item->pickup_name, "Blaster") == 0))
		item = NULL;

	if (!DMFLAGS(DF_QUAD_DROP))
		quad = false;
	else
		quad = (self->client->quad_framenum > (level.framenum + HZ));

	if (item && quad)
		spread = 22.5;
	else
		spread = 0.0;

	if (item) {
		self->client->v_angle[YAW] -= spread;
		drop = Drop_Item(self, item);
		self->client->v_angle[YAW] += spread;
		drop->spawnflags = DROPPED_PLAYER_ITEM;
	}

	if (quad) {
		self->client->v_angle[YAW] += spread;
		drop = Drop_Item(self, FindItemByClassname("item_quad"));
		self->client->v_angle[YAW] -= spread;
		drop->spawnflags |= DROPPED_PLAYER_ITEM;

		drop->touch = Touch_Item;
		drop->nextthink = self->client->quad_framenum;
		drop->think = G_FreeEdict;
	}
}

/*
==================
LookAtKiller
==================
*/
void LookAtKiller(edict_t * self, edict_t * inflictor, edict_t * attacker)
{
	vec3_t dir;

	if (attacker && attacker != world && attacker != self) {
		VectorSubtract(attacker->s.origin, self->s.origin, dir);
	} else if (inflictor && inflictor != world && inflictor != self) {
		VectorSubtract(inflictor->s.origin, self->s.origin, dir);
	} else {
		self->client->killer_yaw = self->s.angles[YAW];
		return;
	}

// NEW FORMULA FOR THIS FROM 3.20  -FB
	if (dir[0])
		self->client->killer_yaw = 180 / M_PI * atan2(dir[1], dir[0]);
	else {
		self->client->killer_yaw = 0;
		if (dir[1] > 0)
			self->client->killer_yaw = 90;
		else if (dir[1] < 0)
			self->client->killer_yaw = -90;
	}
	if (self->client->killer_yaw < 0)
		self->client->killer_yaw += 360;
// ^^^
}

/*
==================
player_die
==================
*/
void player_die(edict_t * self, edict_t * inflictor, edict_t * attacker, int damage, vec3_t point)
{
	int n, mod;

	VectorClear(self->avelocity);

	self->takedamage = DAMAGE_YES;
	self->movetype = MOVETYPE_TOSS;

	//FIREBLADE
	if (self->solid == SOLID_TRIGGER) {
		self->solid = SOLID_BBOX;
		gi.linkentity(self);
		RemoveFromTransparentList(self);
	}
	//FIREBLADE

	// zucc solves problem of people stopping doors while in their dead bodies
	// 
	// ...only need it in DM though...
	// ...for teamplay, non-solid will get set soon after in CopyToBodyQue
	if (!teamplay->value || ctf->value || teamdm->value) {
		self->solid = SOLID_NOT;
		gi.linkentity(self);
	}

	self->s.modelindex2 = 0;	// remove linked weapon model
	self->s.modelindex3 = 0;	// remove linked ctf flag

	self->s.angles[0] = 0;
	self->s.angles[2] = 0;

	self->s.sound = 0;
	self->client->weapon_sound = 0;

	self->client->reload_attempts = 0;	// stop them from trying to reload
	self->client->weapon_attempts = 0;

	//TempFile
	self->client->desired_zoom = 0;
	self->client->autoreloading = false;
	//TempFile

	self->maxs[2] = -8;

	self->svflags |= SVF_DEADMONSTER;
	if (!self->deadflag) {
		if (ctf->value) {
			self->client->respawn_time = level.framenum + CTFGetRespawnTime(self) * HZ;
		}
		else if(teamdm->value) {
			self->client->respawn_time = level.framenum + (int)(teamdm_respawn->value * HZ);
		}
		else {
			self->client->respawn_time = level.framenum + 1 * HZ;
		}
		LookAtKiller(self, inflictor, attacker);
		self->client->ps.pmove.pm_type = PM_DEAD;
		ClientObituary(self, inflictor, attacker);
		if (ctf->value)
			CTFFragBonuses(self, inflictor, attacker);

		//TossClientWeapon (self);
		TossItemsOnDeath(self);

		if (ctf->value)
			CTFDeadDropFlag(self);

		// let's be safe, if the player was killed and grapple disabled before it
		CTFPlayerResetGrapple(self);

		//FIREBLADE
		if (!teamplay->value)
			Cmd_Help_f(self);	// show scores

		// always reset chase to killer, even if NULL
		if(limchasecam->value < 2 && attacker && attacker->client)
			self->client->resp.last_chase_target = attacker;
	}
	// remove powerups
	self->client->quad_framenum = 0;
	self->client->invincible_framenum = 0;
	self->client->breather_framenum = 0;
	self->client->enviro_framenum = 0;

	//zucc remove lasersight
	if (self->lasersight) {
		G_FreeEdict(self->lasersight);
		self->lasersight = NULL;
	}

	// TNG Turn Flashlight off
	if (self->flashlight)
		FL_make(self);

	//FIREBLADE
	// clean up sniper rifle stuff
	self->client->no_sniper_display = 0;
	self->client->resp.sniper_mode = SNIPER_1X;
	self->client->desired_fov = 90;
	self->client->ps.fov = 90;
	//FIREBLADE
	//SLIC2: Removed this from here
	//self->client->resp.streak = 0;
	//SLIC2
	Bandage(self);		// clear up the leg damage when dead sound?
	self->client->bandage_stopped = 0;

	// clear inventory
	memset(self->client->pers.inventory, 0, sizeof(self->client->pers.inventory));

	// zucc - check if they have a primed grenade
	if (self->client->curr_weap == GRENADE_NUM
	    && ((self->client->ps.gunframe >= GRENADE_IDLE_FIRST && self->client->ps.gunframe <= GRENADE_IDLE_LAST)
		|| (self->client->ps.gunframe >= GRENADE_THROW_FIRST
		    && self->client->ps.gunframe <= GRENADE_THROW_LAST))) {
		self->client->ps.gunframe = 0;
		// Reset Grenade Damage to 1.52 when requested:
		if (use_classic->value)
			fire_grenade2(self, self->s.origin, vec3_origin, 170, 0, 2 * HZ, 170 * 2, false);
		else
			fire_grenade2(self, self->s.origin, vec3_origin, GRENADE_DAMRAD, 0,
				      2 * HZ, GRENADE_DAMRAD * 2, false);
	}

	mod = meansOfDeath & ~MOD_FRIENDLY_FIRE;
	// Gibbing on really hard HC hit
	if ((((self->health < -35) && (mod == MOD_HC)) ||
		((self->health < -20) && (mod == MOD_M3))) && (sv_gib->value)) {
		gi.sound(self, CHAN_BODY, gi.soundindex("misc/udeath.wav"), 1, ATTN_NORM, 0);
		for (n = 0; n < 5; n++)
			ThrowGib(self, "models/objects/gibs/sm_meat/tris.md2", damage, GIB_ORGANIC);
		ThrowClientHead(self, damage);
		self->client->anim_priority = ANIM_DEATH;
		self->client->anim_end = 0;
		self->takedamage = DAMAGE_NO;
	} else {		// normal death
		if (!self->deadflag) {
			static int i;

			i = (i + 1) % 3;
			// start a death animation
			self->client->anim_priority = ANIM_DEATH;
			if (self->client->ps.pmove.pm_flags & PMF_DUCKED) {
				self->s.frame = FRAME_crdeath1 - 1;
				self->client->anim_end = FRAME_crdeath5;
			} else
				switch (i) {
				case 0:
					self->s.frame = FRAME_death101 - 1;
					self->client->anim_end = FRAME_death106;
					break;
				case 1:
					self->s.frame = FRAME_death201 - 1;
					self->client->anim_end = FRAME_death206;
					break;
				case 2:
					self->s.frame = FRAME_death301 - 1;
					self->client->anim_end = FRAME_death308;
					break;
				}
			if ((mod == MOD_SNIPER) || (mod == MOD_KNIFE)
				|| (mod == MOD_KNIFE_THROWN)) {
				gi.sound(self, CHAN_VOICE, gi.soundindex("misc/glurp.wav"), 1, ATTN_NORM, 0);
				// TempFile - BEGIN sniper gibbing
				if (mod == MOD_SNIPER) {
					int n;

					switch (locOfDeath) {
					case LOC_HDAM:
						if (sv_gib->value) {
							for (n = 0; n < 8; n++)
								ThrowGib(self,
									 "models/objects/gibs/sm_meat/tris.md2",
									 damage, GIB_ORGANIC);
							ThrowClientHead(self, damage);
						}
					}
				}
			} else
				gi.sound(self, CHAN_VOICE,
					 gi.soundindex(va("*death%i.wav", (rand() % 4) + 1)), 1, ATTN_NORM, 0);
		}
	}
	// zucc this will fix a jump kick death generating a weapon
	self->client->curr_weap = MK23_NUM;
	//PG BUND - BEGIN
	self->client->resp.idletime = 0;
	//    self->client->resp.last_killed_target = NULL;
	ResetKills(self);
	//PG BUND - END        
	//AQ2:TNG Slicer Last Damage Location
	self->client->resp.last_damaged_part = 0;
	self->client->resp.last_damaged_players[0] = '\0';
	//AQ2:TNG END

	//TempFile
	self->client->pers.num_kills = 0;
	//TempFile
	self->deadflag = DEAD_DEAD;
	gi.linkentity(self);

	// in ctf, when a player dies check if he should be moved to the other team
	if(ctf->value)
		CheckForUnevenTeams(self);
}

//=======================================================================

/*
==============
InitClientPersistant

This is only called when the game first initializes in single player,
but is called after each death and level change in deathmatch
==============
*/
// SLIC2 If pers structure gets memset to 0 lets cleannup unecessary initiations ( to 0 ) here
void InitClientPersistant(gclient_t * client)
{
	gitem_t *item;

/*		client_persistant_t	oldpers;

  //FB 6/3/99
		memcpy(oldpers, pers, sizeof(client->pers));
  //FB 6/3/99

*/
	memset(&client->pers, 0, sizeof(client->pers));
	// changed to mk23
	item = GET_ITEM(MK23_NUM);
	client->pers.selected_item = ITEM_INDEX(item);
	client->pers.inventory[client->pers.selected_item] = 1;

	client->pers.weapon = item;
	client->pers.lastweapon = item;

	if (WPF_ALLOWED(KNIFE_NUM)) {
		item = GET_ITEM(KNIFE_NUM);
		client->pers.inventory[ITEM_INDEX(item)] = 1;
		if (!WPF_ALLOWED(MK23_NUM)) {
			client->pers.selected_item = ITEM_INDEX(item);
			client->pers.weapon = item;
			client->pers.lastweapon = item;
		}
	}

	client->pers.health = 100;
	client->pers.max_health = 100;
	//zucc changed maximum ammo amounts
	client->pers.max_bullets = 2;
	client->pers.max_shells = 14;
	client->pers.max_rockets = 2;
	client->pers.max_grenades = 50;
	client->pers.max_cells = 1;
	client->pers.max_slugs = 20;
	client->knife_max = 10;
	client->grenade_max = 2;

	client->pers.connected = true;
	//zucc
	client->fired = 0;
	client->burst = 0;
	client->fast_reload = 0;
	client->machinegun_shots = 0;
	client->unique_weapon_total = 0;
	client->unique_item_total = 0;
	if (WPF_ALLOWED(MK23_NUM)) {
		client->curr_weap = MK23_NUM;
	} else if (WPF_ALLOWED(KNIFE_NUM)) {
		client->curr_weap = KNIFE_NUM;
	} else {
		client->curr_weap = MK23_NUM;
	}
	//AQ2:TNG - Slicer Moved This To Here
	//client->pers.num_kills = 0;
	//AQ2:TNG END
	client->uvTime = 0;
}
// SLIC2 If resp structure gets memset to 0 lets cleannup unecessary initiations ( to 0 ) here
void InitClientResp(gclient_t * client)
{
	int team = client->resp.team;
	gitem_t *item = client->resp.item;
	gitem_t *weapon = client->resp.weapon;
	qboolean menu_shown = client->resp.menu_shown;
	qboolean dm_selected = client->resp.dm_selected;

	memset(&client->resp, 0, sizeof(client->resp));
	client->resp.team = team;
	client->resp.enterframe = level.framenum;

	if (!dm_choose->value && !warmup->value) {
		if (WPF_ALLOWED(MP5_NUM)) {
			client->resp.weapon = GET_ITEM(MP5_NUM);
		} else if (WPF_ALLOWED(MK23_NUM)) {
			client->resp.weapon = GET_ITEM(MK23_NUM);
		} else if (WPF_ALLOWED(KNIFE_NUM)) {
			client->resp.weapon = GET_ITEM(KNIFE_NUM);
		} else {
			client->resp.weapon = GET_ITEM(MK23_NUM);
		}
		client->resp.item = GET_ITEM(KEV_NUM);
	} else {
		if (wp_flags->value < 2)
			client->resp.weapon = GET_ITEM(MK23_NUM);
	}
	client->resp.ir = 1;

	if (!teamplay->value && auto_equip->value) {
		client->resp.menu_shown = menu_shown;
	}

	client->resp.dm_selected = dm_selected;

	// TNG:Freud, restore weapons and items from last map.
	if (auto_equip->value && ((teamplay->value && !teamdm->value) || dm_choose->value) && ctf->value != 2) {
		if (item)
			client->resp.item = item;
		if (weapon)
			client->resp.weapon = weapon;
	}

	client->resp.team = NOTEAM;

	// TNG:Freud, restore team from previous map
	if (auto_join->value && team)
		client->resp.saved_team = team;

	//TempFile - BEGIN
	client->resp.punch_desired = false;
	//client->resp.fire_time = 0;
	//client->resp.ignore_time = 0;

	//client->pers.num_kills = 0; AQ2:TNG Moved This to InitClientPersistant
	//TempFile - END

	//PG BUND - BEGIN
//  client->resp.last_killed_target = NULL;
//  ResetKills(ent);
	//client->resp.idletime = 0;
	//PG BUND - END
	//AQ2:TNG - Slicer : Reset Video Cheking vars
	//memset(client->resp.vidref, 0, sizeof(client->resp.vidref));
	//memset(client->resp.gldriver, 0, sizeof(client->resp.gldriver));
	//client->resp.gllockpvs = 0;
	//client->resp.glclear = 0;
	client->resp.gldynamic = 1;
	//client->resp.glmodulate = 0;
	client->resp.checked = false;
	//memset(&client->resp.checktime, 0, sizeof(client->resp.checktime));
	//memset(&client->resp.rd_when, 0, sizeof(client->resp.rd_when));
	//memset(client->resp.rd_rep, 0, sizeof(client->resp.rd_rep));
	//client->resp.rd_mute = 0;
	//client->resp.rd_repcount = 0;
	//client->resp.rd_reptime = 0;
	//AQ2:TNG END
	//AQ2:TNG Slicer Last Damage Location
	//client->resp.last_damaged_part = 0;
	//client->resp.last_damaged_players[0] = '\0';
	//AQ2:TNG END
	//AQ2:TNG Slicer Moved this to here
	//client->resp.killed_teammates = 0;
	//client->resp.idletime = 0;
	//AQ2:TNG END
	//AQ2:TNG Slicer - Matchmode
	//client->resp.subteam = 0;
	//client->resp.captain = 0;
	//client->resp.admin = 0;
	//client->resp.stat_mode_intermission = 0;
	//AQ2:TNG END

	// No automatic team join
	//  if (ctf->value && client->resp.team < TEAM1)
	//  CTFAssignTeam(client);
}

/*
==================
SaveClientData

Some information that should be persistant, like health, 
is still stored in the edict structure, so it needs to
be mirrored out to the client structure before all the
edicts are wiped.
==================
*/
void SaveClientData(void)
{
	int i;
	edict_t *ent;

	for (i = 0; i < game.maxclients; i++) {
		ent = &g_edicts[1 + i];
		if (!ent->inuse)
			continue;
		game.clients[i].pers.health = ent->health;
		game.clients[i].pers.max_health = ent->max_health;
		game.clients[i].pers.powerArmorActive = (ent->flags & FL_POWER_ARMOR);
	}
}

void FetchClientEntData(edict_t * ent)
{
	ent->health = ent->client->pers.health;
	ent->max_health = ent->client->pers.max_health;
	if (ent->client->pers.powerArmorActive)
		ent->flags |= FL_POWER_ARMOR;
}

/*
=======================================================================

  SelectSpawnPoint

=======================================================================
*/

/*
================
PlayersRangeFromSpot

Returns the distance to the nearest player from the given spot
================
*/
float PlayersRangeFromSpot(edict_t * spot)
{
	edict_t *player;
	float playerdistance, bestplayerdistance = 9999999;
	int n;


	for (n = 1; n <= game.maxclients; n++) {
		player = &g_edicts[n];

		if (!player->inuse)
			continue;

		if (player->health <= 0)
			continue;
	
		playerdistance = Distance(spot->s.origin, player->s.origin);
		if (playerdistance < bestplayerdistance)
			bestplayerdistance = playerdistance;
	}

	return bestplayerdistance;
}

/*
================
SelectRandomDeathmatchSpawnPoint

go to a random point, but NOT the two points closest
to other players
================
*/
edict_t *SelectRandomDeathmatchSpawnPoint(void)
{
	edict_t *spot, *spot1, *spot2;
	int count = 0;
	int selection;
	float range, range1, range2;

	spot = NULL;
	range1 = range2 = 99999;
	spot1 = spot2 = NULL;

	while ((spot = G_Find(spot, FOFS(classname), "info_player_deathmatch")) != NULL) {
		count++;
		range = PlayersRangeFromSpot(spot);
		if (range < range1) {
			if (range1 < range2) {
				range2 = range1;
				spot2 = spot1;
			}
			range1 = range;
			spot1 = spot;
		} else if (range < range2) {
			range2 = range;
			spot2 = spot;
		}
	}

	if (!count)
		return NULL;

	if (count <= 2) {
		spot1 = spot2 = NULL;
	} else
		count -= 2;

	selection = rand() % count;

	spot = NULL;
	do {
		spot = G_Find(spot, FOFS(classname), "info_player_deathmatch");
		if (spot == spot1 || spot == spot2)
			selection++;
	}
	while (selection--);

	return spot;
}

/*
================
SelectFarthestDeathmatchSpawnPoint

================
*/
edict_t *SelectFarthestDeathmatchSpawnPoint(void)
{
	edict_t *bestspot;
	float bestdistance, bestplayerdistance;
	edict_t *spot;

	spot = NULL;
	bestspot = NULL;
	bestdistance = 0;
	while ((spot = G_Find(spot, FOFS(classname), "info_player_deathmatch")) != NULL) {
		bestplayerdistance = PlayersRangeFromSpot(spot);

		if (bestplayerdistance > bestdistance) {
			bestspot = spot;
			bestdistance = bestplayerdistance;
		}
	}

	if (bestspot) {
		return bestspot;
	}
	// if there is a player just spawned on each and every start spot
	// we have no choice to turn one into a telefrag meltdown
	spot = G_Find(NULL, FOFS(classname), "info_player_deathmatch");

	return spot;
}

edict_t *SelectDeathmatchSpawnPoint(void)
{
	if (DMFLAGS(DF_SPAWN_FARTHEST))
		return SelectFarthestDeathmatchSpawnPoint();
	else
		return SelectRandomDeathmatchSpawnPoint();
}

/*
===========
SelectSpawnPoint

Chooses a player start, deathmatch start, coop start, etc
============
*/
void SelectSpawnPoint(edict_t * ent, vec3_t origin, vec3_t angles)
{
	edict_t *spot = NULL;

	//FIREBLADE
	if (ctf->value)
		spot = SelectCTFSpawnPoint(ent);
	else if (teamplay->value && !teamdm->value && ent->client->resp.team != NOTEAM && !in_warmup) {
		spot = SelectTeamplaySpawnPoint(ent);
	} else {
		spot = SelectDeathmatchSpawnPoint();
	}

	// find a single player start spot
	if (!spot) {
		gi.dprintf("Warning: failed to find deathmatch spawn point\n");

		while ((spot = G_Find(spot, FOFS(classname), "info_player_start")) != NULL) {
			if (!game.spawnpoint[0] && !spot->targetname)
				break;

			if (!game.spawnpoint[0] || !spot->targetname)
				continue;

			if (Q_stricmp(game.spawnpoint, spot->targetname) == 0)
				break;
		}

		if (!spot) {
			if (!game.spawnpoint[0]) {	// there wasn't a spawnpoint without a target, so use any
				spot = G_Find(spot, FOFS(classname), "info_player_start");
			}
			if (!spot) {
				gi.error("Couldn't find spawn point %s\n", game.spawnpoint);
				return;
			}
		}
	}

	VectorCopy(spot->s.origin, origin);
	origin[2] += 9;
	VectorCopy(spot->s.angles, angles);
}

//======================================================================

void InitBodyQue(void)
{
	int i;
	edict_t *ent;

	level.body_que = 0;
	for (i = 0; i < BODY_QUEUE_SIZE; i++) {
		ent = G_Spawn();
		ent->classname = "bodyque";
	}
}

void body_die(edict_t * self, edict_t * inflictor, edict_t * attacker, int damage, vec3_t point)
{
/*      int     n;*/

	if (self->health < -40) {
		// remove gibbing
/*                gi.sound (self, CHAN_BODY, gi.soundindex ("misc/udeath.wav"), 1, ATTN_NORM, 0);
                for (n= 0; n < 4; n++)
                        ThrowGib (self, "models/objects/gibs/sm_meat/tris.md2", damage, GIB_ORGANIC);
                self->s.origin[2] -= 48;
                ThrowClientHead (self, damage);*/
		self->takedamage = DAMAGE_NO;
	}
}

void CopyToBodyQue(edict_t * ent)
{
	edict_t *body;

	// grab a body que and cycle to the next one
	body = &g_edicts[game.maxclients + level.body_que + 1];
	level.body_que = (level.body_que + 1) % BODY_QUEUE_SIZE;

	// FIXME: send an effect on the removed body

	gi.unlinkentity(ent);

	gi.unlinkentity(body);

	body->s = ent->s;
	body->s.number = body - g_edicts;
	body->s.event = EV_OTHER_TELEPORT;
	VectorCopy( body->s.origin, body->s.old_origin );
	VectorCopy( body->s.origin, body->old_origin );

	body->svflags = ent->svflags;
	VectorCopy(ent->mins, body->mins);
	VectorCopy(ent->maxs, body->maxs);
	VectorCopy(ent->absmin, body->absmin);
	VectorCopy(ent->absmax, body->absmax);
	VectorCopy(ent->size, body->size);
	// All our bodies will be non-solid -FB
	body->solid = SOLID_NOT;
	//body->solid = ent->solid;
	body->clipmask = ent->clipmask;
	body->owner = ent->owner;

//FB 5/31/99
	body->movetype = MOVETYPE_TOSS;	// just in case?
//        body->movetype = ent->movetype;
	VectorCopy(ent->velocity, body->velocity);
	body->mass = ent->mass;
	body->groundentity = NULL;
//FB 5/31/99
//FB 6/1/99
	body->s.renderfx = 0;
//FB

	body->die = body_die;
	body->takedamage = DAMAGE_YES;

//PG BUND - BEGIN
	//Disable to be seen by irvision
	body->s.renderfx &= ~RF_IR_VISIBLE;
//PG BUND - END

	gi.linkentity(body);
}

void CleanBodies()
{
	int i;
	edict_t *ent;

	ent = g_edicts + game.maxclients + 1;
	for (i = 0; i < BODY_QUEUE_SIZE; i++, ent++) {
		gi.unlinkentity( ent );
		ent->solid = SOLID_NOT;
		ent->movetype = MOVETYPE_NOCLIP;
		ent->svflags |= SVF_NOCLIENT;
	}
	level.body_que = 0;
}

void respawn(edict_t * self)
{
	if (self->solid != SOLID_NOT || self->deadflag == DEAD_DEAD)
		CopyToBodyQue(self);

	PutClientInServer(self);
	if (ctf->value || teamdm->value)
		AddToTransparentList(self);

//FIREBLADE
	self->svflags &= ~SVF_NOCLIENT;
//FIREBLADE

// Disable all this... -FB
//                // add a teleportation effect
//                self->s.event = EV_PLAYER_TELEPORT;
//
//                // hold in place briefly
//                self->client->ps.pmove.pm_flags = PMF_TIME_TELEPORT;
//                self->client->ps.pmove.pm_time = 14;

	if(respawn_effect->value) {
		gi.WriteByte(svc_muzzleflash);
		gi.WriteShort(self - g_edicts);
		gi.WriteByte(MZ_RESPAWN);
		gi.multicast(self->s.origin, MULTICAST_PVS);
	}

	self->client->respawn_time = level.framenum + 2 * HZ;
}

//==============================================================

void AllWeapons(edict_t * ent)
{
	int i;
	gitem_t *it;

	for (i = 0; i < game.num_items; i++) {
		it = itemlist + i;
		if (!it->pickup)
			continue;
		if (!(it->flags & IT_WEAPON))
			continue;

		if (!it->typeNum || !WPF_ALLOWED(it->typeNum))
			continue;

		switch(it->typeNum) {
		case MK23_NUM:
			ent->client->pers.inventory[i] = 1;
			ent->client->mk23_rds = ent->client->mk23_max;
			break;
		case MP5_NUM:
			ent->client->pers.inventory[i] = 1;
			ent->client->mp5_rds = ent->client->mp5_max;
			break;
		case M4_NUM:
			ent->client->pers.inventory[i] = 1;
			ent->client->m4_rds = ent->client->m4_max;	    
			break;
		case M3_NUM:
			ent->client->pers.inventory[i] = 1;
			ent->client->shot_rds = ent->client->shot_max;
			break;
		case HC_NUM:
			ent->client->pers.inventory[i] = 1;
			ent->client->cannon_rds = ent->client->cannon_max;
			ent->client->shot_rds = ent->client->shot_max;
			break;
		case SNIPER_NUM:
			ent->client->pers.inventory[i] = 1;
			ent->client->sniper_rds = ent->client->sniper_max;
			break;
		case DUAL_NUM:
			ent->client->pers.inventory[i] = 1;
			ent->client->dual_rds = ent->client->dual_max;
			break;
		case KNIFE_NUM:
			ent->client->pers.inventory[i] = 10;
			break;
		case GRENADE_NUM:
			ent->client->pers.inventory[i] = tgren->value;
			break;
		}
	}
	
	for (i = 0; i < game.num_items; i++) {
		it = itemlist + i;
		if (!it->pickup)
			continue;
		if (!(it->flags & IT_AMMO))
			continue;
		Add_Ammo(ent, it, 1000);
	}
}

void AllItems(edict_t * ent)
{
	edict_t etemp;
	int i;
	gitem_t *it;

	for (i = 0; i < game.num_items; i++) {
		it = itemlist + i;
		if (!it->pickup)
			continue;
		if (!(it->flags & IT_ITEM))
			continue;

		etemp.item = it;

		if (ent->client->unique_item_total >= unique_items->value)
			ent->client->unique_item_total = unique_items->value - 1;
		Pickup_Special(&etemp, ent);
	}

}

// equips a client with item/weapon in teamplay

void EquipClient(edict_t * ent)
{
	gclient_t *client;
	gitem_t *item;
	edict_t etemp;
	int band = 0;

	client = ent->client;

	if (!(client->resp.item) || !(client->resp.weapon))
		return;

	if(use_grapple->value)
		client->pers.inventory[ITEM_INDEX(FindItem("Grapple"))] = 1;

	if (client->resp.item->typeNum == BAND_NUM) {
		band = 1;
		if (tgren->value > 0)	// team grenades is turned on
		{
			item = GET_ITEM(GRENADE_NUM);
			client->pers.inventory[ITEM_INDEX(item)] = tgren->value;
		}

	}
	// set them up with initial pistol ammo
	if (WPF_ALLOWED(MK23_ANUM)) {
		item = GET_ITEM(MK23_ANUM);
		if (band)
			client->pers.inventory[ITEM_INDEX(item)] = 2;
		else
			client->pers.inventory[ITEM_INDEX(item)] = 1;
	}

	if (client->resp.weapon->typeNum == MP5_NUM) {
		item = GET_ITEM(MP5_NUM);
		client->pers.selected_item = ITEM_INDEX(item);
		client->pers.inventory[client->pers.selected_item] = 1;
		client->pers.weapon = item;
		client->curr_weap = MP5_NUM;
		client->unique_weapon_total = 1;
		item = GET_ITEM(MP5_ANUM);
		if (band)
			client->pers.inventory[ITEM_INDEX(item)] = 2;
		else
			client->pers.inventory[ITEM_INDEX(item)] = 1;
		client->mp5_rds = client->mp5_max;
	} else if (client->resp.weapon->typeNum == M4_NUM) {
		item = GET_ITEM(M4_NUM);
		client->pers.selected_item = ITEM_INDEX(item);
		client->pers.inventory[client->pers.selected_item] = 1;
		client->pers.weapon = item;
		client->curr_weap = M4_NUM;
		client->unique_weapon_total = 1;
		item = GET_ITEM(M4_ANUM);
		if (band)
			client->pers.inventory[ITEM_INDEX(item)] = 2;
		else
			client->pers.inventory[ITEM_INDEX(item)] = 1;
		client->m4_rds = client->m4_max;
	} else if (client->resp.weapon->typeNum == M3_NUM) {
		item = GET_ITEM(M3_NUM);
		client->pers.selected_item = ITEM_INDEX(item);
		client->pers.inventory[client->pers.selected_item] = 1;
		client->pers.weapon = item;
		client->curr_weap = M3_NUM;
		client->unique_weapon_total = 1;
		item = GET_ITEM(SHELL_ANUM);
		if (band)
			client->pers.inventory[ITEM_INDEX(item)] = 14;
		else
			client->pers.inventory[ITEM_INDEX(item)] = 7;
		client->shot_rds = client->shot_max;
	} else if (client->resp.weapon->typeNum == HC_NUM) {
		item = GET_ITEM(HC_NUM);
		client->pers.selected_item = ITEM_INDEX(item);
		client->pers.inventory[client->pers.selected_item] = 1;
		client->pers.weapon = item;
		client->curr_weap = HC_NUM;
		client->unique_weapon_total = 1;
		item = GET_ITEM(SHELL_ANUM);
		if (band)
			client->pers.inventory[ITEM_INDEX(item)] = 24;
		else
			client->pers.inventory[ITEM_INDEX(item)] = 12;
		client->cannon_rds = client->cannon_max;
	} else if (client->resp.weapon->typeNum == SNIPER_NUM) {
		item = GET_ITEM(SNIPER_NUM);
		client->pers.selected_item = ITEM_INDEX(item);
		client->pers.inventory[ITEM_INDEX(item)] = 1;
		client->pers.weapon = item;
		client->curr_weap = SNIPER_NUM;
		client->unique_weapon_total = 1;
		item = GET_ITEM(SNIPER_ANUM);
		if (band)
			client->pers.inventory[ITEM_INDEX(item)] = 20;
		else
			client->pers.inventory[ITEM_INDEX(item)] = 10;
		client->sniper_rds = client->sniper_max;
	} else if (client->resp.weapon->typeNum == DUAL_NUM) {
		item = GET_ITEM(DUAL_NUM);
		client->pers.selected_item = ITEM_INDEX(item);
		client->pers.inventory[client->pers.selected_item] = 1;
		client->pers.weapon = item;
		client->curr_weap = DUAL_NUM;
		item = GET_ITEM(MK23_ANUM);
		if (band)
			client->pers.inventory[ITEM_INDEX(item)] = 4;
		else
			client->pers.inventory[ITEM_INDEX(item)] = 2;
		client->dual_rds = client->dual_max;
	} else if (client->resp.weapon->typeNum == KNIFE_NUM) {
		item = GET_ITEM(KNIFE_NUM);
		client->pers.selected_item = ITEM_INDEX(item);
		if (band)
			client->pers.inventory[client->pers.selected_item] = 20;
		else
			client->pers.inventory[client->pers.selected_item] = 10;
		client->pers.weapon = item;
		client->curr_weap = KNIFE_NUM;
	}

	etemp.item = client->resp.item;
	Pickup_Special(&etemp, ent);
}

// Igor[Rock] start
void EquipClientDM(edict_t * ent)
{
	gclient_t *client;
	gitem_t *item;

	client = ent->client;

	if(use_grapple->value)
		client->pers.inventory[ITEM_INDEX(FindItem("Grapple"))] = 1;

	if (!Q_stricmp(strtwpn->string, MK23_NAME))
		return;

	// Give some ammo for the weapon
	if (Q_stricmp(strtwpn->string, MP5_NAME) == 0) {
		item = GET_ITEM(MP5_NUM);
		client->pers.selected_item = ITEM_INDEX(item);
		client->pers.inventory[client->pers.selected_item] = 1;
		client->pers.weapon = item;
		client->mp5_rds = client->mp5_max;
		client->curr_weap = MP5_NUM;
		if (!allweapon->value) {
			client->unique_weapon_total = 1;
		}
		item = GET_ITEM(MP5_ANUM);
		client->pers.inventory[ITEM_INDEX(item)] = 1;
	} else if (Q_stricmp(strtwpn->string, M4_NAME) == 0) {
		item = GET_ITEM(M4_NUM);
		client->pers.selected_item = ITEM_INDEX(item);
		client->pers.inventory[client->pers.selected_item] = 1;
		client->pers.weapon = item;
		client->m4_rds = client->m4_max;
		client->curr_weap = M4_NUM;
		if (!allweapon->value) {
			client->unique_weapon_total = 1;
		}
		item = GET_ITEM(M4_ANUM);
		client->pers.inventory[ITEM_INDEX(item)] = 1;
	} else if (Q_stricmp(strtwpn->string, M3_NAME) == 0) {
		item = GET_ITEM(M3_NUM);
		client->pers.selected_item = ITEM_INDEX(item);
		client->pers.inventory[client->pers.selected_item] = 1;
		client->pers.weapon = item;
		client->shot_rds = client->shot_max;
		client->curr_weap = M3_NUM;
		if (!allweapon->value) {
			client->unique_weapon_total = 1;
		}
		item = GET_ITEM(SHELL_ANUM);
		client->pers.inventory[ITEM_INDEX(item)] = 7;
	} else if (Q_stricmp(strtwpn->string, HC_NAME) == 0) {
		item = GET_ITEM(HC_NUM);
		client->pers.selected_item = ITEM_INDEX(item);
		client->pers.inventory[client->pers.selected_item] = 1;
		client->pers.weapon = item;
		client->cannon_rds = client->cannon_max;
		client->shot_rds = client->shot_max;
		client->curr_weap = HC_NUM;
		if (!allweapon->value) {
			client->unique_weapon_total = 1;
		}
		item = GET_ITEM(SHELL_ANUM);
		client->pers.inventory[ITEM_INDEX(item)] = 12;
	} else if (Q_stricmp(strtwpn->string, SNIPER_NAME) == 0) {
		item = GET_ITEM(SNIPER_NUM);
		client->pers.selected_item = ITEM_INDEX(item);
		client->pers.inventory[client->pers.selected_item] = 1;
		client->pers.weapon = item;
		client->sniper_rds = client->sniper_max;
		client->curr_weap = SNIPER_NUM;
		if (!allweapon->value) {
			client->unique_weapon_total = 1;
		}
		item = GET_ITEM(SNIPER_ANUM);;
		client->pers.inventory[ITEM_INDEX(item)] = 10;
	} else if (Q_stricmp(strtwpn->string, DUAL_NAME) == 0) {
		item = GET_ITEM(DUAL_NUM);
		client->pers.selected_item = ITEM_INDEX(item);
		client->pers.inventory[client->pers.selected_item] = 1;
		client->pers.weapon = item;
		client->dual_rds = client->dual_max;
		client->mk23_rds = client->mk23_max;
		client->curr_weap = DUAL_NUM;
		item = GET_ITEM(MK23_ANUM);
		client->pers.inventory[ITEM_INDEX(item)] = 2;
	} else if (Q_stricmp(strtwpn->string, GRENADE_NAME) == 0) {
		item = GET_ITEM(GRENADE_NUM);
		client->pers.selected_item = ITEM_INDEX(item);
		client->pers.inventory[client->pers.selected_item] = tgren->value;
		client->pers.weapon = item;
		client->curr_weap = GRENADE_NUM;
	} else if (Q_stricmp(strtwpn->string, KNIFE_NAME) == 0) {
		item = GET_ITEM(KNIFE_NUM);
		client->pers.selected_item = ITEM_INDEX(item);
		client->pers.inventory[client->pers.selected_item] = 10;
		client->pers.weapon = item;
		client->curr_weap = KNIFE_NUM;
	}
}

// Igor[Rock] ende

/*
===========
PutClientInServer

Called when a player connects to a server or respawns in
a deathmatch.
============
*/

void PutClientInServer(edict_t * ent)
{
	vec3_t mins = { -16, -16, -24 };
	vec3_t maxs = { 16, 16, 32 };
	int index;
	vec3_t spawn_origin, spawn_angles;
	gclient_t *client;
	int going_observer;
	int i;
	client_persistant_t saved;
	client_respawn_t resp;
	int save_team_wounds;
	int save_team_kills;
	char save_ipaddr[100];
	char userinfo[MAX_INFO_STRING];

//FF

	// find a spawn point
	// do it before setting health back up, so farthest
	// ranging doesn't count this client
	SelectSpawnPoint(ent, spawn_origin, spawn_angles);

	index = ent - g_edicts - 1;
	client = ent->client;

	// deathmatch wipes most client data every spawn
	resp = client->resp;
	memcpy(userinfo, client->pers.userinfo, sizeof(userinfo));
	InitClientPersistant(client);
	ClientUserinfoChanged(ent, userinfo);

	// clear everything but the persistant data
	saved = client->pers;
//FF 
	save_team_wounds = client->team_wounds;
	save_team_kills = client->team_kills;

	if (client->ipaddr)
		strcpy(save_ipaddr, client->ipaddr);
//FF
	memset(client, 0, sizeof(*client));

	client->pers = saved;
	client->clientNum = ent - g_edicts - 1;
//FF
	client->team_wounds = save_team_wounds;
	client->team_kills = save_team_kills;

	if (save_ipaddr && client->ipaddr)
		strcpy(client->ipaddr, save_ipaddr);
//FF
	if (client->pers.health <= 0)
		InitClientPersistant(client);
	client->resp = resp;

	// copy some data from the client to the entity
	FetchClientEntData(ent);

	// clear entity values
	ent->groundentity = NULL;
	ent->client = &game.clients[index];
	ent->takedamage = DAMAGE_AIM;
	ent->movetype = MOVETYPE_WALK;
	ent->viewheight = 22;
	ent->inuse = true;
	ent->classname = "player";
	ent->mass = 200;
	ent->solid = SOLID_BBOX;
	ent->deadflag = DEAD_NO;
	ent->air_finished_framenum = level.framenum + 12 * HZ;
	ent->clipmask = MASK_PLAYERSOLID;
	ent->model = "players/male/tris.md2";
	ent->pain = player_pain;
	ent->die = player_die;
	ent->waterlevel = 0;
	ent->watertype = 0;
	ent->flags &= ~FL_NO_KNOCKBACK;
	ent->svflags &= ~SVF_DEADMONSTER;

//FIREBLADE
	if (!teamplay->value || ent->client->resp.team != NOTEAM) {
		ent->flags &= ~FL_GODMODE;
		ent->svflags &= ~SVF_NOCLIENT;
	}
//FIREBLADE

	VectorCopy(mins, ent->mins);
	VectorCopy(maxs, ent->maxs);
	VectorClear(ent->velocity);

	// clear playerstate values
	memset(&client->ps, 0, sizeof(client->ps));

	client->ps.pmove.origin[0] = spawn_origin[0] * 8;
	client->ps.pmove.origin[1] = spawn_origin[1] * 8;
	client->ps.pmove.origin[2] = spawn_origin[2] * 8;

	if (ctf->value) {
		//AQ2:TNG Igor not quite sure about this (FIXME)
		client->ps.pmove.pm_flags &= ~PMF_NO_PREDICTION;
		//AQ2:TNG End
	}
	if (DMFLAGS(DF_FIXED_FOV)) {
		client->ps.fov = 90;
	} else {
		client->ps.fov = atoi(Info_ValueForKey(client->pers.userinfo, "fov"));
		if (client->ps.fov < 1)
			client->ps.fov = 90;
		else if (client->ps.fov > 160)
			client->ps.fov = 160;
	}

	client->ps.gunindex = gi.modelindex(client->pers.weapon->view_model);

	// clear entity state values
	ent->s.effects = 0;
	ent->s.skinnum = ent - g_edicts - 1;
	ent->s.modelindex = 255;	// will use the skin specified model

	// zucc vwep
	//ent->s.modelindex2 = 255;             // custom gun model
	ShowGun(ent);

	ent->s.frame = 0;
	VectorCopy(spawn_origin, ent->s.origin);
	ent->s.origin[2] += 1;	// make sure off ground
	VectorCopy(ent->s.origin, ent->s.old_origin);
	VectorCopy(ent->s.origin, ent->old_origin);

	// set the delta angle
	for (i = 0; i < 3; i++)
		client->ps.pmove.delta_angles[i] = ANGLE2SHORT(spawn_angles[i] - client->resp.cmd_angles[i]);

	ent->s.angles[PITCH] = 0;
	ent->s.angles[YAW] = spawn_angles[YAW];
	ent->s.angles[ROLL] = 0;
	VectorCopy(ent->s.angles, client->ps.viewangles);
	VectorCopy(ent->s.angles, client->v_angle);

	//FIREBLADE
	if (teamplay->value) {
		going_observer = StartClient(ent);
	} else {
		going_observer = ent->client->pers.spectator;

		if (dm_choose->value && !ent->client->resp.dm_selected)
			going_observer = 1;

		if (going_observer) {
			ent->movetype = MOVETYPE_NOCLIP;
			ent->solid = SOLID_NOT;
			ent->svflags |= SVF_NOCLIENT;
			ent->client->resp.team = NOTEAM;
			ent->client->ps.gunindex = 0;
		}
	}


	if (!going_observer && !teamplay->value) {	// this handles telefrags...
		KillBox(ent);
	}

	gi.linkentity( ent );

	//zucc give some ammo
	//item = FindItem("Pistol Clip");     
	// Add_Ammo(ent,item,1);
	client->mk23_max = 12;
	client->mp5_max = 30;
	client->m4_max = 24;
	client->shot_max = 7;
	client->sniper_max = 6;
	client->cannon_max = 2;
	client->dual_max = 24;
	if (WPF_ALLOWED(MK23_NUM)) {
		client->mk23_rds = client->mk23_max;
		client->dual_rds = client->mk23_max;
	} else {
		client->mk23_rds = 0;
		client->dual_rds = 0;
	}
	client->knife_max = 10;
	client->grenade_max = 2;

	ent->lasersight = NULL;

	//other
	client->resp.sniper_mode = SNIPER_1X;
	client->bandaging = 0;
	client->leg_damage = 0;
	client->leg_noise = 0;
	client->leg_dam_count = 0;
	client->desired_fov = 90;
	client->ps.fov = 90;
	client->idle_weapon = 0;
	client->drop_knife = 0;
	client->no_sniper_display = 0;
	client->knife_sound = 0;
	client->doortoggle = 0;
	client->have_laser = 0;
	client->reload_attempts = 0;
	client->weapon_attempts = 0;
//TempFile
	client->desired_zoom = 0;
	client->autoreloading = false;
//TempFile

//FIREBLADE
	if (!going_observer)
	{
		if ((int)uvtime->value > 0) {
			if (teamplay->value) {
				if ((ctf->value || teamdm->value) && team_round_going && !lights_camera_action && client->resp.team != NOTEAM) {
					client->uvTime = uvtime->value;
				}
			} else if (dm_shield->value) {
				client->uvTime = uvtime->value;
			}
		}

		// items up here so that the bandolier will change equipclient below
		if (allitem->value) {
			AllItems(ent);
		}

		if ((teamplay->value && !teamdm->value && ctf->value != 2) || (!ctf->value && dm_choose->value))
			EquipClient(ent);
		else
			EquipClientDM(ent);

		if (ent->client->menu) {
			PMenu_Close(ent);
			return;
		}
//FIREBLADE
		if (allweapon->value) {
			AllWeapons(ent);
		}
		// force the current weapon up
		client->newweapon = client->pers.weapon;
		ChangeWeapon(ent);

//FIREBLADE
		if (teamplay->value) {
			ent->solid = SOLID_TRIGGER;
			gi.linkentity(ent);
		}
//FIREBLADE
	}
}

/*
=====================
ClientBeginDeathmatch

A client has just connected to the server in 
deathmatch mode, so clear everything out before starting them.
=====================
*/
void ClientBeginDeathmatch(edict_t * ent)
{
	int checkFrame;

	G_InitEdict(ent);

	InitClientResp(ent->client);

//PG BUND - BEGIN
	ent->client->resp.team = NOTEAM;

	// if no auto equip, prompt for new weapon on level change
	if (!auto_equip->value)
		ent->client->resp.dm_selected = 0;

	/*client->resp.last_killed_target = NULL;
	   client->resp.killed_teammates = 0;

	   client->resp.idletime = 0; - AQ2:TNG Slicer Moved this to InitClientResp */
	ResetKills(ent);
	TourneyNewPlayer(ent);
	vInitClient(ent);
//PG BUND - END

	// locate ent at a spawn point
	PutClientInServer(ent);

// FROM 3.20 -FB
	if (level.intermission_framenum) {
		MoveClientToIntermission(ent);
	} else {
// ^^^
		if (!teamplay->value && !dm_choose->value) {	//FB 5/31/99
			// send effect
			gi.WriteByte(svc_muzzleflash);
			gi.WriteShort(ent - g_edicts);
			gi.WriteByte(MZ_LOGIN);
			gi.multicast(ent->s.origin, MULTICAST_PVS);
		}
	}

	gi.bprintf(PRINT_HIGH, "%s entered the game\n", ent->client->pers.netname);
	IRC_printf(IRC_T_SERVER, "%n entered the game", ent->client->pers.netname);

	// TNG:Freud Automaticly join saved teams.
	if (teamplay->value && ent->client->resp.saved_team)
		JoinTeam(ent, ent->client->resp.saved_team, 1);

//FIREBLADE
	if (!teamplay->value && ent->solid == SOLID_NOT)
	{
		gi.bprintf(PRINT_HIGH, "%s became a spectator\n", ent->client->pers.netname);
		IRC_printf(IRC_T_SERVER, "%n became a spectator", ent->client->pers.netname);
	}
//FIREBLADE

//FIREBLADE
	if (!level.intermission_framenum)
		PrintMOTD(ent);
	ent->client->resp.motd_refreshes = 1;
//FIREBLADE

	//AQ2:TNG - Slicer: Set time to check clients
	checkFrame = level.framenum + (int)(check_time->value * HZ);
	ent->client->resp.checkframe[0] = checkFrame;
	ent->client->resp.checkframe[1] = checkFrame + 2 * HZ;
	ent->client->resp.checkframe[2] = checkFrame + 3 * HZ;

	// make sure all view stuff is valid
	ClientEndServerFrame(ent);
}

/*
===========
ClientBegin

called when a client has finished connecting, and is ready
to be placed into the game.  This will happen every level load.
============
*/
void ClientBegin(edict_t * ent)
{
	ent->client = game.clients + (ent - g_edicts - 1);

	ClientBeginDeathmatch(ent);
}

/*
===========
ClientUserInfoChanged

called whenever the player updates a userinfo variable.

The game can override any of the settings in place
(forcing skins or names, etc) before copying it off.
============
*/
void ClientUserinfoChanged(edict_t * ent, char *userinfo)
{
	char *s, *r, tnick[16];
	qboolean nickChanged = false;
	gclient_t *client = ent->client;

	// check for malformed or illegal info strings
	if (!Info_Validate(userinfo)) {
		strcpy(userinfo, "\\name\\badinfo\\skin\\male/grunt");
	}
	// set name
	s = Info_ValueForKey(userinfo, "name");
	Q_strncpyz(tnick, s, sizeof(tnick));
	if(!tnick[0])
		strcpy(tnick, "unnamed");

	if(strcmp(client->pers.netname, tnick))
	{
		// on the initial update, we won't broadcast the message.
		if (client->pers.netname[0])
		{
			gi.bprintf(PRINT_MEDIUM, "%s is now known as %s.\n", client->pers.netname, tnick);	//TempFile
			IRC_printf(IRC_T_SERVER, "%n is now known as %n.", client->pers.netname, tnick);
			nickChanged = true;
		}
		strcpy(client->pers.netname, tnick);
	}

	//FIREBLADE     
	s = Info_ValueForKey(userinfo, "spectator");
	client->pers.spectator = (strcmp(s, "0") != 0);

	r = Info_ValueForKey(userinfo, "rate");
	client->rate = atoi(r);
	//FIREBLADE

	// set skin
	s = Info_ValueForKey(userinfo, "skin");

	// AQ:TNG - JBravo fixing $$ Skin server crash bug
	if (strstr(s, "$$")) {
		Info_SetValueForKey(userinfo, "skin", "male/grunt");
		s = Info_ValueForKey(userinfo, "skin");
	}
	// End $$ Skin server crash bug


	// combine name and skin into a configstring
	AssignSkin(ent, s, nickChanged);

	client->pers.firing_style = ACTION_FIRING_CENTER;
	// handedness
	s = Info_ValueForKey(userinfo, "hand");
	if (strlen(s)) {
		client->pers.hand = atoi(s);
		if (strstr(s, "classic high") != NULL)
			client->pers.firing_style = ACTION_FIRING_CLASSIC_HIGH;
		else if (strstr(s, "classic") != NULL)
			client->pers.firing_style = ACTION_FIRING_CLASSIC;
	}
	// save off the userinfo in case we want to check something later
	Q_strncpyz(client->pers.userinfo, userinfo, sizeof(client->pers.userinfo));

	s = Info_ValueForKey( client->pers.userinfo, "gender" );
	if (s[0] == 'f' || s[0] == 'F') {
		client->pers.gender = GENDER_FEMALE;
	} else if (s[0] == 'm' || s[0] == 'M') {
		client->pers.gender = GENDER_MALE;
	} else {
		client->pers.gender = GENDER_NEUTRAL;
	}

	// zucc vwep
	ShowGun(ent);

}

/*
===========
ClientConnect

Called when a player begins connecting to the server.
The game can refuse entrance to a client by returning false.
If the client is allowed, the connection process will continue
and eventually get to ClientBegin()
Changing levels will NOT cause this to be called again, but
loadgames will.
============
*/
qboolean ClientConnect(edict_t * ent, char *userinfo)
{
	char *value, *ipaddr;
	char ipaddr_buf[100];
	int tempBan = 0;

	// check to see if they are on the banned IP list
	ipaddr = Info_ValueForKey(userinfo, "ip");

	if (strlen(ipaddr) > sizeof(ipaddr_buf) - 1)
		gi.dprintf("ipaddr_buf length exceeded\n");
	Q_strncpyz(ipaddr_buf, ipaddr, sizeof(ipaddr_buf));

	if (SV_FilterPacket(ipaddr, &tempBan)) {
		userinfo[0] = '\0';
		if(tempBan)
			Info_SetValueForKey(userinfo, "rejmsg", va("Temporary banned for %i games.", tempBan));
		else
			Info_SetValueForKey(userinfo, "rejmsg", "Banned.");
		return false;
	}
	// check for a password
	value = Info_ValueForKey(userinfo, "password");
	if (*password->string && strcmp(password->string, "none") && strcmp(password->string, value)) {
		userinfo[0] = '\0';
		Info_SetValueForKey(userinfo, "rejmsg", "Password required or incorrect.");
		return false;
	}

	if (vClientConnect(ent, userinfo) == false)
		return false;

	// they can connect
	ent->client = game.clients + (ent - g_edicts - 1);

	ent->client->resp.stat_mode = 0;
	ent->client->team_kills = 0;
	ent->client->team_wounds = 0;
	ent->client->team_wounds_before = 0;

	ResetKills(ent);

	// We're not going to attempt to support reconnection...
	if (ent->inuse == true) {
		ClientDisconnect(ent);
		ent->inuse = false;
	}

	if (ent->inuse == false) {
		// clear the respawning variables
		InitClientResp(ent->client);
		if (!game.autosaved || !ent->client->pers.weapon)
			InitClientPersistant(ent->client);
	}

	ClientUserinfoChanged(ent, userinfo);

	if (game.maxclients > 1) {
		gi.dprintf("%s@%s connected\n", ent->client->pers.netname, ipaddr_buf);
		IRC_printf(IRC_T_SERVER, "%n@%s connected", ent->client->pers.netname, ipaddr_buf);
	}

	Q_strncpyz(ent->client->ipaddr, ipaddr_buf, sizeof(ent->client->ipaddr));

	ent->svflags = 0;

	ent->client->pers.connected = true;
	return true;
}

/*
===========
ClientDisconnect

Called when a player drops from the server.
Will not be called between levels.
============
*/
void ClientDisconnect(edict_t * ent)
{
	int playernum, i;
	edict_t *etemp;

	if (!ent->client)
		return;

	MM_LeftTeam( ent );

	// reset item and weapon on disconnect
	ent->client->resp.item = NULL;
	ent->client->resp.weapon = NULL;
	ent->client->resp.dm_selected = 0;
	ent->client->resp.menu_shown = 0;

	// drop items if they are alive/not observer
	if (ent->solid != SOLID_NOT)
		TossItemsOnDeath(ent);

	// zucc free the lasersight if applicable
	if (ent->lasersight) {
		G_FreeEdict(ent->lasersight);
		ent->lasersight = NULL;
	}

	// TNG Free Flashlight
	if (ent->flashlight)
		FL_make(ent);

	if (teamplay->value && ent->solid == SOLID_TRIGGER)
		RemoveFromTransparentList(ent);

	gi.bprintf(PRINT_HIGH, "%s disconnected\n", ent->client->pers.netname);
	IRC_printf(IRC_T_SERVER, "%n disconnected", ent->client->pers.netname);

	// go clear any clients that have this guy as their attacker
	for (i = 1; i <= game.maxclients; i++) {
		if ((etemp = &g_edicts[i]) && etemp->inuse) {
			if (etemp->client->attacker == ent)
				etemp->client->attacker = NULL;
			if (etemp->enemy == ent)	// AQ:TNG - JBravo adding tkok
				etemp->enemy = NULL;
		}
	}

	TourneyRemovePlayer(ent);
	vClientDisconnect(ent);	// client voting disconnect

	if (use_ghosts->value)
		CreateGhost(ent);

	if (ctf->value)
		CTFDeadDropFlag(ent);

	if (!teamplay->value) {
		// send effect
		gi.WriteByte(svc_muzzleflash);
		gi.WriteShort(ent - g_edicts);
		gi.WriteByte(MZ_LOGOUT);
		gi.multicast(ent->s.origin, MULTICAST_PVS);
	}

	gi.unlinkentity(ent);
	ent->s.modelindex = 0;
	ent->solid = SOLID_NOT;
	ent->inuse = false;
	ent->classname = "disconnected";
	ent->client->pers.connected = false;

	playernum = ent - g_edicts - 1;
	gi.configstring(CS_PLAYERSKINS + playernum, "");
}

void CreateGhost(edict_t * ent)
{
	int x;
	qboolean duplicate = false;

	if (ent->client->resp.score == 0 && ent->client->resp.damage_dealt == 0) {
		return;
	}

	for (x = 0; x < num_ghost_players; x++) {
		if (duplicate == true) {
			ghost_players[x - 1] = ghost_players[x];
		} else if (strcmp(ghost_players[x].ipaddr, ent->client->ipaddr) == 0 &&
			   strcmp(ghost_players[x].netname, ent->client->pers.netname) == 0) {
			duplicate = true;
		}
	}

	if (duplicate == true)
		num_ghost_players--;

	if (num_ghost_players < MAX_GHOSTS) {

		strcpy(ghost_players[num_ghost_players].ipaddr, ent->client->ipaddr);
		strcpy(ghost_players[num_ghost_players].netname, ent->client->pers.netname);

		ghost_players[num_ghost_players].enterframe = ent->client->resp.enterframe;
		ghost_players[num_ghost_players].disconnect_frame = level.framenum;

		// Score
		ghost_players[num_ghost_players].score = ent->client->resp.score;
		ghost_players[num_ghost_players].damage_dealt = ent->client->resp.damage_dealt;
		ghost_players[num_ghost_players].kills = ent->client->resp.kills;

		// Teamplay variables
		if (teamplay->value) {
			ghost_players[num_ghost_players].weapon = ent->client->resp.weapon;
			ghost_players[num_ghost_players].item = ent->client->resp.item;
			ghost_players[num_ghost_players].team = ent->client->resp.team;
		}
		// Statistics

		ghost_players[num_ghost_players].shotsTotal = ent->client->resp.shotsTotal;
		ghost_players[num_ghost_players].hitsTotal = ent->client->resp.hitsTotal;

		memcpy(ghost_players[num_ghost_players].hitsLocations, ent->client->resp.hitsLocations,
		       sizeof(ent->client->resp.hitsLocations));
		memcpy(ghost_players[num_ghost_players].gunstats, ent->client->resp.gunstats,
		       sizeof(ent->client->resp.gunstats));

		num_ghost_players++;
	} else {
		gi.dprintf("Maximum number of ghosts reached.\n");
	}

}

//==============================================================

edict_t *pm_passent;

// pmove doesn't need to know about passent and contentmask
trace_t q_gameabi PM_trace(vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end)
{
	if (pm_passent && pm_passent->health > 0)
		return gi.trace(start, mins, maxs, end, pm_passent, MASK_PLAYERSOLID);
	else
		return gi.trace(start, mins, maxs, end, pm_passent, MASK_DEADSOLID);
}

/*
==============
ClientThink

This will be called once for each client frame, which will
usually be a couple times for each server frame.
==============
*/
void ClientThink(edict_t * ent, usercmd_t * ucmd)
{
	gclient_t *client;
	edict_t *other;
	int i, j;
	pmove_t pm;
	char ltm[64] = "\0";

	level.current_entity = ent;
	client = ent->client;

	if (level.intermission_framenum) {
		client->ps.pmove.pm_type = PM_FREEZE;
		// 
		if (level.realFramenum > level.intermission_framenum + 4 * HZ) {
			if (ent->inuse && client->resp.stat_mode > 0
			    && client->resp.stat_mode_intermission == 0) {
				client->resp.stat_mode_intermission = 1;
				Cmd_Stats_f(ent, ltm);
			}
		}
		// can exit intermission after five seconds
		if (level.realFramenum > level.intermission_framenum + 5 * HZ && (ucmd->buttons & BUTTON_ANY))
			level.intermission_exit = 1;
		return;
	}

	if (level.pauseFrames > 0)
	{
		client->ps.pmove.pm_type = PM_FREEZE;
		return;
	}
	pm_passent = ent;
	// FROM 3.20 -FB
	if (client->chase_mode) {
		client->resp.cmd_angles[0] = SHORT2ANGLE(ucmd->angles[0]);
		client->resp.cmd_angles[1] = SHORT2ANGLE(ucmd->angles[1]);
		client->resp.cmd_angles[2] = SHORT2ANGLE(ucmd->angles[2]);
	} else {
		// ^^^
		// set up for pmove
		memset(&pm, 0, sizeof(pm));
		if (ent->movetype == MOVETYPE_NOCLIP)
			client->ps.pmove.pm_type = PM_SPECTATOR;
		else if (ent->s.modelindex != 255)
			client->ps.pmove.pm_type = PM_GIB;
		else if (ent->deadflag)
			client->ps.pmove.pm_type = PM_DEAD;
		else
			client->ps.pmove.pm_type = PM_NORMAL;

		client->ps.pmove.gravity = sv_gravity->value;

		pm.s = client->ps.pmove;
		for (i = 0; i < 3; i++) {
			pm.s.origin[i] = ent->s.origin[i] * 8;
			pm.s.velocity[i] = ent->velocity[i] * 8;
		}

		if (memcmp(&client->old_pmove, &pm.s, sizeof(pm.s))) {
			pm.snapinitial = true;
			//      gi.dprintf ("pmove changed!\n");
		}

		pm.cmd = *ucmd;

		pm.trace = PM_trace;	// adds default parms
		pm.pointcontents = gi.pointcontents;
		// perform a pmove
		gi.Pmove(&pm);

		//FB 6/3/99 - info from Mikael Lindh from AQ:G
		if (pm.maxs[2] == 4) {
			ent->maxs[2] = CROUCHING_MAXS2;
			pm.maxs[2] = CROUCHING_MAXS2;
			ent->viewheight = CROUCHING_VIEWHEIGHT;
			pm.viewheight = (float) ent->viewheight;
		}
		//FB 6/3/99

		// save results of pmove
		client->ps.pmove = pm.s;
		client->old_pmove = pm.s;

		for (i = 0; i < 3; i++) {
			ent->s.origin[i] = pm.s.origin[i] * 0.125;
			ent->velocity[i] = pm.s.velocity[i] * 0.125;
		}

		if (client->leg_damage)
		{
			// really stopping jumping with leg damage
			if (ent->groundentity && pm.s.velocity[2] > 10) {
				ent->velocity[2] = 0.0;
			}

			// zucc stumbling associated with leg damage
			if ((level.framenum / game.framediv) % 6 <= 2) {
				//Slow down code FOO/zucc
				if (ent->groundentity && pm.groundentity) {
					ent->velocity[0] /= 4 * ent->client->leghits;	//FOO  
					ent->velocity[1] /= 4 * ent->client->leghits;	//FOO  
					if (ent->velocity[2] > 0)
						ent->velocity[2] /= 4 * ent->client->leghits;	//FOO     
				}
				if ((level.framenum / game.framediv) % (6 * 12) == 0 && client->leg_damage > 1)
					gi.sound(ent, CHAN_BODY, gi.soundindex(va("*pain100_1.wav")), 1, ATTN_NORM, 0);

				ent->velocity[0] = (float)((int)(ent->velocity[0] * 8)) / 8;
				ent->velocity[1] = (float)((int)(ent->velocity[1] * 8)) / 8;
				ent->velocity[2] = (float)((int)(ent->velocity[2] * 8)) / 8;
			}

		} else {
			// don't play sounds if they have leg damage, they can't jump anyway
			if (ent->groundentity && !pm.groundentity && pm.cmd.upmove >= 10 && pm.waterlevel == 0) {
				/* don't play jumps period.
				gi.sound(ent, CHAN_VOICE, gi.soundindex("*jump1.wav"), 1, ATTN_NORM, 0);
				PlayerNoise(ent, ent->s.origin, PNOISE_SELF);
				*/
				ent->client->jumping = 1;
			}
		}

		VectorCopy(pm.mins, ent->mins);
		VectorCopy(pm.maxs, ent->maxs);
		client->resp.cmd_angles[0] = SHORT2ANGLE(ucmd->angles[0]);
		client->resp.cmd_angles[1] = SHORT2ANGLE(ucmd->angles[1]);
		client->resp.cmd_angles[2] = SHORT2ANGLE(ucmd->angles[2]);

		ent->viewheight = pm.viewheight;
		ent->waterlevel = pm.waterlevel;
		ent->watertype = pm.watertype;
		ent->groundentity = pm.groundentity;
		if (pm.groundentity)
			ent->groundentity_linkcount = pm.groundentity->linkcount;
		if (ent->deadflag) {
			client->ps.viewangles[ROLL] = 40;
			client->ps.viewangles[PITCH] = -15;
			client->ps.viewangles[YAW] = client->killer_yaw;
		} else {
			VectorCopy(pm.viewangles, client->v_angle);
			VectorCopy(pm.viewangles, client->ps.viewangles);
		}

		if(client->ctf_grapple)
			CTFGrapplePull(client->ctf_grapple);

		gi.linkentity(ent);

		if (ent->movetype != MOVETYPE_NOCLIP)
			G_TouchTriggers(ent);

		// stop manipulating doors
		client->doortoggle = 0;

		// touch other objects
		for (i = 0; i < pm.numtouch; i++) {
			other = pm.touchents[i];
			for (j = 0; j < i; j++)
				if (pm.touchents[j] == other)
					break;
			if (j != i)
				continue;	// duplicated
			if (!other->touch)
				continue;
			other->touch(other, ent, NULL, NULL);
		}
	}

	client->oldbuttons = client->buttons;
	client->buttons = ucmd->buttons;
	client->latched_buttons |= client->buttons & ~client->oldbuttons;
	// save light level the player is standing on for
	// monster sighting AI
	ent->light_level = ucmd->lightlevel;

	// fire weapon from final position if needed
	if (client->latched_buttons & BUTTON_ATTACK) {
		//TempFile
		//We're gonna fire in this frame? Then abort any punching.
		client->resp.fire_time = level.framenum;
		client->resp.punch_desired = false;
		//TempFile
		//

		if (ent->solid == SOLID_NOT && ent->deadflag != DEAD_DEAD && !in_warmup) {
			client->latched_buttons = 0;
			NextChaseMode( ent );
		} else if (!client->weapon_thunk && FRAMESYNC) {
			client->weapon_thunk = true;
			Think_Weapon(ent);
		}
	}

	if (client->chase_mode) {
		if (ucmd->upmove >= 10) {
			if (!(client->ps.pmove.pm_flags & PMF_JUMP_HELD)) {
				client->ps.pmove.pm_flags |= PMF_JUMP_HELD;
				if (client->chase_target) {
					ChaseNext(ent);
				} else {
					GetChaseTarget(ent);
				}
			}
		} else {
			client->ps.pmove.pm_flags &= ~PMF_JUMP_HELD;
		}
	}

	if (ucmd->forwardmove || ucmd->sidemove || client->oldbuttons != client->buttons) {
		client->resp.idletime = 0;
	} else if (!client->resp.idletime) {
		client->resp.idletime = level.framenum;
	}
}

/*
==============
ClientBeginServerFrame

This will be called once for each server frame, before running
any other entities in the world.
==============
*/
void ClientBeginServerFrame(edict_t * ent)
{
	gclient_t *client;
	int buttonMask;

	client = ent->client;

	if (client->penalty > 0 && level.realFramenum % HZ == 0)
		client->penalty--;

	if (level.intermission_framenum)
		return;


	if ((int)motd_time->value > (client->resp.motd_refreshes * 2) && !(client->menu)) {
		if (client->resp.last_motd_refresh + 2 * HZ < level.realFramenum) {
			client->resp.last_motd_refresh = level.realFramenum;
			client->resp.motd_refreshes++;
			PrintMOTD( ent );
		}
	}

	// show team or weapon menu immediately when connected
	if (auto_menu->value && !client->menu && !client->resp.menu_shown && (teamplay->value || dm_choose->value)) {
		Cmd_Inven_f( ent );
	}


	// force spawn when weapon and item selected in dm
	if (dm_choose->value && !teamplay->value && !client->resp.dm_selected) {
		if (client->resp.weapon && (client->resp.item || itm_flags->value == 0)) {
			client->resp.dm_selected = 1;
			client->chase_mode = 0;
			client->chase_target = NULL;
			client->desired_fov = 90;
			client->ps.fov = 90;
			client->ps.pmove.pm_flags &= ~PMF_NO_PREDICTION;
			ent->solid = SOLID_BBOX;
			gi.linkentity(ent);
			gi.bprintf(PRINT_HIGH, "%s joined the game\n", client->pers.netname);
			IRC_printf(IRC_T_SERVER, "%n joined the game", client->pers.netname);

			// send effect
			gi.WriteByte(svc_muzzleflash);
			gi.WriteShort(ent - g_edicts);
			gi.WriteByte(MZ_LOGIN);
			gi.multicast(ent->s.origin, MULTICAST_PVS);

			respawn(ent);
		}
		return;
	}

//FIREBLADE
	if (!teamplay->value &&
	    ((ent->solid == SOLID_NOT && ent->deadflag != DEAD_DEAD) != ent->client->pers.spectator)) {
		if (ent->solid != SOLID_NOT || ent->deadflag == DEAD_DEAD) {
			if (ent->deadflag != DEAD_DEAD) {
				ent->flags &= ~FL_GODMODE;
				ent->health = 0;
				meansOfDeath = MOD_SUICIDE;
				player_die(ent, ent, ent, 100000, vec3_origin);
				// don't even bother waiting for death frames
				ent->deadflag = DEAD_DEAD;
				// This will make ClientBeginServerFrame crank us into observer mode
				// as soon as our death frames are done... -FB
				ent->solid = SOLID_NOT;
				// Also set this so we can have a way to know we've already done this...
				ent->movetype = MOVETYPE_NOCLIP;

				gi.linkentity(ent);
				gi.bprintf(PRINT_HIGH, "%s became a spectator\n", ent->client->pers.netname);
				IRC_printf(IRC_T_SERVER, "%n became a spectator", ent->client->pers.netname);
			} else	// immediately become observer...
			{
				if (ent->movetype != MOVETYPE_NOCLIP)	// have we already done this?  see above...
				{
					CopyToBodyQue(ent);
					ent->solid = SOLID_NOT;
					ent->svflags |= SVF_NOCLIENT;
					ent->movetype = MOVETYPE_NOCLIP;
					ent->client->pers.health = 100;
					ent->health = 100;
					ent->deadflag = DEAD_NO;
					gi.linkentity(ent);
					gi.bprintf(PRINT_HIGH, "%s became a spectator\n", ent->client->pers.netname);
					IRC_printf(IRC_T_SERVER, "%n became a spectator", ent->client->pers.netname);
				}
			}
		} else {
			client->chase_mode = 0;
			client->chase_target = NULL;
			client->desired_fov = 90;
			client->ps.fov = 90;	//FB 5/31/99 added
			client->ps.pmove.pm_flags &= ~PMF_NO_PREDICTION;
			ent->solid = SOLID_BBOX;
			gi.linkentity(ent);
			gi.bprintf(PRINT_HIGH, "%s rejoined the game\n", ent->client->pers.netname);
			IRC_printf(IRC_T_SERVER, "%n rejoined the game", ent->client->pers.netname);
			respawn(ent);
		}
	}
//FIREBLADE

	// run weapon animations if it hasn't been done by a ucmd_t
	if (FRAMESYNC) {
		if (!client->weapon_thunk)
			Think_Weapon(ent);
		else
			client->weapon_thunk = false;
	}

	if (ent->deadflag) {
		// wait for any button just going down
		if (level.framenum > client->respawn_time) {
//FIREBLADE
			if (((!ctf->value && !teamdm->value) || (client->resp.team == NOTEAM || client->resp.subteam))
			    && (teamplay->value || (ent->client->pers.spectator
					&& ent->solid == SOLID_NOT && ent->deadflag == DEAD_DEAD))) {
				CopyToBodyQue(ent);
				ent->solid = SOLID_NOT;
				ent->svflags |= SVF_NOCLIENT;
				ent->movetype = MOVETYPE_NOCLIP;
				ent->client->pers.health = 100;
				ent->health = 100;
				ent->deadflag = DEAD_NO;
				client->ps.pmove.delta_angles[PITCH] = ANGLE2SHORT(0 - client->resp.cmd_angles[PITCH]);
				client->ps.pmove.delta_angles[YAW] = ANGLE2SHORT(client->killer_yaw - client->resp.cmd_angles[YAW]);
				client->ps.pmove.delta_angles[ROLL] = ANGLE2SHORT(0 - client->resp.cmd_angles[ROLL]);
				ent->s.angles[PITCH] = 0;
				ent->s.angles[YAW] = client->killer_yaw;
				ent->s.angles[ROLL] = 0;
				VectorCopy(ent->s.angles, client->ps.viewangles);
				VectorCopy(ent->s.angles, client->v_angle);
				gi.linkentity(ent);

				if (teamplay->value && !in_warmup && limchasecam->value) {
					ent->client->chase_mode = 0;
					NextChaseMode( ent );
				}
			}
//FIREBLADE
			else {
				// in deathmatch, only wait for attack button
				buttonMask = BUTTON_ATTACK;

				if ((client->latched_buttons & buttonMask) ||DMFLAGS(DF_FORCE_RESPAWN)) {
					respawn(ent);
					client->latched_buttons = 0;
				}
			}
		}
		return;
	}

	if (ent->solid != SOLID_NOT)
	{
		if (!lights_camera_action && !client->uvTime) {
			if (client->jumping)
				kick_attack( ent );
			else if (client->resp.punch_desired)
				punch_attack( ent );
		}
		client->resp.punch_desired = false;

		if (ppl_idletime->value > 0 && client->resp.idletime && ent->deadflag != DEAD_DEAD) {
			if (level.framenum >= client->resp.idletime + (int)(ppl_idletime->value * HZ)) {
				PlayRandomInsaneSound( ent );
				client->resp.idletime = 0;
			}
		}

		if (client->autoreloading && (client->weaponstate == WEAPON_END_MAG)
			&& (client->curr_weap == MK23_NUM)) {
			client->autoreloading = false;
			Cmd_New_Reload_f( ent );
		}
	}

	if (client->uvTime && FRAMESYNC) {
		client->uvTime--;
		if (!client->uvTime)
		{
			if (team_round_going)
			{
				if (ctf->value && ctfgame.type == 2) {
					gi.centerprintf( ent,
						"ACTION!\n"
						"\n"
						"You are %s the %s base!",
						(client->resp.team == ctfgame.offence ?
						"ATTACKING" : "DEFENDING"),
						CTFOtherTeamName( ctfgame.offence ) );
				} else {
					gi.centerprintf( ent, "ACTION!" );
				}
			}
		}
		else if (client->uvTime % 10 == 0)
		{
			if (ctf->value && ctfgame.type == 2) {
				gi.centerprintf( ent,
					"Shield %d\n"
					"\n"
					"You are %s the %s base!",
					client->uvTime / 10,
					(client->resp.team == ctfgame.offence ?
					"ATTACKING" : "DEFENDING"),
					CTFOtherTeamName( ctfgame.offence ) );
			} else {
				gi.centerprintf( ent, "Shield %d", client->uvTime / 10 );
			}
		}
	}

	if (!in_warmup || ent->movetype != MOVETYPE_NOCLIP)
		client->latched_buttons = 0;
}
