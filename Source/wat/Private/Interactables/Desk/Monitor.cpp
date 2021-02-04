// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Desk/Monitor.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Core/MainGameState.h"
#include "SecurityCamera.h"
#include "Engine/Canvas.h"
#include "Kismet/KismetRenderingLibrary.h"

// Sets default values
AMonitor::AMonitor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ZoneIndex = 0;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	
	MonitorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MonitorMesh"));
	MonitorMesh->SetupAttachment(RootComponent);
	
	ScreenMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ScreenMesh"));
	ScreenMesh->SetupAttachment(MonitorMesh);
	
	BlendTargetCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("BlendTargetCamera"));
	BlendTargetCamera->SetupAttachment(RootComponent);

	PaddingPercentage = 0.05;
	GraphicQuality = 6;
	SnowTransitionTime = 0.3f;
}

void AMonitor::SwitchFloor()
{
	for(auto* Camera : GameState->Cameras[FloorIndex].FloorZones[ZoneIndex].ZoneCameras)
	{
		auto* SceneCapture = Camera->GetCaptureComponent2D();
		SceneCapture->SetVisibility(false);
		SceneCapture->TextureTarget = nullptr;
	}

	FloorIndex = FloorIndex == 0 ? 1 : 0;

	for(int i = 0; i < GameState->Cameras[FloorIndex].FloorZones[ZoneIndex].ZoneCameras.Num(); ++i) 
	{
		auto* SceneCapture = GameState->Cameras[FloorIndex].FloorZones[ZoneIndex].ZoneCameras[i]->GetCaptureComponent2D();
		SceneCapture->SetVisibility(false);
        SceneCapture->TextureTarget = CameraRenderTargets[i];
	}
}

void AMonitor::SetRenderingState(bool bShouldRender)
{
	for(auto* Camera : GameState->Cameras[FloorIndex].FloorZones[ZoneIndex].ZoneCameras)
	{
		auto* SceneCapture = Camera->GetCaptureComponent2D();
		SceneCapture->SetVisibility(bShouldRender);
	}

	if(bShouldRender)
	{
		bStartSnowFading = true;
	}
	else
	{
		bStartSnowGrowth = true;
	}
}

// Called when the game starts or when spawned
void AMonitor::BeginPlay()
{
	Super::BeginPlay();

	FloorIndex = 0;
	
	GameState = Cast<AMainGameState>(GetWorld()->GetGameState());

	FVector ScreenStart;
	FVector ScreenEnd;
	ScreenMesh->GetLocalBounds(ScreenStart, ScreenEnd);
	const auto ScreenDimensions = ScreenEnd - ScreenStart;
	const auto AspectRatio = ScreenDimensions.Z / ScreenDimensions.X;
	const auto RenderingSize = FMath::Pow(2,GraphicQuality);
	const auto ScreenPadding = RenderingSize * PaddingPercentage;

	RenderTargetWidth = RenderingSize - ScreenPadding;
	RenderTargetHeight = RenderTargetWidth * AspectRatio;
	
    const auto CameraCount = GameState->Cameras[FloorIndex].FloorZones[ZoneIndex].ZoneCameras.Num();
    CameraRenderMaterials.Reserve(CameraCount);
    CameraRenderTargets.Reserve(CameraCount);
    for(auto* SourceCamera : GameState->Cameras[FloorIndex].FloorZones[ZoneIndex].ZoneCameras) 
	{
		auto* RenderMaterial = UMaterialInstanceDynamic::Create(BaseRenderMaterial, this);
        RenderMaterial->ShadingModels = FMaterialShadingModelField(EMaterialShadingModel::MSM_Unlit);
		CameraRenderMaterials.Add(RenderMaterial);

		auto* RenderTarget = NewObject<UTextureRenderTarget2D>(this);

        RenderTarget->InitAutoFormat(RenderTargetWidth, RenderTargetHeight);
        CameraRenderTargets.Add(RenderTarget);

        auto* SceneCapture = SourceCamera->GetCaptureComponent2D();
        SceneCapture->TextureTarget = RenderTarget;
    	SceneCapture->SetVisibility(false);

		RenderMaterial->SetTextureParameterValue(TEXT("RenderTexture"), Cast<UTexture>(RenderTarget));
    }

	ScreenMaterial = ScreenMesh->CreateAndSetMaterialInstanceDynamic(0);
	ScreenMaterial->ShadingModels = FMaterialShadingModelField(EMaterialShadingModel::MSM_Unlit);

	ScreenRenderTarget = NewObject<UTextureRenderTarget2D>(this);
	ScreenRenderTarget->InitAutoFormat(RenderingSize * 2.f + ScreenPadding, (RenderingSize * 2.f + ScreenPadding) * AspectRatio);

	ScreenMaterial->SetTextureParameterValue(TEXT("RenderTexture"), Cast<UTexture>(ScreenRenderTarget));

	SnowMaterial = UMaterialInstanceDynamic::Create(BaseSnowMaterial, this);
    SnowMaterial->ShadingModels = FMaterialShadingModelField(EMaterialShadingModel::MSM_Unlit);
	
	bStartSnowGrowth = false;
	bStartSnowFading = false;

	bIsInSnowGrowthMode = false;
	bIsInSnowFadingMode = false;
}

// Called every frame
void AMonitor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if(bStartSnowGrowth && !bIsInSnowFadingMode)
	{
		bStartSnowGrowth = false;
		bIsInSnowGrowthMode = true;
	}
	else if(bStartSnowFading && !bIsInSnowGrowthMode)
	{
		bStartSnowFading = false;
		bIsInSnowFadingMode = true;
	}

	if(bIsInSnowGrowthMode || bIsInSnowFadingMode)
	{
		
		ElapsedTime += DeltaTime;
	
		const auto OpacityRatio = DeltaTime  / SnowTransitionTime;

		float CurrentOpacity;
		SnowMaterial->GetScalarParameterValue(FName("Opacity"), CurrentOpacity);
		
		if(bIsInSnowFadingMode)
		{
			SnowMaterial->SetScalarParameterValue("Opacity",  FMath::Max(0.f, CurrentOpacity - OpacityRatio));
		}
		else if(bIsInSnowGrowthMode)
		{
			SnowMaterial->SetScalarParameterValue("Opacity", FMath::Min(1.f ,CurrentOpacity + OpacityRatio));
		}

		if(ElapsedTime >= SnowTransitionTime)
		{
			OnTransitionEnd.Broadcast(bIsInSnowGrowthMode);
			bIsInSnowFadingMode = false;
			bIsInSnowGrowthMode = false;
			ElapsedTime = 0.f;
		}
	}

    auto* Canvas  = GetWorld()->GetCanvasForDrawMaterialToRenderTarget();
    auto CanvasSize = FVector2D();
    auto Context = FDrawToRenderTargetContext();
    UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(this, ScreenRenderTarget, Canvas, CanvasSize, Context);

	//Cameras
    Canvas->K2_DrawMaterial(CameraRenderMaterials[0], FVector2D(0.f,0.f), FVector2D(RenderTargetWidth,RenderTargetHeight),FVector2D(0.f,0.f));
    Canvas->K2_DrawMaterial(CameraRenderMaterials[1], FVector2D(CanvasSize.X - RenderTargetWidth,0.f), FVector2D(RenderTargetWidth,RenderTargetHeight),FVector2D(0.f,0.f));
    Canvas->K2_DrawMaterial(CameraRenderMaterials[2], FVector2D(0.f,CanvasSize.Y - RenderTargetHeight), FVector2D(RenderTargetWidth,RenderTargetHeight),FVector2D(0.f,0.f));
    Canvas->K2_DrawMaterial(CameraRenderMaterials[3], FVector2D(CanvasSize.X - RenderTargetWidth,CanvasSize.Y - RenderTargetHeight), FVector2D(RenderTargetWidth,RenderTargetHeight),FVector2D(0.f,0.f));

	//Snow
    Canvas->K2_DrawMaterial(SnowMaterial, FVector2D(0.f,0.f), FVector2D(RenderTargetWidth,RenderTargetHeight),FVector2D(0.f,0.f));
    Canvas->K2_DrawMaterial(SnowMaterial, FVector2D(CanvasSize.X - RenderTargetWidth,0.f), FVector2D(RenderTargetWidth,RenderTargetHeight),FVector2D(0.f,0.f));
    Canvas->K2_DrawMaterial(SnowMaterial, FVector2D(0.f,CanvasSize.Y - RenderTargetHeight), FVector2D(RenderTargetWidth,RenderTargetHeight),FVector2D(0.f,0.f));
    Canvas->K2_DrawMaterial(SnowMaterial, FVector2D(CanvasSize.X - RenderTargetWidth,CanvasSize.Y - RenderTargetHeight), FVector2D(RenderTargetWidth,RenderTargetHeight),FVector2D(0.f,0.f));
	
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, Context);
}

