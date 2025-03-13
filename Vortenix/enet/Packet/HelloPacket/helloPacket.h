#pragma once
#ifndef HELLOPACKET_H
#define HELLOPACKET_H

void UDP::handleHelloPacket(Bot& bot) {
    logger(bot, "[SYSTEM]: RECEIVED HELLO LOGIN REUQEST", 0);
    if (bot.local.botloginpacket.UUIDToken.empty()) {
        std::string logToken = "protocol|" + bot.local.botloginpacket.protocol + "\nltoken|" + bot.local.server.ltoken + "\nplatformID|" + bot.local.botloginpacket.platformID;
        UDP::sendPacket(logToken, NET_MESSAGE_GENERIC_TEXT, bot);
        logger(bot, "[SYSTEM]: SENDING REQUEST\n" + logToken, 0);
    }
    else {
        bot.local.botloginpacket.tankIdName = bot.local.name;
        bot.local.botloginpacket.tankIdPass = bot.local.password;
        generator gen;
        gen.LoginPacket(bot);
        UDP::sendPacket(bot.local.botloginpacket.loginPacket, NET_MESSAGE_GENERIC_TEXT, bot);
        logger(bot, "[SYSTEM]: SENDING REQUEST:\n" + bot.local.botloginpacket.loginPacket, 0);
    }
}

#endif // !HELLOPACKET_H