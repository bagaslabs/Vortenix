#pragma once
#ifndef ITEMSMANAGER_H
#define ITEMSMANAGER_H

struct ItemInfo
{
	int id;
	uint8_t type; // used for serialize check
	std::string name = "";
	int8_t collisionType;
	uint8_t breakHits; // 6 * actual break hit
	int8_t clothingType;
	short rarity;
	int growTime; // as seconds
	bool isCollision;
};

class ItemInfoManager
{
public:
	int size = 0;
	short version = 0;
	int itemcount = 0;
	bool Instance = true;
	std::vector<ItemInfo> items;
	bool ParseItemJson(const std::string& filePath);

	ItemInfo* Get(int id);
	std::string GetNameById(int id);
	bool isNotCollision(int id);
	int getGrowtimeByID(int id);
	bool IncludeExtra(int id);
	bool IsBackground(int id);
	void GetItemsByPartialName(std::vector<ItemInfo*>& list, std::string partial);
};

extern std::shared_ptr<ItemInfoManager> itemDefs;
std::shared_ptr<ItemInfoManager> itemDefs = nullptr;

bool ItemInfoManager::ParseItemJson(const std::string& filePath) {
	try {
		std::ifstream file(filePath);
		if (!file.is_open()) {
			std::cerr << "Error: Unable to open file " << filePath << std::endl;
			return false;
		}

		nlohmann::json jsonData;
		file >> jsonData;

		if (jsonData.find("version") == jsonData.end() || !jsonData["version"].is_number_unsigned()) {
			std::cerr << "Error: Missing or invalid 'version' field in JSON." << std::endl;
			return false;
		}
		if (jsonData.find("itemCount") == jsonData.end() || !jsonData["itemCount"].is_number_integer()) {
			std::cerr << "Error: Missing or invalid 'itemCount' field in JSON." << std::endl;
			return false;
		}
		if (jsonData.find("items") == jsonData.end() || !jsonData["items"].is_array()) {
			std::cerr << "Error: Missing or invalid 'items' field in JSON." << std::endl;
			return false;
		}

		version = jsonData["version"].get<short>();
		itemcount = jsonData["itemCount"].get<int>();

		items.clear();
		for (const auto& item : jsonData["items"]) {
			ItemInfo info;
			if (item.count("itemId") > 0 && item["itemId"].is_number_integer())
				info.id = item["itemId"].get<int>();
			if (item.count("itemType") > 0 && item["itemType"].is_number_unsigned())
				info.type = item["itemType"].get<uint8_t>();
			if (item.count("name") > 0 && item["name"].is_string())
				info.name = item["name"].get<std::string>();
			if (item.count("collisionType") > 0 && item["collisionType"].is_number_integer())
				info.collisionType = item["collisionType"].get<int8_t>();
			info.isCollision = (item["collisionType"].get<int8_t>() == 1);
			if (item.count("breakHits") > 0 && item["breakHits"].is_number_unsigned())
				info.breakHits = item["breakHits"].get<uint8_t>();
			if (item.count("clothingType") > 0 && item["clothingType"].is_number_integer())
				info.clothingType = item["clothingType"].get<int8_t>();
			if (item.count("rarity") > 0 && item["rarity"].is_number_integer())
				info.rarity = item["rarity"].get<short>();
			if (item.count("growTime") > 0 && item["growTime"].is_number_integer())
				info.growTime = item["growTime"].get<int>();
			items.push_back(info);
			/*std::cout << "Item ID: " << info.id
          << " Name: " << info.name 
          << " [Rarity: " << info.rarity << "]" 
          << " [Collision: " << std::to_string(info.isCollision) << "]"
          << " [Grow Time: " << info.growTime << "s]" 
          << std::endl;*/
		}
		Instance = true;
		return true;
	}
	catch (const std::exception& e) {
		std::cerr << "Exception occurred while parsing JSON: " << e.what() << std::endl;
		Instance = false;
		return false;
	}
}


ItemInfo* ItemInfoManager::Get(int id)
{
	if (id > -1 && id < itemcount)
		return &items[id];
	return NULL;
}

std::string ItemInfoManager::GetNameById(int id)
{
	ItemInfo* info = Get(id);
	if (info)
	{
		return info->name;
	}
	return "Item not found";
}

bool ItemInfoManager::IncludeExtra(int id)
{
	ItemInfo* info = Get(id);
	if (!info)
		return false;
	return info->id == 3760 || // Data Bedrock
		info->type == 2 ||	 // Door
		info->type == 3 ||	 // Lock
		info->type == 10 || // Sign
		info->type == 13 || // Main Door
		info->type == 19 || // Seed
		info->type == 26 || // Portal
		info->type == 33 || // Mailbox
		info->type == 34 || // Bulletin Board
		info->type == 36 || // Dice Block
		info->type == 38 || // Science Station
		info->type == 40 || // Achievement Block
		info->type == 43 || // Sungate
		info->type == 46 ||
		info->type == 47 ||
		info->type == 49 ||
		info->type == 50 ||
		info->type == 51 || // Bunny Egg
		info->type == 52 ||
		info->type == 53 ||
		info->type == 54 || // Xenonite
		info->type == 55 || // Phone Booth
		info->type == 56 || // Crystal
		info->type == 57 || // Crime In Progress
		info->type == 59 || // Spotlight
		info->type == 61 ||
		info->type == 62 || // Vending Machine
		info->type == 63 || // Fish Tank Port
		info->type == 66 || // Forge
		info->type == 67 || // Giving Tree
		info->type == 73 || // Sewing Machine
		info->type == 74 ||
		info->type == 76 || // Painting Easel
		info->type == 78 || // Pet Trainer (WHY?!)
		info->type == 80 || // Lock-Bot (Why?!)
		info->type == 81 || // Weather Machine - Background
		info->type == 83 || // Display Shelf
		info->type == 84 ||
		info->type == 85 ||	// Challenge Timer
		info->type == 86 ||	// Challenge Start/End Flags
		info->type == 87 ||	// Fish Wall Mount
		info->type == 88 ||	// Portrait
		info->type == 89 ||	// Weather Machine - Stuff
		info->type == 91 ||	// Fossil Prep Station
		info->type == 92 ||	// DNA Processorr
		info->type == 93 ||	// Howler
		info->type == 97 ||	// Storage Box Xtreme / Untrade-a-box
		info->type == 98 ||	// Oven
		info->type == 100 || // Geiger Charger
		info->type == 101 ||
		info->type == 111 || // Magplant
		info->type == 113 || // CyBot
		info->type == 115 || // Lucky Token
		info->type == 116 || // GrowScan 9000 ???
		info->type == 126 || // Storm Cloud
		info->type == 127 || // Temp. Platform
		info->type == 130;
}

bool ItemInfoManager::IsBackground(int id)
{
	ItemInfo* info = Get(id);
	if (!info)
		return false;
	return info->type == 22 || info->type == 28 || info->type == 18;
}

void ItemInfoManager::GetItemsByPartialName(std::vector<ItemInfo*>& list, std::string partial)
{
	list.clear();
	if (partial.empty())
		return;
	std::transform(partial.begin(), partial.end(), partial.begin(), ::tolower);
	for (auto& item : items)
	{
		std::string src = item.name;
		std::transform(src.begin(), src.end(), src.begin(), ::tolower);
		if (src.find(partial) != std::string::npos)
			list.push_back(&item);
	}
}

int ItemInfoManager::getGrowtimeByID(int id)
{
	ItemInfo* info = Get(id);
	if (info)
	{
		return info->growTime;
	}
	return 0;
}

bool ItemInfoManager::isNotCollision(int id)
{
	ItemInfo* info = Get(id);
	if (!info)
	{
		return false;
	}
	return info->collisionType == 0;
}


#endif