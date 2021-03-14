// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "wat/Widgets/MiniGameMouseWidget.h"
#include "MiniGameMouseWidgetAdn.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API UMiniGameMouseWidgetAdn : public UMiniGameMouseWidget
{
	GENERATED_BODY()

	virtual void Start() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "WAT - Setup")
	float ErrorToleranceRatio = 1.5f;

	UPROPERTY(EditAnywhere, Category = "WAT - Setup")
	int StrandRatio = 15;

	TArray<FVector2D> ShadowPointList;
};
