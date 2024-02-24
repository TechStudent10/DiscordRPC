# Discord Rich Presence

## For Players

Read the [about.md](/about.md) for more info

## For Developers

DiscordRPC gives mod developers an API that's a simple bridge to Discord's API.

```cpp
void gdrpc::GDRPC::initDiscordRP()
```

This function initializes the RPC. This should **not** be called by an external mod as this mod already does so for you.

```cpp
void gdrpc::GDRPC::updateDiscordRP(std::string details, std::string state, std::string smallImageKey, std::string smallImageText, bool useTime, bool shouldResetTime, std::string largeImage)
```

This massive jumble of spaghetti is what you're going to be using to update the Rich Presence. Every argument after `details` is optional.

| `argument name`   | argument description                                                                                             |
|-------------------|------------------------------------------------------------------------------------------------------------------|
| `applicationId`   | The ID thats assigned to your application in your Discord Application (in your Discord Developer Portal)         |
| `details`         | Text right below the game name (Geometry Dash)                                                                   |
| `state`           | Additional information below the `details`                                                                       |
| `smallImageKey`   | The key (name) for the small image                                                                               |
| `smallImageText`  | Text that appears when hovering over the small image                                                             |
| `useTime`         | Whether to show the time since the last time reset                                                               |
| `shouldResetTime` | Whether to reset the time                                                                                        |
| `largeImage`      | The key (name) for the large image (typically left as `gd_large`)                                                |
