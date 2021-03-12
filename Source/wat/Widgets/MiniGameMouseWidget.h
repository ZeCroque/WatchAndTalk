// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MiniGames/MiniGameWidget.h"


#include "MiniGameMouseWidget.generated.h"


enum class EDirection
{
	Forward,
	Up,
	Down
};

/**
 * 
 */
UCLASS()
class WAT_API UMiniGameMouseWidget : public UMiniGameWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	class UImage* Image;

	UPROPERTY()
	class UTextureRenderTarget2D* RenderTarget;

	UPROPERTY()
	FVector2D LocalSize;

	UPROPERTY()
	FVector2D StartPoint;

	UPROPERTY()
	FVector2D OldPoint;

	UPROPERTY()
	TArray<FVector2D> PointList;

	virtual void Start() override;
	virtual void Stop() override;

	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WAT - Setup", meta = (AllowPrivateAccess = "true"))
	int WeldingRange = 40;

	FVector2D OldMouseMove;
	FVector2D LastMouseMove;

	FVector2D MousePosition;

	bool bIsClicking;

	bool bIsDraw;

	EDirection DirectionState;
};
