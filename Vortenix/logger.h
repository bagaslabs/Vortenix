#pragma once

#ifndef LOGGER_H
#define LOGGER_H


void ApplyCustomTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.FrameRounding = 0.0f;
    style.ChildRounding = 0.0f;
    style.PopupRounding = 0.0f;
    style.ScrollbarRounding = 0.0f;
    style.GrabRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.12f, 0.18f, 1.0f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.2f, 0.4f, 0.6f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.5f, 0.8f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.1f, 0.3f, 0.5f, 1.0f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.25f, 0.25f, 0.4f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.35f, 0.35f, 0.55f, 1.0f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.5f, 0.5f, 0.5f, 0.5f);
}


void AddButtonInput(const char* placeholderText, char* inputBuffer, int bufferSize, ImVec2 position, float inputWidth = 200.0f) {
    ImGui::SetCursorPos(position);
    ImGui::PushItemWidth(inputWidth);
    if (ImGui::InputText(("##" + std::string(placeholderText)).c_str(), inputBuffer, bufferSize, ImGuiInputTextFlags_CharsNoBlank)) {
    }

    if (!ImGui::IsItemActive() && strlen(inputBuffer) == 0) {
        ImVec2 inputPos = ImGui::GetItemRectMin();
        ImVec2 inputSize = ImGui::GetItemRectSize();
        ImVec2 placeholderPos = ImVec2(
            inputPos.x + (inputWidth - ImGui::CalcTextSize(placeholderText).x) / 2.0f,
            inputPos.y + (inputSize.y - ImGui::CalcTextSize(placeholderText).y) / 2.0f
        );

        ImGui::GetWindowDrawList()->AddText(
            placeholderPos, ImGui::GetColorU32(ImVec4(0.5f, 0.5f, 0.5f, 1.0f)), placeholderText
        );
    }

    ImGui::PopItemWidth();
}

struct Notification {
    std::string message;
    std::chrono::steady_clock::time_point startTime;
    int duration;
};

std::deque<Notification> notifications;

void ShowNotification(const std::string& message, int duration = 3) {
    notifications.push_back({ message, std::chrono::steady_clock::now(), duration });
}

void RenderNotifications() {
    float notificationWidth = 300.0f;
    float notificationHeight = 50.0f;
    float padding = 10.0f;

    if (!notifications.empty()) {
        auto& notif = notifications.front();
        float elapsedTime = std::chrono::duration<float>(std::chrono::steady_clock::now() - notif.startTime).count();
        if (elapsedTime > notif.duration || notifications.size() > 1) {
            notifications.pop_front();
        }
        else {
            ImVec2 windowSize = ImGui::GetIO().DisplaySize;
            float x = windowSize.x - notificationWidth - padding;
            float y = padding;

            ImGui::SetNextWindowPos(ImVec2(x, y));
            ImGui::SetNextWindowSize(ImVec2(notificationWidth, notificationHeight));
            ImGui::Begin("Notification##Fixed", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::TextWrapped("%s", notif.message.c_str());
            ImGui::End();
        }
    }
}

auto ToggleButton = [](const char* label, bool* v) {
    if (*v)
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.8f, 0.2f, 1.0f)); // Hijau saat aktif
    else
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.5f, 1.0f, 1.0f)); // Biru default

    if (ImGui::Button(label, ImVec2(150, 30))) {
        *v = !(*v); // Toggle status
    }

    ImGui::PopStyleColor();
    };

std::unordered_map<uint16_t, ImU32> predefined_colors = {
    {10, IM_COL32(128, 128, 128, 255)}, // Gray for ID 10
    {4, IM_COL32(255, 69, 0, 255)}, // Orange like lava for ID 4
    {6, IM_COL32(255, 255, 255, 255)}, // White for ID 6
    {0, IM_COL32(135, 206, 235, 255)}, // Sky Blue for ID 0
    {2, IM_COL32(139, 69, 19, 255)}, // Brown for ID 2
    {14, IM_COL32(160, 82, 45, 255)}, // SaddleBrown for ID 14
    {8, IM_COL32(50, 50, 50, 255)} // Dark Gray for Bedrock
};


ImU32 default_color = IM_COL32(128, 0, 128, 255); // Purple

ImU32 GetTileColor(uint16_t id) {
    if (predefined_colors.find(id) != predefined_colors.end()) {
        return predefined_colors[id];
    }
    return default_color;
}

void logger(Bot& bot, const std::string& originalMessage, const int& logType = -1) {
    std::string message = std::string(originalMessage) + "\n\n";
    bot.local.allDebug.push_back(message);
    switch (logType) {
    case 0:
        bot.local.onGenerixText.push_back(message);
        break;
    case 1:
        bot.local.gameMessageDebug.push_back(message);
        break;
    case 2:
        bot.local.tankPacketDebug.push_back(message);
        break;
    case 3:
        bot.local.onVariantListDebug.push_back(variantlist_t(message));
        break;
    default:
        // Handle unknown logType if needed
        break;
    }
}

void Bot::readConfigJson(Bot& bot) {
    const std::string filename = "config.json";
    std::ifstream file(filename);
    if (!file.is_open())
    {
        logger(bot, "[SYSTEM]: Failed To Open File Config.json");
        std::cerr << "Failed to open file: " << filename << std::endl;
    }
    try
    {
        nlohmann::json json_data;
        file >> json_data;
        bot.local.botloginpacket.game_version = json_data["version"].get<std::string>();
        bot.local.botloginpacket.protocol = json_data["protocol"].get<std::string>();
        logger(bot, "[SYSTEM]: Version Updated To: " + bot.local.botloginpacket.game_version + ", Protocol: " + bot.local.botloginpacket.protocol);
    }
    catch (const std::exception& e)
    {
        logger(bot, "[SYSTEM]: Failed To PARSING File Config.json");
        std::cerr << "Error saat parsing JSON: " << e.what() << std::endl;
    }
}

int getIndexByName(const std::string &name) {
    for (int i = 0; i < bot.size(); i++) {
        if (bot[i].local.name.find(name) != std::string::npos) {
            return i;
        }
    }
    return -1;
}

#endif