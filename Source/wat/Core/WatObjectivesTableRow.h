// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "WatObjectivesTableRow.generated.h"

UENUM(BlueprintType)
enum class EPlayerClass : uint8
{
	Mechanist = 0,
	Scientist = 1,
	Both = 2
 };

/**
 * 
 */
USTRUCT(BlueprintType)
struct WAT_API FWatObjectivesTableRow : public FTableRowBase
 {
     GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
 	uint8 Index;
 	
	UPROPERTY(EditAnywhere)
	FString ObjectiveDesc;

	UPROPERTY(EditAnywhere)
	EPlayerClass PlayerClass;

 	UPROPERTY(EditAnywhere)
	bool bIsTutorialObjective;
	
};