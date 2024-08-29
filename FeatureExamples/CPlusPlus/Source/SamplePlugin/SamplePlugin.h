#pragma once
#include <EngineCore/Core/Plugin.h>
#include <EngineCore/UI/UIView.h>
#include <EngineCore/UI/UIWindow.h>

namespace REngine {
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

    class SamplePlugin : public IEnginePlugin, public ISamplePlugin {
    public:
		SamplePlugin();
		~SamplePlugin(){}
		void OnSampleStart(SampleStartDesc* desc) override;
		void OnSampleCleanup() override;
	    void OnPluginLoad(Context* context) override;
	    void OnPluginUnload(Context* context) override;
	    void OnPluginSetup(Context* context) override;
	    void OnPluginStart(Context* context) override;
	    u32 GetPluginId() const override;
    private:
		ea::function<void()> back_2_selector_call_;
		WeakPtr<UIWindow> window_;
    };
}