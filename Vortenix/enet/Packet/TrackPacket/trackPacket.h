#pragma once
#ifndef TRACKPACKET_H
#define TRACKPACKET_H

void UDP::handleTrackPacket(ENetPacket* packet, Bot& bot) {
    char zero = 0;
    memcpy(packet->data + packet->dataLength - 1, &zero, 1);
    std::string message(reinterpret_cast<char*>(packet->data + 4));

    logger(bot, "TRACK PACKET:\n" + message, 5);
    std::map<std::string, std::string> parsedData;
    std::stringstream ss(message);
    std::string pair;
    while (std::getline(ss, pair, '\n')) {
        size_t delimiterPos = pair.find('|');
        if (delimiterPos != std::string::npos) {
            std::string key = pair.substr(0, delimiterPos);
            std::string value = pair.substr(delimiterPos + 1);
            parsedData[key] = value;
        }
    }

    if (parsedData.find("Worldlock_balance") != parsedData.end()) {
        bot.local.world_lock = std::stoi(parsedData["Worldlock_balance"]);
    }
    if (parsedData.find("Gems_balance") != parsedData.end()) {
        bot.local.gems = std::stoi(parsedData["Gems_balance"]);
    }
    if (parsedData.find("Level") != parsedData.end()) {
        bot.local.level = std::stoi(parsedData["Level"]);
    }
    if (parsedData.find("Awesomeness") != parsedData.end()) {
        bot.local.Awesomeness = std::stoi(parsedData["Awesomeness"]);
    }
}


#endif