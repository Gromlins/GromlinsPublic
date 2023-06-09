#include <string>
#include <vector>

#include "Vector.h"
#include "xorstr.h"


static const char* TargetModes[] =
{
	"Chest + Head",
	"Head Only",
	"Chest Only",
};

static const char* keyNames[] =
{
	"",
	"Left Mouse",
	"Right Mouse",
	"Cancel",
	"Middle Mouse",
	"Mouse 5",
	"Mouse 4",
	"",
	"Backspace",
	"Tab",
	"",
	"",
	"Clear",
	"Enter",
	"",
	"",
	"Shift",
	"Control",
	"Alt",
	"Pause",
	"Caps",
	"",
	"",
	"",
	"",
	"",
	"",
	"Escape",
	"",
	"",
	"",
	"",
	"Space",
	"Page Up",
	"Page Down",
	"End",
	"Home",
	"Left",
	"Up",
	"Right",
	"Down",
	"",
	"",
	"",
	"Print",
	"Insert",
	"Delete",
	"",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"",
	"",
	"",
	"",
	"",
	"Numpad 0",
	"Numpad 1",
	"Numpad 2",
	"Numpad 3",
	"Numpad 4",
	"Numpad 5",
	"Numpad 6",
	"Numpad 7",
	"Numpad 8",
	"Numpad 9",
	"Multiply",
	"Add",
	"",
	"Subtract",
	"Decimal",
	"Divide",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"F11",
	"F12",
};



namespace setting
{
	extern int SelectLine;

	extern bool OpenMenu;

	extern bool BoxESP;
	extern bool SkeltonESP;
	extern bool DistanceESP;
	extern bool NameESP;
	extern bool HealthESP;
	extern bool SeerESP;
	extern bool SnapLineESP;

	extern bool ESPBack;

	extern float BackColor_trans;
	extern float BackOutline_think;

	extern Vector3 BackColor;
	extern Vector3 BackOutlineColor;
	extern Vector3 BoxColor[2];
	extern Vector3 SkeltonColor[2];
	extern Vector3 GlowColor[2];
	extern Vector3 TextColor;

	extern bool GlowESP;

	extern int ESPMax;
	extern int SeerEspMax;
	extern int SkeltonEspMax;


	extern bool ActiveAimbot;
	extern bool AimbotRcs;
	extern bool DownAim;
	extern bool VisivleCheck;
	extern bool TeamCheck;

	extern int AimKey1;
	extern int AimKey2;
	extern bool TwoKeyMode;

	extern int TargetMode;

	extern int Smooth;
	extern int Fov;
	extern int AimMax;
	extern int rcsp;

	extern bool ShowFov;

	extern bool SpectatorList;

	extern std::vector<std::string> configs;
	extern const char* config_str[100];

	extern char savefile[30];

	void ConvertStringVectorToCharArray(const std::vector<std::string>& strVec, const char* charArray[]);

	std::string GetConfigPath();

	bool SaveConfig(std::string filename);
	bool LoadConfig(std::string filename);

	std::vector<std::string> GetConfigs();
}