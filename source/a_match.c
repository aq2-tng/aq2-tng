#include "g_local.h"
#include "a_match.h"

float matchtime = 0;

int team1ready = 0;
int team2ready = 0;

void SendWorldMsg(char *s,int sound,int center)
{
	int i;
    edict_t *e;

        for (i = 1; i <= maxclients->value; i++)
        {
                e = g_edicts + i;
                if (e->inuse)
                {
					if(center)
					{
						gi.centerprintf(e, "%s", s);
					}
					else
					{
                        gi.cprintf(e, PRINT_HIGH, "%s",s);
						if(sound)
                        stuffcmd(e, "play misc/comp_up.wav");
					}
                }
				
        }
}

void SendScores()
{
	int mins,secs;
	mins = matchtime/60;
	secs = matchtime-(mins*60);
	gi.bprintf (PRINT_HIGH, "žžžžžžžžžžžžžžžžžžžžžžžžžžžžžžžžŸ\n");
	gi.bprintf (PRINT_HIGH, "Team 1 Score - Team 2 Score\n");
	gi.bprintf (PRINT_HIGH, "    [%d]           [%d]\n",team1_score,team2_score);
	gi.bprintf (PRINT_HIGH, "Total Played Time: %d:%02d\n",mins,secs);
	gi.bprintf (PRINT_HIGH, "žžžžžžžžžžžžžžžžžžžžžžžžžžžžžžžžŸ\n");
	gi.bprintf (PRINT_HIGH, "Match is over, waiting for next map, please vote a new one..\n");
}
void Cmd_Kill_f (edict_t *ent); // Used for killing people when they sub off

void Cmd_Sub_f(edict_t *ent)
{
	char temp[200];
	    if (ent->client->resp.team == NOTEAM || !teamplay->value)
		{
			gi.cprintf(ent, PRINT_HIGH, "You need to be on a team for that...\n");
            return;
		}
        if(ent->client->resp.subteam == 0)
		{
		    ent->client->resp.subteam = ent->client->resp.team;

			// lets kill em.
			Cmd_Kill_f(ent);

			if(ent->client->resp.subteam == 1)
			{
				sprintf(temp, "%s is now a substitute for %s\n", ent->client->pers.netname,team1_name);
				temp[200] = 0;
				SendWorldMsg(temp,0,0);
			}

			else
			{
				sprintf(temp, "%s is now a substitute for %s\n", ent->client->pers.netname,team2_name);
				temp[200] = 0;
				SendWorldMsg(temp,0,0);
			}
			return;

        }
		if(ent->client->resp.subteam == TEAM1)
		{
	    sprintf(temp, "%s is no longer a substitute for %s\n", ent->client->pers.netname,team1_name);
		temp[200] = 0;
		SendWorldMsg(temp,0,0);
        ent->client->resp.subteam = 0;
		return;
        }
		if(ent->client->resp.subteam == TEAM2)
		{
        sprintf(temp, "%s is no longer a substitute for %s\n", ent->client->pers.netname,team2_name);
		temp[200] = 0;
		SendWorldMsg(temp,0,0);
		ent->client->resp.subteam = 0;
		return;
        }
}

int CheckForCaptains(int cteam)
{
	edict_t *ent;
	int i;

	for(i=1;i<=(int)(maxclients->value);i++)
	{	
		ent = getEnt(i);
		if(ent->inuse)
		{
			if(ent->client->resp.captain == cteam)
				return 1;
		}
	}
	return 0;

}
void Cmd_Captain_f(edict_t *ent)
{
	int i;
	char temp[200];

        if (ent->client->resp.team == NOTEAM || !teamplay->value)
		{
			gi.cprintf(ent, PRINT_HIGH, "You need to be on a team for that...\n");
            return;
		}
		if(ent->client->resp.captain == TEAM1)
		{
			sprintf(temp, "%s is no longer %s's Captain\n", ent->client->pers.netname,team1_name);
			temp[200] = 0;
			SendWorldMsg(temp,0,0);
			team1ready = 0;
			ent->client->resp.captain = 0;
			return;
		}
		if(ent->client->resp.captain == TEAM2)
		{
			sprintf(temp, "%s is no longer %s's Captain\n", ent->client->pers.netname,team2_name);
			SendWorldMsg(temp,0,0);
			team2ready = 0;
			ent->client->resp.captain = 0;
			return;
		}
		i = CheckForCaptains(ent->client->resp.team);
		if (i == ent->client->resp.team)
		{
			gi.cprintf(ent, PRINT_HIGH, "Your team already has a Captain\n");
            return;

		}
		if ( i == 0)
		{
			if(ent->client->resp.team == 1)
			{
				sprintf(temp, "%s is now %s's Captain\n", ent->client->pers.netname,team1_name);
				temp[200] = 0;
				SendWorldMsg(temp,1,0);
				ent->client->resp.captain = 1;
			}
			else if (ent->client->resp.team == 2)
			{
			    sprintf(temp, "%s is now %s's Captain\n", ent->client->pers.netname,team2_name);
				temp[200] = 0;
				SendWorldMsg(temp,1,0);
				ent->client->resp.captain = 2;

			}
			

		}
		
}

extern int started; // AQ2:M - Matchmode - Used for ready command

void Cmd_Ready_f(edict_t *ent)
{
	char temp[200];

	if(ent->client->resp.captain == 0)
	{
		gi.cprintf(ent, PRINT_HIGH, "You need to be a captain for that\n");
		return;
	}

/*	if(started && !captainpause->value)
	{
		gi.cprintf(ent, PRINT_HIGH, "You can't unready mid-match\n");
		return;
	}
*/

	if(ent->client->resp.captain == 1)
	{
		if (team1ready)
		{
			sprintf(temp, "%s \nis no longer ready to play!\n",team1_name);
			temp[200] = 0;
		    SendWorldMsg(temp,0,1);
			team1ready = 0;
			return;
		}
		else
		{
			sprintf(temp, "%s is ready to play!\n",team1_name);
			temp[200] = 0;
		    SendWorldMsg(temp,1,1);
			team1ready = 1;
			return;
		}

		
	}
	if(ent->client->resp.captain == 2)
	{
		if (team2ready)
		{
			sprintf(temp, "The %s \nis no longer ready to play!\n",team2_name);
			temp[200] = 0;
		    SendWorldMsg(temp,0,1);
			team2ready = 0;
			return;
		}
		else
		{
			sprintf(temp, "%s is ready to play!\n",team2_name);
			temp[200] = 0;
		    SendWorldMsg(temp,1,1);
			team2ready = 1;
			return;
		}

	}



}
void Cmd_Teamname_f(edict_t *ent)
{
	int i,u,team;
    char temp[1024];
	team = ent->client->resp.team;

	if(ent->client->resp.captain == 0)
	{
		gi.cprintf(ent, PRINT_HIGH, "You need to be a captain for that\n");
		return;
	}
	if(team == 1 && team1ready == 1)
	{
		gi.cprintf(ent, PRINT_HIGH, "You cannot use this while 'Ready'\n");
		return;
	}
	if(team == 2 && team2ready == 1)
	{
		gi.cprintf(ent, PRINT_HIGH, "You cannot use this while 'Ready'\n");
		return;
	}
	if(team_round_going)
	{
		gi.cprintf(ent, PRINT_HIGH, "You cannot use this while playing\n");
		return;
	}

	i = gi.argc();
	if(gi.argc() < 2)
	{
       if(team == 1)
			gi.cprintf(ent, PRINT_HIGH, "Your Team Name is %s\n",team1_name);
		else if(team == 2)
			gi.cprintf(ent, PRINT_HIGH, "Your Team Name is %s\n",team2_name);
       return;
	}
	strcpy(temp,gi.argv(1));
		for(u=2;u<=i;u++)
	{
		strcat(temp," ");
		strcat(temp,gi.argv(u));
	}
		temp[18] = 0;
	
	if(team == 1)
	{
		strcpy(team1_name,temp);
		temp[200] = 0;
	    gi.cprintf(ent, PRINT_HIGH, "New Team Name: %s\n",team1_name);
		return;
    }
	if(team == 2)
	{
		strcpy(team2_name,temp);
		gi.cprintf(ent, PRINT_HIGH, "New Team Name: %s\n",team2_name);
		return;
    }
}

void Cmd_Teamskin_f(edict_t *ent)
{
	char *s;
	int i,team;
    char temp[1024];

    team = ent->client->resp.team;
	if(ent->client->resp.captain == 0)
	{
		gi.cprintf(ent, PRINT_HIGH, "You need to be a captain for that\n");
		return;
	}
	if(team == 1 && team1ready == 1)
	{
		gi.cprintf(ent, PRINT_HIGH, "You cannot use this while 'Ready'\n");
		return;
	}
	if(team == 2 && team2ready == 1)
	{
		gi.cprintf(ent, PRINT_HIGH, "You cannot use this while 'Ready'\n");
		return;
	}
	if(team_round_going)
	{
		gi.cprintf(ent, PRINT_HIGH, "You cannot use this while playing\n");
		return;
	}


	i = gi.argc();
	if(gi.argc() < 2)
	{
		s = Info_ValueForKey (ent->client->pers.userinfo, "skin");
		if(ent->client->resp.team == 1)
			gi.cprintf(ent, PRINT_HIGH, "Your Team Skin is %s\n",team1_skin);
		else
			gi.cprintf(ent, PRINT_HIGH, "Your Team Skin is %s\n",team2_skin);
       return;
	}
	strcpy(temp,gi.argv(1));
	temp[32] = 0;
		
	if(team == 1)
	{
		strcpy(team1_skin,temp);
		sprintf(team1_skin_index, "../players/%s_i", team1_skin);
		gi.cprintf(ent, PRINT_HIGH, "New Team Skin: %s\n",team1_skin);
		return;
    }
	if(team == 2)
	{
		strcpy(team2_skin,temp);
		sprintf(team2_skin_index, "../players/%s_i", team2_skin);
		gi.cprintf(ent, PRINT_HIGH, "New Team Skin: %s\n",team2_skin);
		return;
    }

}