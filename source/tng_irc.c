//-----------------------------------------------------------------------------
// IRC related functions
//
// $Id: tng_irc.c,v 1.1 2003/06/15 21:45:11 igor Exp $
//
//-----------------------------------------------------------------------------
// $Log: tng_irc.c,v $
// Revision 1.1  2003/06/15 21:45:11  igor
// added IRC client
//
//-----------------------------------------------------------------------------

#define DEBUG 1

//-----------------------------------------------------------------------------
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdarg.h>

#define TNG_IRC_C 

#include "g_local.h"

#define IRC_DISABLED		0
#define IRC_CONNECTING		2
#define IRC_CONNECTED		3
#define IRC_JOINED		4

#define IRC_ST_DISABLED		"disabled"
#define IRC_ST_CONNECTING	"connecting..."
#define IRC_ST_CONNECTED	"connected..."
#define IRC_ST_JOINED		"channel joined"

#define IRC_QUIT		"QUIT :I'll be back!\n"

tng_irc_t   irc_data;

void
IRC_init ( void )
{
  // init all cvars
  ircserver   = gi.cvar ("ircserver", IRC_SERVER, 0);
  ircport     = gi.cvar ("ircport", "6667", 0);
  ircchannel  = gi.cvar ("ircchannel", IRC_CHANNEL, 0);
  ircuser     = gi.cvar ("ircuser", IRC_USER, 0);
  ircpasswd   = gi.cvar ("ircpasswd", IRC_PASSWD, 0);
  irctopic    = gi.cvar ("irctopic", IRC_TOPIC, 0);
  ircbot      = gi.cvar ("ircbot", "0", 0);
  ircstatus   = gi.cvar ("ircstatus", IRC_ST_DISABLED, 0);
  ircop       = gi.cvar ("ircop", "", 0);
  ircmlevel   = gi.cvar ("ircmlevel", "6", 0);
  ircdebug    = gi.cvar ("ircdebug", "0", 0);
  ircadminpwd = gi.cvar ("ircadminpwd", "off", 0);
  
  // init our internal structure
  irc_data.ircstatus = IRC_DISABLED;
  bzero(irc_data.input, sizeof(irc_data.input));
}


void
IRC_exit ( void )
{
  if (irc_data.ircstatus != IRC_DISABLED) {
    write (irc_data.ircsocket, IRC_QUIT, strlen(IRC_QUIT));
    if (ircdebug->value)
      gi.dprintf ("IRC: %s", IRC_QUIT);
    irc_data.ircstatus = IRC_DISABLED;
    strcpy (ircstatus->string, IRC_ST_DISABLED);
    close (irc_data.ircsocket);
  }
}


void
irc_connect ( void )
{
  char               outbuf[IRC_BUFLEN];
  int                flags;
  struct sockaddr_in hostaddress;
  struct in_addr     ipnum;
  struct hostent     *hostdata;

  
  irc_data.ircstatus = IRC_CONNECTING;
  strcpy  (ircstatus->string, IRC_ST_CONNECTING);
  irc_data.ircsocket = -1;
  irc_data.ircport   = htons(ircport->value);
  strncpy (irc_data.ircserver, ircserver->string, IRC_BUFLEN);
  strncpy (irc_data.ircuser, ircuser->string, IRC_BUFLEN);
  strncpy (irc_data.ircpasswd, ircpasswd->string, IRC_BUFLEN);
  strncpy (irc_data.ircchannel, ircchannel->string, IRC_BUFLEN);
  
  if (! inet_aton(irc_data.ircserver, &ipnum)) {
    /* Maybe it's a FQDN */
    hostdata = gethostbyname (irc_data.ircserver);
    if (hostdata==NULL) {
      gi.dprintf ("IRC: invalid hostname or wrong address! Please use an existing hostname or,the xxx.xxx.xxx.xxx format.\n");
      ircbot->value = 0;
      irc_data.ircstatus = IRC_DISABLED;
      strcpy (ircstatus->string, IRC_ST_DISABLED);
    } else {
      /* Seems I'm just to stupid to find the right functio for this... */
      ((char *)&ipnum.s_addr)[0] = hostdata->h_addr_list[0][0];
      ((char *)&ipnum.s_addr)[1] = hostdata->h_addr_list[0][1];
      ((char *)&ipnum.s_addr)[2] = hostdata->h_addr_list[0][2];
      ((char *)&ipnum.s_addr)[3] = hostdata->h_addr_list[0][3];
    }
  }
  
  if (ircbot->value) {
    gi.dprintf ("IRC: using server %s:%i\n", inet_ntoa(ipnum), ntohs(irc_data.ircport));                                          
    bzero((char *) &hostaddress, sizeof(hostaddress));
    hostaddress.sin_family = AF_INET;
    hostaddress.sin_addr.s_addr = ipnum.s_addr;
    hostaddress.sin_port = irc_data.ircport;
    if ((irc_data.ircsocket = socket (AF_INET, SOCK_STREAM, 0)) == -1) {
      gi.dprintf ("IRC: couldn't open socket.\n");
      ircbot->value = 0;
      irc_data.ircstatus = IRC_DISABLED;
      strcpy (ircstatus->string, IRC_ST_DISABLED);
    } else {
      if (connect(irc_data.ircsocket, (struct sockaddr *)&hostaddress, sizeof(hostaddress)) == -1) {
	gi.dprintf ("IRC: couldn't connect socket.\n");
	ircbot->value = 0;
	irc_data.ircstatus = IRC_DISABLED;
	strcpy (ircstatus->string, IRC_ST_DISABLED);
      } else {
	gi.dprintf ("IRC: connected to %s:%d\n", irc_data.ircserver, ntohs(irc_data.ircport));
	flags = fcntl(irc_data.ircsocket, F_GETFL);
	flags |= O_NONBLOCK;
	if (fcntl(irc_data.ircsocket, F_SETFL, (long) flags)) {
	  gi.dprintf ("IRC: couldn't switch to non-blocking\n");
	  close (irc_data.ircsocket);
	  ircbot->value = 0;
	  irc_data.ircstatus = IRC_DISABLED;
	  strcpy (ircstatus->string, IRC_ST_DISABLED);
	} else {
	  sprintf (outbuf, "NICK %s\nUSER tng-mbot * * :%s\n", irc_data.ircuser, hostname->string);
	  write (irc_data.ircsocket, outbuf, strlen(outbuf));
	  if (ircdebug->value)
	    gi.dprintf("IRC: >> NICK %s\nIRC: >> tng-mbot * * :%s\n", irc_data.ircuser, hostname->string);
	}
      }
    }
  }
}


void
irc_parse ( void )
{
  int   i;
  int   pos;
  char *cp;
  char  outbuf[IRC_BUFLEN];
  char  wer[256];
  
  bzero(outbuf, sizeof(outbuf));
  
  if (strlen (irc_data.input)) {
    if (ircdebug->value)
      gi.dprintf ("IRC: << %s\n", irc_data.input);

    if (*irc_data.input == ':') {
      for ( pos=1; irc_data.input[pos]; pos++) {
	if (irc_data.input[pos] == ' ') {
	  break;
	} else {
	  wer[pos-1] = irc_data.input[pos];
	}
      }
      wer[pos-1] = 0;
      pos++;

      if (Q_strncasecmp (wer, irc_data.ircuser, strlen(irc_data.ircuser)) == 0) {
	cp = strchr(irc_data.input, ' ');
	cp++; // skip the space
	if (Q_strncasecmp (cp, "JOIN :", 6) == 0) {  // channel JOINED
	  cp += 6;
	  gi.dprintf ("IRC: joined channel %s\n", cp);
	  if (Q_strncasecmp(cp, irc_data.ircchannel, strlen(irc_data.ircchannel)) == 0) {
	    // joined our channel
	    irc_data.ircstatus = IRC_JOINED;
	    strcpy (ircstatus->string, IRC_ST_JOINED);
	    if (*ircop->string) {
	      if (ircdebug->value)
		gi.dprintf ("IRC: >> %s\n", ircop->string);
	      sprintf (outbuf, "%s\n", ircop->string);
	      write(irc_data.ircsocket, outbuf, strlen(outbuf));
	    }
	  } else {
	    // joined another channel
	  }
	} else {
	  // Maybe Future Extesion
	}
      } else if (Q_strncasecmp (&irc_data.input[pos], "004 ", 4) == 0) {
	sprintf(outbuf, "mode %s +i\n", irc_data.ircuser);
	if (ircdebug->value)
	  gi.dprintf("IRC: >> mode %s +i set\n", irc_data.ircuser);
	write(irc_data.ircsocket, outbuf, strlen(outbuf));

	if (irc_data.ircpasswd[0]) {
	  sprintf(outbuf, "join %s %s\n", irc_data.ircchannel, irc_data.ircpasswd);
	  gi.dprintf ("IRC: trying to join channel %s %s\n", irc_data.ircchannel, irc_data.ircpasswd);
	  write(irc_data.ircsocket, outbuf, strlen(outbuf));
	  sprintf (outbuf, "mode %s +mntk %s\n", irc_data.ircchannel, irc_data.ircpasswd);
	  write(irc_data.ircsocket, outbuf, strlen(outbuf));
	} else {
	  sprintf(outbuf, "join %s\n", irc_data.ircchannel);
	  gi.dprintf ("IRC: trying to join channel %s\n", irc_data.ircchannel);
	  write(irc_data.ircsocket, outbuf, strlen(outbuf));
	  sprintf (outbuf, "mode %s +mnt\n", irc_data.ircchannel);
	  write(irc_data.ircsocket, outbuf, strlen(outbuf));
	}
	
	irc_data.ircstatus = IRC_CONNECTED;
	strcpy (ircstatus->string, IRC_ST_CONNECTED);
      } else if (Q_strncasecmp (&irc_data.input[pos], "PRIVMSG ", 8) == 0) {
	pos += 8;
	if (Q_strncasecmp (&irc_data.input[pos], irc_data.ircuser, strlen(irc_data.ircuser)) == 0) {
	  pos += strlen(irc_data.ircuser) + 2;
	  if ((Q_strncasecmp (&irc_data.input[pos], ircadminpwd->string, strlen(ircadminpwd->string)) == 0) &&
	      (Q_strncasecmp (ircadminpwd->string, "off", 3) != 0)) {
	    pos += strlen(ircadminpwd->string) + 1;
	    for (i=0; i < strlen(wer); i++) {
	      if (wer[i] == '!') {
		wer[i] = 0;
	      }
	    }
	    if (Q_strncasecmp (&irc_data.input[pos], "op", 2) == 0) {
	      gi.dprintf ("IRC: set +o %s\n", wer);
	      write (irc_data.ircsocket, outbuf, strlen(outbuf));
	    } else if (Q_strncasecmp (&irc_data.input[pos], "say", 3) == 0) {
	      pos += 4;
	      if (strlen(&irc_data.input[pos]) > 225) {
		irc_data.input[pos+225] = 0;
	      }
	      sprintf (outbuf, "say %s\n", &irc_data.input[pos]);
	      gi.AddCommandString (outbuf);
	      IRC_printf (IRC_T_TALK, "console: %s\n", &irc_data.input[pos]);
	    }
	  }
	} else if (Q_strncasecmp (&irc_data.input[pos], irc_data.ircchannel, strlen(irc_data.ircchannel)) == 0) {
	  pos += strlen(irc_data.ircchannel) + 1;
	  // Maybe Future Extesion
	}
      } else if (Q_strncasecmp (&irc_data.input[pos], "NOTICE ", 7) == 0) {
	pos += 7;
	// Maybe Future Extesion
      } else if (strstr (irc_data.input, irc_data.ircuser)) {
	if (strstr (irc_data.input, " KICK ") || strstr (irc_data.input, "kick")) {
	  if (irc_data.ircpasswd[0]) {
	    sprintf(outbuf, "join %s %s\n", irc_data.ircchannel, irc_data.ircpasswd);
	    gi.dprintf ("IRC: trying to join channel %s %s (got kicked)\n", irc_data.ircchannel, irc_data.ircpasswd);
	    write(irc_data.ircsocket, outbuf, strlen(outbuf));
	  } else {
	    sprintf(outbuf, "join %s\n", irc_data.ircchannel);
	    gi.dprintf ("IRC: trying to join channel %s (got kicked)\n", irc_data.ircchannel);
	    write(irc_data.ircsocket, outbuf, strlen(outbuf));
	  }
	}
      }
    } else if (Q_strncasecmp(irc_data.input, "PING :", 6) == 0) {
      /* answer with a pong */
      sprintf(outbuf, "PONG %s\n", &irc_data.input[6]);
      if (ircdebug->value)		
	gi.dprintf ("IRC: >> %s\n", outbuf);
      write(irc_data.ircsocket, outbuf, strlen(outbuf));
    }
  }  
}


//
// puffered IO for asynchronous IRC socket
//
void
irc_getinput ( void )
{
  size_t      length;
  int         anz_net = 0;
  char       *anfang;
  char       *ende;
  char       *abs_ende;
  char        inbuf[IRC_BUFLEN];

  bzero(inbuf, sizeof(inbuf));
  
  anz_net = read(irc_data.ircsocket, inbuf, sizeof(inbuf));
  if (anz_net <= 0) {
    if ((anz_net == 0) || ((anz_net == -1) && (errno != EAGAIN))) {
      gi.dprintf ("IRC: connection terminated!\n");
      close (irc_data.ircsocket);
      irc_data.ircstatus = IRC_DISABLED;
      strcpy (ircstatus->string, IRC_ST_DISABLED);
    }
  } else if (anz_net > 0) {
    anfang = inbuf;
    abs_ende = inbuf + strlen(inbuf);
    if ((abs_ende - inbuf ) > sizeof(inbuf)) {
      abs_ende = inbuf + sizeof(inbuf);
    }
    while (anfang < abs_ende) {
      ende = memchr(anfang, 13, abs_ende - anfang);
      if (ende != NULL) {
	// Zeilenende gefunden
	*ende = 0;
	if (strlen(irc_data.input)) {
	  // schon etwas im Puffer, also anhaengen
	  strcat (irc_data.input, anfang);
	} else {
	  // Puffer leer, also nur kopieren
	  strcpy (irc_data.input, anfang);
	}
	irc_parse();
	// danach Puffer leeren
	bzero(irc_data.input, sizeof(irc_data.input));
	anfang = ende + 1;
	if ((*anfang == 13) || (*anfang == 10)) {
	  anfang++;
	}
      } else {
	length = abs_ende - anfang;
	if (memchr(anfang, 0, length) != NULL) {
	  length = strlen(anfang);
	}
	if (strlen(irc_data.input)) {
	  // schon etwas im Puffer, also anhaengen
	  strncat (irc_data.input, anfang, length);
	} else {
	  // Puffer leer, also nur kopieren
	  strncpy (irc_data.input, anfang, length);
	}
	irc_data.input[length] = 0;
	anfang += length;
      }
    }
  }
}


void
IRC_poll (void)
{
  if (ircbot->value == 0) {
    if (irc_data.ircstatus != IRC_DISABLED) {
      IRC_exit ();
    }
  } else {
    if  (irc_data.ircstatus == IRC_DISABLED) {
      irc_connect ();
    } else {
      irc_getinput ();
    }
  }
}


//
// this function is a little bit more complex.
// in the format string you can have the following parameters:
// %n -> names (nicks, teams, server)
// %w -> weapon/item
// %v -> vote item
// %k -> kills/frags/rounds
// %s -> normal string
void
IRC_printf (int type, char *fmt, ... )
{
  char    outbuf[IRC_BUFLEN];
  char    message[IRC_BUFLEN-128];
  char    topic[IRC_BUFLEN-128];
  char   *s;
  int     i;
  int     std_col;
  int     mpos;
  int     tpos;
  int     normal = 0;
  va_list ap;
  
  bzero(message, sizeof(message));
  bzero(topic, sizeof(topic));
  
  if (irc_data.ircstatus == IRC_JOINED) {
    if ((type <= ircmlevel->value) ||
	((type == IRC_T_TOPIC) && (ircmlevel->value >= IRC_T_GAME))) {
      switch (type) {
      case IRC_T_SERVER:
	{
	  std_col = IRC_C_ORANGE;
	  break;
	}
      
      case IRC_T_TOPIC:
	{
	  // set the topic
	  // no break, so we use GAME settings for the colors
	}
      case IRC_T_GAME:
	{
	  std_col = IRC_C_DRED;
	  break;
	}
      
      case IRC_T_DEATH:
	{
	  std_col = IRC_C_GREY75;
	  break;
	}
      
      case IRC_T_KILL:
	{
	  std_col = IRC_C_GREY25;
	  break;
	}
      
      case IRC_T_VOTE:
	{
	  std_col = IRC_C_GREY50;
	  break;
	}
      
      case IRC_T_TALK:
	{
	  std_col = IRC_C_GREY50;
	  break;
	}
      
      default:
	{
	  std_col = IRC_C_GREY50;
	  break;
	}
      }
    
      mpos = 0;
      tpos = 0;
      va_start (ap, fmt);
      while (*fmt) {
	if (*fmt == '%') {
	  fmt++;
	  switch (*fmt) {
	  case 'n':
	    {
	      s = va_arg(ap, char *);
	      sprintf (&message[mpos], "%c%d%s", IRC_CMCOLORS, IRC_C_DBLUE, s);
	      mpos += strlen (&message[mpos]);
	      sprintf (&topic[tpos], "%s", s);
	      tpos += strlen (&topic[tpos]);
	      normal = 0;
	      break;
	    }
	  
	  case 'w':
	    {
	      s = va_arg(ap, char *);
	      sprintf (&message[mpos], "%c%d%s", IRC_CMCOLORS, IRC_C_BLUE, s);
	      mpos += strlen (&message[mpos]);
	      sprintf (&topic[tpos], "%s", s);
	      tpos += strlen (&topic[tpos]);
	      normal = 0;
	      break;
	    }
	  
	  case 'v':
	    {
	      s = va_arg(ap, char *);
	      sprintf (&message[mpos], "%c%d%s", IRC_CMCOLORS, IRC_C_DGREEN, s);
	      mpos += strlen (&message[mpos]);
	      sprintf (&topic[tpos], "%s", s);
	      tpos += strlen (&topic[tpos]);
	      normal = 0;
	      break;
	    }
	  
	  case 'k':
	    {
	      i = va_arg(ap, int);
	      sprintf (&message[mpos], "%c%d%c", IRC_CBOLD, i, IRC_CBOLD);
	      mpos += strlen (&message[mpos]);
	      sprintf (&topic[tpos], "%d", i);
	      tpos += strlen (&topic[tpos]);
	      normal = 0;
	      break;
	    }
	  
	  case 's':
	    {
	      s = va_arg(ap, char *);
	      sprintf (&message[mpos], "%s", s);
	      mpos += strlen (&message[mpos]);
	      sprintf (&topic[tpos], "%s", s);
	      tpos += strlen (&topic[tpos]);
	      normal = 0;
	      break;
	    }
	  
	  default:
	    {
	      message[mpos++] = '%';
	      topic[tpos++]   = '%';
	      break;
	    }
	  }
	} else {
	  if (!normal) {
	    message[mpos++] = IRC_CMCOLORS;
	    sprintf (&message[mpos], "%d", std_col);
	    mpos += strlen (&message[mpos]);
	    normal = 1;
	  }
	  message[mpos++] = *fmt;
	  topic[tpos++] = *fmt;
	}
	fmt++;
      }
      message[mpos] = 0;
      topic[tpos] = 0;
      va_end(ap);
      // print it
      sprintf (outbuf, "PRIVMSG %s :%s\n", irc_data.ircchannel, message);
      if (ircdebug->value)
	gi.dprintf ("IRC: >> %s", outbuf);
      write (irc_data.ircsocket, outbuf, strlen(outbuf));
      if ((type == IRC_T_TOPIC) && (ircmlevel->value >= IRC_T_TOPIC)) {
	sprintf (outbuf, "TOPIC %s :%s %s\n", irc_data.ircchannel, irctopic->string, topic);
	write (irc_data.ircsocket, outbuf, strlen(outbuf));
      }
    }
  } else if (irc_data.ircstatus == IRC_CONNECTED) {
    if (irc_data.ircpasswd[0]) {
      sprintf(outbuf, "join %s %s\n", irc_data.ircchannel, irc_data.ircpasswd);
      gi.dprintf ("IRC: trying to join channel %s %s\n", irc_data.ircchannel, irc_data.ircpasswd);
      write(irc_data.ircsocket, outbuf, strlen(outbuf));
      sprintf (outbuf, "mode %s +mntk %s\n", irc_data.ircchannel, irc_data.ircpasswd);
      write(irc_data.ircsocket, outbuf, strlen(outbuf));
    } else {
      sprintf(outbuf, "join %s\n", irc_data.ircchannel);
      gi.dprintf ("IRC: trying to join channel %s\n", irc_data.ircchannel);
      write(irc_data.ircsocket, outbuf, strlen(outbuf));
      sprintf (outbuf, "mode %s +mnt\n", irc_data.ircchannel);
      write(irc_data.ircsocket, outbuf, strlen(outbuf));
    }
  }
}

void SVCmd_ircraw_f (void)
{
  int  i;
  char outbuf[IRC_BUFLEN];
  bzero (outbuf, sizeof(outbuf));

  if (irc_data.ircstatus == IRC_DISABLED) {
    gi.cprintf (NULL, PRINT_HIGH, "IRC: Not connected to IRC\n");
  } else {
    for (i = 2; i < gi.argc(); i++) {
      strcat (outbuf, gi.argv(i));
      strcat (outbuf, " ");
    }
    strcat (outbuf, "\n");
    if (ircdebug->value)
      gi.cprintf (NULL, PRINT_HIGH, "IRC: >> %s\n", outbuf);
    write (irc_data.ircsocket, outbuf, strlen(outbuf));
  }
}
