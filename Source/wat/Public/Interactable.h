// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "CoreMinimal.h"
#include "Interactable.generated.h"

UCLASS()
class WAT_API AInteractable : public APawn
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	float BlendTime;
	
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;
	
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* InteractableWidget;
	
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* InteractableCamera;

protected:
	class APlayerCharacter* PlayerCharacter;
	class APlayerController* PlayerController;
	bool bIsOverlappingWithPlayer;
	
public:	
	// Sets default values for this actor's properties
	AInteractable();
	virtual void Interact();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
