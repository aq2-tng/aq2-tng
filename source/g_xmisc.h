//-----------------------------------------------------------------------------
// PG BUND
// g_xmisc.h
//
// header file for g_xmisc.c
//
// $Id: g_xmisc.h,v 1.2 2001/09/28 13:48:34 ra Exp $
//
//-----------------------------------------------------------------------------
// $Log: g_xmisc.h,v $
// Revision 1.2  2001/09/28 13:48:34  ra
// I ran indent over the sources. All .c and .h files reindented.
//
// Revision 1.1.1.1  2001/05/06 17:25:24  igor_rock
// This is the PG Bund Edition V1.25 with all stuff laying around here...
//
//-----------------------------------------------------------------------------

typedef struct
{
  FILE *pfile;
  qboolean inparsing;
  qboolean nextline;
  char cline[1024];
  char ctoken[1024];
  int lnumber;
}
parse_t;


// misc functions
char *strtostr2 (char *s);
void PlayRandomInsaneSound (edict_t * ent);
edict_t *findblock (edict_t * from, vec3_t p1, vec3_t p2);
void KickClient (edict_t * target, char *reason);
int GetClientInternalNumber (edict_t * ent);
edict_t *FindClientByPersName (char *name);

// ini functions
#define ini_t parse_t
#define OpenIniFile(a,b) ParseStartFile(a,b)
#define CloseIniFile(a) ParseEndFile(a)
#define INI_STR_LEN 128
typedef char inistr[INI_STR_LEN];
typedef inistr inisection[];

char *IniPath (void);
int ReadIniSection (ini_t * ini, char *section, char buf[][INI_STR_LEN],
		    int maxarraylen);
char *ReadIniStr (ini_t * ini, char *section, char *key, char *value,
		  char *defvalue);
int ReadIniInt (ini_t * ini, char *section, char *key, int defvalue);

// parse functions
qboolean ParseStartFile (char *filename, parse_t * parse);
void ParseEndFile (parse_t * parse);
char *ParseNextToken (parse_t * parse, char *seperator);

// functions that do not really belong here
void PrecacheUserSounds (void);
void punch_attack (edict_t * ent);


// needed prototypes from other sources 
// from p_weapon.c for punch_attack():
void P_ProjectSource (gclient_t * client, vec3_t point, vec3_t distance,
		      vec3_t forward, vec3_t right, vec3_t result);
