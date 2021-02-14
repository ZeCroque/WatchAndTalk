// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ObjectivesHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API UObjectivesHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY()
	class AWatPlayerCharacter* Parent;

	UPROPERTY()
	class AWatPlayerController* Controller;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> RowClass;
	
	UPROPERTY(meta=(BindWidget))
	class UVerticalBox* ObjectivesList;

protected:

	virtual void NativeConstruct() override;
};
