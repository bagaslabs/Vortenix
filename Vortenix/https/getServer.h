#pragma once
#pragma once

#ifndef GETSERVER_H
#define GETSERVER_H

class HTTPS {
private:
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* out);
    const std::string growtopia_UserAgent = "UbiServices_SDK_2022.Release.9_PC64_ansi_static";
    std::string chrome_UserAgent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/125.0.0.0 Safari/537.36 Edg/125.0.0.0";
    std::map<std::string, std::string> parseResponse(const std::string& response);
    std::string urlEncode(const std::string& value);
    std::string extractLoginUrl(const std::string& page_source);
    void extractCookies(const std::string& httpResponse, Bot& bot);
    void extractFormToken(const std::string& html, Bot& bot);
    void extractFinalToken(const std::string& response, Bot& bot);
public:
    void getServer(Bot& bot);
    void getForm(Bot& bot);
    void getCookies(Bot& bot);
    void getToken(Bot& bot);
    void getAllDataHTTPS(Bot& bot);
    void clearServerLocal(Bot& bot);
};

size_t HTTPS::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* out) {
    size_t totalSize = size * nmemb;
    if (out) {
        out->append(static_cast<char*>(contents), totalSize);
    }
    return totalSize;
}

std::map<std::string, std::string> HTTPS::parseResponse(const std::string& response)
{
    std::map<std::string, std::string> parsedData;
    size_t start = 0, end = 0;
    while ((end = response.find('\n', start)) != std::string::npos)
    {
        std::string line = response.substr(start, end - start);
        size_t delimiter = line.find('|');
        if (delimiter != std::string::npos)
        {
            std::string key = line.substr(0, delimiter);
            std::string value = line.substr(delimiter + 1);
            parsedData[key] = value;
        }
        start = end + 1;
    }
    return parsedData;
}

std::string HTTPS::urlEncode(const std::string& value)
{
    std::ostringstream encoded;
    encoded << std::hex << std::uppercase;
    for (char c : value)
    {
        if (isalnum(static_cast<unsigned char>(c)) || c == '-' || c == '_' || c == '.' || c == '~')
        {
            encoded << c;
        }
        else
        {
            encoded << '%' << std::setw(2) << std::setfill('0')
                << static_cast<int>(static_cast<unsigned char>(c));
        }
    }
    return encoded.str();
}

std::string HTTPS::extractLoginUrl(const std::string& page_source)
{
    std::regex re("<a[^>]*href=[\"'](https?://[^\"']*/growid/[^\"']*)[\"'][^>]*>");
    std::smatch match;
    if (std::regex_search(page_source, match, re))
    {
        return match[1].str();
    }
    return "";
}


void HTTPS::extractCookies(const std::string& httpResponse, Bot& bot)
{
    std::regex cookieRegex(R"(set-cookie:\s*([A-Z0-9_-]+)=([^;]+))", std::regex_constants::icase);
    std::smatch match;
    auto start = httpResponse.cbegin();
    auto end = httpResponse.cend();
    while (std::regex_search(start, end, match, cookieRegex))
    {
        std::string key = match[1];
        std::string value = match[2];
        if (key == "AWSALBTG")
            bot.local.server.cookie1_AWSALBTG = value;
        else if (key == "AWSALBTGCORS")
            bot.local.server.cookie2_AWSALBTGCORS = value;
        else if (key == "AWSALB")
            bot.local.server.cookie3_AWSALB = value;
        else if (key == "AWSALBCORS")
            bot.local.server.cookie4_AWSALBCORS = value;
        else if (key == "XSRF-TOKEN")
            bot.local.server.cookie5_XSRF = value;
        else if (key == "growtopia_game_session")
            bot.local.server.cookie6_GAMESESSION = value;
        start = match.suffix().first;
    }
}


void HTTPS::extractFormToken(const std::string& html, Bot& bot)
{
    std::regex tokenRegex("<input[^>]*name=\"_token\"[^>]*value=\"([^\"]+)\"");
    std::smatch match;
    if (std::regex_search(html, match, tokenRegex) && match.size() > 1)
    {
        bot.local.server.formToken = match[1].str();
    }
    else
    {
        std::cout << "\n[ERROR] Form Token not found in the response]" << std::endl;
    }
}

void HTTPS::extractFinalToken(const std::string& response, Bot& bot)
{
    try
    {
        using json = nlohmann::json;
        json parsedJson = json::parse(response);
        if (parsedJson.find("status") != parsedJson.end() && parsedJson.find("token") != parsedJson.end())
        {
            bot.local.server.statusToken = parsedJson["status"].get<std::string>();
            bot.local.server.ltoken = parsedJson["token"].get<std::string>();
        }
        else
        {
            std::cerr << "\n[ERROR]: JSON response does not contain required keys 'status' or 'token'." << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }
    catch (const nlohmann::json::exception& e)
    {
        std::cerr << "\n[ERROR]: Failed to parse JSON response: " << e.what() << std::endl;
        std::cerr << "\n[RAW RESPONSE]: " << response << std::endl;
        logger(bot, "[SYSTEM]: INVALID CREDENTIALS");
        bot.local.status = "Incorrect Credentials";
        bot.shouldRun = false;
    }
}


void HTTPS::getServer(Bot& bot) {
    logger(bot, "[SYSTEM]: POST HTTPS DATA");
    int maxtry = 0;
    if (bot.shouldRun) {
        do {
            CURL* curl;
            CURLcode res;
            curl = curl_easy_init();
            std::string body = "version=" + bot.local.botloginpacket.game_version + "&platform=" + bot.local.botloginpacket.platformID + "&protocol=" + bot.local.botloginpacket.protocol;
            if (curl and bot.shouldRun)
            {
                std::string responseString;
                struct curl_slist* headers = nullptr;
                headers = curl_slist_append(headers, "Host: www.growtopia1.com");
                headers = curl_slist_append(headers, ("User-Agent: " + growtopia_UserAgent).c_str());
                headers = curl_slist_append(headers, "Accept: /");
                headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
                headers = curl_slist_append(headers, ("Content-Length: " + std::to_string(body.size())).c_str());
                curl_easy_setopt(curl, CURLOPT_URL, "https://www.growtopia1.com/growtopia/server_data.php");
                curl_easy_setopt(curl, CURLOPT_POST, 1L);
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
                logger(
                    bot,
                    (
                        "Host: www.growtopia1.com\n"
                        "User-Agent: " + growtopia_UserAgent + "\n"
                        "Accept: */*\n"
                        "Content-Type: application/x-www-form-urlencoded\n"
                        "Content-Length: " + std::to_string(body.size()) + "\n\n" +
                        body
                        ).c_str()
                );
                bot.local.status = "getting server addres";
                bot.local.HTTPstatus = "Post Data";
                res = curl_easy_perform(curl);
                if (res != CURLE_OK)
                {
                    std::cout << "\n[EROR CURL]" << std::endl;
                    std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
                    std::cout << "Failed to perform a post method" << std::endl;
                    logger(bot, "[SYSTEM]: Failed to perform a post method");
                    bot.local.HTTPstatus = "Failed perform";
                    bot.local.status = "offline";
                }
                else {
                    if (responseString.find("RTENDMARKERBS1001") != std::string::npos) {
                        logger(bot, "[SYSTEM]: PARSING SERVER DATA");
                        bot.local.status = "parsing data";
                        std::map<std::string, std::string> data = parseResponse(responseString);
                        bot.local.server.EnetFirstIP = data["server"];
                        bot.local.server.EnetFirstPort = std::stoi(data["port"]);
                        bot.local.server.EnetNowIp = bot.local.server.EnetFirstIP;
                        bot.local.server.EnetNowPort = bot.local.server.EnetFirstPort;
                        bot.local.botloginpacket.meta = data["meta"];
                        bot.local.HTTPstatus = "succes";
                    }
                    else {
                        bot.local.HTTPstatus = "Data Not Found";
                    }
                }
                if (!bot.shouldRun) {
                    break;
                }
                curl_slist_free_all(headers);
                curl_easy_cleanup(curl);
            }
            maxtry++;
        } while (bot.local.HTTPstatus != "succes" and maxtry <= 3 and bot.shouldRun);
    }
}

void HTTPS::getForm(Bot& bot) {
    logger(bot, "[SYSTEM]: GETTING FORM TOKEN");
    bot.local.HTTPstatus = "Post Data";
    bot.local.status = "getting token";
    std::string loginUrlencodeData = HTTPS::urlEncode(bot.local.botloginpacket.loginPacket);
    int trying = 0;
    if (bot.shouldRun) {
        do {
            if (!bot.shouldRun) {
                break;
            }
            std::string response;
            CURL* curl = curl_easy_init();
            if (!curl and bot.shouldRun)
            {
                std::cerr << "\n[FAILED TO INITIALIZE CURL]" << std::endl;
                return;
            }
            struct curl_slist* headers = nullptr;
            headers = curl_slist_append(headers, ("User-Agent: " + HTTPS::chrome_UserAgent).c_str());
            headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
            curl_easy_setopt(curl, CURLOPT_URL, "https://login.growtopiagame.com/player/login/dashboard");
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, loginUrlencodeData.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
            CURLcode res = curl_easy_perform(curl);
            curl_slist_free_all(headers);

            if (res != CURLE_OK)
            {
                std::cerr << "\n[ERROR CURL]:" << curl_easy_strerror(res) << std::endl;
                bot.local.HTTPstatus = "Failed perform";
            }
            else
            {
                bot.local.server.loginFormUrl = HTTPS::extractLoginUrl(response);
                if (!bot.local.server.loginFormUrl.empty()) {
                    bot.local.HTTPstatus = "succes";
                    logger(bot, "[SYSTEM] Form Url : " + bot.local.server.loginFormUrl);
                }
                else {
                    bot.local.HTTPstatus = "Data Not Found";
                    logger(bot, "[SYSTEM]: ERROR LOGIN URL: " + response);
                    if (!bot.shouldRun) {
                        break;
                    }
                }
            }
            if (!bot.shouldRun) {
                break;
            }
            std::cout << response << std::endl;
            trying++;
        } while (trying < 3 and bot.local.HTTPstatus != "succes" and bot.shouldRun and bot.local.server.loginFormUrl.empty());
        if (bot.local.server.loginFormUrl.empty()) {
            std::ifstream configFile("config.json");
            json config;
            if (configFile.is_open()) {
                configFile >> config;
                configFile.close();
            }
            auto it = config.find("login_form");
            if (it != config.end()) {
                bot.local.server.loginFormUrl = it->get<std::string>();
                logger(bot, "[SYSTEM]: BYPASSING FORM URL");
                bot.local.HTTPstatus = "bypas form url";
            }
        }
    }
}

void HTTPS::getCookies(Bot& bot) {
    bot.local.status = "getting token";
    int trying = 0;
    logger(bot, "[SYSTEM]: Getting Cookies Data");
    if (bot.shouldRun) {
        do {
            CURL* curl;
            CURLcode res;
            std::string response;
            curl_global_init(CURL_GLOBAL_DEFAULT);
            curl = curl_easy_init();
            if (curl && !bot.local.server.loginFormUrl.empty() && bot.shouldRun)
            {
                struct curl_slist* headers = NULL;
                headers = curl_slist_append(headers, "sec-ch-ua: \"Google Chrome\";v=\"131\", \"Chromium\";v=\"131\", \"Not_A Brand\";v=\"24\"");
                headers = curl_slist_append(headers, "sec-ch-ua-mobile: ?1");
                headers = curl_slist_append(headers, "sec-ch-ua-platform: \"Windows\"");
                headers = curl_slist_append(headers, "upgrade-insecure-requests: 1");
                headers = curl_slist_append(headers, ("user-agent: " + HTTPS::chrome_UserAgent).c_str());
                headers = curl_slist_append(headers, "accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,/;q=0.8,application/signed-exchange;v=b3;q=0.7");
                headers = curl_slist_append(headers, "sec-fetch-site: cross-site");
                headers = curl_slist_append(headers, "sec-fetch-mode: navigate");
                headers = curl_slist_append(headers, "sec-fetch-dest: document");
                headers = curl_slist_append(headers, "accept-encoding: gzip, deflate, br, zstd");
                headers = curl_slist_append(headers, "accept-language: id-ID,id;q=0.9,en-US;q=0.8,en;q=0.7");

                std::time_t now = std::time(nullptr);
                bot.local.server.cookievisit = std::to_string(now);
                bot.local.server.cookieactivity = std::to_string(now);
                headers = curl_slist_append(headers, ("cookie: bblastvisit=" + bot.local.server.cookievisit).c_str());
                headers = curl_slist_append(headers, ("cookie: bblastactivity=" + bot.local.server.cookieactivity).c_str());

                curl_easy_setopt(curl, CURLOPT_URL, bot.local.server.loginFormUrl.c_str());
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
                curl_easy_setopt(curl, CURLOPT_HEADER, 1L);
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
                curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

                bot.local.HTTPstatus = "Post Data";
                res = curl_easy_perform(curl);

                if (res == CURLE_OK) {
                    HTTPS::extractCookies(response, bot);
                    HTTPS::extractFormToken(response, bot);
                    if (bot.local.server.cookie1_AWSALBTG.empty() || bot.local.server.formToken.empty()) {
                        bot.local.HTTPstatus = "Data Not Found";
                        logger(bot, "[SYSTEM]: Getting Cookies Data FAILED");
                    }
                    else {
                        logger(bot, "[SYSTEM]: Getting Cookies Data SUCCESS");
                        bot.local.HTTPstatus = "success";
                    }
                }
                else {
                    logger(bot, "[SYSTEM]: ERROR CURL OR LOGIN FORM NOT FOUND");
                }

                if (!bot.shouldRun) {
                    break;
                }
                trying++;
            }
        } while (trying < 5 && bot.local.HTTPstatus != "success" && bot.shouldRun &&
            bot.local.server.cookie1_AWSALBTG.empty() && !bot.local.server.loginFormUrl.empty());
    }
}

void HTTPS::getToken(Bot& bot) {
    if (!bot.local.server.formToken.empty() && !bot.local.server.loginFormUrl.empty() && bot.shouldRun) {
        bot.local.status = "getting token";
        logger(bot, "[SYSTEM]: Getting Login Token");
        int attempts = 0;
        const int maxAttempts = 5;
        do {
            CURL* curl;
            CURLcode res;
            std::string response;
            curl_global_init(CURL_GLOBAL_DEFAULT);
            curl = curl_easy_init();

            std::string urlValidate = "https://login.growtopiagame.com/player/growid/login/validate";
            std::string bodyT = "_token=" + bot.local.server.formToken +
                "&growId=" + bot.local.name +
                "&password=" + bot.local.password;
            if (curl) {
                struct curl_slist* headers = nullptr;

                // Add headers
                headers = curl_slist_append(headers, "HTTP/2.0");
                headers = curl_slist_append(headers, ("Content-Length: " + std::to_string(bodyT.size())).c_str());
                headers = curl_slist_append(headers, "Cache-Control: max-age=0");
                headers = curl_slist_append(headers, "Sec-CH-UA: \"Google Chrome\";v=\"131\", \"Chromium\";v=\"131\", \"Not_A Brand\";v=\"24\"");
                headers = curl_slist_append(headers, "Sec-CH-UA-Mobile: ?1");
                headers = curl_slist_append(headers, "Sec-CH-UA-Platform: \"Windows\"");
                headers = curl_slist_append(headers, "Origin: https://login.growtopiagame.com");
                headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
                headers = curl_slist_append(headers, "Upgrade-Insecure-Requests: 1");
                headers = curl_slist_append(headers, ("User-Agent: " + HTTPS::chrome_UserAgent).c_str());
                headers = curl_slist_append(headers, "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7");
                headers = curl_slist_append(headers, "Sec-Fetch-Site: same-origin");
                headers = curl_slist_append(headers, "Sec-Fetch-Mode: navigate");
                headers = curl_slist_append(headers, "Sec-Fetch-User: ?1");
                headers = curl_slist_append(headers, "Sec-Fetch-Dest: document");
                headers = curl_slist_append(headers, ("Referer: " + bot.local.server.loginFormUrl).c_str());
                headers = curl_slist_append(headers, "Accept-Encoding: gzip, deflate, br, zstd");
                headers = curl_slist_append(headers, "Accept-Language: id-ID,id;q=0.9,en-US;q=0.8,en;q=0.7");

                // Add cookies
                headers = curl_slist_append(headers, ("cookie: bblastvisit=" + bot.local.server.cookievisit).c_str());
                headers = curl_slist_append(headers, ("cookie: bblastactivity=" + bot.local.server.cookieactivity).c_str());
                headers = curl_slist_append(headers, ("cookie: AWSALBTG=" + bot.local.server.cookie1_AWSALBTG).c_str());
                headers = curl_slist_append(headers, ("cookie: AWSALBTGCORS=" + bot.local.server.cookie2_AWSALBTGCORS).c_str());
                headers = curl_slist_append(headers, ("cookie: AWSALB=" + bot.local.server.cookie3_AWSALB).c_str());
                headers = curl_slist_append(headers, ("cookie: AWSALBCORS=" + bot.local.server.cookie4_AWSALBCORS).c_str());
                headers = curl_slist_append(headers, ("cookie: XSRF-TOKEN=" + bot.local.server.cookie5_XSRF).c_str());
                headers = curl_slist_append(headers, ("cookie: growtopia_game_session=" + bot.local.server.cookie6_GAMESESSION).c_str());
                headers = curl_slist_append(headers, "Priority: u=0, i");

                // Set cURL options
                curl_easy_setopt(curl, CURLOPT_URL, urlValidate.c_str());
                curl_easy_setopt(curl, CURLOPT_POST, 1L);
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, bodyT.c_str());
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
                curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");

                // Perform the request
                bot.local.HTTPstatus = "Post Data";
                logger(bot, "[SYSTEM]: Sending Body: " + bodyT);
                res = curl_easy_perform(curl);
                if (res != CURLE_OK) {
                    bot.local.HTTPstatus = "Failed Perform";
                    logger(bot, "[CURL ERROR]: " + response);
                    std::cerr << "\n[FAILED TO GET TOKEN ERROR]: " << curl_easy_strerror(res) << std::endl;
                }
                else {
                    HTTPS::extractFinalToken(response, bot);
                    if (!bot.local.server.ltoken.empty()) {
                        bot.local.HTTPstatus = "success";
                        logger(bot, "[SYSTEM]: TOKEN: " + bot.local.server.ltoken);
                    }
                    else {
                        bot.local.status = "invalid credentials";
                        bot.local.HTTPstatus = "Data Not Found";
                        logger(bot, "[SYSTEM]:\n" + response);
                        bot.shouldRun = false;
                    }
                }
                // Clean up
                curl_slist_free_all(headers);
                curl_easy_cleanup(curl);
            }
            if (!bot.shouldRun) {
                break;
            }
            attempts++;
        } while (attempts < maxAttempts && bot.local.HTTPstatus != "success" && bot.local.server.ltoken.empty() && bot.shouldRun && bot.local.status == "Incorrect Credentials");
        curl_global_cleanup();
    }
}

void HTTPS::clearServerLocal(Bot& bot) {
    bot.local.server.loginFormUrl.clear();
    bot.local.server.formToken.clear();
    bot.local.server.cookievisit.clear();
    bot.local.server.cookieactivity.clear();
    bot.local.server.cookie1_AWSALBTG.clear();
    bot.local.server.cookie2_AWSALBTGCORS.clear();
    bot.local.server.cookie3_AWSALB.clear();
    bot.local.server.cookie4_AWSALBCORS.clear();
    bot.local.server.cookie5_XSRF.clear();
    bot.local.server.cookie6_GAMESESSION.clear();
    bot.local.server.statusToken.clear();
    bot.local.server.ltoken.clear();

    bot.local.server.EnetSubServerIP.clear();
    bot.local.server.EnetSubServerPort = 0;
}

void HTTPS::getAllDataHTTPS(Bot& bot) {
    HTTPS::clearServerLocal(bot);
    HTTPS::getForm(bot);
    HTTPS::getCookies(bot);
    HTTPS::getToken(bot);
    bot.local.HTTPstatus = "nothing";
}



#endif


