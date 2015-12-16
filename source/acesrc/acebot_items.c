///////////////////////////////////////////////////////////////////////
//
//  ACE - Quake II Bot Base Code
//
//  Version 1.0
//
//  Original file is Copyright(c), Steve Yeager 1998, All Rights Reserved
//
//
//	All other files are Copyright(c) Id Software, Inc.
////////////////////////////////////////////////////////////////////////
/*
 * $Header: /LTK2/src/acesrc/acebot_items.c 6     29/02/00 11:11 Riever $
 *
 * $History: acebot_items.c $
 * 
 * *****************  Version 6  *****************
 * User: Riever       Date: 29/02/00   Time: 11:11
 * Updated in $/LTK2/src/acesrc
 * Added ability to collect more knives.
 * 
 * *****************  Version 5  *****************
 * User: Riever       Date: 24/02/00   Time: 19:59
 * Updated in $/LTK2/src/acesrc
 * Fixed item handling (Broken by new node-height code.
 * 
 * *****************  Version 4  *****************
 * User: Riever       Date: 23/02/00   Time: 23:17
 * Updated in $/LTK2/src/acesrc
 * New door node creation implemented and doors removed from item table.
 * 
 * *****************  Version 3  *****************
 * User: Riever       Date: 23/02/00   Time: 17:24
 * Updated in $/LTK2/src/acesrc
 * Added support for 'sv shownodes on/off'
 * Enabled creation of nodes for ALL doors. (Stage 1 of new method)
 * 
 * *****************  Version 2  *****************
 * User: Riever       Date: 17/02/00   Time: 17:53
 * Updated in $/LTK2/src/acesrc
 * Fixed item list to be in the right order!
 * 
 */

///////////////////////////////////////////////////////////////////////
//
//  acebot_items.c - This file contains all of the 
//                   item handling routines for the 
//                   ACE bot, including fact table support
//           
///////////////////////////////////////////////////////////////////////

#include "../g_local.h"

#include "acebot.h"

int	num_players = 0;
int num_items = 0;
item_table_t item_table[MAX_EDICTS];
edict_t *players[MAX_CLIENTS];		// pointers to all players in the game

///////////////////////////////////////////////////////////////////////
// Add the player to our list
///////////////////////////////////////////////////////////////////////
void ACEIT_PlayerAdded(edict_t *ent)
{
	players[num_players++] = ent;
}

///////////////////////////////////////////////////////////////////////
// Remove player from list
///////////////////////////////////////////////////////////////////////
void ACEIT_PlayerRemoved(edict_t *ent)
{
	int i = 0;
	int pos = 0;

	// watch for 0 players
	if(num_players == 0)
		return;

	// special cas for only one player
	if(num_players == 1)
	{	
		num_players = 0;
		return;
	}

	// Find the player
	for(i=0;i<num_players;i++)
		if(ent == players[i])
			pos = i;

	// decrement
	for(i=pos;i<num_players-1;i++)
		players[i] = players[i+1];

	num_players--;
}

///////////////////////////////////////////////////////////////////////
// Can we get there?
///////////////////////////////////////////////////////////////////////
qboolean ACEIT_IsReachable(edict_t *self, vec3_t goal)
{
	trace_t trace;
	vec3_t v;

	VectorCopy(self->mins,v);
	v[2] += 18; // Stepsize

//AQ2	trace = gi.trace (self->s.origin, v, self->maxs, goal, self, MASK_OPAQUE);
	trace = gi.trace (self->s.origin, v, self->maxs, goal, self, MASK_SOLID|MASK_OPAQUE);
	
	// Yes we can see it
	if (trace.fraction == 1.0)
		return true;
	else
		return false;

}

///////////////////////////////////////////////////////////////////////
// Visiblilty check 
///////////////////////////////////////////////////////////////////////
qboolean ACEIT_IsVisible(edict_t *self, vec3_t goal)
{
	trace_t trace;
	
//AQ2	trace = gi.trace (self->s.origin, vec3_origin, vec3_origin, goal, self, MASK_OPAQUE);
	trace = gi.trace (self->s.origin, vec3_origin, vec3_origin, goal, self, MASK_SOLID|MASK_OPAQUE);
	
	// Yes we can see it
	if (trace.fraction == 1.0)
		return true;
	else
		return false;

}

///////////////////////////////////////////////////////////////////////
//  Weapon changing support
///////////////////////////////////////////////////////////////////////
//AQ2 CHANGE
// Completely rewritten for AQ2!
//
qboolean ACEIT_ChangeWeapon (edict_t *ent, gitem_t *item)
{
	int			ammo_index;
	gitem_t		*ammo_item;
	qboolean	loaded = true;
	qboolean	clips = true;

	// Has not picked up weapon yet
	if(!ent->client->pers.inventory[ITEM_INDEX(item)])
		return false;

	// Do we have ammo for it?
	if (item->ammo)
	{
		ammo_item = FindItem(item->ammo);
		ammo_index = ITEM_INDEX(ammo_item);
		if (!ent->client->pers.inventory[ammo_index] )//&& !g_select_empty->value)
			clips = false;
		else
			clips = true;
	}

	// see if we're already using it
	if (item == ent->client->pers.weapon)
	{
		//Do we have one in the chamber?
        if( ent->client->weaponstate == WEAPON_END_MAG)
		{
			loaded = false;
		}
		// No ammo - forget it!
		if(!loaded && !clips)
			return false;
		// If it's not loaded - use a new clip
		else if( !loaded )
			Cmd_New_Reload_f ( ent );
		return true;
	}

	// Change to this weapon
	ent->client->newweapon = item;
	ChangeWeapon( ent );
	
	return true;
}

//===============================
// Handling for MK23 (debugging)
//===============================
qboolean ACEIT_ChangeMK23SpecialWeapon (edict_t *ent, gitem_t *item)
{
	int			ammo_index;
	gitem_t		*ammo_item;
	qboolean	loaded = true;
	qboolean	clips = true;

	// Has not picked up weapon yet
	if(!ent->client->pers.inventory[ITEM_INDEX(item)])
	{
//		safe_bprintf(PRINT_HIGH,"Not got MP23\n");
		return false;
	}

	// Do we have ammo for it?
	if (item->ammo)
	{
		ammo_item = FindItem(item->ammo);
		ammo_index = ITEM_INDEX(ammo_item);
		if (ent->client->pers.inventory[ammo_index] < 1)
			clips = false;
		else
			clips = true;
	}
	// see if we're already using it
	if (item == ent->client->pers.weapon)
	{
		//Do we have one in the chamber?
        if( ent->client->weaponstate == WEAPON_END_MAG)
		{
			loaded = false;
		}
		// No ammo - forget it!
		if(!loaded && !clips)
		{
//			safe_bprintf(PRINT_HIGH,"Not got MK23 Ammo\n");
			return false;
		}
		// If it's not loaded - use a new clip
		else if( !loaded )
		{
//			safe_bprintf(PRINT_HIGH,"Need to reload MK23\n");
			Cmd_New_Reload_f ( ent );
		}
		return true;
	}

	// Not current weapon, check ammo
	if( (ent->client->mk23_rds < 1) && !clips )
	{
//		safe_bprintf(PRINT_HIGH,"No change - No MK23 Ammo\n");
		return false;
	}

	// Change to this weapon
//	safe_bprintf(PRINT_HIGH,"Changing to MK23\n");
	ent->client->newweapon = item;
	ChangeWeapon ( ent );
	return true;
}

//===============================
// Handling for HC (debugging)
//===============================
qboolean ACEIT_ChangeHCSpecialWeapon (edict_t *ent, gitem_t *item)
{
	int			ammo_index;
	gitem_t		*ammo_item;
	qboolean	loaded = true;
	qboolean	clips = true;

	// Has not picked up weapon yet
	if(!ent->client->pers.inventory[ITEM_INDEX(item)])
	{
//		safe_bprintf(PRINT_HIGH,"Not got HC\n");
		return false;
	}

	// Do we have ammo for it?
	if (item->ammo)
	{
		ammo_item = FindItem(item->ammo);
		ammo_index = ITEM_INDEX(ammo_item);
		if (ent->client->pers.inventory[ammo_index] < 2)
			clips = false;
		else
			clips = true;
	}
	// Check ammo
	if( (ent->client->cannon_rds < 2) )
	{
//		safe_bprintf(PRINT_HIGH,"No change - No HC Ammo\n");
		loaded = false;
	}
	// see if we're already using it
	if (item == ent->client->pers.weapon)
	{
/*		//Do we have one in the chamber?
        if( ent->client->weaponstate == WEAPON_END_MAG)
		{
			loaded = false;
		}*/
		// No ammo - forget it!
		if(!loaded && !clips)
		{
//			safe_bprintf(PRINT_HIGH,"Not got HC Ammo\n");
			DropSpecialWeapon ( ent );
			return false;
		}
		// If it's not loaded - use a new clip
		else if( !loaded )
		{
//			safe_bprintf(PRINT_HIGH,"Need to reload HC\n");
			Cmd_New_Reload_f ( ent );
		}
		return true;
	}

	// Not current weapon, check ammo
	if( !loaded && !clips )
	{
//		safe_bprintf(PRINT_HIGH,"No change - No HC Ammo\n");
		return false;
	}

	// Change to this weapon
//	safe_bprintf(PRINT_HIGH,"Changing to HandCannon\n");
	ent->client->newweapon = item;
	ChangeWeapon ( ent );
	return true;
}

//===============================
// Handling for Sniper Rifle (debugging)
//===============================
qboolean ACEIT_ChangeSniperSpecialWeapon (edict_t *ent, gitem_t *item)
{
	int			ammo_index;
	gitem_t		*ammo_item;
	qboolean	loaded = true;
	qboolean	clips = true;

	// Has not picked up weapon yet
	if(!ent->client->pers.inventory[ITEM_INDEX(item)])
	{
//		safe_bprintf(PRINT_HIGH,"Not got Sniper Rifle\n");
		return false;
	}

	// Do we have ammo for it?
	if (item->ammo)
	{
		ammo_item = FindItem(item->ammo);
		ammo_index = ITEM_INDEX(ammo_item);
		if (ent->client->pers.inventory[ammo_index] < 1)
			clips = false;
		else
			clips = true;
	}
	// Check ammo
	if( (ent->client->sniper_rds < 1) )
	{
//		safe_bprintf(PRINT_HIGH,"No change - No Sniper Ammo\n");
		loaded = false;
	}
	// see if we're already using it
	if (item == ent->client->pers.weapon)
	{
/*		//Do we have one in the chamber?
        if( ent->client->weaponstate == WEAPON_END_MAG)
		{
			loaded = false;
		}*/
		// No ammo - forget it!
		if(!loaded && !clips)
		{
//			safe_bprintf(PRINT_HIGH,"Not got Sniper Ammo\n");
			DropSpecialWeapon ( ent );
			return false;
		}
		// If it's not loaded - use a new clip
		else if( !loaded )
		{
//			safe_bprintf(PRINT_HIGH,"Need to reload Sniper Rifle\n");
			Cmd_New_Reload_f ( ent );
		}
		return true;
	}

	// No ammo
	if( !loaded && !clips )
	{
//		safe_bprintf(PRINT_HIGH,"No change - No Sniper Ammo\n");
		return false;
	}

	// Change to this weapon
//	safe_bprintf(PRINT_HIGH,"Changing to Sniper Rifle\n");
	ent->client->newweapon = item;
	ChangeWeapon ( ent );
	return true;
}
//===============================
// Handling for M3 (debugging)
//===============================
qboolean ACEIT_ChangeM3SpecialWeapon (edict_t *ent, gitem_t *item)
{
	int			ammo_index;
	gitem_t		*ammo_item;
	qboolean	loaded = true;
	qboolean	clips = true;

	// Has not picked up weapon yet
	if(!ent->client->pers.inventory[ITEM_INDEX(item)])
	{
//		safe_bprintf(PRINT_HIGH,"Not got M3\n");
		return false;
	}

	// Do we have ammo for it?
	if (item->ammo)
	{
		ammo_item = FindItem(item->ammo);
		ammo_index = ITEM_INDEX(ammo_item);
		if (ent->client->pers.inventory[ammo_index] < 1)
			clips = false;
		else
			clips = true;
	}
	// Check ammo
	if( (ent->client->shot_rds < 1) )
	{
//		safe_bprintf(PRINT_HIGH,"No change - No M3 Ammo\n");
		loaded = false;
	}
	// see if we're already using it
	if (item == ent->client->pers.weapon)
	{
/*		//Do we have one in the chamber?
        if( ent->client->weaponstate == WEAPON_END_MAG)
		{
			loaded = false;
		}*/
		// No ammo - forget it!
		if(!loaded && !clips)
		{
//			safe_bprintf(PRINT_HIGH,"Not got M3 Ammo\n");
			DropSpecialWeapon ( ent );
			return false;
		}
		// If it's not loaded - use a new clip
		else if( !loaded )
		{
//			safe_bprintf(PRINT_HIGH,"Need to reload M3\n");
			Cmd_New_Reload_f ( ent );
		}
		return true;
	}

	// No ammo
	if( !loaded && !clips )
	{
//		safe_bprintf(PRINT_HIGH,"No change - No M3 Ammo\n");
		return false;
	}

	// Change to this weapon
//	safe_bprintf(PRINT_HIGH,"Changing to M3\n");
	ent->client->newweapon = item;
	ChangeWeapon ( ent );
	return true;
}
//===============================
// Handling for M4 (debugging)
//===============================
qboolean ACEIT_ChangeM4SpecialWeapon (edict_t *ent, gitem_t *item)
{
	int			ammo_index;
	gitem_t		*ammo_item;
	qboolean	loaded = true;
	qboolean	clips = true;

	// Has not picked up weapon yet
	if(!ent->client->pers.inventory[ITEM_INDEX(item)])
	{
//		safe_bprintf(PRINT_HIGH,"Not got MP4\n");
		return false;
	}

	// Do we have ammo for it?
	if (item->ammo)
	{
		ammo_item = FindItem(item->ammo);
		ammo_index = ITEM_INDEX(ammo_item);
		if (ent->client->pers.inventory[ammo_index] < 1)//&& !g_select_empty->value)
			clips = false;
		else
			clips = true;
	}
	// Check ammo
	if( (ent->client->m4_rds < 1) )
	{
//		safe_bprintf(PRINT_HIGH,"No change - No M4 Ammo\n");
		loaded = false;
	}
	// see if we're already using it
	if (item == ent->client->pers.weapon)
	{
/*		//Do we have one in the chamber?
		if( ent->client->weaponstate == WEAPON_END_MAG)
		{
			loaded = false;
		}*/
		// No ammo - forget it!
		if(!loaded && !clips)
		{
//			safe_bprintf(PRINT_HIGH,"Not got M4 Ammo\n");
			DropSpecialWeapon ( ent );
			return false;
		}
		// If it's not loaded - use a new clip
		else if( !loaded )
		{
//			safe_bprintf(PRINT_HIGH,"Need to reload M4\n");
			Cmd_New_Reload_f ( ent );
		}
		return true;
	}

	// No ammo
	if( !loaded && !clips )
	{
//		safe_bprintf(PRINT_HIGH,"No change - No M4 Ammo\n");
		return false;
	}

	// Change to this weapon
//	safe_bprintf(PRINT_HIGH,"Changing to M4\n");
	ent->client->newweapon = item;
	ChangeWeapon ( ent );
	return true;
}
//===============================
// Handling for MP5 (debugging)
//===============================
qboolean ACEIT_ChangeMP5SpecialWeapon (edict_t *ent, gitem_t *item)
{
	int			ammo_index;
	gitem_t		*ammo_item;
	qboolean	loaded = true;
	qboolean	clips = true;

	// Has not picked up weapon yet
	if(!ent->client->pers.inventory[ITEM_INDEX(item)])
	{
//		safe_bprintf(PRINT_HIGH,"Not got MP5\n");
		return false;
	}

	// Do we have ammo for it?
	if (item->ammo)
	{
		ammo_item = FindItem(item->ammo);
//		ammo_item = FindItem(MP5_AMMO_NAME);
		ammo_index = ITEM_INDEX(ammo_item);
		if (ent->client->pers.inventory[ammo_index] < 1)
		{
//			safe_bprintf(PRINT_HIGH,"Out of MP5 Mags\n");
			clips = false;
		}
		else
			clips = true;
	}
	// check ammo
	if( (ent->client->mp5_rds < 1) )
	{
//		safe_bprintf(PRINT_HIGH,"No MP5 Ammo\n");
		loaded = false;
	}
	// see if we're already using it
	if (item == ent->client->pers.weapon)
	{
		// No ammo - forget it!
		if(!loaded && !clips)
		{
//			safe_bprintf(PRINT_HIGH,"Stopping MP5 use - no ammo\n");
			DropSpecialWeapon ( ent );
			return false;
		}
		// If it's not loaded - use a new clip
		else if( !loaded  && clips)
		{
//			safe_bprintf(PRINT_HIGH,"Need to reload MP5\n");
			Cmd_New_Reload_f ( ent );
		}
		return true;
	}

	// Not current weapon, check ammo
	if( !loaded && !clips )
	{
//		safe_bprintf(PRINT_HIGH,"No change - No MP5 Ammo\n");
		return false;
	}

	// Change to this weapon
//	safe_bprintf(PRINT_HIGH,"Changing to MP5\n");
	ent->client->newweapon = item;
	ChangeWeapon ( ent );
	return true;
}

//===============================
// Handling for DUAL PISTOLS (debugging)
//===============================
qboolean ACEIT_ChangeDualSpecialWeapon (edict_t *ent, gitem_t *item)
{
	int			ammo_index;
	gitem_t		*ammo_item;
	qboolean	loaded = true;
	qboolean	clips = true;

	// Has not picked up weapon yet
	if(!ent->client->pers.inventory[ITEM_INDEX(item)])
	{
//		safe_bprintf(PRINT_HIGH,"Not got Dual Pistols\n");
		return false;
	}

	// Do we have ammo for it?
	if (item->ammo)
	{
		ammo_item = FindItem(item->ammo);
		ammo_index = ITEM_INDEX(ammo_item);
		if (ent->client->pers.inventory[ammo_index] < 2)//&& !g_select_empty->value)
			clips = false;
		else
			clips = true;
	}
	// Not current weapon, check ammo
	if( (ent->client->dual_rds < 2) )
	{
//		safe_bprintf(PRINT_HIGH,"No change - No Dual Ammo\n");
		loaded = false;
	}

	// see if we're already using it
	if (item == ent->client->pers.weapon)
	{
/*		//Do we have one in the chamber?
        if( ent->client->weaponstate == WEAPON_END_MAG)
		{
			loaded = false;
		}*/
		// No ammo - forget it!
		if(!loaded && !clips)
		{
//			safe_bprintf(PRINT_HIGH,"Not got Dual Ammo\n");
			DropSpecialWeapon ( ent );
			return false;
		}
		// If it's not loaded - use a new clip
		else if( !loaded )
		{
//			safe_bprintf(PRINT_HIGH,"Need to reload Dual\n");
			Cmd_New_Reload_f ( ent );
		}
		return true;
	}

	// Not current weapon, check ammo
	if( !loaded && !clips )
	{
//		safe_bprintf(PRINT_HIGH,"No change - No Dual Ammo\n");
		return false;
	}

	// Change to this weapon
//	safe_bprintf(PRINT_HIGH,"Changing to Dual\n");
	ent->client->newweapon = item;
	ChangeWeapon ( ent );
	return true;
}
//AQ2 END

//===============================
// Handling for Grenades (debugging)
//===============================
qboolean ACEIT_ChangeGrenSpecialWeapon (edict_t *ent, gitem_t *item)
{
	int			ammo_index;
	gitem_t		*ammo_item;
	qboolean	loaded = true;
	qboolean	clips = true;

	// Has not picked up weapon yet
	if(!ent->client->pers.inventory[ITEM_INDEX(item)])
	{
//		safe_bprintf(PRINT_HIGH,"Not got Grenades\n");
		return false;
	}

	// Do we have ammo for it?
	if (item->ammo)
	{
		ammo_item = FindItem(item->ammo);
		ammo_index = ITEM_INDEX(ammo_item);
		if (ent->client->pers.inventory[ammo_index] < 1)
			clips = false;
		else
			clips = true;
	}
	// Check ammo
	if( (ent->client->shot_rds < 1) )
	{
//		safe_bprintf(PRINT_HIGH,"No change - No Grenades\n");
		loaded = false;
	}
	// see if we're already using it
	if (item == ent->client->pers.weapon)
	{
/*		//Do we have one in the chamber?
        if( ent->client->weaponstate == WEAPON_END_MAG)
		{
			loaded = false;
		}*/
		// No ammo - forget it!
		if(!loaded && !clips)
		{
//			safe_bprintf(PRINT_HIGH,"Not got Grenades\n");
			DropSpecialWeapon ( ent );
			return false;
		}
		// If it's not loaded - use a new clip
/*		else if( !loaded )
		{
//			safe_bprintf(PRINT_HIGH,"Need to reload \n");
			Cmd_New_Reload_f ( ent );
		}
*/		return true;
	}

	// No ammo
	if( !loaded && !clips )
	{
//		safe_bprintf(PRINT_HIGH,"No change - No Grenades\n");
		return false;
	}

	// Change to this weapon
//	safe_bprintf(PRINT_HIGH,"Changing to Grenades\n");
	ent->client->newweapon = item;
	ChangeWeapon ( ent );
	return true;
}





extern gitem_armor_t jacketarmor_info;
extern gitem_armor_t combatarmor_info;
extern gitem_armor_t bodyarmor_info;

///////////////////////////////////////////////////////////////////////
// Check if we can use the armor
///////////////////////////////////////////////////////////////////////
qboolean ACEIT_CanUseArmor (gitem_t *item, edict_t *other)
{
	int				old_armor_index;
	gitem_armor_t	*oldinfo;
	gitem_armor_t	*newinfo;
	int				newcount;
	float			salvage;
	int				salvagecount;

	// get info on new armor
	newinfo = (gitem_armor_t *)item->info;

	old_armor_index = ArmorIndex (other);

	// handle armor shards specially
	if (item->tag == ARMOR_SHARD)
		return true;
	
	// get info on old armor
	if (old_armor_index == ITEM_INDEX(FindItem("Jacket Armor")))
		oldinfo = &jacketarmor_info;
	else if (old_armor_index == ITEM_INDEX(FindItem("Combat Armor")))
		oldinfo = &combatarmor_info;
	else // (old_armor_index == body_armor_index)
		oldinfo = &bodyarmor_info;

	if (newinfo->normal_protection <= oldinfo->normal_protection)
	{
		// calc new armor values
		salvage = newinfo->normal_protection / oldinfo->normal_protection;
		salvagecount = salvage * newinfo->base_count;
		newcount = other->client->pers.inventory[old_armor_index] + salvagecount;

		if (newcount > oldinfo->max_count)
			newcount = oldinfo->max_count;

		// if we're already maxed out then we don't need the new armor
		if (other->client->pers.inventory[old_armor_index] >= newcount)
			return false;

	}

	return true;
}


///////////////////////////////////////////////////////////////////////
// Determines the NEED for an item
//
// This function can be modified to support new items to pick up
// Any other logic that needs to be added for custom decision making
// can be added here. For now it is very simple.
///////////////////////////////////////////////////////////////////////
float ACEIT_ItemNeed(edict_t *self, int item)
{
        gitem_t         *haveband;
		int	band;
	
	// Make sure item is at least close to being valid
	if(item < 0 || item > 100)
		return 0.0;
//AQ2 ADD
        // find out if they have a bandolier
        haveband = FindItem(BAND_NAME);
        if ( self->client->pers.inventory[ITEM_INDEX(haveband)] )
                band = 1;
        else
                band = 0;
//AQ2 END
        

	switch(item)
	{
		// Health
		case ITEMLIST_HEALTH_SMALL:	
		case ITEMLIST_HEALTH_MEDIUM:
		case ITEMLIST_HEALTH_LARGE:	
		case ITEMLIST_HEALTH_MEGA:	
			if(self->health < 100)
				return 1.0 - (float)self->health/100.0f; // worse off, higher priority
			else
				return 0.0;

		case ITEMLIST_AMMOPACK:
		case ITEMLIST_QUADDAMAGE:
		case ITEMLIST_INVULNERABILITY:
		case ITEMLIST_SILENCER:			
	//	case ITEMLIST_REBREATHER
	//	case ITEMLIST_ENVIRONMENTSUIT
		case ITEMLIST_ADRENALINE:		
		case ITEMLIST_BANDOLIER:			
			return 0.5;
/*		
		// Weapons
		case ITEMLIST_ROCKETLAUNCHER:
		case ITEMLIST_RAILGUN:
		case ITEMLIST_MACHINEGUN:
		case ITEMLIST_CHAINGUN:
		case ITEMLIST_SHOTGUN:
		case ITEMLIST_SUPERSHOTGUN:
		case ITEMLIST_BFG10K:
		case ITEMLIST_GRENADELAUNCHER:
		case ITEMLIST_HYPERBLASTER:
			if(!self->client->pers.inventory[item])
				return 0.7;
			else
				return 0.0;*/

		// Ammo
		case ITEMLIST_SLUGS:			
			if(self->client->pers.inventory[ITEMLIST_SLUGS] < self->client->pers.max_slugs)
				return 0.3;  
			else
				return 0.0;
	
		case ITEMLIST_BULLETS:
			if(self->client->pers.inventory[ITEMLIST_BULLETS] < self->client->pers.max_bullets)
				return 0.3;  
			else
				return 0.0;
	
		case ITEMLIST_SHELLS:
		   if(self->client->pers.inventory[ITEMLIST_SHELLS] < self->client->pers.max_shells)
				return 0.3;  
			else
				return 0.0;
	
		case ITEMLIST_CELLS:
			if(self->client->pers.inventory[ITEMLIST_CELLS] <	self->client->pers.max_cells)
				return 0.3;  
			else
				return 0.0;
	
		case ITEMLIST_ROCKETS:
			if(self->client->pers.inventory[ITEMLIST_ROCKETS] < self->client->pers.max_rockets)
				return 0.3;  
			else
				return 0.0;
	
		case ITEMLIST_GRENADES:
			if(self->client->pers.inventory[ITEMLIST_GRENADES] < self->client->pers.max_grenades)
				return 0.3;  
			else
				return 0.0;
	
		case ITEMLIST_BODYARMOR:
			if(ACEIT_CanUseArmor (FindItem("Body Armor"), self))
				return 0.6;  
			else
				return 0.0;
	
		case ITEMLIST_COMBATARMOR:
			if(ACEIT_CanUseArmor (FindItem("Combat Armor"), self))
				return 0.6;  
			else
				return 0.0;
	
		case ITEMLIST_JACKETARMOR:
			if(ACEIT_CanUseArmor (FindItem("Jacket Armor"), self))
				return 0.6;  
			else
				return 0.0;
	
		case ITEMLIST_POWERSCREEN:
		case ITEMLIST_POWERSHIELD:
			return 0.5;  

/*		case ITEMLIST_FLAG1:
			// If I am on team one, I want team two's flag
			if(!self->client->pers.inventory[item] && self->client->resp.ctf_team == CTF_TEAM2)
				return 10.0;  
			else 
				return 0.0;

		case ITEMLIST_FLAG2:
			if(!self->client->pers.inventory[item] && self->client->resp.ctf_team == CTF_TEAM1)
				return 10.0;  
			else
				return 0.0;
		
		case ITEMLIST_RESISTANCETECH:
		case ITEMLIST_STRENGTHTECH:
		case ITEMLIST_HASTETECH:			
		case ITEMLIST_REGENERATIONTECH:
			// Check for other tech
			if(!self->client->pers.inventory[ITEMLIST_RESISTANCETECH] &&
			   !self->client->pers.inventory[ITEMLIST_STRENGTHTECH] &&
			   !self->client->pers.inventory[ITEMLIST_HASTETECH] &&
			   !self->client->pers.inventory[ITEMLIST_REGENERATIONTECH])
			    return 0.4;  
			else
				return 0.0;*/
//AQ2 ADD
		case ITEMLIST_MP5:
		case ITEMLIST_M4:
		case ITEMLIST_M3:
		case ITEMLIST_HC:
		case ITEMLIST_SNIPER:
		case ITEMLIST_DUAL:
                if(
					( self->client->unique_weapon_total < unique_weapons->value + band )
					&& 	(!self->client->pers.inventory[item])
					)
					return 0.7;
				else
					return 0.0;

		case MK23_ANUM:
			if(self->client->pers.inventory[MK23_ANUM] 
				< self->client->pers.max_bullets)
				return 0.3;  
			else
				return 0.0;

		case M4_ANUM:
			if(self->client->pers.inventory[M4_ANUM] 
				< self->client->pers.max_cells)
				return 0.3;  
			else
				return 0.0;

		case MP5_ANUM:
			if(self->client->pers.inventory[MP5_ANUM] 
				< self->client->pers.max_rockets)
				return 0.3;  
			else
				return 0.0;

		case SNIPER_ANUM:
			if(self->client->pers.inventory[SNIPER_ANUM] 
				< self->client->pers.max_slugs)
				return 0.3;  
			else
				return 0.0;

		case SHELL_ANUM:
			if(self->client->pers.inventory[SHELL_ANUM] 
				< self->client->pers.max_shells)
				return 0.3;  
			else
				return 0.0;

		case ITEMLIST_KNIFE:
			if(self->client->pers.inventory[ITEM_INDEX(FindItem(KNIFE_NAME))] 
				< self->client->knife_max)
				return 0.3;  
			else
				return 0.0;


//AQ2 END
				
		default:
			return 0.0;
			
	}
		
}

///////////////////////////////////////////////////////////////////////
// Convert a classname to its index value
//
// I prefer to use integers/defines for simplicity sake. This routine
// can lead to some slowdowns I guess, but makes the rest of the code
// easier to deal with.
///////////////////////////////////////////////////////////////////////
int ACEIT_ClassnameToIndex(char *classname)
{

	if(strcmp(classname,"weapon_Mk23")==0) 
		return ITEMLIST_MK23;

	if(strcmp(classname,"weapon_MP5")==0) 
		return ITEMLIST_MP5;

	if(strcmp(classname,"weapon_M4")==0) 
		return ITEMLIST_M4;

	if(strcmp(classname,"weapon_M3")==0) 
		return ITEMLIST_M3;

	if(strcmp(classname,"weapon_HC")==0) 
		return ITEMLIST_HC;

	if(strcmp(classname,"weapon_Sniper")==0) 
		return ITEMLIST_SNIPER;

	if(strcmp(classname,"weapon_Dual")==0) 
		return ITEMLIST_DUAL;

	if(strcmp(classname,"weapon_Knife")==0) 
		return ITEMLIST_KNIFE;

	if(strcmp(classname,"weapon_Grenade")==0) 
		return ITEMLIST_GRENADE;

	if(strcmp(classname,"item_quiet")==0) 
		return ITEMLIST_SIL;

	if(strcmp(classname,"item_slippers")==0) 
		return ITEMLIST_SLIP;

	if(strcmp(classname,"item_band")==0) 
		return ITEMLIST_BAND;

	if(strcmp(classname,"item_vest")==0) 
		return ITEMLIST_KEV;

	if(strcmp(classname,"item_lasersight")==0) 
		return ITEMLIST_LASER;

	if(strcmp(classname,"ammo_clip")==0) 
		return ITEMLIST_AMMO_CLIP;

	if(strcmp(classname,"ammo_m4")==0) 
		return ITEMLIST_AMMO_M4;

	if(strcmp(classname,"ammo_mag")==0) 
		return ITEMLIST_AMMO_MAG;

	if(strcmp(classname,"ammo_sniper")==0) 
		return ITEMLIST_AMMO_SNIPER;

	if(strcmp(classname,"ammo_m3")==0) 
		return ITEMLIST_AMMO_M3;

	// Normal quake stuff:
	if(strcmp(classname,"item_armor_body")==0) 
		return ITEMLIST_BODYARMOR;
	
	if(strcmp(classname,"item_armor_combat")==0)
		return ITEMLIST_COMBATARMOR;

	if(strcmp(classname,"item_armor_jacket")==0)
		return ITEMLIST_JACKETARMOR;
	
	if(strcmp(classname,"item_armor_shard")==0)
		return ITEMLIST_ARMORSHARD;

	if(strcmp(classname,"item_power_screen")==0)
		return ITEMLIST_POWERSCREEN;

	if(strcmp(classname,"item_power_shield")==0)
		return ITEMLIST_POWERSHIELD;

	if(strcmp(classname,"weapon_blaster")==0)
		return ITEMLIST_BLASTER;

	if(strcmp(classname,"weapon_shotgun")==0)
		return ITEMLIST_SHOTGUN;
	
	if(strcmp(classname,"weapon_supershotgun")==0)
		return ITEMLIST_SUPERSHOTGUN;
	
	if(strcmp(classname,"weapon_machinegun")==0)
		return ITEMLIST_MACHINEGUN;
	
	if(strcmp(classname,"weapon_chaingun")==0)
		return ITEMLIST_CHAINGUN;

	if(strcmp(classname,"weapon_chaingun")==0)
		return ITEMLIST_CHAINGUN;
	
	if(strcmp(classname,"ammo_grenades")==0)
		return ITEMLIST_GRENADES;

	if(strcmp(classname,"weapon_grenadelauncher")==0)
		return ITEMLIST_GRENADELAUNCHER;

	if(strcmp(classname,"weapon_rocketlauncher")==0)
		return ITEMLIST_ROCKETLAUNCHER;

	if(strcmp(classname,"weapon_hyperblaster")==0)
		return ITEMLIST_HYPERBLASTER;

	if(strcmp(classname,"weapon_railgun")==0)
		return ITEMLIST_RAILGUN;

	if(strcmp(classname,"weapon_bfg10k")==0)
		return ITEMLIST_BFG10K;

	if(strcmp(classname,"ammo_shells")==0)
		return ITEMLIST_SHELLS;
	
	if(strcmp(classname,"ammo_bullets")==0)
		return ITEMLIST_BULLETS;

	if(strcmp(classname,"ammo_cells")==0)
		return ITEMLIST_CELLS;

	if(strcmp(classname,"ammo_rockets")==0)
		return ITEMLIST_ROCKETS;

	if(strcmp(classname,"ammo_slugs")==0)
		return ITEMLIST_SLUGS;
	
	if(strcmp(classname,"item_quad")==0)
		return ITEMLIST_QUADDAMAGE;

	if(strcmp(classname,"item_invunerability")==0)
		return ITEMLIST_INVULNERABILITY;

	if(strcmp(classname,"item_silencer")==0)
		return ITEMLIST_SILENCER;

	if(strcmp(classname,"item_rebreather")==0)
		return ITEMLIST_REBREATHER;

	if(strcmp(classname,"item_enviornmentsuit")==0)
		return ITEMLIST_ENVIRONMENTSUIT;

	if(strcmp(classname,"item_ancienthead")==0)
		return ITEMLIST_ANCIENTHEAD;

	if(strcmp(classname,"item_adrenaline")==0)
		return ITEMLIST_ADRENALINE;

	if(strcmp(classname,"item_bandolier")==0)
		return ITEMLIST_BANDOLIER;

	if(strcmp(classname,"item_pack")==0)
		return ITEMLIST_AMMOPACK;

	if(strcmp(classname,"item_datacd")==0)
		return ITEMLIST_DATACD;

	if(strcmp(classname,"item_powercube")==0)
		return ITEMLIST_POWERCUBE;

	if(strcmp(classname,"item_pyramidkey")==0)
		return ITEMLIST_PYRAMIDKEY;

	if(strcmp(classname,"item_dataspinner")==0)
		return ITEMLIST_DATASPINNER;

	if(strcmp(classname,"item_securitypass")==0)
		return ITEMLIST_SECURITYPASS;

	if(strcmp(classname,"item_bluekey")==0)
		return ITEMLIST_BLUEKEY;

	if(strcmp(classname,"item_redkey")==0)
		return ITEMLIST_REDKEY;

	if(strcmp(classname,"item_commandershead")==0)
		return ITEMLIST_COMMANDERSHEAD;

	if(strcmp(classname,"item_airstrikemarker")==0)
		return ITEMLIST_AIRSTRIKEMARKER;

	if(strcmp(classname,"item_health")==0) // ??
		return ITEMLIST_HEALTH;

	if(strcmp(classname,"item_health_small")==0)
		return ITEMLIST_HEALTH_SMALL;

	if(strcmp(classname,"item_health_medium")==0)
		return ITEMLIST_HEALTH_MEDIUM;

	if(strcmp(classname,"item_health_large")==0)
		return ITEMLIST_HEALTH_LARGE;
	
	if(strcmp(classname,"item_health_mega")==0)
		return ITEMLIST_HEALTH_MEGA;

	return INVALID;
}


///////////////////////////////////////////////////////////////////////
// Only called once per level, when saved will not be called again
//
// Downside of the routine is that items can not move about. If the level
// has been saved before and reloaded, it could cause a problem if there
// are items that spawn at random locations.
//
#define DEBUG // uncomment to write out items to a file.
///////////////////////////////////////////////////////////////////////
void ACEIT_BuildItemNodeTable (qboolean rebuild)
{
	edict_t *items;
	int i,item_index;
	vec3_t v,v1,v2;

#ifdef DEBUG
	FILE *pOut; // for testing
	if((pOut = fopen("items.txt","wt"))==NULL)
		return;
#endif
	
	num_items = 0;

	// Add game items
	for(items = g_edicts; items < &g_edicts[globals.num_edicts]; items++)
	{
		// filter out crap
		if(items->solid == SOLID_NOT)
			continue;
		
		if(!items->classname)
			continue;
		
		/////////////////////////////////////////////////////////////////
		// Items
		/////////////////////////////////////////////////////////////////
		item_index = ACEIT_ClassnameToIndex(items->classname);
		
		////////////////////////////////////////////////////////////////
		// SPECIAL NAV NODE DROPPING CODE
		////////////////////////////////////////////////////////////////
		// Special node dropping for platforms
		if(strcmp(items->classname,"func_plat")==0)
		{
			if(!rebuild)
				ACEND_AddNode(items,NODE_PLATFORM);
			item_index = 99; // to allow to pass the item index test
		}
		
		// Special node dropping for teleporters
		if(strcmp(items->classname,"misc_teleporter_dest")==0 || strcmp(items->classname,"misc_teleporter")==0)
		{
			if(!rebuild)
				ACEND_AddNode(items,NODE_TELEPORTER);
			item_index = 99;
		}

		// Special node dropping for doors - RiEvEr
		if(		
			(strcmp(items->classname,"func_door_rotating")==0)
			||	(strcmp(items->classname,"func_door")==0 )
			)
		{
//			item_index = 99;
			if(!rebuild)
			{
				// add a pointer to the item entity
//				item_table[num_items].ent = items;
//				item_table[num_items].item = item_index;
				// create the node
//				item_table[num_items].node = ACEND_AddNode(items,NODE_DOOR);
				ACEND_AddNode(items,NODE_DOOR);
//#ifdef DEBUG
//				fprintf(pOut,"item: %s node: %d pos: %f %f %f\n",items->classname,
//					item_table[num_items].node, 
//					items->s.origin[0],items->s.origin[1],items->s.origin[2]);
//#endif	
//				num_items++;
//				continue;
			}
			continue;
		}

		#ifdef DEBUG
		if(item_index == INVALID)
			fprintf(pOut,"Rejected item: %s node: %d pos: %f %f %f\n",items->classname,item_table[num_items].node,items->s.origin[0],items->s.origin[1],items->s.origin[2]);
//		else
//			fprintf(pOut,"item: %s node: %d pos: %f %f %f\n",items->classname,item_table[num_items].node,items->s.origin[0],items->s.origin[1],items->s.origin[2]);
		#endif		
	
		if(item_index == INVALID)
			continue;

		// add a pointer to the item entity
		item_table[num_items].ent = items;
		item_table[num_items].item = item_index;
	
		// If new, add nodes for items
		if(!rebuild)
		{
				item_table[num_items].node = ACEND_AddNode(items,NODE_ITEM);
#ifdef DEBUG
			fprintf(pOut,"item: %s node: %d pos: %f %f %f\n",items->classname,item_table[num_items].node,items->s.origin[0],items->s.origin[1],items->s.origin[2]);
#endif		
			num_items++;
		}
		else // Now if rebuilding, just relink ent structures 
		{
			// Find stored location
			for(i=0;i<numnodes;i++)
			{
				if(nodes[i].type == NODE_ITEM ||
				   nodes[i].type == NODE_PLATFORM ||
//@@				   nodes[i].type == NODE_DOOR||		// RiEvEr
				   nodes[i].type == NODE_TELEPORTER) // valid types
				{
					VectorCopy(items->s.origin,v);
					v[2] +=8; // Raised all nodes to get better links.
					
					// Add 16 to item type nodes
					if(nodes[i].type == NODE_ITEM)
						v[2] += 16;
					
					// Add 32 to teleporter
					if(nodes[i].type == NODE_TELEPORTER)
						v[2] += 32;

/*					// Door handling
					if( nodes[i].type == NODE_DOOR)
					{
						vec3_t	position;
						// Find mid point of door max and min and put the node there
						VectorClear(position);
//						VectorCopy(items->s.origin, position);
						// find center of door
						position[0] = items->absmin[0] + ((items->maxs[0] - items->mins[0]) /2);
						position[1] = items->absmin[1] + ((items->maxs[1] - items->mins[1]) /2);
//						position[2] -= 16; // lower it a little
						position[2] = items->absmin[2] + 32;
						// Set location
						VectorCopy(position, v);
						safe_bprintf(PRINT_HIGH, "%d ",items->s.angles[0]);
					}*/

					if(nodes[i].type == NODE_PLATFORM)
					{
						VectorCopy(items->maxs,v1);
						VectorCopy(items->mins,v2);
		
						// To get the center
						v[0] = (v1[0] - v2[0]) / 2 + v2[0];
						v[1] = (v1[1] - v2[1]) / 2 + v2[1];
						v[2] = items->mins[2]+64;
					}

					if(v[0] == nodes[i].origin[0] &&
 					   v[1] == nodes[i].origin[1] &&
					   v[2] == nodes[i].origin[2])
					{
						// found a match now link to facts
						item_table[num_items].node = i;			
#ifdef DEBUG
						fprintf(pOut,"Relink item: %s node: %d pos: %f %f %f\n",items->classname,item_table[num_items].node,items->s.origin[0],items->s.origin[1],items->s.origin[2]);
#endif							
						num_items++;
					
					}
/*					else
					{
#ifdef DEBUG
						fprintf(pOut,"Failed item: %s node: %d pos: %f %f %f\n",items->classname,item_table[num_items].node,items->s.origin[0],items->s.origin[1],items->s.origin[2]);
#endif				
					}*/
				}
			}
		}
		

	}

#ifdef DEBUG
	fclose(pOut);
#endif

}
