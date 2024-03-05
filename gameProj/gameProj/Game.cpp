//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include <SpriteBatch.h>
#include <CommonStates.h>
#include "AnimatedTexture.h"

extern void ExitGame() noexcept;

using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace std;

using Microsoft::WRL::ComPtr;
namespace {
    const Vector2 Bounds = { 800,600};
    
}
Game::Game() noexcept(false)
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    // TODO: Provide parameters for swapchain format, depth/stencil format, and backbuffer count.
    //   Add DX::DeviceResources::c_AllowTearing to opt-in to variable rate displays.
    //   Add DX::DeviceResources::c_EnableHDR for HDR10 display.
    m_deviceResources->RegisterDeviceNotify(this);
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();
    m_keyboard = make_unique<Keyboard>();
    m_mouse = make_unique<Mouse>();
    m_mouse->SetWindow(window);
   

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    float elapsedTime = float(timer.GetElapsedSeconds());
    m_ship->Update(elapsedTime);
    m_stars->Update(elapsedTime * 500);
    // TODO: Add your game logic here.
    auto kb = m_keyboard->GetState();
    Vector2 move = Vector2::Zero;
    if (kb.W)
    {
        move.y -= 2.f;
    }
    if (kb.D)
    {
        move.x += 2.f;
    }
    if (kb.A)
    {
        move.x -= 2.f;
    }
    if (kb.S)
    {
        move.y += 2.f;
    }
        
    
    if (kb.Escape)
    {
        ExitGame();
    }
    Quaternion q = Quaternion::CreateFromYawPitchRoll(0.f, 0.f, 0.f);

    move = Vector2::Transform(move,q);
    m_shipPos += move;
    m_shipPos = Vector2::Min(m_shipPos,(Bounds*.75));
    m_shipPos = Vector2::Max(m_shipPos, (Bounds *.17));
    auto mouse = m_mouse->GetState();
    elapsedTime;
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();
    float time = float(m_timer.GetTotalSeconds());
    // TODO: Add your rendering code here.
    m_spriteBatch-> Begin(SpriteSortMode_Deferred,nullptr, m_states-> LinearWrap());
    m_spriteBatch->Draw(m_background.Get(), m_fullscreenRect);
    m_stars->Draw(m_spriteBatch.get());
    m_ship->Draw(m_spriteBatch.get(), m_shipPos);
    
    m_spriteBatch->End();

    context;

    m_deviceResources->PIXEndEvent();

    // Show the new frame.
    m_deviceResources->Present();
    m_graphicsMemory->Commit();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto const viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowMoved()
{
    auto const r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnDisplayChange()
{
    m_deviceResources->UpdateColorSpace();
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();

    // TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    width = 800;
    height = 600;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();
    m_graphicsMemory = make_unique<GraphicsMemory>(device);

    auto context = m_deviceResources->GetD3DDeviceContext();
    m_spriteBatch = make_unique<SpriteBatch>(context);

    unique_ptr<CommonStates> states;
    states = make_unique<CommonStates>(device);

    // TODO: Initialize device dependent objects here (independent of window size).
    ComPtr<ID3D11Resource> resource;
   

    DX::ThrowIfFailed(
        CreateWICTextureFromFile(device, L"starfield.png", nullptr,
            m_background.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(
        CreateWICTextureFromFile(device, L"shipanimated.png",
        nullptr, m_texture.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(
        CreateWICTextureFromFile(device, L"Bullet.png",
            nullptr, m_bulletTex.ReleaseAndGetAddressOf()));
    
    

    m_ship = make_unique<AnimatedTexture>();
    m_ship->Load(m_texture.Get(), 4, 20);
    m_states = make_unique<CommonStates>(device);
    m_stars = make_unique<ScrollingBackground>();
    m_stars->Load(m_background.Get());
    m_bullet = make_unique<AnimatedTexture>();
    m_bullet->Load(m_texture.Get(), 4, 20);
    
    
    

    device;
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    // TODO: Initialize windows-size dependent objects here.
    auto size = m_deviceResources->GetOutputSize();
    m_fullscreenRect = m_deviceResources->GetOutputSize();
    m_screenPos.x = float(size.right) / 2.f;
    m_screenPos.y = float(size.bottom) / 2.f; 
    m_shipPos.x = float(size.right / 2);
    m_shipPos.y = float((size.bottom / 2) + (size.bottom / 4));
    m_stars->SetWindow(size.right, size.bottom);
}

void Game::OnDeviceLost()
{
    m_ship.reset();
    m_texture.Reset();
    m_graphicsMemory.reset();
    m_spriteBatch.reset();
    m_states.reset();
    m_background.Reset();
    m_stars.reset();
    // TODO: Add Direct3D resource cleanup here.
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
void shoot()
{

}
#pragma endregion
