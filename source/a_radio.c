//-----------------------------------------------------------------------------
// Radio-related code for Action (formerly Axshun)
// 
// -Fireblade
//
// $Id: a_radio.c,v 1.6 2004/04/08 23:19:51 slicerdw Exp $
//
//-----------------------------------------------------------------------------
// $Log: a_radio.c,v $
// Revision 1.6  2004/04/08 23:19:51  slicerdw
// Optimized some code, added a couple of features and fixed minor bugs
//
// Revision 1.5  2002/03/26 21:49:01  ra
// Bufferoverflow fixes
//
// Revision 1.4  2001/09/28 13:48:34  ra
// I ran indent over the sources. All .c and .h files reindented.
//
// Revision 1.3  2001/09/05 14:33:57  slicerdw
// Added Fix's from the 2.1 release
//
// Revision 1.2  2001/08/15 14:50:48  slicerdw
// Added Flood protections to Radio & Voice, Fixed the sniper bug AGAIN
//
// Revision 1.1.1.1  2001/05/06 17:24:29  igor_rock
// This is the PG Bund Edition V1.25 with all stuff laying around here...
//
//-----------------------------------------------------------------------------

#include "g_local.h"

void Cmd_Say_f (edict_t * ent, qboolean team, qboolean arg0,
		qboolean partner_msg);

// Each of the possible radio messages and their length

radio_msg_t male_radio_msgs[] = {
  {"1", 6},
  {"2", 6},
  {"3", 8},
  {"4", 7},
  {"5", 8},
  {"6", 9},
  {"7", 8},
  {"8", 7},
  {"9", 7},
  {"10", 6},
  {"back", 6},
  {"cover", 7},
  {"down", 13},
  {"enemyd", 10},
  {"enemys", 9},
  {"forward", 6},
  {"go", 6},
  {"im_hit", 7},
  {"left", 7},
  {"reportin", 9},
  {"right", 6},
  {"taking_f", 22},
  {"teamdown", 13},
  {"treport", 12},
  {"up", 4},
  {"END", 0},			// end of list delimiter
};

radio_msg_t female_radio_msgs[] = {
  {"1", 5},
  {"2", 5},
  {"3", 5},
  {"4", 5},
  {"5", 5},
  {"6", 8},
  {"7", 7},
  {"8", 5},
  {"9", 5},
  {"10", 5},
  {"back", 6},
  {"cover", 5},
  {"down", 6},
  {"enemyd", 9},
  {"enemys", 9},
  {"forward", 8},
  {"go", 6},
  {"im_hit", 7},
  {"left", 8},
  {"reportin", 9},
  {"right", 5},
  {"taking_f", 22},
  {"teamdown", 10},
  {"treport", 12},
  {"up", 6},
  {"END", 0},			// end of list delimiter
};

void
PrecacheRadioMsgSet (radio_msg_t * msgs, char *base_path)
{
  char msg_fullpath[2048];

  while (strcmp (msgs->msg, "END"))
    {
      sprintf (msg_fullpath, "%s%s.wav", base_path, msgs->msg);
      gi.soundindex (msg_fullpath);
      msgs++;
    }
}

void
PrecacheRadioSounds ()
{
  gi.soundindex (RADIO_CLICK);
  gi.soundindex (RADIO_DEATH_MALE);
  gi.soundindex (RADIO_DEATH_FEMALE);

  PrecacheRadioMsgSet (male_radio_msgs, "radio/male/");
  PrecacheRadioMsgSet (female_radio_msgs, "radio/female/");
}

void
DeleteFirstRadioQueueEntry (edict_t * ent)
{
  int i;

  if (ent->client->resp.radio_queue_size <= 0)
    {
      gi.
	dprintf
	("DeleteFirstRadioQueueEntry: attempt to delete without any entries\n");
      return;
    }

  for (i = 1; i < ent->client->resp.radio_queue_size; i++)
    {
      memcpy (&(ent->client->resp.radio_queue[i - 1]),
	      &(ent->client->resp.radio_queue[i]),
	      sizeof (radio_queue_entry_t));
    }

  ent->client->resp.radio_queue_size--;
}

void
DeleteRadioQueueEntry (edict_t * ent, int entry_num)
{
  int i;

  if (ent->client->resp.radio_queue_size <= entry_num)
    {
      gi.
	dprintf
	("DeleteRadioQueueEntry: attempt to delete out of range queue entry\n");
      return;
    }

  for (i = entry_num + 1; i < ent->client->resp.radio_queue_size; i++)
    {
      memcpy (&(ent->client->resp.radio_queue[i - 1]),
	      &(ent->client->resp.radio_queue[i]),
	      sizeof (radio_queue_entry_t));
    }

  ent->client->resp.radio_queue_size--;
}

// RadioThink should be called once on each player per server frame.
void
RadioThink (edict_t * ent)
{
  // Try to clean things up, a bit....
  if (ent->client->resp.radio_partner)
    {
      if (!ent->client->resp.radio_partner->inuse ||
	  ent->client->resp.radio_partner->client->resp.radio_partner != ent)
	{
	  ent->client->resp.radio_partner = NULL;
	}
    }
  if (ent->client->resp.partner_last_offered_to)
    {
      if (!ent->client->resp.partner_last_offered_to->inuse ||
	  ent->client->resp.partner_last_offered_to->solid == SOLID_NOT)
	{
	  ent->client->resp.partner_last_offered_to = NULL;
	}
    }
  if (ent->client->resp.partner_last_denied_from)
    {
      if (!ent->client->resp.partner_last_denied_from->inuse ||
	  ent->client->resp.partner_last_denied_from->solid == SOLID_NOT)
	{
	  ent->client->resp.partner_last_denied_from = NULL;
	}
    }
  // ................................

  if (ent->client->resp.radio_power_off)
    {
      ent->client->resp.radio_queue_size = 0;
      return;
    }

  if (ent->client->resp.radio_delay > 1)
    {
      ent->client->resp.radio_delay--;
      return;
    }
  else if (ent->client->resp.radio_delay == 1)
    {
      DeleteFirstRadioQueueEntry (ent);
      ent->client->resp.radio_delay = 0;
    }

  if (ent->client->resp.radio_queue_size)
    {
      char snd_play_cmd[512];
      edict_t *from;
      int check;

      from = ent->client->resp.radio_queue[0].from_player;

      if (!ent->client->resp.radio_queue[0].click &&
	  (!from->inuse || from->solid == SOLID_NOT
	   || from->deadflag == DEAD_DEAD))
	{
	  if (ent->client->resp.radio_queue[0].from_gender)
	    {
	      strcpy (ent->client->resp.radio_queue[0].soundfile,
		      RADIO_DEATH_FEMALE);
	      ent->client->resp.radio_queue[0].length =
		RADIO_DEATH_FEMALE_LEN;
	    }
	  else
	    {
	      strcpy (ent->client->resp.radio_queue[0].soundfile,
		      RADIO_DEATH_MALE);
	      ent->client->resp.radio_queue[0].length = RADIO_DEATH_MALE_LEN;
	    }

	  for (check = 1; check < ent->client->resp.radio_queue_size; check++)
	    {
	      if (ent->client->resp.radio_queue[check].from_player == from)
		{
		  DeleteRadioQueueEntry (ent, check);
		  check--;
		}
	    }
	}

      sprintf (snd_play_cmd, "play %s",
	       ent->client->resp.radio_queue[0].soundfile);
      stuffcmd (ent, snd_play_cmd);
      ent->client->resp.radio_queue[0].now_playing = 1;
      ent->client->resp.radio_delay = ent->client->resp.radio_queue[0].length;
    }
}

int
TotalNonClickMessagesInQueue (edict_t * ent)
{
  int i, count = 0;

  for (i = 0; i < ent->client->resp.radio_queue_size; i++)
    {
      if (!ent->client->resp.radio_queue[i].click)
	count++;
    }

  return count;
}

void
AppendRadioMsgToQueue (edict_t * ent, char *msg, int len, int click,
		       edict_t * from_player)
{
  radio_queue_entry_t *newentry;

  if (ent->client->resp.radio_queue_size >= MAX_RADIO_QUEUE_SIZE)
    {
      gi.
	dprintf
	("AppendRadioMsgToQueue: Maximum radio queue size exceeded\n");
      return;
    }

  newentry =
    &(ent->client->resp.radio_queue[ent->client->resp.radio_queue_size]);

  if (strlen (msg) + 1 > MAX_SOUNDFILE_PATH_LEN)
    {
      gi.dprintf ("Radio sound file path (%s) exceeded maximum length\n",
		  msg);
      *(msg + MAX_SOUNDFILE_PATH_LEN - 1) = 0;
    }
  strcpy (newentry->soundfile, msg);
  newentry->from_player = from_player;
  newentry->from_gender = from_player->client->resp.radio_gender;
  newentry->now_playing = 0;
  newentry->length = len;
  newentry->click = click;

  ent->client->resp.radio_queue_size++;
}

void
InsertRadioMsgInQueueBeforeClick (edict_t * ent, char *msg, int len,
				  edict_t * from_player)
{
  radio_queue_entry_t *newentry;

  if (ent->client->resp.radio_queue_size >= MAX_RADIO_QUEUE_SIZE)
    {
      gi.
	dprintf
	("InsertRadioMsgInQueueBeforeClick: Maximum radio queue size exceeded\n");
      return;
    }

  memcpy (&
	  (ent->client->resp.radio_queue[ent->client->resp.radio_queue_size]),
	  &(ent->client->resp.
	    radio_queue[ent->client->resp.radio_queue_size - 1]),
sizeof (radio_queue_entry_t));

  newentry =
    &(ent->client->resp.radio_queue[ent->client->resp.radio_queue_size - 1]);

  if (strlen (msg) + 1 > MAX_SOUNDFILE_PATH_LEN)
    {
      gi.dprintf ("Radio sound file path (%s) exceeded maximum length\n",
		  msg);
      *(msg + MAX_SOUNDFILE_PATH_LEN - 1) = 0;
    }
  strcpy (newentry->soundfile, msg);
  newentry->from_player = from_player;
  newentry->from_gender = from_player->client->resp.radio_gender;
  newentry->now_playing = 0;
  newentry->length = len;
  newentry->click = 0;

  ent->client->resp.radio_queue_size++;
}

void
AddRadioMsg (edict_t * ent, char *msg, int len, edict_t * from_player)
{
  if (ent->client->resp.radio_queue_size == 0 ||
      (ent->client->resp.radio_queue[0].click &&
       ent->client->resp.radio_queue_size == 1))
    {
      AppendRadioMsgToQueue (ent, RADIO_CLICK, RADIO_CLICK_LEN, 1,
			     from_player);
      AppendRadioMsgToQueue (ent, msg, len, 0, from_player);
      AppendRadioMsgToQueue (ent, RADIO_CLICK, RADIO_CLICK_LEN, 1,
			     from_player);
    }
  else				// we have some msgs in it already...

    {
      if (TotalNonClickMessagesInQueue (ent) < MAX_RADIO_MSG_QUEUE_SIZE)
	InsertRadioMsgInQueueBeforeClick (ent, msg, len, from_player);
      // else ignore the message...
    }
}

void
RadioBroadcast (edict_t * ent, int partner, char *msg)
{
  int j, i, msg_len, found;
  edict_t *other;
  radio_msg_t *radio_msgs;
  char msg_fullpath[2048], *base_path;
  char msgname_num[8], filteredmsg[48];

  if (ent->deadflag == DEAD_DEAD || ent->solid == SOLID_NOT)
    return;

  if (!teamplay->value)
    {
      if (!((int) (dmflags->value) & (DF_MODELTEAMS | DF_SKINTEAMS)))
	return;			// don't allow in a non-team setup...

    }

  if (ent->client->resp.radio_power_off)
    {
      gi.centerprintf (ent, "Your radio is off!\n");
      return;
    }

  if (partner && ent->client->resp.radio_partner == NULL)
    {
      gi.cprintf (ent, PRINT_HIGH, "You don't have a partner.\n");
      return;
    }

  if (ent->client->resp.radio_gender)
    {
      radio_msgs = female_radio_msgs;
      base_path = "radio/female/";
    }
  else
    {
      radio_msgs = male_radio_msgs;
      base_path = "radio/male/";
    }

  i = found = 0;
  msg_len = 0;

  strncpy(filteredmsg, msg, sizeof(filteredmsg)-1);

  while (strcmp (radio_msgs[i].msg, "END"))
    {
      if (!Q_stricmp (radio_msgs[i].msg, filteredmsg))
	{
	  found = 1;
	  sprintf (msg_fullpath, "%s%s.wav", base_path, radio_msgs[i].msg);
	  msg_len = radio_msgs[i].length;
	  break;
	}
      i++;
    }

  if (!found)
    {
      gi.centerprintf (ent, "'%s' is not a valid radio message\n", filteredmsg);
      return;
    }

  if (radiolog->value)
    {
      gi.cprintf (NULL, PRINT_CHAT, "[%s RADIO] %s: %s\n",
		  partner ? "PARTNER" : "TEAM",
		  ent->client->pers.netname, filteredmsg);
    }

//TempFile BEGIN
  if (Q_stricmp (filteredmsg, "enemyd") == 0)
    {
      if ((ent->client->pers.num_kills > 1)
	  && (ent->client->pers.num_kills <= 10))
	{
	  // If we are reporting enemy down, add the number of kills.
	  sprintf (msgname_num, "%i", ent->client->pers.num_kills);
	  ent->client->pers.num_kills = 0;	// prevent from getting into an endless loop

	  RadioBroadcast (ent, partner, msgname_num);	// Now THAT'S recursion! =)

	}
      ent->client->pers.num_kills = 0;
    }
//TempFile END
  //AQ2:TNG Slicer
  if (radio_repeat->value)
    {  //SLIC2 Optimization
      if (CheckForRepeat (ent, i) == false)
		return;
    }

  if (radio_max->value)
    {
      if (CheckForFlood (ent) == false)
		return;
    }


  //AQ2:TNG END
  for (j = 1; j <= game.maxclients; j++)
    {
      other = &g_edicts[j];
      if (!other->inuse)
	continue;
      if (!other->client)
	continue;
      if (!OnSameTeam (ent, other))
	continue;
      if (partner && other != ent->client->resp.radio_partner)
	continue;
      AddRadioMsg (other, msg_fullpath, msg_len, ent);
    }
}

void
Cmd_Radio_f (edict_t * ent)
{
  RadioBroadcast (ent, ent->client->resp.radio_partner_mode, gi.args ());
}

void
Cmd_Radiopartner_f (edict_t * ent)
{
  RadioBroadcast (ent, 1, gi.args ());
}

void
Cmd_Radioteam_f (edict_t * ent)
{
  RadioBroadcast (ent, 0, gi.args ());
}

void
Cmd_Radiogender_f (edict_t * ent)
{
  char *arg;

  if (!teamplay->value)
    {
      if (!((int) (dmflags->value) & (DF_MODELTEAMS | DF_SKINTEAMS)))
	return;			// don't allow in a non-team setup...

    }

  arg = gi.args ();
  if (arg == NULL || !strlen (arg))
    {
      if (ent->client->resp.radio_gender)
	gi.cprintf (ent, PRINT_HIGH,
		    "Radio gender currently set to female\n");
      else
	gi.cprintf (ent, PRINT_HIGH, "Radio gender currently set to male\n");
      return;
    }

  if (!Q_stricmp (arg, "male"))
    {
      gi.cprintf (ent, PRINT_HIGH, "Radio gender set to male\n");
      ent->client->resp.radio_gender = 0;
    }
  else if (!Q_stricmp (arg, "female"))
    {
      gi.cprintf (ent, PRINT_HIGH, "Radio gender set to female\n");
      ent->client->resp.radio_gender = 1;
    }
  else
    {
      gi.cprintf (ent, PRINT_HIGH,
		  "Invalid gender selection, try 'male' or 'female'\n");
    }
}

void
Cmd_Radio_power_f (edict_t * ent)
{
  if (!teamplay->value)
    {
      if (!((int) (dmflags->value) & (DF_MODELTEAMS | DF_SKINTEAMS)))
	return;			// don't allow in a non-team setup...

    }

  ent->client->resp.radio_power_off = !ent->client->resp.radio_power_off;

  if (ent->client->resp.radio_power_off)
    {
      gi.centerprintf (ent, "Radio switched off\n");
      stuffcmd (ent, "play " RADIO_CLICK);
    }
  else
    {
      gi.centerprintf (ent, "Radio switched on\n");
      stuffcmd (ent, "play " RADIO_CLICK);
    }
}

void
Cmd_Channel_f (edict_t * ent)
{
  if (!teamplay->value)
    {
      if (!((int) (dmflags->value) & (DF_MODELTEAMS | DF_SKINTEAMS)))
	return;			// don't allow in a non-team setup...

    }

  ent->client->resp.radio_partner_mode =
    !ent->client->resp.radio_partner_mode;
  if (ent->client->resp.radio_partner_mode)
    {
      gi.centerprintf (ent, "Channel set to 1, partner channel\n");
    }
  else
    {
      gi.centerprintf (ent, "Channel set to 0, team channel\n");
    }
}

// DetermineViewedTeammate: determine the current player you're viewing (only looks for live teammates)
// Modified from SetIDView (which was used from Zoid's CTF)
edict_t *
DetermineViewedTeammate (edict_t * ent)
{
  vec3_t forward, dir;
  trace_t tr;
  edict_t *who, *best;
//FIREBLADE, suggested by hal[9k]  3/11/1999
  float bd = 0.9;
//FIREBLADE
  float d;
  int i;

  AngleVectors (ent->client->v_angle, forward, NULL, NULL);
  VectorScale (forward, 8192, forward);
  VectorAdd (ent->s.origin, forward, forward);
  PRETRACE ();
  tr = gi.trace (ent->s.origin, NULL, NULL, forward, ent, MASK_SOLID);
  POSTTRACE ();
  if (tr.fraction < 1 && tr.ent && tr.ent->client)
    {
      return NULL;
    }

  AngleVectors (ent->client->v_angle, forward, NULL, NULL);
  best = NULL;
  for (i = 1; i <= maxclients->value; i++)
    {
      who = g_edicts + i;
      if (!who->inuse)
	continue;
      VectorSubtract (who->s.origin, ent->s.origin, dir);
      VectorNormalize (dir);
      d = DotProduct (forward, dir);
      if (d > bd && loc_CanSee (ent, who) &&
	  who->solid != SOLID_NOT &&
	  who->deadflag != DEAD_DEAD && OnSameTeam (who, ent))
	{
	  bd = d;
	  best = who;
	}
    }

  if (bd > 0.90)
    {
      return best;
    }

  return NULL;
}

void
Cmd_Partner_f (edict_t * ent)
{
  edict_t *target;
  char *genderstr;

  if (!teamplay->value)
    {
      if (!((int) (dmflags->value) & (DF_MODELTEAMS | DF_SKINTEAMS)))
	return;			// don't allow in a non-team setup...

    }

  if (ent->deadflag == DEAD_DEAD || ent->solid == SOLID_NOT)
    return;

  if (ent->client->resp.radio_partner &&
      !ent->client->resp.radio_partner->inuse)
    {				// just in case RadioThink hasn't caught it yet... avoid any problems

      ent->client->resp.radio_partner = NULL;
    }

  if (ent->client->resp.radio_partner)
    {
      gi.centerprintf (ent, "You already have a partner, %s\n",
		       ent->client->resp.radio_partner->client->pers.netname);
      return;
    }

  target = DetermineViewedTeammate (ent);

  if (target == NULL)
    {
      gi.centerprintf (ent, "No potential partner selected\n");
      return;
    }

  if (target->client->resp.radio_partner)
    {
      gi.centerprintf (ent, "%s already has a partner\n",
		       target->client->pers.netname);
      return;
    }

  if (target->client->resp.partner_last_offered_to == ent &&
      ent->client->resp.partner_last_offered_from == target)
    {
      gi.centerprintf (ent, "%s is now your partner\n",
		       target->client->pers.netname);
      gi.centerprintf (target, "%s is now your partner\n",
		       ent->client->pers.netname);
      ent->client->resp.radio_partner = target;
      target->client->resp.radio_partner = ent;
      ent->client->resp.partner_last_offered_from = NULL;
      target->client->resp.partner_last_offered_to = NULL;
      return;
    }

  if (target->client->resp.partner_last_denied_from == ent)
    {
      gi.centerprintf (ent, "%s has already denied you\n",
		       target->client->pers.netname);
      return;
    }

  if (target == ent->client->resp.partner_last_offered_to)
    {
      if (IsFemale (target))
	genderstr = "her";
      else if (IsNeutral (target))
	genderstr = "it";
      else
	genderstr = "him";

      gi.centerprintf (ent, "Already awaiting confirmation from %s\n",
		       genderstr);
      return;
    }

  if (IsFemale (ent))
    genderstr = "her";
  else if (IsNeutral (ent))
    genderstr = "it";
  else
    genderstr = "him";

  gi.centerprintf (ent, "Awaiting confirmation from %s\n",
		   target->client->pers.netname);
  gi.centerprintf (target,
		   "%s offers to be your partner\n"
		   "To accept:\nView %s and use the 'partner' command\n"
		   "To deny:\nUse the 'deny' command\n",
		   ent->client->pers.netname, genderstr);

  ent->client->resp.partner_last_offered_to = target;
  target->client->resp.partner_last_offered_from = ent;
}

void
Cmd_Unpartner_f (edict_t * ent)
{
  edict_t *target;

  if (!teamplay->value)
    {
      if (!((int) (dmflags->value) & (DF_MODELTEAMS | DF_SKINTEAMS)))
	return;			// don't allow in a non-team setup...

    }

  if (ent->client->resp.radio_partner &&
      !ent->client->resp.radio_partner->inuse)
    {				// just in case RadioThink hasn't caught it yet... avoid any problems

      ent->client->resp.radio_partner = NULL;
    }

  target = ent->client->resp.radio_partner;

  if (target == NULL)
    {
      gi.centerprintf (ent, "You don't have a partner\n");
      return;
    }

  if (target->client->resp.radio_partner == ent)
    {
      gi.centerprintf (target, "%s broke your partnership\n",
		       ent->client->pers.netname);
      target->client->resp.radio_partner = NULL;
    }

  gi.centerprintf (ent, "You broke your partnership with %s\n",
		   target->client->pers.netname);
  ent->client->resp.radio_partner = NULL;
}

void
Cmd_Deny_f (edict_t * ent)
{
  edict_t *target;

  if (!teamplay->value)
    {
      if (!((int) (dmflags->value) & (DF_MODELTEAMS | DF_SKINTEAMS)))
	return;			// don't allow in a non-team setup...

    }

  if (ent->deadflag == DEAD_DEAD || ent->solid == SOLID_NOT)
    return;

  target = ent->client->resp.partner_last_offered_from;
  if (target && target->inuse)
    {
      gi.centerprintf (ent, "You denied %s\n", target->client->pers.netname);
      gi.centerprintf (target, "%s has denied you\n",
		       ent->client->pers.netname);
      ent->client->resp.partner_last_denied_from = target;
      ent->client->resp.partner_last_offered_from = NULL;
      if (target->client->resp.partner_last_offered_to == ent)
	target->client->resp.partner_last_offered_to = NULL;
    }
  else
    {
      gi.centerprintf (ent, "No one has offered to be your partner\n");
      return;
    }
}

void
Cmd_Say_partner_f (edict_t * ent)
{
  if (!teamplay->value)
    {
      if (!((int) (dmflags->value) & (DF_MODELTEAMS | DF_SKINTEAMS)))
	return;			// don't allow in a non-team setup...

    }

  if (ent->client->resp.radio_partner == NULL)
    {
      gi.cprintf (ent, PRINT_HIGH, "You don't have a partner.\n");
      return;
    }

  Cmd_Say_f (ent, false, false, true);
}
//SLIC2 Redesigned and optimized these two functions
qboolean CheckForFlood(edict_t * ent)
{
	//If he's muted..
	if (ent->client->resp.rd_mute) {
		if (ent->client->resp.rd_mute > level.time)	// Still muted..
			return false;
		else
			ent->client->resp.rd_mute = 0;	// No longer muted..
	}
	if (!ent->client->resp.rd_Count) {
		ent->client->resp.rd_time = level.time;
		++ent->client->resp.rd_Count;
	} else {
		++ent->client->resp.rd_Count;
		if (level.time - ent->client->resp.rd_time < radio_time->value) {
			if (ent->client->resp.rd_Count >= radio_max->value) {
				gi.cprintf (ent, PRINT_HIGH,
					 "[RADIO FLOOD PROTECTION]: Flood Detected, you are silenced for %d secs\n",(int) radio_ban->value);
				ent->client->resp.rd_mute = level.time + radio_ban->value;
				return false;
			}
		} else {
			ent->client->resp.rd_Count = 0;
		}
	}

	return true;
}
qboolean CheckForRepeat(edict_t * ent, int radioCode)
{
	//If he's muted..
	if (ent->client->resp.rd_mute) {
		if (ent->client->resp.rd_mute > level.time)	// Still muted..
			return false;
		else
			ent->client->resp.rd_mute = 0;	// No longer muted..
	}

	if (ent->client->resp.rd_lastRadio == radioCode) {	//He's trying to repeat it..
		if ((level.time - ent->client->resp.rd_repTime) < radio_repeat_time->value) {
			if (++ent->client->resp.rd_repCount == radio_repeat->value) {	//Busted
				gi.cprintf (ent, PRINT_HIGH,"[RADIO FLOOD PROTECTION]: Repeat Flood Detected, you are silenced for %d secs\n",(int) radio_ban->value);
				ent->client->resp.rd_mute = level.time + radio_ban->value;
				return false;
			}
		}
		else
			ent->client->resp.rd_repCount = 0;

		ent->client->resp.rd_repTime = level.time;
	}
	ent->client->resp.rd_lastRadio = radioCode;
	ent->client->resp.rd_repTime = level.time;
	return true;
}

