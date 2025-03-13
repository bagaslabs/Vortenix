#pragma once
#ifndef ONCALL_H
#define ONCALL_H

void UDP::handleGamePacket_CallFunction(uint8_t* ptr, Bot& bot) {
    TankPacketStruct* packet = reinterpret_cast<TankPacketStruct*>(ptr);
    variantlist_t varlist{};
    varlist.serialize_from_mem(ptr + 56);
    auto headVar = varlist[0].get_string();
    bot.local.onVariantListDebug.push_back(varlist);
    logger(bot, varlist.print());
    if (!headVar.empty()) {
        if (headVar == "OnSendToServer") {
            bot.local.botloginpacket.user = std::to_string(varlist.get(3).get_uint32());
            bot.local.botloginpacket.userToken = std::to_string(varlist.get(2).get_uint32());
            bot.local.server.EnetSubServerPort = varlist.get(1).get_uint32();
            bot.local.botloginpacket.tankIdName = varlist.get(6).get_string();
            //Split data
            std::string delimiter = "|";
            std::vector<std::string> v = split(varlist.get(4).get_string(), delimiter);
            bot.local.server.EnetSubServerIP = v[0];
            bot.local.botloginpacket.doorID = v[1];
            if (v[2] != "-1" or v[2] != "0" or !v[2].empty())
            {
                bot.local.botloginpacket.UUIDToken = v[2];
            }
            bot.local.botloginpacket.lmode = std::to_string(varlist[5].get_uint32());
            bot.local.server.EnetNowIp = bot.local.server.EnetSubServerIP;
            bot.local.server.EnetNowPort = bot.local.server.EnetSubServerPort;
            TankPacketStruct tank;
            tank.type = NET_GAME_PACKET_DISCONNECT;
            tank.netid = -1;
            UDP::sendPacketRaw((uint8_t*)&tank, bot);
            //Connect New Server
            generator gen;
            gen.LoginPacket(bot);
            UDP::connectClient(bot);
        }
        else if (headVar == "OnSuperMainStartAcceptLogonHrdxs47254722215a") {
            UDP::sendPacket("action|enter_game\n", NET_MESSAGE_GENERIC_TEXT, bot);
            TankPacketStruct tank;
            tank.type = NET_GAME_PACKET_APP_CHECK_RESPONSE;
            tank.netid = -1;
            tank.value = 1125432991;
            UDP::sendPacketRaw((uint8_t*)&tank, bot);
        }
        else if (headVar == "OnSendFavItemsList") {
            bot.local.FavoriteItems.clear();
            std::stringstream ss(varlist.get(1).get_string());
            std::string item;
            while (std::getline(ss, item, ',')) {
                if (!item.empty()) {
                    bot.local.FavoriteItems.push_back(std::stoi(item));
                }
            }
        }
        else if (headVar == "SetHasGrowID") {
            bot.local.botloginpacket.tankIdName = varlist.get(2).get_string();
            bot.local.botloginpacket.tankIdPass = varlist.get(3).get_string();
        }
        else if (headVar == "OnSpawn") {
            UDP::callPlayerSpawn(varlist.get(1).get_string(), bot);
            json config;
            std::ifstream configFile("config.json");
            if (configFile.is_open()) {
                configFile >> config;
                configFile.close();
            }
            config["login_form"] = bot.local.server.loginFormUrl;
            std::ofstream outFile("config.json");
            if (outFile.is_open()) {
                outFile << config.dump(4);
                outFile.close();
            }
            else {
                std::cerr << "Error: Tidak bisa membuka config.json untuk ditulis!" << std::endl;
            }
        }

        else if (headVar == "OnSetPos") {
            for (int i = 0; i < bot.local.players.size(); i++) {
                if (bot.local.players[i].netid == packet->netid) {
                    bot.local.players[i].pos.m_x = varlist.get(1).get_vector2().m_x;
                    bot.local.players[i].pos.m_y = varlist.get(1).get_vector2().m_y;
                    if (packet->netid == bot.local.netid) {
                        bot.local.pos.m_x = varlist.get(1).get_vector2().m_x;
                        bot.local.pos.m_y = varlist.get(1).get_vector2().m_y;
                    }
                }
            }
        }
        else if (headVar == "OnSetClothing") {
            for (int i = 0; i < bot.local.players.size(); i++) {
                if (bot.local.players[i].netid == static_cast<int>(packet->netid)) {
                    bot.local.players[i].ActiveItems.clear();
                    if (bot.local.netid == static_cast<int>(packet->netid)) {
                        bot.local.ActiveItems.clear();
                    }
                    for (int j = 1; j <= 3; j++) {
                        float x = varlist.get(j).get_vector3().m_x;
                        float y = varlist.get(j).get_vector3().m_y;
                        float z = varlist.get(j).get_vector3().m_z;
                        for (float v : {x, y, z}) {
                            if (v != 0) {
                                bot.local.players[i].ActiveItems.push_back(static_cast<int>(v));
                                if (bot.local.netid == static_cast<int>(packet->netid)) {
                                    bot.local.ActiveItems.push_back(static_cast<int>(v));
                                    for (int i = 0; i < bot.local.items.size(); i++) {
                                        if (bot.local.items[i].id == static_cast<int>(v)) {
                                            bot.local.items[i].isActive = true;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        else if (headVar == "OnRemove") {
            UDP::callPlayerRemove(varlist.get(1).get_string(), bot);
        }
        else if (headVar == "OnRequestWorldSelectMenu") {
            UDP::resetData(bot);
            bot.local.world.reset();
        }
        else if (headVar == "OnDialogRequest") {
            std::string wrenchMessage = varlist.get(1).get_string();
            if (wrenchMessage.find("add_popup_name|WrenchMenu|") != std::string::npos) {
                std::size_t pos = wrenchMessage.find("embed_data|netID|");
                if (pos != std::string::npos) {
                    pos += std::string("embed_data|netID|").length();
                    std::size_t endPos = wrenchMessage.find("\n", pos);
                    int netIDWrench = std::stoi(wrenchMessage.substr(pos, endPos - pos));
                    if (netIDWrench == bot.local.netid) {
                        pos = wrenchMessage.find("add_textbox|`oYou have `w");
                        if (pos != std::string::npos) {
                            pos += std::string("add_textbox|`oYou have `w").length();
                            std::size_t endPos = wrenchMessage.find("``", pos);
                            bot.local.inventorySlots = std::stoi(wrenchMessage.substr(pos, endPos - pos));
                        }
                        pos = wrenchMessage.find("add_textbox|`oCurrent world: `w");
                        if (pos != std::string::npos) {
                            pos += std::string("add_textbox|`oCurrent world: `w").length();
                            std::size_t endPos = wrenchMessage.find("`` (", pos);
                            std::string currentWorld = wrenchMessage.substr(pos, endPos - pos);
                            bot.local.world.name = currentWorld;
                            pos = endPos + 3;
                            endPos = wrenchMessage.find("``, `w", pos);
                            int xCoord = std::stoi(wrenchMessage.substr(pos, endPos - pos)) - 1;
                            bot.local.pos.m_x = xCoord * 32;

                            pos = endPos + 5;
                            endPos = wrenchMessage.find("``)", pos);
                            int yCoord = std::stoi(wrenchMessage.substr(pos, endPos - pos)) - 1;
                            bot.local.pos.m_y = yCoord * 32;
                        }
                        pos = wrenchMessage.find("add_textbox|`oTotal time played is `w");
                        if (pos != std::string::npos) {
                            pos += std::string("add_textbox|`oTotal time played is `w").length();
                            std::size_t endPos = wrenchMessage.find("`` hours", pos);
                            bot.local.totalPlaytime = std::stof(wrenchMessage.substr(pos, endPos - pos));
                        }
                    }
                }
            }
        }
    }
}

void UDP::callPlayerSpawn(std::string spawn, Bot& bot) {
    rtvar rtpar;
    std::cout << "OnSpawn" << std::endl;
    rtpar = rtvar::parse(spawn);
    Players player;
    player.name = rtpar.get("name");
    player.country = rtpar.get("country").c_str();
    player.netid = rtpar.get_int("netID");
    player.userid = rtpar.get_int("userID");
    player.eid = rtpar.get("eid");
    player.ip = rtpar.get("ip");
    auto pos = rtpar.find("posXY");
    if (pos && pos->m_values.size() >= 2)
    {
        player.pos.m_x = atoi(pos->m_values[0].c_str());
        player.pos.m_y = atoi(pos->m_values[1].c_str());
    }
    if (rtpar.get("type") == "local") {
        player.isLocal = true;
        bot.local.country = player.country;
        bot.local.netid = player.netid;
        bot.local.userid = player.userid;
        bot.local.eid = player.eid;
        bot.local.ip = player.ip;
        bot.local.pos.m_x = player.pos.m_x;
        bot.local.pos.m_y = player.pos.m_y;
    }
    bool found = false;
    for (auto& p : bot.local.players) {
        if (p.netid == player.netid) {
            p = player;
            found = true;
            break;
        }
    }
    if (!found) {
        bot.local.players.push_back(player);
    }
}

void UDP::callPlayerRemove(std::string remove, Bot& bot)
{
    rtvar rtpur;
    rtpur = rtvar::parse(remove);
    Players player;
    int netidremove = rtpur.get_int("netID");
    for (int i = 0; i < bot.local.players.size(); i++)
    {
        if (bot.local.players[i].netid == netidremove)
        {
            bot.local.players.erase(bot.local.players.begin() + i);
            if (netidremove == bot.local.netid) {
                bot.local.netid = -1;
            }
        }
    }
}

#endif