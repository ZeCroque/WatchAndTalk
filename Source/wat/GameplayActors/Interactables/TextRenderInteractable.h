// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Interactable.h"
#include "TextRenderInteractable.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API ATextRenderInteractable : public AInteractable
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	class UTextRenderComponent* TextRender;

	ATextRenderInteractable();
};
