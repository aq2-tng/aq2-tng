//-----------------------------------------------------------------------------
// PG BUND
// a_xgame.c
//
// this module contains all new and changed functions from a_game.c
//
// REMARKS:
// --------
// 1.   You have to comment the original ParseSayText in
// a_game.c completly out.
// 2.   Look for the DELETEIT comment. All comments 
// regarding DELETEIT are *not* really neccesary.
// They were done because of my compiler (caused
// compiling errors), and not because of functionality!
// Try first to de-comment the DELETEIT sections, if
// you get compiler errors too, comment them out like
// I'd done.
//
// $Id: a_xgame.c,v 1.14 2001/12/24 17:27:58 slicerdw Exp $
//
//-----------------------------------------------------------------------------
// $Log: a_xgame.c,v $
// Revision 1.14  2001/12/24 17:27:58  slicerdw
// Added check for gl_dynamic
//
// Revision 1.13  2001/12/09 14:02:11  slicerdw
// Added gl_clear check -> video_check_glclear cvar
//
// Revision 1.12  2001/09/28 13:48:34  ra
// I ran indent over the sources. All .c and .h files reindented.
//
// Revision 1.11  2001/07/16 19:02:06  ra
// Fixed compilerwarnings (-g -Wall).  Only one remains.
//
// Revision 1.10  2001/06/25 11:44:47  slicerdw
// New Video Check System - video_check and video_check_lockpvs no longer latched
//
// Revision 1.9  2001/06/21 00:05:30  slicerdw
// New Video Check System done -  might need some revision but works..
//
// Revision 1.6  2001/06/19 18:56:38  deathwatch
// New Last killed target system
//
// Revision 1.5  2001/05/20 15:00:19  slicerdw
// Some minor fixes and changings on Video Checking system
//
// Revision 1.4  2001/05/11 12:21:18  slicerdw
// Commented old Location support ( ADF ) With the ML/ETE Compatible one
//
// Revision 1.2  2001/05/07 21:18:34  slicerdw
// Added Video Checking System
//
// Revision 1.1.1.1  2001/05/06 17:25:24  igor_rock
// This is the PG Bund Edition V1.25 with all stuff laying around here...
//
//-----------------------------------------------------------------------------

#include "g_local.h"

//the whole map description
//mapdesc_t mapdesc;

//TempFile BEGIN
//mapdescex_t mapdescex; AQ2:TNG - Slicer old location support, commented.

/* support for .pg dropped 7/26/1999
   int LoadPG(char *mapname)
   { 
   FILE *pntlist;
   char buf[1024];
   int i = 0;
   int j, bs;

   memset(&mapdesc, 0, sizeof(mapdesc));
   sprintf(buf, "%s/maps/%s%s", GAMEVERSION, mapname, PG_LOCEXT);
   pntlist = fopen(buf, "r");
   if (pntlist == NULL)
   {
   gi.dprintf("Warning: No location file for map %s\n", mapname);
   return 0;
   }
   while(fgets(buf, 1000, pntlist) != NULL)
   {
   //first remove trailing spaces
   for (bs = strlen(buf);
   bs > 0 && (buf[bs-1] == '\r' || buf[bs-1] == '\n' || buf[bs-1] == ' ');
   bs--)
   buf[bs-1] = '\0';

   //check if it's a valid line
   if (bs > 0 && strncmp(buf, "#", 1) != 0 && strncmp(buf, "//", 2) != 0) 
   {
   //a little bit dirty... :)
   sscanf(buf, "%f %f %f", &mapdesc[i].pos[0], &mapdesc[i].pos[1], &mapdesc[i].pos[2]);
   j = 0;
   for (bs = 0; bs <= strlen(buf) && j < 3; bs++) if (buf[bs] == ' ') j++;
   j = 0;
   while ((bs < strlen(buf)) && (j < LOC_STR_LEN-1)) mapdesc[i].desc[j++] = buf[bs++];
   mapdesc[i].desc[LOC_STR_LEN-1] = '\0';

   //max points reached?
   if (++i >= LOC_MAX_POINTS) 
   {
   gi.dprintf("Warning: More than %i locations.\n", LOC_MAX_POINTS);
   break;
   }
   }
   }
   return i;
   } */

//initializes mapdesc, read a location file if available
// modified by TempFile to support ADF format, see adf.txt

//TempFile
//FixCubeData() ensures that lower left is actually in the lower left
//Called in DescListInit() and Cmd_AddCube_f()

//AQ2:TNG Slicer commenting old location file support ( ADF )
/*
int num_loccubes = 0;

void
FixCubeData (loccube_t * cube)
{
  float tmp;
  int i;
  char *dimensions[3] =
  {"X", "Y", "Z"};

  for (i = 0; i < 3; i++)
    {
      if (cube->lowerleft[i] > cube->upperright[i])
	{			//flip the two values

	  tmp = cube->lowerleft[i];
	  cube->lowerleft[i] = cube->upperright[i];
	  cube->upperright[i] = tmp;
	}
      else if (cube->lowerleft[i] == cube->upperright[i])
	{
	  //an infinitely thin cube - no, that's not our style.
	  cube->lowerleft[i] -= 10;

	  //print a warning, those corrupted files aren't good.
	  gi.dprintf ("WARNING: Infinitely small %s dimension in area cube %s detected.\n"
		      "LOWERLEFT value decremented by 10.\n",
		      dimensions[i], cube->desc);
	}
    }
}

void
DescListInit (char *mapname)
{
  FILE *pntlist;
  char buf[1024];
  int i = 0;
  int j, bs;
  int num = 0;

  num_loccubes = 0;
  memset (&mapdescex, 0, sizeof (mapdescex));
  sprintf (buf, "%s/location/%s%s", GAMEVERSION, mapname, PG_LOCEXTEX);
  pntlist = fopen (buf, "r");
  if (!pntlist)
    {
      gi.dprintf ("Warning: No area definition file for map %s.\n", mapname);
      return;
    }

  while (fgets (buf, 1000, pntlist) != NULL)
    {
      //first remove trailing spaces
      for (bs = strlen (buf);
	   bs > 0 && (buf[bs - 1] == '\r' || buf[bs - 1] == '\n' || buf[bs - 1] == ' ');
	   bs--)
	buf[bs - 1] = '\0';

      //check if it's a valid line
      if (bs > 0 && strncmp (buf, "#", 1) != 0 && strncmp (buf, "//", 2) != 0)
	{
	  //a little bit dirty... :)
	  sscanf (buf, "<%f %f %f> <%f %f %f>", &mapdescex[i].lowerleft[0], &mapdescex[i].lowerleft[1],
		  &mapdescex[i].lowerleft[2], &mapdescex[i].upperright[0], &mapdescex[i].upperright[1],
		  &mapdescex[i].upperright[2]);
	  j = 0;
	  for (bs = 0; bs <= strlen (buf) && j < 6; bs++)
	    if (buf[bs] == ' ')
	      j++;
	  j = 0;
	  while ((bs < strlen (buf)) && (j < LOC_STR_LEN - 1))
	    mapdescex[i].desc[j++] = buf[bs++];
	  mapdescex[i].desc[LOC_STR_LEN - 1] = '\0';

	  FixCubeData (&(mapdescex[i]));

	  //max points reached?
	  if (++i >= LOC_MAX_POINTS)
	    {
	      gi.dprintf ("Warning: More than %i locations.\n", LOC_MAX_POINTS);
	      break;
	    }
	}
    }

  if (!num)
    num = num_loccubes = i;
  fclose (pntlist);
  gi.dprintf ("%i map location%s read.\n", num, (num != 1) ? "s" : "");
}

//returns wether mapdesc is empty or not
qboolean
DescListIsEmpty (void)
{
  //if(using_mapdescex)   //TempFile
  if (!*mapdescex[0].desc)
    return true;

  //if (!*mapdesc[0].desc) return true;

  return false;
}

//returns nearest description point. if < 0, no point is available.
//before calling this function, check if mapdesc is empty via DescListIsEmpty().
//AQ:TNG - JBravo changed next line not to be a C style comment.
//         If this code is ever uncommented this must be fixed.
/ *      see new function DescListCube()
   int DescListNearestPoint(vec3_t origin, vec_t *distance)
   {
   int i,j;
   vec_t dist, lowest;
   vec3_t line;

   j = -1;
   lowest = 10000.0;
   for (i = 0; i < LOC_MAX_POINTS; i++)
   {
   //any point without desc will not be recognized
   if (*mapdesc[i].desc) 
   {
   VectorSubtract(mapdesc[i].pos, origin, line);
   dist = VectorLength(line);
   if (dist < lowest)
   {
   j = i;
   lowest = dist;
   }
   }
   }
   if (j >= 0) *distance = dist;
   else *distance = 0.0;
   return j;
   } 

//returns the index of the first found cube a point is located in.
//if < 0, no cube was found.

int
DescListCube (vec3_t origin)
{
  int i;
  loccube_t *cube;

  for (i = 0; i < num_loccubes; i++)
    {
      cube = &(mapdescex[i]);
      if ((cube->lowerleft[0] < origin[0]) && (cube->lowerleft[1] < origin[1]) &&
	  (cube->lowerleft[2] < origin[2]) &&
	  (cube->upperright[0] >= origin[0]) && (cube->upperright[1] >= origin[1]) &&
	  (cube->upperright[2] >= origin[2]))
	return i;
    }
  return -1;
}


//if a description is available, it'll be copied to buf and true is returned
//else false is returned and buf remains unchanged.
qboolean
GetPositionText (vec3_t origin, char *buf)
{
  int i;
  char firstword[64];

  buf[0] = 0;			// just for safety

  if (DescListIsEmpty () == false)
    {
      //i = DescListNearestPoint(origin, &dist);
      i = DescListCube (origin);
      if (i >= 0)
	{
	  //TempFile - cool preposition detection
	  sscanf (mapdescex[i].desc, "%s", firstword);

	  if (Q_stricmp (firstword, "near") &&
	      Q_stricmp (firstword, "at") &&
	      Q_stricmp (firstword, "by") &&
	      Q_stricmp (firstword, "close") &&
	      Q_stricmp (firstword, "over") &&
	      Q_stricmp (firstword, "under") &&
	      Q_stricmp (firstword, "above") &&
	      Q_stricmp (firstword, "inside") &&
	      Q_stricmp (firstword, "outside") &&
	      Q_stricmp (firstword, "in") &&	// also "in front of..."
	       Q_stricmp (firstword, "next") &&		// "next to..."
	       Q_stricmp (firstword, "behind") &&
	      Q_stricmp (firstword, "on") &&
	      Q_stricmp (firstword, "down") &&
	      Q_stricmp (firstword, "up"))	// enough?

	    strcpy (buf, "near ");

//AQ:TNG - JBravo changed next line not to be a C style comment.
//         If this code is ever uncommented this must be fixed.
	  / *if(abs((int)(origin[2] - mapdesc[i].pos[2])) > 350)   // TF - z limit to avoid level confusion
	     return false;
		// removed, Z check is no longer needed with cubes

	  strcat (buf, mapdescex[i].desc);
	  return true;
	}
    }
  return false;
}

// TempFile END
AQ2:TNG END
*/

// DetermineViewedEnemy: determine the current player you're viewing (only looks for live Enemy)
// Modified from DetermineViewedTeammate (which is used in a_radio.c)
edict_t *
DetermineViewedEnemy (edict_t * ent)
{
  vec3_t forward, dir;
  trace_t tr;
  edict_t *who, *best;
  float bd = 0, d;
  int i;

  AngleVectors (ent->client->v_angle, forward, NULL, NULL);
  VectorScale (forward, 8192, forward);
  VectorAdd (ent->s.origin, forward, forward);
  PRETRACE ();
  tr = gi.trace (ent->s.origin, NULL, NULL, forward, ent, MASK_SOLID);
  POSTTRACE ();
  if (tr.fraction < 1 && tr.ent && tr.ent->client)
    {
      return NULL;
    }

  AngleVectors (ent->client->v_angle, forward, NULL, NULL);
  best = NULL;
  for (i = 1; i <= maxclients->value; i++)
    {
      who = g_edicts + i;
      if (!who->inuse)
	continue;
      VectorSubtract (who->s.origin, ent->s.origin, dir);
      VectorNormalize (dir);
      d = DotProduct (forward, dir);
      if (d > bd && loc_CanSee (ent, who) &&
	  who->solid != SOLID_NOT &&
	  who->deadflag != DEAD_DEAD && !OnSameTeam (who, ent))
	{
	  bd = d;
	  best = who;
	}
    }

  if (bd > 0.90)
    {
      return best;
    }

  return NULL;
}

//AQ2:TNG Slicer old location support ( ADF )
/*
void
GetViewedPosition (edict_t * ent, char *buf)
{
  vec3_t forward, rel_pos;
  int rel_xy_pos;
  trace_t tr;

  AngleVectors (ent->client->v_angle, forward, NULL, NULL);
  VectorScale (forward, 8192, forward);
  VectorAdd (ent->s.origin, forward, forward);
  PRETRACE ();
  tr = gi.trace (ent->s.origin, NULL, NULL, forward, ent, MASK_ALL);
  POSTTRACE ();
  if (tr.fraction >= 1.0)
    {
      //gi.cprintf(ent, PRINT_HIGH, "GetViewedPosition: fraction >= 1.0\n");
      strcpy (buf, "somewhere");
      return;
    }
  if (GetPositionText (tr.endpos, buf) == false)
    {
      //creating relative vector from origin to destination
      VectorSubtract (ent->s.origin, tr.endpos, rel_pos);
      rel_xy_pos = 0;

      //checking bounds, if one direction is less than half the other, it may
      //be ignored...
      if (fabs (rel_pos[0]) > (fabs (rel_pos[1]) * 2))
	//x width (EAST, WEST) is twice greater than y width (NORTH, SOUTH)
	rel_pos[1] = 0.0;
      if (fabs (rel_pos[1]) > (fabs (rel_pos[0]) * 2))
	//y width (NORTH, SOUTH) is twice greater than x width (EAST, WEST)
	rel_pos[0] = 0.0;

      if (rel_pos[1] > 0.0)
	rel_xy_pos |= RP_NORTH;
      else if (rel_pos[1] < 0.0)
	rel_xy_pos |= RP_SOUTH;

      if (rel_pos[0] > 0.0)
	rel_xy_pos |= RP_EAST;
      else if (rel_pos[0] < 0.0)
	rel_xy_pos |= RP_WEST;

      //creating the text message, regarding to rel_xy_pos
      strcpy (buf, "in the ");
      if (rel_xy_pos & RP_NORTH)
	strcat (buf, "north");
      if (rel_xy_pos & RP_SOUTH)
	strcat (buf, "south");
      if (rel_xy_pos & RP_EAST)
	strcat (buf, "east");
      if (rel_xy_pos & RP_WEST)
	strcat (buf, "west");
      //gi.dprintf ("rel_xy_pos: %i\n", rel_xy_pos);
      //last but not least, the height, limit for up/down: 64
      if (fabs (rel_pos[2]) > 64.0)
	{
	  if (rel_pos[2] < 0.0)
	    strcat (buf, ", up");
	  else
	    strcat (buf, ", down");
	}
      else
	strcat (buf, ", same height");
    }
}


void
GetOwnPosition (edict_t * self, char *buf)
{
  if (GetPositionText (self->s.origin, buf) == false)
    {
      strcpy (buf, "somewhere");
    }
}

void
GetEnemyPosition (edict_t * self, char *buf)
{
  edict_t *the_enemy;
  vec3_t rel_pos;
  int rel_xy_pos;

  the_enemy = DetermineViewedEnemy (self);
  if (the_enemy && the_enemy->client)
    {
      if (GetPositionText (the_enemy->s.origin, buf) == false)
	{
	  //creating relative vector from origin to destination
	  VectorSubtract (self->s.origin, the_enemy->s.origin, rel_pos);

	  rel_xy_pos = 0;

	  //checking bounds, if one direction is less than half the other, it may
	  //be ignored...
	  if (fabs (rel_pos[0]) > (fabs (rel_pos[1]) * 2))
	    //x width (EAST, WEST) is twice greater than y width (NORTH, SOUTH)
	    rel_pos[1] = 0.0;
	  if (fabs (rel_pos[1]) > (fabs (rel_pos[0]) * 2))
	    //y width (NORTH, SOUTH) is twice greater than x width (EAST, WEST)
	    rel_pos[0] = 0.0;

	  if (rel_pos[1] > 0.0)
	    rel_xy_pos |= RP_NORTH;
	  else if (rel_pos[1] < 0.0)
	    rel_xy_pos |= RP_SOUTH;
	  if (rel_pos[0] > 0.0)
	    rel_xy_pos |= RP_EAST;
	  else if (rel_pos[0] < 0.0)
	    rel_xy_pos |= RP_WEST;

	  //creating the text message, regarding to rel_xy_pos
	  strcpy (buf, "in the ");
	  if (rel_xy_pos & RP_NORTH)
	    strcat (buf, "north");
	  if (rel_xy_pos & RP_SOUTH)
	    strcat (buf, "south");
	  if (rel_xy_pos & RP_EAST)
	    strcat (buf, "east");
	  if (rel_xy_pos & RP_WEST)
	    strcat (buf, "west");
	  //gi.dprintf ("rel_xy_pos: %i\n", rel_xy_pos);
	  //last but not least, the height of enemy, limit for up/down: 64
	  if (fabs (rel_pos[2]) > 64.0)
	    {
	      if (rel_pos[2] < 0.0)
		strcat (buf, ", above me");
	      else
		strcat (buf, ", under me");
	    }
	  else
	    strcat (buf, ", on the same level");

	}
    }
  else
    {
      strcpy (buf, "somewhere");
    }
}
*/
//AQ2:TNG END
void
GetViewedEnemyName (edict_t * self, char *buf)
{
  edict_t *the_enemy;

  the_enemy = DetermineViewedEnemy (self);
  if (the_enemy && the_enemy->client)
    {
      strcpy (buf, the_enemy->client->pers.netname);
    }
  else
    {
      strcpy (buf, "no enemy");
    }
}

void
GetViewedEnemyWeapon (edict_t * self, char *buf)
{
  edict_t *the_enemy;

  the_enemy = DetermineViewedEnemy (self);
  if (the_enemy && the_enemy->client)
    {
      strcpy (buf, the_enemy->client->pers.weapon->pickup_name);
    }
  else
    {
      strcpy (buf, "no weapon");
    }
}

//AQ2:TNG SLICER Old support
/*
void
GetLastKilledTarget (edict_t * self, char *buf)
{
  if (self->client->resp.last_killed_target)
    {
      strcpy (buf, self->client->resp.last_killed_target->client->pers.netname);
      //We want to report...ONCE! :)
      self->client->resp.last_killed_target = NULL;

    }
  else
    {
      strcpy (buf, "nobody");
    }
}
*/
//AQ2:TNG END

char *
SeekBufEnd (char *buf)
{
  while (*buf != 0)
    buf++;
  return buf;
}


void
ParseSayText (edict_t * ent, char *text)
{
  static unsigned char buf[10240], infobuf[10240];
  char *p, *pbuf;

  p = text;
  pbuf = buf;
  *pbuf = 0;

  while (*p != 0)
    {
      if (((ptrdiff_t) pbuf - (ptrdiff_t) buf) > 225)
	{
	  break;
	}

      if (*p == '%')
	{
	  switch (*(p + 1))
	    {
	    case 'H':
	      GetHealth (ent, infobuf);
	      strcpy (pbuf, infobuf);
	      pbuf = SeekBufEnd (pbuf);
	      p += 2;
	      continue;
	    case 'A':
	      GetAmmo (ent, infobuf);
	      strcpy (pbuf, infobuf);
	      pbuf = SeekBufEnd (pbuf);
	      p += 2;
	      continue;
	    case 'W':
	      GetWeaponName (ent, infobuf);
	      strcpy (pbuf, infobuf);
	      pbuf = SeekBufEnd (pbuf);
	      p += 2;
	      continue;
	    case 'I':
	      GetItemName (ent, infobuf);
	      strcpy (pbuf, infobuf);
	      pbuf = SeekBufEnd (pbuf);
	      p += 2;
	      continue;
	    case 'T':
	      GetNearbyTeammates (ent, infobuf);
	      strcpy (pbuf, infobuf);
	      pbuf = SeekBufEnd (pbuf);
	      p += 2;
	      continue;
	    case 'E':
	      GetViewedEnemyName (ent, infobuf);
	      strcpy (pbuf, infobuf);
	      pbuf = SeekBufEnd (pbuf);
	      p += 2;
	      continue;
	    case 'F':
	      GetViewedEnemyWeapon (ent, infobuf);
	      strcpy (pbuf, infobuf);
	      pbuf = SeekBufEnd (pbuf);
	      p += 2;
	      continue;
	    case 'G':
	      GetEnemyPosition (ent, infobuf);
	      strcpy (pbuf, infobuf);
	      pbuf = SeekBufEnd (pbuf);
	      p += 2;
	      continue;
	    case 'K':
	      GetLastKilledTarget (ent, infobuf);
	      strcpy (pbuf, infobuf);
	      pbuf = SeekBufEnd (pbuf);
	      p += 2;
	      continue;
//AQ2:TNG Slicer - New Location Code
	      /*
	         case 'L':
	         GetOwnPosition (ent, infobuf);
	         strcpy (pbuf, infobuf);
	         pbuf = SeekBufEnd (pbuf);
	         p += 2;
	         continue;
	         case 'S':
	         GetViewedPosition (ent, infobuf);
	         strcpy (pbuf, infobuf);
	         pbuf = SeekBufEnd (pbuf);
	         p += 2;
	         continue;
	       */
	    case 'S':
	      GetSightedLocation (ent, infobuf);
	      strcpy (pbuf, infobuf);
	      pbuf = SeekBufEnd (pbuf);
	      p += 2;
	      continue;
	    case 'L':
	      GetPlayerLocation (ent, infobuf);
	      strcpy (pbuf, infobuf);
	      pbuf = SeekBufEnd (pbuf);
	      p += 2;
	      continue;
	      //AQ2:TNG Slicer Last Damage Location
	    case 'D':
	      GetLastDamagedPart (ent, infobuf);
	      strcpy (pbuf, infobuf);
	      pbuf = SeekBufEnd (pbuf);
	      p += 2;
	      continue;
	      //AQ2:TNG END
	    }
	}
      *pbuf++ = *p++;
    }

  *pbuf = 0;

  strncpy (text, buf, 225);
  text[225] = 0;		// in case it's 225

}

// AQ2:TNG - Slicer Video Checks

//float next_cheat_check;


/*
===========
VideoCheckClient


===========
*/
void
VideoCheckClient (edict_t * ent)
{
  if (!ent->client->resp.vidref)
    return;

  if (video_check_lockpvs->value) {
      if (ent->client->resp.gllockpvs != 0) {
		gi.cprintf (ent, PRINT_HIGH,
		      "This server does not allow using that value for gl_lockpvs, set it to '0'\n");
		gi.bprintf (PRINT_HIGH, "%s was using an illegal setting\n",
		      ent->client->pers.netname);
		 Kick_Client (ent);
		 return;
		}
  }
  if (video_check_glclear->value) {
      if (ent->client->resp.glclear != 0) {
		gi.cprintf (ent, PRINT_HIGH,
		      "This server does not allow using that value for gl_clear, set it to '0'\n");
		gi.bprintf (PRINT_HIGH, "%s was using an illegal setting\n",
		      ent->client->pers.netname);
		 Kick_Client (ent);
		 return;
		}
  }
   if (video_check_gldynamic->value) {
      if (ent->client->resp.gldynamic != 0) {
		gi.cprintf (ent, PRINT_HIGH,
		      "This server does not allow using that value for gl_dynamic, set it to '0'\n");
		gi.bprintf (PRINT_HIGH, "%s was using an illegal setting\n",
		      ent->client->pers.netname);
		 Kick_Client (ent);
		 return;
		}
  }
  //Starting Modulate checks

  if(video_check->value) {

  if (Q_stricmp (ent->client->resp.gldriver, "3dfxgl") == 0)
    {
      if (ent->client->resp.glmodulate > video_max_3dfx->value)
	{
	  gi.cprintf (ent, PRINT_HIGH,
		      "Your gl_modulate value is too high for this server. Max Allowed is %.1f\n",
		      video_max_3dfx->value);
	  gi.bprintf (PRINT_HIGH,
		      "%s is using a gl_modulated higher than allowed (%.1f)\n",
		      ent->client->pers.netname,
		      ent->client->resp.glmodulate);
	  Kick_Client (ent);
	  return;
	}
      return;
    }

  if (Q_stricmp (ent->client->resp.gldriver, "opengl32") == 0)
    {

      if (ent->client->resp.glmodulate > video_max_opengl->value)
	{
	  gi.cprintf (ent, PRINT_HIGH,
		      "Your gl_modulate value is too high for this server. Max Allowed is %.1f\n",
		      video_max_opengl->value);
	  gi.bprintf (PRINT_HIGH,
		      "%s is using a gl_modulate higher than allowed (%.1f)\n",
		      ent->client->pers.netname,
		      ent->client->resp.glmodulate);
	  Kick_Client (ent);
	  return;
	}
      return;
    }
  if (Q_stricmp (ent->client->resp.gldriver, "3dfxglam") == 0)
    {
      if (ent->client->resp.glmodulate > video_max_3dfxam->value)
	{
	  gi.cprintf (ent, PRINT_HIGH,
		      "Your gl_modulate value is too high for this server. Max Allowed is %.1f\n",
		      video_max_3dfxam->value);
	  gi.bprintf (PRINT_HIGH,
		      "%s is using a gl_modulate higher than allowed (%.1f)\n",
		      ent->client->pers.netname,
		      ent->client->resp.glmodulate);
	  Kick_Client (ent);
	  return;
	}
      return;
    }
  if (ent->client->resp.glmodulate > video_max_opengl->value)
    {
      gi.cprintf (ent, PRINT_HIGH,
		  "Your gl_modulate value is too high for this server. Max Allowed is %.1f\n",
		  video_max_opengl->value);
      gi.bprintf (PRINT_HIGH,
		  "%s is using a gl_modulate higher than allowed (%.1f)\n",
		  ent->client->pers.netname, ent->client->resp.glmodulate);
      Kick_Client (ent);
      return;
  }
  }
}

//AQ2:TNG - Slicer : Last Damage Location
void
GetLastDamagedPart (edict_t * self, char *buf)
{
  if (self->client->resp.last_damaged_part == 0)
    strcpy (buf, "nothing");
  else if (self->client->resp.last_damaged_part == LOC_HDAM)
    {
      strcpy (buf, "head");
      self->client->resp.last_damaged_part = 0;
    }
  else if (self->client->resp.last_damaged_part == LOC_CDAM)
    {
      strcpy (buf, "chest");
      self->client->resp.last_damaged_part = 0;
    }
  else if (self->client->resp.last_damaged_part == LOC_SDAM)
    {
      strcpy (buf, "stomach");
      self->client->resp.last_damaged_part = 0;
    }
  else if (self->client->resp.last_damaged_part == LOC_LDAM)
    {
      strcpy (buf, "legs");
      self->client->resp.last_damaged_part = 0;
    }
  else
    strcpy (buf, "nothing");	// Just in case
}

//AQ2:TNG END

// Gets the location string of a location (xo,yo,zo)
// Modifies the the location areas by value of "mod"
// in the coord-inside-area tests
//
qboolean
GetLocation (int xo, int yo, int zo, int mod, char *buf)
{
  int count;
  int lx, ly, lz, rlx, rly, rlz;

  count = ml_count;
  while (count--)
    {

      // get next location from location base
      lx = locationbase[count].x;
      ly = locationbase[count].y;
      lz = locationbase[count].z;
      rlx = locationbase[count].rx;
      rly = locationbase[count].ry;
      rlz = locationbase[count].rz;

      // Default X-range 1500
      if (!rlx)
	{
	  rlx = 1500;
	}

      // Default Y-range 1500
      if (!rly)
	{
	  rly = 1500;
	}

      // Test if the (xo,yo,zo) is inside the location
      if (xo >= lx - rlx - mod && xo <= lx + rlx - mod &&
	  yo >= ly - rly - mod && yo <= ly + rly - mod)
	{
	  if (rlz && (zo < lz - rlz - mod || zo > lz + rlz + mod))
	    continue;

	  strcpy (buf, locationbase[count].desc);

	  return true;
	}
    }

  strcpy (buf, "around");
  return false;
}

// Get the player location
//
qboolean
GetPlayerLocation (edict_t * self, char *buf)
{
  int xo, yo, zo;

  xo = self->s.origin[0];
  yo = self->s.origin[1];
  zo = self->s.origin[2];

  if (GetLocation (xo, yo, zo, 0, buf) == false)
    return false;
  else
    return true;
}

// Get the sighted location
//
void
GetSightedLocation (edict_t * self, char *buf)
{
  vec3_t start, forward, right, end, up, offset;
  int xo, yo, zo;
  trace_t tr;

  AngleVectors (self->client->v_angle, forward, right, up);

  VectorSet (offset, 24, 8, self->viewheight);
  P_ProjectSource (self->client, self->s.origin, offset, forward, right,
		   start);
  VectorMA (start, 8192, forward, end);

  PRETRACE ();
  tr =
    gi.trace (start, NULL, NULL, end, self,
	      CONTENTS_SOLID | CONTENTS_MONSTER | CONTENTS_DEADMONSTER);
  POSTTRACE ();

  xo = tr.endpos[0];
  yo = tr.endpos[1];
  zo = tr.endpos[2];

  GetLocation (xo, yo, zo, 10, buf);
}

void
GetEnemyPosition (edict_t * self, char *buf)
{
  edict_t *the_enemy;
  vec3_t rel_pos;
  int rel_xy_pos;

  the_enemy = DetermineViewedEnemy (self);
  if (the_enemy && the_enemy->client)
    {
      if (GetPlayerLocation (the_enemy, buf) == false)
	{
	  //creating relative vector from origin to destination
	  VectorSubtract (self->s.origin, the_enemy->s.origin, rel_pos);

	  rel_xy_pos = 0;

	  //checking bounds, if one direction is less than half the other, it may
	  //be ignored...
	  if (fabs (rel_pos[0]) > (fabs (rel_pos[1]) * 2))
	    //x width (EAST, WEST) is twice greater than y width (NORTH, SOUTH)
	    rel_pos[1] = 0.0;
	  if (fabs (rel_pos[1]) > (fabs (rel_pos[0]) * 2))
	    //y width (NORTH, SOUTH) is twice greater than x width (EAST, WEST)
	    rel_pos[0] = 0.0;

	  if (rel_pos[1] > 0.0)
	    rel_xy_pos |= RP_NORTH;
	  else if (rel_pos[1] < 0.0)
	    rel_xy_pos |= RP_SOUTH;
	  if (rel_pos[0] > 0.0)
	    rel_xy_pos |= RP_EAST;
	  else if (rel_pos[0] < 0.0)
	    rel_xy_pos |= RP_WEST;

	  //creating the text message, regarding to rel_xy_pos
	  strcpy (buf, "in the ");
	  if (rel_xy_pos & RP_NORTH)
	    strcat (buf, "north");
	  if (rel_xy_pos & RP_SOUTH)
	    strcat (buf, "south");
	  if (rel_xy_pos & RP_EAST)
	    strcat (buf, "east");
	  if (rel_xy_pos & RP_WEST)
	    strcat (buf, "west");
	  //gi.dprintf ("rel_xy_pos: %i\n", rel_xy_pos);
	  //last but not least, the height of enemy, limit for up/down: 64
	  if (fabs (rel_pos[2]) > 64.0)
	    {
	      if (rel_pos[2] < 0.0)
		strcat (buf, ", above me");
	      else
		strcat (buf, ", under me");
	    }
	  else
	    strcat (buf, ", on the same level");
	}
    }
  else
    {
      strcpy (buf, "somewhere");
    }
}

//AQ2:TNG END

//AQ2:TNG Slicer - New last killed target functions
void
ResetKills (edict_t * ent)
{
  int i;
  for (i = 0; i < MAX_LAST_KILLED; i++)
    {
      ent->client->resp.last_killed_target[i] = NULL;
    }
}
int
ReadKilledPlayers (edict_t * ent)
{
  int results;
  int i;

  results = -1;
  if (ent->client->resp.last_killed_target[0])
    {
      for (i = 0; i < MAX_LAST_KILLED; i++)
	{
	  if (ent->client->resp.last_killed_target[i])
	    {
	      results++;
	    }
	}
      return results;

    }
  else
    return -1;
}

void
AddKilledPlayer (edict_t * self, edict_t * ent)
{
  int kills;

  kills = ReadKilledPlayers (self);
  if (kills == MAX_LAST_KILLED || kills == -1)	// if list is full wite on the first one
    {
      self->client->resp.last_killed_target[0] = ent;
    }
  else
    {
      self->client->resp.last_killed_target[kills + 1] = ent;
    }

}
void
GetLastKilledTarget (edict_t * self, char *buf)
{
  int kills, i;

  kills = ReadKilledPlayers (self);

  if (kills >= 0)
    {
      strcpy (buf,
	      self->client->resp.last_killed_target[0]->client->pers.netname);
      if (kills > 0)
	{
	  for (i = 1; i < kills + 1; i++)
	    {
	      if (i == kills)
		{
		  strcat (buf, " and ");
		  strcat (buf,
			  self->client->resp.last_killed_target[i]->client->
			  pers.netname);
		}
	      else
		{
		  strcat (buf, ", ");
		  strcat (buf,
			  self->client->resp.last_killed_target[i]->client->
			  pers.netname);
		}
	    }
	}
      ResetKills (self);
    }
  else
    {
      strcpy (buf, "nobody");
    }
}
