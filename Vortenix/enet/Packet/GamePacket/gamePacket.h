#pragma once 
#ifndef GAMEPACKET_H
#define GAMEPACKET_H

void UDP::handleGamePacket(uint8_t* ptr, Bot& bot) {
    TankPacketStruct* packet = reinterpret_cast<TankPacketStruct*>(ptr);
    std::string logMessage = "[SYSTEM]: [RECEIVED PACKET RAW]:\n\n";
    logMessage += "  TYPE: " + convertTankPacketType(static_cast<eTankPacketType>(packet->type)) + "\n";
    logMessage += "  DROPPED: " + std::to_string(static_cast<int>(packet->dropped)) + "\n";
    logMessage += "  NETID: " + std::to_string(packet->netid) + "\n";
    logMessage += "  STATE: " + std::to_string(packet->state) + "\n";
    logMessage += "  VALUE: " + std::to_string(packet->value) + "\n";
    logMessage += "  X: " + std::to_string(packet->x) + "\n";
    logMessage += "  Y: " + std::to_string(packet->y) + "\n";
    logMessage += "  XSPEED: " + std::to_string(packet->xspeed) + "\n";
    logMessage += "  YSPEED: " + std::to_string(packet->yspeed) + "\n";
    logMessage += "  TILEX: " + std::to_string(packet->tilex) + "\n";
    logMessage += "  TILEY: " + std::to_string(packet->tiley) + "\n";
    logMessage += "  EXT_DATA_SIZE: " + std::to_string(packet->extDataSize) + "\n";
    logger(bot, "[SYSTEM]: RECEIVED GAME PACKET:\n" + logMessage, 2);
    switch (packet->type) {
    case NET_GAME_PACKET_STATE: {
        UDP::packetState(packet, bot);
        break;
    }
    case NET_GAME_PACKET_CALL_FUNCTION: {
        logger(bot, "[SYSTEM]: RECEIVED GAME PACKET: " + convertTankPacketType(NET_GAME_PACKET_CALL_FUNCTION));
        UDP::handleGamePacket_CallFunction(ptr, bot);
        break;
    }
    case NET_GAME_PACKET_PING_REQUEST: {
        TankPacketStruct tank;
        tank.type = NET_GAME_PACKET_PING_REPLY;
        tank.x = 64;
        tank.y = 64;
        tank.value = packet->value + 5000;
        UDP::sendPacketRaw((uint8_t*)&tank, bot);
        bot.local.status = "online";
        break;
    }
    case NET_GAME_PACKET_SEND_INVENTORY_STATE: {
        UDP::inventoryState(ptr, bot); 
        break;
    }
    case NET_GAME_PACKET_MODIFY_ITEM_INVENTORY: {
        UDP::ModifyInventory(packet, bot);
        break;
    }
    case NET_GAME_PACKET_SEND_MAP_DATA: {
        bot.local.world.parse(ptr + 56, packet->extDataSize - 56);
        break;
    }
    case NET_GAME_PACKET_ITEM_CHANGE_OBJECT: {
        UDP::ChangeItemObjects(ptr, bot);
        break;
    }
    case NET_GAME_PACKET_TILE_CHANGE_REQUEST: {
        UDP::OnTileChangeRequest(packet, ptr, bot);
        break;
    }
    case NET_GAME_PACKET_SEND_TILE_TREE_STATE: {
        UDP::OnTileTreeState(packet, bot);
        break;
    }
    case NET_GAME_PACKET_SEND_TILE_UPDATE_DATA: {
        UDP::OnTileUpdateData(packet, ptr + 56, bot);
    }
    default:
        std::cout << "Unknown packet type: " << convertTankPacketType(packet->type) << std::endl;
        break;
    }
}

void UDP::packetState(TankPacketStruct* packet, Bot& bot) {
    for (int i = 0; i < bot.local.players.size(); i++) {
        if (bot.local.players[i].netid == packet->netid) {
            bot.local.players[i].pos.m_x = packet->x;
            bot.local.players[i].pos.m_y = packet->y;
            bot.local.players[i].speedX = packet->xspeed;
            bot.local.players[i].speedY = packet->yspeed;
            if (bot.local.netid == bot.local.players[i].netid) {
                bot.local.pos.m_x = packet->x;
                bot.local.pos.m_y = packet->y;
                bot.local.speedX = packet->xspeed;
                bot.local.speedY = packet->yspeed;
            }
        }
    }
}

void UDP::inventoryState(uint8_t* ptr, Bot& bot) {
    if (!ptr) return;
    bot.local.items.clear();
    ptr += 56;
    ptr += 5;
    uint8_t size = *(ptr);
    for (uint8_t i = 0; i < size; i++) {
        Inventory item;
        ptr += 2;
        item.id = *(short*)(ptr);
        ptr += 2;
        item.count = *(ptr);
        if (std::find(bot.local.FavoriteItems.begin(), bot.local.FavoriteItems.end(), item.id) != bot.local.FavoriteItems.end()) {
            item.isFavorite = true;
        }
        if (std::find(bot.local.ActiveItems.begin(), bot.local.ActiveItems.end(), item.id) != bot.local.ActiveItems.end()) {
            item.isActive = true;
        }
        bot.local.items.push_back(item);
    }
}

void UDP::ModifyInventory(TankPacketStruct* packet, Bot& bot) {
    for (int i = 0; i < bot.local.items.size(); i++) {
        if (bot.local.items[i].id == packet->value) {
            int temp = bot.local.items[i].count - packet->padding1;
            if (temp < 1)
                bot.local.items.erase(bot.local.items.begin() + i);
            else
                bot.local.items[i].count = temp;
            break;
        }
    }
}


void UDP::ChangeItemObjects(uint8_t* ptr, Bot& bot) {
    TankPacketStruct* packet = reinterpret_cast<TankPacketStruct*>(ptr);

    // New Item Dropped
    if (packet->netid == UINT32_MAX) {
        DroppedItem item;
        item.id = packet->value;
        item.x = packet->x;
        item.y = packet->y;
        item.count = static_cast<uint8_t>(packet->padding4);
        item.flags = packet->dropped;
        item.uid = bot.local.world.dropped.last_dropped_item_uid + 1;
        bot.local.world.dropped.items.push_back(item);
        bot.local.world.dropped.last_dropped_item_uid += 1;
        bot.local.world.dropped.items_count += 1;
    }
    // Update Existing Dropped Item Count
    else if (packet->netid == UINT32_MAX - 3) {
        for (auto& obj : bot.local.world.dropped.items) {
            if (obj.id == packet->value && obj.x == packet->x && obj.y == packet->y) {
                obj.count = static_cast<uint8_t>(packet->padding4);
                break;
            }
        }
    }

    // Player Taking Items
    else if (packet->netid > 0) {
        int removeIndex = -1;
        for (int i = 0; i < bot.local.world.dropped.items.size(); i++) {
            if (bot.local.world.dropped.items[i].uid == packet->value) {
                if (packet->netid == bot.local.netid) {
                    if (bot.local.world.dropped.items[i].id == 112) {  // Taking gems
                        bot.local.gems += bot.local.world.dropped.items[i].count;
                    }
                    else {
                        bool added = false;
                        for (auto& inventory : bot.local.items) {
                            if (inventory.id == bot.local.world.dropped.items[i].id) {
                                int tempMount = inventory.count + bot.local.world.dropped.items[i].count;
                                inventory.count = (tempMount > 200) ? 200 : tempMount;
                                added = true;
                                break;
                            }
                        }
                        if (!added) {
                            Inventory item;
                            item.id = bot.local.world.dropped.items[i].id;
                            item.count = bot.local.world.dropped.items[i].count;
                            bot.local.items.push_back(item);
                        }
                    }
                }
                removeIndex = i;
                break;
            }
        }
        if (removeIndex != -1) {
            bot.local.world.dropped.items.erase(bot.local.world.dropped.items.begin() + removeIndex);
            bot.local.world.dropped.items_count -= 1;
        }
    }
}

void UDP::OnTileChangeRequest(TankPacketStruct* packet, uint8_t* ptr, Bot& bot) {
    if (packet->netid == bot.local.netid && packet->value != 18) { //Update Inventory
        for (int i = 0; i < bot.local.items.size(); i++) {
            if (bot.local.items[i].id == packet->value) {
                bot.local.items[i].count -= 1;
                if (bot.local.items[i].count < 1 or bot.local.items[i].count > 200) {
                    bot.local.items.erase(bot.local.items.begin() + i);
                    break;
                }
            }
        }
    }

    int index = (packet->tilex) + (packet->tiley * bot.local.world.width);
    if (index >= 0 && index <= bot.local.world.tiles.size()) {  // Pastikan index valid
        Tile& tile = bot.local.world.tiles[index];
        if (packet->value == 18) {
            if (tile.foreground_item_id != 0) {
                tile.foreground_item_id = 0;
            }
            else {
                tile.background_item_id = 0;
            }
        }
        else { //Place block
            auto tile_info = itemDefs->Get(packet->value);
            if (itemDefs->IsBackground(packet->value)) {
                tile.background_item_id = packet->value;
            }
            else {
                tile.foreground_item_id = packet->value;
                if (packet->value % 2 != 0) {
                    uint32_t time_passed = 0;
                    uint8_t item_on_tree = 0;
                    tile.tile_type = Seed{time_passed, item_on_tree, tile.foreground_item_id};
                }
            }
        }
    }
}

void UDP::OnTileTreeState(TankPacketStruct* packet, Bot& bot) {
    int index = (packet->tilex) + (packet->tiley * bot.local.world.width);
    if (index >= 0 && index <= bot.local.world.tiles.size()) {  // Pastikan index valid
        Tile& tile = bot.local.world.tiles[index];
        tile.foreground_item_id = 0;
        tile.tile_type = Basic{};
    }
}

void UDP::OnTileUpdateData(TankPacketStruct* packet, uint8_t* ptr, Bot& bot) {
    int index = (packet->tilex) + (packet->tiley * bot.local.world.width);
    if (index < 0 || index >= static_cast<int>(bot.local.world.tiles.size())) {
        return;
    }
    Tile& tile = bot.local.world.tiles[index];
    size_t position = 0;
    tile.foreground_item_id = *reinterpret_cast<const uint16_t*>(&ptr[position]);
    position += sizeof(uint16_t);

    tile.background_item_id = *reinterpret_cast<const uint16_t*>(&ptr[position]);
    position += sizeof(uint16_t);

    tile.parent_block_index = *reinterpret_cast<const uint16_t*>(&ptr[position]);
    position += sizeof(uint16_t);

    tile.flags_number = *reinterpret_cast<const uint16_t*>(&ptr[position]);
    position += sizeof(uint16_t);

    tile.flags = TileFlags::from_u16(tile.flags_number);

    if (tile.flags.has_parent) {
        uint16_t parent = *reinterpret_cast<const uint16_t*>(&ptr[position]);
        position += sizeof(uint16_t);
    }

    if (tile.flags.has_extra_data) {
        uint8_t extra_tile_type = *reinterpret_cast<const uint8_t*>(&ptr[position]);
        position += sizeof(uint8_t);
        bot.local.world.get_extra_tile_data(tile, ptr, position, extra_tile_type);
    }

    if (tile.foreground_item_id == 14666) {
        uint32_t extra_len = *reinterpret_cast<const uint32_t*>(&ptr[position]);
        position += sizeof(uint32_t);
        std::string text(reinterpret_cast<const char*>(&ptr[position]), extra_len);
        position += extra_len;
    }
}

#endif 