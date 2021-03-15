// Fill out your copyright notice in the Description page of Project Settings.


#include "ControlsMap.h"

#include "wat/Core/WatPlayerController.h"

AControlsMap::AControlsMap()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	MapMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MapMesh"));
	MapMesh->SetupAttachment(RootComponent);
}

void AControlsMap::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerController = nullptr;
	bIsShowingGamepad = false;
	MapMesh->SetMaterial(0, KeyboardControlsMaterial);
}

// Called every frame
void AControlsMap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!PlayerController)
	{
		PlayerController = Cast<AWatPlayerController>(GetWorld()->GetFirstPlayerController());
	}
	else
	{
		if(PlayerController->bIsUsingController)
		{
			if(!bIsShowingGamepad)
			{
				MapMesh->SetMaterial(0, GamepadControlsMaterial);
				bIsShowingGamepad = true;
			}
		}
		else
		{
			if(bIsShowingGamepad)
			{
				MapMesh->SetMaterial(0, KeyboardControlsMaterial);
				bIsShowingGamepad = false;
			}
		}
	}
}

