#include "antimacro.txt"

#define PRESSED(%0) \
(((newkeys & (%0)) == (%0)) && ((oldkeys & (%0)) != (%0)))

// Does: update the antimacro variables data for this player
UpdatePlayerSprintMacroData(playerid, speed, tickcount, bool:forget)
{
	AntimacroData[playerid][LastTimeSprinted] = tickcount;
	AntimacroData[playerid][LastMonitoredSpeed] = speed;
	if(forget && AntimacroData[playerid][TimesWarned] > 0)
	{
		if((tickcount - AntimacroData[playerid][LastTimeWarned]) >= (ANTIMACRO_FORGET_WARNING_AFTER - GetPlayerPing(playerid)))  // Has it been a while since they last got suspected?
			AntimacroData[playerid][TimesWarned] = 0; // forgive them and forget their bad history.
	}
	return 1;
}

bool:CheckPlayerSprintMacro(playerid, newkeys, oldkeys)
{	
	if(PRESSED(KEY_SPRINT)) // if pressed the sprint key
	{
		// Avoiding false positive...
		#if ANTIMACRO_BADNET_CHECK == 1
		if(NetStats_PacketLossPercent(playerid) >= ANTIMACRO_PACKETLOSS) // Is the packetloss dangerous?
		{
			//UpdatePlayerSprintMacroData(playerid, speed, tick, true);
			return false;
		}
		if(Player[playerid][PauseCount] > 1) // Player hasn't sent updates for a second, he's probably de-synced
		{
			//UpdatePlayerSprintMacroData(playerid, speed, tick, true);
			return false;
		}
		#endif
		new speed = GetPlayerSpeed(playerid); // player speed
		new tick = GetTickCount(); // current tick count
		if(GetPlayerVehicleID(playerid) != 0) // It's dumb to check for sprint macros if they're in a vehicle
		{
			UpdatePlayerSprintMacroData(playerid, speed, tick, true);
			return false;
		}
		if(GetPlayerSurfingVehicleID(playerid) != INVALID_VEHICLE_ID) // If player is surfing a vehicle, his speed is affected by the vehicle speed
		{
			UpdatePlayerSprintMacroData(playerid, speed, tick, true);
			return false;
		}

		if(speed < ANTIMACRO_MIN_SPEED) // If player's speed is less than the pre-defined speed, then he's harmless with that speed.
		{
			UpdatePlayerSprintMacroData(playerid, speed, tick, true);
			return false;
		}
		// Doing advanced checks now...
		if((speed - AntimacroData[playerid][LastMonitoredSpeed]) < ANTIMACRO_MIN_SPEED_DIFF) 
		{
			// We may not suspect them if their speed progress is less than this!
			UpdatePlayerSprintMacroData(playerid, speed, tick, true);
			return false;
		}
		new diff = tick - AntimacroData[playerid][LastTimeSprinted];
		if(diff >= ANTIMACRO_SPRINT_KEY_LIMIT || diff == 0)
		{
			// The time that's passed since they last pressed sprint key is okay
			UpdatePlayerSprintMacroData(playerid, speed, tick, true);
			return false;
		}
		AntimacroData[playerid][TimesWarned] ++;
		AntimacroData[playerid][LastTimeWarned] = tick;
		if(AntimacroData[playerid][TimesWarned] == ANTIMACRO_MAX_WARNINGS) // Have they been warned/suspected for sprint macros enough?
		{

			AntimacroData[playerid][TimesWarned] = 0;
			AntimacroData[playerid][LastTimeWarned] = 0; 
			new str[144];
			format(str, sizeof(str), "{FF0000}Report: {%06x}(%i) %s {FF0000}(Macros)",GetPlayerColor(playerid) >>> 8,playerid,GetName(playerid));
			SendClientMessageToAll(-1, str);
			TogglePlayerControllable(playerid, false);
			TogglePlayerControllable(playerid, true);
			return true; 
		}
		UpdatePlayerSprintMacroData(playerid, speed, tick, false);
	}
	return false;
}

stock GetPlayerSpeed(playerid)
{
    new Float:velocity[4];
    GetPlayerVelocity(playerid,velocity[0],velocity[1],velocity[2]);
    velocity[3] = floatsqroot(floatpower(floatabs(velocity[0]), 2.0) + floatpower(floatabs(velocity[1]), 2.0) + floatpower(floatabs(velocity[2]), 2.0)) * 179.28625;
    return floatround(velocity[3]);
}
