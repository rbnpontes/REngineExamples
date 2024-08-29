#include "SamplePlugin.h"
#include <EngineCore/Math/StringHash.h>
#include <EngineCore/IO/Log.h>

#include <EngineCore/UI/UILayout.h>
#include <EngineCore/UI/UICheckBox.h>
#include <EngineCore/UI/UIButton.h>
#include <EngineCore/UI/UIEditField.h>
#include <EngineCore/UI/UIWindow.h>
#include <EngineCore/UI/UIEvents.h>

ENGINE_DEFINE_PLUGIN(REngine::SamplePlugin);
namespace REngine
{
	using namespace Atomic;
	static Atomic::StringHash s_plugin_id = "SamplePlugin";

	static UIWindow* sample_create_ui(Context* context, UIView* view)
	{
		UILayout* layout = new UILayout(context);
		layout->SetAxis(UI_AXIS_Y);

		UICheckBox* checkbox = new UICheckBox(context);
		checkbox->SetId("Checkbox");

		layout->AddChild(checkbox);

		UIButton* button = new UIButton(context);
		button->SetText("Button");
		button->SetId("Button");

		layout->AddChild(button);

		UIEditField* edit = new UIEditField(context);
		layout->AddChild(edit);
		edit->SetId("EditField");
		edit->SetText("Hello Plugin System!");

		UIWindow* window = new UIWindow(context);
		window->SetSettings(static_cast<UI_WINDOW_SETTINGS>(UI_WINDOW_SETTINGS_TITLEBAR | UI_WINDOW_SETTINGS_CLOSE_BUTTON));

		window->SetText("Hello from SamplePlugin!");

		window->AddChild(layout);

		window->ResizeToFitContent();

		view->AddChild(window);

		window->Center();

		return window;
	}

	SamplePlugin::SamplePlugin()
	= default;

	void SamplePlugin::OnPluginLoad(Context* context)
	{
		ATOMIC_LOGDEBUG("Loaded SamplePlugin");
	}

	void SamplePlugin::OnPluginUnload(Context* context)
	{
		ATOMIC_LOGDEBUG("Unloaded SamplePlugin");
		window_ = nullptr;
	}

	void SamplePlugin::OnPluginSetup(Context* context)
	{
	}

	void SamplePlugin::OnPluginStart(Context* context)
	{
	}

	void SamplePlugin::OnSampleStart(SampleStartDesc* desc)
	{
		back_2_selector_call_ = desc->back_2_selector_call;
		window_ = sample_create_ui(desc->context, desc->view);

		window_->SubscribeToEvent(E_WIDGETEVENT, [&](StringHash event_type, VariantMap& event_data)
		{
			using namespace WidgetEvent;
			if (event_data[P_TYPE] != UI_EVENT_TYPE_CLICK)
				return;

			UIWidget* widget = static_cast<UIWidget*>(event_data[P_TARGET].GetPtr());
			if (widget)
				window_->SetText(ToString("Hello: %s", widget->GetId().CString()));
		});
		window_->SubscribeToEvent(E_WIDGETDELETED, [&](StringHash event_type, VariantMap& event_data)
		{
			const auto back_2_selector = back_2_selector_call_;
			back_2_selector();
		});
	}

	void SamplePlugin::OnSampleCleanup()
	{
		if (!window_.Expired())
			window_->UnsubscribeFromAllEvents();
		window_ = nullptr;
	}


	u32 SamplePlugin::GetPluginId() const
	{
		return s_plugin_id.Value();
	}
}
