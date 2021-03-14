// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "wat/Widgets/MiniGameMouseWidget.h"
#include "MiniGameMouseWidgetWelding.generated.h"

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
class WAT_API UMiniGameMouseWidgetWelding : public UMiniGameMouseWidget
{
	GENERATED_BODY()

	virtual void Start() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	EDirection DirectionState;
};
