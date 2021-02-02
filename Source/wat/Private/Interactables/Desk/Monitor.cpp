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

	DebugZoneIndex = 0;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	
	MonitorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MonitorMesh"));
	MonitorMesh->SetupAttachment(RootComponent);
	
	ScreenMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ScreenMesh"));
	ScreenMesh->SetupAttachment(MonitorMesh);
	
	BlendTargetCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("BlendTargetCamera"));
	BlendTargetCamera->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AMonitor::BeginPlay()
{
	Super::BeginPlay();

	auto* GameState = Cast<AMainGameState>(GetWorld()->GetGameState());

        const auto CameraCount = GameState->Cameras[DebugZoneIndex].ZoneCameras.Num();
        RenderSourceCameras.Reserve(CameraCount);
        CameraRenderMaterials.Reserve(CameraCount);
        CameraRenderTargets.Reserve(CameraCount);
        for(auto* SourceCamera : GameState->Cameras[DebugZoneIndex].ZoneCameras) 
	{
	    auto* RenderMaterial = UMaterialInstanceDynamic::Create(BaseRenderMaterial, this);
            RenderMaterial->ShadingModels = FMaterialShadingModelField(EMaterialShadingModel::MSM_Unlit);
	    CameraRenderMaterials.Add(RenderMaterial);

	    auto* RenderTarget = NewObject<UTextureRenderTarget2D>(this);
            RenderTarget->InitAutoFormat(512, 512);
            CameraRenderTargets.Add(RenderTarget);

            auto* SceneCapture = SourceCamera->GetCaptureComponent2D();
            SceneCapture->TextureTarget = RenderTarget;
	    SceneCapture->SetVisibleFlag(true);
            RenderSourceCameras.Add(SceneCapture);

	    RenderMaterial->SetTextureParameterValue(TEXT("RenderTexture"), Cast<UTexture>(RenderTarget));
        }

	ScreenMaterial = ScreenMesh->CreateAndSetMaterialInstanceDynamic(0);
	ScreenMaterial->ShadingModels = FMaterialShadingModelField(EMaterialShadingModel::MSM_Unlit);

	ScreenRenderTarget = NewObject<UTextureRenderTarget2D>(this);
	ScreenRenderTarget->InitAutoFormat(1024, 1024);

	ScreenMaterial->SetTextureParameterValue(TEXT("RenderTexture"), Cast<UTexture>(ScreenRenderTarget));
}

// Called every frame
void AMonitor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

        auto* Canvas  = GetWorld()->GetCanvasForDrawMaterialToRenderTarget();
        auto CanvasSize = FVector2D();
        auto Context = FDrawToRenderTargetContext();
        UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(this, ScreenRenderTarget, Canvas, CanvasSize, Context);
        Canvas->K2_DrawMaterial(CameraRenderMaterials[0], FVector2D(0.f,0.f), FVector2D(512.f,512.f),FVector2D(0.f,0.f));
        Canvas->K2_DrawMaterial(CameraRenderMaterials[1], FVector2D(512.f,0.f), FVector2D(512.f,512.f),FVector2D(0.f,0.f));
        Canvas->K2_DrawMaterial(CameraRenderMaterials[2], FVector2D(0.f,512.f), FVector2D(512.f,512.f),FVector2D(0.f,0.f));
        Canvas->K2_DrawMaterial(CameraRenderMaterials[3], FVector2D(512.f,512.f), FVector2D(512.f,512.f),FVector2D(0.f,0.f));
        UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, Context);
}

