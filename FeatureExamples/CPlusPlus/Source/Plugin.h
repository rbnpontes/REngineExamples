#pragma once
#include "Sample.h"
#include <EngineCore/Core/Plugin.h>
#include <EngineCore/UI/UIView.h>

struct SampleStartDesc
{
	Context* context;
	UIView* view;
	ea::function<void()> back_2_selector_call;
};

class ISamplePlugin
{
public:
	virtual void OnSampleStart(SampleStartDesc* desc) = 0;
	virtual void OnSampleCleanup() = 0;
};

class SamplePlugin : public REngine::IEnginePlugin, public ISamplePlugin {};

class Plugin : public Sample
{
	ATOMIC_OBJECT(Plugin, Sample)
public:
	Plugin(Context* context);

	void Start() override;
	void Cleanup() override;
private:
	REngine::IEnginePlugin* loaded_plugin_;
	ea::function<void()> back_2_selector_fn_;
};
