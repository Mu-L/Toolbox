﻿// Copyright Solessfir. All Rights Reserved.

#include "ToolboxSoftwareFunctionLibrary.h"
#include "HAL/PlatformApplicationMisc.h"
#include "ToolboxHelpers.h"

#if WITH_EDITOR
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(ToolboxSoftwareFunctionLibrary)

bool UToolboxSoftwareFunctionLibrary::IsInEditor()
{
	return GIsEditor;
}

bool UToolboxSoftwareFunctionLibrary::IsShippingBuild()
{
	#if UE_BUILD_SHIPPING || UE_BUILD_TEST
	return true;
	#else
	return false;
	#endif
}

bool UToolboxSoftwareFunctionLibrary::IsPlayInEditor(const UObject* WorldContextObject)
{
	if (const UWorld* World = ToolboxHelpers::GetWorld(WorldContextObject))
	{
		return World->IsPlayInEditor();
	}

	return false;
}

bool UToolboxSoftwareFunctionLibrary::IsInEditorPreviewWindow(const UObject* WorldContextObject)
{
	#if WITH_EDITOR
	if (const UWorld* World = ToolboxHelpers::GetWorld(WorldContextObject))
	{
		return World->WorldType == EWorldType::EditorPreview;
	}
	return false;
	#else
	return false;
	#endif
}

int32 UToolboxSoftwareFunctionLibrary::GetFPS(const UObject* WorldContextObject)
{
	if (const UWorld* World = ToolboxHelpers::GetWorld(WorldContextObject))
	{
		return FMath::FloorToInt(1.f / World->GetDeltaSeconds());
	}
	
	return 0;
}

void UToolboxSoftwareFunctionLibrary::SetFrameRateCap(const int32 FrameRateCap)
{
	if (!GEngine)
	{
		return;
	}
	
	if (FrameRateCap > 0)
	{
		GEngine->bUseFixedFrameRate = true;
		GEngine->FixedFrameRate = FMath::Max<float>(static_cast<float>(FrameRateCap), 15.f);
		return;
	}
	
	GEngine->bUseFixedFrameRate = false;
}

void UToolboxSoftwareFunctionLibrary::GetThreadsTime(float& FrameTime, float& GameThreadTime, float& RenderThreadTime, float& RHIThreadTime, float& GPUFrameTime)
{
	extern uint32 GGameThreadTime;
	extern uint32 GRenderThreadTime;
	extern uint32 GRHIThreadTime;

	FrameTime = static_cast<float>((FApp::GetCurrentTime() - FApp::GetLastTime()) * 1000.0);
	GameThreadTime = FPlatformTime::ToMilliseconds(GGameThreadTime);
	RenderThreadTime = FPlatformTime::ToMilliseconds(GRenderThreadTime);
	RHIThreadTime = FPlatformTime::ToMilliseconds(GRHIThreadTime);
	GPUFrameTime = FPlatformTime::ToMilliseconds(RHIGetGPUFrameCycles());
}

bool UToolboxSoftwareFunctionLibrary::IsGameInForeground()
{
	return FPlatformApplicationMisc::IsThisApplicationForeground();
}

void UToolboxSoftwareFunctionLibrary::FlashGameOnTaskBar(const bool bBackgroundOnly)
{
	if (!GEngine)
	{
		return;
	}

	if (!bBackgroundOnly || !IsGameInForeground())
	{
		GEngine->GameViewport->GetWindow()->DrawAttention(FWindowDrawAttentionParameters(EWindowDrawAttentionRequestType::UntilActivated));
	}
}

void UToolboxSoftwareFunctionLibrary::SetWindowMode(const EWindowModeType Mode)
{
	if (!GEngine || Mode == EWindowModeType::Invalid)
	{
		return;
	}

	UGameViewportClient* ViewportClient = GEngine->GameViewport;
	if (!ViewportClient)
	{
		return;
	}

	if (SWindow* Window = ViewportClient->GetWindow().Get())
	{
		Window->SetWindowMode(static_cast<EWindowMode::Type>(Mode));
	}
}

EWindowModeType UToolboxSoftwareFunctionLibrary::GetWindowMode()
{
	if (!GEngine)
	{
		return EWindowModeType::Invalid;
	}

	UGameViewportClient* ViewportClient = GEngine->GameViewport;
	if (!ViewportClient)
	{
		return EWindowModeType::Invalid;
	}

	if (const SWindow* Window = ViewportClient->GetWindow().Get())
	{
		return static_cast<EWindowModeType>(Window->GetWindowMode());
	}

	return EWindowModeType::Invalid;
}

void UToolboxSoftwareFunctionLibrary::PrintMessageLog(const EMessageSeverityType Severity, const FName LogCategory, const FString Message)
{
	#if WITH_EDITOR
	if (!Message.IsEmpty())
	{
		FMessageLog PIELogger(LogCategory);
		PIELogger.Message(static_cast<EMessageSeverity::Type>(Severity), FText::FromString(Message));
	}
	#endif
}

void UToolboxSoftwareFunctionLibrary::ShowNotification(const ENotificationSeverityType Severity, const float Duration, const float FadeInDuration, const float FadeOutDuration, const float Width, const FString Title, const FString Message)
{
	#if WITH_EDITOR
	if (!Title.IsEmpty() && Duration > 0.f)
	{
		FNotificationInfo Info(FText::FromString(Title));
		Info.ExpireDuration = Duration;

		if (!Message.IsEmpty())
		{
			Info.SubText = FText::FromString(Message);
		}

		Info.FadeInDuration = FMath::Max<float>(FadeInDuration, 0.f);
		Info.FadeOutDuration = FMath::Max<float>(FadeOutDuration, 0.f);
		Info.WidthOverride = FMath::Max<float>(Width, 50.f);
		Info.bFireAndForget = true;

		switch (Severity)
		{
			case ENotificationSeverityType::Warning:
				Info.Image = FAppStyle::Get().GetBrush(TEXT("Icons.WarningWithColor.Large"));
				break;
			case ENotificationSeverityType::Success:
				Info.Image = FAppStyle::Get().GetBrush(TEXT("Icons.SuccessWithColor.Large"));
				break;
			case ENotificationSeverityType::Error:
				Info.Image = FAppStyle::Get().GetBrush(TEXT("Icons.ErrorWithColor.Large"));
				break;
			default:
				break;
		}

		FSlateNotificationManager::Get().AddNotification(Info);
	}
	#endif
}
