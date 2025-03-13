#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

//GUI
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//Connection and Encryption
#include <curl/curl.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/md5.h>
#include "enet.cpp"

//Handle Data
#include "json.hpp"
using json = nlohmann::json;
#include "variant.hpp"
#include "rtparam.hpp"
#include "vector.hpp"


//Internal Libraly
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <cstdint>
#include <deque>
#include <fstream>
#include <map>
#include <thread>           
#include <regex> 
#include <cmath>

//Local
#include "ItemManager/itemManager.h"
#include "struct.h"
#include "logger.h"
#include "https/getServer.h"
#include "enet/Packet/LoginPacket/loginGenerator.h"
#include "enet/enetClient.h"

static TankPacketStruct tankPacketInput;

void removeBotByName(const std::string& botName) {
    auto it = std::find_if(bot.begin(), bot.end(), [&](const Bot& b) {
        return b.local.name == botName;
        });
    if (it != bot.end()) {
        bot.erase(it);
        std::cout << "Bot removed: " << botName << std::endl;
    }
    else {
        std::cout << "Bot not found: " << botName << std::endl;
    }
}

void RenderMainContent() {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("Main Interface", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
    if (ImGui::BeginTabBar("Main Tabs")) {
        if (ImGui::BeginTabItem("Main Menu")) {
            ImGui::BeginChild("Left Panel", ImVec2(290, 0), true);
            AddButtonInput("Username", username, sizeof(username), ImVec2(20, 340), 250.0f);
            AddButtonInput("Password", password, sizeof(password), ImVec2(20, 370), 250.0f);

            // Add Bot button
            float currentCursorY = ImGui::GetCursorPosY();
            ImGui::SetCursorPos(ImVec2(20, 400));
            if (ImGui::Button("Add Bot", ImVec2(120, 25))) {
                if (strlen(username) > 0 && strlen(password) > 0 && getIndexByName(username) == -1) {
                    Bot botNow;
                    botNow.local.name = username;
                    botNow.local.password = password;
                    if (bot.size() < maxBot) {
                        bot.push_back(botNow);
                        ShowNotification("BOT SUCCESFULLY ADDED");
                        std::cout << "Bot successfully added!" << std::endl;
                        logger(botNow, "[SYSTEM]: BOT ADDED SUCCESFULLY");
                    }
                    else {
                        std::cout << "Max bot attemp" << std::endl;
                        ShowNotification("Can't Add Bot [MAX]");
                    }
                }
                else {
                    ShowNotification("Username or password is empty or already added!");
                    std::cout << "Username or password is empty or already added!" << std::endl;
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Remove Bot", ImVec2(120, 25))) {
                std::string botToRemove = selectedBot;
                bot[getIndexByName(selectedBot.c_str())].shouldRun = false;
                while (bot[getIndexByName(selectedBot.c_str())].isRunning == true) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
                removeBotByName(botToRemove);
            }

            ImGui::SetCursorPos(ImVec2(20, 438));
            if (ImGui::Button("Edit CONFIG", ImVec2(248, 25))) {
#ifdef _WIN32
                system("notepad.exe config.json");
#else
                system("gedit config.json");
#endif
            }
            if (!bot.empty()) {
                float buttonWidth = 250.0f;
                float buttonHeight = 20.0f;
                float buttonSpacing = 1.0f;
                ImVec4 defaultButtonColor = ImGui::GetStyle().Colors[ImGuiCol_Button];
                ImVec4 defaultButtonHoveredColor = ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered];
                ImVec4 selectedButtonColor = ImVec4(0.3f, 0.6f, 1.0f, 1.0f);
                for (size_t i = 0; i < bot.size(); ++i) {
                    if (bot[i].local.name == selectedBot) {
                        ImGui::PushStyleColor(ImGuiCol_Button, selectedButtonColor);  // Ubah warna tombol
                    }
                    else {
                        ImGui::PushStyleColor(ImGuiCol_Button, defaultButtonColor);  // Gunakan warna default
                    }
                    ImGui::SetCursorPos(ImVec2(20, (25 * i) + buttonHeight + buttonSpacing));
                    if (ImGui::Button(bot[i].local.name.c_str(), ImVec2(buttonWidth, buttonHeight))) {
                        selectedBot = bot[i].local.name;
                        std::cout << "Selected Bot: " << selectedBot << std::endl;
                    }
                    ImGui::PopStyleColor(1);
                }
            }
            ImGui::EndChild();
            ImGui::SameLine();
            ImGui::BeginChild("Right Panel", ImVec2(0, 0), true);
            if (!selectedBot.empty()) {
                int botIndex = getIndexByName(selectedBot.c_str());
                if (botIndex != -1) {
                    if (ImGui::BeginTabBar("Bot Info & Debug")) {
                        if (ImGui::BeginTabItem("Info")) {
                            ImGui::BeginChild("InfoContent", ImVec2(0, ImGui::GetContentRegionAvail().y - 30), true, ImGuiWindowFlags_HorizontalScrollbar);
                            ImGui::Text("Bot Name: %s", selectedBot.c_str());
                            ImGui::Text("Status: %s", bot[botIndex].local.status.c_str());
                            ImGui::Text("HTTP Status: %s", bot[botIndex].local.HTTPstatus.c_str());
                            ImGui::Text("Ping: %d", bot[botIndex].local.ping);
                            ImGui::Text("Level: %d", bot[botIndex].local.level);
                            ImGui::Text("Gems: %d", bot[botIndex].local.gems);
                            ImGui::Text("World:  %s", bot[botIndex].local.world.name.c_str());
                            AddButtonInput("World Name", requestWorld, sizeof(requestWorld), ImVec2(8, ImGui::GetCursorPosY() + 5), 200.0f);
                            ImGui::SameLine();
                            if (ImGui::Button("warp", ImVec2(200, 25))) {
                                UDP udp;
                                udp.warp(std::string(requestWorld), bot[botIndex]);
                                logger(bot[botIndex], "[SISTEM]: Warping to " + std::string(requestWorld));
                            }
                            if (ImGui::Button("Connect", ImVec2(200, 25))) {
                                if (activeThreads.find(selectedBot) != activeThreads.end()) {
                                    std::cout << "Thread for " << selectedBot << " is already running!" << std::endl;
                                    bot[botIndex].shouldRun = false;

                                    while (bot[getIndexByName(selectedBot.c_str())].isRunning) {
                                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                                    }
                                }
                                activeThreads[selectedBot] = std::thread([&, selectedBotCopy = selectedBot]() {
                                    int botIndex = getIndexByName(selectedBot.c_str());
                                    if (botIndex == -1) {
                                        std::cout << "Bot index not found!" << std::endl;
                                        return;
                                    }
                                    bot[botIndex].shouldRun = true;
                                    std::cout << "Thread started for " << selectedBotCopy << std::endl;
                                    bot[botIndex].isRunning = true;
                                    bot[botIndex].readConfigJson(bot[botIndex]);
                                    HTTPS https;
                                    generator gen;
                                    https.getServer(bot[botIndex]);
                                    gen.generateAllLoginPacket(bot[botIndex]);
                                    gen.LoginPacket(bot[botIndex]);
                                    https.getAllDataHTTPS(bot[botIndex]);
                                    if (!bot[botIndex].local.server.ltoken.empty()) {
                                        UDP udp;
                                        udp.connectClient(bot[botIndex]);
                                        udp.eventListener(bot[botIndex]);
                                    }
                                    std::cout << "Thread for " << selectedBotCopy << " has finished." << std::endl;
                                    activeThreads.erase(selectedBotCopy);
                                    bot[botIndex].isRunning = false;
                                    });
                                activeThreads[selectedBot].detach();
                            }
                            ImGui::SameLine();
                            if (ImGui::Button("Disconnect", ImVec2(200, 25))) {
                                bot[botIndex].shouldRun = false;
                                HTTPS https;
                                https.clearServerLocal(bot[botIndex]);
                                generator gen;
                                gen.resetData(bot[botIndex]);
                                bot[botIndex].local.status = "offline";
                                bot[botIndex].local.HTTPstatus = "nothing";
                                UDP udp;
                                udp.disconnectClient(bot[botIndex]);
                            }
                            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
                            ImGui::PushFont(ImGui::GetFont());
                            ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), "Login Data Information");
                            ImGui::PopFont();
                            ImGui::Separator();

                            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
                            ImGui::BeginGroup();
                            ImGui::Text("TankIdName: %s", bot[botIndex].local.botloginpacket.tankIdName.c_str());
                            ImGui::Text("TankIdPass: %s", bot[botIndex].local.botloginpacket.tankIdPass.c_str());
                            ImGui::Text("RequestedName: %s", bot[botIndex].local.botloginpacket.requestedName.c_str());
                            ImGui::Text("F: %s", bot[botIndex].local.botloginpacket.f.c_str());
                            ImGui::Text("Protocol: %s", bot[botIndex].local.botloginpacket.protocol.c_str());
                            ImGui::Text("Game Version: %s", bot[botIndex].local.botloginpacket.game_version.c_str());
                            ImGui::Text("FZ: %s", bot[botIndex].local.botloginpacket.fz.c_str());
                            ImGui::Text("CBits: %s", bot[botIndex].local.botloginpacket.cbits.c_str());
                            ImGui::Text("Player Age: %s", bot[botIndex].local.botloginpacket.player_age.c_str());
                            ImGui::Text("GDPR: %s", bot[botIndex].local.botloginpacket.GDPR.c_str());
                            ImGui::Text("Category: %s", bot[botIndex].local.botloginpacket.category.c_str());
                            ImGui::Text("Total Playtime: %s", bot[botIndex].local.botloginpacket.total_playtime.c_str());
                            ImGui::Text("PlatformID: %s", bot[botIndex].local.botloginpacket.platformID.c_str());
                            ImGui::Text("Device Version: %s", bot[botIndex].local.botloginpacket.device_version.c_str());
                            ImGui::Text("Country: %s", bot[botIndex].local.botloginpacket.country.c_str());
                            ImGui::EndGroup();

                            ImGui::SameLine();
                            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 55);

                            ImGui::BeginGroup();
                            ImGui::Text("KLV: %s", bot[botIndex].local.botloginpacket.klv.c_str());
                            ImGui::Text("Hash2: %s", bot[botIndex].local.botloginpacket.hash2.c_str());
                            ImGui::Text("Meta: %s", bot[botIndex].local.botloginpacket.meta.c_str());
                            ImGui::Text("FHash: %s", bot[botIndex].local.botloginpacket.fhash.c_str());
                            ImGui::Text("RID: %s", bot[botIndex].local.botloginpacket.rid.c_str());
                            ImGui::Text("Hash: %s", bot[botIndex].local.botloginpacket.hash.c_str());
                            ImGui::Text("MAC: %s", bot[botIndex].local.botloginpacket.mac.c_str());
                            ImGui::Text("WK: %s", bot[botIndex].local.botloginpacket.wk.c_str());
                            ImGui::Text("ZF: %s", bot[botIndex].local.botloginpacket.zf.c_str());
                            ImGui::Text("LMode: %s", bot[botIndex].local.botloginpacket.lmode.c_str());
                            ImGui::Text("User: %s", bot[botIndex].local.botloginpacket.user.c_str());
                            ImGui::Text("UserToken: %s", bot[botIndex].local.botloginpacket.userToken.c_str());
                            ImGui::Text("UUIDToken: %s", bot[botIndex].local.botloginpacket.UUIDToken.c_str());
                            ImGui::Text("DoorID: %s", bot[botIndex].local.botloginpacket.doorID.c_str());
                            ImGui::Text("AAT: %s", bot[botIndex].local.botloginpacket.aat.c_str());
                            ImGui::EndGroup();

                            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
                            ImGui::PushFont(ImGui::GetFont());
                            ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), "HTTPS Information");
                            ImGui::PopFont();
                            ImGui::Separator();

                            ImGui::BeginGroup();
                            ImGui::Text("Login Form URL: %s", bot[botIndex].local.server.loginFormUrl.c_str());
                            ImGui::Text("Form Token: %s", bot[botIndex].local.server.formToken.c_str());
                            ImGui::Text("Cookie Visit: %s", bot[botIndex].local.server.cookievisit.c_str());
                            ImGui::Text("Cookie Activity: %s", bot[botIndex].local.server.cookieactivity.c_str());
                            ImGui::Text("Cookie AWSALBTG: %s", bot[botIndex].local.server.cookie1_AWSALBTG.c_str());
                            ImGui::Text("Cookie AWSALBTGCORS: %s", bot[botIndex].local.server.cookie2_AWSALBTGCORS.c_str());
                            ImGui::Text("Cookie AWSALB: %s", bot[botIndex].local.server.cookie3_AWSALB.c_str());
                            ImGui::Text("Cookie AWSALBCORS: %s", bot[botIndex].local.server.cookie4_AWSALBCORS.c_str());
                            ImGui::Text("Cookie XSRF: %s", bot[botIndex].local.server.cookie5_XSRF.c_str());
                            ImGui::Text("Cookie GAMESESSION: %s", bot[botIndex].local.server.cookie6_GAMESESSION.c_str());
                            ImGui::Text("Status Token: %s", bot[botIndex].local.server.statusToken.c_str());
                            ImGui::Text("Login Token: %s", bot[botIndex].local.server.ltoken.c_str());
                            ImGui::EndGroup();

                            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
                            ImGui::PushFont(ImGui::GetFont());
                            ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), "Enet Server Information");
                            ImGui::PopFont();
                            ImGui::Separator();

                            ImGui::BeginGroup();
                            ImGui::Text("Enet First IP: %s", bot[botIndex].local.server.EnetFirstIP.c_str());
                            ImGui::Text("Enet First Port: %d", bot[botIndex].local.server.EnetFirstPort);
                            ImGui::Text("Enet Sub Server IP: %s", bot[botIndex].local.server.EnetSubServerIP.c_str());
                            ImGui::Text("Enet Sub Server Port: %d", bot[botIndex].local.server.EnetSubServerPort);
                            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
                            ImGui::Text("Enet Now Connected IP: %s", bot[botIndex].local.server.EnetNowIp.c_str());
                            ImGui::Text("Enet Now Connected Port: %d", bot[botIndex].local.server.EnetNowPort);
                            ImGui::EndGroup();

                            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
                            ImGui::PushFont(ImGui::GetFont());
                            ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), "Bot Local Information");
                            ImGui::Separator();

                            ImGui::BeginGroup();
                            ImGui::Text("Name: %s", bot[botIndex].local.name.c_str());
                            ImGui::Text("Password: %s", bot[botIndex].local.password.c_str());
                            ImGui::Text("Country: %s", bot[botIndex].local.country.c_str());
                            ImGui::Text("EID: %s", bot[botIndex].local.eid.c_str());
                            ImGui::Text("IP: %s", bot[botIndex].local.ip.c_str());
                            ImGui::Text("Net ID: %d", bot[botIndex].local.netid);
                            ImGui::Text("User ID: %d", bot[botIndex].local.userid);
                            ImGui::Text("Gems: %d", bot[botIndex].local.gems);
                            ImGui::Text("Level: %d", bot[botIndex].local.level);
                            ImGui::Text("Current World: %s", bot[botIndex].local.world.name.c_str());
                            ImGui::Text("Position: (%.2f, %.2f)", bot[botIndex].local.pos.m_x / 32.0f, bot[botIndex].local.pos.m_y / 32.0f);
                            ImGui::Text("Speed: (%.2f, %.2f)", bot[botIndex].local.speedX, bot[botIndex].local.speedY);
                            std::string favoriteItemsStr = "Favorite Items: ";
                            for (size_t i = 0; i < bot[botIndex].local.FavoriteItems.size(); ++i) {
                                favoriteItemsStr += std::to_string(bot[botIndex].local.FavoriteItems[i]);
                                if (i < bot[botIndex].local.FavoriteItems.size() - 1) {
                                    favoriteItemsStr += ", ";
                                }
                            }
                            ImGui::Text("%s", favoriteItemsStr.c_str());
                            std::string activeItemsStr = "Active Items: ";
                            for (size_t i = 0; i < bot[botIndex].local.ActiveItems.size(); ++i) {
                                activeItemsStr += std::to_string(bot[botIndex].local.ActiveItems[i]);
                                if (i < bot[botIndex].local.ActiveItems.size() - 1) {
                                    activeItemsStr += ", ";
                                }
                            }
                            ImGui::Text("%s", activeItemsStr.c_str());
                            ImGui::EndGroup();
                            ImGui::PopFont();
                            ImGui::InputInt("Range", &collect_range);
                            ImGui::InputInt("Interval (ms)", &collect_interval);
                            ImGui::InputText("Item ID (comma separated)", id_input, IM_ARRAYSIZE(id_input));
                            ToggleButton("Auto Collect", &auto_collect);
                            ImGui::SameLine();
                            ToggleButton("Auto CollectSet", &auto_collect_set);
                            if (ImGui::IsAnyItemActive()) {
                                auto_collect = false;
                                auto_collect_set = false;
                                collect_thread_running = false; // Pastikan thread tidak berjalan
                            }
                            if ((auto_collect || auto_collect_set) && !collect_thread_running) {
                                collect_thread_running = true;

                                std::thread([&]() {
                                    while (auto_collect || auto_collect_set) {
                                        if (ImGui::IsAnyItemActive()) {
                                            auto_collect = false;
                                            auto_collect_set = false;
                                            break; // Hentikan loop jika user berinteraksi
                                        }

                                        if (auto_collect) {
                                            UDP udp;
                                            udp.collect(collect_range, bot[botIndex]);
                                        }

                                        if (auto_collect_set) {
                                            std::vector<int> item_ids;
                                            std::istringstream iss(id_input);
                                            std::string token;
                                            while (std::getline(iss, token, ',')) {
                                                item_ids.push_back(std::stoi(token));
                                            }
                                            UDP udp;
                                            udp.collectSet(collect_range, collect_interval, item_ids, bot[botIndex]);
                                        }

                                        std::this_thread::sleep_for(std::chrono::milliseconds(collect_interval));
                                    }

                                    collect_thread_running = false;
                                    }).detach();
                            }
                            ImGui::PushItemWidth(250);  // Atur lebar input
                            ImGui::InputInt("X", &xFp);
                            ImGui::InputInt("Y", &yFp);
                            ImGui::PopItemWidth();  // Kembalikan ke ukuran default
                            if (ImGui::Button("Find Path")) {
                                int targetX = xFp;
                                int targetY = yFp;
                                std::thread([&, targetX, targetY]() {
                                    UDP udp;
                                    udp.findPath(targetX, targetY, bot[botIndex]);
                                    }).detach();
                            }
                            if (ImGui::Button("Execute Script")) {
                                std::thread([&, botIndex]() {
                                    UDP udp;
                                    udp.mainScript(bot[botIndex]); // Jalankan script di thread terpisah
                                    }).detach();
                            }
                            ImGui::Separator();
                            ImGui::EndChild();
                            ImGui::EndTabItem();
                        }
                        static bool autoScroll = false;
                        static int selectedTab = 0; // 0 = All, 1 = Variant
                        if (ImGui::BeginTabItem("Debug")) {
                            ImGui::Checkbox("Auto Scroll", &autoScroll);
                            ImGui::SameLine();
                            if (ImGui::Button("Clear")) {
                                int botIndex = getIndexByName(selectedBot.c_str());
                                if (botIndex != -1) {
                                    switch (selectedTab) {
                                    case 0: bot[botIndex].local.allDebug.clear(); break;
                                    case 1: bot[botIndex].local.onGenerixText.clear(); break;
                                    case 2: bot[botIndex].local.gameMessageDebug.clear(); break;
                                    case 3: bot[botIndex].local.tankPacketDebug.clear(); break;
                                    case 4: bot[botIndex].local.onVariantListDebug.clear(); break;
                                    }
                                }
                            }
                            
                            ImGui::BeginTabBar("DebugTabs");
                            if (ImGui::BeginTabItem("All")) { selectedTab = 0; ImGui::EndTabItem(); }
                            if (ImGui::BeginTabItem("Generic")) { selectedTab = 1; ImGui::EndTabItem(); }
                            if (ImGui::BeginTabItem("Game Message")) { selectedTab = 2; ImGui::EndTabItem(); }
                            if (ImGui::BeginTabItem("Game Packet")) { selectedTab = 3; ImGui::EndTabItem(); }
                            if (ImGui::BeginTabItem("Variant")) { selectedTab = 4; ImGui::EndTabItem(); }
                            if (ImGui::BeginTabItem("Track Packet")) { selectedTab = 5; ImGui::EndTabItem(); }
                            ImGui::EndTabBar();

                            ImGui::BeginChild("DebugContent", ImVec2(0, ImGui::GetContentRegionAvail().y - 30), true, ImGuiWindowFlags_HorizontalScrollbar);
                            int botIndex = getIndexByName(selectedBot.c_str());
                            if (botIndex != -1) {
                                switch (selectedTab) {
                                case 0:
                                    for (const auto& log : bot[botIndex].local.allDebug)
                                        ImGui::Text("%s", log.c_str());
                                    break;
                                case 1:
                                    for (const auto& log : bot[botIndex].local.onGenerixText)
                                        ImGui::Text("%s", log.c_str());
                                    break;
                                case 2:
                                    for (const auto& log : bot[botIndex].local.gameMessageDebug)
                                        ImGui::Text("%s", log.c_str());
                                    break;
                                case 3:
                                    for (const auto& log : bot[botIndex].local.tankPacketDebug)
                                        ImGui::Text("%s", log.c_str());
                                    break;
                                case 4:
                                    for (const auto& variant : bot[botIndex].local.onVariantListDebug) {
                                        ImGui::Text("Variant:");
                                        for (const auto& variant : bot[botIndex].local.onVariantListDebug) {
                                            variantlist_t variantCopy = variant;
                                            std::string variantContent = variantCopy.print();
                                            ImGui::Text("%s", variantContent.c_str() + '\n');
                                        }
                                    }
                                    break;
                                case 5:
                                    for (const auto& log : bot[botIndex].local.trackPacketDebug)
                                        ImGui::Text("%s", log.c_str());
                                    break;
                                }
                                if (autoScroll) {
                                    ImGui::SetScrollHereY(1.0f);
                                }
                            }
                            else {
                                ImGui::Text("Bot not found or no debug data available.");
                            }
                            
                            ImGui::EndChild();
                            ImGui::EndTabItem();
                        } 
                        if (ImGui::BeginTabItem("Inventory")) {
                            if (ImGui::BeginTable("InventoryTable", 7, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 200.0f);
                                ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                                ImGui::TableSetupColumn("Count", ImGuiTableColumnFlags_WidthFixed, 40.0f);
                                ImGui::TableSetupColumn("Active", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                                ImGui::TableSetupColumn("Favorite", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                                ImGui::TableSetupColumn("Drop", ImGuiTableColumnFlags_WidthFixed, 45.0f);
                                ImGui::TableSetupColumn("Trash", ImGuiTableColumnFlags_WidthFixed, 45.0f);
                                ImGui::TableHeadersRow();

                                for (auto& item : bot[botIndex].local.items) {
                                    ImGui::TableNextRow();

                                    ImGui::TableSetColumnIndex(0);
                                    std::string itemName = "[Parsing Items]";
                                    if (!isParsingItems) {
                                        std::string itemNameFromDef = itemDefs->GetNameById(item.id);
                                        if (!itemNameFromDef.empty()) {
                                            itemName = itemNameFromDef;
                                        }
                                    }
                                    ImGui::Text("%s", itemName.c_str());

                                    ImGui::TableSetColumnIndex(1);
                                    ImGui::Text("%d", item.id);

                                    ImGui::TableSetColumnIndex(2);
                                    ImGui::Text("%d", item.count);

                                    ImGui::TableSetColumnIndex(3);
                                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
                                    ImGui::PushID(item.id);
                                    if (ImGui::Button(item.isActive ? "Unwear" : "Wear")) {
                                        std::thread([&, item]() {
                                            UDP udp;
                                            udp.wear(item.id, bot[botIndex]);
                                            }).detach();
                                    }
                                    ImGui::PopID();

                                    ImGui::TableSetColumnIndex(4);
                                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
                                    ImGui::PushID(item.id + 1000);
                                    if (ImGui::Button(item.isFavorite ? "Unfav" : "Fav")) {
                                        //bot.favorite(item.id);
                                    }
                                    ImGui::PopID();

                                    ImGui::TableSetColumnIndex(5);
                                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
                                    ImGui::PushID(item.id + 2000);
                                    if (ImGui::Button("Drop")) {
                                        std::thread([&, item]() {
                                            UDP udp;
                                            udp.drop(item.id, item.count, bot[botIndex]);
                                            }).detach();
                                    }
                                    ImGui::PopID();

                                    ImGui::TableSetColumnIndex(6);
                                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
                                    ImGui::PushID(item.id + 3000);
                                    if (ImGui::Button("Trash")) {
                                        std::thread([&, item]() {
                                        UDP udp;
                                        udp.trash(item.id, item.count, bot[botIndex]);
                                            }).detach();
                                    }
                                    ImGui::PopID();
                                }
                                ImGui::EndTable();
                            }
                            ImGui::EndTabItem();
                        }
                        if (ImGui::BeginTabItem("Player List")) {
                            ImGui::BeginTabBar("Players Tab");
                            if (ImGui::BeginTabItem("General")) {
                                if (ImGui::BeginTable("PlayerTable", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                                    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                                    ImGui::TableSetupColumn("Country", ImGuiTableColumnFlags_WidthFixed, 60.0f);
                                    ImGui::TableSetupColumn("UserID", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                                    ImGui::TableSetupColumn("NetID", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                                    ImGui::TableSetupColumn("Pos (X, Y)", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                                    ImGui::TableSetupColumn("Local Player", ImGuiTableColumnFlags_WidthFixed, 100.0f);

                                    ImGui::TableHeadersRow();

                                    if (botIndex >= 0 && botIndex < bot.size()) {
                                        for (const auto& player : bot[botIndex].local.players) {
                                            ImGui::TableNextRow();
                                            ImGui::TableSetColumnIndex(0);
                                            ImGui::Text("%s", player.name.c_str());
                                            ImGui::TableSetColumnIndex(1);
                                            ImGui::Text("%s", player.country.c_str());
                                            ImGui::TableSetColumnIndex(2);
                                            ImGui::Text("%d", player.userid);
                                            ImGui::TableSetColumnIndex(3);
                                            ImGui::Text("%d", player.netid);
                                            ImGui::TableSetColumnIndex(4);
                                            ImGui::Text("(%d, %d)", static_cast<int>(std::floor(player.pos.m_x / 32.0f)),
                                                static_cast<int>(std::floor(player.pos.m_y / 32.0f)));
                                            ImGui::TableSetColumnIndex(5);
                                            ImGui::Text(player.isLocal ? "Yes" : "No");
                                        }
                                    }

                                    ImGui::EndTable();
                                }
                                ImGui::EndTabItem();
                            }

                            if (ImGui::BeginTabItem("Details")) {
                                if (botIndex >= 0 && botIndex < bot.size()) {
                                    for (const auto& player : bot[botIndex].local.players) {
                                        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8);
                                        ImGui::PushFont(ImGui::GetFont());
                                        ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), "Player Information");
                                        ImGui::PopFont();
                                        ImGui::Separator();

                                        ImGui::Text("Name: %s", player.name.c_str());
                                        ImGui::Text("Country: %s", player.country.c_str());
                                        ImGui::Text("UserID: %d", player.userid);
                                        ImGui::Text("NetID: %d", player.netid);
                                        ImGui::Text("EID: %s", player.eid.c_str());
                                        ImGui::Text("IP: %s", player.ip.c_str());
                                        ImGui::Text("Pos (X, Y): (%d, %d)", static_cast<int>(std::floor(player.pos.m_x / 32.0f)),
                                            static_cast<int>(std::floor(player.pos.m_y / 32.0f)));
                                        ImGui::Text("Speed (X, Y): (%.2f, %.2f)", player.speedX, player.speedY);
                                        ImGui::Text("Local Player: %s", player.isLocal ? "Yes" : "No");

                                        ImGui::Text("Active Items: ");
                                        for (const auto& item : player.ActiveItems) {
                                            ImGui::Text("%d %s %s", item, " : ", itemDefs->GetNameById(item).c_str());
                                        }
                                    }
                                }
                                ImGui::EndTabItem();
                            }
                            ImGui::EndTabBar();
                            ImGui::EndTabItem();
                        }
                        if (ImGui::BeginTabItem("World")) {
                            ImGui::BeginTabBar("World tab");
                            if (ImGui::BeginTabItem("General")) {
                                ImGui::Text("World :%s", bot[botIndex].local.world.name.c_str());
                                ImGui::Text("Height:%d", bot[botIndex].local.world.height);
                                ImGui::Text("Width :%d", bot[botIndex].local.world.width);
                                ImGui::Text("Base Weather :%s",itemDefs->GetNameById(bot[botIndex].local.world.base_weather).c_str());
                                ImGui::Text("Current :%s", itemDefs->GetNameById(bot[botIndex].local.world.current_weather).c_str());
                                ImGui::EndTabItem();
                            }
                            if (ImGui::BeginTabItem("Objects")) {
                                if (ImGui::BeginTable("ObjectsTable", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                                    ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                                    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 150.0f);
                                    ImGui::TableSetupColumn("Position (X,Y)", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                                    ImGui::TableSetupColumn("Count", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                                    ImGui::TableSetupColumn("Flags", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                                    ImGui::TableSetupColumn("UID", ImGuiTableColumnFlags_WidthFixed, 70.0f);
                                    ImGui::TableHeadersRow();
                                    for (const auto& item : bot[botIndex].local.world.dropped.items) {
                                        ImGui::TableNextRow();
                                        ImGui::TableSetColumnIndex(0);
                                        ImGui::Text("%d", item.id);
                                        ImGui::TableSetColumnIndex(1);
                                        ImGui::Text(" %s", itemDefs->GetNameById(item.id).c_str());
                                        ImGui::TableSetColumnIndex(2);
                                        ImGui::Text("(%d, %d)", static_cast<int>(item.x) / 32, static_cast<int>(item.y) / 32);
                                        ImGui::TableSetColumnIndex(3);
                                        ImGui::Text("%d", item.count);
                                        ImGui::TableSetColumnIndex(4);
                                        ImGui::Text("%d", item.flags);
                                        ImGui::TableSetColumnIndex(5);
                                        ImGui::Text("%u", item.uid);
                                    }
                                    ImGui::EndTable();
                                }
                                ImGui::EndTabItem();
                            }
                            if (ImGui::BeginTabItem("Map")) {
                                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                                ImVec2 start_pos = ImGui::GetCursorScreenPos();
                                float tile_size = 6.0f; // Tile size in pixels
                                if (!bot[botIndex].local.world.tiles.empty() && !(bot[botIndex].local.world.tiles.size() < bot[botIndex].local.world.tile_count) && bot[botIndex].local.world.parseStatus == "success") {
                                    for (uint32_t y = 0; y < bot[botIndex].local.world.height; y++) {
                                        for (uint32_t x = 0; x < bot[botIndex].local.world.width; x++) {
                                            Tile& tile = bot[botIndex].local.world.tiles[y * bot[botIndex].local.world.width + x];
                                            uint16_t id = tile.foreground_item_id != 0 ? tile.foreground_item_id : tile.background_item_id;
                                            ImU32 color = GetTileColor(id);

                                            if (static_cast<int>(bot[botIndex].local.pos.m_x/32) == static_cast<int>(tile.x) &&
                                                static_cast<int>(bot[botIndex].local.pos.m_y/32) == static_cast<int>(tile.y)) {
                                                color = IM_COL32(255, 0, 0, 255); // Ubah warna menjadi merah
                                            }

                                            ImVec2 tile_pos = ImVec2(start_pos.x + x * tile_size, start_pos.y + y * tile_size);
                                            ImVec2 tile_end = ImVec2(tile_pos.x + tile_size, tile_pos.y + tile_size);

                                            // Draw the tile
                                            draw_list->AddRectFilled(tile_pos, tile_end, color);

                                            // Check if the mouse is over the tile
                                            if (ImGui::IsMouseHoveringRect(tile_pos, tile_end)) {
                                                // Display information about the tile when hovered
                                                std::string info = "Tile info:\n";
                                                info += "ID: " + std::to_string(id) + "\n";
                                                info += "Foreground Item ID: " + std::to_string(tile.foreground_item_id) + "\n";
                                                info += "Background Item ID: " + std::to_string(tile.background_item_id) + "\n";
                                                info += "Parent Block Index: " + std::to_string(tile.parent_block_index) + "\n";
                                                info += "Flags: " + std::to_string(tile.flags_number) + "\n";
                                                info += "Tile Type: " + TileTypeToString(tile.tile_type) + "\n";
                                                info += "Position: (" + std::to_string(tile.x) + ", " + std::to_string(tile.y) + ")";
                                                if (std::holds_alternative<Seed>(tile.tile_type)) {
                                                    const Seed& seed = std::get<Seed>(tile.tile_type);
                                                    info += "\nSeed ID: " + std::to_string(seed.seedID);
                                                    info += "\nGrowtime: " + std::to_string(itemDefs->getGrowtimeByID(seed.seedID));
                                                    info += "\nTime Passed: " + std::to_string(seed.time_passed);
                                                    info += "\nItem on Tree: " + std::to_string(seed.item_on_tree);
                                                    info += "\nReady to Harvest: " + std::string(seed.ready_to_harvest() ? "Yes" : "No");

                                                    auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(
                                                        std::chrono::steady_clock::now() - seed.elapsed
                                                    ).count();

                                                    info += "\nElapsed: " + std::to_string(elapsed_time) + " seconds";
                                                }
                                                if (std::holds_alternative<Door>(tile.tile_type)) {
                                                    const Door& door = std::get<Door>(tile.tile_type);
                                                    info += "\nText: " + door.text;
                                                    info += "\nUnknown 1: " + std::to_string(door.unknown_1);
                                                }

                                                // Jika tile tipe adalah Sign
                                                if (std::holds_alternative<Sign>(tile.tile_type)) {
                                                    const Sign& sign = std::get<Sign>(tile.tile_type);
                                                    info += "\nText: " + sign.text;
                                                }

                                                // Jika tile tipe adalah Lock
                                                if (std::holds_alternative<Lock>(tile.tile_type)) {
                                                    const Lock& lock = std::get<Lock>(tile.tile_type);
                                                    info += "\nSettings: " + std::to_string(lock.settings);
                                                    info += "\nOwner UID: " + std::to_string(lock.owner_uid);
                                                    info += "\nAccess Count: " + std::to_string(lock.access_count);
                                                    info += "\nAccess UIDs: ";
                                                    for (uint32_t uid : lock.access_uids) {
                                                        info += std::to_string(uid) + " ";
                                                    }
                                                    info += "\nMinimum Level: " + std::to_string(lock.minimum_level);
                                                }

                                                // Jika tile tipe adalah Mailbox
                                                if (std::holds_alternative<Mailbox>(tile.tile_type)) {
                                                    const Mailbox& mailbox = std::get<Mailbox>(tile.tile_type);
                                                    info += "\nUnknown 1: " + mailbox.unknown_1;
                                                    info += "\nUnknown 2: " + mailbox.unknown_2;
                                                    info += "\nUnknown 3: " + mailbox.unknown_3;
                                                    info += "\nUnknown 4: " + std::to_string(mailbox.unknown_4);
                                                }

                                                // Jika tile tipe adalah Dice
                                                if (std::holds_alternative<Dice>(tile.tile_type)) {
                                                    const Dice& dice = std::get<Dice>(tile.tile_type);
                                                    info += "\nSymbol: " + std::to_string(dice.symbol);
                                                }
                                                // Show the info in a tooltip
                                                ImGui::SetTooltip("%s", info.c_str());
                                            }
                                        }
                                    }
                                }
                                ImGui::Dummy(ImVec2(0.0f, 360.0f)); // Tambahkan jarak vertikal
                                ImGui::Text("Find Path:");
                                ImGui::PushItemWidth(250);
                                ImGui::InputInt("X", &xFp);
                                ImGui::InputInt("Y", &yFp);
                                ImGui::PopItemWidth();

                                if (ImGui::Button("Find Path")) {
                                    int targetX = xFp;
                                    int targetY = yFp;
                                    std::thread([&, targetX, targetY]() {
                                        UDP udp;
                                        udp.findPath(targetX, targetY, bot[botIndex]);
                                        }).detach();
                                }
                                ImGui::EndTabItem();
                            }
                            if (ImGui::BeginTabItem("Tank Packet Editor")) {
                                ImGui::Text("Tank Packet Data");
                                ImGui::Separator();

                                ImGui::InputInt("Type", reinterpret_cast<int*>(&tankPacketInput.type));
                                ImGui::InputScalar("Dropped", ImGuiDataType_U8, &tankPacketInput.dropped);
                                ImGui::InputScalar("Padding 1", ImGuiDataType_U8, &tankPacketInput.padding1);
                                ImGui::InputScalar("Padding 2", ImGuiDataType_U8, &tankPacketInput.padding2);
                                ImGui::InputInt("Net ID", reinterpret_cast<int*>(&tankPacketInput.netid));
                                ImGui::InputInt("SNet ID", reinterpret_cast<int*>(&tankPacketInput.snetid));
                                ImGui::InputInt("State", reinterpret_cast<int*>(&tankPacketInput.state));
                                ImGui::InputFloat("Padding 4", &tankPacketInput.padding4);
                                ImGui::InputInt("Value", reinterpret_cast<int*>(&tankPacketInput.value));
                                ImGui::InputFloat("X Position", &tankPacketInput.x);
                                ImGui::InputFloat("Y Position", &tankPacketInput.y);
                                ImGui::InputFloat("X Speed", &tankPacketInput.xspeed);
                                ImGui::InputFloat("Y Speed", &tankPacketInput.yspeed);
                                ImGui::InputFloat("Padding 5", &tankPacketInput.padding5);
                                ImGui::InputInt("Tile X", &tankPacketInput.tilex);
                                ImGui::InputInt("Tile Y", &tankPacketInput.tiley);
                                ImGui::InputInt("Ext Data Size", reinterpret_cast<int*>(&tankPacketInput.extDataSize));

                                if (ImGui::Button("Send Packet")) {
                                    UDP udp;
                                    udp.sendPacketRaw(reinterpret_cast<uint8_t*>(&tankPacketInput), bot[botIndex]);
                                }

                                ImGui::EndTabItem();
                            }
                            ImGui::EndTabBar();
                            ImGui::EndTabItem();
                        }
                        ImGui::EndTabBar();
                    }
                }
                else {
                    ImGui::Text("Bot not found or invalid index!");
                }
            }
            else if (selectedBot.empty() and bot.size() > 0) {
                ImGui::Text("Please Select Bot...");
            }
            else {
                ImGui::Text("You Need To Add Bot");
            }
            ImGui::EndChild();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Bot List")) {
            ImGui::Text("This is the Bot List tab. Add your content here.");
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Database")) {
            static const ItemInfo* selectedItem = nullptr;
            ImGui::Text("Total Items: %d", itemDefs->items.size());

            if (isParsingItems == true) {
                ImGui::Text("Parsing Data On Progress");
            }
            else {
                ImGui::BeginChild("Left Panel", ImVec2(250, 0), true);
                ImGui::Text("Item List");
                for (const auto& item : itemDefs->items) {
                    if (ImGui::Button(item.name.c_str())) {
                        selectedItem = &item;
                    }
                }
                ImGui::EndChild();
                ImGui::SameLine();
                ImGui::BeginChild("Right Panel", ImVec2(0, 0), true);
                ImGui::Text("Search Item: (%d)", itemDefs->items.size());
                static char searchQuery[128] = "";
                ImGui::InputText("##searchInput", searchQuery, IM_ARRAYSIZE(searchQuery));
                if (strlen(searchQuery) > 0) {
                    std::vector<ItemInfo*> searchResults;
                    itemDefs->GetItemsByPartialName(searchResults, searchQuery);
                    if (!searchResults.empty()) {
                        for (auto* item : searchResults) {
                            if (ImGui::Selectable(item->name.c_str())) {
                                selectedItem = item;
                                memset(searchQuery, 0, sizeof(searchQuery));
                            }
                        }
                    }
                    else {
                        ImGui::Text("No items found.");
                    }
                }
                if (selectedItem) {
                    ImGui::Text("Item Details:");
                    ImGui::Text("Name: %s", selectedItem->name.c_str());
                    ImGui::Text("ID: %d", selectedItem->id);
                    ImGui::Text("Type: %d", selectedItem->type);
                    ImGui::Text("Collision Type: %d", selectedItem->collisionType);
                    ImGui::Text("Break Hits: %d", selectedItem->breakHits);
                    ImGui::Text("Clothing Type: %d", selectedItem->clothingType);
                    ImGui::Text("Rarity: %d", selectedItem->rarity);
                    ImGui::Text("Grow Time: %d seconds", selectedItem->growTime);
                    ImGui::Text("Is Collision: %s", selectedItem->isCollision ? "Yes" : "No");
                }

                ImGui::EndChild();
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}


int main() {
    itemDefs = std::make_shared<ItemInfoManager>();
    std::thread parseThread([]() {
        std::this_thread::yield();
        bool success = itemDefs->ParseItemJson("ItemManager/items.json");
        if (true) {
            isParsingItems = false;
            std::cout << "Parsing selesai.\n";
        }
        else {
            std::cerr << "Parsing gagal.\n";
            isParsingItems = true;
            std::cout << "[SYSTEM]: PARSING FAILED, PLEASE TRY AGAIN" << std::endl;
        }
    });
    parseThread.detach();

    bot.reserve(maxBot);

    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(900, 510, "Vortenix 2.0", NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ApplyCustomTheme();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        RenderMainContent();
        RenderNotifications();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        // Render data ImGui
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}