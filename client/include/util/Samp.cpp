/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "Samp.h"

#include <samp/CChat.h>
#include <samp/CInput.h>
#include <samp/CGameSA.h>
#include <samp/CScoreboard.h>

#include "Logger.h"

bool Samp::Init(const AddressesBase& addr_base) noexcept
{
    if (_init_status) return false;

    Logger::LogToFile("[dbg:samp:init] : module initializing...");

    _hook_samp_free = Memory::JumpHook((LPVOID)(addr_base.GetSampDestructAddr()), &HookSampFree);
    _hook_samp_init = Memory::JumpHook((LPVOID)(addr_base.GetSampInitAddr()), &HookSampInit);

    SAMP::InitSamp(addr_base.GetBaseAddr());

    _load_callback = nullptr;
    _exit_callback = nullptr;

    _load_status = false;

    Logger::LogToFile("[dbg:samp:init] : module initialized");

    _init_status = true;

    return true;
}

bool Samp::IsInited() noexcept
{
    return _init_status;
}

bool Samp::IsLoaded() noexcept
{
    return _load_status;
}

void Samp::Free() noexcept
{
    if (_init_status)
    {
        Logger::LogToFile("[dbg:samp:free] : module releasing...");

        _hook_samp_init = {};
        _hook_samp_free = {};

        if (_load_status)
        {
            if (_exit_callback != nullptr) _exit_callback();
        }

        _load_status = false;

        _load_callback = nullptr;
        _exit_callback = nullptr;

        Logger::LogToFile("[dbg:samp:free] : module released");

        _init_status = false;
    }
}

void Samp::AddClientCommand(const char* const name, const SAMP::CMDPROC handler) noexcept
{
    constexpr int    kMaxCommands      = MAX_CLIENT_CMDS - 1;
    constexpr size_t kMaxCommandLength = 30;

    if (name != nullptr && *name != '\0' && handler != nullptr)
    {
        if (_load_status)
        {
            if (const auto input_box = SAMP::pInputBox(); input_box != nullptr &&
                input_box->m_nCommandCount < kMaxCommands && std::strlen(name) <= kMaxCommandLength)
            {
                Logger::LogToFile("[dbg:samp:addclientcommand] : command '%s' adding...", name);
                input_box->AddCommand(name, handler);
            }
        }
    }
}

void Samp::AddMessageToChat(const D3DCOLOR color, const char* const message) noexcept
{
    if (message != nullptr && *message != '\0')
    {
        if (_load_status)
        {
            if (const auto chat = SAMP::pChat(); chat != nullptr)
            {
                chat->AddEntry(SAMP::ChatEntry::CHAT_TYPE_DEBUG, message, nullptr, color, NULL);
            }
        }
    }
}

void Samp::ToggleSampCursor(const int mode) noexcept
{
    if (_load_status)
    {
        if (const auto input_box = SAMP::pInputBox(); input_box != nullptr &&
            input_box->m_bEnabled == FALSE)
        {
            if (const auto scoreboard = SAMP::pScoreboard(); scoreboard != nullptr &&
                scoreboard->m_bIsEnabled == FALSE)
            {
                if (const auto game = SAMP::pGame(); game != nullptr && game->IsMenuVisible() == FALSE)
                {
                    game->SetCursorMode(mode, mode == 0 ? TRUE : FALSE);
                    if (mode == 0) game->ProcessInputEnabling();
                }
            }
        }
    }
}

void Samp::SetLoadCallback(LoadCallback&& callback) noexcept
{
    if (_init_status) _load_callback = std::move(callback);
}

void Samp::SetExitCallback(ExitCallback&& callback) noexcept
{
    if (_init_status) _exit_callback = std::move(callback);
}

void __declspec(naked) Samp::HookSampInit() noexcept
{
    static LPVOID ret_addr = nullptr;

    __asm
    {
        pushad
        mov ebp, esp
        sub esp, __LOCAL_SIZE
    }

    Logger::LogToFile("[dbg:samp:load] : module loading...");

    ret_addr = _hook_samp_init->GetPatch().GetAddr();
    _hook_samp_init = {};

    if (_load_callback != nullptr) _load_callback();

    _load_status = true;

    Logger::LogToFile("[dbg:samp:load] : module loaded");

    __asm
    {
        mov esp, ebp
        popad
        jmp ret_addr
    }
}

void __declspec(naked) Samp::HookSampFree() noexcept
{
    static LPVOID ret_addr = nullptr;

    __asm
    {
        pushad
        mov ebp, esp
        sub esp, __LOCAL_SIZE
    }

    ret_addr = _hook_samp_free->GetPatch().GetAddr();
    _hook_samp_free = {};

    Free();

    __asm
    {
        mov esp, ebp
        popad
        jmp ret_addr
    }
}

bool Samp::_init_status = false;
bool Samp::_load_status = false;

Samp::LoadCallback Samp::_load_callback = nullptr;
Samp::ExitCallback Samp::_exit_callback = nullptr;

Memory::JumpHook Samp::_hook_samp_init;
Memory::JumpHook Samp::_hook_samp_free;
