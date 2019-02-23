#pragma once

#include "ui_local.h"
#include "ui_logger.h"
#include <base_game/bg_misc.h>
#include <qcommon/stv_version.h>
#include <common/Point2d.h>
#include <array>
#include <common/Rectangle.h>
#include <common/Color.h>

namespace ui {
    class Atoms {
    protected:
        static inline constexpr int32_t PROPB_GAP_WIDTH = 4;
        static inline constexpr int32_t PROPB_SPACE_WIDTH = 12;
        static inline constexpr int32_t PROPB_HEIGHT = 32;
        static inline constexpr int32_t CHARMAX = 256;
        static inline constexpr int32_t FONT_BUFF_LENGTH = 20000;

        struct MenuFrame {
            static inline bool initialized;
            qhandle_t cornerUpper;
            qhandle_t cornerUpper2;
            qhandle_t cornerLower;
        };

        static inline MenuFrame s_menuFrame;

        static inline constexpr auto MAXMENUTEXT = 15000;
        static inline constexpr auto MAXBUTTONTEXT = 50000;
        static inline char MenuText[MAXMENUTEXT];
        static inline char ButtonText[MAXBUTTONTEXT];
        static inline constexpr std::string_view menuEmptyLine = " ";

    public:
        static void SecurityCodeSetup();

        static void SecurityFeedback(bool result);

        static std::string LanguageFilename(std::string_view baseName, std::string_view baseExtension);

        static void InitSpinControl(menulist_s *spincontrol);

        static void LoadButtonText();

        static void ParseButtonText();

        static void LoadMenuText();

        static void ParseMenuText();

        static void MenuFrame2(menuframework_s *menu);

        static void MenuFrame(menuframework_s *menu);

        static bool MenuBottomLineEnd_Graphics(std::string_view string, common::Color color);

        static void FrameBottom_Graphics();

        static void FrameTop_Graphics(menuframework_s *menu);

        static void MenuFrame_Cache();

        static void PrecacheMenuGraphics(menugraphics_s *menuGraphics, int32_t maxI);

        static void PrintMenuGraphics(menugraphics_s *menuGraphics, int32_t maxI);

        static void
        DrawNumField(const common::Point2dI &position, size_t width, int32_t value, size_t charWidth,
                     size_t charHeight);

        static bool CursorInRect(const common::RectangleI &rect);

        static void Refresh(int32_t realtime);

        static void DrawRect(float x, float y, float width, float height, const float *color);

        static void FillRect(float x, float y, float width, float height, const float *color);

        static void DrawHandlePic(float x, float y, float w, float h, qhandle_t hShader);

        static void DrawHandleStretchPic(float x, float y, float w, float h, float s0, float t0, float s1, float t1,
                                         qhandle_t hShader);


        static void DrawNamedPic(float x, float y, float width, float height, const char *picname);

        static void AdjustFrom640(float *x, float *y, float *w, float *h);

        static void Init();

        static int32_t PopulateClassSetArray(char *classSets[]);

        static int32_t GetClassSets();

        static int32_t PopulateClassArray(char *classes[]);

        static int32_t GetRankSets();

        static int32_t PopulateRankSetArray(char *rankSets[]);

        static void InitRanksData(char *ranksName);

        static int32_t PopulateRanksArray(char *ranks[]);

        static int32_t InitClassData(char *fileName);

        static void LoadFonts();

        static bool IsWidescreen();

        static constexpr float clampCvar(float min, float max, float value);

        static void pushMenu(menuframework_s *menu);

        static void PopMenu();

        static void ForceMenuOff();

        static void LerpColor(common::Color a, common::Color b, common::Color& c, float t);

        static void DrawBannerString2(int32_t x, int32_t y, const char *str, vec4_t color);

        static void DrawBannerString(int32_t x, int32_t y, const char *str, int32_t style, vec4_t color);

        static size_t ProportionalStringWidth(std::string_view str, int32_t style);

        static void
        DrawProportionalString2(common::Point2dI pos, std::string_view str, common::Color color, int32_t style,
                                qhandle_t charset);

        static float ProportionalSizeScale(int32_t style);

        static void
        DrawProportionalString(common::Point2dI pos, std::string_view str, int32_t style, common::Color color);

        static void DrawString2(common::Point2dI pos, std::string_view str, common::Color color, int32_t charw, int32_t charh);

        static void DrawString(common::Point2dI pos, std::string_view str, int32_t style, common::Color color, bool highRes);

        static void DrawChar(common::Point2dI pos, char ch, int32_t style, common::Color color);

        static qboolean IsFullscreen();

        static void NeedCDAction(qboolean result);

        static void SetActiveMenu(uiMenuCommand_t menu);

        static void KeyEvent(int32_t key);

        static void MouseEvent(int32_t dx, int32_t dy);

        static char *Argv(int32_t arg);

        static char *CvarVariableString(const char *var_name);

        static void Cache();

        static qboolean ConsoleCommand();

        static void Shutdown();

        static char *ParseFontParms(char *buffer, int32_t propArray[CHARMAX][3]);

    protected:
        static inline constexpr int32_t propMapB[26][3]{
                {11,  12,  33},
                {49,  12,  31},
                {85,  12,  31},
                {120, 12,  30},
                {156, 12,  21},
                {183, 12,  21},
                {207, 12,  32},

                {13,  55,  30},
                {49,  55,  13},
                {66,  55,  29},
                {101, 55,  31},
                {135, 55,  21},
                {158, 55,  40},
                {204, 55,  32},

                {12,  97,  31},
                {48,  97,  31},
                {82,  97,  30},
                {118, 97,  30},
                {153, 97,  30},
                {185, 97,  25},
                {213, 97,  30},

                {11,  139, 32},
                {42,  139, 51},
                {93,  139, 32},
                {126, 139, 31},
                {158, 139, 25}
        };

        static inline int32_t propMapBig[CHARMAX][3];
        static inline int32_t propMap[CHARMAX][3];
        static inline int32_t propMapTiny[CHARMAX][3];

        static inline constexpr int32_t specialTinyPropChars[CHARMAX][2] = {
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 10 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 20 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 30 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 40 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 50 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 60 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 70 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 80 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 90 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 100 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 110 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 120 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 130 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 140 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 150 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {2, -3},
                {0, 0},    /* 160 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 170 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 180 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 190 */
                {0, -1},
                {2, -3},
                {2, -3},
                {2, -3},
                {2, -3},
                {2, -3},
                {2, -3},
                {0, 0},
                {2, 0},
                {2, -3},    /* 200 */
                {2, -3},
                {2, -3},
                {2, -3},
                {2, -3},
                {2, -3},
                {2, -3},
                {2, -3},
                {0, -1},
                {2, -3},
                {2, -3},    /* 210 */
                {2, -3},
                {3, -3},
                {2, -3},
                {2, -3},
                {0, 0},
                {0, -1},
                {2, -3},
                {2, -3},
                {2, -3},
                {2, -3},    /* 220 */
                {2, -3},
                {0, -1},
                {0, -1},
                {2, -3},
                {2, -3},
                {2, -3},
                {2, -3},
                {2, -3},
                {2, -3},
                {0, 0},    /* 230 */
                {2, 0},
                {2, -3},
                {2, -3},
                {2, -3},
                {2, -3},
                {2, -3},
                {2, -3},
                {2, -3},
                {2, -3},
                {0, 0},    /* 240 */
                {2, -3},
                {2, -3},
                {2, -3},
                {2, -3},
                {2, -3},
                {2, -3},
                {0, 0},
                {0, -1},
                {2, -3},
                {2, -3},    /* 250 */
                {2, -3},
                {2, -3},
                {2, -3},
                {0, -1},
                {2, -3}                    /* 255 */
        };


        static inline constexpr int32_t specialPropChars[CHARMAX][2] = {
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 10 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 20 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 30 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 40 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 50 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 60 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 70 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 80 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 90 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 100 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 110 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 120 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 130 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 140 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 150 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 160 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 170 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 180 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 190 */
                {2, -2},
                {2, -2},
                {2, -2},
                {2, -2},
                {2, -2},
                {2, -2},
                {2, -2},
                {0, 0},
                {1, 1},
                {2, -2},    /* 200 */
                {2, -2},
                {2, -2},
                {2, -2},
                {2, -2},
                {2, -2},
                {2, -2},
                {2, -2},
                {0, 0},
                {2, -2},
                {2, -2},    /* 210 */
                {2, -2},
                {2, -2},
                {2, -2},
                {2, -2},
                {0, 0},
                {0, 0},
                {2, -2},
                {2, -2},
                {2, -2},
                {2, -2},    /* 220 */
                {2, -2},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 230 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 240 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 250 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0}                    /* 255 */
        };


        static inline constexpr int32_t specialBigPropChars[CHARMAX][2] = {
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 10 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 20 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 30 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 40 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 50 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 60 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 70 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 80 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 90 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 100 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 110 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 120 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 130 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 140 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 150 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 160 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 170 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 180 */
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},    /* 190 */
                {3, -3},
                {3, -3},
                {3, -3},
                {3, -3},
                {3, -3},
                {3, -3},
                {3, -3},
                {0, 0},
                {3, 1},
                {3, -3},    /* 200 */
                {3, -3},
                {3, -3},
                {3, -3},
                {3, -3},
                {3, -3},
                {3, -3},
                {3, -3},
                {0, 0},
                {3, -3},
                {3, -3},    /* 210 */
                {3, -3},
                {3, -3},
                {3, -3},
                {3, -3},
                {0, 0},
                {0, 0},
                {3, -3},
                {3, -3},
                {3, -3},
                {3, -3},    /* 220 */
                {3, -3},
                {0, 0},
                {0, 0},
                {3, -3},
                {3, -3},
                {3, -3},
                {3, -3},
                {3, -3},
                {3, -3},
                {0, 0},    /* 230 */
                {3, 1},
                {3, -3},
                {3, -3},
                {3, -3},
                {3, -3},
                {3, -3},
                {3, -3},
                {3, -3},
                {3, -3},
                {0, 0},    /* 240 */
                {3, -3},
                {3, -3},
                {3, -3},
                {3, -3},
                {3, -3},
                {3, -3},
                {0, 0},
                {0, 0},
                {3, -3},
                {3, -3},    /* 250 */
                {3, -3},
                {3, -3},
                {3, -3},
                {0, 0},
                {3, -3}                    /* 255 */
        };

        static inline int32_t showColorChars;
    };
}
