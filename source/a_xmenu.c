//-----------------------------------------------------------------------------
// a_xmenu.c
//
// $Id: a_xmenu.c,v 1.2 2001/09/28 13:48:34 ra Exp $
//
//-----------------------------------------------------------------------------
// $Log: a_xmenu.c,v $
// Revision 1.2  2001/09/28 13:48:34  ra
// I ran indent over the sources. All .c and .h files reindented.
//
// Revision 1.1.1.1  2001/05/06 17:25:35  igor_rock
// This is the PG Bund Edition V1.25 with all stuff laying around here...
//
//-----------------------------------------------------------------------------

#include "g_local.h"

#define X_MENU ent->x_menu

static XMENU_TITLE xRaw[] = {
  "€‚",		// double line
  "previous page",
  "next page",
  "Use [ and ] to move cursor",
  "ENTER to select, TAB to exit",
  "",	// single line
  "€‚"	// double line
};

qboolean xMenu_Add (edict_t * ent, char *name,
	   void (*SelectFunc) (edict_t * ent, pmenu_t * p))
{
	if (X_MENU->xmenucount < XMENU_TOTAL_ENTRIES)
	{
		if (name)
			Q_strncpyz(X_MENU->xmenuentries[X_MENU->xmenucount].name, name, XMENU_TITLE_MAX);
		else
			X_MENU->xmenuentries[X_MENU->xmenucount].name[0] = '\0';
		X_MENU->xmenuentries[X_MENU->xmenucount].SelectFunc = SelectFunc;
		X_MENU->xmenucount++;
	}

	return (X_MENU->xmenucount < XMENU_TOTAL_ENTRIES);
}


void xMenu_Set (edict_t * ent);

void xMenu_Next (edict_t * ent, pmenu_t * p)
{
	X_MENU->xmenucount = 2;
	X_MENU->DoAddMenu (ent, X_MENU->xmenutop + XMENU_MAX_ENTRIES);
	if (X_MENU->xmenucount > 2)
	{
		X_MENU->xmenutop += XMENU_MAX_ENTRIES;
		xMenu_Set (ent);

		PMenu_Close (ent);
		PMenu_Open (ent, X_MENU->themenu, 5, XMENU_END_ENTRY);
		//PMenu_Update(ent);
	}
}

void xMenu_Prev (edict_t * ent, pmenu_t * p)
{
	int temptop;

	X_MENU->xmenucount = 2;
	if (X_MENU->xmenutop < XMENU_MAX_ENTRIES)
		temptop = 0;
	else
		temptop = X_MENU->xmenutop - XMENU_MAX_ENTRIES;
	X_MENU->DoAddMenu (ent, temptop);
	if (X_MENU->xmenucount > 2)
	{
		X_MENU->xmenutop = temptop;
		xMenu_Set (ent);

		PMenu_Close (ent);
		PMenu_Open (ent, X_MENU->themenu, 5, XMENU_END_ENTRY);
		//PMenu_Update(ent);
	}
}

void xMenu_Set (edict_t * ent)
{
	int i;

	//setting title
	X_MENU->themenu[0].text = X_MENU->xmenuentries[0].name;
	X_MENU->themenu[0].align = PMENU_ALIGN_CENTER;
	X_MENU->themenu[1].text = NULL;
	X_MENU->themenu[1].align = PMENU_ALIGN_CENTER;
	X_MENU->themenu[2].text = X_MENU->xmenuentries[1].name;
	X_MENU->themenu[2].align = PMENU_ALIGN_CENTER;
	X_MENU->themenu[3].text = xRaw[5];
	X_MENU->themenu[3].align = PMENU_ALIGN_CENTER;

	//setting bottom
	X_MENU->themenu[XMENU_END_ENTRY - 3].text = xRaw[5];
	X_MENU->themenu[XMENU_END_ENTRY - 3].align = PMENU_ALIGN_CENTER;
	X_MENU->themenu[XMENU_END_ENTRY - 2].text = xRaw[3];
	X_MENU->themenu[XMENU_END_ENTRY - 2].align = PMENU_ALIGN_CENTER;
	X_MENU->themenu[XMENU_END_ENTRY - 1].text = xRaw[4];
	X_MENU->themenu[XMENU_END_ENTRY - 1].align = PMENU_ALIGN_CENTER;

	for (i = 2; i < X_MENU->xmenucount; i++)
	{
		X_MENU->themenu[i + 3].text = X_MENU->xmenuentries[i].name;
		X_MENU->themenu[i + 3].SelectFunc = X_MENU->xmenuentries[i].SelectFunc;
	}
	while (i < XMENU_TOTAL_ENTRIES)
	{
		X_MENU->themenu[i + 3].text = NULL;
		X_MENU->themenu[i + 3].SelectFunc = NULL;
		i++;
	}

	if (X_MENU->xmenucount == XMENU_TOTAL_ENTRIES)
	{
		//next must be enabled
		X_MENU->themenu[XMENU_END_ENTRY - 5].text = xRaw[2];
		X_MENU->themenu[XMENU_END_ENTRY - 5].SelectFunc = xMenu_Next;
	}
	else
	{
		X_MENU->themenu[XMENU_END_ENTRY - 5].text = NULL;
		X_MENU->themenu[XMENU_END_ENTRY - 5].SelectFunc = NULL;
	}
	if (X_MENU->xmenutop)
	{
		//prev must be enabled
		X_MENU->themenu[XMENU_END_ENTRY - 6].text = xRaw[1];
		X_MENU->themenu[XMENU_END_ENTRY - 6].SelectFunc = xMenu_Prev;
	}
	else
	{
		X_MENU->themenu[XMENU_END_ENTRY - 6].text = NULL;
		X_MENU->themenu[XMENU_END_ENTRY - 6].SelectFunc = NULL;
	}

}

qboolean
xMenu_New (edict_t * ent, char *title, char *subtitle,
	   void (*DoAddMenu) (edict_t * ent, int fromix))
{
	if (!DoAddMenu)
		return false;

	if (!X_MENU)
	{
		//no menu yet, allocate memory for it...
		X_MENU = gi.TagMalloc (sizeof (xmenu_t), TAG_GAME);
		if (!X_MENU)
		{
			gi.dprintf ("Error allocating xmenu space\n");
			return false;
		}
	}
	X_MENU->DoAddMenu = DoAddMenu;
	X_MENU->xmenucount = 2;
	X_MENU->xmenutop = 0;
	//memset(xmenuentries, 0, sizeof(xmenuentries));
	strcpy (X_MENU->xmenuentries[0].name, "*");
	if (title)
		Q_strncatz(X_MENU->xmenuentries[0].name, title, XMENU_TITLE_MAX);
	else
		Q_strncatz(X_MENU->xmenuentries[0].name, "Menu", XMENU_TITLE_MAX);
	if (subtitle)
		Q_strncpyz(X_MENU->xmenuentries[1].name, subtitle, XMENU_TITLE_MAX);
	else
		Q_strncpyz(X_MENU->xmenuentries[1].name, "make your choice", XMENU_TITLE_MAX);

	X_MENU->xmenuentries[0].SelectFunc = NULL;
	X_MENU->xmenuentries[1].SelectFunc = NULL;

	DoAddMenu (ent, 0);
	if (X_MENU->xmenucount > 2)
	{
		xMenu_Set (ent);

		if (ent->client->menu)
			PMenu_Close (ent);

		PMenu_Open (ent, X_MENU->themenu, 5, XMENU_END_ENTRY);
		return true;
	}

	return false;
}
