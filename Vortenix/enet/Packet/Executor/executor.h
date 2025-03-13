#pragma once

#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <future>
#include <atomic>

void sleep_milliseconds(int milliseconds) {
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void UDP::say(std::string message, Bot& bot) {
	UDP::sendPacket("action|input\ntext|" + message, NET_MESSAGE_GENERIC_TEXT, bot);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void UDP::warp(std::string name, Bot& bot) {
	UDP::sendPacket("action|join_request\nname|" + name + "\n|invitedWorld|0", NET_MESSAGE_GAME_MESSAGE, bot);
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
}

void UDP::wear(int id, Bot& bot) {
	TankPacketStruct tank;
	tank.type = NET_GAME_PACKET_ITEM_ACTIVATE_REQUEST;
	tank.value = id;
    UDP::sendPacketRaw((uint8_t*)&tank, bot);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}


void UDP::fav(int id, Bot& bot) {

}

#include <mutex>

std::mutex mtx;  // Mutex untuk mengunci akses ke data bersama

void UDP::trash(int id, int amount, Bot& bot) {
    UDP::sendPacket("action|trash\n|itemID|" + std::to_string(id), NET_MESSAGE_GENERIC_TEXT, bot);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    int trashId = id;
    int trashCount = amount;
    bool packet_received = false;
    auto start = std::chrono::steady_clock::now();
    size_t last_checked_index = bot.local.onVariantListDebug.size();

    while (std::chrono::steady_clock::now() - start < std::chrono::seconds(5)) {
        std::lock_guard<std::mutex> lock(mtx);

        if (bot.local.onVariantListDebug.size() > last_checked_index) {
            const auto& variant = bot.local.onVariantListDebug.back();
            variantlist_t variantCopy = variant;

            if (variantCopy.get(0).get_string() == "OnDialogRequest") {
                if (variantCopy.get(1).get_string().find("add_label_with_icon|big|`4Trash") != std::string::npos &&
                    variantCopy.get(1).get_string().find(std::to_string(trashId)) != std::string::npos) {
                    packet_received = true;
                    break;
                }
            }
            last_checked_index = bot.local.onVariantListDebug.size();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    if (packet_received) {
        UDP::sendPacket("action|dialog_return\ndialog_name|trash_item\nitemID|" + std::to_string(trashId) + "|\ncount|" + std::to_string(trashCount), NET_MESSAGE_GENERIC_TEXT, bot);
    }
}


void UDP::drop(int id, int amount, Bot& bot) {
    UDP::sendPacket("action|drop\n|itemID|" + std::to_string(id), NET_MESSAGE_GENERIC_TEXT, bot);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    int dropId = id;
    int dropCount = amount;
    bool packet_received = false;

    auto start = std::chrono::steady_clock::now();
    size_t last_checked_index = bot.local.onVariantListDebug.size();

    while (std::chrono::steady_clock::now() - start < std::chrono::seconds(5)) { // Diperpanjang jadi 5 detik
        std::lock_guard<std::mutex> lock(mtx);

        if (bot.local.onVariantListDebug.size() > last_checked_index) {
            const auto& variant = bot.local.onVariantListDebug.back();
            variantlist_t variantCopy = variant;

            if (variantCopy.get(0).get_string() == "OnDialogRequest") {
                if (variantCopy.get(1).get_string().find("add_label_with_icon|big|`wDrop") != std::string::npos &&
                    variantCopy.get(1).get_string().find(std::to_string(dropId)) != std::string::npos) {
                    packet_received = true;
                    break;
                }
            }

            last_checked_index = bot.local.onVariantListDebug.size();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (packet_received) {
        UDP::sendPacket("action|dialog_return\ndialog_name|drop_item\nitemID|" + std::to_string(dropId) + "|\ncount|" + std::to_string(dropCount), NET_MESSAGE_GENERIC_TEXT, bot);
    }
}

void UDP::collectRaw(int x, int y, int oid, Bot& bot) {
    TankPacketStruct tank;
    tank.type = NET_GAME_PACKET_ITEM_ACTIVATE_OBJECT_REQUEST;
    tank.value = oid;
    tank.x = x;
    tank.y = y;
    tank.tilex = x + y + 4;
    tank.tiley = 0;
    UDP::sendPacketRaw((uint8_t*)&tank, bot);
};

void UDP::collect(int range, Bot& bot) {
    const int playerX = bot.local.pos.m_x / 32;
    const int playerY = bot.local.pos.m_y / 32;
    for (const auto& item : bot.local.world.dropped.items) {
        uint8_t objX = item.x / 32;
        uint8_t objY = item.y / 32;
        if (std::abs(objX - playerX) <= range && std::abs(objY - playerY) <= range) {
            UDP::collectRaw(item.x, item.y, item.uid, bot);
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    }
};

void UDP::collectSet(int range, int interval, vector<int>& id, Bot& bot) {
    const int playerX = bot.local.pos.m_x / 32;
    const int playerY = bot.local.pos.m_y / 32;
    for (const auto& item : bot.local.world.dropped.items) {
        uint8_t objX = item.x / 32;
        uint8_t objY = item.y / 32;
        if (std::abs(objX - playerX) <= range && std::abs(objY - playerY) <= range) {
            if (std::find(id.begin(), id.end(), item.id) != id.end()) {
                UDP::collectRaw(item.x, item.y, item.uid, bot);
                std::this_thread::sleep_for(std::chrono::milliseconds(interval));
            }
        }
    }
};

void UDP::walk(int x, int y, bool ap, Bot& bot) {
    if (ap) {
        x = x * 32;
        y = y * 32;
    } 
    TankPacketStruct tank;
    tank.type = NET_GAME_PACKET_STATE;
    tank.x = static_cast<float>(x);
    tank.y = static_cast<float>(y);
    tank.tilex = -1;
    tank.tiley = -1;
    tank.state = 32;
    if (static_cast<int>(x/32) < static_cast<int>(bot.local.pos.m_x / 32)) {
        tank.state = 48;
    }
    UDP::sendPacketRaw((uint8_t*)&tank, bot);
    std::this_thread::yield();
    if (bot.local.status == "online") {
        bot.local.pos.m_x = x;
        bot.local.pos.m_y = y;
    }
};

struct Node {
    uint32_t x, y;
    int g, h, f;
    bool operator<(const Node& other) const {
        return f > other.f;
    }
};

bool UDP::isWalkable(int x, int y, Bot& bot) {
    if (x < 0 || y < 0 || x >= bot.local.world.width || y >= bot.local.world.height) {
        return false;  // Past edge of map
    }

    int index = x + y * bot.local.world.width;
    if (index < 0 || index > bot.local.world.tile_count) {
        return false;
    }

    Tile& tile = bot.local.world.tiles[index];

    return (itemDefs->isNotCollision(tile.foreground_item_id));
}


std::vector<std::pair<int, int>> UDP::get_neighbors(int x, int y, Bot& bot) {
    std::vector<std::pair<int, int>> neighbors;
    std::vector<std::pair<int, int>> directions = {
        {-1, 0}, {1, 0}, {0, -1}, {0, 1}
    };
    for (auto& dir : directions) {
        int nx = x + dir.first;
        int ny = y + dir.second;
        if (nx >= 0 && ny >= 0 && nx < bot.local.world.width && ny < bot.local.world.height &&
            UDP::isWalkable(nx, ny, bot)) {
            neighbors.push_back({ nx, ny });
        }
    }
    std::vector<std::pair<int, int>> diagonal_directions = {
        {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
    };
    for (auto& dir : diagonal_directions) {
        int nx = x + dir.first;
        int ny = y + dir.second;
        int block_x = x + dir.first;  // Titik horizontal
        int block_y = y + dir.second; // Titik vertikal

        // Cek apakah gerakan diagonal terhalang oleh salah satu titik di sisi horizontal/vertikal
        if (nx >= 0 && ny >= 0 && nx < bot.local.world.width && ny < bot.local.world.height &&
            UDP::isWalkable(nx, ny, bot) &&
            UDP::isWalkable(x, block_y, bot) &&   // Cek sisi vertikal
            UDP::isWalkable(block_x, y, bot)) {   // Cek sisi horizontal
            neighbors.push_back({ nx, ny });
        }
    }

    return neighbors;
}


int UDP::calculate_h(int from_x, int from_y, int to_x, int to_y) {
    int dx = std::abs(to_x - from_x);
    int dy = std::abs(to_y - from_y);
    return 10 * (dx + dy);  // Manhattan distance (no diagonal penalty)
};


struct pair_hash {
    template <typename T1, typename T2>
    std::size_t operator()(const std::pair<T1, T2>& p) const {
        return std::hash<T1>{}(p.first) ^ (std::hash<T2>{}(p.second) << 1);
    }
};

#include <queue>
#include <set>

std::vector<std::pair<int, int>> UDP::find_path(int x, int y, Bot& bot) {
    std::priority_queue<Node> open_list;
    std::unordered_map<std::pair<int, int>, std::pair<int, int>, pair_hash> came_from;
    std::unordered_map<std::pair<int, int>, int, pair_hash> g_scores;
    std::set<std::pair<int, int>> closed_set;  // Menyimpan node yang sudah diproses

    int start_x = bot.local.pos.m_x / 32;
    int start_y = bot.local.pos.m_y / 32;
    Node start_node = { start_x, start_y, 0, calculate_h(start_x, start_y, x, y), 0 };
    
    open_list.push(start_node);
    g_scores[{start_x, start_y}] = 0;

    //std::cout << "Mulai mencari path dari (" << start_x << "," << start_y << ") ke (" << x << "," << y << ")\n";

    while (!open_list.empty()) {
        Node current = open_list.top();
        open_list.pop();

        // Jika sudah pernah diproses, lanjutkan
        if (closed_set.find({current.x, current.y}) != closed_set.end()) {
            continue;
        }

        closed_set.insert({current.x, current.y}); // Tandai sudah diproses

        //std::cout << "Memproses node (" << current.x << "," << current.y << ") dengan F=" << current.f << "\n";

        // Jika mencapai tujuan, rekonstruksi path
        if (current.x == x && current.y == y) {
            std::vector<std::pair<int, int>> path;
            std::pair<int, int> current_pos = { current.x, current.y };

            while (came_from.find(current_pos) != came_from.end()) {
                path.push_back(current_pos);
                current_pos = came_from[current_pos];
            }

            path.push_back({ start_x, start_y });
            std::reverse(path.begin(), path.end());

            //std::cout << "Jalur ditemukan!\n";
            return path;
        }

        for (auto& neighbor : get_neighbors(current.x, current.y, bot)) {
            int tentative_g_score = g_scores[{current.x, current.y}] + 10;

            if (g_scores.find(neighbor) == g_scores.end() || tentative_g_score < g_scores[neighbor]) {
                g_scores[neighbor] = tentative_g_score;
                int h = calculate_h(neighbor.first, neighbor.second, x, y);
                int f = tentative_g_score + h;

                Node neighbor_node = { neighbor.first, neighbor.second, tentative_g_score, h, f };
                open_list.push(neighbor_node);
                came_from[neighbor] = { current.x, current.y };

                //std::cout << "Menambahkan node (" << neighbor.first << "," << neighbor.second << ") dengan F=" << f << "\n";
            }
        }
    }
    std::cout << "Tidak ada jalur ditemukan!\n";
    return {};  // Jika tidak ada jalur, return path kosong
}


bool UDP::findPath(int x, int y, Bot& bot) {
    std::vector<std::pair<int, int>> path = UDP::find_path(x, y, bot);
    if (!path.empty()) {
        for (const auto& step : path) {
            UDP::walk(step.first, step.second, true, bot);
            std::this_thread::yield();
            std::this_thread::sleep_for(std::chrono::milliseconds(25));
        }
        std::cout << "Current Position: ("
            << static_cast<int>(bot.local.pos.m_x / 32) << ", "
            << static_cast<int>(bot.local.pos.m_y / 32) << ")\n";
        std::cout << "Target Position: (" << x << ", " << y << ")\n";
        return true;
    }
    return false;
}


void UDP::punch(int x, int y, Bot& bot) {
    UDP::place(x, y, 18, bot);
};

void UDP::wrench(int x, int y, Bot& bot) {
    UDP::place(x, y, 32, bot);
};

void UDP::place(int x, int y, int id, Bot& bot) {
    TankPacketStruct tank;
    tank.type = NET_GAME_PACKET_TILE_CHANGE_REQUEST;
    tank.x = bot.local.pos.m_x;
    tank.y = bot.local.pos.m_y;
    tank.tilex = static_cast<int>(bot.local.pos.m_x / 32) + x;
    tank.tiley = static_cast<int>(bot.local.pos.m_y / 32) + y;
    tank.value = id;

    int base_x = static_cast<int>(bot.local.pos.m_x / 32);
    int base_y = static_cast<int>(bot.local.pos.m_y / 32);

    if (tank.tilex <= base_x + 4 && tank.tilex >= base_x - 4 &&
        tank.tiley <= base_y + 4 && tank.tiley >= base_y - 4) {

        UDP::sendPacketRaw((uint8_t*)&tank, bot);

        tank.state = (x > 0) ? 2592 : 2608;
        tank.type = NET_GAME_PACKET_STATE;

        UDP::sendPacketRaw((uint8_t*)&tank, bot);
    }
}  

void UDP::wrenchPeople(int netid, Bot& bot) {
    UDP::sendPacket("action|wrench\n|netid|" + std::to_string(netid), NET_MESSAGE_GENERIC_TEXT, bot);
};

void UDP::updateCoordinat(Bot& bot) {
    UDP::wrenchPeople(bot.local.netid, bot);
}
//Make Your Own Scrip Here
#include <unordered_set>

void UDP::mainScript(Bot& bot) {
    int trashItem[9] = { 14, 2, 3, 4, 5, 10, 11 , 14, 15};
    std::unordered_set<int> trashSet(trashItem, trashItem + 9); // Konversi array menjadi unordered_set untuk akses cepat

    std::cout << "Script Berjalan" << std::endl;
    for (int i = 0; i < bot.local.world.tile_count; i++) {
        auto& tile = bot.local.world.tiles[i];
        tile = bot.local.world.tiles[i];
        if (tile.foreground_item_id == 8) {
            UDP::findPath(tile.x, tile.y - 1, bot);
            while (bot.local.world.tiles[tile.x + ((tile.y + 1) * bot.local.world.width)].background_item_id != 0) {
                tile = bot.local.world.tiles[i];
                bot.local.world.tiles[i];
                UDP::punch(0, 2, bot);
                std::this_thread::yield();
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                UDP::collect(3, bot);
                for (const auto& item : bot.local.items) {
                    if (trashSet.count(item.id) && item.count >= 150) {
                        UDP::sendPacket("action|trash\n|itemID|" + std::to_string(item.id), NET_MESSAGE_GENERIC_TEXT, bot);
                        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Kurangi delay
                        UDP::sendPacket("action|dialog_return\ndialog_name|trash_item\nitemID|" + std::to_string(item.id) +
                            "|\ncount|" + std::to_string(item.count), NET_MESSAGE_GENERIC_TEXT, bot);
                        std::this_thread::sleep_for(std::chrono::milliseconds(2000)); // Kurangi delay
                    }
                }
            }
        }
        if (((tile.foreground_item_id != 0 or tile.background_item_id == 14) and tile.foreground_item_id != 8 and tile.foreground_item_id != 6) && UDP::isWalkable(tile.x, tile.y - 1, bot)) {
            tile = bot.local.world.tiles[i];
            std::cout << "Melakukan findPath ke " << tile.x << ", " << tile.y << std::endl;
            UDP::findPath(tile.x, tile.y - 1, bot);
            while ((tile.foreground_item_id != 0 or tile.background_item_id == 14) and tile.foreground_item_id != 8 and tile.foreground_item_id != 6) {
                std::this_thread::yield();
                tile = bot.local.world.tiles[i];
                std::cout << "Menyerang tile (" << tile.x << ", " << tile.y << ") karena ada item ID " << tile.foreground_item_id << std::endl;
                UDP::punch(0, 1, bot);
                std::this_thread::yield();
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                UDP::collect(3, bot);
                for (const auto& item : bot.local.items) {
                    if (trashSet.count(item.id) && item.count >= 150) {
                        UDP::sendPacket("action|trash\n|itemID|" + std::to_string(item.id), NET_MESSAGE_GENERIC_TEXT, bot);
                        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Kurangi delay

                        UDP::sendPacket("action|dialog_return\ndialog_name|trash_item\nitemID|" + std::to_string(item.id) +
                            "|\ncount|" + std::to_string(item.count), NET_MESSAGE_GENERIC_TEXT, bot);
                        std::this_thread::sleep_for(std::chrono::milliseconds(2000)); // Kurangi delay
                    }
                }
            }
            std::cout << "Item di tile (" << tile.x << ", " << tile.y << ") sudah tidak ada lagi" << std::endl;
        }
    }
}



#endif