//-----------------------------------------------------------------------------
// a_game.c
// General game code for Action (formerly Axshun).
//
// Zucchini (spikard@u.washington.edu) and Fireblade (ucs_brf@shsu.edu) 
// (splat/bullethole/shell ejection code from original Action source)
//
// $Id: a_game.c,v 1.3 2001/05/11 16:07:25 mort Exp $
//
//-----------------------------------------------------------------------------
// $Log: a_game.c,v $
// Revision 1.3  2001/05/11 16:07:25  mort
// Various CTF bits and pieces...
//
// Revision 1.2  2001/05/11 12:21:18  slicerdw
// Commented old Location support ( ADF ) With the ML/ETE Compatible one
//
// Revision 1.1.1.1  2001/05/06 17:24:23  igor_rock
// This is the PG Bund Edition V1.25 with all stuff laying around here...
//
//-----------------------------------------------------------------------------

#include "g_local.h"
#include "cgf_sfx_glass.h"

#define MAX_MAP_ROTATION        1000	// just in case...
#define MAX_STR_LEN             1000
#define MAX_TOTAL_MOTD_LINES    30

char team1_name[MAX_STR_LEN];
char team2_name[MAX_STR_LEN];
char team3_name[MAX_STR_LEN];
char team1_skin[MAX_STR_LEN];
char team2_skin[MAX_STR_LEN];
char team3_skin[MAX_STR_LEN];
char team1_skin_index[MAX_STR_LEN + 30];
char team2_skin_index[MAX_STR_LEN + 30];
char team3_skin_index[MAX_STR_LEN + 30];
char *map_rotation[MAX_MAP_ROTATION];
int num_maps, cur_map, num_allvotes;	// num_allvotes added by Igor[Rock]

char motd_lines[MAX_TOTAL_MOTD_LINES][70];
int motd_num_lines;

/*
 * ReadConfigFile()
 * Config file format is backwards compatible with Action's, but doesn't need 
 * the "###" designator at end of sections.
 * -Fireblade
 */
void
ReadConfigFile ()
{
  FILE *config_file;
  char buf[MAX_STR_LEN];
  char reading_section[MAX_STR_LEN];
  char inipath[MAX_STR_LEN];
  int lines_into_section = -1;
  cvar_t *ininame;

  ininame = gi.cvar ("ininame", "action.ini", 0);
  if (ininame->string && *(ininame->string))
    sprintf (inipath, "%s/%s", GAMEVERSION, ininame->string);
  else
    sprintf (inipath, "%s/%s", GAMEVERSION, "action.ini");

  config_file = fopen (inipath, "r");
  if (config_file == NULL)
    {
      gi.dprintf ("Unable to read %s\n", inipath);
      return;
    }

  while (fgets (buf, MAX_STR_LEN - 10, config_file) != NULL)
    {
      int bs;
      bs = strlen (buf);
      while (buf[bs - 1] == '\r' || buf[bs - 1] == '\n')
	{
	  buf[bs - 1] = 0;
	  bs--;
	}

      if ((buf[0] == '/' && buf[1] == '/') || buf[0] == 0)
	{
	  continue;
	}

      if (buf[0] == '[')
	{
	  char *p;
	  p = strchr (buf, ']');
	  if (p == NULL)
	    continue;
	  *p = 0;
	  strcpy (reading_section, buf + 1);
	  lines_into_section = 0;
	  continue;
	}
      if (buf[0] == '#' && buf[1] == '#' && buf[2] == '#')
	{
	  lines_into_section = -1;
	  continue;
	}
      if (lines_into_section > -1)
	{
	  if (!strcmp (reading_section, "team1"))
	    {
	      if (lines_into_section == 0)
		{
		  strcpy (team1_name, buf);
		}
	      else if (lines_into_section == 1)
		{
		  strcpy (team1_skin, buf);
		}
	    }
	  else if (!strcmp (reading_section, "team2"))
	    {
	      if (lines_into_section == 0)
		{
		  strcpy (team2_name, buf);
		}
	      else if (lines_into_section == 1)
		{
		  strcpy (team2_skin, buf);
		}
	    }
	  else if (!strcmp (reading_section, "team3"))
	    {
	      if (lines_into_section == 0)
		{
		  strcpy (team3_name, buf);
		}
	      else if (lines_into_section == 1)
		{
		  strcpy (team3_skin, buf);
		}
	    }
	  else if (!strcmp (reading_section, "maplist"))
	    {
	      map_rotation[num_maps] = (char *) gi.TagMalloc (strlen (buf) + 1, TAG_GAME);
	      strcpy (map_rotation[num_maps], buf);
	      num_maps++;
	    }
	  lines_into_section++;
	}
    }

  sprintf (team1_skin_index, "../players/%s_i", team1_skin);
  sprintf (team2_skin_index, "../players/%s_i", team2_skin);
  sprintf (team3_skin_index, "../players/%s_i", team3_skin);
  cur_map = 0;

  fclose (config_file);
}

void
ReadMOTDFile ()
{
  FILE *motd_file;
  char buf[1000];
  int lbuf;

  motd_file = fopen (GAMEVERSION "/motd.txt", "r");
  if (motd_file == NULL)
    return;

  motd_num_lines = 0;
  while (fgets (buf, 900, motd_file) != NULL)
    {
      lbuf = strlen (buf);
      while (buf[lbuf - 1] == '\r' || buf[lbuf - 1] == '\n')
	{
	  buf[lbuf - 1] = 0;
	  lbuf--;
	}

      if (lbuf > 40)
	buf[40] = 0;

      strcpy (motd_lines[motd_num_lines], buf);

      motd_num_lines++;

      if (motd_num_lines >= MAX_TOTAL_MOTD_LINES)
	break;
    }

  fclose (motd_file);
}

void
PrintMOTD (edict_t * ent)
{
  int mapnum, i, lines = 0;
  int max_lines = MAX_TOTAL_MOTD_LINES;
  char msg_buf[16384], *server_type;

  /* 
   * Welcome message
   */

  // 3 lines for version number & website (third blank)
  strcpy (msg_buf, "Welcome to AQ2:Edition v" ACTION_VERSION "\n"
	  "http://action.gametown.de/\n"
	  "\n");
  lines = 3;

  if (!skipmotd->value)
    {
      // Line for server hostname, if set
      if (hostname->string && strlen (hostname->string) && strcmp (hostname->string, "unnamed"))
	{
	  //strcat(msg_buf, "Running at: ");
	  strcat (msg_buf, hostname->string);
	  strcat (msg_buf, "\n");
	  lines++;
	}

      /* 
         * Server-specific settings information
       */

      // Line for game type
      if (teamplay->value)
	  {
	    if (use_3teams->value)
	    {
	      server_type = "3 team game";
	    }
		else if (ctf->value)
		{
		  server_type = "Capture the Flag"; // CTF
		}
	    else if (!use_tourney->value)		// Added "->value", duh -TempFile
	    {
	      server_type = "team game";
	    }
        else
	      server_type = "tourney";
	  }
      else
	  {
	  if ((int) dmflags->value & DF_MODELTEAMS)
	    server_type = "deathmatch (teams by model)";
	  else if ((int) dmflags->value & DF_SKINTEAMS)
	    server_type = "deathmatch (teams by skin)";
	  else
	    server_type = "deathmatch (no teams)";
	}
      sprintf (msg_buf + strlen (msg_buf),
	       "Game type: %s\n",
	       server_type);
      lines++;

      // Line for frag and time limit
      if ((int) fraglimit->value)
	sprintf (msg_buf + strlen (msg_buf), "Frag limit: %d", (int) fraglimit->value);
      else
	strcat (msg_buf, "Frag limit: none");
      if ((int) timelimit->value)
	sprintf (msg_buf + strlen (msg_buf), "  Time limit: %d\n", (int) timelimit->value);
      else
	strcat (msg_buf, "  Time limit: none\n");
      lines++;

      // Teamplay: 3 lines for round limit/round time limit, and menu instructions
      if (teamplay->value)
	{
	  if ((int) roundlimit->value)
	    sprintf (msg_buf + strlen (msg_buf), "Round limit: %d", (int) roundlimit->value);
	  else
	    strcat (msg_buf, "Round limit: none");
	  if ((int) roundtimelimit->value)
	    sprintf (msg_buf + strlen (msg_buf), "  Round time limit: %d\n", (int) roundtimelimit->value);
	  else
	    strcat (msg_buf, "  Round time limit: none\n");
	  lines++;
	}

      // Check for weird unique items/weapons settings, and inform with a line
      if ((int) unique_weapons->value != 1 ||
	  (int) unique_items->value != 1)
	{
	  sprintf (msg_buf + strlen (msg_buf), "Max carry of special weaps: %d  items: %d\n",
		   (int) unique_weapons->value, (int) unique_items->value);
	  lines++;
	}
      if (tgren->value > 0 || !(ir->value))
	{
	  char grenade_num[32];
	  if (tgren->value > 0)
	    sprintf (grenade_num, "%d grenade%s",
		     (int) tgren->value,
		     (int) tgren->value == 1 ? "" : "s");
	  sprintf (msg_buf + strlen (msg_buf), "Bandolier w/ %s%s%s\n",
		   !(ir->value) ? "no IR" : "",
		   (tgren->value > 0 && !(ir->value)) ? " & " : "",
		   tgren->value > 0 ? grenade_num : "");
	  lines++;
	}
      if (allitem->value || allweapon->value)
	{
	  sprintf (msg_buf + strlen (msg_buf), "Players receive %s%s%s\n",
		   allweapon->value ? "all weapons" : "",
		   (allweapon->value && allitem->value) ? " & " : "",
		   allitem->value ? "all items" : "");
	  lines++;
	}
      if (limchasecam->value)
	{
	  if ((int) limchasecam->value == 2)
	    sprintf (msg_buf + strlen (msg_buf), "Chase cam disallowed\n");
	  else
	    sprintf (msg_buf + strlen (msg_buf), "Chase cam restricted\n");
	  lines++;
	}
      if (teamplay->value && !((int) dmflags->value & DF_NO_FRIENDLY_FIRE))
	{
	  sprintf (msg_buf + strlen (msg_buf), "Friendly fire enabled\n");
	  lines++;
	}
	  //AQ2:TNG - Slicer : New location code
	  /* Adding MOTD part for the Map Locations (Creator, Build and Number of Locations) */
		if(ml_count!=0)
		{
			sprintf(msg_buf + strlen(msg_buf), "\nLocation File: %s, Build: %s\n",level.mapname,ml_build);
			lines++;
			sprintf(msg_buf + strlen(msg_buf), "%d Locations Loaded\n",ml_count);
			lines++;
			sprintf(msg_buf + strlen(msg_buf), "Created/Modified by %s\n",ml_creator);
			lines++;
		}
		else
		{
			sprintf(msg_buf + strlen(msg_buf), "\nNo Locations Loaded for %s\n",level.mapname);
			lines++;
		}
		//AQ2:TNG END

      // Teamplay: 2 lines (one blank) for menu instructions
      if (teamplay->value)
	{
	  strcat (msg_buf, "\nHit tab to open the team selection menu\n");
	  lines += 2;
	}

      /* 
         * If actionmaps, put a blank line then the maps list
       */

      // hopefully no one will run enough maps to exceed the line limit, if so, oh well... -FB
      if (actionmaps->value && num_maps > 0)
	{
	  int chars_on_line, len_mr;

	  strcat (msg_buf, "\nRunning the following maps in order:\n");
	  lines += 2;
	  // Igor[Rock] BEGIN
	  if (vrot->value)
	    {
	      strcat (msg_buf, "(Dynamic rotation is enabled)\n");
	      lines++;
	    }
	  if (rrot->value)
	    {
	      strcat (msg_buf, "(Random rotation is enabled)\n");
	      lines++;
	    }
	  // Igor[Rock] END

	  chars_on_line = 0;
	  for (mapnum = 0; mapnum < num_maps; mapnum++)
	    {
	      len_mr = strlen (*(map_rotation + mapnum));
	      if ((chars_on_line + len_mr + 2) > 39)
		{
		  strcat (msg_buf, "\n");
		  lines++;
		  if (lines >= max_lines)
		    break;
		  chars_on_line = 0;
		}
	      strcat (msg_buf, *(map_rotation + mapnum));
	      chars_on_line += len_mr;
	      if (mapnum < (num_maps - 1))
		{
		  strcat (msg_buf, ", ");
		  chars_on_line += 2;
		}
	    }

	  if (lines < max_lines)
	    {
	      strcat (msg_buf, "\n");
	      lines++;
	    }
	}

      strcat (msg_buf, "CLIENTS MUST GET THE SOUND PACK\nFROM http://aqdt.fear.net");
      lines += 2;

      if (motd_num_lines && lines < max_lines)
	{
	  strcat (msg_buf, "\n");
	  lines++;
	}
    }

  /* 
   * Insert action/motd.txt contents (whole MOTD gets truncated after 30 lines)
   */

  if (motd_num_lines)
    {
      if (lines < max_lines)
	{
	  for (i = 0; i < motd_num_lines; i++)
	    {
	      strcat (msg_buf, motd_lines[i]);
	      strcat (msg_buf, "\n");
	      lines++;
	      if (lines >= max_lines)
		break;
	    }
	}
    }

  gi.centerprintf (ent, msg_buf);
}

// stuffcmd: forces a player to execute a command.
void
stuffcmd (edict_t * ent, char *c)
{
  gi.WriteByte (svc_stufftext);
  gi.WriteString (c);
  gi.unicast (ent, true);
}



/********************************************************************************
*
*  zucc: following are EjectBlooder, EjectShell, AddSplat, and AddDecal
*  code.  All from actionquake, some had to be modified to fit Axshun or fix
*  bugs. 
*
*/

int decals = 0;
int shells = 0;
int splats = 0;

//blooder used for bleeding

void
BlooderDie (edict_t * self)
{
  G_FreeEdict (self);
}

void
BlooderTouch (edict_t * self, edict_t * other, cplane_t * plane, csurface_t * surf)
{
  G_FreeEdict (self);
}


void
EjectBlooder (edict_t * self, vec3_t start, vec3_t veloc)
{
  edict_t *blooder;
  vec3_t forward;
  int spd = 0;

  blooder = G_Spawn ();

  VectorCopy (veloc, forward);

  VectorCopy (start, blooder->s.origin);



  spd = 0;



  VectorScale (forward, spd, blooder->velocity);

  blooder->solid = SOLID_NOT;
  blooder->movetype = MOVETYPE_TOSS;

  blooder->s.modelindex = gi.modelindex ("sprites/null.sp2");
  blooder->s.effects |= EF_GIB;

  blooder->owner = self;
  blooder->touch = BlooderTouch;
  blooder->nextthink = level.time + 3.2;
  blooder->think = BlooderDie;
  blooder->classname = "blooder";


  gi.linkentity (blooder);
}


// zucc - Adding EjectShell code from action quake, modified for Axshun.
/********* SHELL EJECTION **************/




void
ShellTouch (edict_t * self, edict_t * other, cplane_t * plane, csurface_t * surf)
{
  if (self->owner->client->curr_weap == M3_NUM)
    gi.sound (self, CHAN_WEAPON, gi.soundindex ("weapons/shellhit1.wav"), 1, ATTN_STATIC, 0);
  else if (random () < 0.5)
    gi.sound (self, CHAN_WEAPON, gi.soundindex ("weapons/tink1.wav"), 1, ATTN_STATIC, 0);
  else
    gi.sound (self, CHAN_WEAPON, gi.soundindex ("weapons/tink2.wav"), 1, ATTN_STATIC, 0);
}

void
ShellDie (edict_t * self)
{
  G_FreeEdict (self);

  --shells;
}


// zucc fixed this so it works with the sniper rifle and checks handedness
// had to add the toggle feature to handle the akimbos correctly, if 1
// it sets up for ejecting the shell from the left akimbo weapon, if 2
// it fires right handed akimbo

void
EjectShell (edict_t * self, vec3_t start, int toggle)
{
  edict_t *shell;
  vec3_t forward, right, up;
  float r;
  float fix = 1.0;
  int left = 0;

  if (sv_shelloff->value)
    return;

  shell = G_Spawn ();
  ++shells;

  AngleVectors (self->client->v_angle, forward, right, up);

  if (self->client->pers.hand == LEFT_HANDED)
    {
      left = 1;
      fix = -1.0;
    }
  else if (self->client->pers.hand == CENTER_HANDED)
    fix = 0;


  // zucc spent a fair amount of time hacking these until they look ok,
  // several of them could be improved however.

  if (self->client->curr_weap == MK23_NUM)
    {
      VectorMA (start, left ? -7 : .4, right, start);
      VectorMA (start, left ? 5 : 2, forward, start);
      VectorMA (start, left ? -10 : -8, up, start);
    }
  else if (self->client->curr_weap == M4_NUM)
    {
      VectorMA (start, left ? -10 : 5, right, start);
      VectorMA (start, left ? 6 : 12, forward, start);
      VectorMA (start, left ? -9 : -11, up, start);
    }
  else if (self->client->curr_weap == MP5_NUM)
    {
      VectorMA (start, left ? -10 : 6, right, start);
      VectorMA (start, left ? 6 : 8, forward, start);
      VectorMA (start, left ? -9 : -10, up, start);
    }
  else if (self->client->curr_weap == SNIPER_NUM)
    {
      VectorMA (start, fix * 11, right, start);
      VectorMA (start, 2, forward, start);
      VectorMA (start, -11, up, start);

    }
  else if (self->client->curr_weap == M3_NUM)
    {
      VectorMA (start, left ? -9 : 3, right, start);
      VectorMA (start, left ? 4 : 4, forward, start);
      VectorMA (start, left ? -1 : -1, up, start);
    }

  else if (self->client->curr_weap == DUAL_NUM)
    {
      if (self->client->pers.hand == LEFT_HANDED)
	VectorMA (start, ((toggle == 1) ? 8 : -8), right, start);
      else
	VectorMA (start, ((toggle == 1) ? -4 : 4), right, start);
      VectorMA (start, 6, forward, start);
      VectorMA (start, -9, up, start);

    }


  if ((forward[2] >= -1) && (forward[2] < -0.99))
    {
      VectorMA (start, 5, forward, start);
      VectorMA (start, -0.5, up, start);
    }

  else if ((forward[2] >= -0.99) && (forward[2] < -0.98))
    {
      VectorMA (start, 5, forward, start);
      VectorMA (start, -.1, up, start);
    }
  else if ((forward[2] >= -0.98) && (forward[2] < -0.97))
    {
      VectorMA (start, 5.1, forward, start);
      VectorMA (start, 0.3, up, start);
    }
  else if ((forward[2] >= -0.97) && (forward[2] < -0.96))
    {
      VectorMA (start, 5.2, forward, start);
      VectorMA (start, 0.7, up, start);
    }
  else if ((forward[2] >= -0.96) && (forward[2] < -0.95))
    {
      VectorMA (start, 5.2, forward, start);
      VectorMA (start, 1.1, up, start);
    }
  else if ((forward[2] >= -0.95) && (forward[2] < -0.94))
    {
      VectorMA (start, 5.3, forward, start);
      VectorMA (start, 1.5, up, start);
    }
  else if ((forward[2] >= -0.94) && (forward[2] < -0.93))
    {
      VectorMA (start, 5.4, forward, start);
      VectorMA (start, 1.9, up, start);
    }
  else if ((forward[2] >= -0.93) && (forward[2] < -0.92))
    {
      VectorMA (start, 5.5, forward, start);
      VectorMA (start, 2.3, up, start);
    }
  else if ((forward[2] >= -0.92) && (forward[2] < -0.91))
    {
      VectorMA (start, 5.6, forward, start);
      VectorMA (start, 2.7, up, start);
    }
  else if ((forward[2] >= -0.91) && (forward[2] < -0.9))
    {
      VectorMA (start, 5.7, forward, start);
      VectorMA (start, 3.1, up, start);
    }

  else if ((forward[2] >= -0.9) && (forward[2] < -0.85))
    {
      VectorMA (start, 5.8, forward, start);
      VectorMA (start, 3.5, up, start);
    }
  else if ((forward[2] >= -0.85) && (forward[2] < -0.8))
    {
      VectorMA (start, 6, forward, start);
      VectorMA (start, 4, up, start);
    }
  else if ((forward[2] >= -0.8) && (forward[2] < -0.6))
    {
      VectorMA (start, 6.5, forward, start);
      VectorMA (start, 4.5, up, start);
    }
  else if ((forward[2] >= -0.6) && (forward[2] < -0.4))
    {
      VectorMA (start, 8, forward, start);
      VectorMA (start, 5.5, up, start);
    }
  else if ((forward[2] >= -0.4) && (forward[2] < -0.2))
    {
      VectorMA (start, 9.5, forward, start);
      VectorMA (start, 6, up, start);
    }
  else if ((forward[2] >= -0.2) && (forward[2] < 0))
    {
      VectorMA (start, 11, forward, start);
      VectorMA (start, 6.5, up, start);
    }
  else if ((forward[2] >= 0) && (forward[2] < 0.2))
    {
      VectorMA (start, 12, forward, start);
      VectorMA (start, 7, up, start);
    }
  else if ((forward[2] >= 0.2) && (forward[2] < 0.4))
    {
      VectorMA (start, 14, forward, start);
      VectorMA (start, 6.5, up, start);
    }
  else if ((forward[2] >= 0.4) && (forward[2] < 0.6))
    {
      VectorMA (start, 16, forward, start);
      VectorMA (start, 6, up, start);
    }
  else if ((forward[2] >= 0.6) && (forward[2] < 0.8))
    {
      VectorMA (start, 18, forward, start);
      VectorMA (start, 5, up, start);
    }
  else if ((forward[2] >= 0.8) && (forward[2] < 0.85))
    {
      VectorMA (start, 18, forward, start);
      VectorMA (start, 4, up, start);
    }
  else if ((forward[2] >= 0.85) && (forward[2] < 0.9))
    {
      VectorMA (start, 18, forward, start);
      VectorMA (start, 2.5, up, start);
    }

  else if ((forward[2] >= 0.9) && (forward[2] < 0.91))
    {
      VectorMA (start, 18.2, forward, start);
      VectorMA (start, 2.2, up, start);
    }
  else if ((forward[2] >= 0.91) && (forward[2] < 0.92))
    {
      VectorMA (start, 18.4, forward, start);
      VectorMA (start, 1.9, up, start);
    }
  else if ((forward[2] >= 0.92) && (forward[2] < 0.93))
    {
      VectorMA (start, 18.6, forward, start);
      VectorMA (start, 1.6, up, start);
    }
  else if ((forward[2] >= 0.93) && (forward[2] < 0.94))
    {
      VectorMA (start, 18.8, forward, start);
      VectorMA (start, 1.3, up, start);
    }
  else if ((forward[2] >= 0.94) && (forward[2] < 0.95))
    {
      VectorMA (start, 19, forward, start);
      VectorMA (start, 1, up, start);
    }
  else if ((forward[2] >= 0.95) && (forward[2] < 0.96))
    {
      VectorMA (start, 19.2, forward, start);
      VectorMA (start, 0.7, up, start);
    }
  else if ((forward[2] >= 0.96) && (forward[2] < 0.97))
    {
      VectorMA (start, 19.4, forward, start);
      VectorMA (start, 0.4, up, start);
    }
  else if ((forward[2] >= 0.97) && (forward[2] < 0.98))
    {
      VectorMA (start, 19.6, forward, start);
      VectorMA (start, -0.2, up, start);
    }
  else if ((forward[2] >= 0.98) && (forward[2] < 0.99))
    {
      VectorMA (start, 19.8, forward, start);
      VectorMA (start, -0.6, up, start);
    }

  else if ((forward[2] >= 0.99) && (forward[2] <= 1))
    {
      VectorMA (start, 20, forward, start);
      VectorMA (start, -1, up, start);
    }

  VectorCopy (start, shell->s.origin);
  if (fix == 0)			// we want some velocity on those center handed ones

    fix = 1;
  if (self->client->curr_weap == SNIPER_NUM)
    VectorMA (shell->velocity, fix * (-35 + random () * -60), right, shell->velocity);
  else if (self->client->curr_weap == DUAL_NUM)
    {
      if (self->client->pers.hand == LEFT_HANDED)
	VectorMA (shell->velocity, (toggle == 1 ? 1 : -1) * (35 + random () * 60), right, shell->velocity);
      else
	VectorMA (shell->velocity, (toggle == 1 ? -1 : 1) * (35 + random () * 60), right, shell->velocity);
    }
  else
    VectorMA (shell->velocity, fix * (35 + random () * 60), right, shell->velocity);
  VectorMA (shell->avelocity, 500, right, shell->avelocity);
  if (self->client->curr_weap == SNIPER_NUM)
    VectorMA (shell->velocity, 60 + 40, up, shell->velocity);
  else
    VectorMA (shell->velocity, 60 + random () * 90, up, shell->velocity);

  shell->movetype = MOVETYPE_BOUNCE;
  shell->solid = SOLID_BBOX;

  if (self->client->curr_weap == M3_NUM)
    shell->s.modelindex = gi.modelindex ("models/weapons/shell/tris2.md2");
  else if (self->client->curr_weap == SNIPER_NUM)
    shell->s.modelindex = gi.modelindex ("models/weapons/shell/tris3.md2");
  else
    shell->s.modelindex = gi.modelindex ("models/weapons/shell/tris.md2");

  r = random ();
  if (r < 0.1)
    shell->s.frame = 0;
  else if (r < 0.2)
    shell->s.frame = 1;
  else if (r < 0.3)
    shell->s.frame = 2;
  else if (r < 0.5)
    shell->s.frame = 3;
  else if (r < 0.6)
    shell->s.frame = 4;
  else if (r < 0.7)
    shell->s.frame = 5;
  else if (r < 0.8)
    shell->s.frame = 6;
  else if (r < 0.9)
    shell->s.frame = 7;
  else
    shell->s.frame = 8;

  shell->owner = self;
  shell->touch = ShellTouch;
  shell->nextthink = level.time + 1.2 - (shells * .05);
  shell->think = ShellDie;
  shell->classname = "shell";



  gi.linkentity (shell);
}



/*
   ==================
   FindEdictByClassnum
   ==================
 */
edict_t *
FindEdictByClassnum (char *classname, int classnum)
{
  int i;
  edict_t *it;


  for (i = 0; i < globals.num_edicts; i++)
    {
      it = &g_edicts[i];
      if (!it->classname)
	continue;
      if (!it->classnum)
	continue;
      if (Q_stricmp (it->classname, classname) == 0)
	{
	  if (it->classnum == classnum)
	    return it;
	}

    }

  return NULL;


}





/********* Bulletholes/wall stuff ***********/

void
DecalDie (edict_t * self)
{
  G_FreeEdict (self);
}

void
AddDecal (edict_t * self, trace_t * tr)
{
  edict_t *decal;
  edict_t *dec;

  if (bholelimit->value < 1)
    return;

  decal = G_Spawn ();
  ++decals;

  if (decals > bholelimit->value)
    decals = 1;

  dec = FindEdictByClassnum ("decal", decals);

  if (dec)
    {
//              gi.bprintf (PRINT_HIGH, "Decal->classnum == %i\n", dec->classnum);
      dec->nextthink = level.time + .1;
    }


  decal->solid = SOLID_NOT;
  decal->movetype = MOVETYPE_NONE;

  decal->s.modelindex = gi.modelindex ("models/objects/holes/hole1/hole.md2");

  VectorCopy (tr->endpos, decal->s.origin);

  vectoangles (tr->plane.normal, decal->s.angles);

  decal->owner = self;
  decal->classnum = decals;
  decal->touch = NULL;
  decal->nextthink = level.time + 20;
  decal->think = DecalDie;
  decal->classname = "decal";

  gi.linkentity (decal);
//glass
  if ((tr->ent) && (0 == Q_stricmp ("func_explosive", tr->ent->classname)))
    {
      CGF_SFX_AttachDecalToGlass (tr->ent, decal);
    }
//---
}

void
SplatDie (edict_t * self)
{
  G_FreeEdict (self);
}

void
AddSplat (edict_t * self, vec3_t point, trace_t * tr)
{
  edict_t *splat;
  edict_t *spt;
  float r;


  if (splatlimit->value < 1)
    return;

  splat = G_Spawn ();
  ++splats;

  if (splats > splatlimit->value)
    splats = 1;

  spt = FindEdictByClassnum ("splat", splats);

  if (spt)
    {
//              gi.bprintf (PRINT_HIGH, "Decal->classnum == %i\n", dec->classnum);
      spt->nextthink = level.time + .1;
    }

  splat->solid = SOLID_NOT;
  splat->movetype = MOVETYPE_NONE;

  r = random ();
  if (r > .67)
    splat->s.modelindex = gi.modelindex ("models/objects/splats/splat1/splat.md2");
  else if (r > .33)
    splat->s.modelindex = gi.modelindex ("models/objects/splats/splat2/splat.md2");
  else
    splat->s.modelindex = gi.modelindex ("models/objects/splats/splat3/splat.md2");

  VectorCopy (point, splat->s.origin);

  vectoangles (tr->plane.normal, splat->s.angles);

  splat->owner = self;
  splat->touch = NULL;
  splat->nextthink = level.time + 25;	// - (splats * .05);

  splat->think = SplatDie;
  splat->classname = "splat";
  splat->classnum = splats;

  gi.linkentity (splat);
//glass
  if ((tr->ent) && (0 == Q_stricmp ("func_explosive", tr->ent->classname)))
    {
      CGF_SFX_AttachDecalToGlass (tr->ent, splat);
    }
//---
}

/* %-variables for chat msgs */

void
GetWeaponName (edict_t * ent, char *buf)
{
  if (ent->client->pers.weapon)
    {
      strcpy (buf, ent->client->pers.weapon->pickup_name);
      return;
    }

  strcpy (buf, "No Weapon");
}

void
GetItemName (edict_t * ent, char *buf)
{
  gitem_t *spec;
  int i;

  i = 0;
  while (tnames[i])
    {
      if ((spec = FindItemByClassname (tnames[i])) != NULL &&
	  ent->client->pers.inventory[ITEM_INDEX (spec)])
	{
	  strcpy (buf, spec->pickup_name);
	  return;
	}
      i++;
    }

  strcpy (buf, "No Item");
}

void
GetHealth (edict_t * ent, char *buf)
{
  sprintf (buf, "%d", ent->health);
}

void
GetAmmo (edict_t * ent, char *buf)
{
  int ammo;

  if (ent->client->pers.weapon)
    {
      switch (ent->client->curr_weap)
	{
	case MK23_NUM:
	  sprintf (buf, "%d rounds (%d extra clips)",
		   ent->client->mk23_rds,
		   ent->client->pers.inventory[ent->client->ammo_index]);
	  return;
	case MP5_NUM:
	  sprintf (buf, "%d rounds (%d extra clips)",
		   ent->client->mp5_rds,
		   ent->client->pers.inventory[ent->client->ammo_index]);
	  return;
	case M4_NUM:
	  sprintf (buf, "%d rounds (%d extra clips)",
		   ent->client->m4_rds,
		   ent->client->pers.inventory[ent->client->ammo_index]);
	  return;
	case M3_NUM:
	  sprintf (buf, "%d shells (%d extra shells)",
		   ent->client->shot_rds,
		   ent->client->pers.inventory[ent->client->ammo_index]);
	  return;
	case HC_NUM:
	  sprintf (buf, "%d shells (%d extra shells)",
		   ent->client->cannon_rds,
		   ent->client->pers.inventory[ent->client->ammo_index]);
	  return;
	case SNIPER_NUM:
	  sprintf (buf, "%d rounds (%d extra rounds)",
		   ent->client->sniper_rds,
		   ent->client->pers.inventory[ent->client->ammo_index]);
	  return;
	case DUAL_NUM:
	  sprintf (buf, "%d rounds (%d extra clips)",
		   ent->client->dual_rds,
		   ent->client->pers.inventory[ent->client->ammo_index]);
	  return;
	case KNIFE_NUM:
	  ammo = ent->client->pers.inventory[ITEM_INDEX (FindItem (KNIFE_NAME))];
	  sprintf (buf, "%d kni%s", ammo, (ammo == 1) ? "fe" : "ves");
	  return;
	case GRENADE_NUM:
	  ammo = ent->client->pers.inventory[ITEM_INDEX (FindItem (GRENADE_NAME))];
	  sprintf (buf, "%d grenade%s", ammo, (ammo == 1) ? "" : "s");
	  return;
	}
    }

  strcpy (buf, "no ammo");
}

void
GetNearbyTeammates (edict_t * self, char *buf)
{
  unsigned char nearby_teammates[10][16];
  int nearby_teammates_num, l;
  edict_t *ent = NULL;

  nearby_teammates_num = 0;

  while ((ent = findradius (ent, self->s.origin, 1500)) != NULL)
    {
      if (ent == self || !ent->client || !CanDamage (ent, self) ||
	  !OnSameTeam (ent, self))
	continue;

      strncpy (nearby_teammates[nearby_teammates_num], ent->client->pers.netname, 15);
      nearby_teammates[nearby_teammates_num][15] = 0;	// in case their name is 15 chars...

      nearby_teammates_num++;
      if (nearby_teammates_num >= 10)
	break;
    }

  if (nearby_teammates_num == 0)
    {
      strcpy (buf, "nobody");
      return;
    }

  for (l = 0; l < nearby_teammates_num; l++)
    {
      if (l == 0)
	{
	  strcpy (buf, nearby_teammates[l]);
	}
      else
	{
	  if (nearby_teammates_num == 2)
	    {
	      strcat (buf, " and ");
	      strcat (buf, nearby_teammates[l]);
	    }
	  else
	    {
	      if (l == (nearby_teammates_num - 1))
		{
		  strcat (buf, ", and ");
		  strcat (buf, nearby_teammates[l]);
		}
	      else
		{
		  strcat (buf, ", ");
		  strcat (buf, nearby_teammates[l]);
		}
	    }
	}

    }
}


//PG BUND - BEGIN
//ParseSayText is redefined and extended in a_xgame.c
/*

   char *SeekBufEnd(char *buf)
   {
   while (*buf != 0)
   buf++;
   return buf;
   }    

   void ParseSayText(edict_t *ent, char *text)
   {
   static unsigned char buf[10240], infobuf[10240];
   char *p, *pbuf;

   p = text;
   pbuf = buf;
   *pbuf = 0;

   while (*p != 0)
   {
   if (((ptrdiff_t)pbuf - (ptrdiff_t)buf) > 300)
   {
   break;
   }
   if (*p == '%')
   {
   switch (*(p+1))
   {
   case 'H':
   GetHealth(ent, infobuf);
   strcpy(pbuf, infobuf);
   pbuf = SeekBufEnd(pbuf);
   p += 2;
   continue;
   case 'A':
   GetAmmo(ent, infobuf);
   strcpy(pbuf, infobuf);
   pbuf = SeekBufEnd(pbuf);
   p += 2;
   continue;
   case 'W':
   GetWeaponName(ent, infobuf);
   strcpy(pbuf, infobuf);
   pbuf = SeekBufEnd(pbuf);
   p += 2;
   continue;
   case 'I':
   GetItemName(ent, infobuf);
   strcpy(pbuf, infobuf);
   pbuf = SeekBufEnd(pbuf);
   p += 2;
   continue;
   case 'T':
   GetNearbyTeammates(ent, infobuf);
   strcpy(pbuf, infobuf);
   pbuf = SeekBufEnd(pbuf);
   p += 2;
   continue;
   }
   }
   *pbuf++ = *p++;
   }

   *pbuf = 0;

   strncpy(text, buf, 300);
   text[300] = 0; // in case it's 300
   }
 */
//PG BUND - END
