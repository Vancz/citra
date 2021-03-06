// Copyright 2018 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include <atomic>
#include <memory>
#include <thread>
#include "common/threadsafe_queue.h"
#include "input_common/sdl/sdl.h"

union SDL_Event;
using SDL_Joystick = struct _SDL_Joystick;
using SDL_JoystickID = s32;

namespace InputCommon::SDL {

class SDLJoystick;
class SDLButtonFactory;
class SDLAnalogFactory;

class SDLState : public State {
public:
    /// Initializes and registers SDL device factories
    SDLState();

    /// Unresisters SDL device factories and shut them down.
    ~SDLState() override;

    /// Handle SDL_Events for joysticks from SDL_PollEvent
    void HandleGameControllerEvent(const SDL_Event& event);

    std::shared_ptr<SDLJoystick> GetSDLJoystickBySDLID(SDL_JoystickID sdl_id);
    std::shared_ptr<SDLJoystick> GetSDLJoystickByGUID(const std::string& guid, int port);

    /// Get all DevicePoller that use the SDL backend for a specific device type
    void GetPollers(
        InputCommon::Polling::DeviceType type,
        std::vector<std::unique_ptr<InputCommon::Polling::DevicePoller>>& pollers) override;

    /// Used by the Pollers during config
    std::atomic<bool> polling = false;
    Common::SPSCQueue<SDL_Event> event_queue;

private:
    void InitJoystick(int joystick_index);
    void CloseJoystick(SDL_Joystick* sdl_joystick);

    /// Needs to be called before SDL_QuitSubSystem.
    void CloseJoysticks();

    /// Map of GUID of a list of corresponding virtual Joysticks
    std::unordered_map<std::string, std::vector<std::shared_ptr<SDLJoystick>>> joystick_map;
    std::mutex joystick_map_mutex;

    std::shared_ptr<SDLButtonFactory> button_factory;
    std::shared_ptr<SDLAnalogFactory> analog_factory;

    bool start_thread = false;
    std::atomic<bool> initialized = false;

    std::thread poll_thread;
};
} // namespace InputCommon::SDL
