extern cvar_t *l4d;

#define L4D_FL_FULL		10.0f
#define L4D_FL_EMPTY		0.0f
#define L4D_FL_MIN		2.0f
#define L4D_FL_THINK		1.0f	/* think every second */
#define L4D_FL_CHARGE		1.25f	/* charge rate */
#define L4D_FL_DISCHARGE	1.0f	/* discharge rate */

void L4D_Init();
void L4D_RoundEnd();
void L4D_EquipClient(edict_t *ent);
void L4d_PlayerDie(edict_t *ent);
void L4D_Think(edict_t *ent);
void L4D_ToggleFL(edict_t *ent, qboolean on);
void L4D_HumanLights(edict_t *ent);
void L4D_ZombieLights(edict_t *ent);
void L4D_ResetLights(edict_t *ent);
void L4D_UpdateSpectatorLights(edict_t *ent);
void L4D_UnicastConfigString(edict_t *, int, char *);
