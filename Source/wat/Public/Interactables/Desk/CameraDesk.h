// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/BlendableInteractable.h"
#include "CameraDesk.generated.h"

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
	TArray<UChildActorComponent*> Monitors;
	
	// Sets default values for this actor's properties
	ACameraDesk();

private:
	UPROPERTY()
	int CurrentIndex;
	bool bToggle;
	
	class AMonitor* GetMonitorByIndex(const int Index);

    //Inputs
	void MoveForward(float Value);
	void MoveRight(float Value);

	UFUNCTION(Server, Reliable)
	void Return();

	bool bIsInternalBlend;
	
	virtual void OnNotFinalBlend() override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* DeskInputComponent) override;

};
