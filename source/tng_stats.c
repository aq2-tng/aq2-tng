//-----------------------------------------------------------------------------
// Statistics Related Code
//
// $Id: tng_stats.c,v 1.14 2002/01/24 11:29:34 ra Exp $
//
//-----------------------------------------------------------------------------
// $Log: tng_stats.c,v $
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

void Cmd_Stats_f (edict_t *targetent)
{
/* Variables Used:                              *
* stats_shots_t - Total nr of Shots            *
* stats_shots_h - Total nr of Hits             *
* headshots     - Total nr of Headshots        *
	*                                              */
	
  double perc_hit;
  int total, hits, i, x, y;
  char str_perc_hit[10], str_shots_h[10], str_shots_t[10], str_headshots[10], str_chestshots[10], str_stomachshots[10],	str_legshots[10], argument[10], stathead[50], str_playerid[32], playername[32];
	edict_t *ent, *cl_ent;
	
	if (gi.argc() > 1) {
    if (strcmp (gi.argv (1), "list") == 0) {
      gi.cprintf (targetent, PRINT_HIGH, "\nŸ\n");
      gi.cprintf (targetent, PRINT_HIGH, "PlayerID    Name      Accuracy\n");
			
      for (i = 0; i < game.maxclients; i++)
			{
				y = i;
				cl_ent = &g_edicts[1 + i];
				
				if (!cl_ent->inuse)
					continue;
				
				hits = cl_ent->client->resp.stats_m4_shots_h + cl_ent->client->resp.stats_mp5_shots_h + cl_ent->client->resp.stats_shotgun_shots_h + cl_ent->client->resp.stats_sniper_shots_h + cl_ent->client->resp.stats_knife_shots_h + cl_ent->client->resp.stats_tknife_shots_h + cl_ent->client->resp.stats_hc_shots_h + cl_ent->client->resp.stats_pistol_shots_h + cl_ent->client->resp.stats_dual_shots_h;
				total = cl_ent->client->resp.stats_m4_shots_t + cl_ent->client->resp.stats_mp5_shots_t + cl_ent->client->resp.stats_shotgun_shots_t + cl_ent->client->resp.stats_sniper_shots_t + cl_ent->client->resp.stats_knife_shots_t + cl_ent->client->resp.stats_tknife_shots_t + cl_ent->client->resp.stats_hc_shots_t + cl_ent->client->resp.stats_pistol_shots_t + cl_ent->client->resp.stats_dual_shots_t;
				
				if (hits > 0) {
					perc_hit = (((double) hits / (double) total) * 100.0);
				} else {
					perc_hit = 0.0;
				}
				
				
				sprintf(playername, "%s", cl_ent->client->pers.netname);
				
				for (x = 0; (x + strlen(cl_ent->client->pers.netname)) < 16; x++) {
					sprintf(playername, "%s ", playername);
				}
				
				if (y < 10) {
					sprintf(str_playerid, "%i  ", y);
				} else if (y < 100) {
					sprintf(str_playerid, "%i ", y);
				} else  {
					sprintf(str_playerid, "%i", y);
				}
				
				if (perc_hit < 10) {
					sprintf(str_perc_hit, "  %.2f", perc_hit);
				} else if (perc_hit < 100) {
					sprintf(str_perc_hit, " %.2f", perc_hit);
				} else {
					sprintf(str_perc_hit, "%.2f", perc_hit);
				}
				
				gi.cprintf (targetent, PRINT_HIGH, "   %s %s %s\n", str_playerid, playername, str_perc_hit);
			}
      gi.cprintf (targetent, PRINT_HIGH, "  Use \"stats <PlayerID>\" for individual stats\n\nŸ\n\n");
      return;
    }
    i = atoi (gi.argv (1));
    ent = &g_edicts[1 + i];
    if (!ent->inuse)
      ent = targetent;
  } else {
    ent = targetent;
  }
	
	// Global Stats:
	total = ent->client->resp.stats_m4_shots_t + ent->client->resp.stats_mp5_shots_t + ent->client->resp.stats_shotgun_shots_t + ent->client->resp.stats_sniper_shots_t + ent->client->resp.stats_knife_shots_t + ent->client->resp.stats_tknife_shots_t + ent->client->resp.stats_hc_shots_t + ent->client->resp.stats_pistol_shots_t + ent->client->resp.stats_dual_shots_t;
	hits = ent->client->resp.stats_m4_shots_h + ent->client->resp.stats_mp5_shots_h + ent->client->resp.stats_shotgun_shots_h + ent->client->resp.stats_sniper_shots_h + ent->client->resp.stats_knife_shots_h + ent->client->resp.stats_tknife_shots_h + ent->client->resp.stats_hc_shots_h + ent->client->resp.stats_pistol_shots_h + ent->client->resp.stats_dual_shots_h;
	
  sprintf(stathead, "\nŸ Statistics for %s ", ent->client->pers.netname);
  for (i = 0; i + strlen(ent->client->pers.netname) < 18; i++) {
		sprintf(stathead, "%s", stathead);
  }
  sprintf(stathead, "%sŸ\n", stathead);
  gi.cprintf (targetent, PRINT_HIGH, "%s", stathead);
  //gi.cprintf (targetent, PRINT_HIGH, "\nŸ Statistics Ÿ\n");
  if (total != 0) {
    gi.cprintf (targetent, PRINT_HIGH, "Weapon        Accuracy Hits/Shots  Headshots\n");		
		
		// Heckler & Koch MK23 Pistol
		if (ent->client->resp.stats_pistol_shots_t != 0)
    {
      perc_hit = (((double) ent->client->resp.stats_pistol_shots_h / (double) ent->client->resp.stats_pistol_shots_t) * 100.0);	// Percentage of shots that hit
      //perc_hd = (((double) headshots / (double) total) * 100.0);	// Percentage of total shots that are headshots
			
      if ( perc_hit >= 100 ) {
        sprintf(str_perc_hit, "%.2f", perc_hit);
      } else if ( perc_hit >= 10 ) {
        sprintf(str_perc_hit, " %.2f", perc_hit);
      } else {
        sprintf(str_perc_hit, "  %.2f", perc_hit);
      }
			
      if ( ent->client->resp.stats_pistol_shots_h >= 10000 ) {
        sprintf(str_shots_h, "%i", ent->client->resp.stats_pistol_shots_h);
      } else if ( ent->client->resp.stats_pistol_shots_h >= 1000 ) {
        sprintf(str_shots_h, " %i", ent->client->resp.stats_pistol_shots_h);
      } else if ( ent->client->resp.stats_pistol_shots_h >= 100 ) {
        sprintf(str_shots_h, "  %i", ent->client->resp.stats_pistol_shots_h);
      } else if ( ent->client->resp.stats_pistol_shots_h >= 10 ) {
        sprintf(str_shots_h, "   %i", ent->client->resp.stats_pistol_shots_h);
      } else {
        sprintf(str_shots_h, "    %i", ent->client->resp.stats_pistol_shots_h);
      }
			
      if ( ent->client->resp.stats_pistol_shots_t >= 10000 ) {
        sprintf(str_shots_t, "%i", ent->client->resp.stats_pistol_shots_t);
      } else if ( ent->client->resp.stats_pistol_shots_t >= 1000 ) {
        sprintf(str_shots_t, "%i ", ent->client->resp.stats_pistol_shots_t);
      } else if ( ent->client->resp.stats_pistol_shots_t >= 100 ) {
        sprintf(str_shots_t, "%i  ", ent->client->resp.stats_pistol_shots_t);
      } else if ( ent->client->resp.stats_pistol_shots_t >= 10 ) {
        sprintf(str_shots_t, "%i   ", ent->client->resp.stats_pistol_shots_t);
      } else {
        sprintf(str_shots_t, "%i    ", ent->client->resp.stats_pistol_shots_t);
      }
			
      gi.cprintf(targetent, PRINT_HIGH, "Pistol        %s  %s/%s    %i\n", str_perc_hit, str_shots_h, str_shots_t, ent->client->resp.stats_pistol_shots_hd); 
    }
		
		
		// Dual MK23 Pistols
		if (ent->client->resp.stats_dual_shots_t != 0)
    {
      perc_hit = (((double) ent->client->resp.stats_dual_shots_h / (double) ent->client->resp.stats_dual_shots_t) * 100.0);	// Percentage of shots that hit
      //perc_hd = (((double) headshots / (double) total) * 100.0);	// Percentage of total shots that are headshots
			
      if ( perc_hit >= 100 ) {
        sprintf(str_perc_hit, "%.2f", perc_hit);
      } else if ( perc_hit >= 10 ) {
        sprintf(str_perc_hit, " %.2f", perc_hit);
      } else {
        sprintf(str_perc_hit, "  %.2f", perc_hit);
      }
			
      if ( ent->client->resp.stats_dual_shots_h >= 10000 ) {
        sprintf(str_shots_h, "%i", ent->client->resp.stats_dual_shots_h);
      } else if ( ent->client->resp.stats_dual_shots_h >= 1000 ) {
        sprintf(str_shots_h, " %i", ent->client->resp.stats_dual_shots_h);
      } else if ( ent->client->resp.stats_dual_shots_h >= 100 ) {
        sprintf(str_shots_h, "  %i", ent->client->resp.stats_dual_shots_h);
      } else if ( ent->client->resp.stats_dual_shots_h >= 10 ) {
        sprintf(str_shots_h, "   %i", ent->client->resp.stats_dual_shots_h);
      } else {
        sprintf(str_shots_h, "    %i", ent->client->resp.stats_dual_shots_h);
      }
			
      if ( ent->client->resp.stats_dual_shots_t >= 10000 ) {
        sprintf(str_shots_t, "%i", ent->client->resp.stats_dual_shots_t);
      } else if ( ent->client->resp.stats_dual_shots_t >= 1000 ) {
        sprintf(str_shots_t, "%i ", ent->client->resp.stats_dual_shots_t);
      } else if ( ent->client->resp.stats_dual_shots_t >= 100 ) {
        sprintf(str_shots_t, "%i  ", ent->client->resp.stats_dual_shots_t);
      } else if ( ent->client->resp.stats_dual_shots_t >= 10 ) {
        sprintf(str_shots_t, "%i   ", ent->client->resp.stats_dual_shots_t);
      } else {
        sprintf(str_shots_t, "%i    ", ent->client->resp.stats_dual_shots_t);
      }
			
      gi.cprintf(targetent, PRINT_HIGH, "Dual Pistol   %s  %s/%s    %i\n", str_perc_hit, str_shots_h, str_shots_t, ent->client->resp.stats_dual_shots_hd);
    }
		
		// Combat Knife
		if (ent->client->resp.stats_knife_shots_t != 0)
    {
      perc_hit = (((double) ent->client->resp.stats_knife_shots_h / (double) ent->client->resp.stats_knife_shots_t) * 100.0);	// Percentage of shots that hit
      //perc_hd = (((double) headshots / (double) total) * 100.0);	// Percentage of total shots that are headshots
			
      if ( perc_hit >= 100 ) {
        sprintf(str_perc_hit, "%.2f", perc_hit);
      } else if ( perc_hit >= 10 ) {
        sprintf(str_perc_hit, " %.2f", perc_hit);
      } else {
        sprintf(str_perc_hit, "  %.2f", perc_hit);
      }
			
      if ( ent->client->resp.stats_knife_shots_h >= 10000 ) {
        sprintf(str_shots_h, "%i", ent->client->resp.stats_knife_shots_h);
      } else if ( ent->client->resp.stats_knife_shots_h >= 1000 ) {
        sprintf(str_shots_h, " %i", ent->client->resp.stats_knife_shots_h);
      } else if ( ent->client->resp.stats_knife_shots_h >= 100 ) {
        sprintf(str_shots_h, "  %i", ent->client->resp.stats_knife_shots_h);
      } else if ( ent->client->resp.stats_knife_shots_h >= 10 ) {
        sprintf(str_shots_h, "   %i", ent->client->resp.stats_knife_shots_h);
      } else {
        sprintf(str_shots_h, "    %i", ent->client->resp.stats_knife_shots_h);
      }
			
      if ( ent->client->resp.stats_knife_shots_t >= 10000 ) {
        sprintf(str_shots_t, "%i", ent->client->resp.stats_knife_shots_t);
      } else if ( ent->client->resp.stats_knife_shots_t >= 1000 ) {
        sprintf(str_shots_t, "%i ", ent->client->resp.stats_knife_shots_t);
      } else if ( ent->client->resp.stats_knife_shots_t >= 100 ) {
        sprintf(str_shots_t, "%i  ", ent->client->resp.stats_knife_shots_t);
      } else if ( ent->client->resp.stats_knife_shots_t >= 10 ) {
        sprintf(str_shots_t, "%i   ", ent->client->resp.stats_knife_shots_t);
      } else {
        sprintf(str_shots_t, "%i    ", ent->client->resp.stats_knife_shots_t);
      }
			
      gi.cprintf(targetent, PRINT_HIGH, "Knife         %s  %s/%s    %i\n", str_perc_hit, str_shots_h, str_shots_t, ent->client->resp.stats_knife_shots_hd);
    }
		
		// M4 Assault Rifle
		if (ent->client->resp.stats_m4_shots_t != 0)
    {
      perc_hit = (((double) ent->client->resp.stats_m4_shots_h / (double) ent->client->resp.stats_m4_shots_t) * 100.0);	// Percentage of shots that hit
      //perc_hd = (((double) headshots / (double) total) * 100.0);	// Percentage of total shots that are headshots
			
			
      if ( perc_hit >= 100 ) {
        sprintf(str_perc_hit, "%.2f", perc_hit);
      } else if ( perc_hit >= 10 ) {
        sprintf(str_perc_hit, " %.2f", perc_hit);
      } else {
        sprintf(str_perc_hit, "  %.2f", perc_hit);
      }
			
      if ( ent->client->resp.stats_m4_shots_h >= 10000 ) {
        sprintf(str_shots_h, "%i", ent->client->resp.stats_m4_shots_h);
      } else if ( ent->client->resp.stats_m4_shots_h >= 1000 ) {
        sprintf(str_shots_h, " %i", ent->client->resp.stats_m4_shots_h);
      } else if ( ent->client->resp.stats_m4_shots_h >= 100 ) {
        sprintf(str_shots_h, "  %i", ent->client->resp.stats_m4_shots_h);
      } else if ( ent->client->resp.stats_m4_shots_h >= 10 ) {
        sprintf(str_shots_h, "   %i", ent->client->resp.stats_m4_shots_h);
      } else {
        sprintf(str_shots_h, "    %i", ent->client->resp.stats_m4_shots_h);
      }
			
      if ( ent->client->resp.stats_m4_shots_t >= 10000 ) {
        sprintf(str_shots_t, "%i", ent->client->resp.stats_m4_shots_t);
      } else if ( ent->client->resp.stats_m4_shots_t >= 1000 ) {
        sprintf(str_shots_t, "%i ", ent->client->resp.stats_m4_shots_t);
      } else if ( ent->client->resp.stats_m4_shots_t >= 100 ) {
        sprintf(str_shots_t, "%i  ", ent->client->resp.stats_m4_shots_t);
      } else if ( ent->client->resp.stats_m4_shots_t >= 10 ) {
        sprintf(str_shots_t, "%i   ", ent->client->resp.stats_m4_shots_t);
      } else {
        sprintf(str_shots_t, "%i    ", ent->client->resp.stats_m4_shots_t);
      }
			
      gi.cprintf(targetent, PRINT_HIGH, "M4            %s  %s/%s    %i\n", str_perc_hit, str_shots_h, str_shots_t, ent->client->resp.stats_m4_shots_hd);
			
    }
		
		// MP5 Submachinegun
		if (ent->client->resp.stats_mp5_shots_t != 0)
    {
      perc_hit = (((double) ent->client->resp.stats_mp5_shots_h / (double) ent->client->resp.stats_mp5_shots_t) * 100.0);	// Percentage of shots that hit
      //perc_hd = (((double) headshots / (double) total) * 100.0);	// Percentage of total shots that are headshots
			
      if ( perc_hit >= 100 ) {
        sprintf(str_perc_hit, "%.2f", perc_hit);
      } else if ( perc_hit >= 10 ) {
        sprintf(str_perc_hit, " %.2f", perc_hit);
      } else {
        sprintf(str_perc_hit, "  %.2f", perc_hit);
      }
			
      if ( ent->client->resp.stats_mp5_shots_h >= 10000 ) {
        sprintf(str_shots_h, "%i", ent->client->resp.stats_mp5_shots_h);
      } else if ( ent->client->resp.stats_mp5_shots_h >= 1000 ) {
        sprintf(str_shots_h, " %i", ent->client->resp.stats_mp5_shots_h);
      } else if ( ent->client->resp.stats_mp5_shots_h >= 100 ) {
        sprintf(str_shots_h, "  %i", ent->client->resp.stats_mp5_shots_h);
      } else if ( ent->client->resp.stats_mp5_shots_h >= 10 ) {
        sprintf(str_shots_h, "   %i", ent->client->resp.stats_mp5_shots_h);
      } else {
        sprintf(str_shots_h, "    %i", ent->client->resp.stats_mp5_shots_h);
      }
			
      if ( ent->client->resp.stats_mp5_shots_t >= 10000 ) {
        sprintf(str_shots_t, "%i", ent->client->resp.stats_mp5_shots_t);
      } else if ( ent->client->resp.stats_mp5_shots_t >= 1000 ) {
        sprintf(str_shots_t, "%i ", ent->client->resp.stats_mp5_shots_t);
      } else if ( ent->client->resp.stats_mp5_shots_t >= 100 ) {
        sprintf(str_shots_t, "%i  ", ent->client->resp.stats_mp5_shots_t);
      } else if ( ent->client->resp.stats_mp5_shots_t >= 10 ) {
        sprintf(str_shots_t, "%i   ", ent->client->resp.stats_mp5_shots_t);
      } else {
        sprintf(str_shots_t, "%i    ", ent->client->resp.stats_mp5_shots_t);
      }
			
      gi.cprintf(targetent, PRINT_HIGH, "MP5           %s  %s/%s    %i\n", str_perc_hit, str_shots_h, str_shots_t, ent->client->resp.stats_mp5_shots_hd);
    }
		
		// SSG 3000 Sniper Rifle
		if (ent->client->resp.stats_sniper_shots_t != 0)
    {
      perc_hit = (((double) ent->client->resp.stats_sniper_shots_h / (double) ent->client->resp.stats_sniper_shots_t) * 100.0);	// Percentage of shots that hit
      //perc_hd = (((double) headshots / (double) total) * 100.0);	// Percentage of total shots that are headshots
			
      if ( perc_hit >= 100 ) {
        sprintf(str_perc_hit, "%.2f", perc_hit);
      } else if ( perc_hit >= 10 ) {
        sprintf(str_perc_hit, " %.2f", perc_hit);
      } else {
        sprintf(str_perc_hit, "  %.2f", perc_hit);
      }
			
      if ( ent->client->resp.stats_sniper_shots_h >= 10000 ) {
        sprintf(str_shots_h, "%i", ent->client->resp.stats_sniper_shots_h);
      } else if ( ent->client->resp.stats_sniper_shots_h >= 1000 ) {
        sprintf(str_shots_h, " %i", ent->client->resp.stats_sniper_shots_h);
      } else if ( ent->client->resp.stats_sniper_shots_h >= 100 ) {
        sprintf(str_shots_h, "  %i", ent->client->resp.stats_sniper_shots_h);
      } else if ( ent->client->resp.stats_sniper_shots_h >= 10 ) {
        sprintf(str_shots_h, "   %i", ent->client->resp.stats_sniper_shots_h);
      } else {
        sprintf(str_shots_h, "    %i", ent->client->resp.stats_sniper_shots_h);
      }
			
      if ( ent->client->resp.stats_sniper_shots_t >= 10000 ) {
        sprintf(str_shots_t, "%i", ent->client->resp.stats_sniper_shots_t);
      } else if ( ent->client->resp.stats_sniper_shots_t >= 1000 ) {
        sprintf(str_shots_t, "%i ", ent->client->resp.stats_sniper_shots_t);
      } else if ( ent->client->resp.stats_sniper_shots_t >= 100 ) {
        sprintf(str_shots_t, "%i  ", ent->client->resp.stats_sniper_shots_t);
      } else if ( ent->client->resp.stats_sniper_shots_t >= 10 ) {
        sprintf(str_shots_t, "%i   ", ent->client->resp.stats_sniper_shots_t);
      } else {
        sprintf(str_shots_t, "%i    ", ent->client->resp.stats_sniper_shots_t);
      }
			
      gi.cprintf(targetent, PRINT_HIGH, "Sniper        %s  %s/%s    %i\n", str_perc_hit, str_shots_h, str_shots_t, ent->client->resp.stats_sniper_shots_hd);
    }
		
		// Beneli M3 Shotgun
		if (ent->client->resp.stats_shotgun_shots_t != 0)
    {
      perc_hit = (((double) ent->client->resp.stats_shotgun_shots_h / (double) ent->client->resp.stats_shotgun_shots_t) * 100.0);	// Percentage of shots that hit
      //perc_hd = (((double) headshots / (double) total) * 100.0);	// Percentage of total shots that are headshots
			
      if ( perc_hit >= 100 ) {
        sprintf(str_perc_hit, "%.2f", perc_hit);
      } else if ( perc_hit >= 10 ) {
        sprintf(str_perc_hit, " %.2f", perc_hit);
      } else {
        sprintf(str_perc_hit, "  %.2f", perc_hit);
      }
			
      if ( ent->client->resp.stats_shotgun_shots_h >= 10000 ) {
        sprintf(str_shots_h, "%i", ent->client->resp.stats_shotgun_shots_h);
      } else if ( ent->client->resp.stats_shotgun_shots_h >= 1000 ) {
        sprintf(str_shots_h, " %i", ent->client->resp.stats_shotgun_shots_h);
      } else if ( ent->client->resp.stats_shotgun_shots_h >= 100 ) {
        sprintf(str_shots_h, "  %i", ent->client->resp.stats_shotgun_shots_h);
      } else if ( ent->client->resp.stats_shotgun_shots_h >= 10 ) {
        sprintf(str_shots_h, "   %i", ent->client->resp.stats_shotgun_shots_h);
      } else {
        sprintf(str_shots_h, "    %i", ent->client->resp.stats_shotgun_shots_h);
      }
			
      if ( ent->client->resp.stats_shotgun_shots_t >= 10000 ) {
        sprintf(str_shots_t, "%i", ent->client->resp.stats_shotgun_shots_t);
      } else if ( ent->client->resp.stats_shotgun_shots_t >= 1000 ) {
        sprintf(str_shots_t, "%i ", ent->client->resp.stats_shotgun_shots_t);
      } else if ( ent->client->resp.stats_shotgun_shots_t >= 100 ) {
        sprintf(str_shots_t, "%i  ", ent->client->resp.stats_shotgun_shots_t);
      } else if ( ent->client->resp.stats_shotgun_shots_t >= 10 ) {
        sprintf(str_shots_t, "%i   ", ent->client->resp.stats_shotgun_shots_t);
      } else {
        sprintf(str_shots_t, "%i    ", ent->client->resp.stats_shotgun_shots_t);
      }
			
      gi.cprintf(targetent, PRINT_HIGH, "Shotgun       %s  %s/%s    %i\n", str_perc_hit, str_shots_h, str_shots_t, ent->client->resp.stats_shotgun_shots_hd);
    }
		
		// Sawn Off Shotgun
		if (ent->client->resp.stats_hc_shots_t != 0)
    {
      perc_hit = (((double) ent->client->resp.stats_hc_shots_h / (double) ent->client->resp.stats_hc_shots_t) * 100.0);	// Percentage of shots that hit
      //perc_hd = (((double) headshots / (double) total) * 100.0);	// Percentage of total shots that are headshots
			
      if ( perc_hit >= 100 ) {
        sprintf(str_perc_hit, "%.2f", perc_hit);
      } else if ( perc_hit >= 10 ) {
        sprintf(str_perc_hit, " %.2f", perc_hit);
      } else {
        sprintf(str_perc_hit, "  %.2f", perc_hit);
      }
			
      if ( ent->client->resp.stats_hc_shots_h >= 10000 ) {
        sprintf(str_shots_h, "%i", ent->client->resp.stats_hc_shots_h);
      } else if ( ent->client->resp.stats_hc_shots_h >= 1000 ) {
        sprintf(str_shots_h, " %i", ent->client->resp.stats_hc_shots_h);
      } else if ( ent->client->resp.stats_hc_shots_h >= 100 ) {
        sprintf(str_shots_h, "  %i", ent->client->resp.stats_hc_shots_h);
      } else if ( ent->client->resp.stats_hc_shots_h >= 10 ) {
        sprintf(str_shots_h, "   %i", ent->client->resp.stats_hc_shots_h);
      } else {
        sprintf(str_shots_h, "    %i", ent->client->resp.stats_hc_shots_h);
      }
			
      if ( ent->client->resp.stats_hc_shots_t >= 10000 ) {
        sprintf(str_shots_t, "%i", ent->client->resp.stats_hc_shots_t);
      } else if ( ent->client->resp.stats_hc_shots_t >= 1000 ) {
        sprintf(str_shots_t, "%i ", ent->client->resp.stats_hc_shots_t);
      } else if ( ent->client->resp.stats_hc_shots_t >= 100 ) {
        sprintf(str_shots_t, "%i  ", ent->client->resp.stats_hc_shots_t);
      } else if ( ent->client->resp.stats_hc_shots_t >= 10 ) {
        sprintf(str_shots_t, "%i   ", ent->client->resp.stats_hc_shots_t);
      } else {
        sprintf(str_shots_t, "%i    ", ent->client->resp.stats_hc_shots_t);
      }
			
      gi.cprintf(targetent, PRINT_HIGH, "Handcannon    %s  %s/%s    %i\n", str_perc_hit, str_shots_h, str_shots_t, ent->client->resp.stats_hc_shots_hd);
    }
		
		// Thrown Combat Knife
		if (ent->client->resp.stats_tknife_shots_t != 0)
    {
      perc_hit = (((double) ent->client->resp.stats_tknife_shots_h / (double) ent->client->resp.stats_tknife_shots_t) * 100.0);	// Percentage of shots that hit
      //perc_hd = (((double) headshots / (double) total) * 100.0);	// Percentage of total shots that are headshots
			
      if ( perc_hit >= 100 ) {
        sprintf(str_perc_hit, "%.2f", perc_hit);
      } else if ( perc_hit >= 10 ) {
        sprintf(str_perc_hit, " %.2f", perc_hit);
      } else {
        sprintf(str_perc_hit, "  %.2f", perc_hit);
      }
			
      if ( ent->client->resp.stats_tknife_shots_h >= 10000 ) {
        sprintf(str_shots_h, "%i", ent->client->resp.stats_tknife_shots_h);
      } else if ( ent->client->resp.stats_tknife_shots_h >= 1000 ) {
        sprintf(str_shots_h, " %i", ent->client->resp.stats_tknife_shots_h);
      } else if ( ent->client->resp.stats_tknife_shots_h >= 100 ) {
        sprintf(str_shots_h, "  %i", ent->client->resp.stats_tknife_shots_h);
      } else if ( ent->client->resp.stats_tknife_shots_h >= 10 ) {
        sprintf(str_shots_h, "   %i", ent->client->resp.stats_tknife_shots_h);
      } else {
        sprintf(str_shots_h, "    %i", ent->client->resp.stats_tknife_shots_h);
      }
			
      if ( ent->client->resp.stats_knife_shots_t >= 10000 ) {
        sprintf(str_shots_t, "%i", ent->client->resp.stats_tknife_shots_t);
      } else if ( ent->client->resp.stats_tknife_shots_t >= 1000 ) {
        sprintf(str_shots_t, "%i ", ent->client->resp.stats_tknife_shots_t);
      } else if ( ent->client->resp.stats_tknife_shots_t >= 100 ) {
        sprintf(str_shots_t, "%i  ", ent->client->resp.stats_tknife_shots_t);
      } else if ( ent->client->resp.stats_tknife_shots_t >= 10 ) {
        sprintf(str_shots_t, "%i   ", ent->client->resp.stats_tknife_shots_t);
      } else {
        sprintf(str_shots_t, "%i    ", ent->client->resp.stats_tknife_shots_t);
      }
			
      gi.cprintf(targetent, PRINT_HIGH, "Thrown Knife  %s  %s/%s    %i\n", str_perc_hit, str_shots_h, str_shots_t, ent->client->resp.stats_tknife_shots_hd);
		}
	}
	else 
	{
		gi.cprintf (targetent, PRINT_HIGH, "\n  Player has not fired a shot.\n");
	}
	
  gi.cprintf (targetent, PRINT_HIGH, "\nŸ\n");
	
	
	// Final Part
  if (total != 0)
		gi.cprintf (targetent, PRINT_HIGH, "Location                         Hits   (%%)\n");		
		
	if ( ent->client->resp.headshots != 0 ) {
		perc_hit = (((double) ent->client->resp.headshots / (double) hits) * 100.0);	// Percentage of total that are headshots
			
		if ( perc_hit >= 100 )
			sprintf(str_perc_hit, "(%.2f)", perc_hit);
		if ( perc_hit > 9 )
			sprintf(str_perc_hit, "( %.2f)", perc_hit);
		else
			sprintf(str_perc_hit, "(  %.2f)", perc_hit);
			
		if ( ent->client->resp.headshots >= 10000 )
			sprintf(str_headshots, "%i", ent->client->resp.headshots);
		else if ( ent->client->resp.headshots >= 1000 )
			sprintf(str_headshots, " %i", ent->client->resp.headshots);
		else if ( ent->client->resp.headshots >= 100 )
			sprintf(str_headshots, "  %i", ent->client->resp.headshots);
		else if ( ent->client->resp.headshots >= 10 )
			sprintf(str_headshots, "   %i", ent->client->resp.headshots);
		else
			sprintf(str_headshots, "    %i", ent->client->resp.headshots);
			
		gi.cprintf (targetent, PRINT_HIGH, "Head                          %s  %s\n", str_headshots, str_perc_hit); // Total Headshots
	}
	if ( ent->client->resp.chestshots != 0 ) {
		perc_hit = (((double) ent->client->resp.chestshots / (double) hits) * 100.0);	// Percentage of total that are chestshots
		
		if ( perc_hit >= 100 )
			sprintf(str_perc_hit, "(%.2f)", perc_hit);
		if ( perc_hit > 9 )
			sprintf(str_perc_hit, "( %.2f)", perc_hit);
		else
			sprintf(str_perc_hit, "(  %.2f)", perc_hit);
		
		if ( ent->client->resp.chestshots >= 10000 )
			sprintf(str_chestshots, "%i", ent->client->resp.chestshots);
		else if ( ent->client->resp.chestshots >= 1000 )
			sprintf(str_chestshots, " %i", ent->client->resp.chestshots);
		else if ( ent->client->resp.chestshots >= 100 )
			sprintf(str_chestshots, "  %i", ent->client->resp.chestshots);
		else if ( ent->client->resp.chestshots >= 10 )
			sprintf(str_chestshots, "   %i", ent->client->resp.chestshots);
		else
			sprintf(str_chestshots, "    %i", ent->client->resp.chestshots);
		
		gi.cprintf (targetent, PRINT_HIGH, "Chest                         %s  %s\n", str_chestshots, str_perc_hit); // Total Chestshots
	}
	if ( ent->client->resp.stomachshots != 0 ) {
		perc_hit = (((double) ent->client->resp.stomachshots / (double) hits) * 100.0);	// Percentage of total that are stomachshots
		
		if ( perc_hit >= 100 )
			sprintf(str_perc_hit, "(%.2f)", perc_hit);
		if ( perc_hit > 9 )
			sprintf(str_perc_hit, "( %.2f)", perc_hit);
		else
			sprintf(str_perc_hit, "(  %.2f)", perc_hit);
		
		if ( ent->client->resp.stomachshots >= 10000 )
			sprintf(str_stomachshots, "%i", ent->client->resp.stomachshots);
		else if ( ent->client->resp.stomachshots >= 1000 )
			sprintf(str_stomachshots, " %i", ent->client->resp.stomachshots);
		else if ( ent->client->resp.stomachshots >= 100 )
			sprintf(str_stomachshots, "  %i", ent->client->resp.stomachshots);
		else if ( ent->client->resp.stomachshots >= 10 )
			sprintf(str_stomachshots, "   %i", ent->client->resp.stomachshots);
		else
			sprintf(str_stomachshots, "    %i", ent->client->resp.stomachshots);
		
		gi.cprintf (targetent, PRINT_HIGH, "Stomach                       %s  %s\n", str_stomachshots, str_perc_hit); // Total Stomachshots
	}
	if ( ent->client->resp.legshots != 0 ) {
		perc_hit = (((double) ent->client->resp.legshots / (double) hits) * 100.0);	// Percentage of total that are legshots
		
		if ( perc_hit >= 100 )
			sprintf(str_perc_hit, "(%.2f)", perc_hit);
		if ( perc_hit > 9 )
			sprintf(str_perc_hit, "( %.2f)", perc_hit);
		else
			sprintf(str_perc_hit, "(  %.2f)", perc_hit);
     
		if ( ent->client->resp.legshots >= 10000 )
			sprintf(str_legshots, "%i", ent->client->resp.legshots);
		else if ( ent->client->resp.legshots >= 1000 )
			sprintf(str_legshots, " %i", ent->client->resp.legshots);
		else if ( ent->client->resp.legshots >= 100 )
			sprintf(str_legshots, "  %i", ent->client->resp.legshots);
		else if ( ent->client->resp.legshots >= 10 )
			sprintf(str_legshots, "   %i", ent->client->resp.legshots);
		else
			sprintf(str_legshots, "    %i", ent->client->resp.legshots);
		
		gi.cprintf (targetent, PRINT_HIGH, "Legs                          %s  %s\n", str_legshots, str_perc_hit); // Total Legshots
	}
	gi.cprintf (targetent, PRINT_HIGH, "\n");
	if(total != 0) {
		if(hits != 0)
			perc_hit = (((double) hits / (double) total) * 100.0);
		else
			perc_hit = 0.0;
		
		sprintf(str_perc_hit, "%.2f", perc_hit);
		gi.cprintf (targetent, PRINT_HIGH, "Average Accuracy:                      %s\n", str_perc_hit); // Average
    gi.cprintf (targetent, PRINT_HIGH, "\nŸ\n\n");
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
	   "xv 0 yv 40 string2 \"Frags Player          Shots Acc  FPM \" "
	   "xv 0 yv 48 string2 \"Ÿ Ÿ Ÿ Ÿ Ÿ\" ");

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
      fpm = (((double) sortedscores[i] / (double) level.time) * 100.0);
      if (game.clients[sorted[i]].resp.damage_dealt < 1000000)
	sprintf (damage, "%d", game.clients[sorted[i]].resp.damage_dealt);
      else
	strcpy (damage, "******");
      sprintf (string + strlen (string),
	       "xv 0 yv %d string \"%5d %-15s  %4d %.1f  %.1f\" ",
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

void Cmd_Statmode_f(edict_t* ent)
{
  int i;
  char stuff[128];


  // Ignore if there is no argument.
  if (gi.argc () == 2) {
  memset (stuff, 0, sizeof (stuff));

    // Numerical
    i = atoi (gi.argv (1));

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
