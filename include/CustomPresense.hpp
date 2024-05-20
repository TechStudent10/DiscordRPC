#pragma once

#include <discord_register.h>
#include <discord_rpc.h>
#include <string>
#include <set>
#include "Geode/modify/AppDelegate.hpp"
#include <Geode/Geode.hpp>
#include <Geode/loader/Dispatch.hpp>

#ifdef GEODE_IS_WINDOWS
    #ifdef _DRPC_EXPORTING
        #define DRPC_DLL __declspec(dllexport)
    #else
        #define DRPC_DLL __declspec(dllimport)
    #endif
#else
    #define DRPC_DLL
#endif

namespace gdrpc {
    class DRPC_DLL GDRPC {
    public:
        std::set<std::string> mods;
        GDRPC() {};
        void initDiscordRP();
        void updateDiscordRP(
            std::string modID,
            std::string details,
            std::string state = "",
            std::string smallImageKey = "",
            std::string smallImageText = "",
            bool useTime = false,
            bool shouldResetTime = false,
            std::string largeImage = "",
            int timeOffset = 0,
            std::string joinSecret = "",
            std::string largeImageText = "",
            int partyMax = 30
        );
        void registerMod(std::string modID) {
            mods.insert(modID);
        };
        static GDRPC*& getSharedInstance() {
            static GDRPC* instance = new GDRPC();
            return instance;
        };
    };

    inline bool defaultRPCEnabled = true;
} // namespace gdrpc

