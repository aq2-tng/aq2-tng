//-----------------------------------------------------------------------------
// banning stuff
//
// $Id: a_ban.h,v 1.1 2001/05/06 17:24:07 igor_rock Exp $
//
//-----------------------------------------------------------------------------
// $Log: a_ban.h,v $
// Revision 1.1  2001/05/06 17:24:07  igor_rock
// Initial revision
//
//-----------------------------------------------------------------------------

void InitBanList (void);
qboolean AddToBanList (char *ip);
qboolean Checkban (edict_t * ent, char *userinfo);
void DoBan (edict_t * target, char *msg);
qboolean NameInUse (edict_t * self, char *s);
char *GetDummyName (edict_t * self, char *s);
void MakeLegalName (edict_t * self, char *s);

//zu löschen!!:
void pg_testban (edict_t * ent, char *s);
void pg_hardban (edict_t * ent, char *s);
void pg_ipinfo (edict_t * ent, char *s);
