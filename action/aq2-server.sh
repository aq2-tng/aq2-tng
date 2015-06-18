#!/bin/sh

cd ..
while true
do
	./quake2 +set dedicated 1 +set game action +set port 27910 +set ininame config/action.ini +set maplistname config/maplist.ini +set configlistname config/configlist.ini +exec config/aq2_tp.cfg
	sleep 10
done


