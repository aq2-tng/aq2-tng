<?php
  //-----------------------------------------------------------------------------
  // create_aq2-config.php
  //-----------------------------------------------------------------------------
  // $Id: create_aq2-config.php,v 1.10 2003/06/19 16:37:57 igor_rock Exp $
  //-----------------------------------------------------------------------------

  // check if the user wants to see or to save it
  if ($data[action] == "Show Config")
    {  
    header ("Content-type: text/plain");
    }
  else
    {
    header ("Content-type: application/aq2tng-config");
    }
  header ("Content-Disposition: attachment; filename=$data[configname]");
  header ("Content-Description: AQ2:TNG configfile");
 
  print "//-----------------------------------------------------------------------------\n";
  print "// AQ2:TNG Main Config File $data[configname]\n";
  print "//\n";
  print "// created: ".date("Y-m-d, H:i:s")."\n";
  print "// with the AQ2:TNG config generator at http://www.aq2tng.barrysworld.net/\n";
  print "//-----------------------------------------------------------------------------\n\n";

  print "game action\n";
  print "gamedir action\n";
  
  print "\n//-----------------------------------------------------------------------------\n";
  print "// General Settings\n";
  print "//-----------------------------------------------------------------------------\n";
  print "set rcon_password \"$data[rcon_password]\"\n";
  print "set public $data[public]\n";
  print "setmaster \"$data[masterserver]\"\n";
  print "set skipmotd $data[skipmotd]\n";
  print "set motd_time $data[motd_time]\n";
  print "set ppl_idletime $data[ppl_idletime]\n";
  print "set check_time $data[check_time]\n";
  print "set radiolog $data[radiolog]\n";
  print "set use_voice $data[use_voice]\n";
  print "set use_classic $data[use_classic]\n";
  print "set use_punch $data[use_punch]\n";
  print "set sv_crlf $data[sv_crlf]\n";
  print "set punishkills $data[punishkills]\n";

  print "\n//-----------------------------------------------------------------------------\n";
  print "// Maprotation\n";
  print "//-----------------------------------------------------------------------------\n";
  print "set actionmaps $data[actionmaps]\n";
  print "set rrot $data[rrot]\n";
  print "set vrot $data[vrot]\n";

  print "\n//-----------------------------------------------------------------------------\n";
  print "// Lag related settings\n";
  print "//-----------------------------------------------------------------------------\n";
  print "set llsound $data[llsound]\n";
  print "set bholelimit $data[bholelimit]\n";
  print "set splatlimit $data[splatlimit]\n";
  print "set shelloff $data[shelloff]\n";
  print "set sv_gib $data[sv_gib]\n";
  print "set breakableglass $data[breakableglass]\n";
  print "set glassfragmentlimit $data[glassfragmentlimit]\n";
  
  print "\n//-----------------------------------------------------------------------------\n";
  print "// Voting\n";
  print "//-----------------------------------------------------------------------------\n";
  print "";
  print "set use_cvote $data[use_cvote]\n";
  print "set cvote_min $data[cvote_min]\n";
  print "set cvote_need $data[cvote_need]\n";
  print "set cvote_pass $data[cvote_pass]\n";
  print "set configlistname $data[configlistname]\n";

  print "set use_mapvote $data[use_mapvote]\n";
  print "set mapvote_min $data[mapvote_min]\n";
  print "set mapvote_need $data[mapvote_need]\n";
  print "set mapvote_pass $data[mapvote_pass]\n";
  print "set mapvote_waittime $data[mapvote_waittime]\n";
  print "set mv_public $data[mv_public]\n";

  print "set use_kickvote $data[use_kickvote]\n";
  print "set kickvote_min $data[kickvote_min]\n";
  print "set kickvote_need $data[kickvote_need]\n";
  print "set kickvote_pass $data[kickvote_pass]\n";
  print "set kickvote_tempban $data[kickvote_tempban]\n";
  print "set vk_public $data[vk_public]\n";

  print "\n//-----------------------------------------------------------------------------\n";
  print "// Flood Protection\n";
  print "//-----------------------------------------------------------------------------\n";
  print "set flood_msgs $data[flood_msgs]\n";
  print "set flood_persecond $data[flood_persecond]\n";
  print "set flood_waitdelay $data[flood_waitdelay]\n";
  print "set radio_max $data[radio_max]\n";
  print "set radio_time $data[radio_time]\n";
  print "set radio_repeat $data[radio_repeat]\n";
  print "set radio_ban $data[radio_ban]\n";

  print "\n//-----------------------------------------------------------------------------\n";
  print "// Teamkill Parameters\n";
  print "//-----------------------------------------------------------------------------\n";
  print "set maxteamkills $data[maxteamkills]\n";
  print "set tkbanrounds $data[tkbanrounds]\n";
  print "set twbanrounds $data[twbanrounds]\n";
  print "set ff_afterround $data[ff_afterround]\n";

  print "\n//-----------------------------------------------------------------------------\n";
  print "// IRC Settings\n";
  print "//-----------------------------------------------------------------------------\n";
  print "set ircbot $data[ircbot]\n";
  print "set ircserver \"$data[ircserver]\"\n";
  print "set ircport $data[ircport]\n";
  print "set ircuser \"$data[ircuser]\"\n";
  print "set ircop \"$data[ircop]\"\n";
  print "set ircchannel \"$data[ircchannel]\"\n";
  print "set ircpasswd \"$data[ircpasswd]\"\n";
  print "set ircadminpwd \"$data[ircadminpwd]\"\n";
  print "set ircdebug $data[ircdebug]\n";

  print "\n//-----------------------------------------------------------------------------\n";
  print "// Config End\n";
  print "//-----------------------------------------------------------------------------\n";

  print "exec $data[defaultconfig]\n";
?>
