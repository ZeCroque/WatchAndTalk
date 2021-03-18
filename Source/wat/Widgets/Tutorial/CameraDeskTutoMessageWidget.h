// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TutoMessageWidget.h"
#include "CameraDeskTutoMessageWidget.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API UCameraDeskTutoMessageWidget : public UTutoMessageWidget
{
	GENERATED_BODY()

	virtual void OnInput(FKey PressedInput);
};
