// Fill out your copyright notice in the Description page of Project Settings.


#include "wat/GameplayActors/Interactables/Desk/Monitor.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "wat/Core/MainGameState.h"
#include "SecurityCamera.h"
#include "Engine/Canvas.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "wat/GameplayActors/Interactables/Desk/CameraDesk.h"

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
	BlendingModeSnowTransitionTime = 0.3f;
	FloorSwitchModeSnowTransitionTime = 0.15f;
	MaxSnowOpacity = 0.3f;
	MinSnowOpacity = 0.17f;
	CameraNumberPrefix = "Cam ";
	TextMargin = 10.f;
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

	ParentDesk = Cast<ACameraDesk>(GetParentActor());
	
	GameState = Cast<AMainGameState>(GetWorld()->GetGameState());

	FVector ScreenStart;
	FVector ScreenEnd;
	ScreenMesh->GetLocalBounds(ScreenStart, ScreenEnd);
	const auto ScreenDimensions = (ScreenEnd - ScreenStart) * GetActorScale();
	AspectRatio = ScreenDimensions.Z / ScreenDimensions.X;
	const auto RenderingSize = FMath::Pow(2,GraphicQuality);
	const auto ScreenPadding = RenderingSize * PaddingPercentage;

	RenderTargetWidth = RenderingSize - ScreenPadding;
	RenderTargetHeight = RenderTargetWidth * AspectRatio;

	ScreenWidth = RenderingSize * 2.f + ScreenPadding;
	
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
	ScreenRenderTarget->InitAutoFormat(ScreenWidth, ScreenWidth * AspectRatio);

	ScreenMaterial->SetTextureParameterValue(TEXT("RenderTexture"), Cast<UTexture>(ScreenRenderTarget));

	ElapsedTimeSinceSnowTransitionStart = 0.f;
	ElapsedTime = 0.f;
	
	SnowMaterial = UMaterialInstanceDynamic::Create(BaseSnowMaterial, this);
    SnowMaterial->ShadingModels = FMaterialShadingModelField(EMaterialShadingModel::MSM_Unlit);
	
	bStartSnowGrowth = false;
	bStartSnowFading = false;

	bIsInSnowGrowthMode = false;
	bIsInSnowFadingMode = false;

	PostProcessMaterial = UMaterialInstanceDynamic::Create(PostProcessMaterialBase, this);
    PostProcessMaterial->ShadingModels = FMaterialShadingModelField(EMaterialShadingModel::MSM_Unlit);
	PostProcessMaterial->SetScalarParameterValue("OffSet White Line", FMath::RandRange(0, 50));
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
		
		ElapsedTimeSinceSnowTransitionStart += DeltaTime;

		const auto TransitionTime = ParentDesk->bIsSwitchingFloors ? FloorSwitchModeSnowTransitionTime : BlendingModeSnowTransitionTime;
		const auto OpacityRatio = DeltaTime * (MaxSnowOpacity - MinSnowOpacity) / TransitionTime;
			
		float CurrentOpacity;
		SnowMaterial->GetScalarParameterValue(FName("Opacity"), CurrentOpacity);
		
		if(bIsInSnowFadingMode)
		{
			SnowMaterial->SetScalarParameterValue("Opacity",  FMath::Max(MinSnowOpacity, CurrentOpacity - OpacityRatio));
		}
		else if(bIsInSnowGrowthMode)
		{
			SnowMaterial->SetScalarParameterValue("Opacity", FMath::Min(MaxSnowOpacity ,CurrentOpacity + OpacityRatio));
		}

		if(ElapsedTimeSinceSnowTransitionStart >= TransitionTime)
		{
			bool bWasOn = bIsInSnowGrowthMode;
			bIsInSnowFadingMode = false;
			bIsInSnowGrowthMode = false;
			ElapsedTimeSinceSnowTransitionStart = 0.f;
			OnTransitionEnd.Broadcast(bWasOn);
		}
	}

	ElapsedTime += DeltaTime;

	if(ElapsedTime >= 1.f / 30.f)
	{
		ElapsedTime = 0.f;

		if(!bIsInSnowGrowthMode)
		{
			for(auto* Camera : GameState->Cameras[FloorIndex].FloorZones[ZoneIndex].ZoneCameras)
			{
				Camera->GetCaptureComponent2D()->CaptureScene();
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

		//Texts
		auto TextSize = Canvas->K2_TextSize(Font, CameraNumberPrefix + FString::FromInt(FloorIndex) + ":1");
		Canvas->DrawText(Font, CameraNumberPrefix + FString::FromInt(FloorIndex) + ":1", RenderTargetWidth - TextSize.X - TextMargin, RenderTargetHeight - TextSize.Y  - TextMargin);
		TextSize = Canvas->K2_TextSize(Font, CameraNumberPrefix + FString::FromInt(FloorIndex) + ":2");
		Canvas->DrawText(Font, CameraNumberPrefix + FString::FromInt(FloorIndex) + ":2", CanvasSize.X - TextSize.X  - TextMargin, RenderTargetHeight - TextSize.Y  - TextMargin);
		TextSize = Canvas->K2_TextSize(Font, CameraNumberPrefix + FString::FromInt(FloorIndex) + ":3");
		Canvas->DrawText(Font, CameraNumberPrefix + FString::FromInt(FloorIndex) + ":3", RenderTargetWidth - TextSize.X - TextMargin, CanvasSize.Y - TextSize.Y  - TextMargin);
		TextSize = Canvas->K2_TextSize(Font, CameraNumberPrefix + FString::FromInt(FloorIndex) + ":4");
		Canvas->DrawText(Font, CameraNumberPrefix + FString::FromInt(FloorIndex) + ":4", CanvasSize.X - TextSize.X  - TextMargin, CanvasSize.Y - TextSize.Y - TextMargin);
		
		//Snow
	    Canvas->K2_DrawMaterial(SnowMaterial, FVector2D(0.f,0.f), FVector2D(RenderTargetWidth,RenderTargetHeight),FVector2D(0.f,0.f));
	    Canvas->K2_DrawMaterial(SnowMaterial, FVector2D(CanvasSize.X - RenderTargetWidth,0.f), FVector2D(RenderTargetWidth,RenderTargetHeight),FVector2D(0.f,0.f));
	    Canvas->K2_DrawMaterial(SnowMaterial, FVector2D(0.f,CanvasSize.Y - RenderTargetHeight), FVector2D(RenderTargetWidth,RenderTargetHeight),FVector2D(0.f,0.f));
	    Canvas->K2_DrawMaterial(SnowMaterial, FVector2D(CanvasSize.X - RenderTargetWidth,CanvasSize.Y - RenderTargetHeight), FVector2D(RenderTargetWidth,RenderTargetHeight),FVector2D(0.f,0.f));

		Canvas->K2_DrawMaterial(PostProcessMaterial, FVector2D(0.f,0.f), FVector2D(ScreenWidth,ScreenWidth * AspectRatio),FVector2D(0.f,0.f));
		
		UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, Context);

	}
}

