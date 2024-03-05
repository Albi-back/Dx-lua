//
// Game.h
//

#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"
#include <CommonStates.h>
#include "AnimatedTexture.h"
#include "ScrollingBackground.h"
#include "LuaHelper.h"
using namespace std;

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
public:
    
    
    Game() noexcept(false);
    ~Game() = default;

    Game(Game&&) = default;
    Game& operator= (Game&&) = default;

    Game(Game const&) = delete;
    Game& operator= (Game const&) = delete;

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    void OnDeviceLost() override;
    void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnDisplayChange();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize( int& width, int& height ) const noexcept;

private:

    void Update(DX::StepTimer const& timer);
    void Render();

    void Clear();

    void CreateDeviceDependentResources(lua_State*);
    void CreateWindowSizeDependentResources();
    void shoot();
    // Device resources.
    std::unique_ptr<DX::DeviceResources>    m_deviceResources;

    // Rendering loop timer.
    DX::StepTimer                           m_timer;
    
    std::unique_ptr<DirectX::GraphicsMemory> m_graphicsMemory;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
    unique_ptr < DirectX::SpriteBatch> m_spriteBatch;
    DirectX::SimpleMath::Vector2 m_screenPos;
    DirectX::SimpleMath::Vector2 m_origin;
    std::unique_ptr<DirectX::CommonStates> m_states;
    RECT m_tileRect;
    RECT m_fullscreenRect;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>m_background;
    unique_ptr<AnimatedTexture> m_ship;
    DirectX::SimpleMath::Vector2 m_shipPos;
    std::unique_ptr<DirectX::Keyboard> m_keyboard;
    std::unique_ptr<DirectX::Mouse> m_mouse;
    unique_ptr<ScrollingBackground> m_stars;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_bulletTex;
    unique_ptr<AnimatedTexture> m_bullet;
    

    
    

};
