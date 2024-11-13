#include "../include/CustomPresense.hpp"
#include "discord_rpc.h"

using namespace geode::prelude;

static const char* APPLICATION_ID = "1178492879627366472";
time_t currentTime = time(0);

bool isIdling = false;

class $modify(AppDelegate) {
	void applicationDidEnterBackground() {
		AppDelegate::applicationDidEnterBackground();
		if (Mod::get()->getSettingValue<bool>("idling")) {
			isIdling = true;
			// log::info("idle time");
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

static void handleDiscordJoin(const char* secret) {
	log::info("Getting join request! Secret: {}", secret);

	using RPCGameJoinEvent = geode::DispatchEvent<std::string>;
	RPCGameJoinEvent("game_join"_spr, secret).post();
}

static void handleDiscordSpectate(const char* secret) {
	log::info("Getting spectate request! Secret: {}", secret);
}

static void handleDiscordJoinRequest(const DiscordUser* request) {
	Discord_Respond(request->userId, DISCORD_REPLY_YES);
}

void gdrpc::GDRPC::initDiscordRP() {
	DiscordEventHandlers handlers;
	handlers.ready = handleDiscordReady;
	handlers.errored = handleDiscordError;
	handlers.disconnected = handleDiscordDisconnected;
	handlers.joinGame = handleDiscordJoin;
	handlers.spectateGame = handleDiscordSpectate;
	handlers.joinRequest = handleDiscordJoinRequest;
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
	int timeOffset,
	std::string joinSecret,
	std::string largeImageText,
	int partyMax
) {
	// if (!(mods.contains(modID))) {
	// 	log::error("Mod with ID \"{}\" is not registered. Please register your mod with DiscordRPC before utilizing it (gdrpc::GDRPC::getSharedInstance()->registerMod(\"{}\")).", modID, modID);
	// 	return;
	// }
	// log::info("hi hi id: {}", modID);
	if (modID == ""_spr && gdrpc::defaultRPCEnabled == false) {
		log::info("(ID: {}) Default RPC is **disabled**! Not updating RPC...", modID);
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
	if (largeImageText != "") {
		discordPresence.largeImageText = largeImageText.c_str();
	}
	if (isIdling) {
		discordPresence.details = "Idling";
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
	static std::string sensitiveStr;
	sensitiveStr = fmt::format("{} (playing on {})", gm->m_playerName, GEODE_PLATFORM_NAME);
	static std::string notSensitiveStr;
	notSensitiveStr = fmt::format("Playing Geometry Dash on {}", GEODE_PLATFORM_NAME);
	if (shouldShowSensitive) {
		discordPresence.largeImageText = sensitiveStr.c_str();
	} else {
		discordPresence.largeImageText = notSensitiveStr.c_str();
	}
	static std::string spectateSecret;
	spectateSecret = fmt::format("spectate{}", joinSecret);
	static std::string partyId;
	partyId = fmt::format("party{}", joinSecret);
	static std::string actualJoinSecret;
	actualJoinSecret = fmt::format("join{}", joinSecret);
	if (joinSecret != "") {
		discordPresence.joinSecret = actualJoinSecret.c_str();
		discordPresence.spectateSecret = spectateSecret.c_str();

		discordPresence.partyId = partyId.c_str();
		discordPresence.partySize = 1;
		discordPresence.partyMax = partyMax;
		discordPresence.partyPrivacy = DISCORD_PARTY_PRIVATE;
	}
    discordPresence.instance = 0;
    Discord_UpdatePresence(&discordPresence);
	// log::info("updated the rpc for mod id: {}", modID);
	// log::info("presence: {}", discordPresence.details);
}
