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

//
static char *StripSpaces(char *astring)
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
static qboolean CheckForRemark(char *src)
{
	char *myptr;

	myptr = StripSpaces(src);
	if (!myptr || !*myptr)
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
static qboolean _seekinisection(FILE * afile, char *section)
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
static char *_readinivalue(FILE * afile, char *section, char *key, char *value)
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
