#include <cassert>
#include <mutex>

#include "driver.h"

#include "interface.h"
#include "entity.h"
#include "aimbot.h"
#include "overlay.h"
// #include "auth.hpp"

#include "antidebug.h"
#include "protect.h"
#include "Artemis.h"
#include "utils.h"

// using namespace KeyAuth;

typedef NTSTATUS(NTAPI* TFNRtlAdjustPrivilege)(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN Enabled);

typedef NTSTATUS(NTAPI* TFNNtRaiseHardError)(NTSTATUS ErrorStatus, ULONG NumberOfParameters,
	ULONG UnicodeStringParameterMask, PULONG_PTR* Parameters, ULONG ValidResponseOption, PULONG Response);

//std::string name = es("Apex Public"); // application name. right above the blurred text aka the secret on the licenses tab among other tabs
//std::string ownerid = es("6Rolek0rSb"); // ownerid, found in account settings. click your profile picture on top right of dashboard and then account settings.
//std::string secret = es("fb65b4ffa47288a944e753849790841e5892d797a9ab9757035eb1eed2f8575f"); // app secret, the blurred text on licenses tab and other tabs
//std::string version = es("1.0"); // leave alone unless you've changed version on website
//std::string url = es("https://keyauth.win/api/1.2/"); // change if you're self-hosting

//api KeyAuthApp(name, ownerid, secret, version, url);


MatrixA ViewMatrix{};
std::vector<entity_t> PlayerEntityList;

std::mutex ent_mtx;

entity_t plocal(NULL);

void GetPlayerList()
{
	std::vector<entity_t> Players = std::vector<entity_t>();
	int OldUpdateTime = 99999;
	bool spflag = true;

	while (true)
	{
		//if (utils::GetUnixTimestamp() - OldUpdateTime < 3)
		//{
			utils::SpectatorName.clear();
			utils::SpectatorTime.clear();
			//spflag = true;
		//}
		//else
		//{
			//spflag = false;
		//}

		plocal = entity_t::get_local_player();
		ViewMatrix = utils::Get_ViewMatrix();

		int index = 0;
		std::unique_lock lock(ent_mtx);

		for (int i = 0; i < 64; i++)
		{
			uintptr_t ent = entity_t::get_client_entity(i);

			if (!ent)
				continue;

			auto Entity = entity_t(ent);

			if (Entity.ptr == plocal.ptr)
				continue;

			//if (spflag)
			//{
			if (Entity.is_spectator(plocal.angle_yaw_v))
				utils::SpectatorName.push_back(Entity.pname);
			//OldUpdateTime = utils::GetUnixTimestamp();
			//utils::SpectatorTime.push_back(utils::GetUnixTimestamp());
	//}

			if (!Entity.is_player() || !Entity.is_valid(plocal))
				continue;

			Entity.is_visible_v = Entity.is_visible(index);
			index++;

			Players.push_back(Entity);
		}

		PlayerEntityList = Players;

		Players.clear();

		//std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}


void DrawESP()
{
	if (plocal.ptr != NULL)
	{
		if (setting::ShowFov)
		{
			hDrawCircle(Globals::windowWH.x / 2, Globals::windowWH.y / 2, setting::Fov, 64.f, 0.4f, Vector4(2.5, 2.5, 2.5, 2.5));
		}

		//printf("local_player %x\n", local_player.ptr);

		//entity_t aimtarget(NULL);
		//float lowestAngleChange = FLT_MAX;
		int index = 0, rhitboxpos = 0;

		ImVec2 gradient_p0, gradient_p1;

		Vector3 screenLoc{}, screenLoc2{};

		std::unique_lock lock(ent_mtx);

		for (entity_t& entity : PlayerEntityList)
		{
			float actualDistance = utils::DeltaDistance(plocal.origin_v, entity.origin_v);

			int DistanceMeters = (int)ceil(actualDistance * 0.01905f);

			float HealthPercent = utils::GetHealthPercent(entity.health_v);
			float ShieldPercent = utils::GetShieldPercent(entity.shield_health_v);

			Vector3 topPos = Vector3(entity.origin_v.x, entity.origin_v.y, entity.origin_v.z + 75);

			Vector3 BoxColor = {};
			//Vector4 BoxColor2 = {};
			Vector3 SKeltonColor = {};

			if (entity.is_visible_v)
			{
				BoxColor = setting::BoxColor[0];
				//BoxColor2 = { 1.51961, 1.24118, 0.2, 1 };
				SKeltonColor = setting::SkeltonColor[0];
			}
			else
			{
				BoxColor = setting::BoxColor[1];
				//BoxColor2 = { 0.71961, 1.54118, 0.39216, 1 };
				SKeltonColor = setting::SkeltonColor[1];
			}

			if (!utils::WorldToScreen(ViewMatrix, entity.origin_v, screenLoc))
				continue;

			if (!utils::WorldToScreen(ViewMatrix, topPos, screenLoc2))
				continue;

			float heightDiff = screenLoc.y - screenLoc2.y;

			if (DistanceMeters < setting::ESPMax)
			{
				if (setting::BoxESP)
				{
					gradient_p0 = ImVec2(screenLoc.x - heightDiff / 4, screenLoc.y);
					gradient_p1 = ImVec2(screenLoc.x + heightDiff / 4, screenLoc.y - heightDiff);

					//const int vert_start_idx = ImGui::GetOverlayDrawList()->VtxBuffer.Size;
					//hDrawRect(ImVec2(gradient_p0.x - 2, gradient_p0.y - 2), ImVec2(gradient_p1.x - 2, gradient_p1.y - 2), Vector4(1, 1, 1, 1), 0, 2);
					//const int vert_end_idx = ImGui::GetOverlayDrawList()->VtxBuffer.Size;
					//ImGui::ShadeVertsLinearColorGradientKeepAlpha(ImGui::GetForegroundDrawList(), vert_start_idx, vert_end_idx, ImVec2(gradient_p0.x, gradient_p0.y), ImVec2(gradient_p1.x, gradient_p1.y),
					//	ImGui::GetColorU32({ BoxColor1 }), ImGui::GetColorU32({ BoxColor2 })); // YOUR_COLOR use  IM_COL32(R,G,B,A)

					hDrawRect(ImVec2(gradient_p0.x + 0.8, gradient_p0.y + 0.8), ImVec2(gradient_p1.x + 0.8, gradient_p1.y - 0.8), { BoxColor.x,BoxColor.y,BoxColor.z,2.55 }, 0, 15,1);
					hDrawRect(ImVec2(gradient_p0.x + 0.1, gradient_p0.y + 0.1), ImVec2(gradient_p1.x + 0.1, gradient_p1.y - 0.1), { BoxColor.x,BoxColor.y,BoxColor.z,2.55 }, 0, 15, 1);

					//hDrawRectFilled(ImVec2(gradient_p0.x - 2, gradient_p0.y - 2), ImVec2(gradient_p1.x - 2, gradient_p1.y + 2), BoxColor1, 0, 1);
				}

				if (setting::HealthESP)
				{
					gradient_p0 = ImVec2((screenLoc.x - heightDiff / 2), screenLoc.y - heightDiff - 5);
					gradient_p1 = ImVec2((screenLoc.x - heightDiff / 2 + 60 * HealthPercent), screenLoc.y - heightDiff - 7);
					const int vert_start_idx = ImGui::GetOverlayDrawList()->VtxBuffer.Size;
					hDrawRectFilled(gradient_p0, gradient_p1, Vector4(1, 1, 1, 1), 0, 1);
					const int vert_end_idx = ImGui::GetOverlayDrawList()->VtxBuffer.Size;
					ImGui::ShadeVertsLinearColorGradientKeepAlpha(ImGui::GetOverlayDrawList(), vert_start_idx, vert_end_idx, gradient_p0, gradient_p1, ImGui::GetColorU32({ 0.0f, 1.68f, 1.38f, 2.55f }), ImGui::GetColorU32({ 1.96, 2.53f, 0.0f, 2.55f })); // YOUR_COLOR use  IM_COL32(R,G,B,A)
					hDrawRect(ImVec2(gradient_p0.x, gradient_p0.y), ImVec2(screenLoc.x - heightDiff / 4 + 60, gradient_p1.y), Vector4(0.0f, 0.0f, 0.0f, 1.0f), 0,15, 1);

					gradient_p0 = ImVec2((screenLoc.x - heightDiff / 2), screenLoc.y - heightDiff - 10);
					gradient_p1 = ImVec2((screenLoc.x - heightDiff / 2 + 60 * ShieldPercent), screenLoc.y - heightDiff - 12);
					const int vert_start_idx2 = ImGui::GetOverlayDrawList()->VtxBuffer.Size;
					hDrawRectFilled(gradient_p0, gradient_p1, Vector4(1, 1, 1, 1), 0, 1);
					const int vert_end_idx2 = ImGui::GetOverlayDrawList()->VtxBuffer.Size;
					ImGui::ShadeVertsLinearColorGradientKeepAlpha(ImGui::GetOverlayDrawList(), vert_start_idx2, vert_end_idx2, gradient_p0, gradient_p1, ImGui::GetColorU32({ 0.53f, 0.0f, 0.8f, 2.55f }), ImGui::GetColorU32({ 0.93f, 0.1f, 2.15f, 2.55f })); // YOUR_COLOR use  IM_COL32(R,G,B,A) 
					hDrawRect(ImVec2(gradient_p0.x, gradient_p0.y), ImVec2(screenLoc.x - heightDiff / 4 + 60, gradient_p1.y), Vector4(0.0f, 0.0f, 0.0f, 1.0f), 0,15, 1);
				}

				if (setting::SeerESP && DistanceMeters < setting::SeerEspMax)
				{
					gradient_p0 = ImVec2((screenLoc.x - heightDiff / 2), screenLoc.y - heightDiff - 10);
					hDrawSeerLikeHealth(gradient_p0.x + 85, gradient_p0.y - 10, entity.shield_health_v, entity.shield_health_max_v, entity.shield_type_v, entity.health_v);
				}

				if (setting::ESPBack)
				{
					gradient_p0 = ImVec2((screenLoc.x - heightDiff / 2), screenLoc.y - heightDiff - 40);
					hDrawRectFilled(ImVec2(gradient_p0.x, gradient_p0.y - 43), ImVec2(screenLoc2.x + 11 * entity.pname.length(), gradient_p0.y - 8), Vector4(setting::BackColor.x, setting::BackColor.y, setting::BackColor.z, setting::BackColor_trans), 6.f, 15.f);
					hDrawRect(ImVec2(gradient_p0.x, gradient_p0.y - 43), ImVec2(screenLoc2.x + 11 * entity.pname.length(), gradient_p0.y - 8), Vector4(setting::BackOutlineColor.x, setting::BackOutlineColor.y, setting::BackOutlineColor.z, 1.0f), 6.f, 15.f,setting::BackOutline_think);
				}

				if (setting::NameESP)
				{
					gradient_p0 = ImVec2((screenLoc.x - heightDiff / 2), screenLoc.y - heightDiff - 40);
					hDrawText(ImVec2(gradient_p0.x + 6, gradient_p0.y - 25), entity.pname.c_str(), 13, Vector4(setting::TextColor.x, setting::TextColor.y, setting::TextColor.z, 2.55f));
				}

				if (setting::DistanceESP)
				{
					gradient_p0 = ImVec2((screenLoc.x - heightDiff / 2), screenLoc.y - heightDiff - 40);
					hDrawText(ImVec2(gradient_p0.x + 6, gradient_p0.y - 40), std::string(std::to_string(DistanceMeters) + es(" M")).c_str(), 13, Vector4(setting::TextColor.x, setting::TextColor.y, setting::TextColor.z, 2.55f));
				}

				if (setting::SnapLineESP)
				{
					hDrawLine(ImVec2(screenLoc.x, screenLoc.y), ImVec2(Globals::windowWH.x / 2, Globals::windowWH.y), { SKeltonColor.x,SKeltonColor.y,SKeltonColor.z,2.55f}, 4);
				}


				if (setting::GlowESP)
				{
					if (entity.is_visible_v)
						entity.set_glow(setting::GlowColor[0]);
					else
						entity.set_glow(setting::GlowColor[1]);
				}

				if (setting::SkeltonESP&& DistanceMeters < setting::SkeltonEspMax)
				{
					if (!utils::WorldToScreen(ViewMatrix, entity.hitbox.ScreenHeadBone, entity.hitbox.ScreenHeadBone)) return;
					if (!utils::WorldToScreen(ViewMatrix, entity.hitbox.ScreenNeckBone, entity.hitbox.ScreenNeckBone))  return;
					if (!utils::WorldToScreen(ViewMatrix, entity.hitbox.ScreenChestBone, entity.hitbox.ScreenChestBone))  return;
					if (!utils::WorldToScreen(ViewMatrix, entity.hitbox.ScreenWaistBone, entity.hitbox.ScreenWaistBone))  return;
					if (!utils::WorldToScreen(ViewMatrix, entity.hitbox.ScreenBotmBone, entity.hitbox.ScreenBotmBone))  return;
					if (!utils::WorldToScreen(ViewMatrix, entity.hitbox.ScreenLeftshoulderBone, entity.hitbox.ScreenLeftshoulderBone))  return;
					if (!utils::WorldToScreen(ViewMatrix, entity.hitbox.ScreenLeftelbowBone, entity.hitbox.ScreenLeftelbowBone))  return;
					if (!utils::WorldToScreen(ViewMatrix, entity.hitbox.ScreenLeftHandBone, entity.hitbox.ScreenLeftHandBone))  return;
					if (!utils::WorldToScreen(ViewMatrix, entity.hitbox.ScreenRightshoulderBone, entity.hitbox.ScreenRightshoulderBone))  return;
					if (!utils::WorldToScreen(ViewMatrix, entity.hitbox.ScreenRightelbowBone, entity.hitbox.ScreenRightelbowBone))  return;
					if (!utils::WorldToScreen(ViewMatrix, entity.hitbox.ScreenRightHandBone, entity.hitbox.ScreenRightHandBone))  return;
					if (!utils::WorldToScreen(ViewMatrix, entity.hitbox.ScreenLeftThighsBone, entity.hitbox.ScreenLeftThighsBone))  return;
					if (!utils::WorldToScreen(ViewMatrix, entity.hitbox.ScreenLeftkneesBone, entity.hitbox.ScreenLeftkneesBone))  return;
					if (!utils::WorldToScreen(ViewMatrix, entity.hitbox.ScreenLeftlegBone, entity.hitbox.ScreenLeftlegBone))  return;
					if (!utils::WorldToScreen(ViewMatrix, entity.hitbox.ScreenRightThighsBone, entity.hitbox.ScreenRightThighsBone))  return;
					if (!utils::WorldToScreen(ViewMatrix, entity.hitbox.ScreenRightkneesBone, entity.hitbox.ScreenRightkneesBone))  return;
					if (!utils::WorldToScreen(ViewMatrix, entity.hitbox.ScreenRightlegBone, entity.hitbox.ScreenRightlegBone))  return;

					hDrawLine(ImVec2(entity.hitbox.ScreenHeadBone.x, entity.hitbox.ScreenHeadBone.y), ImVec2(entity.hitbox.ScreenNeckBone.x, entity.hitbox.ScreenNeckBone.y), { SKeltonColor.x,SKeltonColor.y,SKeltonColor.z,2.55 }, 4);
					hDrawLine(ImVec2(entity.hitbox.ScreenNeckBone.x, entity.hitbox.ScreenNeckBone.y), ImVec2(entity.hitbox.ScreenChestBone.x, entity.hitbox.ScreenChestBone.y), { SKeltonColor.x,SKeltonColor.y,SKeltonColor.z,2.55 }, 4);
					hDrawLine(ImVec2(entity.hitbox.ScreenChestBone.x, entity.hitbox.ScreenChestBone.y), ImVec2(entity.hitbox.ScreenWaistBone.x, entity.hitbox.ScreenWaistBone.y), { SKeltonColor.x,SKeltonColor.y,SKeltonColor.z,2.55 }, 4);
					hDrawLine(ImVec2(entity.hitbox.ScreenWaistBone.x, entity.hitbox.ScreenWaistBone.y), ImVec2(entity.hitbox.ScreenBotmBone.x, entity.hitbox.ScreenBotmBone.y), { SKeltonColor.x,SKeltonColor.y,SKeltonColor.z,2.55 }, 4);

					hDrawLine(ImVec2(entity.hitbox.ScreenNeckBone.x, entity.hitbox.ScreenNeckBone.y), ImVec2(entity.hitbox.ScreenLeftshoulderBone.x, entity.hitbox.ScreenLeftshoulderBone.y), { SKeltonColor.x,SKeltonColor.y,SKeltonColor.z,2.55 }, 4);
					hDrawLine(ImVec2(entity.hitbox.ScreenLeftshoulderBone.x, entity.hitbox.ScreenLeftshoulderBone.y), ImVec2(entity.hitbox.ScreenLeftelbowBone.x, entity.hitbox.ScreenLeftelbowBone.y), { SKeltonColor.x,SKeltonColor.y,SKeltonColor.z,2.55 }, 4);
					hDrawLine(ImVec2(entity.hitbox.ScreenLeftelbowBone.x, entity.hitbox.ScreenLeftelbowBone.y), ImVec2(entity.hitbox.ScreenLeftHandBone.x, entity.hitbox.ScreenLeftHandBone.y), { SKeltonColor.x,SKeltonColor.y,SKeltonColor.z,2.55 }, 4);

					hDrawLine(ImVec2(entity.hitbox.ScreenNeckBone.x, entity.hitbox.ScreenNeckBone.y), ImVec2(entity.hitbox.ScreenRightshoulderBone.x, entity.hitbox.ScreenRightshoulderBone.y), { SKeltonColor.x,SKeltonColor.y,SKeltonColor.z,2.55 }, 4);
					hDrawLine(ImVec2(entity.hitbox.ScreenRightshoulderBone.x, entity.hitbox.ScreenRightshoulderBone.y), ImVec2(entity.hitbox.ScreenRightelbowBone.x, entity.hitbox.ScreenRightelbowBone.y), { SKeltonColor.x,SKeltonColor.y,SKeltonColor.z,2.55 }, 4);
					hDrawLine(ImVec2(entity.hitbox.ScreenRightelbowBone.x, entity.hitbox.ScreenRightelbowBone.y), ImVec2(entity.hitbox.ScreenRightHandBone.x, entity.hitbox.ScreenRightHandBone.y), { SKeltonColor.x,SKeltonColor.y,SKeltonColor.z,2.55 }, 4);

					hDrawLine(ImVec2(entity.hitbox.ScreenBotmBone.x, entity.hitbox.ScreenBotmBone.y), ImVec2(entity.hitbox.ScreenLeftThighsBone.x, entity.hitbox.ScreenLeftThighsBone.y), { SKeltonColor.x,SKeltonColor.y,SKeltonColor.z,2.55 }, 4);
					hDrawLine(ImVec2(entity.hitbox.ScreenLeftThighsBone.x, entity.hitbox.ScreenLeftThighsBone.y), ImVec2(entity.hitbox.ScreenLeftkneesBone.x, entity.hitbox.ScreenLeftkneesBone.y), { SKeltonColor.x,SKeltonColor.y,SKeltonColor.z,2.55 }, 4);
					hDrawLine(ImVec2(entity.hitbox.ScreenLeftkneesBone.x, entity.hitbox.ScreenLeftkneesBone.y), ImVec2(entity.hitbox.ScreenLeftlegBone.x, entity.hitbox.ScreenLeftlegBone.y), { SKeltonColor.x,SKeltonColor.y,SKeltonColor.z,2.55 }, 4);

					hDrawLine(ImVec2(entity.hitbox.ScreenBotmBone.x, entity.hitbox.ScreenBotmBone.y), ImVec2(entity.hitbox.ScreenRightThighsBone.x, entity.hitbox.ScreenRightThighsBone.y), { SKeltonColor.x,SKeltonColor.y,SKeltonColor.z,2.55 }, 4);
					hDrawLine(ImVec2(entity.hitbox.ScreenRightThighsBone.x, entity.hitbox.ScreenRightThighsBone.y), ImVec2(entity.hitbox.ScreenRightkneesBone.x, entity.hitbox.ScreenRightkneesBone.y), { SKeltonColor.x,SKeltonColor.y,SKeltonColor.z,2.55 }, 4);
					hDrawLine(ImVec2(entity.hitbox.ScreenRightkneesBone.x, entity.hitbox.ScreenRightkneesBone.y), ImVec2(entity.hitbox.ScreenRightlegBone.x, entity.hitbox.ScreenRightlegBone.y), { SKeltonColor.x,SKeltonColor.y,SKeltonColor.z,2.55 }, 4);
				}
			}
		}


		if (setting::SpectatorList)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.75f);

			ImGui::SetNextWindowSize(ImVec2(250, 100), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowPos(ImVec2(1600, 30), ImGuiCond_FirstUseEver);

			ImGui::Begin(("Spectator"), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoTitleBar);
			{
				for (int i = 0; i < utils::SpectatorName.size(); i++)
				{
					ImGui::SetCursorPosX(5);
					ImGui::Text(utils::SpectatorName[i].c_str());
				}
			}
			ImGui::End();

			ImGui::PopStyleVar();
		}

		//ImGui::Begin(es("Spectator"));
		//{
		//	for (std::string& spec : utils::SpectatorName)
		//	{
		//		ImGui::Text(spec.c_str());
		//	}
		//}
	}

	//printf("%d\n", rtcore.rpm<int>(plocal + 0x043c));

	//Sleep(10);
}

void Aimbot()
{
	while (true)
	{
		if (setting::ActiveAimbot)
		{
			std::unique_lock lock(ent_mtx);

			if (plocal.ptr != NULL)
			{
				entity_t aimtarget(NULL);
				float lowestAngleChange = FLT_MAX;
				int index = 0, rhitboxpos = NULL, snum = 0,max = 1;

				if (setting::TargetMode == 1)
				{
					snum = 0;
					max = 1;
				}
				else if (setting::TargetMode == 2)
				{
					snum = 1;
					max = 2;
				}
				else
				{
					snum = 0;
					max = 2;
				}

				for (entity_t& entity : PlayerEntityList)
				{
					float actualDistance = utils::DeltaDistance(plocal.origin_v, entity.origin_v);

					int DistanceMeters = (int)ceil(actualDistance * 0.01905f);

					if(DistanceMeters < setting::AimMax && (GetAsyncKeyState(setting::AimKey1) || (GetAsyncKeyState(setting::AimKey2) && setting::TwoKeyMode)))
					{
						float bonefov;
						Vector3 screenent;

						for (int num = snum; num < max; num++) {
							if (utils::WorldToScreen(ViewMatrix, entity.get_bone_position(utils::bones[num]), screenent)) {
								bonefov = utils::GetFov(utils::WndSize / 2.f, screenent);

								if (bonefov < setting::Fov && bonefov < lowestAngleChange)
								{
									if (!setting::DownAim && entity.is_down()) continue;

									if (setting::VisivleCheck && !entity.is_visible_v) continue;

									rhitboxpos = utils::bones[num];
									aimtarget = entity;
									lowestAngleChange = bonefov;
								}
							}
						}
					}
				}

				if (rhitboxpos != NULL)
				{
					aimbot::init(plocal, aimtarget, rhitboxpos);
				}
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void Menu()
{
	Overlay::InputHandler();

	if (setting::OpenMenu)
		SetWindowLongA(Globals::hWnd, GWL_EXSTYLE, (WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT));
	else
		SetWindowLongA(Globals::hWnd, GWL_EXSTYLE, (WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT));

	if (setting::OpenMenu)
	{
		ImGui::SetNextWindowSize(ImVec2(150, 600));

		ImGui::SetNextWindowPos(ImVec2(utils::MainWindowPos.x - 205, utils::MainWindowPos.y));

		ImGui::Begin(es("Tab"), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
		{
			//static ImVec4 active = { 44, 15, 15, 255 };
			//static ImVec4 inactive = { 12, 13, 13, 255 };

			ImGui::Spacing();

			ImGui::SetCursorPosX(10);

			//ImGui::PushStyleColor(ImGuiCol_Button, utils::MenuTab == 1 ? active : inactive);
			if (ImGui::Button(es("ESP"), ImVec2(130, 35)))
			{
				utils::MenuTab = 1;
			}

			ImGui::Spacing();

			ImGui::SetCursorPosX(10);

			//ImGui::PushStyleColor(ImGuiCol_Button, utils::MenuTab == 2 ? active : inactive);
			if (ImGui::Button(es("AIM"), ImVec2(130, 35)))
			{
				utils::MenuTab = 2;
			}

			ImGui::Spacing();

			ImGui::SetCursorPosX(10);
			//ImGui::PushStyleColor(ImGuiCol_Button, utils::MenuTab == 3 ? active : inactive);
			if (ImGui::Button(es("COLOR"), ImVec2(130, 35)))
			{
				utils::MenuTab = 3;
			}

			ImGui::Spacing();

			ImGui::SetCursorPosX(10);
			//ImGui::PushStyleColor(ImGuiCol_Button, utils::MenuTab == 4 ? active : inactive);
			if (ImGui::Button(es("OTHER"), ImVec2(130, 35)))
			{
				utils::MenuTab = 4;
			}

			ImGui::Spacing();

			ImGui::SetCursorPosX(10);
			//ImGui::PushStyleColor(ImGuiCol_Button, utils::MenuTab == 5 ? active : inactive);
			if (ImGui::Button(es("CONFIG"), ImVec2(130, 35)))
			{
				utils::MenuTab = 5;
			}

			//ImGui::PopStyleColor(5);
		}
		ImGui::End;

		ImGui::SetNextWindowSize(ImVec2(500, 600));
		ImGui::SetNextWindowPos(ImVec2(300, 100), ImGuiCond_FirstUseEver);

		ImGui::Begin(es("Gremlins Public"), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
		{
			utils::MainWindowPos =	ImGui::GetWindowPos();
			//ImGui::Columns(2);

			//ImGui::SetColumnOffset(1, 230);
			//{
			//	ImGui::SetCursorPosX(2.5);
			//	ImGui::Button(es("ESP"), ImVec2(190, 70));
			//	//ImGui::EndChild();
			//}

			ImGui::Columns(1, NULL, false);
			{
				ImGui::BeginChild(es("COL"), ImVec2(0, 0), false);
				{
					if(utils::MenuTab == 1)
					{
						ImGui::Spacing();

						ImGui::Checkbox(es("Box ESP"), &setting::BoxESP);
						ImGui::Checkbox(es("Skelton ESP"), &setting::SkeltonESP);
						ImGui::Checkbox(es("Distance ESP"), &setting::DistanceESP);
						ImGui::Checkbox(es("Name ESP"), &setting::NameESP);
						ImGui::Checkbox(es("Health ESP"), &setting::HealthESP);
						ImGui::Checkbox(es("Seer ESP"), &setting::SeerESP);
						ImGui::Checkbox(es("Snap Line ESP"), &setting::SnapLineESP);

						ImGui::Spacing();

						ImGui::Checkbox(es("Glow ESP"), &setting::GlowESP);

						ImGui::Spacing();

						ImGui::Checkbox(es("ESP Back"), &setting::ESPBack);

						ImGui::SetCursorPosX(10);
						ImGui::Text(es("ESP Back Transparency"));
						ImGui::SetCursorPosX(7);
						ImGui::SliderFloat(es("##ESP Back Transparency"), &setting::BackColor_trans, 0.f, 1.f, es("%f"));

						ImGui::SetCursorPosX(10);
						ImGui::Text(es("ESP Back Outline Thickness"));
						ImGui::SetCursorPosX(7);
						ImGui::SliderFloat(es("##ESP Back Outline Thickness"), &setting::BackOutline_think, 0.f, 10.f, es("%f"));

						ImGui::Spacing();

						ImGui::SetCursorPosX(10);
						ImGui::Text(es("Max ESP Distance"));
						ImGui::SetCursorPosX(7);
						ImGui::SliderInt(es("##Max ESP Distance"), &setting::ESPMax, 0, 1500, es("%dM"));

						ImGui::SetCursorPosX(10);
						ImGui::Text(es("Max Seer ESP Distance"));
						ImGui::SetCursorPosX(7);
						ImGui::SliderInt(es("##Max Seer ESP Distance"), &setting::SeerEspMax, 0, 1500, es("%dM"));

						ImGui::SetCursorPosX(10);
						ImGui::Text(es("Max Skelton ESP Distance"));
						ImGui::SetCursorPosX(7);
						ImGui::SliderInt(es("##Max Skelton ESP Distance"), &setting::SkeltonEspMax, 0, 1500, es("%dM"));
					}


					if (utils::MenuTab == 2)
					{
						ImGui::Spacing();

						ImGui::Checkbox(es("Aimbot"), &setting::ActiveAimbot);

						ImGui::Checkbox(es("Aimbot Recoil Controle"), &setting::AimbotRcs);

						ImGui::Checkbox(es("Aimbot DownCheck"), &setting::DownAim);

						ImGui::Checkbox(es("Aimbot Visible Check"), &setting::VisivleCheck);

						ImGui::Spacing();

						ImGui::SetCursorPosX(10);
						ImGui::Text(es("Aim Key [1]"));
						ImGui::SetCursorPosX(10);
						ImGui::Combo(es("##Aim Key1"), &setting::AimKey1, keyNames, sizeof(keyNames) / sizeof(*keyNames));


						if (setting::TwoKeyMode)
						{
							ImGui::SetCursorPosX(10);
							ImGui::Text(es("Aim Key [2]"));
							ImGui::SetCursorPosX(10);
							ImGui::Combo(es("##Aim Key2"), &setting::AimKey2, keyNames, sizeof(keyNames) / sizeof(*keyNames));
						}

						ImGui::Spacing();

						ImGui::SetCursorPosX(10);
						ImGui::Text(es("Target Mode"));
						ImGui::SetCursorPosX(10);
						ImGui::Combo(es("##Target Mode"), &setting::TargetMode, TargetModes, sizeof(TargetModes) / sizeof(*TargetModes));

						ImGui::Spacing();

						ImGui::SetCursorPosX(10);
						ImGui::Text(es("AimBot Fov"));
						ImGui::SetCursorPosX(7);
						ImGui::SliderInt(es("##AimBot Fov"), &setting::Fov, 0, 700, es("%d"));
						ImGui::SetCursorPosX(10);
						ImGui::Text(es("AimBot Smooth"));
						ImGui::SetCursorPosX(7);
						ImGui::SliderInt(es("##AimBot Smooth"), &setting::Smooth, 6, 100, es("%d"));
						ImGui::SetCursorPosX(10);
						ImGui::Text(es("Recoil Controle Percentage"));
						ImGui::SetCursorPosX(7);
						ImGui::SliderInt(es("##Recoil Controle Percentage"), &setting::rcsp, 0, 100, es("%d %%"));

						ImGui::SetCursorPosX(10);
						ImGui::Text(es("Max Aimbot Distance"));
						ImGui::SetCursorPosX(7);
						ImGui::SliderInt(es("##Max Aimbot Distance"), &setting::AimMax, 0, 1500, es("%dM"));
						ImGui::Spacing();

						ImGui::Checkbox(es("Double AimKey Mode"), &setting::TwoKeyMode);

						ImGui::Checkbox(es("Show Fov"), &setting::ShowFov);
					}

					if (utils::MenuTab == 3)
					{
						ImGui::Spacing();
						ImGui::SetCursorPosX(40);
						ImGui::Text(es("Back Color"));
						ImGui::SameLine();
						ImGui::SetCursorPosX(250);
						ImGui::Text(es("Text Color"));
						

						ImGui::SetCursorPosX(5);
						ImGui::SetNextItemWidth(100);
						ImGui::ColorPicker3(es("##Back Color"), (float*)&setting::BackColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
						ImGui::SameLine();
						ImGui::SetCursorPosX(215);
						ImGui::SetNextItemWidth(100);
						ImGui::ColorPicker3(es("##Text Color"), (float*)&setting::TextColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);

						ImGui::Spacing();
						ImGui::SetCursorPosX(40);
						ImGui::Text(es("Back Outline Color"));

						ImGui::SetCursorPosX(5);
						ImGui::SetNextItemWidth(100);
						ImGui::ColorPicker3(es("##Back Outline Color"), (float*)&setting::BackOutlineColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);

						ImGui::Spacing();

						ImGui::SetCursorPosX(40);
						ImGui::Text(es("Box Color [visible]"));
						ImGui::SameLine();
						ImGui::SetCursorPosX(250);
						ImGui::Text(es("Box Color [invisible]"));

						ImGui::SetCursorPosX(5);
						ImGui::SetNextItemWidth(100);
						ImGui::ColorPicker3(es("##Box Color Visible"), (float*)&setting::BoxColor[0], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
						ImGui::SameLine();
						ImGui::SetCursorPosX(215);
						ImGui::SetNextItemWidth(100);
						ImGui::ColorPicker3(es("##Box Color Invisible"), (float*)&setting::BoxColor[1], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);


						ImGui::Spacing();

						ImGui::SetCursorPosX(40);
						ImGui::Text(es("Glow Color [visible]"));
						ImGui::SameLine();
						ImGui::SetCursorPosX(250);
						ImGui::Text(es("Glow Color [invisible]"));

						ImGui::SetCursorPosX(5);
						ImGui::SetNextItemWidth(100);
						ImGui::ColorPicker3(es("##Glow Color Visible"), (float*)&setting::GlowColor[0], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
						ImGui::SameLine();
						ImGui::SetCursorPosX(215);
						ImGui::SetNextItemWidth(100);
						ImGui::ColorPicker3(es("##Glow Color Invisible"), (float*)&setting::GlowColor[1], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);

						ImGui::Spacing();

						ImGui::SetCursorPosX(40);
						ImGui::Text(es("Skelton Color [visible]"));
						ImGui::SameLine();
						ImGui::SetCursorPosX(250);
						ImGui::Text(es("Skelton Color [invisible]"));

						ImGui::SetCursorPosX(5);
						ImGui::SetNextItemWidth(100);
						ImGui::ColorPicker3(es("##Skelton Color Visible"), (float*)&setting::SkeltonColor[0], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
						ImGui::SameLine();
						ImGui::SetCursorPosX(215);
						ImGui::SetNextItemWidth(100);
						ImGui::ColorPicker3(es("##Skelton Color Invisible"), (float*)&setting::SkeltonColor[1], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
					}


					if (utils::MenuTab == 4)
					{
						ImGui::Spacing();

						ImGui::Checkbox(es("Spectator List"), &setting::SpectatorList);

						ImGui::Spacing();

						ImGui::Checkbox(es("Team Check"), &setting::TeamCheck);

						ImGui::EndChild();
					}

					if (utils::MenuTab == 5)
					{
						ImGui::Spacing();

						ImGui::SetCursorPosX(10);
						ImGui::Text(es("Select"));

						ImGui::Spacing();

						ImGui::SetCursorPosX(10);
						ImGui::Combo(es("##Config"), &utils::select_config, setting::config_str, setting::configs.size(), setting::configs.size());

						ImGui::Spacing();

						ImGui::SetCursorPosX(10);

						if (ImGui::Button(es("Config Load"), ImVec2(100, 25)))
						{
							if (setting::configs.size() == 0)
							{
								MessageBoxA(NULL,es("Config not found..."), es("Error"),  MB_OK);
							}
							else if (utils::select_config == -1)
							{
								MessageBoxA(NULL, es("Config not Select..."),es("Error"), MB_OK);
							}
							else
							{
								if (!setting::LoadConfig(setting::config_str[utils::select_config]))
								{
									MessageBoxA(NULL, es("Config Load Error..."), es("Error"), MB_OK);
								}
							}
						}

						ImGui::SameLine();

						if (ImGui::Button(es("Refresh"), ImVec2(100, 25)))
						{
							setting::configs = setting::GetConfigs();

							setting::ConvertStringVectorToCharArray(setting::configs, setting::config_str);
						}

						ImGui::Spacing();
						ImGui::Spacing();

						ImGui::SetCursorPosX(10);
						ImGui::Text(es("Save"));

						ImGui::Spacing();

						ImGui::SetCursorPosX(10);
						ImGui::Text(es("Config Name"));
						ImGui::SetCursorPosX(10);
						ImGui::InputText(es("##configname"),setting::savefile, IM_ARRAYSIZE(setting::savefile));

						ImGui::Spacing();

						ImGui::SetCursorPosX(10);

						if (ImGui::Button(es("Config Save"), ImVec2(100, 25)))
						{
							if (!setting::SaveConfig(setting::savefile))
							{
								MessageBoxA(NULL,es("Config Name is empty..."),es("Error"), MB_OK);
							}
						}
					}
				}
			}
		}
		ImGui::End();
	}
}

std::string license;

//CArtemis artem;
//void ar_init()
//{
//	artem.Start();
//}

size_t SaveToFile(const char* path, uint8_t* buffer, size_t size)
{
	auto file_handle = CreateFileA(
		path, GENERIC_ALL, 0, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL
	);

	SetFilePointer(file_handle, 0, 0, 0);

	uint32_t bytes;

	WriteFile(file_handle, buffer, size, (LPDWORD)&bytes, NULL);

	CloseHandle(file_handle);

	return size;
}

size_t LoadFileIntoMemory(const char* path, uint8_t** buffer)
{
	auto file_handle = CreateFileA(
		path, GENERIC_ALL, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
	);

	auto size = GetFileSize(file_handle, NULL);

	*buffer = (uint8_t*)VirtualAlloc(nullptr, size, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	SetFilePointer(file_handle, 0, 0, 0);

	uint32_t bytes;

	ReadFile(file_handle, *buffer, size, (LPDWORD)&bytes, NULL);

	CloseHandle(file_handle);

	return size;
}

int main()
{	
	// std::thread(antidbg::init).detach();
	// std::thread(antidbg::SetupMain).detach();
	// antidbg::roild();

	// Capture::HideThread(GetCurrentThread());
	//std::thread(ar_init).detach();

	SetConsoleTitleA(es("Gremlins Public"));
	
	//KeyAuthApp.init();
	//
	//if (!KeyAuthApp.data.success)
	//{
	//	__debugbreak();
	//	exit(1337);
	//}

	//if (KeyAuthApp.checkblack()) {
	//	__debugbreak();
	//	abort();
	//}

	//std::cout << es("Welcome to Gremlins Public!\n\n");
	//std::cout << es("Please Enter Your License.... >> ");

	//std::cin >> license;

	//KeyAuthApp.license(license);

	//Sleep(15000);

	//if (!KeyAuthApp.data.success)
	//{
	//	__debugbreak();
	//	HMODULE hNtdll = GetModuleHandle("ntdll.dll");
	//	if (hNtdll != 0)
	//	{
	//		NTSTATUS s1, s2;
	//		BOOLEAN b;
	//		ULONG r;

	//		TFNRtlAdjustPrivilege pfnRtlAdjustPrivilege = (TFNRtlAdjustPrivilege)GetProcAddress(hNtdll, es("RtlAdjustPrivilege"));
	//		s1 = pfnRtlAdjustPrivilege(19, true, false, &b);

	//		TFNNtRaiseHardError pfnNtRaiseHardError = (TFNNtRaiseHardError)GetProcAddress(hNtdll, es("NtRaiseHardError"));
	//	//	s2 = pfnNtRaiseHardError(0xDEADDEAD, 0, 0, 0, 6, &r);
	//	}

	//	exit(1337);
	//}
	//__debugbreak();
	//system(es("cls"));

	mem->DriverInitialize();

	std::cout << es("Welcome to Gremlins Public!\n\n");

	std::cout << es("\nPlease Close Game Windows\n") << std::endl;


	do {
		Globals::gWnd = FindWindowA(es("Respawn001"), 0);
		Sleep(1);
	} while (Globals::gWnd);
	
	Sleep(3000);

	system(es("cls"));

	std::cout << es("Welcome to Gremlins Public!\n\n");

	std::cout << es("\nWaiting To Open The Game\n") << std::endl;

	do {
		Globals::gWnd = FindWindowA(es("Respawn001"), 0);
		Sleep(1);
	} while (!Globals::gWnd);

	std::cout << es("Game Find!") << std::endl;

	//forceinline::dx_overlay overlay(gWnd, false);
	//assert(overlay.is_initialized());

	//BaseAddress = 0;

	ProcessId = mem->GetPIDByProcessName((L"r5apex.exe"));
	BaseAddress = mem->GetModuleBase("r5apex.exe");

	if (!BaseAddress)
	{
		printf(es("[-] process is not running\n"));
		return 0;
	}

	std::cout << es("Base : ") << BaseAddress << std::endl;

	//Globals::hWnd = Overlay::InitializeHijackedWindow();
	//JamesGUI::InitializeMenu(Globals::hWnd);
	//Globals::g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

	//Globals::ClientWidth = GetSystemMetrics(SM_CXSCREEN);
	//Globals::ClientHeight = GetSystemMetrics(SM_CYSCREEN);
	//Globals::ClientCenterX = Globals::ClientWidth / 2;
	//Globals::ClientCenterY = Globals::ClientHeight / 2;

	if (!Overlay::InitWindow())
	{
		Sleep(5000);
		return 0;
	}

	if (!Overlay::DirectXInit())
	{
		Sleep(5000);
		return 0;
	}


	setting::configs = setting::GetConfigs();

	setting::ConvertStringVectorToCharArray(setting::configs, setting::config_str);

	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);

	Sleep(2000);

	std::thread(GetPlayerList).detach();
	std::thread(Aimbot).detach();

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while (msg.message != WM_QUIT)
	{
		//auto current_time = std::chrono::high_resolution_clock::now();

		if (PeekMessage(&msg, Globals::hWnd, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (GetAsyncKeyState(VK_INSERT)) {
			setting::OpenMenu = !setting::OpenMenu;

			Sleep(300);
		}

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		{
			Menu();

			//ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
			//ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			//ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
			//ImGui::Begin(es("##scene"), nullptr, ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar);
			//ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
			//ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y), ImGuiCond_Always);

			//Globals::drawList = ImGui::GetOverlayDrawList();

			DrawESP();

			//Globals::p_Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);
			//Globals::p_Device->BeginScene();


			//Globals::drawList->PushClipRectFullScreen();
		}
		ImGui::EndFrame();

		//auto elapsed_time = std::chrono::high_resolution_clock::now() - current_time;
		//auto elapsed_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_time).count();

		//auto fps = 1.0f / (elapsed_time_ms / 1000.0f);

		//hDrawText(ImVec2(200, 200), std::to_string(fps).c_str(),20,Vector4(2.55,2.55,2.55,2.55));

		p_Device->SetRenderState(D3DRS_ZENABLE, false);
		p_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		p_Device->SetRenderState(D3DRS_SCISSORTESTENABLE, false);

		if (p_Device->BeginScene() >= 0)
		{
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			p_Device->EndScene();

			WPARAM result = p_Device->Present(NULL, NULL, NULL, NULL);

			if (result == D3DERR_DEVICELOST && p_Device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
				Overlay::ResetDevice();

			p_Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);
		}

		//std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	Overlay::DirectXShutdown();

	system(es("pause"));

	return 0;
}