#pragma once

#include "Editor/EditorWindow.h"
#include "Interface/ISwitchable.h"

class StatWindow : public UEditorWindow, public ISwitchable
{

public:
	StatWindow();
	StatWindow(const StatWindow&) {};

	void Render() override;

	void OnResize(UINT32 Width, UINT32 Height) override;

	void Toggle() override;
private:
	
	bool bWasOpen;
};

