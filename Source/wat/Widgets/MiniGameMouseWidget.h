// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "MiniGameMouseWidget.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API UMiniGameMouseWidget : public UUserWidget
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
	TArray<FVector2D> PointList;
	
	
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
};
