// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "wat/Widgets/Tutorial/TutoMessageWidget.h"
#include "CrouchTutoMessageWidget.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API UCrouchTutoMessageWidget : public UTutoMessageWidget
{
	GENERATED_BODY()
	
	virtual void Init() override;
};
