// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "wat/GameplayActors/Interactables/BlendableInteractable.h"
#include "CameraDesk.generated.h"

enum class EBlendDirection {Unset = 0, Right = 1, Left = 2};

UCLASS()
class WAT_API ACameraDesk : public ABlendableInteractable
{
	GENERATED_BODY()
	
public:

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* MapMesh;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* MapBorderMesh;

	UPROPERTY(EditAnywhere,  Category = "WAT MonitorSettings")
	UMaterialInstance* PostProcessMaterialBase;

	UPROPERTY(EditAnywhere, Category = "WAT MonitorSettings", meta = (ClampMin = "2", ClampMax = "12"))
	int MapGraphicQuality;
	
	UPROPERTY(EditAnywhere, Category = "WAT MonitorSettings")
	UTexture* FirstFloor;

	UPROPERTY(EditAnywhere, Category = "WAT MonitorSettings")
	UTexture* SecondFloor;

	UPROPERTY(EditAnywhere, Category = "WAT MonitorSettings")
	UMaterialInstance* PlayerIcon;

	UPROPERTY(EditAnywhere, Category = "WAT MonitorSettings")
	float PlayerIconSize = 10.f;

	UPROPERTY(EditAnywhere, Category = "WAT MonitorSettings")
	float MapWidthReduction = 281.f;

	UPROPERTY(EditAnywhere, Category = "WAT MonitorSettings")
	float MapXOffset = 85.f;
	
	UPROPERTY(EditAnywhere, Category = "WAT MonitorSettings")
	float MapYOffset = 23.f;

	UPROPERTY(EditAnywhere, Category= "WAT HUDSettings")
	TSubclassOf<UUserWidget> CameraDeskHUDWidgetClass;

	bool bIsSwitchingFloors;
	
private:

	UPROPERTY()
	TArray<class AMonitor*> Monitors;

	UPROPERTY()
	UMaterialInstanceDynamic* MapMaterial;

	UPROPERTY()
	UTextureRenderTarget2D* MapRenderTarget;

	UPROPERTY()
	UTexture* CurrentFloor;

	UPROPERTY()
	UMaterialInstanceDynamic* PostProcessMaterial;

	UPROPERTY()
	class AWatGameState* GameState;
	
	int CurrentIndex;
	int TargetIndex;
	int LastHoveredScreenIndex;

	bool bIsBlendingHorizontally;
	bool bIsBlendingVertically;
	bool bIsInternalBlend;
	bool bIsSwitchingBetweenAdjoiningScreensMoreThanOnce;
	bool bIsOnMap;

	EBlendDirection BlendDirection;

	float TimeToTargetScreen;
	float ElapsedBlendTime;

	float RenderingWidth;
	float RenderingHeight;

	//Inputs
	bool bHorizontalToggle;
	bool bVerticalToggle;

public:
	// Sets default values for this actor's properties
	ACameraDesk();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual  FColor GetColor() override;

private:
	void Initialize();
	
	virtual void OnNotFinalBlend() override;
	
	class AMonitor* GetMonitorByIndex(const int Index);

	void BlendToTargetMonitor();

	void SetCurrentMonitorRenderingState(bool bShouldRender);

	void SwitchFloorInMonitors();
	
	UFUNCTION()
	void OnSnowTransitionEnd(bool bWasOn);

public:
	//Inputs
	void MoveForward(float Value);
	void MoveRight(float Value);
	
	void Return();
	void SwitchFloor();
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* DeskInputComponent) override;

};
