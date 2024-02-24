#pragma once

#include <discord_register.h>
#include <discord_rpc.h>

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
        GDRPC() {};
        void initDiscordRP();
        void updateDiscordRP(
            std::string details,
            std::string state = "",
            std::string smallImageKey = "",
            std::string smallImageText = "",
            bool useTime = false,
            bool shouldResetTime = false,
            std::string largeImage = ""
        );
    };
} // namespace gdrpc

