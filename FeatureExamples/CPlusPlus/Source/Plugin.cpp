#include "Plugin.h"
#include <EngineCore/Core/PluginSystem.h>
#include <EngineCore/Scene/Scene.h>
#include "FeatureExamples.h"

Plugin::Plugin(Context* context) : Sample(context),
	loaded_plugin_(nullptr)
{
	back_2_selector_fn_ = [&]()
	{
		BackToSelector();
	};
}

void Plugin::Start()
{
	Sample::Start();

	loaded_plugin_ = GetSubsystem<REngine::PluginSystem>()->LoadPlugin("SamplePlugin");
	if (!loaded_plugin_)
		return;

	const auto sample_plugin = reinterpret_cast<SamplePlugin*>(loaded_plugin_);
	SampleStartDesc desc{ context_, FeatureExamples::GetUIView(), back_2_selector_fn_ };
	if(sample_plugin)
		sample_plugin->OnSampleStart(&desc);
}

void Plugin::Cleanup()
{
	if (loaded_plugin_)
	{
		const auto sample_plugin = reinterpret_cast<SamplePlugin*>(loaded_plugin_);
		if (sample_plugin)
			sample_plugin->OnSampleCleanup();
		GetSubsystem<REngine::PluginSystem>()->UnloadPlugin(loaded_plugin_->GetPluginId());
	}
	loaded_plugin_ = nullptr;
}
