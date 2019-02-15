#include "Atoms.h"
#include <string>
#include <numeric>
#include <common/Rectangle.h>
#include <common/ColorTable.h>

using namespace std;
using namespace common;

namespace ui {

    void Atoms::SecurityCodeSetup() {
        fileHandle_t f;
        char buffer[SECURITY_SIZE];
        int32_t fileLen;
        rpgxSecurityFile_t *code;
        rpgxSecurityFile_t wCode;
        static bool ui_SecuritySetup = false;

        /* QVM Hack */
        if (!ui_SecuritySetup) {
            ui_SecuritySetup = true;
        } else {
            return;
        }

        fileLen = trap_FS_FOpenFile(SECURITY_FILE, &f, FS_READ);

        if (!f) {
            UI_ConfirmMenu(menu_normal_text[MNT_ID_NOTTHERE], 0, [](qboolean b) {
                SecurityFeedback(b == qtrue ? true : false);
            });
            return;
        }

        if (fileLen != SECURITY_SIZE) {
            UI_Logger(LL_ERROR, "rpgxid.dat is wrong size. %i, should be %i\n", fileLen, SECURITY_SIZE);
            UI_ConfirmMenu(menu_normal_text[MNT_ID_WRONGSIZE], 0, [](qboolean b) {
                SecurityFeedback(b == qtrue ? true : false);
            });
            return;
        }

        trap_FS_Read(buffer, SECURITY_SIZE, f);
        trap_FS_FCloseFile(f);

        code = (rpgxSecurityFile_t *) ((char *) buffer);

#if defined(__linux__)
        unsigned long ID = (buffer[3]<<24|buffer[2]<<16|buffer[1]<<8|buffer[0]);
    unsigned long SECID = ('7'<<24|'X'<<16|'G'<<8|'R');
    if ( !code || ID != SECID)
#else
        if (!code || code->ID != SECURITY_ID)
#endif
        {
            if (!code)
                UI_Logger(LL_ERROR, "No data was able to be loaded\n");
            else
                UI_Logger(LL_ERROR, "ID was %" PRIu64 ", should be %u\n", code->ID, SECURITY_ID);

            UI_ConfirmMenu(menu_normal_text[MNT_ID_INVALID], 0, [](qboolean b) {
                SecurityFeedback(b == qtrue ? true : false);
            });
            return;
        }

        /* if hash is identical to console to default, then generate a new one */
        if (code->hash == atoul(sv_securityHash.string)
            &&
            code->hash == SECURITY_HASH) {
            fileHandle_t wf;
            int32_t i;

            /*UI_Logger( LL_DEBUG, "Building new key!\n" );*/

            memset(&wCode, 0, sizeof(rpgxSecurityFile_t));

            code = &wCode;

            code->ID = SECURITY_ID;

            /* generate our player hash */
            while (code->hash == 0 || code->hash == SECURITY_HASH) {
                unsigned long bit = 0;
                /* set a pretty good random seed */
                srand(trap_Milliseconds());
                /*code->hash = (int32_t)(rand() / (((double)RAND_MAX + 1)/ SECURITY_HASH));*/
                for (i = 0; i < 32; i++) {
                    if ((irandom(1, 2) - 1)) {
                        bit += 1 << i;
                    }

                    /*bit = bit | (irandom(1, 2)-1);*/
                    /*bit <<= 1;*/
                }

                code->hash = bit;
            }

            /* generate our player id */
            /* TiM - shifted to client to be built off of IP */
            while (code->playerID == 0 || code->playerID == SECURITY_PID) {
                srand((int32_t) (trap_Milliseconds() * irandom(0, 0x7FFF)));
                for (i = 0; i < 32; i++) {
                    code->playerID |= irandom(1, 2) - 1;
                    code->playerID <<= 1;
                }
            }
            /*code->playerID = SECURITY_PID;*/

            /*generate random padding to make viewing this in a hex editor harder*/
            /*code->padding	= irandom( 0, 0xffff );*/
            /*code->padding2	= irandom( 0, 0xffff );*/
            /*code->padding3	= irandom( 0, 0xffff );*/

            /* overwrite the file */
            trap_FS_FOpenFile(SECURITY_FILE, &wf, FS_WRITE);
            trap_FS_Write(code, SECURITY_SIZE, wf);
            trap_FS_FCloseFile(wf);

            trap_Cvar_Set("sv_securityHash", va("%" PRIu64, code->hash));
        }

        /*
         * update the security code value and lock it each time
         * from here, it is subsequently sent to the server on player connect
         */
        trap_Cvar_Set("sv_securityCode", va("%" PRIu64, code->playerID));

    }

    void Atoms::SecurityFeedback(bool result) {
        trap_Cmd_ExecuteText(EXEC_APPEND, "quit\n");
    }

    std::string Atoms::LanguageFilename(std::string_view baseName, std::string_view baseExtension) {
        std::string result;
        char language[32];
        fileHandle_t file;

        trap_Cvar_VariableStringBuffer("g_language", language, 32);

        /* If it's English then no extension */
        if (language[0] == '\0' || Q_stricmp("ENGLISH", language) == 0) {
            result = baseName.data() + "."s + baseExtension.data();
        } else {
            result = baseName.data() + "_"s + language + "." + baseExtension.data();

            /* Attempt to load the file */
            trap_FS_FOpenFile(result.data(), &file, FS_READ);

            if (file <= 0) /* This extension doesn't exist, go English. */
            {
                result = baseName.data() + "."s + baseExtension.data();
            } else {
                trap_FS_FCloseFile(file);
            }
        }

        return result;
    }

    void Atoms::InitSpinControl(menulist_s *spincontrol) {
        UI_LogFuncBegin();
        spincontrol->generic.type = MTYPE_SPINCONTROL;
        spincontrol->generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
        spincontrol->textcolor = CT_BLACK;
        spincontrol->textcolor2 = CT_WHITE;
        spincontrol->color = CT_DKPURPLE1;
        spincontrol->color2 = CT_LTPURPLE1;
        spincontrol->textX = MENU_BUTTON_TEXT_X;
        spincontrol->textY = MENU_BUTTON_TEXT_Y;
        UI_LogFuncEnd();
    }

    void Atoms::LoadButtonText() {
        int32_t len, i;
        fileHandle_t f;

        auto filename = LanguageFilename("ext_data/mp_buttontext", "dat");

        len = trap_FS_FOpenFile(filename.data(), &f, FS_READ);

        if (!f) {
            UI_Logger(LL_ERROR, "UI_LoadButtonText : MP_BUTTONTEXT.DAT file not found!\n");
            return;
        }

        if (len > MAXBUTTONTEXT) {
            UI_Logger(LL_ERROR, "UI_LoadButtonText : MP_BUTTONTEXT.DAT too big!\n");
            return;
        }

        for (i = 0; i < MBT_MAX; ++i) {
            menu_button_text[i][0] = menuEmptyLine.data();
            menu_button_text[i][1] = menuEmptyLine.data();
        }

        /* initialise the data area */
        memset(ButtonText, 0, sizeof(ButtonText));

        trap_FS_Read(ButtonText, len, f);

        trap_FS_FCloseFile(f);

        ParseButtonText();
    }

    void Atoms::ParseButtonText() {
        char *token;
        char *buffer;
        int32_t i;
        int32_t len;

        COM_BeginParseSession();

        buffer = ButtonText;

        i = 1;    /* Zero is null string */
        while (buffer) {
            /*G_ParseString( &buffer, &token);*/
            token = COM_ParseExt(&buffer, qtrue);

            len = strlen(token);
            if (len) {

                if ((len == 1) && (token[0] == '/'))    /* A NULL? */
                {
                    menu_button_text[i][0] = menuEmptyLine.data();
                    menu_button_text[i][1] = menuEmptyLine.data();
                } else {
                    menu_button_text[i][0] = (buffer - (len +
                                                        1));    /* The +1 is to get rid of the " at the beginning of the string.*/
                }

                *(buffer - 1) = '\0';        /* Place an string end where is belongs. */

                token = COM_ParseExt(&buffer, qtrue);
                len = strlen(token);
                if (len) {
                    menu_button_text[i][1] = (buffer - (len +
                                                        1));    /* The +1 is to get rid of the " at the beginning of the string. */
                    *(buffer - 1) = '\0';        /* Place an string end where is belongs. */
                }
                ++i;
            }

            if (uis.debug)
                UI_Logger(LL_DEBUG, "UI_ParseButtonText - Line: %i, String1: %s, String2: %s\n", i - 1,
                          menu_button_text[i - 1][0], menu_button_text[i - 1][1]);

            if (i > MBT_MAX) {
                UI_Logger(LL_ERROR, "UI_ParseButtonText : too many values! Needed %d but got %d.\n", MBT_MAX, i);
                return;
            }
        }
        if (i != MBT_MAX) {
            UI_Logger(LL_DEBUG, "UI_ParseButtonText : not enough lines. Read %d of %d!\n", i, MBT_MAX);
            for (; i < MBT_MAX; i++) {
                menu_button_text[i][0] = "?";
                menu_button_text[i][1] = "?";
            }
        }
    }

    void Atoms::LoadMenuText() {
        int32_t len;/*,i;*/
        fileHandle_t f;
        auto filename = LanguageFilename("ext_data/mp_normaltext", "dat");

        len = trap_FS_FOpenFile(filename.data(), &f, FS_READ);

        if (!f) {
            Com_Error(ERR_FATAL, "UI_LoadMenuText : MP_NORMALTEXT.DAT file not found!\n");
            return;
        }

        if (len > MAXMENUTEXT) {
            Com_Error(ERR_FATAL, "UI_LoadMenuText : MP_NORMALTEXT.DAT size (%d) > max (%d)!\n", len, MAXMENUTEXT);
            return;
        }

        /* initialise the data area */
        memset(MenuText, 0, sizeof(MenuText));

        trap_FS_Read(MenuText, len, f);

        trap_FS_FCloseFile(f);

        ParseMenuText();
    }

    void Atoms::ParseMenuText() {
        char *token;
        char *buffer;
        int32_t i;
        int32_t len;
        /*int32_t j;*/

        COM_BeginParseSession();

        buffer = MenuText;

        i = 1;    /* Zero is null string */
        while (buffer) {
            token = COM_ParseExt(&buffer, qtrue);

            len = strlen(token);
            if (len) {
                menu_normal_text[i] = (buffer - (len + 1));
                *(buffer - 1) = '\0';        /* Place an string end where is belongs. */
                i++;

            }

            if (uis.debug)
                UI_Logger(LL_DEBUG, "UI_ParseMenuText - Line: %i, String: %s\n", i - 1, menu_normal_text[i - 1]);

            if (i > MNT_MAX) {
                UI_Logger(LL_ERROR, "UI_ParseMenuText : too many values! Needed %d but got %d.\n", MNT_MAX, i);
                return;
            }
        }
        if (i != MNT_MAX) {
            UI_Logger(LL_ERROR, "UI_ParseMenuText : not enough lines. Read %d of %d!\n", i, MNT_MAX);
            for (; i < MNT_MAX; i++) {
                menu_normal_text[i] = "?";
            }
        }
    }

    void Atoms::MenuFrame2(menuframework_s *menu) {
        qboolean space = qtrue;

        if (!ui::Atoms::MenuFrame::initialized) {
            MenuFrame_Cache();
        }

        if (!ingameFlag) {
            menu->fullscreen = qtrue;
        } else    /* In game menu */
        {
            menu->fullscreen = qfalse;
        }

        if (menu->titleI) {
            DrawProportionalString(menu->titleX, menu_normal_text[menu->titleI],
                                   UI_RIGHT | UI_BIGFONT, ColorTable[CT_LTORANGE]);
        }

        trap_R_SetColor(colorTable[CT_DKBROWN1]);
        DrawHandlePic(30, 25, 47, 119, uis.whiteShader);    /* Top left column */
        DrawHandlePic(30, 147, 47, 53, uis.whiteShader);    /* left column */

        trap_R_SetColor(colorTable[CT_DKBROWN1]);
        /*UI_DrawHandlePic(  30, 175,  47,  25, uis.whiteShader);*/ /* Mid left column*/
        DrawHandlePic(30, 392, 47, 33, uis.whiteShader);    /* Bottom left column */
        DrawHandlePic(30, 425, 128, 64, s_menuFrame.cornerLower); /* Bottom Left Corner */

        trap_R_SetColor(colorTable[CT_LTBROWN1]);
        DrawHandlePic(96, 438, 268, 18, uis.whiteShader);    /* Bottom front Line */

        /* Add foot note */
        if (menu->footNoteEnum) {
            DrawProportionalString(MENU_TITLE_X, menu_normal_text[menu->footNoteEnum],
                                   UI_RIGHT | UI_SMALLFONT, colorTable[CT_LTORANGE]);
            MenuBottomLineEnd_Graphics(menu_normal_text[menu->footNoteEnum], ColorTable[CT_LTBROWN1]);
        }
        trap_R_SetColor(NULL);

        /* Print version */
        if (space)
            DrawProportionalString(371, Q3_VERSION, UI_TINYFONT, ColorTable[CT_BLACK]);
    }

    void Atoms::MenuFrame(menuframework_s *menu) {
        qboolean space = qtrue;

        if (!ui::Atoms::MenuFrame::initialized) {
            MenuFrame_Cache();
        }

        if (!ingameFlag) {
            menu->fullscreen = qtrue;
        } else    /* In game menu */
        {
            menu->fullscreen = qfalse;
        }

        /* Graphic frame */
        FrameTop_Graphics(menu);    /* Top third */
        FrameBottom_Graphics();    /* Bottom two thirds */

        /* Add foot note */
        if (menu->footNoteEnum) {
            DrawProportionalString(MENU_TITLE_X, menu_normal_text[menu->footNoteEnum],
                                   UI_RIGHT | UI_SMALLFONT, colorTable[CT_LTORANGE]);
            MenuBottomLineEnd_Graphics(menu_normal_text[menu->footNoteEnum], CT_LTBROWN1);
        }

        /* Print version */
        if (space)
            DrawProportionalString(371, Q3_VERSION, UI_TINYFONT, colorTable[CT_BLACK]);
    }

    bool Atoms::MenuBottomLineEnd_Graphics(string_view string, Color color) {
        trap_R_SetColor(color.values);
        auto holdX = MENU_TITLE_X - (ProportionalStringWidth(string, UI_SMALLFONT));
        auto holdLength = (367 + 6) - holdX;
        DrawHandlePic(367, 438, holdLength, 18, uis.whiteShader); /* Bottom end line */

        /* TiM - stop the text leaving the box in some instances */
        return (holdLength < 0 ? -holdLength : holdLength) >= ProportionalStringWidth(Q3_VERSION, UI_TINYFONT);
    }

    void Atoms::FrameBottom_Graphics() {
        trap_R_SetColor(colorTable[CT_DKBROWN1]);
        DrawHandlePic(30, 147, 128, 64, s_menuFrame.cornerUpper2); /* Top corner */
        DrawHandlePic(50, 147, 99, 7, uis.whiteShader);

        trap_R_SetColor(colorTable[CT_LTBROWN1]); /* DKBROWN1 */
        DrawHandlePic(152, 147, 135, 7, uis.whiteShader);

        trap_R_SetColor(colorTable[CT_DKRED1]); /* DKBROWN1 */
        DrawHandlePic(290, 147, 12, 7, uis.whiteShader);

        trap_R_SetColor(colorTable[CT_LTBROWN1]);
        DrawHandlePic(305, 147, 60, 4, uis.whiteShader);

        trap_R_SetColor(colorTable[CT_LTGOLD1]); /* DKBROWN1 */
        DrawHandlePic(368, 147, 111, 7, uis.whiteShader);

        trap_R_SetColor(colorTable[CT_DKBROWN1]);
        DrawHandlePic(30, 173, 47, 27, uis.whiteShader);    /* Top left column (81453) */

        /* Start of piece (431108) */
        DrawHandlePic(30, 392, 47, 33, uis.whiteShader);    /* Bottom left column */
        DrawHandlePic(30, 425, 128, 64, s_menuFrame.cornerLower); /* Bottom Left Corner */

        trap_R_SetColor(colorTable[CT_LTBLUE1]); /* LTBROWN1]); */
        DrawHandlePic(96, 438, 268, 18, uis.whiteShader);    /* Bottom front Line */

        trap_R_SetColor(NULL);
    }

    void Atoms::FrameTop_Graphics(menuframework_s *menu) {
        trap_R_SetColor(colorTable[CT_DKPURPLE2]);
        DrawHandlePic(30, 24, 47, 54, uis.whiteShader);    /* Top left hand column */

        trap_R_SetColor(colorTable[CT_DKPURPLE3]);
        DrawHandlePic(30, 81, 47, 34, uis.whiteShader);    /* Middle left hand column */
        DrawHandlePic(30, 115, 128, 64, s_menuFrame.cornerUpper);    /* Corner */
        DrawHandlePic(100, 136, 49, 6.5,
                      uis.whiteShader);    /* Start of line across bottom of top third section */
        /*ABOVE HAS LINE BUG 111 - 38 - 7 */

        trap_R_SetColor(colorTable[CT_LTBROWN1]);
        DrawHandlePic(152, 136, 135, 7, uis.whiteShader);    /* 2nd line across bottom of top third section */

        trap_R_SetColor(colorTable[CT_LTPURPLE2]);
        DrawHandlePic(290, 136, 12, 7, uis.whiteShader);    /* 3rd line across bottom of top third section */

        trap_R_SetColor(colorTable[CT_LTBROWN1]); /* RPG-X ADDITION */
        DrawHandlePic(305, 139, 60, 4, uis.whiteShader);    /* 4th line across bottom of top third section */

        /*RPG-X REMOVE trap_R_SetColor( colorTable[CT_LTBROWN1]);*/
        DrawHandlePic(368, 136, 111, 7, uis.whiteShader); /* 5th line across bottom of top third section */

        if (menu->titleI) {
            DrawProportionalString(menu->titleX, menu_normal_text[menu->titleI],
                                   UI_RIGHT | UI_BIGFONT, colorTable[CT_LTORANGE]);
        }
    }

    void Atoms::MenuFrame_Cache() {
        s_menuFrame.cornerUpper = trap_R_RegisterShaderNoMip("menu/common/corner_ll_47_7.tga");
        s_menuFrame.cornerUpper2 = trap_R_RegisterShaderNoMip("menu/common/corner_ul_47_7.tga");
        s_menuFrame.cornerLower = trap_R_RegisterShaderNoMip("menu/common/corner_ll_47_18.tga");
    }

    void Atoms::PrecacheMenuGraphics(menugraphics_s *menuGraphics, int32_t maxI) {
        int32_t i;

        for (i = 0; i < maxI; ++i) {
            if (menuGraphics[i].type == MG_GRAPHIC) {
                menuGraphics[i].graphic = trap_R_RegisterShaderNoMip(menuGraphics[i].file);
            }
        }
    }

    void Atoms::PrintMenuGraphics(menugraphics_s *menuGraphics, int32_t maxI) {
        int32_t i;
        const char *text;

        /* Now that all the changes are made, print up the graphics */
        for (i = 0; i < maxI; ++i) {
            if (menuGraphics[i].type == MG_GRAPHIC) {
                trap_R_SetColor(colorTable[menuGraphics[i].color]);
                DrawHandlePic(menuGraphics[i].x,
                              menuGraphics[i].y,
                              menuGraphics[i].width,
                              menuGraphics[i].height,
                              menuGraphics[i].graphic);
                trap_R_SetColor(colorTable[CT_NONE]);
            } else if (menuGraphics[i].type == MG_STRING) {
                if (menuGraphics[i].file) {
                    text = menuGraphics[i].file;
                } else if (menuGraphics[i].normaltextEnum) {
                    text = menu_normal_text[menuGraphics[i].normaltextEnum];
                } else {
                    return;
                }

                DrawProportionalString(menuGraphics[i].x,
                                       text,
                                       menuGraphics[i].style,
                                       colorTable[menuGraphics[i].color]);
            } else if (menuGraphics[i].type == MG_NUMBER) {
                trap_R_SetColor(colorTable[menuGraphics[i].color]);
                DrawNumField({menuGraphics[i].x,
                              menuGraphics[i].max},
                             menuGraphics[i].target,
                             menuGraphics[i].width,
                             menuGraphics[i].height);
                trap_R_SetColor(colorTable[CT_NONE]);
            } else if (menuGraphics[i].type == MG_NONE) { ;    /* Don't print anything */
            }
        }
    }

    void
    Atoms::DrawNumField(const Point2dI &position, size_t width, int32_t value, size_t charWidth,
                        size_t) {
        if (width < 1) {
            return;
        }

        /* draw number string */
        if (width > 15)
            width = 15;

        switch (width) {
            case 1:
                value = value > 9 ? 9 : value;
                value = value < 0 ? 0 : value;
                break;
            case 2:
                value = value > 99 ? 99 : value;
                value = value < -9 ? -9 : value;
                break;
            case 3:
                value = value > 999 ? 999 : value;
                value = value < -99 ? -99 : value;
                break;
            case 4:
                value = value > 9999 ? 9999 : value;
                value = value < -999 ? -999 : value;
                break;
            default:
                break;
        }

        auto number_str = std::to_string(value);
        auto l = number_str.length();
        if (l > width) {
            l = width;
        }

        auto xWidth = (charWidth / 3);
        auto x = position.x;

        x += (xWidth) * (width - l);

        for (const auto &c : number_str) {
            auto frame = c - '0';
            DrawHandlePic(x, position.y, 16, 16, uis.smallNumbers[frame]);
            x += xWidth;

            l--;
            if (l == 0) {
                break;
            }
        }
    }

    bool Atoms::CursorInRect(const RectangleI &rect) {
        return !(uis.cursorx < rect.left ||
                 uis.cursory < rect.top ||
                 uis.cursorx > rect.right ||
                 uis.cursory > rect.bottom);
    }

    void Atoms::Refresh(int32_t realtime) {
        UI_LogFuncBegin();
        vec4_t color;

        uis.frametime = realtime - uis.realtime;
        uis.realtime = realtime;

        /*trap_Cvar_Set( "sys_lastactive", uis.realtime );*/

        if (!(trap_Key_GetCatcher() & KEYCATCH_UI)) {
            UI_LogFuncEnd();
            return;
        }

        UI_UpdateCvars();

        if (uis.activemenu) {
            uis.widescreen.state = WIDESCREEN_NONE;

            if (uis.activemenu->fullscreen) {
                /* draw the background */
                trap_R_SetColor(colorTable[CT_BLACK]);
                UI_DrawHandlePic(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, uis.whiteShader);
            } else if (!uis.activemenu->nobackground) {
                /* draw the background */
                color[0] = colorTable[CT_BLACK][0];
                color[1] = colorTable[CT_BLACK][1];
                color[2] = colorTable[CT_BLACK][1];
                color[3] = .75;

                trap_R_SetColor(color);
                UI_DrawHandlePic(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, uis.whiteShader);
            }

            uis.widescreen.state = WIDESCREEN_CENTER;

            if (uis.activemenu->draw)
                uis.activemenu->draw();
            else
                Menu_Draw(uis.activemenu);

            if (uis.firstdraw) {
                UI_MouseEvent(0, 0);
                uis.firstdraw = qfalse;
            }

        }

        /* draw cursor */
        trap_R_SetColor(NULL);
        if (uis.cursorDraw) {
            UI_DrawHandlePic(uis.cursorx, uis.cursory, 16, 16, uis.cursor);
        }

#ifndef NDEBUG
        if (uis.debug) {
            /* cursor coordinates */
            uis.widescreen.state = WIDESCREEN_LEFT;
            UI_DrawString(0, 0, va("(%d,%d)", uis.cursorx, uis.cursory), UI_LEFT | UI_SMALLFONT, colorRed, qtrue);
            uis.widescreen.state = WIDESCREEN_CENTER;
        }
#endif

        /*
         * delay playing the enter sound until after the
         * menu has been drawn, to avoid delay while
         * caching images
         */
        if (m_entersound) {
            trap_S_StartLocalSound(menu_in_sound, CHAN_LOCAL_SOUND);
            m_entersound = qfalse;
        }
        UI_LogFuncEnd();
    }

    void Atoms::DrawRect(float x, float y, float width, float height, const float *color) {
        UI_LogFuncBegin();
        trap_R_SetColor(color);

        UI_AdjustFrom640(&x, &y, &width, &height);

        trap_R_DrawStretchPic(x, y, width, 1, 0, 0, 0, 0, uis.whiteShader);
        trap_R_DrawStretchPic(x, y, 1, height, 0, 0, 0, 0, uis.whiteShader);
        trap_R_DrawStretchPic(x, y + height - 1, width, 1, 0, 0, 0, 0, uis.whiteShader);
        trap_R_DrawStretchPic(x + width - 1, y, 1, height, 0, 0, 0, 0, uis.whiteShader);

        trap_R_SetColor(NULL);
        UI_LogFuncEnd();
    }

    void Atoms::FillRect(float x, float y, float width, float height, const float *color) {
        UI_LogFuncBegin();
        trap_R_SetColor(color);

        UI_AdjustFrom640(&x, &y, &width, &height);
        trap_R_DrawStretchPic(x, y, width, height, 0, 0, 0, 0, uis.whiteShader);

        trap_R_SetColor(NULL);
        UI_LogFuncEnd();
    }

    void Atoms::DrawHandlePic(float x, float y, float w, float h, qhandle_t hShader) {
        UI_LogFuncBegin();
        float s0;
        float s1;
        float t0;
        float t1;

        /* TiM - security check */
        if (w == 0.0f || h == 0.0f) {
            UI_LogFuncEnd();
            return;
        }

        if (w < 0) {    /* flip about vertical */
            w = -w;
            s0 = 1;
            s1 = 0;
        } else {
            s0 = 0;
            s1 = 1;
        }

        if (h < 0) {    /* flip about horizontal */
            h = -h;
            t0 = 1;
            t1 = 0;
        } else {
            t0 = 0;
            t1 = 1;
        }

        UI_AdjustFrom640(&x, &y, &w, &h);
        trap_R_DrawStretchPic(x, y, w, h, s0, t0, s1, t1, hShader);
        UI_LogFuncEnd();
    }

    void Atoms::DrawHandleStretchPic(float x, float y, float w, float h, float s0, float t0, float s1, float t1,
                                     qhandle_t hShader) {
        UI_LogFuncBegin();
        UI_AdjustFrom640(&x, &y, &w, &h);
        trap_R_DrawStretchPic(x, y, w, h, s0, t0, s1, t1, hShader);
        UI_LogFuncEnd();
    }

    void Atoms::DrawNamedPic(float x, float y, float width, float height, const char *picname) {
        UI_LogFuncBegin();
        qhandle_t hShader;

        hShader = trap_R_RegisterShaderNoMip(picname);
        UI_AdjustFrom640(&x, &y, &width, &height);
        trap_R_DrawStretchPic(x, y, width, height, 0, 0, 1, 1, hShader);
        UI_LogFuncEnd();
    }

    void Atoms::AdjustFrom640(float *x, float *y, float *w, float *h) {
        UI_LogFuncBegin();
        /* expect valid pointers */
        /* *x = *x * uis.scale + uis.bias;*/
        *x *= uis.scalex;
        *y *= uis.scaley;
        *w *= uis.scalex;
        *h *= uis.scaley;

        /* handle widescreen projections */
        if (IsWidescreen()) {
            *x *= uis.widescreen.ratio;
            *w *= uis.widescreen.ratio;

            /* center the elements into the middle of the screen */
            if (uis.widescreen.state == WIDESCREEN_CENTER)
                *x += uis.widescreen.bias;
        }
        UI_LogFuncEnd();
    }

    void Atoms::Init() {
        UI_LogFuncBegin();

        UI_Logger(LL_ALWAYS, "This is RPG-X version %s compiled by %s on %s.\n", RPGX_VERSION, RPGX_COMPILEDBY,
                  RPGX_COMPILEDATE);

        memset(&uis, 0, sizeof(uis));

        init_tonextint(qfalse);

        UI_RegisterCvars();

        UI_LoadMenuText();

        UI_LoadButtonText();

        LoadFonts();

        BG_LoadItemNames();

        UI_InitGameinfo();

        /* Initialize the ranks data */
        UI_InitRanksData(UI_Cvar_VariableString("ui_currentRankSet"));
        GetRankSets();

        UI_InitClassData(UI_Cvar_VariableString("ui_currentClassSet"));
        GetClassSets();

        /* cache redundant calulations */
        trap_GetGlconfig(&uis.glconfig);

        /* for 640x480 virtualized screen */
        uis.scaley = uis.glconfig.vidHeight * (1.0 / 480.0);
        uis.scalex = uis.glconfig.vidWidth * (1.0 / 640.0);
        /*	uis.scale = uis.glconfig.vidHeight * (1.0/480.0);
            if ( uis.glconfig.vidWidth * 480 > uis.glconfig.vidHeight * 640 ) {
            // wide screen
            uis.bias = 0.5 * ( uis.glconfig.vidWidth - ( uis.glconfig.vidHeight * (640.0/480.0) ) );
            }
            else {
            // no wide screen
            uis.bias = 0;
            }
            */
        /* TiM - handle wide screens */
        if (uis.glconfig.vidWidth * 480 > uis.glconfig.vidHeight * 640) {
            uis.widescreen.ratio = 640.0f * uis.scaley * (1.0f / uis.glconfig.vidWidth);
            uis.widescreen.bias = 0.5 * (uis.glconfig.vidWidth - (uis.glconfig.vidHeight * (640.0 / 480.0)));
        } else {
            uis.widescreen.ratio = 0;
            uis.widescreen.bias = 0;
        }

        /* initialize the menu system */
        Menu_Cache();

        uis.activemenu = NULL;
        uis.menusp = 0;
        trap_Cvar_Create("ui_initialsetup", "0", CVAR_ARCHIVE);

        /* TiM - initiate the client side portion of the security code */
        UI_SecurityCodeSetup();

        /* trap_Cvar_Create ("rpg_playIntro", "1", CVAR_ARCHIVE ); *//*RPG-X | Phenix | 25/02/2005 */
        UI_LogFuncEnd();
    }

    int32_t Atoms::PopulateClassSetArray(char **classSets) {
        int32_t i;

        for (i = 0; i < MAX_CLASSSETS; i++) {
            if (!uis.classList[i][0])
                break;

            classSets[i] = uis.classList[i];
        }

        return i;
    }

    int32_t Atoms::GetClassSets() {
        int32_t numFiles, i;
        char fileBuffer[2048];
        char *filePtr;
        /*char		filePath[128];*/
        int32_t fileLen;

        numFiles = trap_FS_GetFileList("ext_data/classes", ".classes", fileBuffer, sizeof(fileBuffer));
        /*UI_Logger( LL_DEBUG, "%s\n", filePtr);*/

        if (numFiles == 1) {
            return 1;
        }

        memset(&uis.classList, 0, sizeof(uis.classList));

        if (numFiles > MAX_CLASSSETS)
            numFiles = MAX_CLASSSETS;

        filePtr = fileBuffer;
        i = 0;
        while (i < numFiles) {
            if (!filePtr)
                break;

            fileLen = strlen(filePtr);

            /* Remove the extension */
            if (fileLen > 8 && !Q_stricmp(filePtr + fileLen - 8, ".classes")) {
                filePtr[fileLen - 8] = '\0';
            }

            /*UI_Logger( LL_DEBUG, "%s\n", filePtr ); */

            Q_strncpyz(uis.classList[i], filePtr, sizeof(uis.classList[i]));

            filePtr += fileLen + 1;
            i++;
        }

        trap_Print(va("%i class sets detected\n", i));

        return i;
    }

    int32_t Atoms::PopulateClassArray(char **classes) {
        int32_t i;

        for (i = 0; i < MAX_CLASSES; i++) {
            if (!uis.classData[i].classNameFull[0])
                break;

            classes[i] = uis.classData[i].classNameFull;
        }

        classes[i] = "Other";
        /*classes[i+1] = 0;*/

        return i;
    }

    int32_t Atoms::GetRankSets() {
        int32_t numFiles, i;
        char fileBuffer[2048];
        char *filePtr;
        /*char		filePath[128];*/
        int32_t fileLen;

        numFiles = trap_FS_GetFileList("ext_data/ranksets", ".ranks", fileBuffer, sizeof(fileBuffer));
        /*UI_Logger( LL_DEBUG, "%s\n", filePtr);*/

        if (numFiles == 1)
            return 1;

        memset(&uis.rankSet.rankSetNames, 0, sizeof(uis.rankSet.rankSetNames));

        if (numFiles > MAX_RANKSETS)
            numFiles = MAX_RANKSETS;

        filePtr = fileBuffer;
        i = 0;
        while (i < numFiles) {
            if (!filePtr)
                break;

            fileLen = strlen(filePtr);

            /*Remove the extension*/
            if (fileLen > 6 && !Q_stricmp(filePtr + fileLen - 6, ".ranks")) {
                filePtr[fileLen - 6] = '\0';
            }

            /*UI_Logger( LL_DEBUG, "%s\n", filePtr );*/

            Q_strncpyz(uis.rankSet.rankSetNames[i], filePtr, sizeof(uis.rankSet.rankSetNames[i]));

            filePtr += fileLen + 1;
            i++;
        }

        trap_Print(va("%i ranksets detected\n", i));
        return i;
    }

    int32_t Atoms::PopulateRankSetArray(char **rankSets) {
        int32_t i;

        for (i = 0; i < MAX_RANKSETS; i++) {
            if (!uis.rankSet.rankSetNames[i][0])
                break;

            rankSets[i] = uis.rankSet.rankSetNames[i];
        }

        rankSets[i] = 0;

        return i;
    }

    void Atoms::InitRanksData(char *ranksName) {
        char filePath[MAX_QPATH];

        if (!Q_stricmp(uis.rankSet.rankSetName, ranksName))
            goto refreshRank;

        /* Init the transfer space */
        memset(&uis.rankSet.rankNames, 0, sizeof(uis.rankSet.rankNames));

        /* Create the file route */
        Com_sprintf(filePath, sizeof(filePath), "ext_data/ranksets/%s.ranks", ranksName);

        /* attempt to parse */
        if (!BG_ParseRankNames(filePath, uis.rankSet.rankNames, sizeof(rankNames_t) * MAX_RANKS)) {

            /* Rank attempt failed.  Try loading the defaults.  If we end up with no loaded ranks... many menu elements will start crapping. bad */
            if (!BG_ParseRankNames(va("ext_data/ranksets/%s.ranks", RANKSET_DEFAULT), uis.rankSet.rankNames,
                                   sizeof(rankNames_t) * MAX_RANKS))
                trap_Error(va("UI_InitRanksData: Was unable to load default rankset: %s", RANKSET_DEFAULT));
        } else {
            int32_t i;
            /*
            * nvm, all loaded good :)
            * set the current rank CVAR so it'll use this rankset next time they start the game
            */
            trap_Cvar_Set("ui_currentRankSet", ranksName);

            /* eh... to be on the safe side, save the name of the ranks in a local string */
            Q_strncpyz(uis.rankSet.rankSetName, ranksName, sizeof(uis.rankSet.rankSetName));

            refreshRank:
            /* using our current cvar'd rank, do a compare.  if we find a match, set our player to that rank in the menu */
            for (i = 0, uis.currentRank = 0; i < MAX_RANKS; i++) {
                // TODO: replace UI_Cvar_VariableString by modern system call
                if (!Q_stricmp(uis.rankSet.rankNames[i].consoleName, UI_Cvar_VariableString("ui_playerRank"))) {
                    uis.currentRank = i;
                    break;
                }
            }
        }
    }

    int32_t Atoms::PopulateRanksArray(char **ranks) {
        int32_t i;
        rankNames_t *rank;

        for (i = 0; i < MAX_RANKS; i++) {
            rank = &uis.rankSet.rankNames[i];

            if (!rank->formalName[0])
                break;

            ranks[i] = rank->formalName;
        }

        ranks[i] = "Other";
        /*ranks[i+1] = 0; *//*IMPORTANT: Spin controls need these or else values bleed into different controls*/

        return i;
    }

    int32_t Atoms::InitClassData(char *fileName) {
        char buffer[32000];
        fileHandle_t f;
        int32_t fileLen;
        char *textPtr;
        char filePath[MAX_QPATH];
        int32_t classIndex = 0;
        char *token;

        /* TiM - check if we've already loaded this file */
        if (!Q_stricmp(uis.classSetName, fileName)) {
            return qtrue;
        }

        /* TiM - build the file name */
        Com_sprintf(filePath, sizeof(filePath), "ext_data/classes/%s.classes", fileName);

        fileLen = trap_FS_FOpenFile(filePath, &f, FS_READ);

        if (!fileLen) {
            UI_Logger(LL_ERROR, "File not found: %s\n", fileName);
            return qfalse;
        }

        /* init file buffer */
        memset(buffer, 0, sizeof(buffer));

        trap_FS_Read(buffer, fileLen, f);

        if (!buffer[0]) {
            UI_Logger(LL_ERROR, "File could not be read: %s\n", fileName);
            return qfalse;
        }

        trap_FS_FCloseFile(f);

        /* Re-init class list */
        memset(uis.classData, 0, sizeof(uis.classData));

        textPtr = buffer;

        COM_BeginParseSession();

        token = COM_Parse(&textPtr);

        if (!token[0]) {
            UI_Logger(LL_ERROR, "File was loaded, but no data could be read: %s\n", fileName);
            return qfalse;
        }

        if (Q_stricmpn(token, "{", 1)) {
            UI_Logger(LL_ERROR, "No opening brace { found in: %s\n", fileName);
            return qfalse;
        }

        while (1) {
            if (classIndex >= MAX_CLASSES)
                break;

            if (!Q_stricmpn(token, "{", 1)) {
                while (1) {

                    /* formal Name */
                    if (!Q_stricmpn(token, "formalName", 10)) {
                        if (COM_ParseString(&textPtr, &token)) {
                            UI_Logger(LL_ERROR, "Error parsing formalName parameter in file: %s.\n", fileName);
                            continue;
                        }

                        Q_strncpyz(uis.classData[classIndex].classNameFull, token,
                                   sizeof(uis.classData[classIndex].classNameFull));
                        continue;
                    }

                    /* console Name */
                    if (!Q_stricmpn(token, "consoleName", 11)) {
                        if (COM_ParseString(&textPtr, &token)) {
                            UI_Logger(LL_ERROR, "Error parsing consoleName parameter in file: %s.\n", fileName);
                            continue;
                        }

                        Q_strncpyz(uis.classData[classIndex].classNameConsole, token,
                                   sizeof(uis.classData[classIndex].classNameConsole));
                        continue;
                    }

                    /* TiM : Disregard noShow Classes */
                    if (!Q_stricmp(token, "noShow")) {
                        token = COM_Parse(&textPtr);

                        if (atoi(token) >= 1) {
                            memset(uis.classData[classIndex].classNameConsole, 0,
                                   sizeof(uis.classData[classIndex].classNameConsole));
                            memset(uis.classData[classIndex].classNameFull, 0,
                                   sizeof(uis.classData[classIndex].classNameFull));
                            break;
                        }
                    }

                    token = COM_Parse(&textPtr);
                    if (!token[0])
                        break;

                    /* skip any more braces. They're obviously color vals */
                    if (!Q_stricmpn(token, "{", 1)) {
                        SkipBracedSection(&textPtr);
                        continue;
                    }

                    if (!Q_strncmp(token, "}", 1)) {
                        classIndex++;
                        break;
                    }
                }
            }

            token = COM_Parse(&textPtr);
            if (!token[0])
                break;
        }

        Q_strncpyz(uis.classSetName, fileName, sizeof(uis.classSetName));

        return qtrue;
    }

    void Atoms::LoadFonts() {
        char buffer[FONT_BUFF_LENGTH];
        int32_t len;
        fileHandle_t f;
        char *holdBuf;

        len = trap_FS_FOpenFile("ext_data/fonts.dat", &f, FS_READ);

        if (!f) {
            trap_Print(va(S_COLOR_RED "UI_LoadFonts : FONTS.DAT file not found!\n"));
            return;
        }

        if (len > FONT_BUFF_LENGTH) {
            trap_Print(va(S_COLOR_RED "UI_LoadFonts : FONTS.DAT file bigger than %d!\n", FONT_BUFF_LENGTH));
            return;
        }

        /* initialise the data area */
        memset(buffer, 0, sizeof(buffer));

        trap_FS_Read(buffer, len, f);

        trap_FS_FCloseFile(f);

        COM_BeginParseSession();

        holdBuf = (char *) buffer;
        holdBuf = ParseFontParms(holdBuf, propMapTiny);
        holdBuf = ParseFontParms(holdBuf, propMap);
        holdBuf = ParseFontParms(holdBuf, propMapBig);
    }

    bool Atoms::IsWidescreen() {
        return ui_handleWidescreen.integer && uis.widescreen.ratio && uis.widescreen.state != WIDESCREEN_NONE;
    }

    constexpr float Atoms::clampCvar(float min, float max, float value) {
        if (value < min) {
            return min;
        }

        if (value > max) {
            return max;
        }

        return value;
    }

    void Atoms::pushMenu(menuframework_s *menu) {
        auto i = 0;

        for (; i < uis.menusp; i++) {
            if (uis.stack[i] == menu) {
                uis.menusp = i;
                break;
            }
        }

        if (i == uis.menusp) {
            if (uis.menusp >= MAX_MENUDEPTH) {
                trap_Error("PushMenu: menu stack overflow");
            }

            uis.stack[uis.menusp++] = menu;
        }

        menu->cursor = 0;
        menu->cursor_prev = 0;

        m_entersound = qtrue;

        trap_Key_SetCatcher(KEYCATCH_UI);

        for (i = 0; i < menu->nitems; i++) {
            auto item = static_cast<menucommon_s *>(menu->items[i]);
            if ((item->flags & (QMF_GRAYED | QMF_MOUSEONLY | QMF_INACTIVE)) != QMF_GRAYED | QMF_MOUSEONLY |
                QMF_INACTIVE) {
                menu->cursor_prev = -1;
                Menu_SetCursor(menu, i);
                break;
            }
        }

        uis.firstdraw = qtrue;
    }

    void Atoms::PopMenu() {
        trap_S_StartLocalSound(menu_out_sound, CHAN_LOCAL_SOUND);

        uis.menusp--;

        if (uis.menusp < 0)
            trap_Error("PopMenu: menu stack underflow");

        if (uis.menusp) {
            uis.activemenu = uis.stack[uis.menusp - 1];
            uis.firstdraw = qtrue;
        } else {
            ForceMenuOff();
        }
    }

    void Atoms::ForceMenuOff() {
        uis.menusp = 0;
        uis.activemenu = nullptr;

        trap_Key_SetCatcher(trap_Key_GetCatcher() & ~KEYCATCH_UI);
        trap_Key_ClearStates();
        trap_Cvar_Set("cl_paused", "0");
    }

    void Atoms::LerpColor(vec_t *a, vec_t *b, vec_t *c, float t) {
        int32_t i;

        /* lerp and clamp each component */
        for (i = 0; i < 4; i++) {
            c[i] = a[i] + t * (b[i] - a[i]);
            if (c[i] < 0)
                c[i] = 0;
            else if (c[i] > 1.0)
                c[i] = 1.0;
        }
    }

    void Atoms::DrawBannerString2(int32_t x, int32_t y, const char *str, vec_t *color) {
        const char *s;
        char ch;
        float ax;
        float ay;
        float aw;
        float ah;
        float frow;
        float fcol;
        float fwidth;
        float fheight;

        /* draw the colored text */
        trap_R_SetColor(color);

        /*ax = x * uis.scale + uis.bias;*/
        ax = x * uis.scalex;
        ay = y * uis.scaley;

        s = str;
        while (*s) {
            ch = *s & 255;
            if (ch == ' ') {
                ax += ((float) PROPB_SPACE_WIDTH + (float) PROPB_GAP_WIDTH) * uis.scalex;
            } else if (ch >= 'A' && ch <= 'Z') {
                ch -= 'A';
                fcol = (float) propMapB[(int32_t) ch][0] / 256.0f; /*256.0f*/
                frow = (float) propMapB[(int32_t) ch][1] / 256.0f;
                fwidth = (float) propMapB[(int32_t) ch][2] / 256.0f;
                fheight = (float) PROPB_HEIGHT / 256.0f;
                aw = (float) propMapB[(int32_t) ch][2] * uis.scalex;
                ah = (float) PROPB_HEIGHT * uis.scaley;
                trap_R_DrawStretchPic(ax, ay, aw, ah, fcol, frow, fcol + fwidth, frow + fheight, uis.charsetPropB);
                ax += (aw + (float) PROPB_GAP_WIDTH * uis.scalex);
            }
            s++;
        }

        trap_R_SetColor(nullptr);
    }

    void Atoms::DrawBannerString(int32_t x, int32_t y, const char *str, int32_t style, vec_t *color) {
        const char *s;
        int32_t ch;
        int32_t width;
        vec4_t drawcolor;

        /* find the width of the drawn text */
        s = str;
        width = 0;
        while (*s) {
            ch = *s;
            if (ch == ' ') {
                width += PROPB_SPACE_WIDTH;
            } else if (ch >= 'A' && ch <= 'Z') {
                width += propMapB[ch - 'A'][2] + PROPB_GAP_WIDTH;
            }
            s++;
        }
        width -= PROPB_GAP_WIDTH;

        switch (style & UI_FORMATMASK) {
            case UI_CENTER:
                x -= width / 2;
                break;

            case UI_RIGHT:
                x -= width;
                break;

            case UI_LEFT:
            default:
                break;
        }

        if (style & UI_DROPSHADOW) {
            drawcolor[0] = drawcolor[1] = drawcolor[2] = 0;
            drawcolor[3] = color[3];
            DrawBannerString2(x + 2, y + 2, str, drawcolor);
        }

        DrawBannerString2(x, y, str, color);
    }

    size_t Atoms::ProportionalStringWidth(string_view str, int32_t style) {
        if (style == UI_TINYFONT) {
            auto width = std::accumulate(str.begin(), str.end(), 0U, [](auto sum, auto ch) {
                auto charWidth = propMapTiny[ch][2];
                if (charWidth > -1) {
                    return sum + charWidth + PROP_GAP_TINY_WIDTH;
                }

                return sum;
            });
            return width - PROP_GAP_TINY_WIDTH;
        } else if (style == UI_BIGFONT) {
            auto width = std::accumulate(str.begin(), str.end(), 0U, [](auto sum, auto ch) {
                auto charWidth = propMapBig[ch][2];
                if (charWidth > -1) {
                    return sum + charWidth + PROP_GAP_BIG_WIDTH;
                }

                return sum;
            });
            return width - PROP_GAP_BIG_WIDTH;
        }

        auto width = std::accumulate(str.begin(), str.end(), 0U, [](auto sum, auto ch) {
            auto charWidth = propMap[ch][2];
            if (charWidth > -1) {
                return sum + charWidth + PROPB_GAP_WIDTH;
            }

            return sum;
        });
        return width - PROP_GAP_WIDTH;
    }

    void Atoms::DrawProportionalString2(common::Point2dI pos, std::string_view str, common::Color color, int32_t style,
                                        qhandle_t charset) {
        float ax;
        float ay, holdY;
        float aw;
        float ah;
        float frow;
        float fcol;
        float fwidth;
        float fheight;
        float sizeScale;
        int32_t colorI;
        int32_t special;

        /* draw the colored text */
        trap_R_SetColor(color.values);

        /*ax = x * uis.scale + uis.bias;*/
        ax = pos.x * uis.scalex;
        ay = pos.y * uis.scaley;
        holdY = ay;

        /* TiM - adjust for widescreen monitors */
        if (IsWidescreen()) {
            ax *= uis.widescreen.ratio;

            /* center the elements into the middle of the screen */
            if (uis.widescreen.state == WIDESCREEN_CENTER)
                ax += uis.widescreen.bias;
        }
        /*else
            UI_Logger( LL_DEBUG, "CVAR: %i, Ratio: %f, State: %i\n", ui_handleWidescreen.integer, uis.widescreen.ratio, uis.widescreen.state );*/

        sizeScale = ProportionalSizeScale(style);

        if (style & UI_TINYFONT) {
            for(auto it = str.begin(); it != str.end(); ++it) {
                if(Q_IsColorString(it) && ((style & UI_SHOWCOLOR) == UI_SHOWCOLOR)) {
                    colorI = ColorIndex(*(it + 1));
                    trap_R_SetColor(g_color_table[colorI]);
                    it += 2;
                    continue;
                }

               auto ch = *it & 255;
                if(ch == ' ') {
                    aw = static_cast<float>(PROP_SPACE_TINY_WIDTH);
                } else if(propMap[ch][2] > -1) {
                    special = specialTinyPropChars[ch][0];
                    ay = holdY + (specialTinyPropChars[ch][1] * uis.scaley);
                    fcol = static_cast<float>(propMapTiny[ch][0] / 256.0f);
                    frow = static_cast<float>(propMapTiny[ch][1] / 256.0f);
                    fwidth = static_cast<float>(propMapTiny[ch][2] / 256.0f);
                    fheight = static_cast<float>(PROP_TINY_HEIGHT + special) / 256.0f;
                    aw = static_cast<float>(propMapTiny[ch][2]) * uis.scalex * sizeScale;
                    ah = static_cast<float>(PROP_TINY_HEIGHT + special) * uis.scaley * sizeScale;

                    if(IsWidescreen()) {
                        aw *= uis.widescreen.ratio;
                    }

                    trap_R_DrawStretchPic(ax, ay, aw, ah, fcol, frow, fcol + fwidth, frow + fheight, charset);
                } else {
                    aw = 0;
                }

                ax += aw + static_cast<float>(PROP_GAP_TINY_WIDTH) * uis.scalex * sizeScale;

                if(IsWidescreen()) {
                    ax -= static_cast<float>(PROP_GAP_TINY_WIDTH * uis.scalex * sizeScale) * (1.0f - uis.widescreen.ratio);
                }
            }
        } else if (style & UI_BIGFONT) {
            s = str;
            while (*s) {
                /* Is this a color???? */
                if (Q_IsColorString(s) && !(style & UI_SHOWCOLOR)) {
                    colorI = ColorIndex(*(s + 1));
                    trap_R_SetColor(g_color_table[colorI]);
                    s += 2;
                    continue;
                }

                ch = *s & 255;
                if (ch == ' ') {
                    aw = (float) PROP_SPACE_BIG_WIDTH * uis.scalex;
                } else if (propMap[ch][2] != -1) {
                    /* Because some foreign characters were a little different */
                    special = specialBigPropChars[ch][0];
                    ay = holdY + (specialBigPropChars[ch][1] * uis.scaley);

                    fcol = (float) propMapBig[ch][0] / 256.0f; /* 256.0f */
                    frow = (float) propMapBig[ch][1] / 256.0f;
                    fwidth = (float) propMapBig[ch][2] / 256.0f;
                    fheight = (float) (PROP_BIG_HEIGHT + special) / 256.0f;
                    aw = (float) propMapBig[ch][2] * uis.scalex * sizeScale;
                    ah = (float) (PROP_BIG_HEIGHT + special) * uis.scaley * sizeScale;

                    /* TiM - adjust for widescreen */
                    if (IsWidescreen()) {
                        aw *= uis.widescreen.ratio;
                    }

                    trap_R_DrawStretchPic(ax, ay, aw, ah, fcol, frow, fcol + fwidth, frow + fheight, charset);
                } else {
                    aw = 0;
                }


                ax += (aw + (float) PROP_GAP_BIG_WIDTH * uis.scalex * sizeScale);

                /* again adjust for widescreen */
                if (IsWidescreen())
                    ax -= ((float) PROP_GAP_BIG_WIDTH * uis.scalex * sizeScale) * (1.0f - uis.widescreen.ratio);

                s++;
            }
        } else {
            s = str;
            while (*s) {
                /* Is this a color???? */
                if (Q_IsColorString(s) && !(style & UI_SHOWCOLOR)) {
                    colorI = ColorIndex(*(s + 1));
                    trap_R_SetColor(g_color_table[colorI]);
                    s += 2;
                    continue;
                }

                ch = *s & 255;
                if (ch == ' ') {
                    aw = (float) PROP_SPACE_WIDTH * uis.scalex * sizeScale;
                } else if (propMap[ch][2] != -1) {
                    /* Because some foreign characters were a little different */
                    special = specialPropChars[ch][0];
                    ay = holdY + (specialPropChars[ch][1] * uis.scaley);

                    fcol = (float) propMap[ch][0] / 256.0f;
                    frow = (float) propMap[ch][1] / 256.0f;
                    fwidth = (float) propMap[ch][2] / 256.0f;
                    fheight = (float) (PROP_HEIGHT + special) / 256.0f;
                    aw = (float) propMap[ch][2] * uis.scalex * sizeScale;
                    ah = (float) (PROP_HEIGHT + special) * uis.scaley * sizeScale;

                    /* TiM - adjust for widescreen */
                    if (ui_handleWidescreen.integer && uis.widescreen.ratio) {
                        aw *= uis.widescreen.ratio;
                    }

                    trap_R_DrawStretchPic(ax, ay, aw, ah, fcol, frow, fcol + fwidth, frow + fheight, charset);
                } else {
                    aw = 0;
                }


                ax += (aw + (float) PROP_GAP_WIDTH * uis.scalex * sizeScale);

                /* again adjust for widescreen */
                if (IsWidescreen())
                    ax -= ((float) PROP_GAP_WIDTH * uis.scalex * sizeScale) * (1.0f - uis.widescreen.ratio);

                s++;
            }
        }

        trap_R_SetColor(nullptr);
    }

    float Atoms::ProportionalSizeScale(int32_t style) {
        if (style & UI_SMALLFONT) {
            return PROP_SMALL_SIZE_SCALE;
        }

        return 1.00;
    }

    void Atoms::DrawProportionalString(common::Point2dI pos, std::string_view str, int32_t style, common::Color color) {
        Color drawcolor
        size_t width;
        float sizeScale;
        int32_t charstyle = 0;

        if ((style & UI_BLINK) && ((uis.realtime / BLINK_DIVISOR) & 1)) {
            return;
        }

        /* Get char style */
        if (style & UI_TINYFONT) {
            charstyle = UI_TINYFONT;
        } else if (style & UI_SMALLFONT) {
            charstyle = UI_SMALLFONT;
        } else if (style & UI_BIGFONT) {
            charstyle = UI_BIGFONT;
        } else if (style & UI_GIANTFONT) {
            charstyle = UI_GIANTFONT;
        } else    /* Just in case */
        {
            charstyle = UI_SMALLFONT;
        }

        if (style & UI_SHOWCOLOR)
            charstyle |= UI_SHOWCOLOR;

        sizeScale = ProportionalSizeScale(style);

        switch (style & UI_FORMATMASK) {
            case UI_CENTER:
                width = static_cast<size_t>(ProportionalStringWidth(str, charstyle) * sizeScale);
                pos.x -= width / 2;
                break;

            case UI_RIGHT:
                width = static_cast<size_t>(ProportionalStringWidth(str, charstyle) * sizeScale);
                pos.x -= width;
                break;

            case UI_LEFT:
            default:
                break;
        }

        if (style & UI_DROPSHADOW) {
            drawcolor.r = drawcolor.g = drawcolor.b = 0;
            drawcolor.a = color.a;
            DrawProportionalString2({pos.x + 2, pos.y + 2}, drawcolor, sizeScale, uis.charsetProp, 0);
        }

        if (style & UI_INVERSE) {
            drawcolor.r = color.r * 0.7f;
            drawcolor.g = color.g * 0.7f;
            drawcolor.b = color.b * 0.7f;
            drawcolor.a = color.a;
            DrawProportionalString2(pos, drawcolor, sizeScale, uis.charsetProp, 0);
            return;
        }

        if (style & UI_PULSE) {
            drawcolor.r = color.a * 0.7f;
            drawcolor.g = color.g * 0.7f;
            drawcolor.b = color.b * 0.7f;
            drawcolor.a = color.a;
            DrawProportionalString2(pos, color, sizeScale, uis.charsetProp, 0);

            drawcolor.r = color.r;
            drawcolor.g = color.g;
            drawcolor.b = color.b;
            drawcolor.a = static_cast<float>(0.5f + 0.5f * std::sin(uis.realtime / PULSE_DIVISOR));
            DrawProportionalString2(pos, drawcolor, sizeScale, uis.charsetProp, 0);
            return;
        }


        if (style & UI_TINYFONT) {
            DrawProportionalString2(pos, color, charstyle, uis.charsetPropTiny, 0);
        } else if (style & UI_BIGFONT) {
            DrawProportionalString2(pos, color, charstyle, uis.charsetPropBig, 0);
        } else {
            DrawProportionalString2(pos, color, charstyle, uis.charsetProp, 0);
        }
    }

    void Atoms::DrawString2(int32_t x, int32_t y, const char *str, vec_t *color, int32_t charw, int32_t charh) {
        const char *s;
        char ch;
        int32_t forceColor = qfalse; /* APSFIXME; */
        vec4_t tempcolor;
        float ax;
        float ay;
        float aw;
        float ah;
        float frow;
        float fcol;

        if (y < -charh) {
            /* offscreen */
            return;
        }

        /* draw the colored text */
        trap_R_SetColor(color);

        /* ax = x * uis.scale + uis.bias; */
        ax = x * uis.scalex;
        ay = y * uis.scaley;
        aw = charw * uis.scalex;
        ah = charh * uis.scaley;

        if (IsWidescreen()) {
            ax *= uis.widescreen.ratio;
            aw *= uis.widescreen.ratio;

            if (uis.widescreen.state == WIDESCREEN_CENTER)
                ax += uis.widescreen.bias;
        }

        s = str;
        while (*s) {
            if (!showColorChars) {
                if (Q_IsColorString(s)) {
                    if (!forceColor) {
                        memcpy(tempcolor, g_color_table[ColorIndex(s[1])], sizeof(tempcolor));
                        tempcolor[3] = color[3];
                        trap_R_SetColor(tempcolor);
                    }
                    s += 2;
                    continue;
                }
            }

            ch = *s & 255;

            if (ch != ' ') {
                /*frow = (ch>>4)*0.0625;
                fcol = (ch&15)*0.0625;
                trap_R_DrawStretchPic( ax, ay, aw, ah, fcol, frow, fcol + 0.0625, frow + 0.0625, uis.charset );*/

                frow = (ch >> 4) * 0.0625;
                fcol = (ch & 15) * 0.0625;

                trap_R_DrawStretchPic(ax, ay, aw, ah, fcol, frow, fcol + 0.03125, frow + 0.0625, uis.charset);

            }

            ax += aw;
            s++;
        }

        trap_R_SetColor(NULL);
    }

    void Atoms::DrawString(int32_t x, int32_t y, const char *str, int32_t style, vec_t *color, qboolean highRes) {
        int32_t len;
        int32_t charw;
        int32_t charh;
        vec4_t newcolor;
        vec4_t lowlight;
        float *drawcolor;
        vec4_t dropcolor;

        if (!str) {
            return;
        }

        if ((style & UI_BLINK) && ((uis.realtime / BLINK_DIVISOR) & 1)) { ;
            return;
        }

        if (style & UI_TINYFONT) {
            charw = TINYCHAR_WIDTH;
            charh = TINYCHAR_HEIGHT;
        } else if (style & UI_BIGFONT) {
            charw = BIGCHAR_WIDTH;
            charh = BIGCHAR_HEIGHT;
        } else if (style & UI_GIANTFONT) {
            charw = GIANTCHAR_WIDTH;
            charh = GIANTCHAR_HEIGHT;
        } else {
            charw = SMALLCHAR_WIDTH;
            charh = SMALLCHAR_HEIGHT;
        }

        if (style & UI_PULSE) {
            lowlight[0] = 0.8 * color[0];
            lowlight[1] = 0.8 * color[1];
            lowlight[2] = 0.8 * color[2];
            lowlight[3] = 0.8 * color[3];
            LerpColor(color, lowlight, newcolor, 0.5 + 0.5 * sin(uis.realtime / PULSE_DIVISOR));
            drawcolor = newcolor;
        } else
            drawcolor = color;

        switch (style & UI_FORMATMASK) {
            case UI_CENTER:
                /* center justify at x */
                len = strlen(str);
                x = x - len * charw / 2;
                break;

            case UI_RIGHT:
                /* right justify at x */
                len = strlen(str);
                x = x - len * charw;
                break;

            default:
                /* left justify at x */
                break;
        }

        if (style & UI_SHOWCOLOR) {
            showColorChars = qtrue;
        } else {
            showColorChars = qfalse;
        }

        if (style & UI_DROPSHADOW) {
            dropcolor[0] = dropcolor[1] = dropcolor[2] = 0;
            dropcolor[3] = drawcolor[3];

            if (highRes)
                DrawProportionalString(x + 2, str, style, dropcolor);
            else
                DrawString2(x + 2, y + 2, str, dropcolor, charw, charh);
        }

        /* TiM - Using a different char set now... */
        if (!highRes) /* keep the low res version for specific instances */
            DrawString2(x, y, str, drawcolor, charw, charh);
        else
            DrawProportionalString(x, str, style, drawcolor);
    }

    void Atoms::DrawChar(int32_t x, int32_t y, int32_t ch, int32_t style, vec_t *color) {
        char buff[2];

        buff[0] = ch;
        buff[1] = '\0';

        DrawString(x, y, buff, style, color, qfalse);
    }

    qboolean Atoms::IsFullscreen() {
        if (uis.activemenu && (trap_Key_GetCatcher() & KEYCATCH_UI)) {
            return uis.activemenu->fullscreen;
        }

        return qfalse;
    }

    void Atoms::NeedCDAction(qboolean result) {
        if (!result) {
            /*trap_Cvar_Set ("rpg_playIntro", "1");*/
            trap_Cmd_ExecuteText(EXEC_APPEND, "quit\n");
        }
    }

    void Atoms::SetActiveMenu(uiMenuCommand_t menu) {
        /*
         * this should be the ONLY way the menu system is brought up, except for ConsoleCommand below
         * enusure minumum menu data is cached
         */
        Menu_Cache();

        switch (menu) {
            case UIMENU_NONE:
                ForceMenuOff();
                return;
            case UIMENU_MAIN:
                UI_MainMenu();
                return;
            case UIMENU_NEED_CD:
                UI_ConfirmMenu(menu_normal_text[MNT_INSERTCD], 0, NeedCDAction);
                return;
            case UIMENU_INGAME:
                trap_Cvar_Set("cl_paused", "1");
                UI_InGameMenu();
                return;
        }
    }

    void Atoms::KeyEvent(int32_t key) {
        sfxHandle_t s;

        if (!uis.activemenu) {
            return;
        }

        if (uis.activemenu->key)
            s = uis.activemenu->key(key);
        else
            s = Menu_DefaultKey(uis.activemenu, key);

        if ((s > 0) && (s != menu_null_sound))
            trap_S_StartLocalSound(s, CHAN_LOCAL_SOUND);
    }

    void Atoms::MouseEvent(int32_t dx, int32_t dy) {
        int32_t i;
        menucommon_s *m;

        if (!uis.activemenu) {
            return;
        }

        /* update mouse screen position */
        uis.cursorx += dx;
        /* kinda pointless, but looks nice. allow negative offsets for widescreen setups (we must maintain the ratio or buttons will stop working) */
        if (IsWidescreen() && uis.widescreen.state == WIDESCREEN_CENTER) {
            if (uis.cursorx < (0 - uis.widescreen.bias))
                uis.cursorx = 0 - uis.widescreen.bias;
            else if (uis.cursorx > (SCREEN_WIDTH + uis.widescreen.bias))
                uis.cursorx = SCREEN_WIDTH + uis.widescreen.bias;
        } else {
            if (uis.cursorx < 0)
                uis.cursorx = 0;
            else if (uis.cursorx > SCREEN_WIDTH)
                uis.cursorx = SCREEN_WIDTH;
        }

        uis.cursory += dy;
        if (uis.cursory < 0)
            uis.cursory = 0;
        else if (uis.cursory > SCREEN_HEIGHT)
            uis.cursory = SCREEN_HEIGHT;

        /* RPG-X: TiM - Skip new selections if a spin control window is open */
        if (uis.activemenu->noNewSelecting) {
            return;
        }

        /* region test the active menu items */
        for (i = 0; i < uis.activemenu->nitems; i++) {
            m = (menucommon_s *) uis.activemenu->items[i];

            if (m->flags & (QMF_GRAYED | QMF_INACTIVE))
                continue;

            if ((uis.cursorx < m->left) ||
                (uis.cursorx > m->right) ||
                (uis.cursory < m->top) ||
                (uis.cursory > m->bottom)) {
                /* cursor out of item bounds */
                continue;
            }

            /* set focus to item at cursor */
            if (uis.activemenu->cursor != i) {
                Menu_SetCursor(uis.activemenu, i);
                ((menucommon_s *) (uis.activemenu->items[uis.activemenu->cursor_prev]))->flags &= ~QMF_HASMOUSEFOCUS;

                if (!(((menucommon_s *) (uis.activemenu->items[uis.activemenu->cursor]))->flags & QMF_SILENT)) {
                    trap_S_StartLocalSound(menu_move_sound, CHAN_LOCAL_SOUND);
                }
            }

            ((menucommon_s *) (uis.activemenu->items[uis.activemenu->cursor]))->flags |= QMF_HASMOUSEFOCUS;
            return;
        }

        if (uis.activemenu->nitems > 0) {
            /* out of any region */
            ((menucommon_s *) (uis.activemenu->items[uis.activemenu->cursor]))->flags &= ~QMF_HASMOUSEFOCUS;
        }
    }

    char *Atoms::Argv(int32_t arg) {
        static char buffer[MAX_STRING_CHARS];

        trap_Argv(arg, buffer, sizeof(buffer));

        return buffer;
    }

    char *Atoms::CvarVariableString(const char *var_name) {
        static char buffer[MAX_STRING_CHARS];

        trap_Cvar_VariableStringBuffer(var_name, buffer, sizeof(buffer));

        return buffer;
    }

    void Atoms::Cache() {
        MainMenu_Cache();
        InGame_Cache();
        ConfirmMenu_Cache();
        PlayerModel_Cache();
        PlayerSettings_Cache();
        ServerInfo_Cache();
        SpecifyServer_Cache();
        ArenaServers_Cache();
        StartServer_Cache();
        ServerOptions_Cache();
        DriverInfo_Cache();
        UI_NetworkOptionsMenu_Cache();
        UI_BotSelectMenu_Cache();
        UI_CDKeyMenu_Cache();
        UI_ModsMenu_Cache();
        UI_SoundMenu_Cache();
        UI_QuitMenu_Cache();
        UI_DemosMenu_Cache();
        UI_VideoDataMenu_Cache();
        UI_GameOptionsMenu_Cache();
        UI_ControlsMouseJoyStickMenu_Cache();
        UI_VideoData2Menu_Cache();
        UI_VideoDriverMenu_Cache();
        UI_HolomatchInMenu_Cache();
        UI_ChooseServerTypeMenu_Cache();
        UI_AdminMenu_Cache();
        UI_CreditsMenu_Cache();
        UI_PlayerEmotes_Cache();
        UI_MotdMenu_Cache();
        UI_msdMenu_Cache();
    }

    qboolean Atoms::ConsoleCommand() {
        char *cmd;
        /*int32_t	i;*/

        cmd = Argv(0);

        /* ensure minimum menu data is available*/
        Menu_Cache();

        if (Q_stricmp(cmd, "ui_cache") == 0) {
            Cache();
            return qtrue;
        }

        if (Q_stricmp(cmd, "ui_cdkey") == 0) {
            UI_CDKeyMenu_f();
            return qtrue;
        }

        if (Q_stricmp(cmd, "ui_emotes") == 0) {
            UI_EmotesMenu(qtrue);
            return qtrue;
        }

        if (Q_stricmp(cmd, "ui_admin") == 0) {
            UI_AdminMenu(qtrue);
            return qtrue;
        }

        if (Q_stricmp(cmd, "ui_turbolift") == 0) {
            UI_TurboliftMenu(atoi(Argv(1)));
            return qtrue;
        }

        if (Q_stricmp(cmd, "err_dialog") == 0) {
            UI_ConfirmMenu(Argv(1), 0, 0);
            return qtrue;
        }

        /* RPG-X | Marcin | 15/12/2008 */
        if (Q_stricmp(cmd, "ui_motd") == 0) {
            UI_MotdMenu();
            return qtrue;
        }

        if (Q_stricmp(cmd, "ui_motd_reset") == 0) {
            MotdReset();
            return qtrue;
        }

        /*
         *  RPG-X | Marcin | 18/12/2008
         * I hope this is the right way to do it...
         */
        if (Q_stricmp(cmd, "ui_motd_line") == 0) {
            MotdReceiveLine(Argv(1));
            return qtrue;
        }

        if (Q_stricmp(cmd, "ui_msd") == 0) {
            static char ui_msd1[MAX_STRING_CHARS], ui_msd2[MAX_STRING_CHARS], ui_msd3[MAX_STRING_CHARS], ui_msd4[MAX_STRING_CHARS], ui_msd5[MAX_STRING_CHARS], ui_msd6[MAX_STRING_CHARS], ui_msd7[MAX_STRING_CHARS], ui_msd8[MAX_STRING_CHARS], ui_msd9[MAX_STRING_CHARS], ui_msd10[MAX_STRING_CHARS];
            trap_Argv(1, ui_msd1, sizeof(ui_msd1));
            trap_Argv(2, ui_msd2, sizeof(ui_msd2));
            trap_Argv(3, ui_msd3, sizeof(ui_msd3));
            trap_Argv(4, ui_msd4, sizeof(ui_msd4));
            trap_Argv(5, ui_msd5, sizeof(ui_msd5));
            trap_Argv(6, ui_msd6, sizeof(ui_msd6));
            trap_Argv(7, ui_msd7, sizeof(ui_msd7));
            trap_Argv(8, ui_msd8, sizeof(ui_msd8));
            trap_Argv(9, ui_msd9, sizeof(ui_msd9));
            trap_Argv(10, ui_msd10, sizeof(ui_msd10));
            UI_msdMenu(atoi(ui_msd1), atoi(ui_msd2), atoi(ui_msd3), atoi(ui_msd4), atoi(ui_msd5), atoi(ui_msd6),
                       atoi(ui_msd7), atoi(ui_msd8), atoi(ui_msd9), ui_msd10);
            return qtrue;
        }

        if (Q_stricmp(cmd, "ui_transporter") == 0) {
            UI_TransporterMenu(atoi(Argv(1)));
            return qtrue;
        }

        if (Q_stricmp(cmd, "holo_data") == 0) {
            /*HoloDataReceived(Argv(1));*/
            return qtrue;
        }

        if (Q_stricmp(cmd, "ui_trdata") == 0) {
            TransDataReceived(Argv(1));
            return qtrue;
        }

        if (Q_stricmp(cmd, "ui_holodeck") == 0) {
            /*UI_HolodeckMenu( atoi(Argv(1)) );*/
            return qtrue;
        }

        if (Q_stricmp(cmd, "ui_sqlmenu") == 0) {
            UI_sqlMenu();
            return qtrue;
        }

        return qfalse;
    }

    void Atoms::Shutdown() {
    }

    char *Atoms::ParseFontParms(char *buffer, int32_t (*propArray)[3]) {
        char *token;
        int32_t i, i2;

        while (buffer) {
            token = COM_ParseExt(&buffer, qtrue);

            /* Start with open braket */
            if (!Q_stricmp(token, "{")) {
                for (i = 0; i < CHARMAX; ++i) {
                    /* Brackets for the numbers */
                    token = COM_ParseExt(&buffer, qtrue);
                    if (!Q_stricmp(token, "{")) { ;
                    } else {
                        trap_Print(
                                va(S_COLOR_RED "ParseFontParms : Invalid FONTS.DAT data, near character %d!\n", i));
                        return (NULL);
                    }

                    for (i2 = 0; i2 < 3; ++i2) {
                        token = COM_ParseExt(&buffer, qtrue);
                        propArray[i][i2] = atoi(token);
                    }

                    token = COM_ParseExt(&buffer, qtrue);
                    if (!Q_stricmp(token, "}")) { ;
                    } else {
                        trap_Print(
                                va(S_COLOR_RED "ParseFontParms : Invalid FONTS.DAT data, near character %d!\n", i));
                        return (NULL);
                    }
                }
            }

            token = COM_ParseExt(&buffer, qtrue);    /* Grab closing bracket */
            if (!Q_stricmp(token, "}")) {
                break;
            }
        }

        return (buffer);
    }
}
