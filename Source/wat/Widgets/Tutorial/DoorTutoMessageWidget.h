// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "wat/Widgets/Tutorial/TutoMessageWidget.h"
#include "DoorTutoMessageWidget.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API UDoorTutoMessageWidget : public UTutoMessageWidget
{
	GENERATED_BODY()

	virtual void Init() override;
};
