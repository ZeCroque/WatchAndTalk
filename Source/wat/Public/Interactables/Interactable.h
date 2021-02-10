// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "CoreMinimal.h"
#include "Interactable.generated.h"

UCLASS()
class WAT_API AInteractable : public APawn
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* InteractableWidget;

protected:

	UPROPERTY(Replicated)
	bool bInUse;
	
public:	
	// Sets default values for this actor's properties
	AInteractable();
	void Interact();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated)
	class AMainPlayerController* PlayerController;

	UPROPERTY(Replicated)
	class APlayerCharacter* PlayerCharacter;

	bool bCanInteract;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void OnInteractionBegin(){}
	
	virtual void OnInteraction(){}

	void RefreshIconWidgetForCurrentDevice(bool bIsGamePad) const;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
