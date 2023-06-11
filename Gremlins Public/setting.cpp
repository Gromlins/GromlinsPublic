#include <filesystem>
#include <fstream>
#include <shlobj.h>

#include "setting.hpp"
#include "json.hpp"

#include "xorstr.h"

using namespace nlohmann;

namespace setting
{
	int SelectLine = 1;

	bool OpenMenu = true;

	bool BoxESP = false;
	bool SkeltonESP = false;
	bool DistanceESP = true;
	bool NameESP = true;
	bool HealthESP = false;
	bool SeerESP = true;
	bool SnapLineESP = false;

	bool ESPBack = true;

	float BackColor_trans = 0.78f;
	float BackOutline_think = 2.5f;

	Vector3 BackColor = { 0.24f, 0.51f, 2.38f };
	Vector3 BackOutlineColor = {2.55f,2.55f,2.55f};
	Vector3 BoxColor[2] = { {1.88f,0.07f,0.09f },  {0.46f, 2.44f, 0.33f } };
	Vector3 SkeltonColor[2] = { {1.88f,0.07f,0.09f },  {0.46f, 2.44f, 0.33f } };
	Vector3 GlowColor[2] = { {1.88f,0.07f,0.09f },  {0.46f, 2.44f, 0.33f } };
	Vector3 TextColor = { 2.55f,2.55f,2.55f };

	bool GlowESP = false;

	int ESPMax = 400;
	int SeerEspMax = 100;
	int SkeltonEspMax = 100;


	bool ActiveAimbot = true;
	bool AimbotRcs = true;
	bool DownAim = false;
	bool VisivleCheck = true;
	bool TeamCheck = true;

	int AimKey1 = 1;
	int AimKey2 = 2;
	bool TwoKeyMode = false;

	int TargetMode = 0;

	int Smooth = 12;
	int Fov = 100;
	int AimMax = 300;
	int rcsp = 30;

	bool ShowFov = false;

	bool SpectatorList = true;

	std::vector<std::string> configs;
	const char* config_str[100];

	char savefile[30] = "NewConfig";
}

void setting::ConvertStringVectorToCharArray(const std::vector<std::string>& strVec, const char* charArray[])
{
	for (size_t i = 0; i < strVec.size(); i++)
	{
		if (i > sizeof(charArray))
			break;

		charArray[i] = strVec[i].c_str();
	}
}

std::string setting::GetConfigPath()
{
	TCHAR waFolderPath[MAX_PATH];

	SHGetSpecialFolderPath(NULL, waFolderPath, CSIDL_APPDATA, 0);

	return std::string(waFolderPath) + es("\\Gremlins\\");
}

std::vector<std::string> setting::GetConfigs()
{
	std::vector<std::string> files;

	if(!std::filesystem::exists(GetConfigPath()))
		return  files;

	for (const auto& file : std::filesystem::directory_iterator(GetConfigPath()))
	{
		if(file.path().filename().string().find(es(".grm")) != std::string::npos)
			files.push_back(file.path().filename().string().replace(file.path().filename().string().find(es(".grm")), 4, es("")));
	}

	return  files;
}

bool setting::SaveConfig(std::string filename)
{
	if (filename.empty())
		return false;

	if (!std::filesystem::exists(setting::GetConfigPath()))
	{
		std::filesystem::create_directory(setting::GetConfigPath());
	}

	json configs;

	configs[es("BoxESP")] = BoxESP;
	configs[es("SkeltonESP")] = SkeltonESP;
	configs[es("DistanceESP")] = DistanceESP;
	configs[es("NameESP")] = NameESP;
	configs[es("HealthESP")] = HealthESP;
	configs[es("SeerESP")] = SeerESP;
	configs[es("SnapLineESP")] = SnapLineESP;
	configs[es("ESPBack")] = ESPBack;

	configs[es("BackColor-Trans")] = BackColor_trans;
	configs[es("BackOutline-Think")] = BackOutline_think;

	configs[es("BackColor[X]")] = BackColor.x;
	configs[es("BackColor[Y]")] = BackColor.y;
	configs[es("BackColor[Z]")] = BackColor.z;

	configs[es("BackOutlineColor[X]")] = BackOutlineColor.x;
	configs[es("BackOutlineColor[Y]")] = BackOutlineColor.y;
	configs[es("BackOutlineColor[Z]")] = BackOutlineColor.z;

	configs[es("BoxColor-Visible[X]")] = BoxColor[0].x;
	configs[es("BoxColor-Visible[Y]")] = BoxColor[0].y;
	configs[es("BoxColor-Visible[Z]")] = BoxColor[0].z;

	configs[es("BoxColor-InVisible[X]")] = BoxColor[1].x;
	configs[es("BoxColor-InVisible[Y]")] = BoxColor[1].y;
	configs[es("BoxColor-InVisible[Z]")] = BoxColor[1].z;

	configs[es("SkeltonColor-Visible[X]")] = SkeltonColor[0].x;
	configs[es("SkeltonColor-Visible[Y]")] = SkeltonColor[0].y;
	configs[es("SkeltonColor-Visible[Z]")] = SkeltonColor[0].z;

	configs[es("SkeltonColor-InVisible[X]")] = SkeltonColor[1].x;
	configs[es("SkeltonColor-InVisible[Y]")] = SkeltonColor[1].y;
	configs[es("SkeltonColor-InVisible[Z]")] = SkeltonColor[1].z;

	configs[es("GlowColor-Visible[X]")] = GlowColor[0].x;
	configs[es("GlowColor-Visible[Y]")] = GlowColor[0].y;
	configs[es("GlowColor-Visible[Z]")] = GlowColor[0].z;

	configs[es("GlowColor-InVisible[X]")] = GlowColor[1].x;
	configs[es("GlowColor-InVisible[Y]")] = GlowColor[1].y;
	configs[es("GlowColor-InVisible[Z]")] = GlowColor[1].z;

	configs[es("TextColor[X]")] = TextColor.x;
	configs[es("TextColor[Y]")] = TextColor.y;
	configs[es("TextColor[Z]")] = TextColor.z;

	configs[es("GlowESP")] = GlowESP;

	configs[es("ESPMax")] = ESPMax;
	configs[es("SeerEspMax")] = SeerEspMax;
	configs[es("SkeltonEspMax")] = SkeltonEspMax;


	configs[es("ActiveAimbot")] = ActiveAimbot;
	configs[es("AimbotRcs")] = AimbotRcs;
	configs[es("DownAim")] = DownAim;
	configs[es("VisivleCheck")] = VisivleCheck;
	configs[es("TeamCheck")] = TeamCheck;

	configs[es("AimKey1")] = AimKey1;
	configs[es("AimKey2")] = AimKey2;
	configs[es("TwoKeyMode")] = TwoKeyMode;

	configs[es("TargetMode")] = TargetMode;

	configs[es("Smooth")] = Smooth;
	configs[es("Fov")] = Fov;
	configs[es("AimMax")] = AimMax;
	configs[es("rcsp")] = rcsp;

	configs[es("ShowFov")] = ShowFov;
	configs[es("SpectatorList")] = SpectatorList;

	std::ofstream output(GetConfigPath() + filename + es(".grm"));

	output << std::setw(4) << configs << std::endl;
	output.close();

	return true;
}

bool setting::LoadConfig(std::string filename)
{
	std::ifstream inputs(GetConfigPath() + filename + es(".grm"));
	if (inputs.fail())
	{
		return false;
	}

	json loadedSettings;
	inputs >> loadedSettings;
	inputs.close();



	if (loadedSettings.find(es("BoxESP")) != loadedSettings.end())
		BoxESP = loadedSettings[es("BoxESP")];
	if (loadedSettings.find(es("SkeltonESP")) != loadedSettings.end())
		SkeltonESP = loadedSettings[es("SkeltonESP")];
	if (loadedSettings.find(es("DistanceESP")) != loadedSettings.end())
		DistanceESP = loadedSettings[es("DistanceESP")];
	if (loadedSettings.find(es("NameESP")) != loadedSettings.end())
		NameESP = loadedSettings[es("NameESP")];
	if (loadedSettings.find(es("HealthESP")) != loadedSettings.end())
		HealthESP = loadedSettings[es("HealthESP")];
	if (loadedSettings.find(es("SeerESP")) != loadedSettings.end())
		SeerESP = loadedSettings[es("SeerESP")];
	if (loadedSettings.find(es("SnapLineESP")) != loadedSettings.end())
		SnapLineESP = loadedSettings[es("SnapLineESP")];
	if (loadedSettings.find(es("ESPBack")) != loadedSettings.end())
		ESPBack = loadedSettings[es("ESPBack")];

	if (loadedSettings.find(es("BackColor-Trans")) != loadedSettings.end())
		BackColor_trans = loadedSettings[es("BackColor-Trans")];
	if (loadedSettings.find(es("BackOutline-Think")) != loadedSettings.end())
		BackOutline_think = loadedSettings[es("BackOutline-Think")];

	if (loadedSettings.find(es("BackColor[X]")) != loadedSettings.end())
		BackColor.x = loadedSettings[es("BackColor[X]")];
	if (loadedSettings.find(es("BackColor[Y]")) != loadedSettings.end())
		BackColor.y = loadedSettings[es("BackColor[Y]")];
	if (loadedSettings.find(es("BackColor[Z]")) != loadedSettings.end())
		BackColor.z = loadedSettings[es("BackColor[Z]")];

	if (loadedSettings.find(es("BackOutlineColor[X]")) != loadedSettings.end())
		BackOutlineColor.x = loadedSettings[es("BackOutlineColor[X]")];
	if (loadedSettings.find(es("BackOutlineColor[Y]")) != loadedSettings.end())
		BackOutlineColor.y = loadedSettings[es("BackOutlineColor[Y]")];
	if (loadedSettings.find(es("BackOutlineColor[Z]")) != loadedSettings.end())
		BackOutlineColor.z = loadedSettings[es("BackOutlineColor[Z]")];

	if (loadedSettings.find(es("BoxColor-Visible[X]")) != loadedSettings.end())
		BoxColor[0].x = loadedSettings[es("BoxColor-Visible[X]")];
	if (loadedSettings.find(es("BoxColor-Visible[Y]")) != loadedSettings.end())
		BoxColor[0].y = loadedSettings[es("BoxColor-Visible[Y]")];
	if (loadedSettings.find(es("BoxColor-Visible[Z]")) != loadedSettings.end())
		BoxColor[0].z = loadedSettings[es("BoxColor-Visible[Z]")];

	if (loadedSettings.find(es("BoxColor-InVisible[X]")) != loadedSettings.end())
		BoxColor[1].x = loadedSettings[es("BoxColor-InVisible[X]")];
	if (loadedSettings.find(es("BoxColor-InVisible[Y]")) != loadedSettings.end())
		BoxColor[1].y = loadedSettings[es("BoxColor-InVisible[Y]")];
	if (loadedSettings.find(es("BoxColor-InVisible[Z]")) != loadedSettings.end())
		BoxColor[1].z = loadedSettings[es("BoxColor-InVisible[Z]")];


	if (loadedSettings.find(es("SkeltonColor-Visible[X]")) != loadedSettings.end())
		SkeltonColor[0].x = loadedSettings[es("SkeltonColor-Visible[X]")];
	if (loadedSettings.find(es("SkeltonColor-Visible[Y]")) != loadedSettings.end())
		SkeltonColor[0].y = loadedSettings[es("SkeltonColor-Visible[Y]")];
	if (loadedSettings.find(es("SkeltonColor-Visible[Z]")) != loadedSettings.end())
		SkeltonColor[0].z = loadedSettings[es("SkeltonColor-Visible[Z]")];

	if (loadedSettings.find(es("SkeltonColor-InVisible[X]")) != loadedSettings.end())
		SkeltonColor[1].x = loadedSettings[es("SkeltonColor-InVisible[X]")];
	if (loadedSettings.find(es("SkeltonColor-InVisible[Y]")) != loadedSettings.end())
		SkeltonColor[1].y = loadedSettings[es("SkeltonColor-InVisible[Y]")];
	if (loadedSettings.find(es("SkeltonColor-InVisible[Z]")) != loadedSettings.end())
		SkeltonColor[1].z = loadedSettings[es("SkeltonColor-InVisible[Z]")];


	if (loadedSettings.find(es("GlowColor-Visible[X]")) != loadedSettings.end())
		GlowColor[0].x = loadedSettings[es("GlowColor-Visible[X]")];
	if (loadedSettings.find(es("GlowColor-Visible[Y]")) != loadedSettings.end())
		GlowColor[0].y = loadedSettings[es("GlowColor-Visible[Y]")];
	if (loadedSettings.find(es("GlowColor-Visible[Z]")) != loadedSettings.end())
		GlowColor[0].z = loadedSettings[es("GlowColor-Visible[Z]")];

	if (loadedSettings.find(es("GlowColor-InVisible[X]")) != loadedSettings.end())
		GlowColor[1].x = loadedSettings[es("GlowColor-InVisible[X]")];
	if (loadedSettings.find(es("GlowColor-InVisible[Y]")) != loadedSettings.end())
		GlowColor[1].y = loadedSettings[es("GlowColor-InVisible[Y]")];
	if (loadedSettings.find(es("GlowColor-InVisible[Z]")) != loadedSettings.end())
		GlowColor[1].z = loadedSettings[es("GlowColor-InVisible[Z]")];

	if (loadedSettings.find(es("TextColor[X]")) != loadedSettings.end())
		TextColor.x = loadedSettings[es("TextColor[X]")];
	if (loadedSettings.find(es("TextColor[Y]")) != loadedSettings.end())
		TextColor.y = loadedSettings[es("TextColor[Y]")];
	if (loadedSettings.find(es("TextColor[Z]")) != loadedSettings.end())
		TextColor.z = loadedSettings[es("TextColor[Z]")];

	if(loadedSettings.find(es("GlowESP")) != loadedSettings.end())
		GlowESP = loadedSettings[es("GlowESP")];

	if (loadedSettings.find(es("ESPMax")) != loadedSettings.end())
		ESPMax = loadedSettings[es("ESPMax")];
	if (loadedSettings.find(es("SeerEspMax")) != loadedSettings.end())
		SeerEspMax = loadedSettings[es("SeerEspMax")];
	if (loadedSettings.find(es("SkeltonEspMax")) != loadedSettings.end())
		SkeltonEspMax = loadedSettings[es("SkeltonEspMax")];


	if (loadedSettings.find(es("ActiveAimbot")) != loadedSettings.end())
		ActiveAimbot = loadedSettings[es("ActiveAimbot")];
	if (loadedSettings.find(es("AimbotRcs")) != loadedSettings.end())
		AimbotRcs = loadedSettings[es("AimbotRcs")];
	if (loadedSettings.find(es("DownAim")) != loadedSettings.end())
		DownAim = loadedSettings[es("DownAim")];
	if (loadedSettings.find(es("TeamCheck")) != loadedSettings.end())
		TeamCheck = loadedSettings[es("TeamCheck")];
	if (loadedSettings.find(es("VisivleCheck")) != loadedSettings.end())
		VisivleCheck = loadedSettings[es("VisivleCheck")];


	if (loadedSettings.find(es("AimKey1")) != loadedSettings.end())
		AimKey1 = loadedSettings[es("AimKey1")];
	if (loadedSettings.find(es("AimKey2")) != loadedSettings.end())
		AimKey2 = loadedSettings[es("AimKey2")];
	if (loadedSettings.find(es("TwoKeyMode")) != loadedSettings.end())
		TwoKeyMode = loadedSettings[es("TwoKeyMode")];

	if (loadedSettings.find(es("TargetMode")) != loadedSettings.end())
		TargetMode = loadedSettings[es("TargetMode")];

	if (loadedSettings.find(es("Smooth")) != loadedSettings.end())
		Smooth = loadedSettings[es("Smooth")];
	if (loadedSettings.find(es("Fov")) != loadedSettings.end())
		Fov = loadedSettings[es("Fov")];
	if (loadedSettings.find(es("AimMax")) != loadedSettings.end())
		AimMax = loadedSettings[es("AimMax")];
	if (loadedSettings.find(es("rcsp")) != loadedSettings.end())
		rcsp = loadedSettings[es("rcsp")];

	if (loadedSettings.find(es("ShowFov")) != loadedSettings.end())
		ShowFov = loadedSettings[es("ShowFov")];
	if (loadedSettings.find(es("SpectatorList")) != loadedSettings.end())
		SpectatorList = loadedSettings[es("SpectatorList")];

	return true;
}