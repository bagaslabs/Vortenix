#pragma once
#ifndef GAMEMESSAGE_H
#define GAMEMESSAGE_H

void UDP::handleGameMessage(const std::vector<uint8_t>& data, Bot& bot) {
    std::string message(reinterpret_cast<const char*>(data.data()), data.size());
    logger(bot, "[SYSTEM]: RECEIVED GAME MESSAGE:\n" + message, 1);
    std::cout << message << std::endl;

    if (message.find("logon_fail") != std::string::npos) {
        bot.local.status = "login_fail";
        generator gen;
        gen.resetData(bot);
        bot.local.server.EnetNowIp = bot.local.server.EnetFirstIP;
        bot.local.server.EnetNowPort = bot.local.server.EnetFirstPort;
        UDP::disconnectClient(bot);
        UDP::connectClient(bot);
    }
    else if (message.find("currently banned") != std::string::npos) {
        bot.shouldRun = false;
        bot.local.status = "banned";
    }
    else if (message.find("View GT Twitter") != std::string::npos) {
        bot.shouldRun = false;
        bot.local.status = "maintenance";
    }
    else if (message.find("password is wrong") != std::string::npos) {
        bot.shouldRun = false;
        bot.local.status = "wrong password";
    }
    else if (message.find("Advanced Account Protection") != std::string::npos) {
        bot.shouldRun = false;
        bot.local.status = "AAP";
    }
    else if (message.find("temporarily suspended") != std::string::npos) {
        bot.shouldRun = false;
        bot.local.status = "temporarily suspended";
    }
    else if (message.find("has been suspended") != std::string::npos) {
        bot.shouldRun = false;
        bot.local.status = "suspended";
    }
    else if (message.find("Growtopia is not quite ready for users") != std::string::npos) {
        bot.local.status = "Server issues";
        bot.shouldRun = false;
    }
    else if (message.find("UPDATE REQUIRED") != std::string::npos) {
        bot.local.status = "UPDATE";
        std::regex versionRegex(R"(\$V([\d\.]+))");
        std::smatch match;
        if (std::regex_search(message, match, versionRegex) && match.size() > 1) {
            std::string newVersion = match[1].str();
            logger(bot, "[SYSTEM]: Detected update version: " + newVersion);
            const std::string configFilePath = "config.json";
            std::ifstream configFile(configFilePath);
            if (configFile.is_open()) {
                nlohmann::json config;
                configFile >> config;
                configFile.close();
                if (config.find("version") != config.end()) {
                    config["version"] = newVersion;
                    std::ofstream outFile(configFilePath);
                    if (outFile.is_open()) {
                        outFile << config.dump(4);
                        outFile.close();
                        logger(bot, "[SYSTEM]: Updated config.json version to " + newVersion + ". You Need To Connect Again");
                        bot.shouldRun = false;
                    }
                    else {
                        std::cerr << "[ERROR]: Failed to write to config.json file!" << std::endl;
                    }
                }
            }
            else {
                std::cerr << "[ERROR]: Failed to open config.json file!" << std::endl;
            }
        }
        else {
            std::cerr << "[ERROR]: Failed to detect version in the update message!" << std::endl;
        }
    }
}

#endif