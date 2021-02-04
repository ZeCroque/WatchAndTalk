// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/BlendableInteractable.h"
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
	
	UPROPERTY()
	TArray<class AMonitor*> Monitors;

	//TODO
	UPROPERTY()
	UMaterialInstanceDynamic* MapMaterial;
	
	UPROPERTY(EditAnywhere, Category = "WAT Debug")
	UTexture* FirstFloor;

	UPROPERTY(EditAnywhere, Category = "WAT Debug")
	UTexture* SecondFloor;
	
private:
	
	int CurrentIndex;
	int TargetIndex;
	int LastHoveredScreenIndex;

	bool bIsInternalBlend;
	bool bIsBlendingHorizontally;
	bool bIsBlendingVertically;
	bool bIsSwitchingBetweenAdjoiningScreensMoreThanOnce;
	bool bIsOnMap;
	bool bIsSwitchingFloors;

	EBlendDirection BlendDirection;

	float TimeToTargetScreen;
	float ElapsedBlendTime;

	//Inputs
	bool bToggle;

public:
	// Sets default values for this actor's properties
	ACameraDesk();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

private:
	void Initialize();
	
	virtual void OnNotFinalBlend() override;
	
	class AMonitor* GetMonitorByIndex(const int Index);

	void BlendToTargetMonitor();

	void SetCurrentMonitorRenderingState(bool bShouldRender);

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
