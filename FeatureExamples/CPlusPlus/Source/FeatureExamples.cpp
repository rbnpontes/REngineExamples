//
// Copyright (c) 2008-2016 the Urho3D project.
// Copyright (c) 2014-2016, THUNDERBEAST GAMES LLC All rights reserved
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include <EngineCore/Engine/Application.h>
#include <EngineCore/Graphics/Camera.h>
#include <EngineCore/Engine/Engine.h>
#include <EngineCore/IO/FileSystem.h>
#include <EngineCore/Graphics/Graphics.h>
#include <EngineCore/Input/Input.h>
#include <EngineCore/Input/InputEvents.h>
#include <EngineCore/Graphics/Renderer.h>
#include <EngineCore/Resource/ResourceCache.h>
#include <EngineCore/Scene/Scene.h>
#include <EngineCore/Scene/SceneEvents.h>
#include <EngineCore/Graphics/Texture2D.h>
#include <EngineCore/Core/Timer.h>
#include <EngineCore/UI/UI.h>
#include <EngineCore/UI/UIView.h>
#include <EngineCore/Resource/XMLFile.h>
#include <EngineCore/IO/Log.h>
#include <EngineCore/Engine/EngineDefs.h>

#include "FeatureExamples.h"
#include "SampleSelector.h"

ATOMIC_DEFINE_APPLICATION_MAIN(FeatureExamples)

WeakPtr<UIView> FeatureExamples::uiView_;

FeatureExamples::FeatureExamples(Context* context) :
    Application(context)

{
}

void FeatureExamples::Setup()
{
    // Modify engine startup parameters
    engineParameters_[EP_WINDOW_TITLE]   = GetTypeName();
    engineParameters_[EP_WINDOW_WIDTH]   = 1440;
    engineParameters_[EP_WINDOW_HEIGHT]  = 960;
    engineParameters_[EP_LOG_NAME]       = GetSubsystem<FileSystem>()->GetAppPreferencesDir("atomic", "logs") + GetTypeName() + ".log";
    engineParameters_[EP_FULL_SCREEN]    = false;
    engineParameters_[EP_WINDOW_RESIZABLE] = true;
    engineParameters_[EP_HEADLESS ]      = false;
    engineParameters_[EP_SOUND]          = false;
    engineParameters_[EP_HIGH_DPI]       = true;
    engineParameters_[EP_RESOURCE_PATHS] = "Data;PlayerData;CoreData";

    // Construct a search path to find the resource prefix with two entries:
    // The first entry is an empty path which will be substituted with program/bin directory -- this entry is for binary when it is still in build tree
    // The second and third entries are possible relative paths from the installed program/bin directory to the asset directory -- these entries are for binary when it is in the Atomic SDK installation location
    if (!engineParameters_.Contains(EP_RESOURCE_PREFIX_PATHS))
    {
        // TODO: This is dependent on a source build
#if ENGINE_PLATFORM_IOS
        String resourcePrefix = ToString(";../Resources;../.."); 
#elif ENGINE_PLATFORM_ANDROID
        String resourcePrefix = ";Resources";
#else
        String resourcePrefix = ToString("%s/Resources;%s/Submodules/EngineExamples/FeatureExamples/CPlusPlus", ENGINE_ROOT_SOURCE_DIR, ENGINE_ROOT_SOURCE_DIR);
#endif
        engineParameters_[EP_RESOURCE_PREFIX_PATHS] = resourcePrefix;
    }

}

void FeatureExamples::Start()
{
    UI* ui = GetSubsystem<UI>();
    ui->Initialize("DefaultUI/language/lng_en.tb.txt");
    ui->LoadDefaultPlayerSkin();

    // create new UIView
    uiView_ = new UIView(context_);

    // Set custom window Title & Icon
    SetWindowTitleAndIcon();

    new SampleSelector(context_);

    SubscribeToEvent(E_KEYDOWN, ATOMIC_HANDLER(FeatureExamples, HandleKeyDown));

}

void FeatureExamples::Stop()
{
    engine_->DumpResources(true);    
}

void FeatureExamples::HandleKeyDown(StringHash eventType, VariantMap& eventData)
{
    using namespace KeyDown;

    int key = eventData[P_KEY].GetInt();

    // Toggle console with F1
    if (key == KEY_F1)
    {
        GetSubsystem<UI>()->ToggleConsole();
    }
    // Toggle debug HUD with F2
    else if (key == KEY_F2)
    {
        GetSubsystem<UI>()->ToggleDebugHud();
    }
}

void FeatureExamples::SetWindowTitleAndIcon()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Graphics* graphics = GetSubsystem<Graphics>();
    Image* icon = cache->GetResource<Image>("Textures/AtomicIcon48.png");
    graphics->SetWindowIcon(icon);
    graphics->SetWindowTitle(ToString("%s Sample", ENGINE_NAME));
}


