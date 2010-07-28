//-----------------------------------------------------------------------------
// CTF related code
//
// $Id: a_ctf.c,v 1.20 2003/06/15 21:43:53 igor Exp $
//
//-----------------------------------------------------------------------------
// $Log: a_ctf.c,v $
// Revision 1.20  2003/06/15 21:43:53  igor
// added IRC client
//
// Revision 1.19  2003/02/10 02:12:25  ra
// Zcam fixes, kick crashbug in CTF fixed and some code cleanup.
//
// Revision 1.18  2002/04/01 15:16:06  freud
// Stats code redone, tng_stats now much more smarter. Removed a few global
// variables regarding stats code and added kevlar hits to stats.
//
// Revision 1.17  2002/02/18 17:17:20  freud
// Fixed the CTF leaving team bug. Also made the shield more efficient,
// No falling damage.
//
// Revision 1.16  2001/09/28 13:48:34  ra
// I ran indent over the sources. All .c and .h files reindented.
//
// Revision 1.15  2001/08/08 12:42:22  slicerdw
// Ctf Should finnaly be fixed now, lets hope so
//
// Revision 1.14  2001/08/06 14:38:44  ra
// Adding UVtime for ctf
//
// Revision 1.13  2001/06/26 18:47:30  igor_rock
// added ctf_respawn cvar
//
// Revision 1.12  2001/06/20 10:04:13  igor_rock
// corrected the path for the flagsounds (from ctf/ to tng/)
//
// Revision 1.11  2001/06/15 14:18:07  igor_rock
// corrected bug with destroyed flags (won't be destroyed anymore, instead they
// return to the base).
//
// Revision 1.10  2001/06/13 13:05:41  igor_rock
// corrected a minor error in CTFEffects
//
// Revision 1.9  2001/06/13 07:55:17  igor_rock
// Re-Added a_match.h and a_match.c
// Added CTF Header for a_ctf.h and a_ctf.c
//
//-----------------------------------------------------------------------------

#include "g_local.h"

ctfgame_t ctfgame;

cvar_t *ctf;
cvar_t *ctf_forcejoin;
cvar_t *ctf_mode;
cvar_t *ctf_dropflag;
cvar_t *ctf_respawn;
cvar_t *ctf_model;

/*--------------------------------------------------------------------------*/

gitem_t *flag1_item;
gitem_t *flag2_item;

void CTFInit(void)
{
	if (!flag1_item)
		flag1_item = FindItemByClassname("item_flag_team1");
	if (!flag2_item)
		flag2_item = FindItemByClassname("item_flag_team2");
	if(ctfgame.author)
		gi.TagFree(ctfgame.author);
	if(ctfgame.comment)
		gi.TagFree(ctfgame.comment);
	memset(&ctfgame, 0, sizeof(ctfgame));
}

/*--------------------------------------------------------------------------*/

qboolean CTFLoadConfig(char *mapname)
{
	char buf[1024];
	char *ptr;
	FILE *fh;

	gi.dprintf("Trying to load CTF configuration file\n", mapname);

	/* zero is perfectly acceptable respawn time, but we want to know if it came from the config or not */
	ctfgame.spawn_red = -1;
	ctfgame.spawn_blue = -1;

	sprintf (buf, "%s/tng/%s.ctf", GAMEVERSION, mapname);
	fh = fopen (buf, "r");
	if (!fh)
	{
		gi.dprintf ("Warning: CTF configuration file %s was not found.\n", buf);
		return false;
	}

	gi.dprintf("-------------------------------------\n");
	gi.dprintf("CTF configuration loaded from %s\n", buf);
	ptr = INI_Find(fh, "ctf", "author");
	if(ptr) {
		gi.dprintf(" Author    : %s\n", ptr);
		ctfgame.author = gi.TagMalloc(strlen(ptr)+1, TAG_LEVEL);
		strcpy(ctfgame.author, ptr);
	}
	ptr = INI_Find(fh, "ctf", "comment");
	if(ptr) {
		gi.dprintf(" Comment   : %s\n", ptr);
		ctfgame.comment = gi.TagMalloc(strlen(ptr)+1, TAG_LEVEL);
		strcpy(ctfgame.comment, ptr);
	}

	ptr = INI_Find(fh, "ctf", "type");
	if(ptr) {
		gi.dprintf(" Game type : %s\n", ptr);
		if(strcmp(ptr, "balanced") == 0)
			ctfgame.type = 1;
		if(strcmp(ptr, "offdef") == 0)
			ctfgame.type = 2;
	}
	ptr = INI_Find(fh, "ctf", "offence");
	if(ptr) {
		gi.dprintf(" Offence   : %s\n", ptr);
		ctfgame.offence = TEAM1;
		if(strcmp(ptr, "blue") == 0)
			ctfgame.offence = TEAM2;
	}
	ptr = INI_Find(fh, "ctf", "grapple");
	gi.cvar_forceset("use_grapple", "0");
	if(ptr) {
		gi.dprintf(" Grapple   : %s\n", ptr);
		if(strcmp(ptr, "1") == 0)
			gi.cvar_forceset("use_grapple", "1");
		else if(strcmp(ptr, "2") == 0)
			gi.cvar_forceset("use_grapple", "2");
	}

	gi.dprintf(" Spawn times\n");
	ptr = INI_Find(fh, "respawn", "red");
	if(ptr) {
		gi.dprintf("  Red      : %s\n", ptr);
		ctfgame.spawn_red = atoi(ptr);
	}
	ptr = INI_Find(fh, "respawn", "blue");
	if(ptr) {
		gi.dprintf("  Blue     : %s\n", ptr);
		ctfgame.spawn_blue = atoi(ptr);
	}

	gi.dprintf(" Flags\n");
	ptr = INI_Find(fh, "flags", "red");
	if(ptr) {
		gi.dprintf("  Red      : %s\n", ptr);
		CTFSetFlag(TEAM1, ptr);
	}
	ptr = INI_Find(fh, "flags", "blue");
	if(ptr) {
		gi.dprintf("  Blue     : %s\n", ptr);
		CTFSetFlag(TEAM2, ptr);
	}

	gi.dprintf(" Spawns\n");
	ptr = INI_Find(fh, "spawns", "red");
	if(ptr) {
		gi.dprintf("  Red      : %s\n", ptr);
		CTFSetTeamSpawns(TEAM1, ptr);
		ctfgame.custom_spawns = true;
	}
	ptr = INI_Find(fh, "spawns", "blue");
	if(ptr) {
		gi.dprintf("  Blue     : %s\n", ptr);
		CTFSetTeamSpawns(TEAM2, ptr);
		ctfgame.custom_spawns = true;
	}

	// automagically change spawns *only* when we do not have team spawns
	if(!ctfgame.custom_spawns)
		ChangePlayerSpawns();

	gi.dprintf("-------------------------------------\n");

	fclose(fh);

	return true;
}

/* taken from g_spawn */
char *ED_NewString (char *string);
void CTFSetFlag(int team, char *str)
{
	char *flag_name;
	edict_t *ent = NULL;
	vec3_t position;

	if(team == TEAM1)
		flag_name = "item_flag_team1";
	else if(team == TEAM2)
		flag_name = "item_flag_team2";
	else
		return;

	if (sscanf(str, "<%f %f %f>", &position[0], &position[1], &position[2]) != 3)
		return;

	/* find and remove existing flag(s) if any */
	while ((ent = G_Find(ent, FOFS(classname), flag_name)) != NULL) {
		G_FreeEdict (ent);
	}

	ent = G_Spawn ();

	ent->classname = ED_NewString (flag_name);
	ent->spawnflags &=
		~(SPAWNFLAG_NOT_EASY | SPAWNFLAG_NOT_MEDIUM | SPAWNFLAG_NOT_HARD |
		SPAWNFLAG_NOT_COOP | SPAWNFLAG_NOT_DEATHMATCH);

	VectorCopy(position, ent->s.origin);

	ED_CallSpawn (ent);
}

void CTFSetTeamSpawns(int team, char *str)
{
	edict_t *spawn = NULL;
	char *next;
	vec3_t pos;
	float angle;

	char *team_spawn_name = "info_player_team1";
	if(team == TEAM2)
		team_spawn_name = "info_player_team2";

	/* find and remove all team spawns for this team */
	while ((spawn = G_Find(spawn, FOFS(classname), team_spawn_name)) != NULL) {
		G_FreeEdict (spawn);
	}

	next = strtok(str, ",");
	do {
		if (sscanf(next, "<%f %f %f %f>", &pos[0], &pos[1], &pos[2], &angle) != 4) {
			gi.dprintf("CTFSetTeamSpawns: invalid spawn point: %s, expected <x y z a>\n", next);
			continue;
		}

		spawn = G_Spawn ();
		VectorCopy(pos, spawn->s.origin);
		spawn->s.angles[YAW] = angle;
		spawn->classname = ED_NewString (team_spawn_name);
		ED_CallSpawn (spawn);

		next = strtok(NULL, ",");
	} while(next != NULL);
}

/* returns the respawn time for this particular client */
int CTFGetRespawnTime(edict_t *ent)
{
	int spawntime = ctf_respawn->value;
	if(ent->client->resp.team == TEAM1 && ctfgame.spawn_red > -1)
		spawntime = ctfgame.spawn_red;
	if(ent->client->resp.team == TEAM2 && ctfgame.spawn_blue > -1)
		spawntime = ctfgame.spawn_blue;

	gi.cprintf(ent, PRINT_HIGH, "You will respawn in %d seconds\n", spawntime);

	return spawntime;
}

/* Has flag:
 * JBravo: Does the player have a flag ?
 */
qboolean HasFlag(edict_t * ent)
{
	if (!ctf->value)
		return false;
	if (ent->client->pers.inventory[ITEM_INDEX(flag1_item)] || ent->client->pers.inventory[ITEM_INDEX(flag2_item)])
		return true;
	return false;
}

char *CTFTeamName(int team)
{
	switch (team) {
	case TEAM1:
		return "RED";
	case TEAM2:
		return "BLUE";
	}
	return "UKNOWN";
}

char *CTFOtherTeamName(int team)
{
	switch (team) {
	case TEAM1:
		return "BLUE";
	case TEAM2:
		return "RED";
	}
	return "UKNOWN";
}

int CTFOtherTeam(int team)
{
	switch (team) {
	case TEAM1:
		return TEAM2;
	case TEAM2:
		return TEAM1;
	case NOTEAM:
		return NOTEAM; /* there is no other team for NOTEAM, but I want it back! */
	}
	return -1;		// invalid value
}

/*--------------------------------------------------------------------------*/

edict_t *SelectRandomDeathmatchSpawnPoint(void);
edict_t *SelectFarthestDeathmatchSpawnPoint(void);
float PlayersRangeFromSpot(edict_t * spot);

void ResetPlayers()
{
	edict_t *ent;
	int i;

	for (i = 0; i < game.maxclients; i++) {
		ent = &g_edicts[1 + i];
		if (ent->inuse) {
//			ent->client->resp.team = NOTEAM;
			PutClientInServer(ent);
		}
	}
}

void CTFSwapTeams()
{
	vec3_t point;
	edict_t *ent;
	int i;

	for (i = 0; i < game.maxclients; i++) {
		ent = &g_edicts[1 + i];
		if (ent->inuse) {
			ent->client->resp.team = CTFOtherTeam(ent->client->resp.team);
		}
	}

	/* swap scores too! */
	i = ctfgame.team1;
	ctfgame.team1 = ctfgame.team2;
	ctfgame.team2 = i;
}

void CTFAssignTeam(gclient_t * who)
{
	edict_t *player;
	int i, team1count = 0, team2count = 0;

	who->resp.ctf_state = CTF_STATE_START;

	if (!((int) dmflags->value & DF_CTF_FORCEJOIN)) {
		who->resp.team = NOTEAM;
		return;
	}

	for (i = 1; i <= (int)maxclients->value; i++) {
		player = &g_edicts[i];
		if (!player->inuse || player->client == who)
			continue;
		switch (player->client->resp.team) {
		case TEAM1:
			team1count++;
			break;
		case TEAM2:
			team2count++;
		}
	}
	if (team1count < team2count)
		who->resp.team = TEAM1;
	else if (team2count < team1count)
		who->resp.team = TEAM2;
	else if (rand() & 1)
		who->resp.team = TEAM1;
	else
		who->resp.team = TEAM2;
}

/*
================
SelectCTFSpawnPoint

go to a ctf point, but NOT the two points closest
to other players
================
*/
edict_t *SelectCTFSpawnPoint(edict_t * ent)
{
	edict_t *spot, *spot1, *spot2;
	int count = 0;
	int selection;
	float range, range1, range2;
	char *cname;

	/*if (ent->client->resp.ctf_state != CTF_STATE_START) {
		if (rand() & 1) {
			if ((int) (dmflags->value) & DF_SPAWN_FARTHEST)
				return SelectFarthestDeathmatchSpawnPoint();
			else
				return SelectRandomDeathmatchSpawnPoint();
		}
	}*/ //Why the fuck this was here? -Mani

	ent->client->resp.ctf_state = CTF_STATE_PLAYING;

	switch (ent->client->resp.team) {
	case TEAM1:
		cname = "info_player_team1";
		break;
	case TEAM2:
		cname = "info_player_team2";
		break;
	default:
		/* FIXME: might return NULL when dm spawns are converted to team ones */
		return SelectRandomDeathmatchSpawnPoint();
	}

	spot = NULL;
	range1 = range2 = 99999;
	spot1 = spot2 = NULL;

	while ((spot = G_Find(spot, FOFS(classname), cname)) != NULL) {
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
		return SelectRandomDeathmatchSpawnPoint();

	if (count <= 2) {
		spot1 = spot2 = NULL;
	} else
		count -= 2;

	selection = rand() % count;

	spot = NULL;
	do {
		spot = G_Find(spot, FOFS(classname), cname);
		if (spot == spot1 || spot == spot2)
			selection++;
	}
	while (selection--);

	return spot;
}

/*------------------------------------------------------------------------*/
/*
CTFFragBonuses

Calculate the bonuses for flag defense, flag carrier defense, etc.
Note that bonuses are not cumaltive.  You get one, they are in importance
order.
*/
void CTFFragBonuses(edict_t * targ, edict_t * inflictor, edict_t * attacker)
{
	int i, otherteam;
	gitem_t *flag_item, *enemy_flag_item;
	edict_t *ent, *flag, *carrier;
	char *c;
	vec3_t v1, v2;

	carrier = NULL;

	// no bonus for fragging yourself
	if (!targ->client || !attacker->client || targ == attacker)
		return;

	otherteam = CTFOtherTeam(targ->client->resp.team);
	if (otherteam < 0)
		return;		// whoever died isn't on a team

	// same team, if the flag at base, check to he has the enemy flag
	if (targ->client->resp.team == TEAM1) {
		flag_item = flag1_item;
		enemy_flag_item = flag2_item;
	} else {
		flag_item = flag2_item;
		enemy_flag_item = flag1_item;
	}

	// did the attacker frag the flag carrier?
	if (targ->client->pers.inventory[ITEM_INDEX(enemy_flag_item)]) {
		attacker->client->resp.ctf_lastfraggedcarrier = level.time;
		attacker->client->resp.score += CTF_FRAG_CARRIER_BONUS;
		gi.cprintf(attacker, PRINT_MEDIUM,
			   "BONUS: %d points for fragging enemy flag carrier.\n", CTF_FRAG_CARRIER_BONUS);

		// the the target had the flag, clear the hurt carrier
		// field on the other team
		for (i = 1; i <= maxclients->value; i++) {
			ent = g_edicts + i;
			if (ent->inuse && ent->client->resp.team == otherteam)
				ent->client->resp.ctf_lasthurtcarrier = 0;
		}
		return;
	}

	if (targ->client->resp.ctf_lasthurtcarrier &&
	    level.time - targ->client->resp.ctf_lasthurtcarrier <
	    CTF_CARRIER_DANGER_PROTECT_TIMEOUT && !attacker->client->pers.inventory[ITEM_INDEX(flag_item)]) {
		// attacker is on the same team as the flag carrier and
		// fragged a guy who hurt our flag carrier
		attacker->client->resp.score += CTF_CARRIER_DANGER_PROTECT_BONUS;
		gi.bprintf(PRINT_MEDIUM,
			   "%s defends %s's flag carrier against an agressive enemy\n",
			   attacker->client->pers.netname, CTFTeamName(attacker->client->resp.team));
		IRC_printf(IRC_T_GAME,
			   "%n defends %n's flag carrier against an agressive enemy\n",
			   attacker->client->pers.netname,
			   CTFTeamName(attacker->client->resp.team));
		return;
	}
	// flag and flag carrier area defense bonuses
	// we have to find the flag and carrier entities
	// find the flag
	switch (attacker->client->resp.team) {
	case TEAM1:
		c = "item_flag_team1";
		break;
	case TEAM2:
		c = "item_flag_team2";
		break;
	default:
		return;
	}

	flag = NULL;
	while ((flag = G_Find(flag, FOFS(classname), c)) != NULL) {
		if (!(flag->spawnflags & DROPPED_ITEM))
			break;
	}

	if (!flag)
		return;		// can't find attacker's flag

	// find attacker's team's flag carrier
	for (i = 1; i <= maxclients->value; i++) {
		carrier = g_edicts + i;
		if (carrier->inuse && carrier->client->pers.inventory[ITEM_INDEX(flag_item)])
			break;
		carrier = NULL;
	}

	// ok we have the attackers flag and a pointer to the carrier
	// check to see if we are defending the base's flag
	VectorSubtract(targ->s.origin, flag->s.origin, v1);
	VectorSubtract(attacker->s.origin, flag->s.origin, v2);

	if (VectorLength(v1) < CTF_TARGET_PROTECT_RADIUS ||
	    VectorLength(v2) < CTF_TARGET_PROTECT_RADIUS || loc_CanSee(flag, targ) || loc_CanSee(flag, attacker)) {
		// we defended the base flag
		attacker->client->resp.score += CTF_FLAG_DEFENSE_BONUS;
		if (flag->solid == SOLID_NOT) {
			gi.bprintf(PRINT_MEDIUM, "%s defends the %s base.\n",
				   attacker->client->pers.netname, CTFTeamName(attacker->client->resp.team));
			IRC_printf(IRC_T_GAME, "%n defends the %n base.\n",
				   attacker->client->pers.netname,
				   CTFTeamName(attacker->client->resp.team));
		} else {
			gi.bprintf(PRINT_MEDIUM, "%s defends the %s flag.\n",
				   attacker->client->pers.netname, CTFTeamName(attacker->client->resp.team));
			IRC_printf(IRC_T_GAME, "%n defends the %n flag.\n",
				   attacker->client->pers.netname,
				   CTFTeamName(attacker->client->resp.team));
		}
		return;
	}

	if (carrier && carrier != attacker) {
		VectorSubtract(targ->s.origin, carrier->s.origin, v1);
		VectorSubtract(attacker->s.origin, carrier->s.origin, v1);

		if (VectorLength(v1) < CTF_ATTACKER_PROTECT_RADIUS ||
		    VectorLength(v2) < CTF_ATTACKER_PROTECT_RADIUS ||
		    loc_CanSee(carrier, targ) || loc_CanSee(carrier, attacker)) {
			attacker->client->resp.score += CTF_CARRIER_PROTECT_BONUS;
			gi.bprintf(PRINT_MEDIUM, "%s defends the %s's flag carrier.\n",
				   attacker->client->pers.netname, CTFTeamName(attacker->client->resp.team));
			IRC_printf(IRC_T_GAME, "%n defends the %n's flag carrier.\n",
				   attacker->client->pers.netname,
				   CTFTeamName(attacker->client->resp.team));
			return;
		}
	}
}

void CTFCheckHurtCarrier(edict_t * targ, edict_t * attacker)
{
	gitem_t *flag_item;

	if (!targ->client || !attacker->client)
		return;

	if (targ->client->resp.team == TEAM1)
		flag_item = flag2_item;
	else
		flag_item = flag1_item;

	if (targ->client->pers.inventory[ITEM_INDEX(flag_item)] &&
	    targ->client->resp.team != attacker->client->resp.team)
		attacker->client->resp.ctf_lasthurtcarrier = level.time;
}

/*------------------------------------------------------------------------*/

void CTFResetFlag(int team)
{
	char *c;
	int i;
	edict_t *ent = NULL;

	switch (team) {
	case TEAM1:
		c = "item_flag_team1";
		break;
	case TEAM2:
		c = "item_flag_team2";
		break;
	default:
		return;
	}

	/* hifi: drop this team flag if a player is carrying one (so the next loop returns it correctly) */
	for (i = 1; i <= (int)maxclients->value; i++) {
		ent= &g_edicts[i];
		if(team == TEAM1) {
			if (ent->client->pers.inventory[ITEM_INDEX(flag1_item)]) {
				Drop_Item(ent, flag1_item);
				ent->client->pers.inventory[ITEM_INDEX(flag1_item)] = 0;
			}
		} else if(team == TEAM2) {
			if (ent->client->pers.inventory[ITEM_INDEX(flag2_item)]) {
				Drop_Item(ent, flag2_item);
				ent->client->pers.inventory[ITEM_INDEX(flag2_item)] = 0;
			}
		}
	}

	while ((ent = G_Find(ent, FOFS(classname), c)) != NULL) {
		if (ent->spawnflags & DROPPED_ITEM)
			G_FreeEdict(ent);
		else {
			ent->svflags &= ~SVF_NOCLIENT;
			ent->solid = SOLID_TRIGGER;
			gi.linkentity(ent);
			ent->s.event = EV_ITEM_RESPAWN;
		}
	}
}

void CTFResetFlags(void)
{
	CTFResetFlag(TEAM1);
	CTFResetFlag(TEAM2);
}

qboolean CTFPickup_Flag(edict_t * ent, edict_t * other)
{
	int team, i;
	edict_t *player;
	gitem_t *flag_item, *enemy_flag_item;

	/* FIXME: players shouldn't be able to touch flags before LCA! */
	if(!team_round_going)
		return false;

	// figure out what team this flag is
	if (strcmp(ent->classname, "item_flag_team1") == 0)
		team = TEAM1;
	else if (strcmp(ent->classname, "item_flag_team2") == 0)
		team = TEAM2;
	else {
		gi.cprintf(ent, PRINT_HIGH, "Don't know what team the flag is on.\n");
		return false;
	}

	// same team, if the flag at base, check to he has the enemy flag
	if (team == TEAM1) {
		flag_item = flag1_item;
		enemy_flag_item = flag2_item;
	} else {
		flag_item = flag2_item;
		enemy_flag_item = flag1_item;
	}

	if (team == other->client->resp.team) {

		if (!(ent->spawnflags & DROPPED_ITEM)) {
			// the flag is at home base.  if the player has the enemy
			// flag, he's just won!
			if (other->client->pers.inventory[ITEM_INDEX(enemy_flag_item)]) {
				gi.bprintf(PRINT_HIGH, "%s captured the %s flag!\n",
					   other->client->pers.netname, CTFOtherTeamName(team));
				IRC_printf(IRC_T_GAME, "%n captured the %n flag!\n",
					   other->client->pers.netname,
					   CTFOtherTeamName(team));
				other->client->pers.inventory[ITEM_INDEX(enemy_flag_item)] = 0;

				ctfgame.last_flag_capture = level.time;
				ctfgame.last_capture_team = team;
				if (team == TEAM1)
					ctfgame.team1++;
				else
					ctfgame.team2++;

				gi.sound(ent, CHAN_RELIABLE + CHAN_NO_PHS_ADD + CHAN_VOICE,
					 gi.soundindex("tng/flagcap.wav"), 1, ATTN_NONE, 0);

				// other gets another 10 frag bonus
				other->client->resp.score += CTF_CAPTURE_BONUS;

				CTFCapReward(other);

				// Ok, let's do the player loop, hand out the bonuses
				for (i = 1; i <= maxclients->value; i++) {
					player = &g_edicts[i];
					if (!player->inuse)
						continue;

					if (player->client->resp.team != other->client->resp.team)
						player->client->resp.ctf_lasthurtcarrier = -5;
					else if (player->client->resp.team == other->client->resp.team) {
						if (player != other)
							player->client->resp.score += CTF_TEAM_BONUS;
						// award extra points for capture assists
						if (player->client->resp.ctf_lastreturnedflag +
						    CTF_RETURN_FLAG_ASSIST_TIMEOUT > level.time) {
							gi.bprintf(PRINT_HIGH,
								   "%s gets an assist for returning the flag!\n",
								   player->client->pers.netname);
							IRC_printf(IRC_T_GAME,
								   "%n gets an assist for returning the flag!\n",
								   player->client->pers.netname);
							player->client->resp.score += CTF_RETURN_FLAG_ASSIST_BONUS;
						}
						if (player->client->resp.ctf_lastfraggedcarrier +
						    CTF_FRAG_CARRIER_ASSIST_TIMEOUT > level.time) {
							gi.bprintf(PRINT_HIGH,
								   "%s gets an assist for fragging the flag carrier!\n",
								   player->client->pers.netname);
							IRC_printf(IRC_T_GAME,
								   "%n gets an assist for fragging the flag carrier!\n",
								   player->client->pers.netname);
							player->client->resp.score += CTF_FRAG_CARRIER_ASSIST_BONUS;
						}
					}
				}

				CTFResetFlags();
				return false;
			}
			return false;	// its at home base already
		}
		// hey, its not home.  return it by teleporting it back
		gi.bprintf(PRINT_HIGH, "%s returned the %s flag!\n", other->client->pers.netname, CTFTeamName(team));
		IRC_printf(IRC_T_GAME, "%n returned the %s flag!\n", other->client->pers.netname, CTFTeamName(team));

		other->client->resp.score += CTF_RECOVERY_BONUS;
		other->client->resp.ctf_lastreturnedflag = level.time;
		gi.sound(ent, CHAN_RELIABLE + CHAN_NO_PHS_ADD + CHAN_VOICE,
			 gi.soundindex("tng/flagret.wav"), 1, ATTN_NONE, 0);
		//CTFResetFlag will remove this entity!  We must return false
		CTFResetFlag(team);
		return false;
	}
// AQ2:TNG - JBravo adding UVtime
	if (other->client->ctf_uvtime) {
		other->client->ctf_uvtime = 0;
		gi.centerprintf(other, "Flag taken! Shields are DOWN! Run for it!");
	} else {
		gi.centerprintf(other, "You've got the ENEMY FLAG! Run for it!");
	}
	// hey, its not our flag, pick it up
	gi.bprintf(PRINT_HIGH, "%s got the %s flag!\n", other->client->pers.netname, CTFTeamName(team));
	IRC_printf(IRC_T_GAME, "%n got the %n flag!\n", other->client->pers.netname, CTFTeamName(team));
	other->client->resp.score += CTF_FLAG_BONUS;

	other->client->pers.inventory[ITEM_INDEX(flag_item)] = 1;
	other->client->resp.ctf_flagsince = level.time;

	// pick up the flag
	// if it's not a dropped flag, we just make is disappear
	// if it's dropped, it will be removed by the pickup caller
	if (!(ent->spawnflags & DROPPED_ITEM)) {
		ent->flags |= FL_RESPAWN;
		ent->svflags |= SVF_NOCLIENT;
		ent->solid = SOLID_NOT;
	}
	return true;
}

static void CTFDropFlagTouch(edict_t * ent, edict_t * other, cplane_t * plane, csurface_t * surf)
{
	//owner (who dropped us) can't touch for two secs
	if (other == ent->owner && ent->nextthink - level.time > CTF_AUTO_FLAG_RETURN_TIMEOUT - 2)
		return;

	Touch_Item(ent, other, plane, surf);
}

static void CTFDropFlagThink(edict_t * ent)
{
	// auto return the flag
	// reset flag will remove ourselves
	if (strcmp(ent->classname, "item_flag_team1") == 0) {
		CTFResetFlag(TEAM1);
		gi.bprintf(PRINT_HIGH, "The %s flag has returned!\n", CTFTeamName(TEAM1));
		IRC_printf(IRC_T_GAME, "The %n flag has returned!\n", CTFTeamName(TEAM1));
	} else if (strcmp(ent->classname, "item_flag_team2") == 0) {
		CTFResetFlag(TEAM2);
		gi.bprintf(PRINT_HIGH, "The %s flag has returned!\n", CTFTeamName(TEAM2));
		IRC_printf(IRC_T_GAME, "The %n flag has returned!\n", CTFTeamName(TEAM2));
	}
}

// Called from PlayerDie, to drop the flag from a dying player
void CTFDeadDropFlag(edict_t * self)
{
	edict_t *dropped = NULL;

	if (!flag1_item || !flag2_item)
		CTFInit();

	if (self->client->pers.inventory[ITEM_INDEX(flag1_item)]) {
		dropped = Drop_Item(self, flag1_item);
		self->client->pers.inventory[ITEM_INDEX(flag1_item)] = 0;
		gi.bprintf(PRINT_HIGH, "%s lost the %s flag!\n", self->client->pers.netname, CTFTeamName(TEAM1));
		IRC_printf(IRC_T_GAME, "%n lost the %n flag!\n", self->client->pers.netname, CTFTeamName(TEAM1));

	} else if (self->client->pers.inventory[ITEM_INDEX(flag2_item)]) {
		dropped = Drop_Item(self, flag2_item);
		self->client->pers.inventory[ITEM_INDEX(flag2_item)] = 0;
		gi.bprintf(PRINT_HIGH, "%s lost the %s flag!\n", self->client->pers.netname, CTFTeamName(TEAM2));
		IRC_printf(IRC_T_GAME, "%n lost the %n flag!\n", self->client->pers.netname, CTFTeamName(TEAM2));
	}

	if (dropped) {
		dropped->think = CTFDropFlagThink;
		dropped->nextthink = level.time + CTF_AUTO_FLAG_RETURN_TIMEOUT;
		dropped->touch = CTFDropFlagTouch;
	}
}

void CTFDrop_Flag(edict_t * ent, gitem_t * item)
{
	edict_t *dropped = NULL;

	if (ctf_dropflag->value) {
		if (!flag1_item || !flag2_item)
			CTFInit();

		if (ent->client->pers.inventory[ITEM_INDEX(flag1_item)]) {
			dropped = Drop_Item(ent, flag1_item);
			ent->client->pers.inventory[ITEM_INDEX(flag1_item)] = 0;
		} else if (ent->client->pers.inventory[ITEM_INDEX(flag2_item)]) {
			dropped = Drop_Item(ent, flag2_item);
			ent->client->pers.inventory[ITEM_INDEX(flag2_item)] = 0;
		}

		if (dropped) {
			dropped->think = CTFDropFlagThink;
			dropped->nextthink = level.time + CTF_AUTO_FLAG_RETURN_TIMEOUT;
			dropped->touch = CTFDropFlagTouch;
		}
	} else {
		if (rand() & 1)
			gi.cprintf(ent, PRINT_HIGH, "Only lusers drop flags.\n");
		else
			gi.cprintf(ent, PRINT_HIGH, "Winners don't drop flags.\n");
	}
	return;
}

static void CTFFlagThink(edict_t * ent)
{
	if (ent->solid != SOLID_NOT)
		ent->s.frame = 173 + (((ent->s.frame - 173) + 1) % 16);
	ent->nextthink = level.time + FRAMETIME;
}

void CTFFlagSetup(edict_t * ent)
{
	trace_t tr;
	vec3_t dest;

	VectorSet(ent->mins, -15, -15, -15);
	VectorSet(ent->maxs, 15, 15, 15);

	if (ent->model)
		gi.setmodel(ent, ent->model);
	else
		gi.setmodel(ent, ent->item->world_model);
	ent->solid = SOLID_TRIGGER;
	ent->movetype = MOVETYPE_TOSS;
	ent->touch = Touch_Item;

	VectorCopy(ent->s.origin, dest);
	dest[2] -= 128;

	tr = gi.trace(ent->s.origin, ent->mins, ent->maxs, dest, ent, MASK_SOLID);
	if (tr.startsolid) {
		gi.dprintf("CTFFlagSetup: %s startsolid at %s\n", ent->classname, vtos(ent->s.origin));
		G_FreeEdict(ent);
		return;
	}

	VectorCopy(tr.endpos, ent->s.origin);

	gi.linkentity(ent);

	ent->nextthink = level.time + FRAMETIME;
	ent->think = CTFFlagThink;
}

void CTFEffects(edict_t * player)
{
	player->s.effects &= ~(EF_FLAG1 | EF_FLAG2);
	if (player->health > 0) {
		if (player->client->pers.inventory[ITEM_INDEX(flag1_item)]) {
			player->s.effects |= EF_FLAG1;
		}
		if (player->client->pers.inventory[ITEM_INDEX(flag2_item)]) {
			player->s.effects |= EF_FLAG2;
		}
	}

	// megahealth players glow anyway
	if(player->health > 100)
		player->s.effects |= EF_TAGTRAIL;

	if (player->client->pers.inventory[ITEM_INDEX(flag1_item)])
		player->s.modelindex3 = gi.modelindex("models/flags/flag1.md2");
	else if (player->client->pers.inventory[ITEM_INDEX(flag2_item)])
		player->s.modelindex3 = gi.modelindex("models/flags/flag2.md2");
	else
		player->s.modelindex3 = 0;
}

// called when we enter the intermission
void CTFCalcScores(void)
{
	int i;

	ctfgame.total1 = ctfgame.total2 = 0;
	for (i = 0; i < maxclients->value; i++) {
		if (!g_edicts[i + 1].inuse)
			continue;
		if (game.clients[i].resp.team == TEAM1)
			ctfgame.total1 += game.clients[i].resp.score;
		else if (game.clients[i].resp.team == TEAM2)
			ctfgame.total2 += game.clients[i].resp.score;
	}
}

void GetCTFScores(int *t1score, int *t2score)
{
	*t1score = ctfgame.team1;
	*t2score = ctfgame.team2;
}

void SetCTFStats(edict_t * ent)
{
	int p1, p2;
	edict_t *e;

	// logo headers for the frag display
	ent->client->ps.stats[STAT_TEAM1_HEADER] = gi.imageindex("ctfsb1");
	ent->client->ps.stats[STAT_TEAM2_HEADER] = gi.imageindex("ctfsb2");

	// if during intermission, we must blink the team header of the winning team
	if (level.intermissiontime && (level.framenum & 8)) {	// blink 1/8th second
		// note that ctfgame.total[12] is set when we go to intermission
		if (ctfgame.team1 > ctfgame.team2)
			ent->client->ps.stats[STAT_TEAM1_HEADER] = 0;
		else if (ctfgame.team2 > ctfgame.team1)
			ent->client->ps.stats[STAT_TEAM2_HEADER] = 0;
		else if (ctfgame.total1 > ctfgame.total2)	// frag tie breaker
			ent->client->ps.stats[STAT_TEAM1_HEADER] = 0;
		else if (ctfgame.total2 > ctfgame.total1)
			ent->client->ps.stats[STAT_TEAM2_HEADER] = 0;
		else {		// tie game!
			ent->client->ps.stats[STAT_TEAM1_HEADER] = 0;
			ent->client->ps.stats[STAT_TEAM2_HEADER] = 0;
		}
	}
	// figure out what icon to display for team logos
	// three states:
	//   flag at base
	//   flag taken
	//   flag dropped
	p1 = gi.imageindex("i_ctf1");
	e = G_Find(NULL, FOFS(classname), "item_flag_team1");
	if (e != NULL) {
		if (e->solid == SOLID_NOT) {
			int i;

			// not at base
			// check if on player
			p1 = gi.imageindex("i_ctf1d");	// default to dropped
			for (i = 1; i <= maxclients->value; i++)
				if (g_edicts[i].inuse && g_edicts[i].client->pers.inventory[ITEM_INDEX(flag1_item)]) {
					// enemy has it
					p1 = gi.imageindex("i_ctf1t");
					break;
				}
		} else if (e->spawnflags & DROPPED_ITEM)
			p1 = gi.imageindex("i_ctf1d");	// must be dropped
	}
	p2 = gi.imageindex("i_ctf2");
	e = G_Find(NULL, FOFS(classname), "item_flag_team2");
	if (e != NULL) {
		if (e->solid == SOLID_NOT) {
			int i;

			// not at base
			// check if on player
			p2 = gi.imageindex("i_ctf2d");	// default to dropped
			for (i = 1; i <= maxclients->value; i++)
				if (g_edicts[i].inuse && g_edicts[i].client->pers.inventory[ITEM_INDEX(flag2_item)]) {
					// enemy has it
					p2 = gi.imageindex("i_ctf2t");
					break;
				}
		} else if (e->spawnflags & DROPPED_ITEM)
			p2 = gi.imageindex("i_ctf2d");	// must be dropped
	}

	ent->client->ps.stats[STAT_TEAM1_PIC] = p1;
	ent->client->ps.stats[STAT_TEAM2_PIC] = p2;

	if (ctfgame.last_flag_capture && level.time - ctfgame.last_flag_capture < 5) {
		if (ctfgame.last_capture_team == TEAM1)
			if (level.framenum & 8)
				ent->client->ps.stats[STAT_TEAM1_PIC] = p1;
			else
				ent->client->ps.stats[STAT_TEAM1_PIC] = 0;
		else if (level.framenum & 8)
			ent->client->ps.stats[STAT_TEAM2_PIC] = p2;
		else
			ent->client->ps.stats[STAT_TEAM2_PIC] = 0;
	}

	ent->client->ps.stats[STAT_TEAM1_SCORE] = ctfgame.team1;
	ent->client->ps.stats[STAT_TEAM2_SCORE] = ctfgame.team2;

	ent->client->ps.stats[STAT_FLAG_PIC] = 0;
	if (ent->client->resp.team == TEAM1 &&
	    ent->client->pers.inventory[ITEM_INDEX(flag2_item)] && (level.framenum & 8))
		ent->client->ps.stats[STAT_FLAG_PIC] = gi.imageindex("i_ctf2");

	else if (ent->client->resp.team == TEAM2 &&
		 ent->client->pers.inventory[ITEM_INDEX(flag1_item)] && (level.framenum & 8))
		ent->client->ps.stats[STAT_FLAG_PIC] = gi.imageindex("i_ctf1");

	ent->client->ps.stats[STAT_ID_VIEW] = 0;
	if (!ent->client->resp.id)
		SetIDView(ent);
}

/*------------------------------------------------------------------------*/

/*QUAKED info_player_team1 (1 0 0) (-16 -16 -24) (16 16 32)
potential team1 spawning position for ctf games
*/
void SP_info_player_team1(edict_t * self)
{
}

/*QUAKED info_player_team2 (0 0 1) (-16 -16 -24) (16 16 32)
potential team2 spawning position for ctf games
*/
void SP_info_player_team2(edict_t * self)
{
}

/*
==================
CTFScoreboardMessage
==================
*/
void CTFScoreboardMessage(edict_t * ent, edict_t * killer)
{
	char entry[1024], string[1400], damage[50];
	int len, i, j, k, n;
	int sorted[2][MAX_CLIENTS];
	int sortedscores[2][MAX_CLIENTS];
	int score, total[2], totalscore[2];
	int last[2];
	gclient_t *cl;
	edict_t *cl_ent;
	int team, maxsize = 1000;

	if (ent->client->scoreboardnum == 1) {
		// sort the clients by team and score
		total[0] = total[1] = 0;
		last[0] = last[1] = 0;
		totalscore[0] = totalscore[1] = 0;
		for (i = 0; i < game.maxclients; i++) {
			cl_ent = g_edicts + 1 + i;
			if (!cl_ent->inuse)
				continue;
			if (game.clients[i].resp.team == TEAM1)
				team = 0;
			else if (game.clients[i].resp.team == TEAM2)
				team = 1;
			else
				continue;	// unknown team?

			score = game.clients[i].resp.score;
			for (j = 0; j < total[team]; j++) {
				if (score > sortedscores[team][j])
					break;
			}
			for (k = total[team]; k > j; k--) {
				sorted[team][k] = sorted[team][k - 1];
				sortedscores[team][k] = sortedscores[team][k - 1];
			}
			sorted[team][j] = i;
			sortedscores[team][j] = score;
			totalscore[team] += score;
			total[team]++;
		}

		// print level name and exit rules
		// add the clients in sorted order
		*string = 0;
		len = 0;

		// team one
		sprintf(string, "if 30 xv 8 yv 8 pic 30 endif "
			"xv 40 yv 28 string \"%4d/%-3d\" "
			"xv 98 yv 12 num 2 26 "
			"if 31 xv 168 yv 8 pic 31 endif "
			"xv 200 yv 28 string \"%4d/%-3d\" "
			"xv 256 yv 12 num 2 27 ", totalscore[0], total[0], totalscore[1], total[1]);
		len = strlen(string);

		for (i = 0; i < 16; i++) {
			if (i >= total[0] && i >= total[1])
				break;	// we're done
			*entry = 0;

			// left side
			if (i < total[0]) {
				cl = &game.clients[sorted[0][i]];
				cl_ent = g_edicts + 1 + sorted[0][i];
				sprintf(entry + strlen(entry),
					"ctf 0 %d %d %d %d ",
					42 + i * 8, sorted[0][i], cl->resp.score, cl->ping > 999 ? 999 : cl->ping);
				if (cl_ent->client->pers.inventory[ITEM_INDEX(flag2_item)])
					sprintf(entry + strlen(entry), "xv 56 yv %d picn sbfctf2 ", 42 + i * 8);
				if (maxsize - len > strlen(entry)) {
					strcat(string, entry);
					len = strlen(string);
					last[0] = i;
				}
			}
			// right side
			if (i < total[1]) {
				cl = &game.clients[sorted[1][i]];
				cl_ent = g_edicts + 1 + sorted[1][i];

				sprintf(entry + strlen(entry),
					"ctf 160 %d %d %d %d ",
					42 + i * 8, sorted[1][i], cl->resp.score, cl->ping > 999 ? 999 : cl->ping);

				if (cl_ent->client->pers.inventory[ITEM_INDEX(flag1_item)])
					sprintf(entry + strlen(entry), "xv 216 yv %d picn sbfctf1 ", 42 + i * 8);
				if (maxsize - len > strlen(entry)) {
					strcat(string, entry);
					len = strlen(string);
					last[1] = i;
				}
			}
		}

		// put in spectators if we have enough room
		if (last[0] > last[1])
			j = last[0];
		else
			j = last[1];
		j = (j + 2) * 8 + 42;

		k = n = 0;
		if (maxsize - len > 50) {
			for (i = 0; i < maxclients->value; i++) {
				cl_ent = g_edicts + 1 + i;
				cl = &game.clients[i];
				if (!cl_ent->inuse || cl_ent->solid != SOLID_NOT || cl_ent->client->resp.team != NOTEAM)
					continue;

				if (!k) {
					k = 1;
					sprintf(entry, "xv 0 yv %d string2 \"Spectators\" ", j);
					strcat(string, entry);
					len = strlen(string);
					j += 8;
				}

				sprintf(entry + strlen(entry), "ctf %d %d %d %d %d ", (n & 1) ? 160 : 0,	// x
					j,	// y
					i,	// playernum
					cl->resp.score, cl->ping > 999 ? 999 : cl->ping);
				if (maxsize - len > strlen(entry)) {
					strcat(string, entry);
					len = strlen(string);
				}

				if (n & 1)
					j += 8;
				n++;
			}
		}

		if (total[0] - last[0] > 1)	// couldn't fit everyone
			sprintf(string + strlen(string),
				"xv 8 yv %d string \"..and %d more\" ", 42 + (last[0] + 1) * 8, total[0] - last[0] - 1);
		if (total[1] - last[1] > 1)	// couldn't fit everyone
			sprintf(string + strlen(string),
				"xv 168 yv %d string \"..and %d more\" ",
				42 + (last[1] + 1) * 8, total[1] - last[1] - 1);
	} else if (ent->client->scoreboardnum == 2) {
		int total, score, ping;
		int sortedscores[MAX_CLIENTS], sorted[MAX_CLIENTS];

		total = score = 0;

		for (i = 0; i < game.maxclients; i++) {
			cl_ent = g_edicts + 1 + i;
			if (!cl_ent->inuse)
				continue;

			score = game.clients[i].resp.score;
			if (noscore->value) {
				j = total;
			} else {
				for (j = 0; j < total; j++) {
					if (score > sortedscores[j])
						break;
				}
				for (k = total; k > j; k--) {
					sorted[k] = sorted[k - 1];
					sortedscores[k] = sortedscores[k - 1];
				}
			}
			sorted[j] = i;
			sortedscores[j] = score;
			total++;
		}

		if (noscore->value)
			// AQ2:TNG Deathwatch - Nice little bar
		{
			strcpy(string, "xv 0 yv 32 string2 \"Player          Time Ping\" "
			       "xv 0 yv 40 string2 \"ùûûûûûûûûûûûûûü ùûûü ùûûü\" ");
		} else {
			strcpy(string,
			       "xv 0 yv 32 string2 \"Frags Player          Time Ping Damage Kills\" "
			       "xv 0 yv 40 string2 \"ùûûûü ùûûûûûûûûûûûûûü ùûûü ùûûü ùûûûûü ùûûûü\" ");
		}
		// AQ2:TNG END

		for (i = 0; i < total; i++) {
			ping = game.clients[sorted[i]].ping;
			if (ping > 999)
				ping = 999;
			if (noscore->value) {
				sprintf(string + strlen(string),
					"xv 0 yv %d string \"%-15s %4d %4d\" ",
					48 + i * 8,
					game.clients[sorted[i]].pers.netname,
					(level.framenum - game.clients[sorted[i]].resp.enterframe) / 600, ping);
			} else {
				if (game.clients[sorted[i]].resp.damage_dealt < 1000000)
					sprintf(damage, "%d", game.clients[sorted[i]].resp.damage_dealt);
				else
					strcpy(damage, "******");
				sprintf(string + strlen(string),
					"xv 0 yv %d string \"%5d %-15s %4d %4d %6s %5d\" ",
					48 + i * 8,
					sortedscores[i],
					game.clients[sorted[i]].pers.netname,
					(level.framenum -
					 game.clients[sorted[i]].resp.enterframe) / 600, ping,
					damage, game.clients[sorted[i]].resp.kills);
			}

			if (strlen(string) > (maxsize - 100) && i < (total - 2)) {
				sprintf(string + strlen(string),
					"xv 0 yv %d string \"..and %d more\" ", 48 + (i + 1) * 8, (total - i - 1));
				break;
			}
		}
	}

	if (strlen(string) > 1023)	// for debugging...
		gi.dprintf("Warning: scoreboard string neared or exceeded max length\nDump:\n%s\n---\n", string);

	gi.WriteByte(svc_layout);
	gi.WriteString(string);
}

/*-----------------------------------------------------------------------*/
/*QUAKED misc_ctf_banner (1 .5 0) (-4 -64 0) (4 64 248) TEAM2
The origin is the bottom of the banner.
The banner is 248 tall.
*/
static void misc_ctf_banner_think(edict_t * ent)
{
	ent->s.frame = (ent->s.frame + 1) % 16;
	ent->nextthink = level.time + FRAMETIME;
}

void SP_misc_ctf_banner(edict_t * ent)
{
	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_NOT;
	ent->s.modelindex = gi.modelindex("models/ctf/banner/tris.md2");
	if (ent->spawnflags & 1)	// team2
		ent->s.skinnum = 1;

	ent->s.frame = rand() % 16;
	gi.linkentity(ent);

	ent->think = misc_ctf_banner_think;
	ent->nextthink = level.time + FRAMETIME;
}

/*QUAKED misc_ctf_small_banner (1 .5 0) (-4 -32 0) (4 32 124) TEAM2
The origin is the bottom of the banner.
The banner is 124 tall.
*/
void SP_misc_ctf_small_banner(edict_t * ent)
{
	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_NOT;
	ent->s.modelindex = gi.modelindex("models/ctf/banner/small.md2");
	if (ent->spawnflags & 1)	// team2
		ent->s.skinnum = 1;

	ent->s.frame = rand() % 16;
	gi.linkentity(ent);

	ent->think = misc_ctf_banner_think;
	ent->nextthink = level.time + FRAMETIME;
}

/*-----------------------------------------------------------------------*/

void CTFCredits(edict_t * ent, pmenu_t * p);

void DeathmatchScoreboard(edict_t * ent);

void CTFShowScores(edict_t * ent, pmenu_t * p)
{
	PMenu_Close(ent);

	ent->client->showscores = true;
	ent->client->showinventory = false;
	DeathmatchScoreboard(ent);
}

qboolean CTFCheckRules(void)
{
	if (capturelimit->value && (ctfgame.team1 >= capturelimit->value || ctfgame.team2 >= capturelimit->value)) {
		gi.bprintf(PRINT_HIGH, "Capturelimit hit.\n");
		IRC_printf(IRC_T_GAME, "Capturelimit hit.\n");
		return true;
	}

	if(timelimit->value > 0 && ctfgame.type > 0) {
		if(ctfgame.halftime == 0 && level.time == (timelimit->value * 60) / 2 - 60) {
			CenterPrintAll ("1 MINUTE LEFT...");
			gi.sound (&g_edicts[0], CHAN_VOICE | CHAN_NO_PHS_ADD,
				gi.soundindex ("tng/1_minute.wav"), 1.0, ATTN_NONE, 0.0);
			ctfgame.halftime = 1;
		}

		if(ctfgame.halftime == 1 && level.time == (timelimit->value * 60) / 2 - 10) {
			gi.sound (&g_edicts[0], CHAN_VOICE | CHAN_NO_PHS_ADD,
				gi.soundindex ("world/10_0.wav"), 1.0, ATTN_NONE, 0.0);
			ctfgame.halftime = 2;
		}

		if(ctfgame.halftime < 3 && level.time == (timelimit->value * 60) / 2 + 1) {
			team_round_going = team_round_countdown = team_game_going = 0;
			MakeAllLivePlayersObservers ();
			CTFSwapTeams();
			CenterPrintAll("The teams have been switched!");
			gi.sound(&g_edicts[0], CHAN_VOICE | CHAN_NO_PHS_ADD,
				 gi.soundindex("misc/secret.wav"), 1.0, ATTN_NONE, 0.0);
			ctfgame.halftime = 3;
		}
	}
	return false;
}

/*--------------------------------------------------------------------------
 * just here to help old map conversions
 *--------------------------------------------------------------------------*/

static void old_teleporter_touch(edict_t * self, edict_t * other, cplane_t * plane, csurface_t * surf)
{
	edict_t *dest;
	int i;
	vec3_t forward;

	if (!other->client)
		return;
	dest = G_Find(NULL, FOFS(targetname), self->target);
	if (!dest) {
		gi.dprintf("Couldn't find destination\n");
		return;
	}
	// unlink to make sure it can't possibly interfere with KillBox
	gi.unlinkentity(other);

	VectorCopy(dest->s.origin, other->s.origin);
	VectorCopy(dest->s.origin, other->s.old_origin);

	// clear the velocity and hold them in place briefly
	VectorClear(other->velocity);
	other->client->ps.pmove.pm_time = 160 >> 3;	// hold time
	other->client->ps.pmove.pm_flags |= PMF_TIME_TELEPORT;

	// draw the teleport splash at source and on the player
	self->enemy->s.event = EV_PLAYER_TELEPORT;
	other->s.event = EV_PLAYER_TELEPORT;

	// set angles
	for (i = 0; i < 3; i++)
		other->client->ps.pmove.delta_angles[i] =
		    ANGLE2SHORT(dest->s.angles[i] - other->client->resp.cmd_angles[i]);

	other->s.angles[PITCH] = 0;
	other->s.angles[YAW] = dest->s.angles[YAW];
	other->s.angles[ROLL] = 0;
	VectorCopy(dest->s.angles, other->client->ps.viewangles);
	VectorCopy(dest->s.angles, other->client->v_angle);

	// give a little forward velocity
	AngleVectors(other->client->v_angle, forward, NULL, NULL);
	VectorScale(forward, 200, other->velocity);

	// kill anything at the destination
	if (!KillBox(other)) {
	}

	gi.linkentity(other);
}

/*QUAKED trigger_teleport (0.5 0.5 0.5) ?
Players touching this will be teleported
*/
void SP_trigger_teleport(edict_t * ent)
{
	edict_t *s;
	int i;

	if (!ent->target) {
		gi.dprintf("teleporter without a target.\n");
		G_FreeEdict(ent);
		return;
	}

	ent->svflags |= SVF_NOCLIENT;
	ent->solid = SOLID_TRIGGER;
	ent->touch = old_teleporter_touch;
	gi.setmodel(ent, ent->model);
	gi.linkentity(ent);

	// noise maker and splash effect dude
	s = G_Spawn();
	ent->enemy = s;
	for (i = 0; i < 3; i++)
		s->s.origin[i] = ent->mins[i] + (ent->maxs[i] - ent->mins[i]) / 2;
	s->s.sound = gi.soundindex("world/hum1.wav");
	gi.linkentity(s);

}

/*QUAKED info_teleport_destination (0.5 0.5 0.5) (-16 -16 -24) (16 16 32)
Point trigger_teleports at these.
*/
void SP_info_teleport_destination(edict_t * ent)
{
	ent->s.origin[2] += 16;
}

void CTFDestroyFlag(edict_t * self)
{
	//flags are important
	if (ctf->value) {
		if (strcmp(self->classname, "item_flag_team1") == 0) {
			CTFResetFlag(TEAM1);	// this will free self!
			gi.bprintf(PRINT_HIGH, "The %s flag has returned!\n", CTFTeamName(TEAM1));
			IRC_printf(IRC_T_GAME, "The %n flag has returned!\n", CTFTeamName(TEAM1));
			return;
		}
		if (strcmp(self->classname, "item_flag_team2") == 0) {
			CTFResetFlag(TEAM2);	// this will free self!
			gi.bprintf(PRINT_HIGH, "The %s flag has returned!\n", CTFTeamName(TEAM2));
			IRC_printf(IRC_T_GAME, "The %n flag has returned!\n", CTFTeamName(TEAM2));
			return;
		}
	}
	// just release it.
	G_FreeEdict(self);
}

/* give client full health and some ammo for reward */
void CTFCapReward(edict_t * ent)
{
	gclient_t *client;
	gitem_t *item;
	edict_t etemp;
	int was_bandaging = 0;
	int band;
	int player_weapon;

	if(!ctf_mode->value)
		return;

	if(ctf_mode->value > 1)
		ent->client->resp.ctf_capstreak++;
	else /* capstreak is used as a multiplier so default it to one */
		ent->client->resp.ctf_capstreak = 1;

	band = ent->client->resp.ctf_capstreak;
	client = ent->client;

	// give initial knife if none
	if ((int)wp_flags->value & WPF_KNIFE &&
			ent->client->pers.inventory[ITEM_INDEX(GET_ITEM(KNIFE_NUM))] == 0)
		ent->client->pers.inventory[ITEM_INDEX(GET_ITEM(KNIFE_NUM))] += 1;

	if (client->resp.item->typeNum == BAND_NUM) {
		band =+ 1;
		if (tgren->value > 0)	// team grenades is turned on
		{
			item = GET_ITEM(GRENADE_NUM);
			client->pers.inventory[ITEM_INDEX(item)] = tgren->value;
		}

	}

	// give pistol clips
	if ((int)wp_flags->value & WPF_MK23) {
		item = GET_ITEM(MK23_ANUM);
		client->mk23_rds = client->mk23_max;
		client->pers.inventory[ITEM_INDEX(item)] = 1*band;
	}


	player_weapon = client->resp.weapon->typeNum;
	// find out which weapon the player is holding in it's inventory
	if(client->unique_weapon_total > 0) {
		if(ent->client->pers.inventory[ITEM_INDEX(GET_ITEM(MP5_NUM))])
			player_weapon = MP5_NUM;
		if(ent->client->pers.inventory[ITEM_INDEX(GET_ITEM(M4_NUM))])
			player_weapon = M4_NUM;
		if(ent->client->pers.inventory[ITEM_INDEX(GET_ITEM(M3_NUM))])
			player_weapon = M3_NUM;
		if(ent->client->pers.inventory[ITEM_INDEX(GET_ITEM(HC_NUM))])
			player_weapon = HC_NUM;
		if(ent->client->pers.inventory[ITEM_INDEX(GET_ITEM(SNIPER_NUM))])
			player_weapon = SNIPER_NUM;
	}


	// if player has no special weapon, give the initial one
	if (player_weapon == MP5_NUM) {
		if(client->unique_weapon_total < 1) {
			item = GET_ITEM(MP5_NUM);
			client->pers.inventory[ITEM_INDEX(item)] = 1;
			client->unique_weapon_total = 1;
		}
		item = GET_ITEM(MP5_ANUM);
		client->pers.inventory[ITEM_INDEX(item)] = 1*band;
		client->mp5_rds = client->mp5_max;
	} else if (player_weapon == M4_NUM) {
		if(client->unique_weapon_total < 1) {
			item = GET_ITEM(M4_NUM);
			client->pers.inventory[ITEM_INDEX(item)] = 1;
			client->unique_weapon_total = 1;
		}
		item = GET_ITEM(M4_ANUM);
		client->pers.inventory[ITEM_INDEX(item)] = 1*band;
		client->m4_rds = client->m4_max;
	} else if (player_weapon == M3_NUM) {
		if(client->unique_weapon_total < 1) {
			item = GET_ITEM(M3_NUM);
			client->pers.inventory[ITEM_INDEX(item)] = 1;
			client->unique_weapon_total = 1;
		}
		item = GET_ITEM(SHELL_ANUM);
		client->pers.inventory[ITEM_INDEX(item)] = 7*band;
		client->shot_rds = client->shot_max;
	} else if (player_weapon == HC_NUM) {
		if(client->unique_weapon_total < 1) {
			item = GET_ITEM(HC_NUM);
			client->pers.inventory[ITEM_INDEX(item)] = 1;
			client->unique_weapon_total = 1;
		}
		item = GET_ITEM(SHELL_ANUM);
		client->pers.inventory[ITEM_INDEX(item)] = 12*band;
		client->cannon_rds = client->cannon_max;
	} else if (player_weapon == SNIPER_NUM) {
		if(client->unique_weapon_total < 1) {
			item = GET_ITEM(SNIPER_NUM);
			client->pers.inventory[ITEM_INDEX(item)] = 1;
			client->unique_weapon_total = 1;
		}
		item = GET_ITEM(SNIPER_ANUM);
		client->pers.inventory[ITEM_INDEX(item)] = 10*band;
		client->sniper_rds = client->sniper_max;
	} else if (player_weapon == DUAL_NUM) {
		item = GET_ITEM(DUAL_NUM);
		client->pers.inventory[ITEM_INDEX(item)] = 1;

		item = GET_ITEM(MK23_ANUM);
		client->pers.inventory[ITEM_INDEX(item)] = 2*band;
		client->dual_rds = client->dual_max;
	} else if (player_weapon == KNIFE_NUM) {
		item = GET_ITEM(KNIFE_NUM);
		client->pers.inventory[ITEM_INDEX(item)] = 10*band;
	}

	if(ent->client->bandaging || ent->client->bandage_stopped)
		was_bandaging = 1;
	
	ent->client->leg_noise = 0;
	ent->client->leg_damage = 0;
	ent->client->leghits = 0;
	ent->client->bleeding = 0;
	ent->client->bleed_remain = 0;
	ent->client->bandaging = 0;
	ent->client->leg_dam_count = 0;
	ent->client->attacker = NULL;

	ent->client->bandage_stopped = 0;
	ent->client->idle_weapon = 0;

	// automagically change to special in any case, it's fully reloaded
	if((client->curr_weap != player_weapon && ent->client->weaponstate == WEAPON_READY) || was_bandaging) {
		client->newweapon = client->pers.weapon;
		ent->client->weaponstate = WEAPON_READY;
		ent->client->ps.gunframe = 0;
		ReadySpecialWeapon(ent);
	}

	// give health times cap streak
	ent->health = ent->max_health * (ent->client->resp.ctf_capstreak > 4 ? 4 : ent->client->resp.ctf_capstreak);
	ent->client->pers.health = ent->health;

	if(ent->client->resp.ctf_capstreak == 2)
		gi.centerprintf(ent, "CAPTURED TWO TIMES IN A ROW!\n\nYou have been rewarded with DOUBLE health and ammo!\n\nNow go get some more!");
	else if(ent->client->resp.ctf_capstreak == 3)
		gi.centerprintf(ent, "CAPTURED THREE TIMES IN A ROW!\n\nYou have been rewarded with TRIPLE health and ammo!\n\nNow go get some more!");
	else if(ent->client->resp.ctf_capstreak == 4)
		gi.centerprintf(ent, "CAPTURED FOUR TIMES IN A ROW!\n\nYou have been rewarded with QUAD health and ammo!\n\nNow go get some more!");
	else if(ent->client->resp.ctf_capstreak > 4)
		gi.centerprintf(ent, "CAPTURED YET AGAIN!\n\nYou have been rewarded QUAD health and %d times your ammo!\n\nNow go get some more!",
				ent->client->resp.ctf_capstreak);
	else	gi.centerprintf(ent, "CAPTURED!\n\nYou have been rewarded.\n\nNow go get some more!");
}
