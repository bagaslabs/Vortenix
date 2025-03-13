#pragma once

#ifndef LOGINGENERATOR_H
#define LOGINGENERATOR_H

#include <array>              
#include <algorithm>   
#include <random>
#include "hash.h"

class generator {
private:
    class Random {
    public:
        static uint64_t RandomUInt64();
        static uint64_t RandomUInt64(uint64_t min, uint64_t max);
        static std::string RandomHex(size_t length, bool upper = false);
    };
public:
    std::string generateRid();
    std::string generateKlv(Bot& bot);
    std::string generateMac();
    std::string generateWk();

    void generateAllLoginPacket(Bot& bot);
    void LoginPacket(Bot& bot);
    void resetData(Bot& bot);
};

uint64_t generator::Random::RandomUInt64()
{
    static std::random_device hw_rand;
    static std::mt19937_64 sw_mt_rand{ hw_rand() };
    return sw_mt_rand();
}

uint64_t generator::Random::RandomUInt64(uint64_t min, uint64_t max)
{
    static std::random_device hw_rand;
    static std::mt19937_64 sw_mt_rand{ hw_rand() };
    std::uniform_int_distribution<uint64_t> distr(min, max);
    return distr(sw_mt_rand);
}

std::string generator::Random::RandomHex(size_t length, bool upper)
{
    static std::random_device hw_rand;
    static std::mt19937_64 sw_mt_rand{ hw_rand() };

    const std::string chars = upper ? "0123456789ABCDEF" : "0123456789abcdef";
    std::uniform_int_distribution<size_t> distr(0, chars.length() - 1);

    std::string ret;
    ret.reserve(length);

    for (size_t i = 0; i < length; ++i)
    {
        ret.push_back(chars[distr(sw_mt_rand)]);
    }
    return ret;
}

std::string generator::generateRid() {
    try
    {
        int bytes_length = 32 / 2;
        std::random_device rd;
        std::uniform_int_distribution<int> dist(0, 255);
        std::ostringstream oss;
        for (int i = 0; i < bytes_length; ++i)
        {
            int random_byte = dist(rd);
            oss << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << random_byte;
        }
        return oss.str();
    }
    catch (const std::exception& e)
    {
        return "";
    }
}

using json = nlohmann::json;

std::string generator::generateKlv(Bot& bot) {
    const std::string jsonFilePath = "klv.json";
    const std::string klvExePath = "klv.exe";
    std::string klv;
    try {
        json jsonData;
        jsonData["rid"] = bot.local.botloginpacket.rid;
        jsonData["version"] = bot.local.botloginpacket.game_version;
        jsonData["protocol"] = std::stoi(bot.local.botloginpacket.protocol);
        jsonData["klv"] = "";
        std::ofstream outputFile(jsonFilePath);
        if (!outputFile.is_open()) {
            throw std::runtime_error("Failed to open klv.json for writing.");
        }
        outputFile << jsonData.dump(4);
        outputFile.close();
        int returnCode = std::system(klvExePath.c_str());
        if (returnCode != 0) {
            throw std::runtime_error("Failed to execute klv.exe or it returned an error.");
        }
        std::ifstream inputFile(jsonFilePath);
        if (!inputFile.is_open()) {
            throw std::runtime_error("Failed to open klv.json for reading.");
        }
        inputFile >> jsonData;
        inputFile.close();
        klv = jsonData.value("klv", "");
        if (klv.empty()) {
            throw std::runtime_error("KLV value is empty in klv.json.");
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error in generateKlv: " << e.what() << std::endl;
    }

    return klv;
}


std::string generator::generateWk() {
    try
    {
        int bytes_length = 32 / 2;
        std::random_device rd;
        std::uniform_int_distribution<int> dist(0, 255);
        std::ostringstream oss;
        for (int i = 0; i < bytes_length; ++i)
        {
            int random_byte = dist(rd);
            oss << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << random_byte;
        }
        return oss.str();
    }
    catch (const std::exception& e)
    {
        return "";
    }
}

std::string generator::generateMac() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    std::ostringstream oss;
    for (int i = 0; i < 6; ++i)
    {
        int byte = dis(gen);
        if (i == 0)
        {
            byte = (byte & 0xFE) | 0x02;
        }
        oss << std::hex << std::setw(2) << std::setfill('0') << byte;
        if (i != 5)
        {
            oss << ":";
        }
    }
    return oss.str();
}

void generator::generateAllLoginPacket(Bot& bot) {
    logger(bot, "[SYSTEM]: Generate Login Data");
    bot.local.botloginpacket.rid = generateRid();
    bot.local.botloginpacket.klv = generateKlv(bot);
    bot.local.botloginpacket.mac = generateMac();
    bot.local.botloginpacket.wk = generateWk();
    bot.local.botloginpacket.hash2 = std::to_string(Utils::Hash::proton((bot.local.botloginpacket.mac + "RT").c_str()));
    bot.local.botloginpacket.hash = std::to_string(Utils::Hash::proton((Random::RandomHex(16, true) + "RT").c_str()));
}


void generator::LoginPacket(Bot& bot) {
    logger(bot, "[SYSTEM]: CREATING LOGIN PACKET");
    std::string loginPacket;
    loginPacket += "tankIDName|" + bot.local.botloginpacket.tankIdName + "\n";
    loginPacket += "tankIDPass|" + bot.local.botloginpacket.tankIdPass + "\n";
    loginPacket += "requestedName|" + bot.local.botloginpacket.requestedName + "\n";
    loginPacket += "f|" + bot.local.botloginpacket.f + "\n";
    loginPacket += "protocol|" + bot.local.botloginpacket.protocol + "\n";
    loginPacket += "game_version|" + bot.local.botloginpacket.game_version + "\n";
    loginPacket += "fz|" + bot.local.botloginpacket.fz + "\n";
    loginPacket += "cbits|" + bot.local.botloginpacket.cbits + "\n";
    loginPacket += "player_age|" + bot.local.botloginpacket.player_age + "\n";
    loginPacket += "GDPR|" + bot.local.botloginpacket.GDPR + "\n";
    loginPacket += "category|" + bot.local.botloginpacket.category + "\n";
    loginPacket += "totalPlaytime|" + bot.local.botloginpacket.total_playtime + "\n";
    loginPacket += "klv|" + bot.local.botloginpacket.klv + "\n";
    loginPacket += "hash2|" + bot.local.botloginpacket.hash2 + "\n";
    loginPacket += "meta|" + bot.local.botloginpacket.meta + "\n";
    loginPacket += "fhash|" + bot.local.botloginpacket.fhash + "\n";
    loginPacket += "rid|" + bot.local.botloginpacket.rid + "\n";
    loginPacket += "platformID|" + bot.local.botloginpacket.platformID + "\n";
    loginPacket += "deviceVersion|" + bot.local.botloginpacket.device_version + "\n";
    loginPacket += "country|" + bot.local.botloginpacket.country + "\n";
    loginPacket += "hash|" + bot.local.botloginpacket.hash + "\n";
    loginPacket += "mac|" + bot.local.botloginpacket.mac + "\n";
    loginPacket += "wk|" + bot.local.botloginpacket.wk + "\n";
    loginPacket += "zf|" + bot.local.botloginpacket.zf + "\n";
    loginPacket += "lmode|" + bot.local.botloginpacket.lmode + "\n";

    if (!bot.local.botloginpacket.UUIDToken.empty()) {
        loginPacket += "user|" + bot.local.botloginpacket.user + "\n";
        loginPacket += "token|" + bot.local.botloginpacket.userToken + "\n";
        loginPacket += "UUIDToken|" + bot.local.botloginpacket.UUIDToken + "\n";
        if (!bot.local.botloginpacket.doorID.empty()) {
            loginPacket += "doorID|" + bot.local.botloginpacket.doorID + "\n";
        }
        loginPacket += "aat|" + bot.local.botloginpacket.aat;
    }

    bot.local.botloginpacket.loginPacket = loginPacket;
}

void generator::resetData(Bot& bot) {
    bot.local.botloginpacket.UUIDToken.clear();
    bot.local.botloginpacket.lmode = "1";
    bot.local.botloginpacket.user.clear();
    bot.local.botloginpacket.userToken.clear();
    bot.local.botloginpacket.doorID.clear();
    bot.local.botloginpacket.aat = "2";
}


#endif 