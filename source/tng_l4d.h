extern cvar_t *l4d;

void L4D_Init();
void L4D_RoundEnd();
void L4D_EquipClient(edict_t *ent);
void L4d_PlayerDie(edict_t *ent);
void L4D_Think(edict_t *ent);
void L4D_ResetLights(edict_t *ent);
qboolean L4d_Flashlight(edict_t *ent);
void L4D_UnicastConfigString(edict_t *, int, char *);
