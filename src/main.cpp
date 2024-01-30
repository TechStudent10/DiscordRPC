#include <Geode/Geode.hpp>

#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>

#include <discord_register.h>
#include <discord_rpc.h>

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

static void initDiscordRP() {
	DiscordEventHandlers handlers;
	handlers.ready = handleDiscordReady;
	handlers.errored = handleDiscordError;
	handlers.disconnected = handleDiscordDisconnected;
	Discord_Initialize(APPLICATION_ID, &handlers, 1, "322170");
	Discord_RunCallbacks();

}

void updateDiscordRP(std::string details, std::string state = "", std::string smallImageKey = "", std::string smallImageText = "", bool useTime = false, bool shouldResetTime = false) {
	auto gm = GameManager::sharedState();
	auto shouldShowSensitive = Mod::get()->getSettingValue<bool>("private-info");
	auto shouldShowTime = Mod::get()->getSettingValue<bool>("show-time");
	DiscordRichPresence discordPresence{};
    discordPresence.details = details.c_str();
    discordPresence.state = state.c_str();
    discordPresence.largeImageKey = "gd_large";
	if (useTime && shouldShowTime) {
		if (shouldResetTime) currentTime = time(0);
		discordPresence.startTimestamp = currentTime;
	}
	if (shouldShowSensitive) {
    	discordPresence.largeImageText = gm->m_playerName.c_str();
	} else {
		discordPresence.largeImageText = "Playing a Game";
	}
	if (smallImageKey != "") {
		discordPresence.smallImageKey = smallImageKey.c_str();
		discordPresence.smallImageText = smallImageText.c_str();
	}
    discordPresence.instance = 0;
    Discord_UpdatePresence(&discordPresence);
}

std::string convertGJDifficultyDemonToAssetKey(int difficulty) {
	switch (difficulty) {
		case 3:
			return "easy_demon";
		case 4:
			return "medium_demon";
		case 0:
			return "hard_demon";
		case 5:
			return "insane_demon";
		case 6:
			return "extreme_demon";
	}
	return "na";
}

std::string convertGJDifficultyToAssetKey(int difficulty) {
	switch (difficulty) {
		case -1:
			return "auto";
		case 0:
			return "na";
		case static_cast<int>(GJDifficulty::Easy):
			return "easy";
		case static_cast<int>(GJDifficulty::Normal):
			return "normal";
		case static_cast<int>(GJDifficulty::Hard):
			return "hard";
		case static_cast<int>(GJDifficulty::Harder):
			return "harder";
		case static_cast<int>(GJDifficulty::Insane):
			return "insane";
	}
	return "na";
}

std::string convertRobTopLevelToAssetKey(int lvlID) {
	switch (lvlID) {
		case 1:
			return "easy"; // Stereo Madness
		case 2:
			return "easy"; // Back On Track
		case 3:
			return "normal"; // Polargeist
		case 4:
			return "normal"; // Dry Out
		case 5:
			return "hard"; // Base After Base
		case 6:
			return "hard"; // Can't Let Go
		case 7:
			return "harder"; // Jumper
		case 8:
			return "harder"; // Time Machine
		case 9:
			return "harder"; // Cycles
		case 10:
			return "insane"; // xStep
		case 11:
			return "insane"; // Clutterfunk
		case 12:
			return "insane"; // Theory of Everything
		case 13:
			return "insane"; // Electroman Adventures
		case 14:
			return "hard_demon"; // Clubstep
		case 15:
			return ((Mod::get()->getSettingValue<bool>("funny-mode")) ? "easy_demon" : "insane"); // Electrodynamix
		case 16:
			return "insane"; // Hexagon Force
		case 17:
			return "harder"; // Blast Processing
		case 18:
			return "hard_demon"; // TOE 2
		case 19:
			return "harder"; // Geometrical Dominator
		case 20:
			return "hard_demon"; // Deadlocked
		case 21:
			return "insane"; // Fingerdash
		case 22:
			return "insane"; // Dash
		case 23:
			return "hard_demon"; // Explorers
		case 3001:
			return "hard"; // The Challenge
	}
}

auto getAverageDifficulty(GJGameLevel* level) {
	return level->m_ratings / level->m_ratingsSum;
}

std::string getAssetKey(GJGameLevel* level) {
	int stars = level->m_stars.value();
	// auto difficulty = getAverageDifficulty(level); // binding
	auto difficulty = level->getAverageDifficulty();
	
	log::info("demoz: {}", std::to_string(level->m_demonDifficulty));
	// log::info(std::to_string(level->m_levelID.value()));
	log::info("diff: {}", std::to_string(difficulty));
	if (stars == 0) {
		return convertGJDifficultyToAssetKey(difficulty);
	}
	if (stars == 10) {
		return convertGJDifficultyDemonToAssetKey(level->m_demonDifficulty);
	}
	if (level->m_levelID.value() < 128 || level->m_levelID.value() == 3001) {
		return convertRobTopLevelToAssetKey(level->m_levelID.value());
	}

	switch (stars) {
		case 1:
			return "auto";
		case 2:
			return "easy";
		case 3:
			return "normal";
		case 4:
			return "hard";
		case 5:
			return "hard";
		case 6:
			return "harder";
		case 7:
			return "harder";
		case 8:
			return "insane";
		case 9:
			return "insane";
	}
	return "na";
}

class $modify(MenuLayer) {
	bool init() {
		if (!MenuLayer::init()) return false;
		initDiscordRP();
		updateDiscordRP("Browsing Menus");

		return true;
	}
};

class $modify(CreatorLayer) {
	bool init() {
		if (!CreatorLayer::init()) return false;
		updateDiscordRP("Browsing Menus", "Creator Tab");
		return true;
	}
	
	// TODO: uncomment this stuff when geode implements functionality for adding virtuals

	void onLeaderboards(CCObject* p0) {
		updateDiscordRP("Browsing Menus", "Checking out leaderboards");
		return CreatorLayer::onLeaderboards(p0);
	}

	void onMyLevels(CCObject* p0) {
		updateDiscordRP("Browsing Menus", "Checking out created levels");
		return CreatorLayer::onMyLevels(p0);
	}

	void onSavedLevels(CCObject* p0) {
		updateDiscordRP("Browsing Menus", "Checking out saved levels");
		return CreatorLayer::onSavedLevels(p0);
	}

	void onMapPacks(CCObject* p0) {
		auto shouldBeFunny = Mod::get()->getSettingValue<bool>("funny-mode");
		std::string state = "";
		if (shouldBeFunny) {
			state = "Checking out the worst levels known to man";
		} else {
			state = "Checking out map packs";
		}
		updateDiscordRP("Browsing Menus", state);
		return CreatorLayer::onMapPacks(p0);
	}

	void onDailyLevel(CCObject* p0) {
		updateDiscordRP("Browsing Menus", "Checking out the daily level");
		return CreatorLayer::onDailyLevel(p0);
	}

	void onWeeklyLevel(CCObject* p0) {
		updateDiscordRP("Browsing Menus", "Checking out the weekly level");
		return CreatorLayer::onWeeklyLevel(p0);
	}

	void onFeaturedLevels(CCObject* p0) {
		updateDiscordRP("Browsing Menus", "Checking out the featured tab");
		return CreatorLayer::onFeaturedLevels(p0);
	}

	void onGauntlets(CCObject* p0) {
		auto shouldBeFunny = Mod::get()->getSettingValue<bool>("funny-mode");
		std::string state = "";
		if (shouldBeFunny) {
			state = "Checking out the better map packs";
		} else {
			state = "Checking out the gauntlets";
		}
		updateDiscordRP("Browsing Menus", state);
		return CreatorLayer::onGauntlets(p0);
	}
};

class $modify(LevelSearchLayer) {
	bool init(int p0) {
		if (!LevelSearchLayer::init(p0)) return false;
		updateDiscordRP("Browsing Menus", "Search Tab");
		return true;
	}
};

class $modify(LevelInfoLayer) {
	bool init(GJGameLevel* level, bool p1) {
		if (!LevelInfoLayer::init(level, p1)) return false;
		// testing stuff, this works properly now
		///
		// log::info(std::to_string(
		// 	static_cast<int>(level->getAverageDifficulty())
		// ));
		// log::info(std::to_string(
		// 	static_cast<int>(level->m_difficulty)
		// ));
		// log::info(std::to_string(
		// 	static_cast<int>(level->m_demonDifficulty)
		// ));
		// // TODO: ignore all todos below and just use the goddamn star count. only use m_difficulty for unrated levels
		// // TODO: Determine if Easy and Normal rated level are not Easy/Medium demons somehow
		// // TODO: Then I need to add the N/A difficulty face
		// // TODO: And finally I need to distinguish between demons and non-demons
		// // ... all of this for a 512px/512px circle face on texting platform
		
		// log::info(std::to_string(level->m_stars.value()));

		bool isRated = level->m_stars.value() != 0;
		
		updateDiscordRP(
			"Viewing Level",
			std::string(level->m_levelName) + " by " + std::string(level->m_creatorName),
			getAssetKey(level),
			(isRated) ? "Rated" : "Not Rated"
		);
		return true;
	}
};

class $modify(MyLevelEditorLayer, LevelEditorLayer) {
	bool init(GJGameLevel* level, bool p0) {
		if (!LevelEditorLayer::init(level, p0)) return false;

		MyLevelEditorLayer::updateStatus();

		return true;
	}

	void updateStatus() {
		auto shouldShowSensitive = Mod::get()->getSettingValue<bool>("private-info");
		std::string details = "";
		if (shouldShowSensitive) {
			details = "Working on level \"" + std::string(m_level->m_levelName) + "\"";
		} else {
			details = "Working on a level";
		}
		updateDiscordRP(details, std::to_string(m_level->m_objectCount.value()) + " objects as of opening the editor", "", "", true);
	}

	// GameObject* createObject(int objectID, CCPoint pos, bool p2) {
	// 	auto object = LevelEditorLayer::createObject(objectID, pos, p2); // find bindings for this too

	// 	updateStatus();

	// 	return object;
	// }

	// void removeObject(GameObject* object, bool p1) {
	// 	LevelEditorLayer::removeObject(object, p1); // binding
	// 	updateStatus();
// 	}
};

class $modify(MyPlayLayer, PlayLayer) {
	bool init(GJGameLevel* level, bool p1, bool p2) {
		if (!PlayLayer::init(level, p1, p2)) return false;

		MyPlayLayer::updateRP(true);

		return true;
	}

	void showNewBest(bool p0, int p1, int p2, bool p3, bool p4, bool p5) {
		PlayLayer::showNewBest(p0, p1, p2, p3, p4, p5);

		MyPlayLayer::updateRP();
	}

	void resetLevel() {
		PlayLayer::resetLevel();
		MyPlayLayer::updateRP();
	}

	void levelComplete() {
		PlayLayer::levelComplete();
		MyPlayLayer::updateRP();
	}

	void updateRP(bool isInitial = false) {
		log::info("is initial: {}", std::to_string(isInitial));

		bool isRated = m_level->m_stars.value() != 0;
		auto shouldShowSensitive = Mod::get()->getSettingValue<bool>("private-info");

		std::string state;

		bool isRobTopLevel = m_level->m_levelID.value() < 128 || m_level->m_levelID.value() == 3001;

		if (m_level->m_levelType != GJLevelType::Editor || shouldShowSensitive) {
			state = std::string(m_level->m_levelName) 
				+ " by " + 
				(
					(isRobTopLevel) ? "RobTopGames" : std::string(m_level->m_creatorName)
				) +
				" (" + std::to_string(m_level->m_normalPercent.value()) + "%)";
		} else if (!shouldShowSensitive) {
			state = "Playtesting a created level";
		}

		std::string details = "Playing a";
		if (m_level->isPlatformer()) {
			details = details + " Platformer";
		}

		bool isDaily = m_level->m_dailyID.value() != 0;

		if (isDaily) {
			details = details + " daily";
		}

		details = details + " level";

		updateDiscordRP(
			details,
			state,
			getAssetKey(m_level),
			(isRated) ? "Rated" : "Not Rated",
			true,
			isInitial
		);
	}
};
