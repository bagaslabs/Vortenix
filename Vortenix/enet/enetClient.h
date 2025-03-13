#pragma once
#ifndef ENETCLIENT_H
#define ENETCLIENT_H

class UDP {
private:
    std::string convertTankPacketType(eTankPacketType type);
    std::string convertPacketType(ePacketType packetType);
    std::vector<std::string> split(const std::string& str, const std::string& delimiter);
public:
    void resetData(Bot& bot);
    void disconnectClient(Bot& bot);

    void connectClient(Bot& bot);
    void eventListener(Bot& bot);
    void OnReceive(const ENetEvent& event, Bot& bot);

    void getPing(Bot& bot);

    //Send Packet Function
    void sendPacket(const std::string& text, ePacketType type, Bot& bot);
    void sendPacket(const std::string& text, int type, Bot& bot);
    void sendPacketRaw(uint8_t* ptr, Bot& bot);

    //Handle Packet
    void handleHelloPacket(Bot& bot);
    void handleGameMessage(const std::vector<uint8_t>& data, Bot& bot);
    void handleTrackPacket(ENetPacket* packet, Bot& bot);

    //Game Packet
    void handleGamePacket(uint8_t* ptr, Bot& bot);
    void handleGamePacket_CallFunction(uint8_t* ptr, Bot& bot);
    void callPlayerSpawn(std::string spawn, Bot& bot);
    void callPlayerRemove(std::string remove, Bot& bot);
    void packetState(TankPacketStruct* packet, Bot& bot);
    void inventoryState(uint8_t* ptr, Bot& bot);
    void ModifyInventory(TankPacketStruct* packet, Bot& bot);
    void ChangeItemObjects(uint8_t* ptr, Bot& bot); 
    void OnTileChangeRequest(TankPacketStruct* packet,uint8_t* ptr, Bot& bot);
    void OnTileTreeState(TankPacketStruct* packet, Bot& bot);
    void OnTileUpdateData(TankPacketStruct* packet, uint8_t* ptr, Bot& bot);

    //EXECUTOR
    void say(std::string message, Bot& bot);
    void warp(std::string name, Bot& bot);
    void wear(int id, Bot& bot);
    void fav(int id, Bot& bot);
    void trash(int id, int amount, Bot& bot);
    void drop(int id, int amount, Bot& bot);
    void collectRaw(int x, int y, int oid, Bot& bot);
    void collect(int range, Bot& bot);
    void collectSet(int range, int interval, vector<int>& id, Bot& bot);
    void walk(int x, int y, bool ap, Bot& bot);
    void punch(int x, int y, Bot& bot);
    void place(int x, int y, int id, Bot& bot);
    void wrench(int x, int y, Bot& bot);
    void wrenchPeople(int netid, Bot& bot);
    std::vector<std::pair<int, int>> get_neighbors(int x, int y, Bot& bot);
    bool isWalkable(int x, int y, Bot& bot);
    int calculate_h(int from_x, int from_y, int to_x, int to_y);
    std::vector<std::pair<int, int>> find_path(int x, int y, Bot& bot); //Node 
    bool findPath(int x, int y, Bot& bot); //Main findPath 
    void updateCoordinat(Bot& bot);
    void mainScript(Bot& bot);
};

#include "handlePacket.h"
#include "Packet/HelloPacket/helloPacket.h"
#include "Packet/GameMessage/gameMessage.h"
#include "Packet/GamePacket/gamePacket.h"
#include "Packet/GamePacket/OnCall.h"
#include "Packet/TrackPacket/trackPacket.h"
#include "Packet/Executor/executor.h"

void UDP::resetData(Bot& bot) {
    bot.local.world.dropped.items.clear();
    bot.local.world.tiles.clear();
    bot.local.items.clear();
    bot.local.players.clear();
}

void UDP::disconnectClient(Bot& bot)
{
    if (bot.peer)
    {
        enet_peer_disconnect(bot.peer, 0);
        enet_host_flush(bot.client);
        enet_peer_reset(bot.peer);
    }
    if (bot.client)
    {
        enet_host_destroy(bot.client);
        enet_deinitialize();
    }
    UDP::resetData(bot);
    bot.local.world.reset();
}

void UDP::getPing(Bot& bot) {
    if (bot.peer) {
        bot.local.ping = bot.peer->roundTripTime;
    }
    else {
        bot.local.status = "offline";
        bot.local.ping = 500;
    }
}

void UDP::connectClient(Bot& bot) {
    bot.local.world.reset();
    bot.local.status = "connecting to server";
    if (enet_initialize() != 0)
    {
        std::cerr << "\n[FAILED TO INITIALIZE ENET]" << std::endl;
        exit(EXIT_FAILURE);
    }
    bot.client = enet_host_create(nullptr, 1, 2, 0, 0);
    if (bot.client == nullptr)
    {
        std::cout << "\n[FAILED TO CREATE CLIENT]" << std::endl;
        exit(EXIT_FAILURE);
    } 
    bot.client->usingNewPacket = true;
    bot.client->checksum = enet_crc32;
    enet_host_compress_with_range_coder(bot.client);
    ENetAddress address;
    if (enet_address_set_host(&address, bot.local.server.EnetNowIp.c_str()) != 0)
    {
        std::cerr << "\n[FAILED TO CONNECT TO SERVER]:" << bot.local.server.EnetNowIp.c_str() << std::endl;
        enet_host_destroy(bot.client);
        exit(EXIT_FAILURE);
    }
    address.port = bot.local.server.EnetNowPort;
    bot.peer = enet_host_connect(bot.client, &address, 2, 0);
    if (bot.peer == nullptr)
    {
        std::cerr << "\n[NO PEER CONNECTED]" << std::endl;
        enet_host_destroy(bot.client);
        exit(EXIT_FAILURE);
    }
    enet_host_flush(bot.client);
    std::cout << "\n[CONNECTION TO SERVER ESTABLISHED]" << std::endl;
    logger(bot, "[SYSTEM]: CONNECTION TO SERVER ESTABLISHED: " + bot.local.server.EnetNowIp + ":" + std::to_string(bot.local.server.EnetNowPort));
}

char ipAddress[64];
int port;

void UDP::eventListener(Bot& bot) {
    while (true and bot.shouldRun) {
        auto lastResponseTime = std::chrono::steady_clock::now();
        if (!bot.shouldRun) {
            break;
        }
        while (bot.shouldRun) {
            UDP::getPing(bot);
            ENetEvent event;
            int eventStatus = enet_host_service(bot.client, &event, 500);
            if (eventStatus > 0) {
                lastResponseTime = std::chrono::steady_clock::now(); // Reset timer jika ada respon dari server
                if (!bot.shouldRun) {
                    break;
                }
                switch (event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    UDP::getPing(bot);
                    enet_address_get_host_ip(&event.peer->address, ipAddress, sizeof(ipAddress));
                    port = event.peer->address.port;
                    std::cout << "\n[CONNECTED TO SERVER] IP: " << ipAddress << " | PORT: " << port << std::endl;
                    logger(bot, "[SYSTEM]: CONNECTED TO SERVER - IP: " + std::string(ipAddress) + " | PORT: " + std::to_string(port));
                    break;

                case ENET_EVENT_TYPE_DISCONNECT:
                    std::cout << "\n[DISCONNECTED FROM SERVER] IP: " << ipAddress << " | PORT: " << port << std::endl;
                    logger(bot, "[SYSTEM]: DISCONNECTED FROM SERVER - IP: " + std::string(ipAddress) + " | PORT: " + std::to_string(port));
                    bot.local.status = "offline";
                    UDP::disconnectClient(bot);
                    break;

                case ENET_EVENT_TYPE_RECEIVE:
                    UDP::getPing(bot);
                    UDP::OnReceive(event, bot);
                    enet_packet_destroy(event.packet);
                    break;
                default:
                    break;
                }
            }
            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::seconds>(now - lastResponseTime).count() >= bot.pingInterval) {
                TankPacketStruct tank;
                tank.type = NET_GAME_PACKET_PING_REQUEST;
                tank.x = 0;
                tank.y = 0;
                UDP::sendPacketRaw((uint8_t*)&tank, bot);
            }
            if (std::chrono::duration_cast<std::chrono::seconds>(now - lastResponseTime).count() > bot.timeOut and bot.local.status != "UPDATE" and bot.local.status != "suspended" and bot.local.status != "temporarily suspended" and bot.local.status != "banned" and bot.local.status != "maintenance") {
                bot.local.status = "offline";
                bot.local.ping = 500;
            }
            if (bot.local.server.EnetNowIp != bot.local.server.EnetSubServerIP and bot.local.status != "UPDATE" and bot.local.status != "suspended" and bot.local.status != "temporarily suspended" and bot.local.status != "banned" and bot.local.status != "maintenance" and bot.local.status != "login_fail" and bot.local.status != "currently banned") {
                bot.local.status = "changing subserver";
            }
        }
    }
}


void UDP::OnReceive(const ENetEvent& event, Bot& bot)
{
    uint8_t* packetData = event.packet->data;
    size_t dataLength = event.packet->dataLength;
    if (dataLength < 4)
    {
        std::cerr << "\n[BAD PACKET LENGTH < 4]" << std::endl;
        enet_packet_destroy(event.packet);
        return;
    }
    uint32_t packetId = 0;
    std::memcpy(&packetId, packetData, sizeof(uint32_t));
    switch (packetId)
    {
    case NET_MESSAGE_SERVER_HELLO:
    {
        UDP::handleHelloPacket(bot); // Login Request
        break;
    }
    case NET_MESSAGE_GAME_MESSAGE:
    {
        UDP::handleGameMessage(std::vector<uint8_t>(packetData + 4, packetData + dataLength), bot);
        break;
    }
    case NET_MESSAGE_GAME_PACKET:
    {
        UDP::handleGamePacket(packetData + 4, bot);
        break;
    }
    case NET_MESSAGE_ERROR:
    {
        break;
    }
    case NET_MESSAGE_TRACK:
    {
        UDP::handleTrackPacket(event.packet, bot);
        break;
    }
    case NET_MESSAGE_CLIENT_LOG_REQUEST:
    {
        break;
    }
    case NET_MESSAGE_CLIENT_LOG_RESPONSE:
        {
            break;
        }
    case NET_MESSAGE_MAX:
    {
        break;
    }
    default:
    {
        break;
    }
    }
}
#endif