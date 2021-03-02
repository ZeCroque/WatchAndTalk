// Fill out your copyright notice in the Description page of Project Settings.


#include "wat/GameplayActors/Interactables/Desk/CameraDesk.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SceneCaptureComponent2D.h"
#include "wat/Core//WatPlayerCharacter.h"
#include "wat/Core/WatPlayerController.h"
#include "Engine/TextureRenderTarget2D.h"
#include "wat/GameplayActors/Interactables/Desk/Monitor.h"
#include "Engine/Canvas.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "wat/AI/GridManager.h"
#include "wat/Core/WatGameState.h"
#include "wat/Widgets/CameraDeskHUDWidget.h"

// Sets default values
ACameraDesk::ACameraDesk()
{
	PrimaryActorTick.bCanEverTick = true;
	
	MapMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MapMesh"));
	MapMesh->SetupAttachment(StaticMesh);

	MapBorderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MapBorderMesh"));
	MapBorderMesh->SetupAttachment(StaticMesh);

	MapGraphicQuality = 8;
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

	FVector ScreenStart;
	FVector ScreenEnd;
	MapMesh->GetLocalBounds(ScreenStart, ScreenEnd);
	const auto ScreenDimensions = (ScreenEnd - ScreenStart) * MapMesh->GetComponentScale();
	const auto AspectRatio = ScreenDimensions.Z / ScreenDimensions.Y;
	RenderingWidth = FMath::Pow(2,MapGraphicQuality);
	RenderingHeight = RenderingWidth * AspectRatio;
	
	MapMaterial = MapMesh->CreateAndSetMaterialInstanceDynamic(0);
	MapMaterial->ShadingModels = FMaterialShadingModelField(EMaterialShadingModel::MSM_Unlit);

	MapRenderTarget = NewObject<UTextureRenderTarget2D>(this);
	MapRenderTarget->InitAutoFormat(RenderingWidth, RenderingHeight);

	MapMaterial->SetTextureParameterValue(TEXT("RenderTexture"), Cast<UTexture>(MapRenderTarget));

	PostProcessMaterial = UMaterialInstanceDynamic::Create(PostProcessMaterialBase, this);
    PostProcessMaterial->ShadingModels = FMaterialShadingModelField(EMaterialShadingModel::MSM_Unlit);
	PostProcessMaterial->SetScalarParameterValue("OffSet White Line", FMath::RandRange(0, 50));
	
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

	auto* Canvas  = GetWorld()->GetCanvasForDrawMaterialToRenderTarget();
    auto CanvasSize = FVector2D();
    auto Context = FDrawToRenderTargetContext();
    UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(this, MapRenderTarget, Canvas, CanvasSize, Context);

	Canvas->K2_DrawTexture(CurrentFloor, FVector2D(0.f,0.f), FVector2D(RenderingWidth,RenderingHeight),FVector2D(0.f,0.f));
	Canvas->K2_DrawMaterial(PostProcessMaterial, FVector2D(0.f,0.f), FVector2D(RenderingWidth,RenderingWidth),FVector2D(0.f,0.f));

	auto* GameState = Cast<AWatGameState>(GetWorld()->GetGameState());
	const auto LogicBounds = GameState->GridManager->BoxComponent->GetNavigationBounds();
	
	for(auto* PlayerStateRef : GameState->PlayerArray)
	{
		const auto* Player = PlayerStateRef->GetPawn();
		if(Player)
		{
			const auto LogicPlayerLocation = PlayerStateRef->GetPawn()->GetActorLocation() - LogicBounds.Min;
			const auto MapX = (LogicPlayerLocation.X * RenderingWidth) / (LogicBounds.Max.X - LogicBounds.Min.X) + MapXOffset;
			const auto MapY = (LogicPlayerLocation.Y * RenderingHeight) / (LogicBounds.Max.Y - LogicBounds.Min.Y) + MapYOffset;
			Canvas->K2_DrawMaterial(PlayerIcon, FVector2D(MapX,MapY),FVector2D(PlayerIconSize,PlayerIconSize),FVector2D(0.f, 0.f));
		}
	}
	
	
	
   
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, Context);
}

void ACameraDesk::Initialize()
{
	CurrentIndex = 0;
	TargetIndex = 0;
	bToggle = false;
	bIsInternalBlend = false;
	bIsBlendingHorizontally = false;
	bIsBlendingVertically = false;
	bIsOnMap = true;
	bIsSwitchingFloors = false;
	LastHoveredScreenIndex = 0;
	BlendDirection = EBlendDirection::Unset;
	ElapsedBlendTime = 0.f;
	TimeToTargetScreen = 0.f;

	CurrentFloor = FirstFloor;
}

void ACameraDesk::OnNotFinalBlend()
{
	if(!bIsInternalBlend)
	{
		Super::OnNotFinalBlend();
		bIsOnMap = true;

		if (!PlayerController->CameraDeskHUDWidget)
		{
			PlayerController->CameraDeskHUDWidget = CreateWidget<UCameraDeskHUDWidget>(PlayerController, CameraDeskHUDWidgetClass);
			PlayerController->CameraDeskHUDWidget->Parent = this;
		}

		PlayerController->CameraDeskHUDWidget->AddToViewport();
		PlayerController->CameraDeskHUDWidget->Init();
		
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
		PlayerController->CameraDeskHUDWidget->Update(bIsOnMap, CurrentIndex, CurrentFloor == FirstFloor);
		if(!bIsOnMap)
		{
			SetCurrentMonitorRenderingState(true);
		}
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

void ACameraDesk::SwitchFloorInMonitors()
{
	for(auto* Monitor : Monitors)
	{
		Monitor->SwitchFloor();
	}
}

void ACameraDesk::OnSnowTransitionEnd(bool bWasOn)
{
	if(bWasOn)
	{
		SwitchFloorInMonitors();

		SetCurrentMonitorRenderingState(true);
	}
	else
	{
		GetMonitorByIndex(CurrentIndex)->OnTransitionEnd.Clear();
		bIsSwitchingFloors = false;
	}
}

void ACameraDesk::MoveForward(float Value)
{
	if (bIsInternalBlend && !bIsBlendingHorizontally && !bIsSwitchingFloors && PlayerController)
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
	if (bIsInternalBlend && !bIsBlendingVertically && !bIsOnMap && !bIsSwitchingFloors && PlayerController)
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
		GetMonitorByIndex(CurrentIndex)->OnTransitionEnd.Clear();
		PlayerController->CameraDeskHUDWidget->RemoveFromParent();
		BlendToPlayer();
		Initialize();
	}
}

void ACameraDesk::SwitchFloor()
{
	const auto Monitor = GetMonitorByIndex(CurrentIndex);
	if(!bIsFinalBlend && !bIsSwitchingFloors && !bIsBlendingHorizontally && !bIsBlendingVertically && !Monitor->bIsInSnowGrowthMode && !Monitor->bIsInSnowFadingMode)
	{		
		if(Monitor->FloorIndex != 0)
		{
			CurrentFloor = FirstFloor;
		}
		else
		{
			CurrentFloor = SecondFloor;
		}

		if(!bIsOnMap)
		{
			bIsSwitchingFloors = true;
			Monitor->OnTransitionEnd.AddDynamic(this, &ACameraDesk::OnSnowTransitionEnd);
			SetCurrentMonitorRenderingState(false);
		}
		else
		{
			SwitchFloorInMonitors();
		}
		PlayerController->CameraDeskHUDWidget->Update(bIsOnMap, CurrentIndex, CurrentFloor == FirstFloor);
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

