// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "MonitorChildActorComponent.h"
#include "CameraDesk.generated.h"

UCLASS()
class WAT_API ACameraDesk : public AInteractable
{
	GENERATED_BODY()
	
public:

	UPROPERTY(VisibleAnywhere)
	UChildActorComponent* MapChildActorComponent;
	
	UPROPERTY()
	TArray<UMonitorChildActorComponent*> Monitors;
	
	// Sets default values for this actor's properties
	ACameraDesk();

private:
	UPROPERTY()
	int CurrentIndex;
	bool bToggle;
	
	class AMonitor* GetMonitorByIndex(const int Index);
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Return();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Interact() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* DeskInputComponent) override;

};
