/*************************************************************************
> File Name: App.cpp
> Project Name: CubbyFlow
> This code is based on Jet Framework that was created by Doyub Kim.
> References: https://github.com/doyubkim/fluid-engine-dev
> Purpose: The application class for representating application side cpu tasks.
> Created Time: 2020/07/26
> Copyright (c) 2020, Ji-Hong snowapril
*************************************************************************/
#include <Vox/App.hpp>
#include <Vox/FrameContext.hpp>
#include <Vox/PerspectiveCamera.hpp>
#include <Vox/VoxScene.hpp>
#include <glad/glad.h>

using namespace CubbyFlow;

namespace Vox {

    App::App()
    {
        //! Do nothing
    }

    App::~App()
    {
        //! Remove All Contained Contexts 
        while (!_ctxQueue.empty())
        {
            auto& ctx = _ctxQueue.front();
            ctx.reset();
            _ctxQueue.pop();
        }
    }

    CubbyFlow::Vector2I App::GetWindowSize()
    {
        return _windowSize;
    }
    
    bool App::Initialize(const Vox::Path& scenePath)
    {
        _scene = Vox::VoxScene::CreateScene(scenePath);
        _camera = _scene->GetSceneObject<Vox::PerspectiveCamera>("Camera");
        _camera->UpdateMatrix();

        return true;
    }

    void App::BeginFrame(std::shared_ptr<FrameContext>& ctx)
    {
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(_bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w);
    }

    void App::EndFrame(std::shared_ptr<FrameContext>& ctx)
    {
        //! On screen draw stuffs like GUI
    }

    void App::SetWindowSize(Vector2I size)
    {
        _windowSize = size;
        OnSetWindowSize();
    }

    void App::SetBackgroundColor(Vector4F color)  
    {
        _bgColor = color;
    }
};
