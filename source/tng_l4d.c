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
	gitem_t *item;

	if(ent->client->resp.team == TEAM1) {
		/* zombies see a little */
		L4D_ZombieLights(ent);
		/* give double health for our zombies */
		ent->health = 500;
		ent->client->pers.health = ent->health;
		/* remove mk23 rounds */
		ent->client->mk23_rds = 0;
		ent->client->dual_rds = 0;
		ent->client->resp.knife_mode = 0;

		/* remove default mk23 */
		ent->client->pers.inventory[ITEM_INDEX(GET_ITEM(MK23_NUM))] = 0;

		/* set knife as the default weapon */
		item = GET_ITEM(KNIFE_NUM);
		ent->client->pers.selected_item = ITEM_INDEX(item);
		ent->client->pers.inventory[ent->client->pers.selected_item] = 1;
		ent->client->pers.weapon = item;

		ent->client->pers.lastweapon = item;
	} else {
		/* hunters see nothing */
		L4D_HumanLights(ent);
		/* normal teamplay equip */
		EquipClient(ent);

		ent->client->l4d_fl_charge = L4D_FL_FULL;
		ent->client->l4d_fl_nextthink = level.time + L4D_FL_THINK;
	}

	/* everyone starts with flashlight off, always */
	L4D_ToggleFL(ent, false);
}

void L4D_PlayerDie(edict_t *ent)
{
	gitem_t *item;

	/* make sure the dead player has a mk23 */
	item = GET_ITEM(MK23_NUM);
	ent->client->pers.selected_item = ITEM_INDEX(item);
	ent->client->pers.inventory[ent->client->pers.selected_item] = 1;
	ent->client->pers.weapon = item;
	ent->client->pers.lastweapon = item;
	ent->client->l4d_fl_charge = L4D_FL_FULL;
	ent->client->l4d_fl_nextthink = 0;

	L4D_ToggleFL(ent, false);
	L4D_ResetLights(ent);
}

void L4D_HumanLights(edict_t *ent)
{
	L4D_UnicastConfigString(ent, CS_LIGHTS + 0, "a");
}

void L4D_ZombieLights(edict_t *ent)
{
	L4D_UnicastConfigString(ent, CS_LIGHTS + 0, "c");
}

void L4D_ResetLights(edict_t *ent)
{
	L4D_UnicastConfigString(ent, CS_LIGHTS + 0, "m");
}

void L4D_UpdateSpectatorLights(edict_t *ent)
{
	gi.dprintf("L4D_UpdateSpectatorLight called: target: %s, mode: %d\n", (ent->client->chase_target != NULL ? "yes" : "no"), ent->client->chase_mode );

	if(ent->client->chase_target) {
		if(ent->client->chase_target->client->resp.team == TEAM1)
			L4D_ZombieLights(ent);
		else
			L4D_HumanLights(ent);
		return;
	}

	L4D_ResetLights(ent);
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
	if(team_round_going && ent->deadflag == DEAD_NO && ent->client->resp.team == TEAM1) {
		ent->client->ps.fov = 80;
		if(level.time > ent->client->l4d_nextsoundtime) {
			ent->client->l4d_nextsoundtime = level.time + rand() % 6 + 2;
			if(ent->client->l4d_nextsoundtime > 0)
				L4D_PlayRandomZombieSound(ent);
		}
	}

	// flashlight think
	if(!team_round_going)
		ent->client->l4d_fl_nextthink = level.time + L4D_FL_THINK;
	else if(ent->client->resp.team == TEAM2 && ent->client->l4d_fl_nextthink < level.time) {
		if(ent->flashlight) {
			// discharging
			if(ent->client->l4d_fl_charge > L4D_FL_EMPTY) {
				ent->client->l4d_fl_charge -= L4D_FL_DISCHARGE;
				if(ent->client->l4d_fl_charge < L4D_FL_EMPTY)
					ent->client->l4d_fl_charge = L4D_FL_EMPTY;
				ent->client->l4d_fl_nextthink = level.time + L4D_FL_THINK;
				gi.cprintf(ent, PRINT_HIGH, "Flashlight: %d/100\n",
						(int)(ent->client->l4d_fl_charge / L4D_FL_FULL * 100));
			}

			if(ent->client->l4d_fl_charge <= L4D_FL_EMPTY) {
				L4D_ToggleFL(ent, false);
				gi.cprintf(ent, PRINT_HIGH, "Flashlight empty! Charging...\n", ent->client->l4d_fl_charge);
			}
		} else {
			// charging
			if(ent->client->l4d_fl_charge < L4D_FL_FULL) {
				ent->client->l4d_fl_charge += L4D_FL_CHARGE;
				if(ent->client->l4d_fl_charge > L4D_FL_FULL)
					ent->client->l4d_fl_charge = L4D_FL_FULL;
				ent->client->l4d_fl_nextthink = level.time + L4D_FL_THINK;
				gi.cprintf(ent, PRINT_HIGH, "Flashlight: %d/100\n",
						(int)(ent->client->l4d_fl_charge / L4D_FL_FULL * 100));
			}
		}
	}
}

void L4D_ToggleFL(edict_t *ent, qboolean on)
{
	if (!on)
	{
	       	if(ent->flashlight)
		{
			G_FreeEdict (ent->flashlight);
			ent->flashlight = NULL;
		}
		return;
	}

	FL_make(ent);
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
