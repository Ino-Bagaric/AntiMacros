//Includes
#include <a_samp>
#include <antimacro.inc> 
#include <zcmd>

//Boolean

new bool:AntiMacros;

//Dialogs

#define D_MACROCFG 4444


public OnFilterScriptInit()
{

	AntiMacros = true;
}



public OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{

	if(AntiMacros == true && CheckPlayerSprintMacro(playerid, newkeys, oldkeys) == true)
	{
		return 1;
	}
	return 1;
}


public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{

if(dialogid == D_MACROCFG)
    {
        if(response)
        {
        	switch(listitem)
            {
                case 0:
                {
                	AntiMacros = !AntiMacros;
                	ShowDialog(playerid);
                }
                           
            }
        }

        return 1;
    }

return 1;
}



CMD:macrocfg(playerid,params[])
{
	if(IsPlayerAdmin(playerid)) //Adapt to your Admin System
	{
		SendClientMessage(playerid, -1, "You're not allowed to use this command");
		return 1;
	}
	ShowDialog(playerid);

	return 1;
}

ShowDialog(playerid)
{
	new
		buffer[350],
		status1[24];
		

	if (AntiMacros)
		status1 = "{80FF00}[ENABLED]";
	else
		status1 = "{FF8080}[DISABLED]";



	format (buffer, sizeof(buffer), "\
	Name\tStatus\tType\n\
	Anti Macros\t%s\tBOOLEAN", status1);

	ShowPlayerDialog(playerid, D_MACROCFG, DIALOG_STYLE_TABLIST_HEADERS, "Configuration", buffer, "Select", "Close");
	return 1;
}


stock GetName(playerid)
{
	new name[MAX_PLAYER_NAME];
	GetPlayerName(playerid, name, MAX_PLAYER_NAME);
	return name;
}
