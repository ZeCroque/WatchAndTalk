// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CameraDeskHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API UCameraDeskHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta=(BindWidget))
    class UKeyIconWidget* ReturnKeyIcon;

	UPROPERTY(meta=(BindWidget))
    class UKeyIconWidget* SwitchFloorsKeyIcon;

	UPROPERTY(meta=(BindWidget))
    class UKeyIconWidget* SwitchFloorsIcon;

	UPROPERTY(meta=(BindWidget))
    class UImage* TopArrow;

	UPROPERTY(meta=(BindWidget))
    class UImage* RightArrow;

	UPROPERTY(meta=(BindWidget))
    class UImage* BottomArrow;

	UPROPERTY(meta=(BindWidget))
    class UImage* LeftArrow;

	UPROPERTY(EditAnywhere)
	UTexture2D* FirstFloorIcon;

	UPROPERTY(EditAnywhere)
	UTexture2D* SecondFloorIcon;

public:
	UPROPERTY(meta=(BindWidget))
    class UHorizontalBox* EndGameHBox;
	
	UPROPERTY()
	class ACameraDesk* Parent;

	void Init() const;

	void Update(bool bIsOnMap, int MonitorIndex = 0, bool bIsFirstFloor = true) const;

	void HideAll() const;
};
