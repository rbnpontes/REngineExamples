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

#include <EngineCore/Core/CoreEvents.h>
#include <EngineCore/Engine/Engine.h>
#include <EngineCore/Graphics/Camera.h>
#include <EngineCore/Graphics/DebugRenderer.h>
#include <EngineCore/Graphics/Graphics.h>
#include <EngineCore/Graphics/Octree.h>
#include <EngineCore/Graphics/Renderer.h>
#include <EngineCore/Input/Input.h>
#include <EngineCore/Resource/ResourceCache.h>
#include <EngineCore/Scene/Scene.h>
#include <EngineCore/Scene/SceneEvents.h>
#include <EngineCore/2D/CollisionBox2D.h>
#include <EngineCore/2D/CollisionCircle2D.h>
#include <EngineCore/2D/Drawable2D.h>
#include <EngineCore/2D/PhysicsWorld2D.h>
#include <EngineCore/2D/RigidBody2D.h>
#include <EngineCore/2D/Sprite2D.h>
#include <EngineCore/2D/StaticSprite2D.h>

#include "Physics2D.h"

#include <EngineCore/DebugNew.h>


static const unsigned NUM_OBJECTS = 100;

Physics2D::Physics2D(Context* context) :
    Sample(context)
{
}

void Physics2D::Start()
{
    // Execute base class startup
    Sample::Start();

    // Create the scene content
    CreateScene();

    // Create the UI content
    CreateInstructions();

    // Setup the viewport for displaying the scene
    SetupViewport();

    // Hook up to the frame update events
    SubscribeToEvents();

    // Set the mouse mode to use in the sample
    Sample::InitMouseMode(MM_FREE);
}

void Physics2D::CreateScene()
{
    scene_ = new Scene(context_);
    scene_->CreateComponent<Octree>();
    scene_->CreateComponent<DebugRenderer>();
    // Create camera node
    cameraNode_ = scene_->CreateChild("Camera");
    // Set camera's position
    cameraNode_->SetPosition(Vector3(0.0f, 0.0f, -10.0f));

    Camera* camera = cameraNode_->CreateComponent<Camera>();
    camera->SetOrthographic(true);

    Graphics* graphics = GetSubsystem<Graphics>();
    camera->SetOrthoSize((float)graphics->GetHeight() * PIXEL_SIZE);
    camera->SetZoom(1.2f * Min((float)graphics->GetWidth() / 1280.0f, (float)graphics->GetHeight() / 800.0f)); // Set zoom according to user's resolution to ensure full visibility (initial zoom (1.2) is set for full visibility at 1280x800 resolution)

    // Create 2D physics world component
    /*PhysicsWorld2D* physicsWorld = */scene_->CreateComponent<PhysicsWorld2D>();

    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Sprite2D* boxSprite = cache->GetResource<Sprite2D>("Urho2D/Box.png");
    Sprite2D* ballSprite = cache->GetResource<Sprite2D>("Urho2D/Ball.png");

    // Create ground.
    Node* groundNode = scene_->CreateChild("Ground");
    groundNode->SetPosition(Vector3(0.0f, -3.0f, 0.0f));
    groundNode->SetScale(Vector3(200.0f, 1.0f, 0.0f));

    // Create 2D rigid body for gound
    /*RigidBody2D* groundBody = */groundNode->CreateComponent<RigidBody2D>();

    StaticSprite2D* groundSprite = groundNode->CreateComponent<StaticSprite2D>();
    groundSprite->SetSprite(boxSprite);

    // Create box collider for ground
    CollisionBox2D* groundShape = groundNode->CreateComponent<CollisionBox2D>();
    // Set box size
    groundShape->SetSize(Vector2(0.32f, 0.32f));
    // Set friction
    groundShape->SetFriction(0.5f);

    for (unsigned i = 0; i < NUM_OBJECTS; ++i)
    {
        Node* node  = scene_->CreateChild("RigidBody");
        node->SetPosition(Vector3(Random(-0.1f, 0.1f), 5.0f + i * 0.4f, 0.0f));

        // Create rigid body
        RigidBody2D* body = node->CreateComponent<RigidBody2D>();
        body->SetBodyType(BT_DYNAMIC);

        StaticSprite2D* staticSprite = node->CreateComponent<StaticSprite2D>();

        if (i % 2 == 0)
        {
            staticSprite->SetSprite(boxSprite);

            // Create box
            CollisionBox2D* box = node->CreateComponent<CollisionBox2D>();
            // Set size
            box->SetSize(Vector2(0.32f, 0.32f));
            // Set density
            box->SetDensity(1.0f);
            // Set friction
            box->SetFriction(0.5f);
            // Set restitution
            box->SetRestitution(0.1f);
        }
        else
        {
            staticSprite->SetSprite(ballSprite);

            // Create circle
            CollisionCircle2D* circle = node->CreateComponent<CollisionCircle2D>();
            // Set radius
            circle->SetRadius(0.16f);
            // Set density
            circle->SetDensity(1.0f);
            // Set friction.
            circle->SetFriction(0.5f);
            // Set restitution
            circle->SetRestitution(0.1f);
        }
    }
}

void Physics2D::CreateInstructions()
{
    SimpleCreateInstructions("Use WASD keys to move, use PageUp PageDown keys to zoom.");
}

void Physics2D::SetupViewport()
{
    Renderer* renderer = GetSubsystem<Renderer>();

    // Set up a viewport to the Renderer subsystem so that the 3D scene can be seen
    SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
    renderer->SetViewport(0, viewport);
}

void Physics2D::MoveCamera(float timeStep)
{
    Input* input = GetSubsystem<Input>();

    // Movement speed as world units per second
    const float MOVE_SPEED = 4.0f;

    // Read WASD keys and move the camera scene node to the corresponding direction if they are pressed
    if (input->GetKeyDown(KEY_W))
        cameraNode_->Translate(Vector3::UP * MOVE_SPEED * timeStep);
    if (input->GetKeyDown(KEY_S))
        cameraNode_->Translate(Vector3::DOWN * MOVE_SPEED * timeStep);
    if (input->GetKeyDown(KEY_A))
        cameraNode_->Translate(Vector3::LEFT * MOVE_SPEED * timeStep);
    if (input->GetKeyDown(KEY_D))
        cameraNode_->Translate(Vector3::RIGHT * MOVE_SPEED * timeStep);

    if (input->GetKeyDown(KEY_PAGEUP))
    {
        Camera* camera = cameraNode_->GetComponent<Camera>();
        camera->SetZoom(camera->GetZoom() * 1.01f);
    }

    if (input->GetKeyDown(KEY_PAGEDOWN))
    {
        Camera* camera = cameraNode_->GetComponent<Camera>();
        camera->SetZoom(camera->GetZoom() * 0.99f);
    }
}

void Physics2D::SubscribeToEvents()
{
    // Subscribe HandleUpdate() function for processing update events
    SubscribeToEvent(E_UPDATE, ATOMIC_HANDLER(Physics2D, HandleUpdate));

    // Unsubscribe the SceneUpdate event from base class to prevent camera pitch and yaw in 2D sample
    UnsubscribeFromEvent(E_SCENEUPDATE);
}

void Physics2D::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace Update;

    // Take the frame time step, which is stored as a float
    float timeStep = eventData[P_TIMESTEP].GetFloat();

    // Move the camera, scale movement with time step
    MoveCamera(timeStep);
}
