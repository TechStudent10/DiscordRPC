#include "../include/CustomPresense.hpp"
#include "Geode/modify/AppDelegate.hpp"
#include <Geode/Geode.hpp>
// #include <string>

using namespace geode::prelude;

static const char* APPLICATION_ID = "1178492879627366472";
time_t currentTime = time(0);

bool isIdling = false;

class $modify(AppDelegate) {
	void applicationDidEnterBackground() {
		AppDelegate::applicationDidEnterBackground();
		if (Mod::get()->getSettingValue<bool>("idling")) {
			isIdling = true;
			log::info("idle time");
			gdrpc::GDRPC::getSharedInstance()->updateDiscordRP("techstudent10.discord_rich_presence", "idling");
		}
	}

	void applicationWillEnterForeground() {
		AppDelegate::applicationWillEnterForeground();
		isIdling = false;
	}
};


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
	std::string modID,
    std::string details,
    std::string state,
    std::string smallImageKey,
    std::string smallImageText,
    bool useTime,
    bool shouldResetTime,
    std::string largeImage,
	int timeOffset
) {
	if (!(mods.contains(modID))) {
		log::error("Mod with ID \"{}\" is not registered. Please register your mod with DiscordRPC before utilizing it (gdrpc::GDRPC::getSharedInstance()->registerMod(\"{}\")).", modID, modID);
		return;
	}
	auto gm = GameManager::sharedState();
	auto shouldShowSensitive = Mod::get()->getSettingValue<bool>("private-info");
	auto shouldShowTime = Mod::get()->getSettingValue<bool>("show-time");
	DiscordRichPresence discordPresence{};
    if (largeImage == "") {
        discordPresence.largeImageKey = "gd_large";
    } else {
        discordPresence.largeImageKey = largeImage.c_str();
    }
	if (isIdling) {
		discordPresence.details = "Idling";
		if (shouldShowSensitive) {
			discordPresence.largeImageText = fmt::format("{} (playing on {})", gm->m_playerName, GEODE_PLATFORM_NAME).c_str();
		} else {
			discordPresence.largeImageText = fmt::format("Playing Geometry Dash on {}", GEODE_PLATFORM_NAME).c_str();
		}
	} else {
		discordPresence.details = details.c_str();
		discordPresence.state = state.c_str();
		if (useTime && shouldShowTime) {
			if (shouldResetTime) currentTime = time(0);
			discordPresence.startTimestamp = currentTime - timeOffset;
		}
		if (smallImageKey != "") {
			discordPresence.smallImageKey = smallImageKey.c_str();
			discordPresence.smallImageText = smallImageText.c_str();
		}
	}
    discordPresence.instance = 0;
    Discord_UpdatePresence(&discordPresence);
}
