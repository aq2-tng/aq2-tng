//-----------------------------------------------------------------------------
// g_svcmds.c
//
// $Id: g_svcmds.c,v 1.1 2001/05/06 17:31:45 igor_rock Exp $
//
//-----------------------------------------------------------------------------
// $Log: g_svcmds.c,v $
// Revision 1.1  2001/05/06 17:31:45  igor_rock
// Initial revision
//
//-----------------------------------------------------------------------------

#include "g_local.h"

void SVCmd_ReloadMOTD_f()
{
	ReadMOTDFile();
	gi.cprintf(NULL, PRINT_HIGH, "MOTD reloaded.\n");
}

/*
==============================================================================

PACKET FILTERING
 

You can add or remove addresses from the filter list with:

addip <ip>
removeip <ip>

The ip address is specified in dot format, and any unspecified digits will match any value, so you can specify an entire class C network with "addip 192.246.40".

Removeip will only remove an address specified exactly the same way.  You cannot addip a subnet, then removeip a single host.

listip
Prints the current list of filters.

writeip
Dumps "addip <ip>" commands to listip.cfg so it can be execed at a later date.  The filter lists are not saved and restored by default, because I beleive it would cause too much confusion.

filterban <0 or 1>

If 1 (the default), then ip addresses matching the current list will be prohibited from entering the game.  This is the default setting.

If 0, then only addresses matching the list will be allowed.  This lets you easily set up a private game, or a game that only allows players from your local network.


==============================================================================
*/

typedef struct
{
      unsigned        mask;
      unsigned        compare;

//AZEROV
      int             temp_ban_games;
//AZEROV
} ipfilter_t;

#define       MAX_IPFILTERS   1024

ipfilter_t    ipfilters[MAX_IPFILTERS];
int                   numipfilters;

/*
=================
StringToFilter
=================
*/
static qboolean StringToFilter (char *s, ipfilter_t *f, int temp_ban_games)
{
      char    num[128];
      int             i, j;
      byte    b[4];
      byte    m[4];
      
      for (i=0 ; i<4 ; i++)
      {
              b[i] = 0;
              m[i] = 0;
      }
      
      for (i=0 ; i<4 ; i++)
      {
              if (*s < '0' || *s > '9')
              {
                      gi.cprintf(NULL, PRINT_HIGH, "Bad filter address: %s\n", s);
                      return false;
              }
              
              j = 0;
              while (*s >= '0' && *s <= '9')
              {
                      num[j++] = *s++;
              }
              num[j] = 0;
              b[i] = atoi(num);
              if (b[i] != 0)
                      m[i] = 255;

              if (!*s)
                      break;
              s++;
      }
      
      f->mask = *(unsigned *)m;
      f->compare = *(unsigned *)b;
      
	  f->temp_ban_games = temp_ban_games;

      return true;
}

/*
=================
SV_FilterPacket
=================
*/
qboolean SV_FilterPacket (char *from)
{
      int             i;
      unsigned        in;
      byte m[4];
      char *p;

      i = 0;
      p = from;
      while (*p && i < 4) {
              m[i] = 0;
              while (*p >= '0' && *p <= '9') {
                      m[i] = m[i]*10 + (*p - '0');
                      p++;
              }
              if (!*p || *p == ':')
                      break;
              i++, p++;
      }
      
      in = *(unsigned *)m;

      for (i=0 ; i<numipfilters ; i++)
              if ( (in & ipfilters[i].mask) == ipfilters[i].compare)
                      return (int)filterban->value;

      return (int)!filterban->value;
}


/*
=================
SV_AddIP_f
=================
*/
void SVCmd_AddIP_f (void)
{
      int             i;
      
      if (gi.argc() < 3) {
              gi.cprintf(NULL, PRINT_HIGH, "Usage:  addip <ip-mask>\n");
              return;
      }

      for (i=0 ; i<numipfilters ; i++)
              if (ipfilters[i].compare == 0xffffffff)
                      break;          // free spot
      if (i == numipfilters)
      {
              if (numipfilters == MAX_IPFILTERS)
              {
                      gi.cprintf (NULL, PRINT_HIGH, "IP filter list is full\n");
                      return;
              }
              numipfilters++;
      }
      
      if (!StringToFilter (gi.argv(2), &ipfilters[i], 0))
              ipfilters[i].compare = 0xffffffff;
}

/*
=================
SV_RemoveIP_f
=================
*/
void SVCmd_RemoveIP_f (void)
{
      ipfilter_t      f;
      int                     i, j;

      if (gi.argc() < 3) {
              gi.cprintf(NULL, PRINT_HIGH, "Usage:  sv removeip <ip-mask>\n");
              return;
      }

      if (!StringToFilter (gi.argv(2), &f, 0))
              return;

      for (i=0 ; i<numipfilters ; i++)
              if (ipfilters[i].mask == f.mask
              && ipfilters[i].compare == f.compare)
              {
                      for (j=i+1 ; j<numipfilters ; j++)
                              ipfilters[j-1] = ipfilters[j];
                      numipfilters--;
                      gi.cprintf (NULL, PRINT_HIGH, "Removed.\n");
                      return;
              }
      gi.cprintf (NULL, PRINT_HIGH, "Didn't find %s.\n", gi.argv(2));
}

/*
=================
SV_ListIP_f
=================
*/
void SVCmd_ListIP_f (void)
{
      int             i;
      byte    b[4];

      gi.cprintf (NULL, PRINT_HIGH, "Filter list:\n");
      for (i=0 ; i<numipfilters ; i++)
      {
           *(unsigned *)b = ipfilters[i].compare;
           if (!ipfilters[i].temp_ban_games)
           {
                gi.cprintf (NULL, PRINT_HIGH, "%3i.%3i.%3i.%3i\n", b[0], b[1], b[2], b[3]);
           }
           else
           {
                gi.cprintf (NULL, PRINT_HIGH, "%3i.%3i.%3i.%3i (%d more game(s))\n", b[0], b[1], b[2], b[3], ipfilters[i].temp_ban_games);
           }
       }
}

/*
=================
SV_WriteIP_f
=================
*/
void SVCmd_WriteIP_f (void)
{
      FILE    *f;
      char    name[MAX_OSPATH];
      byte    b[4];
      int             i;
      cvar_t  *game;

      game = gi.cvar("game", "", 0);

      if (!*game->string)
              sprintf (name, "%s/listip.cfg", GAMEVERSION);
      else
              sprintf (name, "%s/listip.cfg", game->string);

      gi.cprintf (NULL, PRINT_HIGH, "Writing %s.\n", name);

      f = fopen (name, "wb");
      if (!f)
      {
              gi.cprintf (NULL, PRINT_HIGH, "Couldn't open %s\n", name);
              return;
      }
      
      fprintf(f, "set filterban %d\n", (int)filterban->value);

      for (i=0 ; i<numipfilters ; i++)
      {
              if (!ipfilters[i].temp_ban_games)
              {
                       *(unsigned *)b = ipfilters[i].compare;
                       fprintf (f, "sv addip %i.%i.%i.%i\n", b[0], b[1], b[2], b[3]);
              }
      }
      
      fclose (f);
}

// zucc so it works under vc++
void ExitLevel (void);

//Black Cross - Begin
/*
=================
SV_Nextmap_f
=================
*/
void SVCmd_Nextmap_f (char *arg)
{
  // end level and go to next map in map rotation
  gi.bprintf(PRINT_HIGH, "Changing to next map in rotation.\n");
  EndDMLevel ();
  if (arg != NULL && Q_stricmp(arg, "force") == 0)
      ExitLevel ();
  return;
}
//Black Cross - End


/*
=================
ServerCommand

ServerCommand will be called when an "sv" command is issued.
The game can issue gi.argc() / gi.argv() commands to get the rest
of the parameters
=================
*/
void    ServerCommand (void)
{
        char    *cmd;

        cmd = gi.argv(1);

        if (Q_stricmp (cmd, "addip") == 0)
                SVCmd_AddIP_f ();
        else if (Q_stricmp (cmd, "removeip") == 0)
                SVCmd_RemoveIP_f ();
        else if (Q_stricmp (cmd, "listip") == 0)
                SVCmd_ListIP_f ();
        else if (Q_stricmp (cmd, "writeip") == 0)
                SVCmd_WriteIP_f ();
		else if (Q_stricmp (cmd, "nextmap") == 0)
				SVCmd_Nextmap_f (gi.argv(2)); // Added by Black Cross
		else if (Q_stricmp (cmd, "reloadmotd") == 0)
				SVCmd_ReloadMOTD_f();
        else
				gi.cprintf (NULL, PRINT_HIGH, "Unknown server command \"%s\"\n", cmd);
}


/*
==========================
Kick a client entity
==========================
*/
void Kick_Client (edict_t *ent)
{
	int i = 0;
	char ban_string[256];
	edict_t *entL;

	if (!ent->client)
	{
		return;
	}

	// We used to kick on names, but people got crafty and figured
	// out that putting in a space after their name let them get
	// around the stupid 'kick' function. So now we kick by number.
	for (i=0 ; i<game.maxclients ; i++)
	{
		entL = &g_edicts[1+i];
		if (!entL || !entL->inuse)
			continue;
		if (entL->client && ent == entL)
		{
			sprintf (ban_string, "kick %d\n", i);
			gi.AddCommandString (ban_string);
		}
	}
}

/*
==========================
Ban a client for N rounds
==========================
*/
qboolean Ban_TeamKiller ( edict_t *ent, int rounds )
{
	int i = 0;

	if (!ent || !ent->client || !ent->client->ipaddr)
	{
		gi.cprintf (NULL, PRINT_HIGH, "Unable to determine client->ipaddr for edict\n");
		return false;
	}

	for (i=0 ; i<numipfilters ; i++)
	{
		if (ipfilters[i].compare == 0xffffffff)
			break;          // free spot
	}

	if (i == numipfilters)
	{
		if (numipfilters == MAX_IPFILTERS)
		{
			gi.cprintf (NULL, PRINT_HIGH, "IP filter list is full\n");
			return false;
		}
		numipfilters++;
	}
	if (!StringToFilter (ent->client->ipaddr, &ipfilters[i], rounds))
	{
		ipfilters[i].compare = 0xffffffff;
		return false;
	}
	
	return true;
}

void UnBan_TeamKillers (void)
{
	// We don't directly unban them all - we subtract 1 from temp_ban_games,
	// and unban them if it's 0.

	int i, j;

	for (i=0 ; i<numipfilters ; i++)
	{
		if (ipfilters[i].temp_ban_games > 0)
		{
			if (!--ipfilters[i].temp_ban_games)
			{
				// re-pack the filters
				for (j=i+1 ; j<numipfilters ; j++)
					  ipfilters[j-1] = ipfilters[j];
				numipfilters--;
				gi.cprintf (NULL, PRINT_HIGH, "Unbanned teamkiller.\n");

				// since we removed the current we have to re-process the new current
				i--;
			}
		}
	}
}
//AZEROV

