// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Desk/CameraDesk.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Characters/PlayerCharacter.h"
#include "Core/MainPlayerController.h"
#include "Interactables/Desk/Monitor.h"

// Sets default values
ACameraDesk::ACameraDesk()
{
	MapMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MapMesh"));
	MapMesh->SetupAttachment(StaticMesh);

	MapBorderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MapBorderMesh"));
	MapBorderMesh->SetupAttachment(StaticMesh);
}

// Called when the game starts or when spawned
void ACameraDesk::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<UActorComponent*> Components;
	GetComponents(UChildActorComponent::StaticClass(), Components);

	const int Length = Components.Num();
	
	Monitors.Reserve(Length);
	Monitors.SetNum(Length);
	for (int i = 0; i < Length; i++)
	{
		auto* Monitor = Cast<UChildActorComponent>(Components[i]);
		if (Monitor)
		{
			Monitors[i] = Cast<AMonitor>(Monitor->GetChildActor());
		}
	}

	Initialize();
}

void ACameraDesk::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
	if(bIsBlendingHorizontally)
	{
		ElapsedBlendTime += DeltaSeconds;

		const bool bHasHoveredScreen = static_cast<int>(ElapsedBlendTime / BlendTime) > 0;
		ElapsedBlendTime -= BlendTime * bHasHoveredScreen;	
		LastHoveredScreenIndex = BlendDirection == EBlendDirection::Right ? LastHoveredScreenIndex + bHasHoveredScreen : BlendDirection == EBlendDirection::Left ? LastHoveredScreenIndex - bHasHoveredScreen : CurrentIndex;
	}
}

void ACameraDesk::Initialize()
{
	CurrentIndex = 0;
	TargetIndex = 0;
	bToggle = false;
	bIsInternalBlend = false;
	bIsBlendingHorizontally = false;
	bIsBlendingVertically = false;
	LastHoveredScreenIndex = 0;
	BlendDirection = EBlendDirection::Unset;
	ElapsedBlendTime = 0.f;
	TimeToTargetScreen = 0.f;
}

void ACameraDesk::OnNotFinalBlend()
{
	if(!bIsInternalBlend)
	{
		Super::OnNotFinalBlend();
		bIsOnMap = true;
		bIsInternalBlend = true;	
	}
	else
	{
		if(bIsBlendingHorizontally)
		{
			bIsBlendingHorizontally = false;
			ElapsedBlendTime = 0.f;	
			CurrentIndex = TargetIndex;
			LastHoveredScreenIndex = CurrentIndex;
			TimeToTargetScreen = 0.f;
			BlendDirection = EBlendDirection::Unset;
		}
		else
		{
			bIsBlendingVertically = false;
		}

		SetCurrentMonitorRenderingState(true);
	}
}

AMonitor* ACameraDesk::GetMonitorByIndex(const int Index)
{
	for (auto* Monitor : Monitors)
	{
		if (Monitor->ZoneIndex == Index)
		{
			return Monitor;
		}
	}
	return nullptr;
}

void ACameraDesk::BlendToTargetMonitor()
{
	bIsBlendingHorizontally = true;

	const int IndexDelta = TargetIndex - LastHoveredScreenIndex;

	EBlendDirection CurrentBlendDirection;
	if(IndexDelta)
	{
		bIsSwitchingBetweenAdjoiningScreensMoreThanOnce = false;
		if(IndexDelta > 0)
		{
			CurrentBlendDirection = EBlendDirection::Right;
		}
		else
		{
			CurrentBlendDirection = EBlendDirection::Left;
		}

		if(BlendDirection == CurrentBlendDirection)
		{		
			TimeToTargetScreen = BlendTime * FMath::Abs(IndexDelta) - ElapsedBlendTime;
		}
		else
		{		
			TimeToTargetScreen = BlendTime * FMath::Abs(IndexDelta) + ElapsedBlendTime;
		}
	}
	else
	{	
		LastHoveredScreenIndex = BlendDirection == EBlendDirection::Right ? FMath::Min(LastHoveredScreenIndex + 1, Monitors.Num() - 1) : FMath::Max(LastHoveredScreenIndex - 1, 0);
		if(!bIsSwitchingBetweenAdjoiningScreensMoreThanOnce)
		{
			TimeToTargetScreen = ElapsedBlendTime;
			bIsSwitchingBetweenAdjoiningScreensMoreThanOnce = true;
		}
		else
		{
			TimeToTargetScreen = BlendTime - TimeToTargetScreen + ElapsedBlendTime;
		}
		ElapsedBlendTime = 0.f;	
		
		CurrentBlendDirection = BlendDirection == EBlendDirection::Right ? EBlendDirection::Left : EBlendDirection::Right;
	}

	BlendDirection = CurrentBlendDirection;
	PlayerController->SetViewTargetWithBlend(GetMonitorByIndex(TargetIndex), TimeToTargetScreen, VTBlend_Linear, 0, true);
}

void ACameraDesk::SetCurrentMonitorRenderingState(bool bShouldRender)
{
	auto* Monitor = GetMonitorByIndex(CurrentIndex);
	if(Monitor)
	{
		Monitor->SetRenderingState(bShouldRender);
	}
}

void ACameraDesk::MoveForward(float Value)
{
	if (bIsInternalBlend && !bIsBlendingHorizontally && PlayerController)
	{
		if (Value > 0.0f)
		{
			bIsOnMap = true;
			bIsBlendingVertically = true;
			PlayerController->SetViewTargetWithBlend(this, BlendTime, VTBlend_Linear, 0, true);
			SetCurrentMonitorRenderingState(false);
		}
		else if (Value < 0.0f)
		{
			bIsOnMap = false;
			bIsBlendingVertically = true;
			PlayerController->SetViewTargetWithBlend(GetMonitorByIndex(CurrentIndex), BlendTime, VTBlend_Linear, 0, true);
		}
	}
}

void ACameraDesk::MoveRight(float Value)
{
	if (bIsInternalBlend && !bIsBlendingVertically && PlayerController)
	{
		if (Value > 0.0f && !bToggle)
		{
			bToggle = true;
			if (TargetIndex < Monitors.Num() - 1)
			{
				SetCurrentMonitorRenderingState(false);
				++TargetIndex;
				BlendToTargetMonitor();
			}
		}
		else if (Value < 0.0f && !bToggle)
		{
			bToggle = true;
			if (TargetIndex > 0)
			{
				SetCurrentMonitorRenderingState(false);
				--TargetIndex;
				BlendToTargetMonitor();
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
		SetCurrentMonitorRenderingState(false);
		BlendToPlayer();
		Initialize();
	}
}

void ACameraDesk::SwitchFloor()
{
	for(auto* Monitor : Monitors)
	{
		Monitor->SwitchFloor();
	}
	if(!bIsOnMap)
	{
		SetCurrentMonitorRenderingState(true);
	}
}

void ACameraDesk::SetupPlayerInputComponent(UInputComponent* DeskInputComponent)
{
	Super::SetupPlayerInputComponent(DeskInputComponent);
	// Bind action events
	DeskInputComponent->BindAction("Return", IE_Pressed, this, &ACameraDesk::Return);
	DeskInputComponent->BindAction("Interact", IE_Pressed, this,&ACameraDesk::SwitchFloor);
	
	// Bind movement events
	DeskInputComponent->BindAxis("MoveForward", this, &ACameraDesk::MoveForward);
	DeskInputComponent->BindAxis("MoveRight", this, &ACameraDesk::MoveRight);
}

