// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Desk/Monitor.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Core/MainGameState.h"
#include "SecurityCamera.h"

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

	ScreenMaterial = ScreenMesh->CreateAndSetMaterialInstanceDynamic(0);
	ScreenMaterial->ShadingModels = FMaterialShadingModelField(EMaterialShadingModel::MSM_Unlit);

	RenderTarget = NewObject<UTextureRenderTarget2D>(this);
	RenderTarget->InitAutoFormat(512, 512);

	auto* GameState = Cast<AMainGameState>(GetWorld()->GetGameState());

	RenderSourceCamera = GameState->Cameras[DebugZoneIndex].ZoneCameras[0]->GetCaptureComponent2D();
	RenderSourceCamera->TextureTarget = RenderTarget;
	ScreenMaterial->SetTextureParameterValue(TEXT("RenderTexture"), Cast<UTexture>(RenderTarget));
}

// Called every frame
void AMonitor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

