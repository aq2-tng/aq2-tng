#include "g_local.h"

/* Stats Command */

void Cmd_Stats_f(edict_t *ent)
{
	/* Variables Used:				*
	 * stats_shots_t - Total nr of Shots		*
	 * stats_shots_m - Total nr of Misses		*
	 * stats_shots_h - Total nr of Hits		*
	 * stats_shots_hd - Total nr of Headshots	*
	 *						*/

	double perc_hit, perc_miss, perc_hd;
	int total, hits, misses, headshots;
	
	hits = ent->client->resp.stats_shots_h;
	total = ent->client->resp.stats_shots_t;
	misses = total - hits;
	headshots = ent->client->resp.headshots;
	
	if(total!=0) {
		perc_hit = (((double) hits / (double) total) * 100.0); // Percentage of shots that hit
		perc_miss = (((double) misses / (double) total) * 100.0); // Percentage of shots missed
		perc_hd = (((double) headshots / (double) total) * 100.0); // Percentage of total shots that are headshots
	} else {
		perc_hit = 0.0;
		perc_miss = 0.0;
		perc_hd = 0.0;
	}
	
	gi.cprintf(ent, PRINT_HIGH, "\nùûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûü\n");
	gi.cprintf(ent, PRINT_HIGH, "Statistics for %s:\n", ent->client->pers.netname);
	gi.cprintf(ent, PRINT_HIGH, "Of the %i shots fired, %.2f%% (%i) hit.\n", total, perc_hit, hits);
	gi.cprintf(ent, PRINT_HIGH, "Of the %i shots that hit, %i (%.2f%%) were headshots.\n", hits, headshots, perc_hd);
	gi.cprintf(ent, PRINT_HIGH, "ùûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûûü\n\n");

}
void A_ScoreboardEndLevel ( edict_t *ent,  edict_t *killer)
{
  char string[1400], damage[50];
  edict_t *cl_ent;
  int maxsize = 1000, i, j, k;

  int total, score, ping,shots;
  double accuracy;
  double fpm;
  int sortedscores[MAX_CLIENTS], sorted[MAX_CLIENTS];
  int totalplayers     [TEAM_TOP];
  int totalscore[TEAM_TOP];
  int name_pos  [TEAM_TOP];

	  totalplayers[TEAM1] = totalplayers[TEAM2] = totalplayers[TEAM3]= 0;
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
		   totalscore[TEAM1], totalplayers[TEAM1], name_pos[TEAM1] - 80, team1_name);
	  sprintf (string + strlen (string),
		   // TEAM2
		   "if 25 xv 80 yv 8 pic 25 endif "
		   "if 22 xv 112 yv 8 pic 22 endif "
		   "xv 112 yv 28 string \"%4d/%-3d\" "
		   "xv 168 yv 12 num 2 27 "
		   "xv %d yv 0 string \"%s\" ",
		   totalscore[TEAM2], totalplayers[TEAM2], name_pos[TEAM2] + 80, team2_name);
	  sprintf (string + strlen (string),
		   // TEAM3
		   "if 30 xv 240 yv 8 pic 30 endif "
		   "if 22 xv 272 yv 8 pic 22 endif "
		   "xv 272 yv 28 string \"%4d/%-3d\" "
		   "xv 328 yv 12 num 2 31 "
		   "xv %d yv 0 string \"%s\" ",
		   totalscore[TEAM3], totalplayers[TEAM3], name_pos[TEAM3] + 240, team3_name);
	  }
   else
   {
	sprintf (string,
		   // TEAM1
		   "if 24 xv 0 yv 8 pic 24 endif "
		   "if 22 xv 32 yv 8 pic 22 endif "
		   "xv 32 yv 28 string \"%4d/%-3d\" "
		   "xv 90 yv 12 num 2 26 "
		   "xv %d yv 0 string \"%s\" "
		   // TEAM2
		   "if 25 xv 160 yv 8 pic 25 endif "
		   "if 22 xv 192 yv 8 pic 22 endif "
		   "xv 192 yv 28 string \"%4d/%-3d\" "
		   "xv 248 yv 12 num 2 27 "
		   "xv %d yv 0 string \"%s\" ",
		   totalscore[TEAM1], totalplayers[TEAM1], name_pos[TEAM1], team1_name,
		   totalscore[TEAM2], totalplayers[TEAM2], name_pos[TEAM2] + 160, team2_name);
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
      	sprintf (string + strlen (string), "xv 0 yv 40 string2 \"Frags Player          Shots Acc  FPM \" "
		  "xv 0 yv 48 string2 \"ùûûûü ùûûûûûûûûûûûûûü ùûûûü ùûûü ùûûü\" ");

//	  strcpy (string, "xv 0 yv 32 string2 \"Frags Player          Time Ping Damage Kills\" "
//		  "xv 0 yv 40 string2 \"ùûûûü ùûûûûûûûûûûûûûü ùûûü ùûûü ùûûûûü ùûûûü\" ");
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
		  if(shots > 9999)
			  shots = 9999;
	  if (shots != 0)
		  accuracy = (((double) game.clients[sorted[i]].resp.stats_shots_h / (double) shots) * 100.0);
	  else
		  accuracy = 0;
	  fpm = (((double) sortedscores[i] / (double) level.time) * 100.0);
	      if (game.clients[sorted[i]].resp.damage_dealt < 1000000)
		sprintf (damage, "%d", game.clients[sorted[i]].resp.damage_dealt);
	      else
		strcpy (damage, "******");
	      sprintf (string + strlen (string),
		       "xv 0 yv %d string \"%5d %-15s  %4d %.1f  %.1f\" ",
		       56 + i * 8,
		       sortedscores[i],
		       game.clients[sorted[i]].pers.netname,
		       shots,
			   accuracy,fpm);

	  if (strlen (string) > (maxsize - 100) &&
	      i < (total - 2))
	    {
	      sprintf (string + strlen (string),
		       "xv 0 yv %d string \"..and %d more\" ",
		       56 + (i + 1) * 8,
		       (total - i - 1));
	      break;
	    }
    }
  
  
  if (strlen (string) > 1023)	// for debugging...
    {
      gi.dprintf ("Warning: scoreboard string neared or exceeded max length\nDump:\n%s\n---\n",
		  string);
    }
  
  gi.WriteByte (svc_layout);
  gi.WriteString (string);
}

