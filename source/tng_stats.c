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

	double perc_hit, perc_miss, accuracy, perc_hd;
	int total, hits, misses, headshots;
	
	hits = ent->client->resp.stats_shots_h;
	total = ent->client->resp.stats_shots_t;
	misses = total - hits;
	headshots = ent->client->resp.stats_shots_hd;
	
	if(total!=0) {
		perc_hit = ((double) hits / (double) total) * 100.0; // Percentage of shots that hit
		perc_miss = ((double) misses / (double) total) * 100.0; // Percentage of shots missed
		perc_hd = ((double) headshots / (double) total) * 100.0; // Percentage of total shots that are headshots
	} else {
		perc_hit = 0.0;
		perc_miss = 0.0;
		perc_hd = 0.0;
	}
	
	gi.cprintf(ent, PRINT_HIGH, "\nŸ\n");
	gi.cprintf(ent, PRINT_HIGH, "Statistics for %s:\n",ent->client->pers.netname);
	gi.cprintf(ent, PRINT_HIGH, "Of the %i shots fired, %d%% (%i) hit.\n", total, perc_hit, hits);
	gi.cprintf(ent, PRINT_HIGH, "Number of Headshots:      %i - %d%% of the total nr of hits.\n", headshots, perc_hd);
	gi.cprintf(ent, PRINT_HIGH, "Ÿ\n\n");

}

