#pragma once
#ifndef STRUCT_H
#define STRUCT_H

enum ePacketType : int32_t
{
    NET_MESSAGE_UNKNOWN,
    NET_MESSAGE_SERVER_HELLO,
    NET_MESSAGE_GENERIC_TEXT,
    NET_MESSAGE_GAME_MESSAGE,
    NET_MESSAGE_GAME_PACKET,
    NET_MESSAGE_ERROR,
    NET_MESSAGE_TRACK,
    NET_MESSAGE_CLIENT_LOG_REQUEST,
    NET_MESSAGE_CLIENT_LOG_RESPONSE,
    NET_MESSAGE_MAX
};

enum eTankPacketType : uint8_t
{
    NET_GAME_PACKET_STATE, //Player Move
    NET_GAME_PACKET_CALL_FUNCTION,//Variant
    NET_GAME_PACKET_UPDATE_STATUS,
    NET_GAME_PACKET_TILE_CHANGE_REQUEST, //Tile Change
    NET_GAME_PACKET_SEND_MAP_DATA, //Tile World
    NET_GAME_PACKET_SEND_TILE_UPDATE_DATA,
    NET_GAME_PACKET_SEND_TILE_UPDATE_DATA_MULTIPLE,
    NET_GAME_PACKET_TILE_ACTIVATE_REQUEST,
    NET_GAME_PACKET_TILE_APPLY_DAMAGE,
    NET_GAME_PACKET_SEND_INVENTORY_STATE,
    NET_GAME_PACKET_ITEM_ACTIVATE_REQUEST,
    NET_GAME_PACKET_ITEM_ACTIVATE_OBJECT_REQUEST,
    NET_GAME_PACKET_SEND_TILE_TREE_STATE,
    NET_GAME_PACKET_MODIFY_ITEM_INVENTORY,
    NET_GAME_PACKET_ITEM_CHANGE_OBJECT,
    NET_GAME_PACKET_SEND_LOCK,
    NET_GAME_PACKET_SEND_ITEM_DATABASE_DATA,
    NET_GAME_PACKET_SEND_PARTICLE_EFFECT,
    NET_GAME_PACKET_SET_ICON_STATE,
    NET_GAME_PACKET_ITEM_EFFECT,
    NET_GAME_PACKET_SET_CHARACTER_STATE, //Player Move
    NET_GAME_PACKET_PING_REPLY, //Ping 
    NET_GAME_PACKET_PING_REQUEST, //ping
    NET_GAME_PACKET_GOT_PUNCHED,
    NET_GAME_PACKET_APP_CHECK_RESPONSE,//OnSuperMain
    NET_GAME_PACKET_APP_INTEGRITY_FAIL,
    NET_GAME_PACKET_DISCONNECT, //dc
    NET_GAME_PACKET_BATTLE_JOIN,
    NET_GAME_PACKET_BATTLE_EVENT,
    NET_GAME_PACKET_USE_DOOR,
    NET_GAME_PACKET_SEND_PARENTAL,
    NET_GAME_PACKET_GONE_FISHIN,
    NET_GAME_PACKET_STEAM,
    NET_GAME_PACKET_PET_BATTLE,
    NET_GAME_PACKET_NPC,
    NET_GAME_PACKET_SPECIAL,
    NET_GAME_PACKET_SEND_PARTICLE_EFFECT_V2,
    NET_GAME_PACKET_ACTIVATE_ARROW_TO_ITEM,
    NET_GAME_PACKET_SELECT_TILE_INDEX,
    NET_GAME_PACKET_SEND_PLAYER_TRIBUTE_DATA,
    NET_GAME_PACKET_FTUE_SET_ITEM_TO_QUICK_INVENTORY,
    NET_GAME_PACKET_PVE_NPC,
    NET_GAME_PACKET_PVP_CARD_BATTLE,
    NET_GAME_PACKET_PVE_APPLY_PLAYER_DAMAGE,
    NET_GAME_PACKET_PVE_NPC_POSITION_UPDATE,
    NET_GAME_PACKET_SET_EXTRA_MODS,
    NET_GAME_PACKET_ON_STEP_TILE_MOD
};

struct TankPacketStruct
{
    eTankPacketType type;
    uint8_t dropped = 0; //unk 1
    uint8_t padding1 = 0; //unk 2
    uint8_t padding2 = 0; //unk 3
    uint32_t netid = 0;
    uint32_t snetid = 0;
    uint32_t state = 0; //flags
    float padding4 = 0; //unk 6 
    uint32_t value = 0;
    float x = 0; //pos x
    float y = 0; //pos y
    float xspeed = 0;
    float yspeed = 0;
    float padding5 = 0; //unk 12
    int32_t tilex = 0; //int x
    int32_t tiley = 0; //int y
    uint32_t extDataSize = 0;
};

#include "enet/Packet/World/world.h"

struct ServerLocal {
    //HTTPS
    std::string loginFormUrl; // Can bypass
    std::string formToken;
    std::string cookievisit;
    std::string cookieactivity;
    std::string cookie1_AWSALBTG;
    std::string cookie2_AWSALBTGCORS;
    std::string cookie3_AWSALB;
    std::string cookie4_AWSALBCORS;
    std::string cookie5_XSRF;
    std::string cookie6_GAMESESSION;
    std::string statusToken;
    std::string ltoken; //Login Token

    //Enet Server
    std::string EnetFirstIP;
    int EnetFirstPort = 0;
    std::string EnetSubServerIP;
    int EnetSubServerPort = 0;
    std::string EnetNowIp;
    int EnetNowPort = 0;
};

struct BotLoginPacket {
    std::string tankIdName;
    std::string tankIdPass;
    std::string requestedName = "";
    std::string f = "1";
    std::string protocol = "213";
    std::string game_version = "5.06";
    std::string fz = "21562392"; 
    std::string cbits = "1024";
    std::string player_age = "17";
    std::string GDPR = "1";
    std::string category = "_-5100";
    std::string total_playtime = "0";
    std::string klv;
    std::string hash2;
    std::string meta;
    std::string fhash = "-716928004";
    std::string rid;
    std::string platformID = "0,1,1";
    std::string device_version = "0";
    std::string country = "id";
    std::string hash;
    std::string mac;
    std::string wk;
    std::string zf = "1010550505";
    std::string lmode = "1";
    // Entering  Subserver
    std::string user;
    std::string userToken;
    std::string UUIDToken;
    std::string doorID;
    std::string aat = "2";
    // LoginPaket
    std::string loginPacket;
};

struct Players {
    std::string name;
    std::string country;
    int userid;
    int netid;
    std::string eid;
    std::string ip;
    vector2_t pos;
    float speedX = 0, speedY = 0;
    bool isLocal = false;
    std::vector<int> ActiveItems;
};

struct Inventory {
    short id;
    uint8_t count;
    bool isFavorite = false;
    bool isActive = false;
};

struct Local {
    std::string name;
    std::string password;
    std::string country;
    std::string eid;
    std::string ip;
    int netid = -1;
    int userid = -1;
    int gems = 0;
    int level = 0;
    vector2_t pos;
    float speedX = 0, speedY = 0;
    std::string HTTPstatus;
    std::string status;
    int ping = 500;
    int inventorySlots = 0;
    float totalPlaytime = 0;

    //Track
    int Awesomeness = 0;
    int Global_Playtime = 0;
    int world_lock = 0;
    std::vector<int> FavoriteItems;
    std::vector<int> ActiveItems;

    std::vector<Players> players;
    std::vector<Inventory> items;
    World world;

    ServerLocal server;
    BotLoginPacket botloginpacket;

    std::vector<std::string> allDebug;
    std::vector<std::string> onGenerixText;
    std::vector<std::string> gameMessageDebug;
    std::vector<std::string> tankPacketDebug;
    std::vector<variantlist_t> onVariantListDebug;
    std::vector<std::string> trackPacketDebug;
};

struct Bot {
    Local local;
    
    bool shouldRun = false;
    bool isRunning = false;

    //Json Protocol And Version
    void readConfigJson(Bot& bot);

    //Enet Connection
    ENetHost* client = nullptr;
    ENetPeer* peer = nullptr;

    int pingInterval = 24; //Second
    int timeOut = 30;
};

bool isParsingItems = true;
char username[128] = "";
char password[128] = "";
char requestWorld[128] = "";
int maxBot = 3;
std::vector<Bot> bot;
std::map<std::string, std::thread> activeThreads;
std::string selectedBot = "";

static bool auto_collect = false;
static bool auto_collect_set = false;
static bool collect_thread_running = false;
static int collect_range = 5;
static int collect_interval = 200;
static char id_input[256] = "";

int xFp = 0;
int yFp = 0;

#endif // !STRUCT_H