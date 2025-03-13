#pragma once

#ifndef HANDLEPACKET_H
#define HANDLEPACKET_H

std::vector<std::string> UDP::split(const std::string& str, const std::string& delimiter)
{
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = str.find(delimiter);
    while (end != std::string::npos)
    {
        tokens.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
    tokens.push_back(str.substr(start));
    return tokens;
}


std::string UDP::convertTankPacketType(eTankPacketType type) {
    switch (type) {
    case NET_GAME_PACKET_STATE: return "NET_GAME_PACKET_STATE";
    case NET_GAME_PACKET_CALL_FUNCTION: return "NET_GAME_PACKET_CALL_FUNCTION";
    case NET_GAME_PACKET_UPDATE_STATUS: return "NET_GAME_PACKET_UPDATE_STATUS";
    case NET_GAME_PACKET_TILE_CHANGE_REQUEST: return "NET_GAME_PACKET_TILE_CHANGE_REQUEST";
    case NET_GAME_PACKET_SEND_MAP_DATA: return "NET_GAME_PACKET_SEND_MAP_DATA";
    case NET_GAME_PACKET_SEND_TILE_UPDATE_DATA: return "NET_GAME_PACKET_SEND_TILE_UPDATE_DATA";
    case NET_GAME_PACKET_SEND_TILE_UPDATE_DATA_MULTIPLE: return "NET_GAME_PACKET_SEND_TILE_UPDATE_DATA_MULTIPLE";
    case NET_GAME_PACKET_TILE_ACTIVATE_REQUEST: return "NET_GAME_PACKET_TILE_ACTIVATE_REQUEST";
    case NET_GAME_PACKET_TILE_APPLY_DAMAGE: return "NET_GAME_PACKET_TILE_APPLY_DAMAGE";
    case NET_GAME_PACKET_SEND_INVENTORY_STATE: return "NET_GAME_PACKET_SEND_INVENTORY_STATE";
    case NET_GAME_PACKET_ITEM_ACTIVATE_REQUEST: return "NET_GAME_PACKET_ITEM_ACTIVATE_REQUEST";
    case NET_GAME_PACKET_ITEM_ACTIVATE_OBJECT_REQUEST: return "NET_GAME_PACKET_ITEM_ACTIVATE_OBJECT_REQUEST";
    case NET_GAME_PACKET_SEND_TILE_TREE_STATE: return "NET_GAME_PACKET_SEND_TILE_TREE_STATE";
    case NET_GAME_PACKET_MODIFY_ITEM_INVENTORY: return "NET_GAME_PACKET_MODIFY_ITEM_INVENTORY";
    case NET_GAME_PACKET_ITEM_CHANGE_OBJECT: return "NET_GAME_PACKET_ITEM_CHANGE_OBJECT";
    case NET_GAME_PACKET_SEND_LOCK: return "NET_GAME_PACKET_SEND_LOCK";
    case NET_GAME_PACKET_SEND_ITEM_DATABASE_DATA: return "NET_GAME_PACKET_SEND_ITEM_DATABASE_DATA";
    case NET_GAME_PACKET_SEND_PARTICLE_EFFECT: return "NET_GAME_PACKET_SEND_PARTICLE_EFFECT";
    case NET_GAME_PACKET_SET_ICON_STATE: return "NET_GAME_PACKET_SET_ICON_STATE";
    case NET_GAME_PACKET_ITEM_EFFECT: return "NET_GAME_PACKET_ITEM_EFFECT";
    case NET_GAME_PACKET_SET_CHARACTER_STATE: return "NET_GAME_PACKET_SET_CHARACTER_STATE";
    case NET_GAME_PACKET_PING_REPLY: return "NET_GAME_PACKET_PING_REPLY";
    case NET_GAME_PACKET_PING_REQUEST: return "NET_GAME_PACKET_PING_REQUEST";
    case NET_GAME_PACKET_GOT_PUNCHED: return "NET_GAME_PACKET_GOT_PUNCHED";
    case NET_GAME_PACKET_APP_CHECK_RESPONSE: return "NET_GAME_PACKET_APP_CHECK_RESPONSE";
    case NET_GAME_PACKET_APP_INTEGRITY_FAIL: return "NET_GAME_PACKET_APP_INTEGRITY_FAIL";
    case NET_GAME_PACKET_DISCONNECT: return "NET_GAME_PACKET_DISCONNECT";
    case NET_GAME_PACKET_BATTLE_JOIN: return "NET_GAME_PACKET_BATTLE_JOIN";
    case NET_GAME_PACKET_BATTLE_EVENT: return "NET_GAME_PACKET_BATTLE_EVENT";
    case NET_GAME_PACKET_USE_DOOR: return "NET_GAME_PACKET_USE_DOOR";
    case NET_GAME_PACKET_SEND_PARENTAL: return "NET_GAME_PACKET_SEND_PARENTAL";
    case NET_GAME_PACKET_GONE_FISHIN: return "NET_GAME_PACKET_GONE_FISHIN";
    case NET_GAME_PACKET_STEAM: return "NET_GAME_PACKET_STEAM";
    case NET_GAME_PACKET_PET_BATTLE: return "NET_GAME_PACKET_PET_BATTLE";
    case NET_GAME_PACKET_NPC: return "NET_GAME_PACKET_NPC";
    case NET_GAME_PACKET_SPECIAL: return "NET_GAME_PACKET_SPECIAL";
    case NET_GAME_PACKET_SEND_PARTICLE_EFFECT_V2: return "NET_GAME_PACKET_SEND_PARTICLE_EFFECT_V2";
    case NET_GAME_PACKET_ACTIVATE_ARROW_TO_ITEM: return "NET_GAME_PACKET_ACTIVATE_ARROW_TO_ITEM";
    case NET_GAME_PACKET_SELECT_TILE_INDEX: return "NET_GAME_PACKET_SELECT_TILE_INDEX";
    case NET_GAME_PACKET_SEND_PLAYER_TRIBUTE_DATA: return "NET_GAME_PACKET_SEND_PLAYER_TRIBUTE_DATA";
    case NET_GAME_PACKET_FTUE_SET_ITEM_TO_QUICK_INVENTORY: return "NET_GAME_PACKET_FTUE_SET_ITEM_TO_QUICK_INVENTORY";
    case NET_GAME_PACKET_PVE_NPC: return "NET_GAME_PACKET_PVE_NPC";
    case NET_GAME_PACKET_PVP_CARD_BATTLE: return "NET_GAME_PACKET_PVP_CARD_BATTLE";
    case NET_GAME_PACKET_PVE_APPLY_PLAYER_DAMAGE: return "NET_GAME_PACKET_PVE_APPLY_PLAYER_DAMAGE";
    case NET_GAME_PACKET_PVE_NPC_POSITION_UPDATE: return "NET_GAME_PACKET_PVE_NPC_POSITION_UPDATE";
    case NET_GAME_PACKET_SET_EXTRA_MODS: return "NET_GAME_PACKET_SET_EXTRA_MODS";
    case NET_GAME_PACKET_ON_STEP_TILE_MOD: return "NET_GAME_PACKET_ON_STEP_TILE_MOD";
    default: return "UNKNOWN_PACKET_TYPE";
    }
}

std::string UDP::convertPacketType(ePacketType packetType) {
    switch (packetType) {
    case NET_MESSAGE_UNKNOWN:
        return "NET_MESSAGE_UNKNOWN";
    case NET_MESSAGE_SERVER_HELLO:
        return "NET_MESSAGE_SERVER_HELLO";
    case NET_MESSAGE_GENERIC_TEXT:
        return "NET_MESSAGE_GENERIC_TEXT";
    case NET_MESSAGE_GAME_MESSAGE:
        return "NET_MESSAGE_GAME_MESSAGE";
    case NET_MESSAGE_GAME_PACKET:
        return "NET_MESSAGE_GAME_PACKET";
    case NET_MESSAGE_ERROR:
        return "NET_MESSAGE_ERROR";
    case NET_MESSAGE_TRACK:
        return "NET_MESSAGE_TRACK";
    case NET_MESSAGE_CLIENT_LOG_REQUEST:
        return "NET_MESSAGE_CLIENT_LOG_REQUEST";
    case NET_MESSAGE_CLIENT_LOG_RESPONSE:
        return "NET_MESSAGE_CLIENT_LOG_RESPONSE";
    case NET_MESSAGE_MAX:
        return "NET_MESSAGE_MAX";
    default:
        return "UNKNOWN_PACKET_TYPE";
    }
}


void UDP::sendPacket(const std::string& text, ePacketType type, Bot& bot) {
    int packetType = type;
    if (!bot.peer)
        return;
    if (bot.peer->state != ENET_PEER_STATE_CONNECTED)
        return;
    int len = 5 + text.length();
    char* packet = new char[len];
    memset(packet, 0, len);
    memcpy(packet, &packetType, 4);
    memcpy(packet + 4, text.c_str(), text.length());
    ENetPacket* nPacket = enet_packet_create(packet, len, 1);
    logger(bot, "[SYSTEM]: [SEND PACKET]: TYPE : " + UDP::convertPacketType(type) + "\n\n" + text);
    enet_peer_send(bot.peer, 0, nPacket);
    delete[] packet;
}

void UDP::sendPacket(const std::string& text, int type, Bot& bot) {
    int packetType = type;
    if (!bot.peer)
        return;
    if (bot.peer->state != ENET_PEER_STATE_CONNECTED)
        return;
    int len = 5 + text.length();
    char* packet = new char[len];
    memset(packet, 0, len);
    memcpy(packet, &packetType, 4);
    memcpy(packet + 4, text.c_str(), text.length());
    ENetPacket* nPacket = enet_packet_create(packet, len, 1);
    logger(bot, "[SYSTEM]: [SEND PACKET]: TYPE : " + type + text);
    enet_peer_send(bot.peer, 0, nPacket);
    delete[] packet;
}

void UDP::sendPacketRaw(uint8_t* ptr, Bot& bot) {
    int packetType = NET_MESSAGE_GAME_PACKET;
    if (!bot.peer)
        return;
    if (bot.peer->state != ENET_PEER_STATE_CONNECTED)
        return;
    logger(bot, "[SYSTEM]: [SEND PACKET RAW]: TYPE : " + UDP::convertPacketType(NET_MESSAGE_GAME_PACKET) + "}");
    auto* tankPacket = reinterpret_cast<TankPacketStruct*>(ptr);
    if (tankPacket) {
        std::string logMessage = "[SYSTEM]: [SEND PACKET RAW]:\n\n";
        logMessage += "  TYPE: " + convertTankPacketType(static_cast<eTankPacketType>(tankPacket->type)) + "\n";
        logMessage += "  DROPPED: " + std::to_string(static_cast<int>(tankPacket->dropped)) + "\n";
        logMessage += "  NETID: " + std::to_string(tankPacket->netid) + "\n";
        logMessage += "  STATE: " + std::to_string(tankPacket->state) + "\n";
        logMessage += "  VALUE: " + std::to_string(tankPacket->value) + "\n";
        logMessage += "  X: " + std::to_string(tankPacket->x) + "\n";
        logMessage += "  Y: " + std::to_string(tankPacket->y) + "\n";
        logMessage += "  XSPEED: " + std::to_string(tankPacket->xspeed) + "\n";
        logMessage += "  YSPEED: " + std::to_string(tankPacket->yspeed) + "\n";
        logMessage += "  TILEX: " + std::to_string(tankPacket->tilex) + "\n";
        logMessage += "  TILEY: " + std::to_string(tankPacket->tiley) + "\n";
        logMessage += "  EXT_DATA_SIZE: " + std::to_string(tankPacket->extDataSize) + "\n";
        logger(bot, "[SYSTEM]: SEND PACKET RAW TANK STRUCT:\n" + logMessage, 2);
    }
    int len = 4 + 56; // 4 byte untuk tipe + ukuran TankPacketStruct (56 byte)
    char* packet = new char[len];
    memset(packet, 0, len);
    memcpy(packet, &packetType, 4);
    memcpy(packet + 4, ptr, 56);
    ENetPacket* nPacket = enet_packet_create(packet, len, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(bot.peer, 0, nPacket);
    delete[] packet;
}

#endif