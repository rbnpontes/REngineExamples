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
#include <EngineCore/UI/UI.h>
#include <EngineCore/Scene/Scene.h>
#include <EngineCore/Scene/SceneEvents.h>
#include <EngineCore/2D/CollisionBox2D.h>
#include <EngineCore/2D/CollisionEdge2D.h>
#include <EngineCore/2D/ConstraintRevolute2D.h>
#include <EngineCore/2D/ConstraintRope2D.h>
#include <EngineCore/2D/PhysicsWorld2D.h>
#include <EngineCore/2D/RigidBody2D.h>

#include "PhysicsRope2D.h"

#include <EngineCore/DebugNew.h>

static const unsigned NUM_OBJECTS = 10;

PhysicsRope2D::PhysicsRope2D(Context* context) :
    Sample(context)
{
}

void PhysicsRope2D::Start()
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

void PhysicsRope2D::CreateScene()
{
    scene_ = new Scene(context_);
    scene_->CreateComponent<Octree>();
    scene_->CreateComponent<DebugRenderer>();
    // Create camera node
    cameraNode_ = scene_->CreateChild("Camera");
    // Set camera's position
    cameraNode_->SetPosition(Vector3(0.0f, 5.0f, -10.0f));

    Camera* camera = cameraNode_->CreateComponent<Camera>();
    camera->SetOrthographic(true);

    Graphics* graphics = GetSubsystem<Graphics>();
    camera->SetOrthoSize((float)graphics->GetHeight() * 0.05f);
    camera->SetZoom(1.5f * Min((float)graphics->GetWidth() / 1280.0f, (float)graphics->GetHeight() / 800.0f)); // Set zoom according to user's resolution to ensure full visibility (initial zoom (1.5) is set for full visibility at 1280x800 resolution)

    // Create 2D physics world component
    PhysicsWorld2D* physicsWorld = scene_->CreateComponent<PhysicsWorld2D>();
    physicsWorld->SetDrawJoint(true);

    // Create ground
    Node* groundNode = scene_->CreateChild("Ground");
    // Create 2D rigid body for gound
    RigidBody2D* groundBody = groundNode->CreateComponent<RigidBody2D>();
    // Create edge collider for ground
    CollisionEdge2D* groundShape = groundNode->CreateComponent<CollisionEdge2D>();
    groundShape->SetVertices(Vector2(-40.0f, 0.0f), Vector2(40.0f, 0.0f));

    const float y = 15.0f;
    RigidBody2D* prevBody = groundBody;

    for (unsigned i = 0; i < NUM_OBJECTS; ++i)
    {
        Node* node  = scene_->CreateChild("RigidBody");

        // Create rigid body
        RigidBody2D* body = node->CreateComponent<RigidBody2D>();
        body->SetBodyType(BT_DYNAMIC);

        // Create box
        CollisionBox2D* box = node->CreateComponent<CollisionBox2D>();
        // Set friction
        box->SetFriction(0.2f);
        // Set mask bits.
        box->SetMaskBits(0xFFFF & ~0x0002);

        if (i == NUM_OBJECTS - 1)
        {
            node->SetPosition(Vector3(1.0f * i, y, 0.0f));
            body->SetAngularDamping(0.4f);
            box->SetSize(3.0f, 3.0f);
            box->SetDensity(100.0f);
            box->SetCategoryBits(0x0002);
        }
        else
        {
            node->SetPosition(Vector3(0.5f + 1.0f * i, y, 0.0f));
            box->SetSize(1.0f, 0.25f);
            box->SetDensity(20.0f);
            box->SetCategoryBits(0x0001);
        }

        ConstraintRevolute2D* joint = node->CreateComponent<ConstraintRevolute2D>();
        joint->SetOtherBody(prevBody);
        joint->SetAnchor(Vector2(float(i), y));
        joint->SetCollideConnected(false);

        prevBody = body;
    }

    ConstraintRope2D* constraintRope = groundNode->CreateComponent<ConstraintRope2D>();
    constraintRope->SetOtherBody(prevBody);
    constraintRope->SetOwnerBodyAnchor(Vector2(0.0f, y));
    constraintRope->SetMaxLength(NUM_OBJECTS - 1.0f + 0.01f);
}

void PhysicsRope2D::CreateInstructions()
{
    SimpleCreateInstructions("Use WASD keys and mouse/touch to move, Use PageUp PageDown to zoom.");
}

void PhysicsRope2D::SetupViewport()
{
    Renderer* renderer = GetSubsystem<Renderer>();

    // Set up a viewport to the Renderer subsystem so that the 3D scene can be seen
    SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
    renderer->SetViewport(0, viewport);
}

void PhysicsRope2D::MoveCamera(float timeStep)
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

void PhysicsRope2D::SubscribeToEvents()
{
    // Subscribe HandleUpdate() function for processing update events
    SubscribeToEvent(E_UPDATE, ATOMIC_HANDLER(PhysicsRope2D, HandleUpdate));

    // Unsubscribe the SceneUpdate event from base class to prevent camera pitch and yaw in 2D sample
    UnsubscribeFromEvent(E_SCENEUPDATE);
}

void PhysicsRope2D::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace Update;

    // Take the frame time step, which is stored as a float
    float timeStep = eventData[P_TIMESTEP].GetFloat();

    // Move the camera, scale movement with time step
    MoveCamera(timeStep);

    PhysicsWorld2D* physicsWorld = scene_->GetComponent<PhysicsWorld2D>();
    physicsWorld->DrawDebugGeometry();
}
