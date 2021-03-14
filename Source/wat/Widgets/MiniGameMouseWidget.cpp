// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniGameMouseWidget.h"

#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Canvas.h"
#include "Components/Image.h"
#include "wat/Core/WatPlayerController.h"
#include "wat/GameplayActors/Interactables/Objectives/Objective.h"


void UMiniGameMouseWidget::Start()
{
	Super::Start();

	PointList.Empty();
	LocalSize = Image->Brush.ImageSize;
	RenderTarget = NewObject<UTextureRenderTarget2D>(this);
	RenderTarget->InitAutoFormat(LocalSize.X, LocalSize.Y);
	ParentInteractable->PlayerController->bShowMouseCursor = true;
	SetUserFocus(ParentInteractable->PlayerController);

	Image->Brush.SetResourceObject(RenderTarget);
}

void UMiniGameMouseWidget::Stop()
{
	ParentInteractable->PlayerController->bShowMouseCursor = false;
	Super::Stop();
}

void UMiniGameMouseWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

FReply UMiniGameMouseWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bIsClicking)
	{
		LastMouseMove = (InMouseEvent.GetScreenSpacePosition() - MousePosition) / 250;
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
