// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TextRenderInteractable.h"
#include "SwitchClassInteractable.generated.h"

UCLASS()
class WAT_API ASwitchClassInteractable : public ATextRenderInteractable
{
	GENERATED_BODY()

protected:

	UFUNCTION()
	virtual void OnInteraction() override;
};
