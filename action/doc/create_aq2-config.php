<?php
  //-----------------------------------------------------------------------------
  // create_aq2-config.php
  //-----------------------------------------------------------------------------
  // $Id: create_aq2-config.php,v 1.1 2001/05/13 14:31:56 igor_rock Exp $
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
    header ("Content-Disposition: attachment; filename=$data[configname]");
    header ("Content-Description: AQ2:TNG configfile");
    }
 
  print "#-----------------------------------------------------------------------------\n";
  print "# AQ2:TNG Config file $data[configname]\n";
  print "#\n";
  print "# created: ".date("Y-m-d, H:i:s")."\n";
  print "# with the AQ2:TNG config generator at http://www.FIXME.com\n";
  print "#-----------------------------------------------------------------------------\n\n";

  print "game action\n";
  print "gamedir action\n";
  
  print "\n#-----------------------------------------------------------------------------\n";
  print "# General Settings\n";
  print "#-----------------------------------------------------------------------------\n";
  print "hostname \"$data[hostname]\"\n";
  print "rcon_password \"$data[rcon_password]\"\n";
  $dmflags = 0;
  for ($i = 0; $i < 20; $i++)
    {
    $dmflags += $data[dmflag][$i];
    }
  print "dmflags $dmflags\n";
  print "public $data[public]\n";
  print "masterserver $data[masterserver]\n";
  print "maplistname $data[maplistname]\n";
  print "ininame $data[ininame]\n";
  print "maxclients $data[maxclients]\n";

  print "\n#-----------------------------------------------------------------------------\n";
  print "# Maprotation\n";
  print "#-----------------------------------------------------------------------------\n";
  print "actionmaps $data[actionmaps]\n";
  print "rrot $data[rrot]\n";
  print "vrot $data[vrot]\n";

  print "\n#-----------------------------------------------------------------------------\n";
  print "# Game Mode Settings\n";
  print "#-----------------------------------------------------------------------------\n";
  print "deathmatch $data[deathmatch]\n";
  print "teamplay $data[teamplay]\n";
  print "ctf $data[ctf]\n";
  print "use_3teams $data[use_3teams]\n";
  print "use_tourney $data[use_tourney]\n";
  
  print "\n#-----------------------------------------------------------------------------\n";
  print "# Limits\n";
  print "#-----------------------------------------------------------------------------\n";
  print "fraglimit $data[fraglimit]\n";
  print "timelimit $data[timelimit]\n";
  print "roundlimit $data[roundlimit]\n";
  print "roundtimelimit $data[roundtimelimit]\n";
  print "limchasecam $data[limchasecam]\n";
  print "knifelimit $data[knifelimit]\n";

  print "\n#-----------------------------------------------------------------------------\n";
  print "# Lag related settings\n";
  print "#-----------------------------------------------------------------------------\n";
  print "llsound $data[llsound]\n";
  print "bholelimit $data[bholelimit]\n";
  print "splatlimit $data[splatlimit]\n";
  print "shelloff $data[shelloff]\n";
  print "sv_gib $data[sv_gib]\n";
  print "breakableglass $data[breakableglass]\n";
  print "glassfragmentlimit $data[glassfragmentlimit]\n";
  
  print "\n#-----------------------------------------------------------------------------\n";
  print "# Miscellaneous Options\n";
  print "#-----------------------------------------------------------------------------\n";
  print "allweapon $data[allweapon]\n";
  print "weapons $data[weapons]\n";
  print "allitem $data[allitem]\n";
  print "items $data[items]\n";
  print "ir $data[ir]\n";
  print "tgren $data[tgren]\n";
  print "use_voice $data[use_voice]\n";
  print "sv_crlf $data[sv_crlf]\n";
  print "dmweapon $data[dmweapon]\n";
  print "hc_single $data[hc_single]\n";

  print "\n#-----------------------------------------------------------------------------\n";
  print "# Voting\n";
  print "#-----------------------------------------------------------------------------\n";
  print "";
  print "cvote $data[cvote]\n";
  print "cvote_min $data[cvote_min]\n";
  print "cvote_need $data[cvote_need]\n";
  print "cvote_pass $data[cvote_pass]\n";

  print "use_mapvote $data[use_mapvote]\n";
  print "mapvote_min $data[mapvote_min]\n";
  print "mapvote_need $data[mapvote_need]\n";
  print "mapvote_pass $data[mapvote_pass]\n";
  print "mv_public $data[mv_public]\n";

  print "use_kickvote $data[use_kickvote]\n";
  print "kickvote_min $data[kickvote_min]\n";
  print "kickvote_need $data[kickvote_need]\n";
  print "kickvote_pass $data[kickvote_pass]\n";
  print "kickvote_tempban $data[kickvote_tempban]\n";
  print "vk_public $data[vk_public]\n";

  print "\n#-----------------------------------------------------------------------------\n";
  print "# CTF Settigns\n";
  print "#-----------------------------------------------------------------------------\n";
  print "ctf_effects $data[ctf_effects]\n";
  print "ctf_flag_respawn_time $data[ctf_flag_respawn_time]\n";
  print "ctf_player_respawn_time $data[ctf_player_respawn_time]\n";
  print "ctf_item_remove_time $data[ctf_item_remove_time]\n";

  print "\n-----------------------------------------------------------------------------\n";
  print "# Config End\n";
  print "#-----------------------------------------------------------------------------\n";
?>