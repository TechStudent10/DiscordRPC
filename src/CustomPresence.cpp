#include "../include/CustomPresence.hpp"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

static const char* APPLICATION_ID = "1178492879627366472";
time_t currentTime = time(0);

static void handleDiscordReady(const DiscordUser* user) {
	log::info("Connected to Discord RPC");
	log::info("Username: {}", user->username);
	log::info("UserID: {}", user->userId);
}

static void handleDiscordError(int errorCode, const char* message) {
	log::info("Discord RPC error");
	log::info("Error Code: {}", std::to_string(errorCode));
	log::info("Message: {}", std::string(message));
}

static void handleDiscordDisconnected(int errorCode, const char* message) {
	log::info("Discord RPC disconnected");
	log::info("Error Code: {}", std::to_string(errorCode));
	log::info("Message: {}", std::string(message));
}

void gdrpc::GDRPC::initDiscordRP() {
	DiscordEventHandlers handlers;
	handlers.ready = handleDiscordReady;
	handlers.errored = handleDiscordError;
	handlers.disconnected = handleDiscordDisconnected;
	Discord_Initialize(APPLICATION_ID, &handlers, 1, "322170");
	Discord_RunCallbacks();

}

void gdrpc::GDRPC::updateDiscordRP(
    std::string details,
    std::string state,
    std::string smallImageKey,
    std::string smallImageText,
    bool useTime,
    bool shouldResetTime,
    std::string largeImage
) {
	auto gm = GameManager::sharedState();
	auto shouldShowSensitive = Mod::get()->getSettingValue<bool>("private-info");
	auto shouldShowTime = Mod::get()->getSettingValue<bool>("show-time");
	DiscordRichPresence discordPresence{};
    discordPresence.details = details.c_str();
    discordPresence.state = state.c_str();
    if (largeImage == "") {
        discordPresence.largeImageKey = "gd_large";
    } else {
        discordPresence.largeImageKey = largeImage.c_str();
    }
	if (useTime && shouldShowTime) {
		if (shouldResetTime) currentTime = time(0);
		discordPresence.startTimestamp = currentTime;
	}
	if (shouldShowSensitive) {
    	discordPresence.largeImageText = fmt::format("{} (playing on {})", gm->m_playerName, GEODE_PLATFORM_NAME).c_str();
	} else {
		discordPresence.largeImageText = fmt::format("Playing Geometry Dash on {}", GEODE_PLATFORM_NAME).c_str();
	}
	if (smallImageKey != "") {
		discordPresence.smallImageKey = smallImageKey.c_str();
		discordPresence.smallImageText = smallImageText.c_str();
	}
    discordPresence.instance = 0;
    Discord_UpdatePresence(&discordPresence);
}
