<?php
  //-----------------------------------------------------------------------------
  // create_aq2-config.php
  //-----------------------------------------------------------------------------
  // $Id: create_aq2-config.php,v 1.3 2001/05/31 17:01:48 igor_rock Exp $
  //-----------------------------------------------------------------------------

  // Do some checks for illegal game modes
  if ($data[ctf])
    {
    if ($data[use_3teams] or $data[use_tourney])
      {
      $data[use_3teams] = 0;
      $data[use_tourney] = 0;
      }
    else if (! $data[teamplay])
      {
      $data[teamplay] = 1;
      }
    }
  
  if ($data[use_tourney] and $data[use_3teams])
    {
    $data[use_3teams] = 0;
    }

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
  print "// AQ2:TNG Config file $data[configname]\n";
  print "//\n";
  print "// created: ".date("Y-m-d, H:i:s")."\n";
  print "// with the AQ2:TNG config generator at http://www.FIXME.com\n";
  print "//-----------------------------------------------------------------------------\n\n";

  print "game action\n";
  print "gamedir action\n";
  
  print "\n//-----------------------------------------------------------------------------\n";
  print "// General Settings\n";
  print "//-----------------------------------------------------------------------------\n";
  print "set hostname \"$data[hostname]\"\n";
  print "set rcon_password \"$data[rcon_password]\"\n";
  $dmflags = 0;
  for ($i = 0; $i < 20; $i++)
    {
    $dmflags += $data[dmflag][$i];
    }
  print "set dmflags $dmflags\n";
  print "set public $data[public]\n";
  print "set masterserver $data[masterserver]\n";
  print "set maplistname $data[maplistname]\n";
  print "set ininame $data[ininame]\n";
  print "set maxclients $data[maxclients]\n";

  print "\n//-----------------------------------------------------------------------------\n";
  print "// Maprotation\n";
  print "//-----------------------------------------------------------------------------\n";
  print "set actionmaps $data[actionmaps]\n";
  print "set rrot $data[rrot]\n";
  print "set vrot $data[vrot]\n";

  print "\n//-----------------------------------------------------------------------------\n";
  print "// Game Mode Settings\n";
  print "//-----------------------------------------------------------------------------\n";
  print "set deathmatch $data[deathmatch]\n";
  print "set teamplay $data[teamplay]\n";
  print "set ctf $data[ctf]\n";
  print "set use_3teams $data[use_3teams]\n";
  print "set use_tourney $data[use_tourney]\n";
  
  print "\n//-----------------------------------------------------------------------------\n";
  print "// Limits\n";
  print "//-----------------------------------------------------------------------------\n";
  print "set fraglimit $data[fraglimit]\n";
  print "set timelimit $data[timelimit]\n";
  print "set roundlimit $data[roundlimit]\n";
  print "set roundtimelimit $data[roundtimelimit]\n";
  print "set limchasecam $data[limchasecam]\n";
  print "set knifelimit $data[knifelimit]\n";

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
  print "// Miscellaneous Options\n";
  print "//-----------------------------------------------------------------------------\n";
  print "set allweapon $data[allweapon]\n";
  print "set weapons $data[weapons]\n";
  $wp_flags = 0;
  for ($i = 0; $i < 20; $i++)
    {
    $wp_flags += $data[wp_flags][$i];
    }
  print "set wp_flags $wp_flags\n";
  print "set allitem $data[allitem]\n";
  print "set items $data[items]\n";
  $itm_flags = 0;
  for ($i = 0; $i < 20; $i++)
    {
    $itm_flags += $data[itm_flags][$i];
    }
  print "set itm_flags $itm_flags\n";
  print "set ir $data[ir]\n";
  print "set tgren $data[tgren]\n";
  print "set use_voice $data[use_voice]\n";
  print "set sv_crlf $data[sv_crlf]\n";
  print "set dmweapon \"$data[dmweapon]\"\n";
  print "set hc_single $data[hc_single]\n";

  print "\n//-----------------------------------------------------------------------------\n";
  print "// Voting\n";
  print "//-----------------------------------------------------------------------------\n";
  print "";
  print "set cvote $data[cvote]\n";
  print "set cvote_min $data[cvote_min]\n";
  print "set cvote_need $data[cvote_need]\n";
  print "set cvote_pass $data[cvote_pass]\n";

  print "set use_mapvote $data[use_mapvote]\n";
  print "set mapvote_min $data[mapvote_min]\n";
  print "set mapvote_need $data[mapvote_need]\n";
  print "set mapvote_pass $data[mapvote_pass]\n";
  print "set mv_public $data[mv_public]\n";

  print "set use_kickvote $data[use_kickvote]\n";
  print "set kickvote_min $data[kickvote_min]\n";
  print "set kickvote_need $data[kickvote_need]\n";
  print "set kickvote_pass $data[kickvote_pass]\n";
  print "set kickvote_tempban $data[kickvote_tempban]\n";
  print "set vk_public $data[vk_public]\n";

  print "\n//-----------------------------------------------------------------------------\n";
  print "// CTF Settigns\n";
  print "//-----------------------------------------------------------------------------\n";
  print "set ctf_mode $data[ctf_mode]\n";
  print "set ctf_dropflag $data[ctf_dropflag]\n";
#  print "set ctf_forcejoin $data[ctf_forcejoin]\n";

  print "\n//-----------------------------------------------------------------------------\n";
  print "// Config End\n";
  print "//-----------------------------------------------------------------------------\n";

  print "map $data[startmap]\n";
?>