#pragma once

// Interface
#include "Interface/ISwitchable.h"

#include "Types/Types.h"
#include "EditorWindow.h"
#include <memory>
#include "../../ImGui/imgui_impl_dx11.h"

class UEditorDesigner
{
public:
	static UEditorDesigner& Get()
	{
		static UEditorDesigner Instance;
		return Instance;
	}

	void AddWindow(const FString& WindowId, std::shared_ptr<UEditorWindow> Window)
	{
		Windows[WindowId] = Window;
	}
	
	/**
	    캐스팅을 통해 Window를 가져옵니다.
		이후 setter 를 통해 값을 변경할 수 있습니다.
		auto window = UEditorDesigner::Get().GetWindow("mainWindow");
		if (window) {
			// dynamic_cast를 통해 MyWindow 타입으로 변환 후 setter 호출
			if (MyWindow* mw = dynamic_cast<MyWindow*>(window.get())) {
				mw->SetValue(42);
			}
		}
	
	*/
	// 현재 관리되고 있는 윈도우 창을 가져옵니다.
	std::shared_ptr<UEditorWindow> GetWindow(const FString WindowId)
	{
		auto it = Windows.find(WindowId);
		if (it != Windows.end())
		{
			return it->second;
		}
		return nullptr;
	}

	void OnResize(UINT32 Width, UINT32 Height)
	{
		bFinishClearWindows = false;
		Windows.clear();
		bFinishClearWindows = true;
	}

	void Render()
	{
		if (Windows.empty()) return;

		for ( TPair<FString, std::shared_ptr<UEditorWindow>> Window : Windows)
		{
			Window.second->Render();
		}
	}
	bool IsClear()
	{
		return bFinishClearWindows;
	}

	void Clear()
	{
		bFinishClearWindows = false;
	}

private:
	UEditorDesigner() = default;
	~UEditorDesigner() = default;
	UEditorDesigner(const UEditorDesigner&) = delete;
	UEditorDesigner& operator=(const UEditorDesigner&) = delete;

	TMap<FString, std::shared_ptr<UEditorWindow>> Windows;

	bool bFinishClearWindows;
};