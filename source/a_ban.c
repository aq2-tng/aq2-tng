//-----------------------------------------------------------------------------
// a_ban.c
//
// $Id: a_ban.c,v 1.2 2001/09/28 13:48:34 ra Exp $
//
//-----------------------------------------------------------------------------
// $Log: a_ban.c,v $
// Revision 1.2  2001/09/28 13:48:34  ra
// I ran indent over the sources. All .c and .h files reindented.
//
// Revision 1.1.1.1  2001/05/06 17:24:07  igor_rock
// This is the PG Bund Edition V1.25 with all stuff laying around here...
//
//-----------------------------------------------------------------------------

#include "g_local.h"

#define BANLISTSIZE 32
#define BANSTRLEN 128

typedef char banstr[BANSTRLEN];
typedef banstr banlist_t[BANLISTSIZE];

banlist_t banlist;
int banlistix;


void
InitBanList (void)
{
  banlistix = 0;
  memset (banlist, 0, sizeof (banlist_t));
}

qboolean NameInUse (edict_t * self, char *s)
{
  int i;
  edict_t *other;

  for (i = 1; i <= game.maxclients; i++)
    {
      other = &g_edicts[i];
      if (other->client && other != self && other->inuse)
	{
	  if (stricmp (s, other->client->pers.netname) == 0)
	    return true;
	}
    }

  return false;
}

char *
GetDummyName (edict_t * self, char *s)
{
  int i;

  i = 1;
  do
    {
      sprintf (s, "dummy%i", i++);
    }
  while (NameInUse (self, s) == true);
  return s;
}

void
MakeLegalName (edict_t * self, char *s)
{
  char *p;

  if (!*s)
    {
      s = GetDummyName (self, s);
      return;
    }
  if (*s == ' ')
    {
      *s = '!';
    }

  p = s + strlen (s) - 1;
  if (*p == ' ')
    {
      *p = '!';
    }
}

qboolean ipstr (char *s)
{
  int i;
  char *p;

  // TempFile
  if (!Q_stricmp (s, "loopback"))
    return true;

  i = 0;
  p = s;
  while (*p != '\0' && i < 3)
    {
      if (*p == '.')
	i++;
      if (i < 3)
	p++;
    }

  if (*p == '.')
    {
      *p = '\0';
      return true;
    }
  return false;
}

qboolean AddToBanList (char *ip)
{
  if (ipstr (ip) == false);
  //gi.dprintf("banned ip: %s\n", ip);
  strncpy (banlist[banlistix++], ip, BANSTRLEN);
  if (banlistix >= BANLISTSIZE)
    banlistix = 0;
  return true;
}

qboolean IsOnBanList (char *ip)
{
  int i;

  if (!ipstr (ip))
    return true;
  for (i = 0; i < BANLISTSIZE; i++)
    {
      if (strcmp (banlist[i], ip) == 0)
	return true;
    }
  return false;
}

qboolean Checkban (edict_t * ent, char *userinfo)
{
  char *p;
  banstr ipaddr;

  p = Info_ValueForKey (userinfo, "ip");
  strncpy (ipaddr, p, BANSTRLEN);
  if (IsOnBanList (ipaddr) == true)
    {
      Info_SetValueForKey (userinfo, "rejmsg", "Temporarily banned.");
      return false;
    }
  return true;
}

void
DoBan (edict_t * target, char *msg)
{
  char *p;
  char userinfo[MAX_INFO_STRING];

  if (!target || !target->inuse || !target->client)
    return;
  strcpy (userinfo, target->client->resp.coop_respawn.userinfo);
  p = Info_ValueForKey (userinfo, "ip");
  AddToBanList (p);
  KickClient (target, msg);
}

char *
_stripname (char *source, char *dest)
{
  char *p, buf[512];

  strcpy (buf, source);
  p = buf;
  if (*p == '\"')
    {
      while (*p)
	p++;
      while (p != buf && *p != '\"')
	p--;
      *p = '\0';
      p = buf;
      p++;
    }
  else
    {
      while (*p && *p != ' ')
	p++;
      *p = '\0';
      p = buf;
    }
  strcpy (dest, p);
  return dest;
}

// test functions
void
pg_ipinfo (edict_t * ent, char *s)
{
  char *p;
  char userinfo[MAX_INFO_STRING];
  edict_t *other;

  other = FindClientByPersName (_stripname (s, s));
  if (other)
    {
      strcpy (userinfo, other->client->resp.coop_respawn.userinfo);
      p = Info_ValueForKey (userinfo, "ip");
      gi.cprintf (ent, PRINT_MEDIUM, "%s: %s\n", s, p);
    }
  else
    gi.cprintf (ent, PRINT_MEDIUM, "No such player: %s\n", s);
}

void
pg_testban (edict_t * ent, char *s)
{
  char *p;
  char userinfo[MAX_INFO_STRING];
  edict_t *other;

  other = FindClientByPersName (_stripname (s, s));
  if (other)
    {
      strcpy (userinfo, other->client->resp.coop_respawn.userinfo);
      p = Info_ValueForKey (userinfo, "ip");
      AddToBanList (p);
      KickClient (other, "User is banned.");
    }
  else
    gi.cprintf (ent, PRINT_MEDIUM, "No such player: %s\n", s);
}

void
pg_hardban (edict_t * ent, char *s)
{
  char *p, userinfo[MAX_INFO_STRING], msg[128];
  edict_t *other;

  p = s;
  if (p)
    while (*p != '\0' && *p != '/')
      p++;

  if (*p == '/')
    {
      *p = '\0';
      p++;
      sprintf (msg, "error \"%s\"\n", p);
    }
  else
    strcpy (msg, "error \"plz behave!\"\n");

  other = FindClientByPersName (_stripname (s, s));
  if (other)
    {
      strcpy (userinfo, other->client->resp.coop_respawn.userinfo);
      p = Info_ValueForKey (userinfo, "ip");
      AddToBanList (p);
      stuffcmd (other, msg);
    }
  else
    gi.cprintf (ent, PRINT_MEDIUM, "No such player: %s\n", s);
}
