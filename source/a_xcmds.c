//-----------------------------------------------------------------------------
// PG BUND
// a_xcmds.c
//
// contains all new non standard command functions
//
// $Id: a_xcmds.c,v 1.7 2001/09/28 13:44:23 slicerdw Exp $
//
//-----------------------------------------------------------------------------
// $Log: a_xcmds.c,v $
// Revision 1.7  2001/09/28 13:44:23  slicerdw
// Several Changes / improvements
//
// Revision 1.6  2001/08/15 14:50:48  slicerdw
// Added Flood protections to Radio & Voice, Fixed the sniper bug AGAIN
//
// Revision 1.5  2001/07/13 00:34:52  slicerdw
// Adjusted Punch command
//
// Revision 1.4  2001/05/31 16:58:14  igor_rock
// conflicts resolved
//
// Revision 1.3.2.2  2001/05/31 06:47:51  igor_rock
// - removed crash bug with non exisitng flag files
// - added new commands "setflag1", "setflag2" and "saveflags" to create
//   .flg files
//
// Revision 1.3.2.1  2001/05/25 18:59:52  igor_rock
// Added CTF Mode completly :)
// Support for .flg files is still missing, but with "real" CTF maps like
// tq2gtd1 the ctf works fine.
// (I hope that all other modes still work, just tested DM and teamplay)
//
// Revision 1.3  2001/05/13 01:23:01  deathwatch
// Added Single Barreled Handcannon mode, made the menus and scoreboards
// look nicer and made the voice command a bit less loud.
//
// Revision 1.2  2001/05/11 12:21:18  slicerdw
// Commented old Location support ( ADF ) With the ML/ETE Compatible one
//
// Revision 1.1.1.1  2001/05/06 17:25:16  igor_rock
// This is the PG Bund Edition V1.25 with all stuff laying around here...
//
//-----------------------------------------------------------------------------

#include "g_local.h"
#include "m_player.h"

//AQ2:TNG - Slicer Old Location support
//loccube_t *setcube = NULL;
//AQ2:TNG End

//
void
_Cmd_Rules_f (edict_t * self, char *argument)
{
  char section[1024], mbuf[4096], *p, buf[30][INI_STR_LEN];
  int i, j;
  ini_t ini;

  j = 0;
  strcpy (mbuf, "\n");
  if (*argument)
    strcpy (section, argument);
  else
    strcpy (section, "main");

  if (OpenIniFile (GAMEVERSION "/prules.ini", &ini))
    {
      i = ReadIniSection (&ini, section, buf, 30);
      while (j < i)
	{
	  p = buf[j++];
	  if (*p == '.')
	    p++;
	  strcat (mbuf, p);
	  strcat (mbuf, "\n");
	}
      CloseIniFile (&ini);
    }
  if (!j)
    gi.cprintf (self, PRINT_MEDIUM, "No rules on %s available\n", section);
  else
    gi.cprintf (self, PRINT_MEDIUM, mbuf);
}

void
Cmd_Rules_f (edict_t * self)
{
  char *s;

  s = gi.args ();
  _Cmd_Rules_f (self, s);
}

//
void
Cmd_Menu_f (edict_t * self)
{
  char *s;

  s = gi.args ();
  vShowMenu (self, s);
}

//
void
Cmd_Punch_f (edict_t * self)
{
  if (!use_punch->value 
	  ||self->deadflag == DEAD_DEAD
	  || self->solid == SOLID_NOT
	  || self->client->resp.sniper_mode != SNIPER_1X
	  || self->client->weaponstate != WEAPON_READY)
      return;

  // animation moved to punch_attack() in a_xgame.c
  // punch_attack is now called in ClientThink after evaluation punch_desired
  // for "no punch when firing" stuff - TempFile

  self->client->resp.punch_desired = true;
}

//Adds a point with name to location file - cheats must be enabled!
void
Cmd_Addpoint_f (edict_t * self)
{
  gi.cprintf (self, PRINT_MEDIUM, "\nLocation point feature was dropped in 1.20 and\n"
	 "replaced by location area cubes.\nSee readme.txt for details.\n");
  /*FILE *pntlist;
     char *s, buf[1024];

     s = gi.args();
     if (!*s)  
     { 
     gi.cprintf(self, PRINT_MEDIUM, "\nCommand needs argument, use addpoint <description>.\n");
     return;
     }
     sprintf(buf, "%s/maps/%s%s", GAMEVERSION, level.mapname, PG_LOCEXT);
     pntlist = fopen(buf, "a");
     if (pntlist == NULL)
     {
     gi.cprintf(self, PRINT_MEDIUM, "\nError accessing loc file %s.\n", buf);
     return;
     }
     sprintf(buf, "%.2f %.2f %.2f %s\n", self->s.origin[0], self->s.origin[1], self->s.origin[2], s);
     fputs(buf, pntlist);
     fclose(pntlist);
     gi.cprintf(self, PRINT_MEDIUM, "\nPoint added.\n"); */
}


//Plays a sound file
void
Cmd_Voice_f (edict_t * self)
{
  char *s;
  char fullpath[2048];
  qboolean d;
  s = gi.args ();
  //check if no sound is given
  if (!*s)
    {
      gi.cprintf (self, PRINT_MEDIUM, "\nCommand needs argument, use voice <soundfile.wav>.\n");
      return;
    }
  if (strlen (s) > 32)
    {
      gi.cprintf (self, PRINT_MEDIUM, "\nArgument is too long. Maximum length is 32 characters.\n");
      return;
    }
  if (strstr (s, ".."))
    {
      gi.cprintf (self, PRINT_MEDIUM, "\nArgument must not contain \"..\".\n");
    }
  //check if player is dead
  if (self->deadflag == DEAD_DEAD || self->solid == SOLID_NOT)
    return;
  strcpy (fullpath, PG_SNDPATH);
  strcat (fullpath, s);

    if(radio_repeat->value)
  {
	if((d = CheckForRepeat(self,s))==false)
		return;
  }
  if(radio_max->value)
  {
	if((d = CheckForFlood(self))==false)
		return;
  }
  // AQ2:TNG Deathwatch - This should be IDLE not NORM
  gi.sound (self, CHAN_VOICE, gi.soundindex (fullpath), 1, ATTN_IDLE, 0);
// AQ2:TNG END
}

//AQ2:TNG SLicer - Old location support
// TempFile - BEGIN
/*
void
Cmd_BeginCube_f (edict_t * self)
{
  if (setcube)
    {
      gi.cprintf (self, PRINT_MEDIUM, "There is already a cube allocated at 0x%p.\n",
		  (void *) setcube);
      return;
    }

  setcube = gi.TagMalloc (sizeof (loccube_t), TAG_GAME);
  if (!setcube)
    return;

  gi.cprintf (self, PRINT_MEDIUM, "New cube successfully allocated at 0x%p.\n", (void *) setcube);
  gi.cprintf (self, PRINT_MEDIUM, "Please set lower left and upper right corner.\n");
}

void
Cmd_SetCubeLL_f (edict_t * self)
{
  if (!setcube)
    {
      gi.cprintf (self, PRINT_MEDIUM, "Please allocate a cube first by executing BeginCube.\n");
      return;
    }

  memcpy (setcube->lowerleft, self->s.origin, sizeof (vec3_t));
  gi.cprintf (self, PRINT_MEDIUM, "Lower left has been set to <%.2f %.2f %.2f>.\n",
       setcube->lowerleft[0], setcube->lowerleft[1], setcube->lowerleft[2]);
}

void
Cmd_SetCubeUR_f (edict_t * self)
{
  if (!setcube)
    {
      gi.cprintf (self, PRINT_MEDIUM, "Please allocate a cube first by executing BeginCube.\n");
      return;
    }

  memcpy (setcube->upperright, self->s.origin, sizeof (vec3_t));
  gi.cprintf (self, PRINT_HIGH, "Upper right has been set to <%.2f %.2f %.2f>.\n",
    setcube->upperright[0], setcube->upperright[1], setcube->upperright[2]);
}

void
Cmd_AbortCube_f (edict_t * self)
{
  if (!setcube)
    {
      gi.cprintf (self, PRINT_MEDIUM, "No cube to deallocate.\n");
      return;
    }

  gi.TagFree (setcube);
  gi.cprintf (self, PRINT_MEDIUM, "Cube at 0x%p successfully deallocated.\n",
	      (void *) setcube);
  setcube = NULL;
}

//Adds a cube with name to location file - cheats must be enabled!
void
Cmd_AddCube_f (edict_t * self)
{
  FILE *pntlist;
  char *s, buf[1024];

  if (!setcube)
    {
      gi.cprintf (self, PRINT_MEDIUM, "\nPlease allocate a cube first by executing BeginCube.\n");
      return;
    }

  if (!setcube->lowerleft[0] || !setcube->lowerleft[1] || !setcube->lowerleft[2] ||
      !setcube->upperright[0] || !setcube->upperright[1] || !setcube->upperright[2])
    {
      gi.cprintf (self, PRINT_MEDIUM, "\nPlease set cube corners first using SetCubeLL and SetCubeUR.\n");
      return;
    }

  FixCubeData (setcube);
  s = gi.args ();
  strcpy (setcube->desc, s);

  if (!*s)
    {
      gi.cprintf (self, PRINT_MEDIUM, "\nCommand needs argument, use addcube <description>.\n");
      return;
    }
  sprintf (buf, "%s/location/%s%s", GAMEVERSION, level.mapname, PG_LOCEXTEX);
  pntlist = fopen (buf, "a");
  if (pntlist == NULL)
    {
      gi.cprintf (self, PRINT_MEDIUM, "\nError accessing adf file %s.\n", buf);
      return;
    }

  sprintf (buf, "<%.2f %.2f %.2f> <%.2f %.2f %.2f> %s\n",
	setcube->lowerleft[0], setcube->lowerleft[1], setcube->lowerleft[2],
	   setcube->upperright[0], setcube->upperright[1], setcube->upperright[2], setcube->desc);
  fputs (buf, pntlist);
  fclose (pntlist);
  memcpy (&mapdescex[num_loccubes], setcube, sizeof (*setcube));
  num_loccubes++;

  gi.TagFree (setcube);
  setcube = NULL;

  gi.cprintf (self, PRINT_MEDIUM, "\nCube added.\n");
}

void
Cmd_PrintCubeState_f (edict_t * self)
{
  if (!setcube)
    {
      gi.cprintf (self, PRINT_MEDIUM, "\nPlease allocate a cube first by executing BeginCube.\n");
      return;
    }

  gi.cprintf (self, PRINT_MEDIUM, "\nTemporary cube allocated at %p.\nLower left corner: "
	      "<%.2f %.2f %.2f>\nUpper right corner: <%.2f %.2f %.2f>\n", (void *) setcube,
	setcube->lowerleft[0], setcube->lowerleft[1], setcube->lowerleft[2],
    setcube->upperright[1], setcube->upperright[2], setcube->upperright[2]);
}


// TempFile - END
*/
//AQ2:TNG END

// Variables for new flags

char flagpos1[256] = { 0 };
char flagpos2[256] = { 0 };

//sets red flag position - cheats must be enabled!
void
Cmd_SetFlag1_f (edict_t * self)
{
     sprintf(flagpos1, "<%.2f %.2f %.2f>", self->s.origin[0], self->s.origin[1], self->s.origin[2]);
     gi.cprintf(self, PRINT_MEDIUM, "\nRed Flag added at %s.\n", flagpos1);
}

//sets blue flag position - cheats must be enabled!
void
Cmd_SetFlag2_f (edict_t * self)
{
     sprintf(flagpos2, "<%.2f %.2f %.2f>", self->s.origin[0], self->s.origin[1], self->s.origin[2]);
     gi.cprintf(self, PRINT_MEDIUM, "\nBlue Flag added at %s.\n", flagpos2);
}

//Save flag definition file - cheats must be enabled!
void
Cmd_SaveFlags_f (edict_t * self)
{
  FILE *fp;

  char buf[1024];

  if (!(flagpos1[0] && flagpos2[0])) 
    {
    gi.cprintf (self, PRINT_MEDIUM, "You only can save flag positions when you've already set them\n");
    return;
    }

  sprintf(buf, "%s/tng/%s.flg", GAMEVERSION, level.mapname);
  fp = fopen(buf, "w");
  if (fp == NULL)
    {
      gi.cprintf(self, PRINT_MEDIUM, "\nError accessing flg file %s.\n", buf);
      return;
    }
  sprintf (buf, "# %s\n", level.mapname);
  fputs(buf, fp);
  sprintf(buf, "%s\n", flagpos1);
  fputs(buf, fp);
  sprintf(buf, "%s\n", flagpos2);
  fputs(buf, fp);
  fclose(fp);

  flagpos1[0] = 0;
  flagpos2[0] = 0;

  gi.cprintf(self, PRINT_MEDIUM, "\nFlag File saved.\n");
}

