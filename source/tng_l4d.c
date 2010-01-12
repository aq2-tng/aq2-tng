/* Zombie hunting game mode */

#include "g_local.h"

cvar_t *l4d;

void L4D_Init()
{
	gi.dprintf("L4D enabled\n");
	if(allitem->value) {
		gi.dprintf(" forcing allitem off\n");
		gi.cvar_forceset(allitem->name, "0");
	}

	if(allweapon->value) {
		gi.dprintf(" forcing allweapon off\n");
		gi.cvar_forceset(allweapon->name, "0");
	}

	if(use_grapple->value) {
		gi.dprintf(" forcing grapple off\n");
		gi.cvar_forceset(use_grapple->name, "0");
	}

	if(!teamplay->value) {
		gi.dprintf(" forcing teamplay on\n");
		gi.cvar_forceset(teamplay->name, "1");
	}

	sprintf(teams[TEAM1].name, "The Zombies");
	sprintf(teams[TEAM2].name, "The Hunters");
}

void L4D_EquipClient(edict_t *ent)
{
	gi.dprintf("L4D_EquipClient called\n");
}

void L4D_JoinTeam(edict_t *ent)
{
	gi.dprintf("L4D_JoinTeam called\n");
}

void L4D_PlayerSpawn(edict_t *ent)
{
	gi.dprintf("L4D_PlayerSpawn called\n");
}

void L4D_PlayerDie(edict_t *ent)
{
	gi.dprintf("L4d_PlayerDie called\n");
}
