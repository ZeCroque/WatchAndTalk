// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"
#include "MiniGames/MiniGameWidget.h"
#include "MiniGameMouseWidget.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API UMiniGameMouseWidget : public UMiniGameWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	UAudioComponent* ClickDownComponent;
	
	UPROPERTY(meta = (BindWidget))
	class UImage* Image;

	UPROPERTY(EditAnywhere, Category= "WAT MouseMinigame")
    class USoundCue* MouseDownSound;

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

	TArray<FVector2D> DrawPointList;

	TArray<FVector2D> FirstDrawPointList;

	virtual void Start() override;

	virtual void Stop() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	FVector2D OldMouseMove;
	FVector2D LastMouseMove;

	FVector2D MousePosition;

	bool bIsClicking;

	bool bIsAButtonPressed;

	virtual bool OnInput(FKey PressedInput, EInputEvent EventType) override;

public:
	UPROPERTY(EditAnywhere, Category = "WAT - Setup")
	UTexture* BackGround;

	UPROPERTY(EditAnywhere, Category = "WAT - Setup")
	UTexture* ToolTexture;

	UPROPERTY(EditAnywhere, Category = "WAT - Setup")
	FLinearColor ProgressColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, Category = "WAT - Setup")
	FLinearColor WayColor = FLinearColor::Red;

	UPROPERTY(EditAnywhere, Category = "WAT - Setup")
	int ProgressRange = 40;

	UPROPERTY(EditAnywhere, Category = "WAT - Setup")
	FVector2D ToolTextureOffset = FVector2D(0, 100);

	UPROPERTY(EditAnywhere, Category="WAT - Setup")
	float SpeedMult = 2.f;
};
