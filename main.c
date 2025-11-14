/*
 * Split-Field - Two Player Cooperative Puzzle Game for PSP
 * Main Menu System
 */

#include <pspkernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <string.h>
#include <stdlib.h>
#include <psppower.h>
#include <pspiofilemgr.h>
#include "game.h"

PSP_MODULE_INFO("Split-Field", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

/* Define printf to use pspDebugScreenPrintf */
#define printf pspDebugScreenPrintf

/* Exit callback */
int exit_callback(int arg1, int arg2, void *common)
{
    sceKernelExitGame();
    return 0;
}

/* Callback thread */
int CallbackThread(SceSize args, void *argp)
{
    int cbid;

    cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
    sceKernelRegisterExitCallback(cbid);

    sceKernelSleepThreadCB();

    return 0;
}

/* Sets up the callback thread and returns its thread id */
int SetupCallbacks(void)
{
    int thid = 0;

    thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
    if(thid >= 0)
    {
        sceKernelStartThread(thid, 0, 0);
    }

    return thid;
}

/* Draw fancy ASCII title */
static void draw_title(void)
{
    pspDebugScreenSetXY(0, 3);
    printf("\n");
    printf("  ========================================================\n");
    printf("  #     ###  ####  #     ### #####                      #\n");
    printf("  #    #    #   #  #      #    #                        #\n");
    printf("  #     ##  ####   #      #    #                        #\n");
    printf("  #       # #      #      #    #                        #\n");
    printf("  #    ###  #      ##### ###   #                        #\n");
    printf("  #    #### ###  #### #     ####                        #\n");
    printf("  #    #     #   #    #     #   #                       #\n");
    printf("  #    ###   #   ###  #     #   #                       #\n");
    printf("  #    #     #   #    #     #   #                       #\n");
    printf("  #    #    ### #### ##### ####                         #\n");
    printf("  ========================================================\n");
}

int main(void)
{
    SceCtrlData pad;
    SceCtrlData oldpad;
    memset(&oldpad, 0, sizeof(oldpad));
    
    /* Set up callbacks */
    SetupCallbacks();

    /* Initialize the debug screen */
    pspDebugScreenInit();
    
    /* Draw menu once */
    int menu_needs_redraw = 1;

    /* Main menu loop */
    while(1)
    {
        /* Only redraw menu when needed */
        if (menu_needs_redraw) {
            /* Clear screen and set cursor to top-left */
            pspDebugScreenClear();
            pspDebugScreenSetXY(0, 0);

        /* Display fancy title */
        draw_title();

        /* Display menu options */
        printf("\n\n");
        printf("                    Press START to begin\n");
        printf("\n");
        printf("                    Press SELECT to exit\n");
        printf("\n\n");
        
        /* Game description */
        printf("        # 2 players share one PSP                  #\n");
        printf("        # Player 1: D-PAD controls                 #\n");
        printf("        # Player 2: ABXO buttons (as directions)   #\n");
        printf("        # Work together to push boxes to goals     #\n");
        printf("        # Avoid enemies (red tiles)                #\n");
        printf("        # Some boxes only one player can move!     #\n");
            menu_needs_redraw = 0;
        }

        /* Read controller input */
        sceCtrlReadBufferPositive(&pad, 1);

        /* Check if START button is pressed */
        if((pad.Buttons & PSP_CTRL_START) && !(oldpad.Buttons & PSP_CTRL_START))
        {
            /* Launch the game */
            GameContext game_ctx;
            game_init(&game_ctx);
            game_run(&game_ctx);
            game_cleanup(&game_ctx);
            
            /* Redraw menu after game ends */
            menu_needs_redraw = 1;
            continue;
        }

        /* Check if SELECT button is pressed to exit */
        if((pad.Buttons & PSP_CTRL_SELECT) && !(oldpad.Buttons & PSP_CTRL_SELECT))
        {
            break;
        }

        oldpad = pad;

        /* Wait for next frame to prevent flickering */
        sceDisplayWaitVblankStart();
    }

    /* Exit */
    sceKernelExitGame();
    return 0;
}
