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

	if(ir->value) {
		gi.dprintf(" forcing irvision off\n");
		gi.cvar_forceset(ir->name, "0");
	}

	if(ctf->value) {
		gi.dprintf(" forcing CTF off\n");
		gi.cvar_forceset(ctf->name, "0");
	}

	if(!teamplay->value) {
		gi.dprintf(" forcing teamplay on\n");
		gi.cvar_forceset(teamplay->name, "1");
	}

	sprintf(teams[TEAM1].name, "The Zombies");
	sprintf(teams[TEAM1].skin, "male/nut");
	sprintf(teams[TEAM1].skin_index, "../players/%s_i", teams[TEAM1].skin);
	sprintf(teams[TEAM2].name, "The Hunters");
	sprintf(teams[TEAM2].skin, "terror/skyterr");
	sprintf(teams[TEAM2].skin_index, "../players/%s_i", teams[TEAM2].skin);
}

void L4D_EquipClient(edict_t *ent)
{
	if(ent->client->resp.team == TEAM1) {
		/* zombies see a little */
		L4D_UnicastConfigString(ent, CS_LIGHTS + 0, "c");
		/* give double health for our zombies */
		ent->health = 500;
		ent->client->pers.health = ent->health;
		/* remove mk23 rounds */
		ent->client->mk23_rds = 0;
		ent->client->dual_rds = 0;
		/* activate knife into hands */
		ent->client->resp.knife_mode = 0;
		ent->client->pers.weapon = FindItem(KNIFE_NAME);
	} else {
		/* hunters see nothing */
		L4D_UnicastConfigString(ent, CS_LIGHTS + 0, "a");
		/* normal teamplay equip */
		EquipClient(ent);
	}
}

void L4D_PlayerDie(edict_t *ent)
{
	if (ent->flashlight)
	{
		G_FreeEdict (ent->flashlight);
		ent->flashlight = NULL;
	}
	L4D_ResetLights(ent);
}

void L4D_ResetLights(edict_t *ent)
{
	L4D_UnicastConfigString(ent, CS_LIGHTS + 0, "m");
}

qboolean L4D_Flashlight(edict_t *ent)
{
	if(ent->client->resp.team == TEAM2)
		return true;
	return false;
}

void L4D_PlayRandomZombieSound(edict_t * ent)
{
	int n;
	char buffer[32];

	int zombie_sounds[8] = { 1, 3, 5, 6, 7, 8, 10, 11 };

	n = rand() % 8;
	Com_sprintf(buffer, sizeof(buffer), "insane/insane%i.wav", zombie_sounds[n]);
	gi.sound(ent, CHAN_VOICE, gi.soundindex(buffer), 1, ATTN_NORM, 0);
}

void L4D_Think(edict_t *ent)
{
	if(team_round_going && ent->client->resp.team == TEAM1) {
		if(level.time > ent->client->l4d_nextsoundtime) {
			ent->client->l4d_nextsoundtime = level.time + rand() % 6 + 2;
			if(ent->client->l4d_nextsoundtime > 0)
				L4D_PlayRandomZombieSound(ent);
		}
	}
}

void L4D_RoundEnd()
{
	/* send a different value before if the server caches gi.configstring calls */
	gi.configstring(CS_LIGHTS + 0, "c");
	gi.configstring(CS_LIGHTS + 0, "m");
}

void L4D_UnicastConfigString(edict_t *ent, int index, char *value)
{
	gi.WriteByte(109 /* a REALLY magic number, it should be 14, wtf... */);
	gi.WriteShort(index);
	gi.WriteString(value);
	gi.unicast(ent, true);
}
