// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraDesk.h"
#include "Kismet/GameplayStatics.h"
#include "MainPlayerState.h"
#include "PlayerCharacter.h"
#include "Monitor.h"

// Sets default values
ACameraDesk::ACameraDesk()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CurrentIndex = 0;
	bToggle = false;
	
	MapChildActorComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("MapChildActor"));
	MapChildActorComponent->SetupAttachment(RootComponent);
}

AMonitor* ACameraDesk::GetMonitorByIndex(const int Index)
{
	for (auto* MonitorComponent : Monitors)
	{
		auto* Monitor = Cast<AMonitor>(MonitorComponent->GetChildActor());
		if (Monitor && Monitor->DebugZoneIndex == Index)
		{
			return Monitor;
		}
	}
	return nullptr;
}

void ACameraDesk::MoveForward(float Value)
{
	if (PlayerController)
	{
		if (Value > 0.0f)
		{
			PlayerController->SetViewTargetWithBlend(MapChildActorComponent->GetChildActor(), BlendTime);
		}
		else if (Value < 0.0f)
		{
			PlayerController->SetViewTargetWithBlend(GetMonitorByIndex(CurrentIndex), BlendTime);
		}
	}
}

void ACameraDesk::MoveRight(float Value)
{
	if (PlayerController)
	{
		if (Value > 0.0f && !bToggle)
		{
			bToggle = true;
			if (CurrentIndex < Monitors.Num() - 1)
			{
				CurrentIndex++;
				PlayerController->SetViewTargetWithBlend(GetMonitorByIndex(CurrentIndex), BlendTime);
			}
		}
		else if (Value < 0.0f && !bToggle)
		{
			bToggle = true;
			if (CurrentIndex > 0)
			{
				CurrentIndex--;
				PlayerController->SetViewTargetWithBlend(GetMonitorByIndex(CurrentIndex), BlendTime);
			}
		}
		else if (Value == 0.0f && bToggle)
		{
			bToggle = false;
		}
	}
}

void ACameraDesk::Return()
{
		if(PlayerCharacter && PlayerController)
		{
			auto* ClientState = PlayerController->GetPlayerState<AMainPlayerState>();
			ClientState->Possess(PlayerController, PlayerCharacter);
			bIsOverlappingWithPlayer = true;
		}
}

// Called when the game starts or when spawned
void ACameraDesk::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<UActorComponent*> Components;
	this->GetComponents(UMonitorChildActorComponent::StaticClass(), Components);

	const int Length = Components.Num();
	
	Monitors.Reserve(Length);
	Monitors.SetNum(Length);
	for (int i = 0; i < Length; i++)
	{
		auto* Monitor = Cast<UMonitorChildActorComponent>(Components[i]);
		if (Monitor)
		{
			Monitors[i] = Monitor;
		}
	}
}

void ACameraDesk::Interact()
{
	InteractableWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Hidden);
	bIsOverlappingWithPlayer = false;
	
	if (PlayerCharacter)
	{
		PlayerController = Cast<APlayerController>(PlayerCharacter->GetController());
		if (PlayerController)
		{	
			auto* ClientState = PlayerController->GetPlayerState<AMainPlayerState>();
			ClientState->Possess(PlayerController, this);
			PlayerController->SetViewTargetWithBlend(MapChildActorComponent->GetChildActor(), BlendTime);
		}
	}

}

// Called every frame
void ACameraDesk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACameraDesk::SetupPlayerInputComponent(UInputComponent* DeskInputComponent)
{
	// Bind jump events
	DeskInputComponent->BindAction("Return", IE_Pressed, this, &ACameraDesk::Return);

	// Bind movement events
	DeskInputComponent->BindAxis("MoveForward", this, &ACameraDesk::MoveForward);
	DeskInputComponent->BindAxis("MoveRight", this, &ACameraDesk::MoveRight);
}

