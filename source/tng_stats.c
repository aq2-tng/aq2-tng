//-----------------------------------------------------------------------------
// Statistics Related Code
//
// $Id: tng_stats.c,v 1.32 2002/04/03 15:05:03 freud Exp $
//
//-----------------------------------------------------------------------------
// $Log: tng_stats.c,v $
// Revision 1.32  2002/04/03 15:05:03  freud
// My indenting broke something, rolled the source back.
//
// Revision 1.30  2002/04/01 16:08:59  freud
// Fix in hits/shots counter for each weapon
//
// Revision 1.29  2002/04/01 15:30:38  freud
// Small stat fix
//
// Revision 1.28  2002/04/01 15:16:06  freud
// Stats code redone, tng_stats now much more smarter. Removed a few global
// variables regarding stats code and added kevlar hits to stats.
//
// Revision 1.27  2002/03/28 12:10:12  freud
// Removed unused variables (compiler warnings).
// Added cvar mm_allowlock.
//
// Revision 1.26  2002/03/15 19:28:36  deathwatch
// Updated with stats rifle name fix
//
// Revision 1.25  2002/02/26 23:09:20  freud
// Stats <playerid> not working, fixed.
//
// Revision 1.24  2002/02/21 23:38:39  freud
// Fix to a BAD stats bug. CRASH
//
// Revision 1.23  2002/02/18 23:47:33  freud
// Fixed FPM if time was 0
//
// Revision 1.22  2002/02/18 19:31:40  freud
// FPM fix.
//
// Revision 1.21  2002/02/18 17:21:14  freud
// Changed Knife in stats to Slashing Knife
//
// Revision 1.20  2002/02/17 21:48:56  freud
// Changed/Fixed allignment of Scoreboard
//
// Revision 1.19  2002/02/05 09:27:17  freud
// Weapon name changes and better alignment in "stats list"
//
// Revision 1.18  2002/02/03 01:07:28  freud
// more fixes with stats
//
// Revision 1.14  2002/01/24 11:29:34  ra
// Cleanup's in stats code
//
// Revision 1.13  2002/01/24 02:24:56  deathwatch
// Major update to Stats code (thanks to Freud)
// new cvars:
// stats_afterround - will display the stats after a round ends or map ends
// stats_endmap - if on (1) will display the stats scoreboard when the map ends
//
// Revision 1.12  2001/12/31 13:29:06  deathwatch
// Added revision header
//
//
//-----------------------------------------------------------------------------

#include "g_local.h"

/* Stats Command */

void Cmd_Stats_f (edict_t *targetent, char *arg)
{
/* Variables Used:                              *
* stats_shots_t - Total nr of Shots             *
* stats_shots_h - Total nr of Hits              *
* headshots     - Total nr of Headshots         *
*                                               */
	
  double	perc_hit;
  int		total, hits, i, y;
  char		location[64];
  char		stathead[50], current_weapon[64];
  edict_t	*ent, *cl_ent;

  if (!targetent->inuse)
	return;

	
	if (arg[0] != '\0') {
		if (strcmp (arg, "list") == 0) {
			gi.cprintf (targetent, PRINT_HIGH, "\nŸ\n");
			gi.cprintf (targetent, PRINT_HIGH, "PlayerID  Name                  Accuracy\n");

			for (i = 0; i < game.maxclients; i++)
			{
				cl_ent = &g_edicts[1 + i];

				if (!cl_ent->inuse)
					continue;

				hits = 0;
				total = 0;
				for (y = 0; y < 100 ; y++) {
					hits += cl_ent->client->resp.stats_hits[y];
					total += cl_ent->client->resp.stats_shots[y];
				}

				if (hits > 0) {
					perc_hit = (((double) hits / (double) total) * 100.0);
				} else {
					perc_hit = 0.0;
				}

				gi.cprintf (targetent, PRINT_HIGH, "   %-3i    %-16s        %6.2f\n", i, cl_ent->client->pers.netname, perc_hit);
			}
			gi.cprintf (targetent, PRINT_HIGH, "\n  Use \"stats <PlayerID>\" for\n  individual stats\nŸ\n");
			return;
		}
		i = atoi (arg);
		ent = &g_edicts[1 + i];
		if (i >= game.maxclients || !ent->inuse)
			ent = targetent;

	} else {
		ent = targetent;
	}

		// Global Stats:
		hits = 0;
		total = 0;

		for (y = 0; y < 100 ; y++) {
			hits += ent->client->resp.stats_hits[y];
			total += ent->client->resp.stats_shots[y];
		}

		sprintf(stathead, "\nŸ Statistics for %s ", ent->client->pers.netname);
		for (i = 0; i + strlen(ent->client->pers.netname) < 20; i++) {
			sprintf(stathead, "%s", stathead);
		}
		sprintf(stathead, "%sŸ\n", stathead);
		gi.cprintf (targetent, PRINT_HIGH, "%s", stathead);

		if (total != 0) {
			gi.cprintf (targetent, PRINT_HIGH, "Weapon             Accuracy Hits/Shots  Headshots\n");		

			for (y = 0; y < 100 ; y++) {

				if (y == MOD_MK23)
					sprintf(current_weapon, "Pistol            ");
				else if (y == MOD_DUAL)
					sprintf(current_weapon, "Dual Pistols      ");
				else if (y == MOD_KNIFE)
					sprintf(current_weapon, "Slashing Knife    ");
				else if (y == MOD_KNIFE_THROWN)
					sprintf(current_weapon, "Throwing Knife    ");
				else if (y == MOD_M4)
					sprintf(current_weapon, "M4 Assault Rifle  ");
				else if (y == MOD_MP5)
					sprintf(current_weapon, "MP5 Submachinegun ");
				else if (y == MOD_SNIPER)
					sprintf(current_weapon, "Sniper Rifle      ");
				else if (y == MOD_HC)
					sprintf(current_weapon, "Handcannon        ");
				else if (y == MOD_M3)
					sprintf(current_weapon, "M3 Shotgun        ");
				else
					sprintf(current_weapon, "Unknown Weapon    ");
				

				if (ent->client->resp.stats_shots[y] > 0) {
					perc_hit = (((double) ent->client->resp.stats_hits[y] /
					(double) ent->client->resp.stats_shots[y]) * 100.0);	// Percentage of shots that hit

      					gi.cprintf(targetent, PRINT_HIGH, "%s %6.2f  %5i/%-5i    %i\n", current_weapon, perc_hit, ent->client->resp.stats_hits[y], ent->client->resp.stats_shots[y], ent->client->resp.stats_headshot[y]); 
				}
			}
    		} else {
			gi.cprintf (targetent, PRINT_HIGH, "\n  Player has not fired a shot.\n");
		}
	
  gi.cprintf (targetent, PRINT_HIGH, "\nŸ\n");
	
	
	// Final Part
  if (total != 0)
		gi.cprintf (targetent, PRINT_HIGH, "Location                         Hits     (%%)\n");		
		
	for (y = 0;y < 10;y++) {
		if (ent->client->resp.stats_locations[y] > 0) {
			perc_hit = (((double) ent->client->resp.stats_locations[y] / (double) hits) * 100.0);

			if (y == LOC_HDAM)
				sprintf(location, "Head");
			else if (y == LOC_CDAM)
				sprintf(location, "Chest");
			else if (y == LOC_SDAM)
				sprintf(location, "Stomach");
			else if (y == LOC_LDAM)
				sprintf(location, "Legs");
			else if (y == LOC_KVLR_HELMET)
				sprintf(location, "Kevlar Helmet");
			else if (y == LOC_KVLR_VEST)
				sprintf(location, "Kevlar Vest");
			else if (y == LOC_NO)
				sprintf(location, "Spread (Shotgun/Handcannon)");
			else
				sprintf(location, "Unknown");

			gi.cprintf(targetent, PRINT_HIGH, "%-26s %10i  (%6.2f)\n", location, ent->client->resp.stats_locations[y], perc_hit);
		}
	}
	gi.cprintf (targetent, PRINT_HIGH, "\n");
	if(total > 0) {
		if(hits > 0)
			perc_hit = (((double) hits / (double) total) * 100.0);
		else
			perc_hit = 0.0;
		
		gi.cprintf (targetent, PRINT_HIGH, "Average Accuracy:                        %.2f\n", perc_hit); // Average
    gi.cprintf (targetent, PRINT_HIGH, "\nŸ\n\n");
	}
}

void
A_ScoreboardEndLevel (edict_t * ent, edict_t * killer)
{
  char string[1400], damage[50];
  edict_t *cl_ent;
  int maxsize = 1000, i, j, k;

  int total, score, ping, shots;
  double accuracy;
  double fpm;
  int sortedscores[MAX_CLIENTS], sorted[MAX_CLIENTS];
  int totalplayers[TEAM_TOP];
  int totalscore[TEAM_TOP];
  int name_pos[TEAM_TOP];

  totalplayers[TEAM1] = totalplayers[TEAM2] = totalplayers[TEAM3] = 0;
  totalscore[TEAM1] = totalscore[TEAM2] = totalscore[TEAM3] = 0;
  ent->client->ps.stats[STAT_TEAM_HEADER] = gi.imageindex ("tag3");

  for (i = 0; i < game.maxclients; i++)
    {
      cl_ent = g_edicts + 1 + i;
      if (!cl_ent->inuse)
	{
	  continue;
	}
      if (game.clients[i].resp.team == 1)
	{
	  totalscore[TEAM1] = totalscore[TEAM1] + game.clients[i].resp.score;
	  totalplayers[TEAM1]++;
	}
      else if (game.clients[i].resp.team == 2)
	{
	  totalscore[TEAM2] = totalscore[TEAM2] + game.clients[i].resp.score;
	  totalplayers[TEAM2]++;
	}
      else if (game.clients[i].resp.team == 3)
	{
	  totalscore[TEAM3] = totalscore[TEAM3] + game.clients[i].resp.score;
	  totalplayers[TEAM3]++;
	}

    }
  name_pos[TEAM1] = ((20 - strlen (team1_name)) / 2) * 8;
  if (name_pos[TEAM1] < 0)
    {
      name_pos[TEAM1] = 0;
    }

  name_pos[TEAM2] = ((20 - strlen (team2_name)) / 2) * 8;
  if (name_pos[TEAM2] < 0)
    {
      name_pos[TEAM2] = 0;
    }

  name_pos[TEAM3] = ((20 - strlen (team2_name)) / 2) * 8;
  if (name_pos[TEAM3] < 0)
    {
      name_pos[TEAM3] = 0;
    }
  if (use_3teams->value)
    {
      sprintf (string,
	       // TEAM1
	       "if 24 xv -80 yv 8 pic 24 endif "
	       "if 22 xv -48 yv 8 pic 22 endif "
	       "xv -48 yv 28 string \"%4d/%-3d\" "
	       "xv 10 yv 12 num 2 26 "
	       "xv %d yv 0 string \"%s\" ",
	       totalscore[TEAM1], totalplayers[TEAM1], name_pos[TEAM1] - 80,
	       team1_name);
      sprintf (string + strlen (string),
	       // TEAM2
	       "if 25 xv 80 yv 8 pic 25 endif "
	       "if 22 xv 112 yv 8 pic 22 endif "
	       "xv 112 yv 28 string \"%4d/%-3d\" "
	       "xv 168 yv 12 num 2 27 "
	       "xv %d yv 0 string \"%s\" ",
	       totalscore[TEAM2], totalplayers[TEAM2], name_pos[TEAM2] + 80,
	       team2_name);
      sprintf (string + strlen (string),
	       // TEAM3
	       "if 30 xv 240 yv 8 pic 30 endif "
	       "if 22 xv 272 yv 8 pic 22 endif "
	       "xv 272 yv 28 string \"%4d/%-3d\" "
	       "xv 328 yv 12 num 2 31 "
	       "xv %d yv 0 string \"%s\" ",
	       totalscore[TEAM3], totalplayers[TEAM3], name_pos[TEAM3] + 240,
	       team3_name);
    }
  else
    {
      sprintf (string,
	       // TEAM1
	       "if 24 xv 0 yv 8 pic 24 endif "
	       "if 22 xv 32 yv 8 pic 22 endif "
	       "xv 32 yv 28 string \"%4d/%-3d\" "
	       "xv 90 yv 12 num 2 26 " "xv %d yv 0 string \"%s\" "
	       // TEAM2
	       "if 25 xv 160 yv 8 pic 25 endif "
	       "if 22 xv 192 yv 8 pic 22 endif "
	       "xv 192 yv 28 string \"%4d/%-3d\" "
	       "xv 248 yv 12 num 2 27 "
	       "xv %d yv 0 string \"%s\" ",
	       totalscore[TEAM1], totalplayers[TEAM1], name_pos[TEAM1],
	       team1_name, totalscore[TEAM2], totalplayers[TEAM2],
	       name_pos[TEAM2] + 160, team2_name);
    }


  total = score = 0;

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
	      if (score > sortedscores[j])
		break;
	    }
	  for (k = total; k > j; k--)
	    {
	      sorted[k] = sorted[k - 1];
	      sortedscores[k] = sortedscores[k - 1];
	    }
	}
      sorted[j] = i;
      sortedscores[j] = score;
      total++;
    }
  sprintf (string + strlen (string),
	   "xv 0 yv 40 string2 \"Frags Player          Shots   Acc   FPM \" "
	   "xv 0 yv 48 string2 \"Ÿ Ÿ Ÿ Ÿ Ÿ\" ");

//        strcpy (string, "xv 0 yv 32 string2 \"Frags Player          Time Ping Damage Kills\" "
//                "xv 0 yv 40 string2 \"Ÿ Ÿ Ÿ Ÿ Ÿ Ÿ\" ");
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
      shots = game.clients[sorted[i]].resp.stats_shots_t;
      if (shots > 9999)
	shots = 9999;
      if (shots != 0)
	accuracy =
	  (((double)
	    game.clients[sorted[i]].resp.stats_shots_h / (double) shots) *
	   100.0);
      else
	accuracy = 0;

      if ((int) ((level.framenum - game.clients[sorted[i]].resp.enterframe) / 10))
      	fpm = (((double) sortedscores[i] / (double) ((level.framenum - game.clients[sorted[i]].resp.enterframe) / 10)) * 100.0);
      else
        fpm = 0.0;

      if (game.clients[sorted[i]].resp.damage_dealt < 1000000)
	sprintf (damage, "%d", game.clients[sorted[i]].resp.damage_dealt);
      else
	strcpy (damage, "******");
      sprintf (string + strlen (string),
	       "xv 0 yv %d string \"%5d %-15s  %4d %5.1f  %4.1f\" ",
	       56 + i * 8,
	       sortedscores[i],
	       game.clients[sorted[i]].pers.netname, shots, accuracy, fpm);

      if (strlen (string) > (maxsize - 100) && i < (total - 2))
	{
	  sprintf (string + strlen (string),
		   "xv 0 yv %d string \"..and %d more\" ",
		   56 + (i + 1) * 8, (total - i - 1));
	  break;
	}
    }


  if (strlen (string) > 1023)	// for debugging...
    {
      gi.
	dprintf
	("Warning: scoreboard string neared or exceeded max length\nDump:\n%s\n---\n",
	 string);
    }

  gi.WriteByte (svc_layout);
  gi.WriteString (string);
}

void Cmd_Statmode_f(edict_t* ent, char *arg)
{
  int i;
  char stuff[128];


  // Ignore if there is no argument.
  if (arg[0] != '\0') {
    memset (stuff, 0, sizeof (stuff));

    // Numerical
    i = atoi (arg);

    if (i > 2 || i < 0) {
      gi.dprintf("Warning: stat_mode set to %i by %s\n", i, ent->client->pers.netname);

      // Force the old mode if it is valid else force 0
      if (ent->client->resp.stat_mode > 0 && ent->client->resp.stat_mode < 3)
        sprintf(stuff, "set stat_mode \"%i\"\n", ent->client->resp.stat_mode);
      else
        sprintf(stuff, "set stat_mode \"0\"\n");
    } else {
      sprintf(stuff, "set stat_mode \"%i\"\n", i);
      ent->client->resp.stat_mode = i;
    }
    stuffcmd(ent, stuff);
  }
}
