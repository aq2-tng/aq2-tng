//-----------------------------------------------------------------------------
// PG BUND
// g_xmisc.c
//
// this module contains all new functions regarding g_misc.c
//
// $Id: g_xmisc.c,v 1.6 2004/01/02 11:58:08 igor_rock Exp $
//
//-----------------------------------------------------------------------------
// $Log: g_xmisc.c,v $
// Revision 1.6  2004/01/02 11:58:08  igor_rock
// corrected punch bug, you weren't able to punch teammates even if FF was on
//
// Revision 1.5  2003/02/10 02:12:25  ra
// Zcam fixes, kick crashbug in CTF fixed and some code cleanup.
//
// Revision 1.4  2001/09/28 13:48:34  ra
// I ran indent over the sources. All .c and .h files reindented.
//
// Revision 1.3  2001/08/06 14:38:45  ra
// Adding UVtime for ctf
//
// Revision 1.2  2001/08/06 03:00:49  ra
// Added FF after rounds. Please someone look at the EVIL if statments for me :)
//
// Revision 1.1.1.1  2001/05/06 17:31:34  igor_rock
// This is the PG Bund Edition V1.25 with all stuff laying around here...
//
//-----------------------------------------------------------------------------

#include "g_local.h"
#include "m_player.h"		//TempFile for punch animation

#ifndef MAX_STR_LEN
#define MAX_STR_LEN 1000
#endif				// MAX_STR_LEN

static char _inipath[MAX_STR_LEN] = "";

//this function belongs to g_weapon.c
void punch_attack(edict_t * ent)
{
	vec3_t start, forward, right, offset, end;
	int damage = 7;
	int kick = 100;
	int randmodify;
	trace_t tr;

	AngleVectors(ent->client->v_angle, forward, right, NULL);
	VectorScale(forward, 0, ent->client->kick_origin);
	VectorSet(offset, 0, 0, ent->viewheight - 20);
	P_ProjectSource(ent->client, ent->s.origin, offset, forward, right, start);
	VectorMA(start, 50, forward, end);
	PRETRACE();
	tr = gi.trace(ent->s.origin, NULL, NULL, end, ent, MASK_SHOT);
	POSTTRACE();

	if (!((tr.surface) && (tr.surface->flags & SURF_SKY)))
	{
		if (tr.fraction < 1.0 && tr.ent->takedamage)
		{
			if (tr.ent->health <= 0)
				return;
			
			if (teamplay->value) {
				// AQ2:TNG - JBravo adding UVtime
				if (tr.ent->client && tr.ent->client->ctf_uvtime)
					return;

				if ((tr.ent != ent) && tr.ent->client && ent->client &&
					(tr.ent->client->resp.team == ent->client->resp.team) &&
					((int)dmflags->value & DF_NO_FRIENDLY_FIRE))
				{
					if (team_round_going || !ff_afterround->value)
						return;
				}
			}
			else if (((tr.ent != ent) && ((deathmatch->value &&
				((int) (dmflags->value) & (DF_MODELTEAMS | DF_SKINTEAMS)))
				|| coop->value) && OnSameTeam(tr.ent, ent)))
				return;

			// add some random damage, damage range from 8 to 20.
			randmodify = rand() % 13 + 1;
			damage += randmodify;
			// modify kick by damage
			kick += (randmodify * 10);

			// reduce damage, if he tries to punch within or out of water
			if (ent->waterlevel)
				damage -= rand() % 10 + 1;
			// reduce kick, if victim is in water
			if (tr.ent->waterlevel)
				kick /= 2;

			T_Damage(tr.ent, ent, ent, forward, tr.endpos, tr.plane.normal,
				damage, kick, 0, MOD_PUNCH);
			gi.sound(ent, CHAN_WEAPON, gi.soundindex("weapons/kick.wav"), 1, ATTN_NORM, 0);
			PlayerNoise(ent, ent->s.origin, PNOISE_SELF);

			//only hit weapon out of hand if damage >= 15
			if (tr.ent->client && (tr.ent->client->curr_weap == M4_NUM
				|| tr.ent->client->curr_weap == MP5_NUM
				|| tr.ent->client->curr_weap == M3_NUM
				|| tr.ent->client->curr_weap == SNIPER_NUM
				|| tr.ent->client->curr_weap == HC_NUM) && damage >= 15)
			{
				DropSpecialWeapon(tr.ent);
				gi.cprintf (ent, PRINT_HIGH, "You hit %s's %s from %s hands!\n",
					tr.ent->client->pers.netname,(tr.ent->client->pers.weapon)->pickup_name, IsFemale(tr.ent) ? "her" : "his");

				gi.cprintf(tr.ent, PRINT_HIGH, "%s hit your weapon from your hands!\n",
					ent->client->pers.netname);
			}
			return;
		}
	}
	gi.sound(ent, CHAN_WEAPON, gi.soundindex("weapons/swish.wav"), 1, ATTN_NORM, 0);

	// animate the punch
	// can't animate a punch when ducked
	if (ent->client->ps.pmove.pm_flags & PMF_DUCKED)
		return;
	if (ent->client->anim_priority >= ANIM_WAVE)
		return;

	ent->client->anim_priority = ANIM_WAVE;
	ent->s.frame = FRAME_flip01 - 1;
	ent->client->anim_end = FRAME_flip03;
	//TempFile - END
}

//"colours" a normal string to green - for output via cprintf() without using
//PRINT_CHAT - got it by Riviera
char *strtostr2(char *s)
{
	unsigned char *p = s;

	while (*p) {
		if ((*p >= 0x1b && *p <= 0x7f)
		    || ((*p > 0x0a && *p <= 0x11) && (*p != 0x0d)))
			*p += 0x80;

		p++;
	}
	return (char *) s;
}

//plays a random sound/insane sound, insane1-9.wav
void PlayRandomInsaneSound(edict_t * ent)
{
	int n;
	char buffer[32];

	n = rand() % 9 + 1;
	Com_sprintf(buffer, sizeof(buffer), "insane/insane%i.wav", n);
	gi.sound(ent, CHAN_VOICE, gi.soundindex(buffer), 1, ATTN_NORM, 0);
}

//determe client entities that are within a cube.
//from: start entity
//p1,p2: cube description
//returns next client entity that is within cube
edict_t *findblock(edict_t * from, vec3_t p1, vec3_t p2)
{
	vec3_t eorg;
	int j;

	if (!from)
		from = g_edicts;
	else
		from++;

	for (j = 0; j < 3; j++)
		if (p2[j] < p1[j]) {
			eorg[1] = p2[j];
			p2[j] = p1[j];
			p1[j] = eorg[1];
		}

	for (; from < &g_edicts[globals.num_edicts]; from++) {
		if (!from->inuse)
			continue;
		if (from->solid == SOLID_NOT)
			continue;
		for (j = 0; j < 3; j++)
			eorg[j] = from->s.origin[j] + (from->mins[j] + from->maxs[j]) * 0.5;
		if (eorg[0] >= p1[0] && eorg[1] >= p1[1] && eorg[2] >= p1[2] &&
		    eorg[0] <= p2[0] && eorg[1] <= p2[1] && eorg[2] <= p2[2])
			return from;
	}

	return NULL;
}

//Precaches and enables download options for user sounds. All sounds
//have to be listed within "sndlist.ini". called from g_spawn.c -> SP_worldspawn
void PrecacheUserSounds(void)
{
	int i, bs;
	FILE *soundlist;
	char buf[MAX_STR_LEN], fullpath[MAX_STR_LEN];

	soundlist = fopen(GAMEVERSION "/sndlist.ini", "r");
	if (soundlist == NULL) {
		// no "sndlist.ini" file...
		gi.dprintf("Cannot load %s, sound download is disabled.\n", GAMEVERSION "/sndlist.ini");
	} else {
		// read the sndlist.ini file
		for (i = 0; (fgets(buf, MAX_STR_LEN - 10, soundlist) != NULL) && (i < 100);) {
			//first remove trailing spaces
			for (bs = strlen(buf);
			     bs > 0 && (buf[bs - 1] == '\r' || buf[bs - 1] == '\n' || buf[bs - 1] == ' '); bs--)
				buf[bs - 1] = '\0';
			//Comments are marked with # or // at line start
			if (bs > 0 && strncmp(buf, "#", 1) != 0 && strncmp(buf, "//", 2) != 0) {
				strcpy(fullpath, PG_SNDPATH);
				strcat(fullpath, buf);
				gi.soundindex(fullpath);
				//gi.dprintf("Sound %s: precache %i",fullpath, gi.soundindex(fullpath)); 
				i++;
			}
		}
		fclose(soundlist);
		if (!i)
			gi.dprintf("%s is empty, no sounds to precache.\n", GAMEVERSION "/sndlist.ini");
		else
			gi.dprintf("%i user sounds precached.\n", i);
	}
}

//Returns the edict for the given name, not case sensitive, and
//only if the edict is a client.
edict_t *FindClientByPersName(char *name)
{
	int i;
	edict_t *other;

	for (i = 1; i <= game.maxclients; i++) {
		other = &g_edicts[i];
		if (other && other->client && (Q_stricmp(other->client->pers.netname, name) == 0)) {
			return other;
		}
	}
	return NULL;
}

//Returns the internal edict # of a client 
//Attention: you must decrement it to get the right one, 0 means not found!
int GetClientInternalNumber(edict_t * ent)
{
	int i;

	if (!ent || !ent->client)
		return 0;

	for (i = 1; i <= game.maxclients; i++) {
		if (&g_edicts[i] == ent)
			return i;
	}

	return 0;
}

/*
   Kicks the given (client) edict out of the server, reason will be printed before
*/
void KickClient(edict_t * target, char *reason)
{
	int j;
	char buffer[32];

	if (target && target->client && target->inuse) {
		j = GetClientInternalNumber(target);
		if (j) {
			Com_sprintf(buffer, sizeof(buffer), "kick %i\n", --j);
			gi.bprintf(PRINT_HIGH, "%s has to be KICKED from the server.\n", target->client->pers.netname);
			gi.bprintf(PRINT_MEDIUM, "Reason: %s\n", reason);
			gi.AddCommandString(buffer);
		} else
			gi.dprintf("Error: %s has to be kicked, cannot determine userid.",
				   target->client->pers.netname);
	}
}

//
char *StripSpaces(char *astring)
{
	char *p, buf[1024];

	p = astring;
	if (!p || !*p)
		return astring;

	while (*p != '\0' && (*p == ' ' || *p == '\r' || *p == '\n' || *p == '\t'))
		p++;
	if (!*p) {
		*astring = '\0';
		return astring;
	}

	strcpy(buf, p);
	p = &buf[strlen(buf) - 1];

	while (p != buf && (*p == ' ' || *p == '\r' || *p == '\n' || *p == '\t'))
		p--;
	p++;
	*p = '\0';
	strcpy(astring, buf);
	return astring;
}

//
qboolean CheckForRemark(char *src)
{
	char *myptr;

	myptr = StripSpaces(src);
	if (strlen(myptr) == 0)
		return (true);
	if (*myptr == '#')
		return (true);
	if (strlen(myptr) > 1 && *myptr == '/') {
		myptr++;
		return (*myptr == '/');
	}

	return false;
}

//
qboolean ParseStartFile(char *filename, parse_t * parse)
{
	if (parse->inparsing == true)
		return false;
	parse->pfile = fopen(filename, "r");
	if (parse->pfile) {
		parse->inparsing = true;
		parse->nextline = true;
		parse->lnumber = 0;
		return true;
	} else
		gi.dprintf("Error opening file %s\n", filename);
	return false;
}

//
void ParseEndFile(parse_t * parse)
{
	if (parse->pfile) {
		fclose(parse->pfile);
		parse->pfile = NULL;
		parse->inparsing = false;
		parse->nextline = false;
		parse->lnumber = 0;
	}
}

//due the usage of strtok, only ONE file should be parsed at
//the same time...this may change in the future.
char *ParseNextToken(parse_t * parse, char *seperator)
{
	char *dummy;

	if (parse->inparsing == true) {
		dummy = NULL;
		while (!dummy) {
			if (parse->nextline == true) {
				do {
					dummy = fgets(parse->cline, 1020, parse->pfile);
					parse->lnumber++;
					if (!dummy)	//we reached end of file
						return NULL;
				}
				while (CheckForRemark(parse->cline));
				dummy = strtok(parse->cline, seperator);
				if (dummy)	//this should always happen :)
					parse->nextline = false;
			} else {
				dummy = strtok(NULL, seperator);
				if (!dummy)	//no more tokens in current line
					parse->nextline = true;
			}
		}
		//we finally got a new token
		strcpy(parse->ctoken, dummy);
		StripSpaces(parse->ctoken);
		return (parse->ctoken);
	}
	return NULL;
}

// internal use only, ini section must occupy one line
// and has the format "[section]". This may be altered
// Sections are case insensitive.
qboolean _seekinisection(FILE * afile, char *section)
{
	inistr buf, comp;

	if (!afile)
		return false;
	sprintf(comp, "[%s]", section);
	fseek(afile, 0, SEEK_SET);
	while (fgets(buf, INI_STR_LEN, afile)) {
		if (Q_stricmp(StripSpaces(buf), comp) == 0)
			return true;
	}
	return false;
}

// internal use only, make sure sizeof(value) >= INI_STR_LEN
char *_readinivalue(FILE * afile, char *section, char *key, char *value)
{
	inistr buf, akey;
	char *p;

	if (!afile)
		return NULL;

	if (_seekinisection(afile, section) == true) {
		while (fgets(buf, INI_STR_LEN, afile)) {
			//splitting line into key and value
			p = strrchr(buf, '=');
			if (p) {
				*p = '\0';
				p++;
				strcpy(akey, buf);
				strcpy(value, p);
			} else {
				strcpy(akey, buf);
				value[0] = '\0';
			}
			if (Q_stricmp(StripSpaces(akey), key) == 0)
				return (StripSpaces(value));	// found!
			if (akey[0] == '[')
				return NULL;	// another section begins
		}
	}
	return NULL;
}

int ReadIniSection(ini_t * ini, char *section, char buf[][INI_STR_LEN], int maxarraylen)
{
	int i;
	char *p;

	i = 0;
	if (_seekinisection(ini->pfile, section) == true) {
		while (i < maxarraylen && fgets(buf[i], 127, ini->pfile)) {
			buf[i][127] = '\0';
			p = buf[i];
			StripSpaces(p);
			if (*p == '[')	// next section?
			{
				*p = '\0';
				return i;
			}
			i++;
		}
	}
	return i;
}

char *ReadIniStr(ini_t * ini, char *section, char *key, char *value, char *defvalue)
{
	if (!_readinivalue(ini->pfile, section, key, value))
		strcpy(value, defvalue);
	return value;
}

int ReadIniInt(ini_t * ini, char *section, char *key, int defvalue)
{
	inistr value;

	if (_readinivalue(ini->pfile, section, key, value))
		return atoi(value);
	return defvalue;
}

char *IniPath(void)
{
	cvar_t *p;

	if (!*_inipath) {
		p = gi.cvar("ininame", "action.ini", 0);
		if (p->string && *(p->string))
			sprintf(_inipath, "%s/%s", GAMEVERSION, p->string);
		else
			sprintf(_inipath, "%s/%s", GAMEVERSION, "action.ini");
	}
	return _inipath;
}
