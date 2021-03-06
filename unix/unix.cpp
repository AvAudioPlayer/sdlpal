#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "util.h"
#include "palcfg.h"

#include <syslog.h>

#ifndef PAL_NO_LAUNCH_UI

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Radio_Round_Button.H>
#include <FL/Fl_Hor_Value_Slider.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Choice.H>

struct {
   Fl_Input* gamepath;
   Fl_Input* fontfile;
   Fl_Input* logfile;
   Fl_Input* msgfile;
   Fl_Check_Button* touch;
   Fl_Check_Button* aspect;
   Fl_Check_Button* fullscreen;
   Fl_Check_Button* avi;
   Fl_Choice* loglevel;
   Fl_Choice* cd;
   Fl_Choice* bgm;
   Fl_Choice* opl;
   Fl_Int_Input* samplerate;
   Fl_Check_Button* stereo;
   Fl_Int_Input* oplrate;
   Fl_Check_Button* surround;
   Fl_Int_Input* buffer;
   Fl_Hor_Value_Slider* quality;
   Fl_Hor_Value_Slider* music;
   Fl_Hor_Value_Slider* sound;
} gWidgets;

struct {
   const char* title;
   const char* language;
   const char* display;
   const char* audio;
   const char* logging;
   const char* gamepath;
   const char* msgfile;
   const char* fontfile;
   const char* logfile;
   const char* loglevel;
   const char* touch;
   const char* aspect;
   const char* fullscreen;
   const char* avi;
   const char* cd;
   const char* bgm;
   const char* opl;
   const char* samplerate;
   const char* stereo;
   const char* oplrate;
   const char* surround;
   const char* musvol;
   const char* sndvol;
   const char* buffer;
   const char* quality;
   const char* exit;
   const char* launch;
   const char* def;
   const char* levels;
} gLabels[3] = {
   { "SDLPAL Launcher",     "Language & Font",    "Display",      "Audio",      "Logging",
     "Game resource path:", "Message file:",      "Font file:",   "Log file:",  "Log level:",
     "Use touc&h overlay",  "&Keep aspect ratio", "&Full screen", "Enable A&VI","&CD type:",
     "&BGM type:",          "&OPL type:",         "Sample rate:", "Ste&reo",    "OPL rate:",
     "Surround O&PL",       "Music volume:",      "Sound volume:","Buffer:",    "Quality:",
     "E&xit",               "&Launch game",       "&Default",     "Verbose|Debug|Informational|Warning|Error|Fatal" },
   { "SDLPAL 启动器",        "字体及语言设置",      "显示设置",      "音频设置",     "日志记录设置",
     "游戏资源目录：",        "语言文件：",         "字体文件",      "日志文件",     "日志记录级别：",
     "启用触屏辅助(&H)",      "保持纵横比(&K)",     "全屏模式(&F)",  "AVI 动画(&V)", "&CD 音源：",
     "&BGM 音源：",          "&OPL 类型：",        "采样率：",      "立体声(&R)",   "OPL 采样率：",
     "环绕声 O&PL",          "音乐音量：",         "音效音量：",     "缓冲区：",     "质量：",
     "退出(&X)",             "启动游戏(&L)",       "默认设置(&D)",  "详细信息|调试信息|运行信息|普通警告|严重错误|致命错误" },
   { "SDLPAL 啟動器",        "字體及語言設定",      "顯示設定",      "音訊設定",      "日誌記錄設定",
     "遊戲資源檔夾：",        "語言檔：",           "字體檔：",      "日誌檔：",      "日誌記錄級別：",
     "啟用觸屏輔助(&H)",      "保持縱橫比(&K)",     "全屏模式(&F)",  "AVI 動畫(&V)",  "&CD 音源：",
     "&BGM 音源：",          "&OPL 類型：",        "取樣速率：",    "立體聲(&R)",    "OPL 取樣速率：",
     "環繞聲 O&PL",          "音樂音量：",          "音效音量：",    "緩衝區：",      "品質：",
     "退出(&X)",             "啟動遊戲(&L)",       "默認設定(&D)",  "詳細信息|調試信息|運行信息|普通警告|嚴重錯誤|致命錯誤" },
};

void InitControls()
{
   char buffer[64];
   gWidgets.gamepath->value(gConfig.pszGamePath);
   gWidgets.msgfile->value(gConfig.pszMsgFile);
   gWidgets.fontfile->value(gConfig.pszFontFile);
   gWidgets.logfile->value(gConfig.pszLogFile);
   gWidgets.loglevel->value(gConfig.iLogLevel);
   gWidgets.touch->value(gConfig.fUseTouchOverlay ? 1 : 0);
   gWidgets.aspect->value(gConfig.fKeepAspectRatio ? 1 : 0);
   gWidgets.fullscreen->value(gConfig.fFullScreen ? 1 : 0);
   gWidgets.avi->value(gConfig.fEnableAviPlay ? 1 : 0);
   gWidgets.cd->value(gConfig.eCDType - MUSIC_MP3);
   gWidgets.bgm->value(gConfig.eMusicType);
   gWidgets.stereo->value(gConfig.iAudioChannels == 2 ? 1 : 0);
   sprintf(buffer, "%d", gConfig.iSampleRate); gWidgets.samplerate->value(buffer);
   gWidgets.opl->value(gConfig.eOPLType);
   sprintf(buffer, "%d", gConfig.iOPLSampleRate); gWidgets.oplrate->value(buffer);
   gWidgets.surround->value(gConfig.fUseSurroundOPL ? 1 : 0);
   sprintf(buffer, "%d", gConfig.wAudioBufferSize); gWidgets.buffer->value(buffer);
   gWidgets.quality->value(gConfig.iResampleQuality);
   gWidgets.music->value(gConfig.iMusicVolume);
   gWidgets.sound->value(gConfig.iSoundVolume);
}

void SaveControls()
{
   free(gConfig.pszGamePath);
   free(gConfig.pszMsgFile);
   free(gConfig.pszFontFile);
   free(gConfig.pszLogFile);

   gConfig.pszGamePath = *gWidgets.gamepath->value() ? strdup(gWidgets.gamepath->value()) : nullptr;
   gConfig.pszSavePath = *gWidgets.gamepath->value() ? strdup(gWidgets.gamepath->value()) : nullptr;
   gConfig.pszMsgFile = *gWidgets.msgfile->value() ? strdup(gWidgets.msgfile->value()) : nullptr;
   gConfig.pszFontFile = *gWidgets.fontfile->value() ? strdup(gWidgets.fontfile->value()) : nullptr;
   gConfig.pszLogFile = *gWidgets.logfile->value() ? strdup(gWidgets.logfile->value()) : nullptr;

   gConfig.iLogLevel = (LOGLEVEL)gWidgets.loglevel->value();
   gConfig.fUseTouchOverlay = gWidgets.touch->value();
   gConfig.fKeepAspectRatio = gWidgets.aspect->value();
   gConfig.fFullScreen = gWidgets.fullscreen->value();
   gConfig.fEnableAviPlay = gWidgets.avi->value();
   gConfig.eCDType = (MUSICTYPE)(gWidgets.cd->value() + MUSIC_MP3);
   gConfig.eMusicType = (MUSICTYPE)(gWidgets.bgm->value());
   gConfig.iAudioChannels = gWidgets.stereo->value() ? 2 : 1;
   gConfig.iSampleRate = atoi(gWidgets.samplerate->value());
   gConfig.eOPLType = (OPLTYPE)gWidgets.opl->value();
   gConfig.iOPLSampleRate = atoi(gWidgets.oplrate->value());
   gConfig.fUseSurroundOPL = gWidgets.surround->value();
   gConfig.wAudioBufferSize = atoi(gWidgets.buffer->value());
   gConfig.iResampleQuality = (int)gWidgets.quality->value();
   gConfig.iMusicVolume = (int)gWidgets.music->value();
   gConfig.iSoundVolume = (int)gWidgets.sound->value();
   gConfig.fLaunchSetting = FALSE;
}

int GetLanguage()
{
   auto lang = getenv("LANG");
   if (!lang) return 0;
   if (strncasecmp(lang, "zh_", 3) == 0)
   {
      if (strncasecmp(lang + 3, "hans", 4) == 0 || strncasecmp(lang + 3, "CN", 2) == 0 || strncasecmp(lang + 3, "SG", 2) == 0)
         return 1;
      else
         return 2;
   }
   else
      return 0;
}

Fl_Window* InitWindow()
{
   int lang = GetLanguage();
   Fl_Window* window = new Fl_Window(640, 400, gLabels[lang].title);

   gWidgets.gamepath = new Fl_Input(160, 9, 475, 22, gLabels[lang].gamepath);

   (new Fl_Box(FL_BORDER_BOX, 5, 50, 630, 55, gLabels[lang].language))->align(FL_ALIGN_TOP);
   gWidgets.msgfile = new Fl_Input(109, 54, 516, 22, gLabels[lang].msgfile);
   gWidgets.fontfile = new Fl_Input(109, 79, 516, 22, gLabels[lang].fontfile);

   (new Fl_Box(FL_BORDER_BOX, 5, 127, 630, 30, gLabels[lang].logging))->align(FL_ALIGN_TOP);
   (gWidgets.loglevel = new Fl_Choice(85, 132, 120, 20, gLabels[lang].loglevel))->add(gLabels[lang].levels);
   gWidgets.logfile = new Fl_Input(284, 131, 341, 22, gLabels[lang].logfile);

   (new Fl_Box(FL_BORDER_BOX, 5, 180, 630, 30, gLabels[lang].display))->align(FL_ALIGN_TOP);
   gWidgets.touch = new Fl_Check_Button(10, 185, 150, 20, gLabels[lang].touch);
   gWidgets.aspect = new Fl_Check_Button(200, 185, 150, 20, gLabels[lang].aspect);
   gWidgets.avi = new Fl_Check_Button(390, 185, 100, 20, gLabels[lang].avi);
   gWidgets.fullscreen = new Fl_Check_Button(530, 185, 100, 20, gLabels[lang].fullscreen);

   (new Fl_Box(FL_BORDER_BOX, 5, 230, 630, 130, gLabels[lang].audio))->align(FL_ALIGN_TOP);
   (gWidgets.cd = new Fl_Choice(84, 239, lang ? 100 : 120, 22, gLabels[lang].cd))->add("MP3|OGG");
   (gWidgets.bgm = new Fl_Choice(285, 239, 60, 22, gLabels[lang].bgm))->add("MIDI|RIX|MP3|OGG");
   gWidgets.stereo = new Fl_Check_Button(365, 240, 70, 20, gLabels[lang].stereo);
   gWidgets.samplerate = new Fl_Int_Input(570, 239, 60, 22, gLabels[lang].samplerate);
   (gWidgets.opl = new Fl_Choice(84, 269, lang ? 100 : 120, 22, gLabels[lang].opl))->add("DOSBOX|MAME|DOSBOXNEW|NUKED");
   gWidgets.oplrate = new Fl_Int_Input(285, 269, 60, 22, gLabels[lang].oplrate);
   gWidgets.surround = new Fl_Check_Button(365, 270, 120, 20, gLabels[lang].surround);
   gWidgets.buffer = new Fl_Int_Input(570, 269, 60, 22, gLabels[lang].buffer);

   gWidgets.quality = new Fl_Hor_Value_Slider(72, 299, 180, 22, gLabels[lang].quality);
   gWidgets.quality->align(FL_ALIGN_LEFT);
   gWidgets.quality->bounds(0, 4);
   gWidgets.quality->precision(0);

   gWidgets.music = new Fl_Hor_Value_Slider(380, 299, 250, 22, gLabels[lang].musvol);
   gWidgets.music->align(FL_ALIGN_LEFT);
   gWidgets.music->bounds(0, 100);
   gWidgets.music->precision(0);

   gWidgets.sound = new Fl_Hor_Value_Slider(380, 329, 250, 22, gLabels[lang].sndvol);
   gWidgets.sound->align(FL_ALIGN_LEFT);
   gWidgets.sound->bounds(0, 100);
   gWidgets.sound->precision(0);

   (new Fl_Button(5, 370, 120, 24, gLabels[lang].exit))->callback([](Fl_Widget* ctrl, void* window) {
      if (ctrl->when() == FL_WHEN_RELEASE)
         static_cast<Fl_Window*>(window)->hide();
   }, window);

   (new Fl_Button(260, 370, 120, 24, gLabels[lang].launch))->callback([](Fl_Widget* ctrl, void* window) {
      if (ctrl->when() == FL_WHEN_RELEASE) {
         SaveControls();
         PAL_SaveConfig();
         static_cast<Fl_Window*>(window)->hide();
      }
   }, window);

   (new Fl_Button(515, 370, 120, 24, gLabels[lang].def))->callback([](Fl_Widget*) { PAL_LoadConfig(FALSE); InitControls(); });

   window->end();

   InitControls();

   return window;
}

#endif

BOOL
UTIL_GetScreenSize(
   DWORD *pdwScreenWidth,
   DWORD *pdwScreenHeight
)
{
   return pdwScreenWidth && pdwScreenHeight && *pdwScreenWidth && *pdwScreenHeight;
}

BOOL
UTIL_IsAbsolutePath(
   LPCSTR lpszFileName
)
{
   return lpszFileName && *lpszFileName == '/';
}

INT
UTIL_Platform_Init(
   int argc,
   char* argv[]
)
{
	openlog("sdlpal", LOG_PERROR | LOG_PID, LOG_USER);
	UTIL_LogAddOutputCallback([](LOGLEVEL level, const char* str, const char*)->void {
		const static int priorities[] = {
			LOG_DEBUG,
			LOG_DEBUG,
			LOG_INFO,
			LOG_WARNING,
			LOG_ERR,
			LOG_EMERG
		};
		syslog(priorities[level], "%s", str);
	}, PAL_DEFAULT_LOGLEVEL);

#if !defined(UNIT_TEST) && !defined(PAL_NO_LAUNCH_UI)
   if (gConfig.fLaunchSetting)
   {
      Fl_Window *window = InitWindow();
      window->show(argc, argv);
      Fl::run();
      Fl::flush();
      delete window;
   }
#else
   gConfig.fLaunchSetting = FALSE;
#endif
   return 0;
}

VOID
UTIL_Platform_Quit(
   VOID
)
{
	closelog();
}
