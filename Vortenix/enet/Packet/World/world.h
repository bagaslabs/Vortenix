#pragma once
#include <array>
#include <cstdint>
#include <string>
#include <variant>
#include <vector>
#include <chrono>
#include <cstring> // Untuk std::memcpy
#include <unordered_map>

using namespace std;
using namespace chrono;

struct TileFlags {
    bool has_extra_data;
    bool has_parent;
    bool was_spliced;
    bool will_spawn_seeds_too;
    bool is_seedling;
    bool flipped_x;
    bool is_on;
    bool is_open_to_public;
    bool bg_is_on;
    bool fg_alt_mode;
    bool is_wet;
    bool glued;
    bool on_fire;
    bool painted_red;
    bool painted_green;
    bool painted_blue;

    static TileFlags from_u16(uint16_t value) {
        return TileFlags{
            static_cast<bool>(value & 0x01),
            static_cast<bool>(value & 0x02),
            static_cast<bool>(value & 0x04),
            static_cast<bool>(value & 0x08),
            static_cast<bool>(value & 0x10),
            static_cast<bool>(value & 0x20),
            static_cast<bool>(value & 0x40),
            static_cast<bool>(value & 0x80),
            static_cast<bool>(value & 0x100),
            static_cast<bool>(value & 0x200),
            static_cast<bool>(value & 0x400),
            static_cast<bool>(value & 0x800),
            static_cast<bool>(value & 0x1000),
            static_cast<bool>(value & 0x2000),
            static_cast<bool>(value & 0x4000),
            static_cast<bool>(value & 0x8000)
        };
    }

    uint16_t to_u16() const {
        uint16_t value = 0;
        if (has_extra_data) value |= 0x01;
        if (has_parent) value |= 0x02;
        if (was_spliced) value |= 0x04;
        if (will_spawn_seeds_too) value |= 0x08;
        if (is_seedling) value |= 0x10;
        if (flipped_x) value |= 0x20;
        if (is_on) value |= 0x40;
        if (is_open_to_public) value |= 0x80;
        if (bg_is_on) value |= 0x100;
        if (fg_alt_mode) value |= 0x200;
        if (is_wet) value |= 0x400;
        if (glued) value |= 0x800;
        if (on_fire) value |= 0x1000;
        if (painted_red) value |= 0x2000;
        if (painted_green) value |= 0x4000;
        if (painted_blue) value |= 0x8000;
        return value;
    }
};

struct Basic {
};

struct Door {
    string text;
    uint8_t unknown_1;
};

struct Sign {
    string text;
};

struct Lock {
    uint8_t settings;
    uint32_t owner_uid;
    uint32_t access_count;
    vector<uint32_t> access_uids;
    uint8_t minimum_level;
};

struct Seed {
    uint32_t time_passed;
    uint8_t item_on_tree;
    uint16_t seedID;

    std::chrono::steady_clock::time_point elapsed = std::chrono::steady_clock::now();
    bool ready_to_harvest() const {
        auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - elapsed
        ).count();
        if (time_passed + elapsed_time >= itemDefs->getGrowtimeByID(seedID)) {
            return true;
        }
        else {
            return false;
        }
    }
};

struct Mailbox {
    string unknown_1, unknown_2, unknown_3;
    uint8_t unknown_4;
};

struct Bulletin {
    string unknown_1, unknown_2, unknown_3;
    uint8_t unknown_4;
};

struct Dice {
    uint8_t symbol;
};

struct ChemicalSource {
    uint32_t time_passed;
    bool ready_to_harvest;
    duration<uint32_t> elapsed;
};

struct AchievementBlock {
    uint32_t unknown_1;
    uint8_t tile_type;
};

struct HearthMonitor {
    uint32_t unknown_1;
    string player_name;
};

struct DonationBox {
    string unknown_1, unknown_2, unknown_3;
    uint8_t unknown_4;
};

struct Mannequin {
    std::string text;
    uint8_t unknown_1;
    uint32_t clothing_1;
    uint16_t clothing_2;
    uint16_t clothing_3;
    uint16_t clothing_4;
    uint16_t clothing_5;
    uint16_t clothing_6;
    uint16_t clothing_7;
    uint16_t clothing_8;
    uint16_t clothing_9;
    uint16_t clothing_10;
};

struct BunnyEgg {
    uint32_t egg_placed;
};

struct GamePack {
    uint8_t team;
};

struct GameGenerator{};

struct XenoniteCrystal {
    uint8_t unknown_1;
    uint32_t unknown_2;
};

struct PhoneBooth {
    uint16_t clothing[9];
};

struct Crystal {
    string unknown_1;
};

struct CrimeInProgress {
    string unknown_1;
    uint32_t unknown_2;
    uint8_t unknown_3;
};

struct DisplayBlock {
    uint32_t item_id;
};

struct VendingMachine {
    uint32_t item_id;
    int32_t price;
};

struct GivingTree{
    uint16_t unknown_1;
    uint32_t unknown_2;
};

struct CountryFlag {
    string country;
};

struct WeatherMachine {
    uint32_t settings;
};

struct DataBedrock {};
struct Spotlight{};

struct FishInfo {
    uint32_t fish_item_id;
    uint32_t lbs;
};

struct FishTankPort {
    uint8_t flags;
    vector<FishInfo> fishes;
};

struct SolarCollector {
    std::array<uint8_t, 5> unknown_1;
};

struct Forge {
    uint32_t temperature;
};

struct SteamOrgan {
    uint8_t instrument_type;
    uint32_t note;
};

struct SilkWormColor {
    uint8_t a;
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct SilkWorm {
    uint8_t type_;
    string name;
    uint32_t age, unknown_1, unknown_2;
    uint8_t can_be_fed;
    SilkWormColor color;
    uint32_t sick_duration;
};

struct SewingMachine {
    std::vector<uint32_t> bolt_id_list;
};

struct PaintingEasel {
    uint32_t item_id;
    string label;
};

struct PetBattleCage {
    string label;
    uint32_t base_pet, combined_pet_1, combined_pet_2;
};

struct PetTrainer {
    string name;
    uint32_t pet_total_count, unknown_1;
    vector<uint32_t> pets_id;
};

struct SteamEngine {
    uint32_t temperature;
};

struct LockBot {
    uint32_t time_passed;
};

struct Shelf {
    uint32_t top_left_item_id, top_right_item_id, bottom_left_item_id, bottom_right_item_id;
};

struct VipEntrance {
    uint8_t unknown_1;
    uint32_t owner_uid;
    vector<uint32_t> access_uids;
};

struct FishWallMount {
    string label;
    uint32_t item_id;
    uint8_t lb;
};

struct Portrait {
    std::string label;
    uint32_t unknown_1;
    uint32_t unknown_2;
    uint32_t unknown_3;
    uint32_t unknown_4;
    uint16_t face;
    uint16_t hat;
    uint16_t mask;
    uint16_t unknown_5;
    uint16_t unknown_6;
};

struct FossilPrepStation {
    uint32_t unknown_1;
};

struct ChemsynthTank {
    uint32_t current_chem, target_chem;
};

struct StorageBlockItemInfo {
    uint32_t id;
    uint32_t amount;
};

struct StorageBlock {
    vector<StorageBlockItemInfo> items;
};

struct CookingOvenIngredientInfo {
    uint32_t item_id;
    uint32_t time_added;
};

struct CookingOven {
    uint32_t temperature_level;
    vector<CookingOvenIngredientInfo> ingredients;
    uint32_t unknown_1, unknown_2, unknown_3;
};

struct AudioRack {
    string note;
    uint32_t volume;
};

struct GeigerCharger {
    uint32_t unknown_1;
};

struct BalloonOMatic {
    uint32_t total_rarity;
    uint8_t team_type;
};

struct TrainingPort {
    uint32_t fish_lb, fish_status, fish_id, fish_total_exp, fish_level, unknown_2;
};

struct ItemSucker {
    uint32_t item_id_to_suck, item_amount;
    uint16_t flags;
    uint32_t limit;
};

struct CyBotCommandData {
    uint32_t command_id;
    uint32_t is_command_used;
};

struct CyBot {
    uint32_t sync_timer, activated;
    vector<CyBotCommandData> command_datas;
};

struct InfinityWeatherMachine {
    uint32_t interval_minutes;
    vector<uint32_t> weather_machine_list;
};

struct KrakenGalaticBlock {
    uint8_t pattern_index;
    uint32_t unknown_1;
    uint8_t r, g, b;
};

struct FriendsEntrance {
    uint32_t owner_user_id;
    uint16_t unknown_1, unknown_2;
};

struct LobsterTrap{};
struct ChallangeTimer{};
struct DnaExtractor{};
struct Howler{};
struct AdventureBegins {};
struct TombRobber{};

struct SpiritStorageUnit {
    uint32_t ghost_jar_count;
};

struct GuildWeatherMachine {
    uint32_t unknown_1;
    uint32_t gravity;
    uint8_t flags;
};

struct GuildItem {};

struct Growscan {
    uint8_t unknown_1;
};

struct ContainmentFieldPowerNode{
    uint32_t ghost_jar_count;
    vector<uint32_t> unknown_1;
};

struct SpiritBoard {
    uint32_t unknown_1;
    uint32_t unknown_2;
    uint32_t unknown_3;
};

struct StormyCloud {
    uint32_t sting_duration;
    uint32_t is_solid;
    uint32_t non_solid_duration;
};

struct TemporaryPlatform {
    uint32_t unknown_1;
};

struct SafeVault {};

struct AngelicCountingCloud {
    uint32_t is_raffling;
    uint16_t unknown_1;
    uint8_t ascii_code;
};

struct PineappleGuzzler{};

using TileType = variant<Basic, Door, Sign, Lock, Seed, Mailbox, Bulletin, Dice, ChemicalSource, AchievementBlock, HearthMonitor, DonationBox, Mannequin, BunnyEgg, GamePack, GameGenerator, XenoniteCrystal, PhoneBooth, Crystal, CrimeInProgress, DisplayBlock, VendingMachine, GivingTree, CountryFlag, WeatherMachine, DataBedrock, Spotlight, FishTankPort, SolarCollector, Forge, SteamOrgan, SilkWorm, SewingMachine, LobsterTrap, PaintingEasel, PetBattleCage, PetTrainer, SteamEngine, LockBot, SpiritStorageUnit, Shelf, VipEntrance, ChallangeTimer, FishWallMount, Portrait, GuildWeatherMachine, FossilPrepStation, 
    DnaExtractor, Howler, ChemsynthTank, StorageBlock, CyBotCommandData, CookingOven, AudioRack, GeigerCharger, AdventureBegins, TombRobber, BalloonOMatic, TrainingPort, ItemSucker, CyBot, GuildItem, Growscan, ContainmentFieldPowerNode, SpiritBoard, StormyCloud, TemporaryPlatform, SafeVault, AngelicCountingCloud, InfinityWeatherMachine, PineappleGuzzler, KrakenGalaticBlock, FriendsEntrance, CookingOvenIngredientInfo>;

std::string TileTypeToString(const TileType& tileType) {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, Basic>) return "Basic";
        else if constexpr (std::is_same_v<T, Door>) return "Door";
        else if constexpr (std::is_same_v<T, Sign>) return "Sign";
        else if constexpr (std::is_same_v<T, Lock>) return "Lock";
        else if constexpr (std::is_same_v<T, Seed>) return "Seed";
        else if constexpr (std::is_same_v<T, Mailbox>) return "Mailbox";
        else if constexpr (std::is_same_v<T, Bulletin>) return "Bulletin";
        else if constexpr (std::is_same_v<T, Dice>) return "Dice";
        else if constexpr (std::is_same_v<T, ChemicalSource>) return "ChemicalSource";
        else if constexpr (std::is_same_v<T, AchievementBlock>) return "AchievementBlock";
        else if constexpr (std::is_same_v<T, HearthMonitor>) return "HearthMonitor";
        else if constexpr (std::is_same_v<T, DonationBox>) return "DonationBox";
        else if constexpr (std::is_same_v<T, Mannequin>) return "Mannequin";
        else if constexpr (std::is_same_v<T, BunnyEgg>) return "BunnyEgg";
        else if constexpr (std::is_same_v<T, GamePack>) return "GamePack";
        else if constexpr (std::is_same_v<T, GameGenerator>) return "GameGenerator";
        else if constexpr (std::is_same_v<T, XenoniteCrystal>) return "XenoniteCrystal";
        else if constexpr (std::is_same_v<T, PhoneBooth>) return "PhoneBooth";
        else if constexpr (std::is_same_v<T, Crystal>) return "Crystal";
        else if constexpr (std::is_same_v<T, CrimeInProgress>) return "CrimeInProgress";
        else if constexpr (std::is_same_v<T, DisplayBlock>) return "DisplayBlock";
        else if constexpr (std::is_same_v<T, VendingMachine>) return "VendingMachine";
        else if constexpr (std::is_same_v<T, GivingTree>) return "GivingTree";
        else if constexpr (std::is_same_v<T, CountryFlag>) return "CountryFlag";
        else if constexpr (std::is_same_v<T, WeatherMachine>) return "WeatherMachine";
        else if constexpr (std::is_same_v<T, DataBedrock>) return "DataBedrock";
        else if constexpr (std::is_same_v<T, Spotlight>) return "Spotlight";
        else if constexpr (std::is_same_v<T, FishTankPort>) return "FishTankPort";
        else if constexpr (std::is_same_v<T, SolarCollector>) return "SolarCollector";
        else if constexpr (std::is_same_v<T, Forge>) return "Forge";
        else if constexpr (std::is_same_v<T, SteamOrgan>) return "SteamOrgan";
        else if constexpr (std::is_same_v<T, SilkWorm>) return "SilkWorm";
        else if constexpr (std::is_same_v<T, SewingMachine>) return "SewingMachine";
        else if constexpr (std::is_same_v<T, LobsterTrap>) return "LobsterTrap";
        else if constexpr (std::is_same_v<T, PaintingEasel>) return "PaintingEasel";
        else if constexpr (std::is_same_v<T, PetBattleCage>) return "PetBattleCage";
        else if constexpr (std::is_same_v<T, PetTrainer>) return "PetTrainer";
        else if constexpr (std::is_same_v<T, SteamEngine>) return "SteamEngine";
        else if constexpr (std::is_same_v<T, LockBot>) return "LockBot";
        else if constexpr (std::is_same_v<T, SpiritStorageUnit>) return "SpiritStorageUnit";
        else if constexpr (std::is_same_v<T, Shelf>) return "Shelf";
        else if constexpr (std::is_same_v<T, VipEntrance>) return "VipEntrance";
        else if constexpr (std::is_same_v<T, ChallangeTimer>) return "ChallangeTimer";
        else if constexpr (std::is_same_v<T, FishWallMount>) return "FishWallMount";
        else if constexpr (std::is_same_v<T, Portrait>) return "Portrait";
        else if constexpr (std::is_same_v<T, GuildWeatherMachine>) return "GuildWeatherMachine";
        else if constexpr (std::is_same_v<T, FossilPrepStation>) return "FossilPrepStation";
        else if constexpr (std::is_same_v<T, DnaExtractor>) return "DnaExtractor";
        else if constexpr (std::is_same_v<T, Howler>) return "Howler";
        else if constexpr (std::is_same_v<T, ChemsynthTank>) return "ChemsynthTank";
        else if constexpr (std::is_same_v<T, StorageBlock>) return "StorageBlock";
        else if constexpr (std::is_same_v<T, CyBotCommandData>) return "CyBotCommandData";
        else if constexpr (std::is_same_v<T, CookingOven>) return "CookingOven";
        else if constexpr (std::is_same_v<T, AudioRack>) return "AudioRack";
        else if constexpr (std::is_same_v<T, GeigerCharger>) return "GeigerCharger";
        else if constexpr (std::is_same_v<T, AdventureBegins>) return "AdventureBegins";
        else if constexpr (std::is_same_v<T, TombRobber>) return "TombRobber";
        else if constexpr (std::is_same_v<T, BalloonOMatic>) return "BalloonOMatic";
        else if constexpr (std::is_same_v<T, TrainingPort>) return "TrainingPort";
        else if constexpr (std::is_same_v<T, ItemSucker>) return "ItemSucker";
        else if constexpr (std::is_same_v<T, CyBot>) return "CyBot";
        else if constexpr (std::is_same_v<T, GuildItem>) return "GuildItem";
        else if constexpr (std::is_same_v<T, Growscan>) return "Growscan";
        else if constexpr (std::is_same_v<T, ContainmentFieldPowerNode>) return "ContainmentFieldPowerNode";
        else if constexpr (std::is_same_v<T, SpiritBoard>) return "SpiritBoard";
        else if constexpr (std::is_same_v<T, StormyCloud>) return "StormyCloud";
        else if constexpr (std::is_same_v<T, TemporaryPlatform>) return "TemporaryPlatform";
        else if constexpr (std::is_same_v<T, SafeVault>) return "SafeVault";
        else if constexpr (std::is_same_v<T, AngelicCountingCloud>) return "AngelicCountingCloud";
        else if constexpr (std::is_same_v<T, InfinityWeatherMachine>) return "InfinityWeatherMachine";
        else if constexpr (std::is_same_v<T, PineappleGuzzler>) return "PineappleGuzzler";
        else if constexpr (std::is_same_v<T, KrakenGalaticBlock>) return "KrakenGalaticBlock";
        else if constexpr (std::is_same_v<T, FriendsEntrance>) return "FriendsEntrance";
        else if constexpr (std::is_same_v<T, CookingOvenIngredientInfo>) return "CookingOvenIngredientInfo";
        else return "Unknown";  // Jika tipe tidak ditemukan
        }, tileType);
}

struct DroppedItem {
    uint16_t id;
    float x;
    float y;
    uint8_t count;
    uint8_t flags;
    uint32_t uid;
};

struct Dropped {
    uint32_t items_count;
    uint32_t last_dropped_item_uid;
    std::vector<DroppedItem> items;
};

struct Tile {
    uint16_t foreground_item_id;
    uint16_t background_item_id;
    uint16_t parent_block_index;
    TileFlags flags;
    uint16_t flags_number;
    TileType tile_type;
    uint32_t x;
    uint32_t y;
};

class World {
public:
    std::string name = "EXIT";
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t tile_count = 0;
    std::vector<Tile> tiles;
    Dropped dropped;
    uint16_t base_weather = 0;
    uint16_t current_weather = 0;

    void reset();
    void parse(uint8_t* data, size_t extData);
    void get_extra_tile_data(Tile& tile, uint8_t* data, size_t& position, uint8_t extra_tile_type);

    std::string parseStatus = "none";
};

void World::reset() {
    name = "EXIT";
    width = 0;
    height = 0;
    tile_count = 0;
    tiles.clear();
    dropped.items.clear();
    dropped.items_count = 0;
    dropped.last_dropped_item_uid = 0;
    base_weather = 0;
    current_weather = 0;
}

void World::parse(uint8_t* data, size_t extData) {
    reset();
    /*
    json jsonData;
    for (size_t j = 0; j < extData; j++) {
        uint8_t data8 = *reinterpret_cast<const uint8_t*>(&data[j]);
        uint16_t data16 = *reinterpret_cast<const uint16_t*>(&data[j]);
        uint32_t data32 = *reinterpret_cast<const uint32_t*>(&data[j]);
        json entry = {
            {"index", j},
            {"data8", data8},
            {"data16", data16},
            {"data32", data32}
        };
        jsonData.push_back(entry);
    }
    std::ofstream file("output.json");
    if (file.is_open()) {
        file << jsonData.dump(4); // 4 = indentasi biar rapi
        file.close();
        std::cout << "Data tersimpan dalam output.json\n";
    }
    else {
        std::cerr << "Gagal membuka file!\n";
    }
    */
    try {
        parseStatus = "none";
        size_t position = 0;
        position += 6;  // Skip first 6 unknown bytes

        // read the world name
        uint16_t str_len = *reinterpret_cast<const uint16_t*>(&data[position]);
        position += sizeof(uint16_t);

        this->name.assign(reinterpret_cast<const char*>(&data[position]), str_len);
        position += str_len;

        // read width, height, tile_count
        this->width = *reinterpret_cast<const uint32_t*>(&data[position]);
        position += sizeof(uint32_t);

        this->height = *reinterpret_cast<const uint32_t*>(&data[position]);
        position += sizeof(uint32_t);

        this->tile_count = *reinterpret_cast<const uint32_t*>(&data[position]);
        position += sizeof(uint32_t);

        position += 5; // skip 5 unknown bytes

        json map_json;
        // parse tiles
        for (uint32_t i = 0; i < this->tile_count; ++i) {
            Tile tile;

            tile.foreground_item_id = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);

            tile.background_item_id = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);

            tile.parent_block_index = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);

            tile.x = i % this->width;
            tile.y = i / this->width;

            tile.flags_number = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);
            tile.flags = TileFlags::from_u16(tile.flags_number);

            if (tile.flags.has_parent) {
                uint16_t parent = *reinterpret_cast<const uint16_t*>(&data[position]);
                position += sizeof(uint16_t);
            }

            if (tile.flags.has_extra_data) {
                uint8_t extra_tile_type = *reinterpret_cast<const uint8_t*>(&data[position]);
                position += sizeof(uint8_t);
                this->get_extra_tile_data(tile, data, position, extra_tile_type);
            }

            if (tile.foreground_item_id == 14666) {
                uint32_t extra_len = *reinterpret_cast<const uint32_t*>(&data[position]);
                position += sizeof(uint32_t);
                std::string text(reinterpret_cast<const char*>(&data[position]), extra_len);
                position += extra_len;
            }
            /*
            std::cout << "[Berhasil Akses Tile ke " << i << " (" << position << ") < (" << extData << ")" << "]\n1.Foreground And Background:" << tile.foreground_item_id << " : " << tile.background_item_id << "\nX,Y: " << tile.x << " : " << tile.y << "\nType: " << tile.tile_type << std::endl;
            // push tile

            json tile_json = {
        {"index", i},
        {"position", position},
        {"extData", extData},
        {"foreground_item_id", tile.foreground_item_id},
        {"background_item_id", tile.background_item_id},
        {"x", tile.x},
        {"y", tile.y},
        {"Flags Number", tile.flags_number},
            };
            map_json.push_back(tile_json);

            */
            this->tiles.push_back(tile);
        }
        /*
        std::ofstream file("map.json");
        if (file.is_open()) {
            file << map_json.dump(4); // Indentasi agar lebih rapi
            file.close();
            std::cout << "Data tersimpan dalam map.json\n";
        }
        else {
            std::cerr << "Gagal membuka file!\n";
        }
        */
        // skip 12 unknown bytes
        position += 12;

        // read dropped item info
        this->dropped.items_count = *reinterpret_cast<const uint32_t*>(&data[position]);
        position += sizeof(uint32_t);

        this->dropped.last_dropped_item_uid = *reinterpret_cast<const uint32_t*>(&data[position]);
        position += sizeof(uint32_t);

        for (uint32_t i = 0; i < this->dropped.items_count; ++i) {
            DroppedItem item;
            item.id = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);

            item.x = *reinterpret_cast<const float*>(&data[position]);
            position += sizeof(float);

            item.y = *reinterpret_cast<const float*>(&data[position]);
            position += sizeof(float);

            item.count = data[position];
            position += sizeof(uint8_t);

            item.flags = data[position];
            position += sizeof(uint8_t);

            item.uid = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);

            this->dropped.items.push_back(item);
        }

        // read base_weather, skip 2 unknown bytes, read current_weather
        uint16_t base_weather_val = *reinterpret_cast<const uint16_t*>(&data[position]);
        position += sizeof(uint16_t);

        position += sizeof(uint16_t); // skip unknown

        uint16_t current_weather_val = *reinterpret_cast<const uint16_t*>(&data[position]);
        position += sizeof(uint16_t);

        this->base_weather = base_weather_val;
        this->current_weather = current_weather_val;
        parseStatus = "success";
    } 
    catch (const std::exception& e) {
        std::cerr << "Exception in World::parse: " << e.what() << std::endl;
        parseStatus = "failed";
    }
    catch (...) {
        std::cerr << "Unknown exception in World::parse." << std::endl;
        parseStatus = "failed";
    }
} 


// --------------------------------------------------
// get_extra_tile_data()
// --------------------------------------------------


void World::get_extra_tile_data(Tile& tile, uint8_t* data, size_t& position, uint8_t extra_tile_type) {
        switch (extra_tile_type) {
        case 1: { // TileType::Door
            uint16_t str_len = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);

            std::string text(reinterpret_cast<const char*>(&data[position]), str_len);
            position += str_len;

            uint8_t unknown_1 = data[position];
            position += sizeof(uint8_t);

            tile.tile_type = Door{ text, unknown_1 };
            break;
        }
        case 2: { // TileType::Sign
            uint16_t str_len = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);

            std::string text(reinterpret_cast<const char*>(&data[position]), str_len);
            position += str_len;

            uint32_t _ = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);

            tile.tile_type = Sign{ text };
            break;
        }
        case 3: { // TileType::Lock
            uint8_t settings = data[position];
            position += sizeof(uint8_t);

            uint32_t owner_uid = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);

            uint32_t access_count = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);

            std::vector<uint32_t> access_uids;
            access_uids.reserve(access_count);
            for (uint32_t i = 0; i < access_count; ++i) {
                access_uids.push_back(*reinterpret_cast<const uint32_t*>(&data[position]));
                position += sizeof(uint32_t);
            }

            uint8_t minimum_level = data[position];
            position += sizeof(uint8_t);

            position += 7; // Skip 7 unknown bytes

            if (tile.foreground_item_id == 5814) {
                position += 16;
            }

            tile.tile_type = Lock{ settings, owner_uid, access_count, access_uids, minimum_level };
            break;
        }
        case 4: { // TileType::Seed
            uint32_t time_passed = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);

            uint8_t item_on_tree = data[position];
            position += sizeof(uint8_t);

            tile.tile_type = Seed{ time_passed, item_on_tree, tile.foreground_item_id};
            break;
        }
        case 6: { // TileType::Mailbox
            uint16_t str_len_1 = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);
            std::string unknown_1(reinterpret_cast<const char*>(&data[position]), str_len_1);
            position += str_len_1;

            uint16_t str_len_2 = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);
            std::string unknown_2(reinterpret_cast<const char*>(&data[position]), str_len_2);
            position += str_len_2;

            uint16_t str_len_3 = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);
            std::string unknown_3(reinterpret_cast<const char*>(&data[position]), str_len_3);
            position += str_len_3;

            uint8_t unknown_4 = data[position];
            position += sizeof(uint8_t);

            tile.tile_type = Mailbox{ unknown_1, unknown_2, unknown_3, unknown_4 };
            break;
        }
        case 7: { // TileType::Bulletin
            uint16_t str_len_1 = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);
            std::string unknown_1(reinterpret_cast<const char*>(&data[position]), str_len_1);
            position += str_len_1;

            uint16_t str_len_2 = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);
            std::string unknown_2(reinterpret_cast<const char*>(&data[position]), str_len_2);
            position += str_len_2;

            uint16_t str_len_3 = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);
            std::string unknown_3(reinterpret_cast<const char*>(&data[position]), str_len_3);
            position += str_len_3;

            uint8_t unknown_4 = data[position];
            position += sizeof(uint8_t);

            tile.tile_type = Bulletin{ unknown_1, unknown_2, unknown_3, unknown_4 };
            break;
        }
        case 8: { // TileType::Dice
            uint8_t symbol = data[position];
            position += sizeof(uint8_t);

            tile.tile_type = Dice{ symbol };
            break;
        }
        case 9: { // TileType::ChemicalSource
            uint32_t time_passed = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);

            bool ready_to_harvest = (time_passed >= itemDefs->getGrowtimeByID(tile.foreground_item_id));

            std::chrono::seconds elapsed(time_passed);

            tile.tile_type = ChemicalSource{ time_passed, ready_to_harvest, elapsed };
            break;
        }
        case 10: { // TileType::AchievementBlock
            uint32_t unknown_1 = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);

            uint8_t tile_type = data[position];
            position += sizeof(uint8_t);

            tile.tile_type = AchievementBlock{ unknown_1, tile_type };
            break;
        }
        case 11: { // TileType::HearthMonitor
            uint32_t unknown_1 = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);

            uint16_t str_len = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);

            std::string player_name(reinterpret_cast<const char*>(&data[position]), str_len);
            position += str_len;

            tile.tile_type = HearthMonitor{ unknown_1, player_name };
            break;
        }
        case 12: { // TileType::DonationBox
            uint16_t str_len_1 = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);
            std::string unknown_1(reinterpret_cast<const char*>(&data[position]), str_len_1);
            position += str_len_1;

            uint16_t str_len_2 = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);
            std::string unknown_2(reinterpret_cast<const char*>(&data[position]), str_len_2);
            position += str_len_2;

            uint16_t str_len_3 = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);
            std::string unknown_3(reinterpret_cast<const char*>(&data[position]), str_len_3);
            position += str_len_3;

            uint8_t unknown_4 = data[position];
            position += sizeof(uint8_t);

            tile.tile_type = DonationBox{ unknown_1, unknown_2, unknown_3, unknown_4 };
            break;
        }
        case 14: { // TileType::Mannequin
            uint16_t str_len = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);

            std::string text(reinterpret_cast<const char*>(&data[position]), str_len);
            position += str_len;

            uint8_t unknown_1 = data[position];
            position += sizeof(uint8_t);

            uint32_t clothing_1 = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);

            uint16_t clothing_2 = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);

            uint16_t clothing_3 = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);

            uint16_t clothing_4 = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);

            uint16_t clothing_5 = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);

            uint16_t clothing_6 = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);

            uint16_t clothing_7 = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);

            uint16_t clothing_8 = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);

            uint16_t clothing_9 = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);

            uint16_t clothing_10 = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);

            tile.tile_type = Mannequin{ text, unknown_1, clothing_1, clothing_2, clothing_3, clothing_4, clothing_5, clothing_6, clothing_7, clothing_8, clothing_9, clothing_10 };
            break;
        }
        case 15: { // TileType::BunnyEgg
            uint32_t egg_placed = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);

            tile.tile_type = BunnyEgg{ egg_placed };
            break;
        }
        case 16: { // TileType::GamePack
            uint8_t team = data[position];
            position += sizeof(uint8_t);

            tile.tile_type = GamePack{ team };
            break;
        }
        case 17: { // TileType::GameGenerator
            tile.tile_type = GameGenerator{};
            break;
        }
        case 18: { // TileType::XenoniteCrystal
            uint8_t unknown_1 = data[position];
            position += sizeof(uint8_t);

            uint32_t unknown_2 = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);

            tile.tile_type = XenoniteCrystal{ unknown_1, unknown_2 };
            break;
        }
        case 19: { // TileType::PhoneBooth
            uint16_t clothing_1 = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);

            uint16_t clothing_2 = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);

            uint16_t clothing_3 = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);

            uint16_t clothing_4 = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);

            uint16_t clothing_5 = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);

            uint16_t clothing_6 = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);

            uint16_t clothing_7 = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);

            uint16_t clothing_8 = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);

            uint16_t clothing_9 = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);

            tile.tile_type = PhoneBooth{
                clothing_1, clothing_2, clothing_3, clothing_4,
                clothing_5, clothing_6, clothing_7, clothing_8, clothing_9
            };
            break;
        }
        case 20: { // TileType::Crystal
            uint16_t str_len = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);

            std::string unknown_1(reinterpret_cast<const char*>(&data[position]), str_len);
            position += str_len;

            tile.tile_type = Crystal{ unknown_1 };
            break;
        }

        case 21: { // TileType::CrimeInProgress
            uint16_t str_len = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);

            std::string unknown_1(reinterpret_cast<const char*>(&data[position]), str_len);
            position += str_len;

            uint32_t unknown_2 = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);

            uint8_t unknown_3 = data[position];
            position += sizeof(uint8_t);

            tile.tile_type = CrimeInProgress{ unknown_1, unknown_2, unknown_3 };
            break;
        }

        case 23: { // TileType::DisplayBlock
            uint32_t item_id = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);

            tile.tile_type = DisplayBlock{ item_id };
            break;
        }

        case 24: { // TileType::VendingMachine
            uint32_t item_id = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);

            int32_t price = *reinterpret_cast<const int32_t*>(&data[position]);
            position += sizeof(int32_t);

            tile.tile_type = VendingMachine{ item_id, price };
            break;
        }

        case 25: { // TileType::FishTankPort
            uint8_t flags = data[position];
            position += sizeof(uint8_t);

            uint32_t fish_count = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);

            std::vector<FishInfo> fishes;
            for (uint32_t i = 0; i < fish_count / 2; i++) {
                uint32_t fish_item_id = *reinterpret_cast<const uint32_t*>(&data[position]);
                position += sizeof(uint32_t);

                uint32_t lbs = *reinterpret_cast<const uint32_t*>(&data[position]);
                position += sizeof(uint32_t);

                fishes.push_back(FishInfo{ fish_item_id, lbs });
            }

            tile.tile_type = FishTankPort{ flags, fishes };
            break;
        }

        case 26: { // TileType::SolarCollector
            std::array<uint8_t, 5> unknown_1;
            std::memcpy(unknown_1.data(), &data[position], 5);
            position += 5;

            tile.tile_type = SolarCollector{ unknown_1 };
            break;
        }

        case 27: { // TileType::Forge
            uint32_t temperature = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);

            tile.tile_type = Forge{ temperature };
            break;
        }

        case 28: { // TileType::GivingTree
            uint16_t unknown_1 = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);

            uint32_t unknown_2 = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);

            tile.tile_type = GivingTree{ unknown_1, unknown_2 };
            break;
        }

        case 30: { // TileType::SteamOrgan
            uint8_t instrument_type = data[position];
            position += sizeof(uint8_t);

            uint32_t note = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);

            tile.tile_type = SteamOrgan{ instrument_type, note };
            break;
        }
        case 31: { // TileType::SilkWorm
            uint8_t type_ = data[position];
            position += sizeof(uint8_t);

            uint16_t name_len = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);

            std::string name(reinterpret_cast<const char*>(&data[position]), name_len);
            position += name_len;

            uint32_t age = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);

            uint32_t unknown_1 = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);

            uint32_t unknown_2 = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);

            uint8_t can_be_fed = data[position];
            position += sizeof(uint8_t);

            uint32_t color = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);

            uint32_t sick_duration = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);

            tile.tile_type = SilkWorm{
                type_, name, age, unknown_1, unknown_2, can_be_fed,
                {static_cast<uint8_t>(color >> 24), static_cast<uint8_t>((color >> 16) & 0xFF),
                 static_cast<uint8_t>((color >> 8) & 0xFF), static_cast<uint8_t>(color & 0xFF)},
                sick_duration
            };
            break;
        }

        case 32: { // TileType::SewingMachine
            uint16_t bolt_len = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);

            std::vector<uint32_t> bolt_id_list;
            for (uint16_t i = 0; i < bolt_len; i++) {
                uint32_t bolt_id = *reinterpret_cast<const uint32_t*>(&data[position]);
                position += sizeof(uint32_t);
                bolt_id_list.push_back(bolt_id);
            }

            tile.tile_type = SewingMachine{ bolt_id_list };
            break;
        }

        case 33: { // TileType::CountryFlag
            uint16_t country_len = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);

            std::string country(reinterpret_cast<const char*>(&data[position]), country_len);
            position += country_len;

            tile.tile_type = CountryFlag{ country };
            break;
        }

        case 34: { // TileType::LobsterTrap
            tile.tile_type = LobsterTrap{};
            break;
        }

        case 35: { // TileType::PaintingEasel
            uint32_t item_id = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);

            uint16_t label_len = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);

            std::string label(reinterpret_cast<const char*>(&data[position]), label_len);
            position += label_len;

            tile.tile_type = PaintingEasel{ item_id, label };
            break;
        }

        case 36: { // TileType::PetBattleCage
            uint16_t label_len = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);

            std::string label(reinterpret_cast<const char*>(&data[position]), label_len);
            position += label_len;

            uint32_t base_pet = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);

            uint32_t combined_pet_1 = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);

            uint32_t combined_pet_2 = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);

            tile.tile_type = PetBattleCage{ label, base_pet, combined_pet_1, combined_pet_2 };
            break;
        }

        case 37: { // TileType::PetTrainer
            uint16_t name_len = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);

            std::string name(reinterpret_cast<const char*>(&data[position]), name_len);
            position += name_len;

            uint32_t pet_total_count = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);

            uint32_t unknown_1 = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);

            std::vector<uint32_t> pets_id;
            for (uint32_t i = 0; i < pet_total_count; i++) {
                uint32_t pet_id = *reinterpret_cast<const uint32_t*>(&data[position]);
                position += sizeof(uint32_t);
                pets_id.push_back(pet_id);
            }

            tile.tile_type = PetTrainer{ name, pet_total_count, unknown_1, pets_id };
            break;
        }

        case 38: { // TileType::SteamEngine
            uint32_t temperature = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);

            tile.tile_type = SteamEngine{ temperature };
            break;
        }

        case 39: { // TileType::LockBot
            uint32_t time_passed = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);

            tile.tile_type = LockBot{ time_passed };
            break;
        }
        case 40: { // TileType::WeatherMachine
            uint32_t settings = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            tile.tile_type = WeatherMachine{ settings };
            break;
        }

        case 41: { // TileType::SpiritStorageUnit
            uint32_t ghost_jar_count = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            tile.tile_type = SpiritStorageUnit{ ghost_jar_count };
            break;
        }

        case 42: { // TileType::DataBedrock
            position += 21;
            tile.tile_type = DataBedrock{};
            break;
        }

        case 43: { // TileType::Shelf
            uint32_t top_left_item_id = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            uint32_t top_right_item_id = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            uint32_t bottom_left_item_id = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            uint32_t bottom_right_item_id = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            tile.tile_type = Shelf{ top_left_item_id, top_right_item_id, bottom_left_item_id, bottom_right_item_id };
            break;
        }

        case 44: { // TileType::VipEntrance
            uint8_t unknown_1 = data[position];
            position += sizeof(uint8_t);
            uint32_t owner_uid = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            uint32_t access_count = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);

            std::vector<uint32_t> access_uids;
            for (uint32_t i = 0; i < access_count; i++) {
                uint32_t uid = *reinterpret_cast<const uint32_t*>(&data[position]);
                position += sizeof(uint32_t);
                access_uids.push_back(uid);
            }

            tile.tile_type = VipEntrance{ unknown_1, owner_uid, access_uids };
            break;
        }

        case 45: { // TileType::ChallangeTimer
            tile.tile_type = ChallangeTimer{};
            break;
        }

        case 47: { // TileType::FishWallMount
            uint16_t label_len = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);
            std::string label(reinterpret_cast<const char*>(&data[position]), label_len);
            position += label_len;
            uint32_t item_id = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            uint8_t lb = data[position];
            position += sizeof(uint8_t);
            tile.tile_type = FishWallMount{ label, item_id, lb };
            break;
        }

        case 48: { // TileType::Portrait pp
            uint16_t label_len = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);
            std::string label(reinterpret_cast<const char*>(&data[position]), label_len);
            position += label_len; //Bener
            uint32_t unknown_1 = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            uint32_t unknown_2 = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            uint32_t unknown_3 = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            uint32_t unknown_4 = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            uint16_t face = *reinterpret_cast<const uint16_t*>(&data[position]); 
            position += sizeof(uint16_t);
            uint16_t hat = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);
            uint16_t mask = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);
            uint16_t unknown_5 = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);
            uint16_t unknown_6 = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);

            tile.tile_type = Portrait{ label, unknown_1, unknown_2, unknown_3, unknown_4, face, hat, mask, unknown_5, unknown_6 };
            break;
        }

        case 49: { // TileType::GuildWeatherMachine
            uint32_t unknown_1 = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            uint32_t gravity = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            uint8_t flags = data[position];
            position += sizeof(uint8_t);
            tile.tile_type = GuildWeatherMachine{ unknown_1, gravity, flags };
            break;
        }

        case 50: { // TileType::FossilPrepStation
            uint32_t unknown_1 = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            tile.tile_type = FossilPrepStation{ unknown_1 };
            break;
        }

        case 51: { // TileType::DnaExtractor
            tile.tile_type = DnaExtractor{};
            break;
        }
        case 52: { // TileType::Howler
            tile.tile_type = Howler{};
            break;
        }
        case 53: {
            // TileType::ChemsynthTank
            uint32_t current_chem = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            uint32_t target_chem = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            tile.tile_type = ChemsynthTank{ current_chem, target_chem };
            break;
        }
        case 54: {
            // TileType::StorageBlock
            uint16_t data_len = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);
            std::vector<StorageBlockItemInfo> items;
            for (int i = 0; i < data_len / 13; i++) {
                position += 3;
                uint32_t id = *reinterpret_cast<const uint32_t*>(&data[position]);
                position += sizeof(uint32_t);
                position += 2;
                uint32_t amount = *reinterpret_cast<const uint32_t*>(&data[position]);
                position += sizeof(uint32_t);
                items.push_back({ id, amount });
            }
            tile.tile_type = StorageBlock{ items };
            break;
        }
        case 55: {
            // TileType::CookingOven
            uint32_t temperature_level = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            uint32_t ingredient_count = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            std::vector<CookingOvenIngredientInfo> ingredients;
            for (uint32_t i = 0; i < ingredient_count; i++) {
                uint32_t item_id = *reinterpret_cast<const uint32_t*>(&data[position]);
                position += sizeof(uint32_t);
                uint32_t time_added = *reinterpret_cast<const uint32_t*>(&data[position]);
                position += sizeof(uint32_t);
                ingredients.push_back({ item_id, time_added });
            }
            uint32_t unknown_1 = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            uint32_t unknown_2 = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            uint32_t unknown_3 = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            tile.tile_type = CookingOven{ temperature_level, ingredients, unknown_1, unknown_2, unknown_3 };
            break;
        }
        case 56: {
            // TileType::AudioRack
            uint16_t note_len = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);
            std::string note(reinterpret_cast<const char*>(&data[position]), note_len);
            position += note_len;
            uint32_t volume = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            tile.tile_type = AudioRack{ note, volume };
            break;
        }
        case 57: {
            // TileType::GeigerCharger
            uint32_t unknown_1 = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            tile.tile_type = GeigerCharger{ unknown_1 };
            break;
        }
        case 58: {
            // TileType::AdventureBegins
            tile.tile_type = AdventureBegins{};
            break;
        }
        case 59: {
            // TileType::TombRobber
            tile.tile_type = TombRobber{};
            break;
        }
        case 60: {
            // TileType::BalloonOMatic
            uint32_t total_rarity = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            uint8_t team_type = data[position];
            position += sizeof(uint8_t);
            tile.tile_type = BalloonOMatic{ total_rarity, team_type };
            break;
        }
        case 61: {
            // TileType::TrainingPort
            uint32_t fish_lb = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            uint16_t fish_status = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);
            uint32_t fish_id = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            uint32_t fish_total_exp = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            uint32_t fish_level = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            uint32_t unknown_2 = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            tile.tile_type = TrainingPort{ fish_lb, fish_status, fish_id, fish_total_exp, fish_level, unknown_2 };
            break;
        }
        case 62: {
            // TileType::ItemSucker
            uint32_t item_id_to_suck = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            uint32_t item_amount = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            uint16_t flags = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);
            uint32_t limit = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            tile.tile_type = ItemSucker{ item_id_to_suck, item_amount, flags, limit };
            break;
        }
        case 63: {
            // TileType::CyBot
            uint32_t sync_timer = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            uint32_t activated = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            uint32_t command_data_count = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            std::vector<CyBotCommandData> command_datas;
            for (uint32_t i = 0; i < command_data_count; i++) {
                uint32_t command_id = *reinterpret_cast<const uint32_t*>(&data[position]);
                position += sizeof(uint32_t);
                uint32_t is_command_used = *reinterpret_cast<const uint32_t*>(&data[position]);
                position += sizeof(uint32_t);
                position += 7; // Skip 7 bytes
                command_datas.push_back({ command_id, is_command_used });
            }
            tile.tile_type = CyBot{ sync_timer, activated, command_datas };
            break;
        }
        case 65: {
            // TileType::GuildItem
            position += 17; // Skip 17 bytes
            tile.tile_type = GuildItem{};
            break;
        }

        case 66: { // TileType::Growscan
            uint8_t unknown_1 = data[position];
            position += sizeof(uint8_t);
            tile.tile_type = Growscan{ unknown_1 };
            break;
        }
        case 67: {
            // TileType::ContainmentFieldPowerNode
            uint32_t ghost_jar_count = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            uint32_t unknown_1_size = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            std::vector<uint32_t> unknown_1;
            for (uint32_t i = 0; i < unknown_1_size; i++) {
                uint32_t value = *reinterpret_cast<const uint32_t*>(&data[position]);
                position += sizeof(uint32_t);
                unknown_1.push_back(value);
            }
            tile.tile_type = ContainmentFieldPowerNode{ ghost_jar_count, unknown_1 };
            break;
        }
        case 68: {
            // TileType::SpiritBoard
            uint32_t unknown_1 = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            uint32_t unknown_2 = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            uint32_t unknown_3 = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            tile.tile_type = SpiritBoard{ unknown_1, unknown_2, unknown_3 };
            break;
        }
        case 72: {
            // TileType::StormyCloud
            uint32_t sting_duration = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            uint32_t is_solid = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            uint32_t non_solid_duration = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            tile.tile_type = StormyCloud{ sting_duration, is_solid, non_solid_duration };
            break;
        }
        case 73: {
            // TileType::TemporaryPlatform
            uint32_t unknown_1 = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            tile.tile_type = TemporaryPlatform{ unknown_1 };
            break;
        }
        case 74: {
            // TileType::SafeVault
            tile.tile_type = SafeVault{};
            break;
        }
        case 75: {
            // TileType::AngelicCountingCloud
            uint32_t is_raffling = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            uint16_t unknown_1 = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);
            uint8_t ascii_code = data[position];
            position += sizeof(uint8_t);
            tile.tile_type = AngelicCountingCloud{ is_raffling, unknown_1, ascii_code };
            break;
        }
        case 77: {
            // TileType::InfinityWeatherMachine
            uint32_t interval_minutes = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            uint32_t weather_machine_list_size = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            std::vector<uint32_t> weather_machine_list;
            for (uint32_t i = 0; i < weather_machine_list_size; i++) {
                uint32_t weather_machine = *reinterpret_cast<const uint32_t*>(&data[position]);
                position += sizeof(uint32_t);
                weather_machine_list.push_back(weather_machine);
            }
            tile.tile_type = InfinityWeatherMachine{ interval_minutes, weather_machine_list };
            break;
        }
        case 79: {
            // TileType::PineappleGuzzler
            tile.tile_type = PineappleGuzzler{};
            break;
        }
        case 80: {
            // TileType::KrakenGalaticBlock
            uint8_t pattern_index = data[position];
            position += sizeof(uint8_t);
            uint32_t unknown_1 = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            uint8_t r = data[position];
            position += sizeof(uint8_t);
            uint8_t g = data[position];
            position += sizeof(uint8_t);
            uint8_t b = data[position];
            position += sizeof(uint8_t);
            tile.tile_type = KrakenGalaticBlock{ pattern_index, unknown_1, r, g, b };
            break;
        }
        case 81: {
            // TileType::FriendsEntrance
            uint32_t owner_user_id = *reinterpret_cast<const uint32_t*>(&data[position]);
            position += sizeof(uint32_t);
            uint16_t unknown_1 = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);
            uint16_t unknown_2 = *reinterpret_cast<const uint16_t*>(&data[position]);
            position += sizeof(uint16_t);
            tile.tile_type = FriendsEntrance{ owner_user_id, unknown_1, unknown_2 };
            break;
        }
        default: {
            tile.tile_type = Basic{};
            break;
        }
        }
}

