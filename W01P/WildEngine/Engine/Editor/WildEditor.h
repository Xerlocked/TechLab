#pragma once

#include "Windows.h"
#include "Types/Types.h"
#include "memory"

class URenderer;
class UScene;

class UWildEditor
{
public:
	UWildEditor(URenderer* InRenderer);
	UWildEditor(const UWildEditor&);
	~UWildEditor();

	void Create(ID3D11Device* InDevice, ID3D11DeviceContext* InDeviceContext, HWND InHWND);
	void Release();

	void Render();

	void OnResize();

private:
	void PreferenceStyle();

	void CreateUsingFont();

	void SetupControlWindow();

	void NewScene(FString SceneName);
	void LoadScene(FString SceneName);
	void SaveScene(FString SceneName);

	FString CleanTypeName(const FString& rawName);

	void SetupPropertyWindow();

	void SetupConsoleWindow();

private:
	URenderer* Renderer;
	UScene* Scene;

	ID3D11Device* Device;
	ID3D11DeviceContext* DeviceContext;
	HWND hWnd;
};

