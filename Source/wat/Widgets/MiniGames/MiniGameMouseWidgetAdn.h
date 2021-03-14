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

	TArray<FVector2D> ShadowPointList;

public:
	UPROPERTY(EditAnywhere, Category = "WAT - Setup")
	float ErrorToleranceRatio = 1.1f;

	UPROPERTY(EditAnywhere, Category = "WAT - Setup")
	int StrandRatio = 15;
};
