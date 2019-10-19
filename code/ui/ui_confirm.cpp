// Copyright (C) 1999-2000 Id Software, Inc.
//
/*
=======================================================================

CONFIRMATION MENU

=======================================================================
*/

#include <utility>

#include "Atoms.h"
#include "ui_local.h"
#include <common/ColorTable.h>

using namespace common;
using namespace ui;

enum ui_confirmIDs_e { ID_CONFIRM_NO = 10, ID_CONFIRM_YES };

typedef struct {
  menuframework_s menu_{};

  menubitmap_s no_{};
  menubitmap_s yes_{};

  std::string question_;
  std::function<void()> draw_;
  std::function<void(bool result)> action_;
  qhandle_t corner_pic_{0};
} confirmMenu_t;

static confirmMenu_t s_confirm;

/*
=================
ConfirmMenu_Event
=================
*/
static void ConfirmMenu_Event(void *ptr, int32_t event) {
  qboolean result;

  if (event != QM_ACTIVATED) {
    return;
  }

  UI_PopMenu();

  if (((menucommon_s *)ptr)->id == ID_CONFIRM_NO) {
    result = qfalse;
  } else {
    result = qtrue;
  }

  if (s_confirm.action_) {
    s_confirm.action_(result);
  }
}

/*
=================
ConfirmMenu_Key
=================
*/
static sfxHandle_t ConfirmMenu_Key(int32_t key) {
  switch (key) {
  case K_KP_LEFTARROW:
  case K_LEFTARROW:
  case K_KP_RIGHTARROW:
  case K_RIGHTARROW:
    key = K_TAB;
    break;

  case 'n':
  case 'N':
    ConfirmMenu_Event(&s_confirm.no_, QM_ACTIVATED);
    break;

  case 'y':
  case 'Y':
    ConfirmMenu_Event(&s_confirm.yes_, QM_ACTIVATED);
    break;
  }

  return Menu_DefaultKey(&s_confirm.menu_, key);
}

/*
=================
ConfirmMenu_Draw
=================
*/
static void ConfirmMenu_Draw() {
  UI_MenuFrame(&s_confirm.menu_);

  // End of upper line
  trap_R_SetColor(colorTable[CT_LTBROWN1]);
  Atoms::DrawHandlePic(
      {482, 136, MENU_BUTTON_MED_WIDTH - 22, MENU_BUTTON_MED_HEIGHT},
      uis.whiteShader);
  Atoms::DrawHandlePic(
      {460 + MENU_BUTTON_MED_WIDTH - 6, 136, -19, MENU_BUTTON_MED_HEIGHT},
      uis.graphicButtonLeftEnd); // right

  trap_R_SetColor(colorTable[CT_DKBLUE2]);
  Atoms::DrawHandlePic({30, 203, 47, 186}, uis.whiteShader); // Left hand column

  Atoms::DrawProportionalString({74, 66}, "286", UI_RIGHT | UI_TINYFONT,
                                ColorTable[CT_BLACK]);
  Atoms::DrawProportionalString({74, 84}, "386", UI_RIGHT | UI_TINYFONT,
                                ColorTable[CT_BLACK]);
  Atoms::DrawProportionalString({74, 188}, "486", UI_RIGHT | UI_TINYFONT,
                                ColorTable[CT_BLACK]);
  Atoms::DrawProportionalString({74, 207}, "8088", UI_RIGHT | UI_TINYFONT,
                                ColorTable[CT_BLACK]);
  Atoms::DrawProportionalString({74, 395}, "12799", UI_RIGHT | UI_TINYFONT,
                                ColorTable[CT_BLACK]);

  trap_R_SetColor(colorTable[CT_LTBLUE1]);
  Atoms::DrawHandlePic({132, 142, 128, -64},
                       s_confirm.corner_pic_); // Top Left corner
  Atoms::DrawHandlePic({132, 352, 128, 64},
                       s_confirm.corner_pic_); // Bottom Left corner

  Atoms::DrawHandlePic({429, 142, -128, -64},
                       s_confirm.corner_pic_); // Top Right corner
  Atoms::DrawHandlePic({429, 352, -128, 64},
                       s_confirm.corner_pic_); // Bottom Right corner

  Atoms::DrawHandlePic({145, 175, 395, 18}, uis.whiteShader); // Top
  Atoms::DrawHandlePic({132, 193, 47, 175}, uis.whiteShader); // Left side
  Atoms::DrawHandlePic({510, 193, 47, 175}, uis.whiteShader); // Right side
  Atoms::DrawHandlePic({147, 365, 65, 18}, uis.whiteShader);  // Bottom Left
  Atoms::DrawHandlePic({477, 365, 65, 18}, uis.whiteShader);  // Bottom Right

  // TiM - if message is a long one, break it up and display it on separate
  // lines
  if (s_confirm.question_.length() < 45) {
    Atoms::DrawProportionalString({345, 269}, s_confirm.question_,
                                  UI_SMALLFONT | UI_CENTER, color_red);
  } else {
    int32_t numLines;
    int32_t height;
    int32_t y;
    int32_t i, j;
    int32_t spacePoint = 45;

    auto message = s_confirm.question_;
    numLines = message.length() / 45 + 1;
    height = (SMALLCHAR_HEIGHT + 4) * numLines;
    y = 277 - (height * 0.5); // 269

    // UI_Logger( LL_DEBUG, "numLines: %i\n", numLines );

    for (i = 0; i < numLines; i++) {
      if (!message.empty()) {
        break;
      }

      if (message.length() >= 45) {
        for (j = 1; j <= 45; j++) {
          if (message[j] == ' ')
            spacePoint = j;
        }
      } else {
        spacePoint = message.length();
      }

      Atoms::DrawProportionalString({345, y}, message.substr(0, spacePoint + 1),
                                    UI_SMALLFONT | UI_CENTER, color_red);

      y += SMALLCHAR_HEIGHT + 4;
      message += s_confirm.question_.substr(spacePoint);
    }
  }

  Menu_Draw(&s_confirm.menu_);

  if (s_confirm.draw_) {
    s_confirm.draw_();
  }
}

/*
=================
ConfirmMenu_Cache
=================
*/
void ConfirmMenu_Cache() {
  s_confirm.corner_pic_ =
      trap_R_RegisterShaderNoMip("menu/common/corner_ll_47_18.tga");
}

std::string TranslateQuestion(const std::string &question) {
  if (question == "Unpure client detected. Invalid .PK3 files referenced!") {
    return menu_normal_text[MNT_UNPURECLIENT];
  }

  if (question == "Cannot validate pure client!") {
    return menu_normal_text[MNT_CANNOTVALIDATE];
  }

  if (question == "kicked") {
    return menu_normal_text[MNT_KICKED];
  }

  if (question == "timed out") {
    return menu_normal_text[MNT_TIMEDOUT];
  }

  if (question == "server shut down") {
    return menu_normal_text[MNT_SERVERSHUTDOWN];
  }

  if (question == "disconnected") {
    return menu_normal_text[MNT_DISCONNECTED];
  }

  if (question == "broken download") {
    return menu_normal_text[MNT_BROKENDOWNLOAD];
  }

  if (question == "Server command overflow") {
    return menu_normal_text[MNT_SERVERCOMMANDOVERFLOW];
  }

  if (question == "Lost reliable commands") {
    return menu_normal_text[MNT_LOSTRELIABLECOMMANDS];
  }

  return question;
}

/*
=================
UI_ConfirmMenu
=================
*/
void UI_ConfirmMenu(const std::string &question, std::function<void()> draw,
                    std::function<void(bool)> action) {
  s_confirm = {};

  ConfirmMenu_Cache();

  Mouse_Show();

  s_confirm.question_ = TranslateQuestion(question);
  s_confirm.draw_ = std::move(draw);
  s_confirm.action_ = std::move(action);

  s_confirm.menu_.draw = ConfirmMenu_Draw;
  s_confirm.menu_.key = ConfirmMenu_Key;
  s_confirm.menu_.wrapAround = qtrue;
  s_confirm.menu_.fullscreen = qtrue;
  s_confirm.menu_.descX = MENU_DESC_X;
  s_confirm.menu_.descY = MENU_DESC_Y;
  s_confirm.menu_.titleX = MENU_TITLE_X;
  s_confirm.menu_.titleY = MENU_TITLE_Y;
  s_confirm.menu_.titleI = MNT_CONFIRMATIONMENU_TITLE;
  s_confirm.menu_.footNoteEnum = MNT_CONFIRMATION;

  s_confirm.yes_.generic.type = MTYPE_BITMAP;
  s_confirm.yes_.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
  s_confirm.yes_.generic.x = 215;
  s_confirm.yes_.generic.y = 365;
  s_confirm.yes_.generic.callback = ConfirmMenu_Event;
  s_confirm.yes_.generic.id = ID_CONFIRM_YES;
  s_confirm.yes_.generic.name = GRAPHIC_SQUARE;
  s_confirm.yes_.width = MENU_BUTTON_MED_WIDTH;
  s_confirm.yes_.height = MENU_BUTTON_MED_HEIGHT;
  s_confirm.yes_.color = CT_DKPURPLE1;
  s_confirm.yes_.color2 = CT_LTPURPLE1;
  s_confirm.yes_.textX = MENU_BUTTON_TEXT_X;
  s_confirm.yes_.textY = MENU_BUTTON_TEXT_Y;
  s_confirm.yes_.textEnum = MBT_GENERIC_YES;
  s_confirm.yes_.textcolor = CT_BLACK;
  s_confirm.yes_.textcolor2 = CT_WHITE;

  s_confirm.no_.generic.type = MTYPE_BITMAP;
  s_confirm.no_.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
  s_confirm.no_.generic.x = 371;
  s_confirm.no_.generic.y = 365;
  s_confirm.no_.generic.callback = ConfirmMenu_Event;
  s_confirm.no_.generic.id = ID_CONFIRM_NO;
  s_confirm.no_.generic.name = GRAPHIC_SQUARE;
  s_confirm.no_.width = 103;
  s_confirm.no_.height = MENU_BUTTON_MED_HEIGHT;
  s_confirm.no_.color = CT_DKPURPLE1;
  s_confirm.no_.color2 = CT_LTPURPLE1;
  s_confirm.no_.textX = MENU_BUTTON_TEXT_X;
  s_confirm.no_.textY = MENU_BUTTON_TEXT_Y;
  s_confirm.no_.textEnum = MBT_GENERIC_NO;
  s_confirm.no_.textcolor = CT_BLACK;
  s_confirm.no_.textcolor2 = CT_WHITE;

  Menu_AddItem(&s_confirm.menu_, &s_confirm.yes_);
  if (s_confirm.action_) {
    Menu_AddItem(&s_confirm.menu_, &s_confirm.no_);
  }

  UI_PushMenu(&s_confirm.menu_);

  Menu_SetCursorToItem(&s_confirm.menu_, &s_confirm.no_);
}
