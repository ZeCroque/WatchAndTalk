// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Desk/CameraDesk.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Characters/PlayerCharacter.h"
#include "Core/MainPlayerController.h"
#include "Interactables/Desk/Monitor.h"
#include "wat/Interactables/Desk/MonitorDegeu.h"

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
			AMonitorDegeu* MonitorDegeu = Cast<AMonitorDegeu>(GetMonitorByIndex(CurrentIndex));
			MonitorDegeu->RenderSourceCamera->SetVisibleFlag(true);
			MonitorDegeu->RenderSourceCamera1->SetVisibleFlag(true);
			MonitorDegeu->RenderSourceCamera2->SetVisibleFlag(true);
			MonitorDegeu->RenderSourceCamera3->SetVisibleFlag(true);
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
				AMonitorDegeu* MonitorDegeu = Cast<AMonitorDegeu>(GetMonitorByIndex(CurrentIndex));
				MonitorDegeu->RenderSourceCamera->SetVisibleFlag(false);
				MonitorDegeu->RenderSourceCamera1->SetVisibleFlag(false);
				MonitorDegeu->RenderSourceCamera2->SetVisibleFlag(false);
				MonitorDegeu->RenderSourceCamera3->SetVisibleFlag(false);
				CurrentIndex++;
				MonitorDegeu = Cast<AMonitorDegeu>(GetMonitorByIndex(CurrentIndex));
				MonitorDegeu->RenderSourceCamera->SetVisibleFlag(true);
				MonitorDegeu->RenderSourceCamera1->SetVisibleFlag(true);
				MonitorDegeu->RenderSourceCamera2->SetVisibleFlag(true);
				MonitorDegeu->RenderSourceCamera3->SetVisibleFlag(true);
				PlayerController->SetViewTargetWithBlend(GetMonitorByIndex(CurrentIndex), BlendTime);
			}
		}
		else if (Value < 0.0f && !bToggle)
		{
			bToggle = true;
			if (CurrentIndex > 0)
			{
				AMonitorDegeu* MonitorDegeu = Cast<AMonitorDegeu>(GetMonitorByIndex(CurrentIndex));
				MonitorDegeu->RenderSourceCamera->SetVisibleFlag(false);
				MonitorDegeu->RenderSourceCamera1->SetVisibleFlag(false);
				MonitorDegeu->RenderSourceCamera2->SetVisibleFlag(false);
				MonitorDegeu->RenderSourceCamera3->SetVisibleFlag(false);
				CurrentIndex--;
				MonitorDegeu = Cast<AMonitorDegeu>(GetMonitorByIndex(CurrentIndex));
				MonitorDegeu->RenderSourceCamera->SetVisibleFlag(true);
				MonitorDegeu->RenderSourceCamera1->SetVisibleFlag(true);
				MonitorDegeu->RenderSourceCamera2->SetVisibleFlag(true);
				MonitorDegeu->RenderSourceCamera3->SetVisibleFlag(true);
				PlayerController->SetViewTargetWithBlend(GetMonitorByIndex(CurrentIndex), BlendTime);
			}
		}
		else if (Value == 0.0f && bToggle)
		{
			bToggle = false;
		}
	}
}

void ACameraDesk::Return_Implementation()
{
	if(PlayerCharacter && PlayerController)
	{	
		PlayerController->Possess(PlayerCharacter);
		PlayerController->SetViewTargetWithBlend(PlayerCharacter, BlendTime);
		bInUse = false;

		AMonitorDegeu* MonitorDegeu = Cast<AMonitorDegeu>(GetMonitorByIndex(CurrentIndex));
		MonitorDegeu->RenderSourceCamera->SetVisibleFlag(false);
		MonitorDegeu->RenderSourceCamera1->SetVisibleFlag(false);
		MonitorDegeu->RenderSourceCamera2->SetVisibleFlag(false);
		MonitorDegeu->RenderSourceCamera3->SetVisibleFlag(false);
	}
}

void ACameraDesk::OnInteractionBegin()
{	
	PlayerController->Possess(this);
	PlayerController->SetViewTargetWithBlend(MapChildActorComponent->GetChildActor(), BlendTime);
}

// Called every frame
void ACameraDesk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACameraDesk::SetupPlayerInputComponent(UInputComponent* DeskInputComponent)
{
	Super::SetupPlayerInputComponent(DeskInputComponent);
	// Bind jump events
	DeskInputComponent->BindAction("Return", IE_Pressed, this, &ACameraDesk::Return);

	// Bind movement events
	DeskInputComponent->BindAxis("MoveForward", this, &ACameraDesk::MoveForward);
	DeskInputComponent->BindAxis("MoveRight", this, &ACameraDesk::MoveRight);
}

