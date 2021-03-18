// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniGameMouseWidget.h"


#include "Components/AudioComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Canvas.h"
#include "Components/Image.h"
#include "wat/Core/WatPlayerCharacter.h"
#include "wat/Core/WatPlayerController.h"
#include "wat/Core/WatStaticUtils.h"
#include "wat/GameplayActors/Interactables/Objectives/Objective.h"


void UMiniGameMouseWidget::Start()
{
	Super::Start();

	ClickDownComponent = nullptr;
	PointList.Empty();
	LocalSize = Image->Brush.ImageSize;
	RenderTarget = NewObject<UTextureRenderTarget2D>(this);
	RenderTarget->InitAutoFormat(LocalSize.X, LocalSize.Y);
	ParentInteractable->PlayerController->bShowMouseCursor = true;
	const FInputModeUIOnly InputUIOnly;
	ParentInteractable->PlayerController->SetInputMode(InputUIOnly);
	Image->Brush.SetResourceObject(RenderTarget);
}

void UMiniGameMouseWidget::Stop()
{
	if(ClickDownComponent && ClickDownComponent->IsPlaying())
	{
		ClickDownComponent->Stop();
	}
	ParentInteractable->PlayerController->bShowMouseCursor = false;
	const FInputModeGameAndUI InputGameAndUI;
	ParentInteractable->PlayerController->SetInputMode(InputGameAndUI);
	Super::Stop();
}

void UMiniGameMouseWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	
	if(bIsAButtonPressed)
	{		
		LastMouseMove = FVector2D(ParentInteractable->XVelocity, ParentInteractable->YVelocity * (ParentInteractable->PlayerCharacter->bInvertYAxis ? 1.f : -1.f)) * SpeedMult;
	}
}

FReply UMiniGameMouseWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bIsClicking && !bIsAButtonPressed)
	{
		const auto ScreenPosition = InMouseEvent.GetScreenSpacePosition();
		LastMouseMove = FVector2D( FMath::Min(1.f, (ScreenPosition.X - MousePosition.X) / (LocalSize.X / 2.f)),  FMath::Min(1.f,(ScreenPosition.Y - MousePosition.Y) / (LocalSize.Y / 2.f))) * SpeedMult;
	}

	return FReply::Handled();
}

FReply UMiniGameMouseWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	bIsClicking = true;
	MousePosition = InMouseEvent.GetScreenSpacePosition();
	return FReply::Handled();
}

FReply UMiniGameMouseWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	bIsClicking = false;
	return FReply::Handled();
}

bool UMiniGameMouseWidget::OnInput(FKey PressedInput, EInputEvent EventType)
{
	if(!Super::OnInput(PressedInput,EventType))
	{
		if(PressedInput.IsGamepadKey() && PressedInput == UWatStaticUtils::GetKeyForDevice("Interact", true))
		{
			bIsAButtonPressed = EventType != IE_Released;
			bIsClicking = bIsAButtonPressed;
			return true;
		}
	}
	return false;
}
