#include "g_local.h"

cvar_t *eventeams;
cvar_t *use_balancer;

edict_t *FindNewestPlayer(int team)
{
	edict_t *etmp,*ent = NULL;
	int i;
	int most_time = 0;

	for (i = 1; i <= maxclients->value; i++)
	{
		etmp = g_edicts + i;
		if (etmp->inuse)
		{
			if(etmp->client->resp.team == team) {
				if(etmp->client->resp.joined_team > most_time) {
					most_time = etmp->client->resp.joined_team;
                                        ent = etmp;
				}
			}
		}
	}

	return ent;
}

void CalculatePlayers(int *team1, int *team2, int *team3, int *spectator)
{
	edict_t *e;
	int i;

	for (i = 1; i <= maxclients->value; i++)
	{
		e = g_edicts + i;
		if (e->inuse)
		{
			if(e->client->resp.team == TEAM1 && team1 != NULL)
				(*team1)++;
			else if(e->client->resp.team == TEAM2 && team2 != NULL)
				(*team2)++;
			else if(e->client->resp.team == TEAM3 && team3 != NULL)
				(*team3)++;
			else if(e->client->resp.team == NOTEAM && spectator != NULL)
				(*spectator)++;
			// if it's none of the above, what the heck is it?!
		}
	}
}

/* parameter can be current (dead) player or null */
qboolean CheckForUnevenTeams (edict_t *ent)
{
        edict_t *swap_ent = NULL;
	int team1 = 0, team2 = 0, other_team = 0;

	if(!use_balancer->value || use_3teams->value)
		return false;

	CalculatePlayers(&team1, &team2, NULL, NULL);

	if(team1 > team2+1) {
		other_team = TEAM2;
		swap_ent = FindNewestPlayer(TEAM1);
	} else if(team2 > team1+1) {
		other_team = TEAM1;
		swap_ent = FindNewestPlayer(TEAM2);
	}

	if(swap_ent && (!ent || ent == swap_ent)) {
		gi.centerprintf (swap_ent, "You have been swapped to the other team to even the game.");
		unicastSound(swap_ent, gi.soundindex("misc/talk1.wav"), 1.0);
		swap_ent->client->team_force = true;
		JoinTeam(swap_ent, other_team, 1);
		return true;
	}

	return false;
}

qboolean IsAllowedToJoin(edict_t *ent, int desired_team)
{
	int onteam1 = 0, onteam2 = 0;

	/* FIXME: make this work with threeteam */
	if (use_3teams->value)
		return true;

	if(ent->client->team_force) {
		ent->client->team_force = false;
		return true;
	}

	CalculatePlayers(&onteam1, &onteam2, NULL, NULL);

	/* can join both teams if they are even and can join if the other team has less players than current */
	if((desired_team == TEAM1 && onteam1 < onteam2) ||
		(desired_team == TEAM2 && onteam2 < onteam1) ||
		(ent->client->resp.team == NOTEAM && onteam1 == onteam2))
		return true;
	return false;
}

