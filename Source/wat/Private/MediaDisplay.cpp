// Fill out your copyright notice in the Description page of Project Settings.


#include "MediaDisplay.h"


#include "Components/SceneCaptureComponent2D.h"
#include "Kismet/GameplayStatics.h"
#include "MainGameMode.h"
#include "Materials/MaterialInstanceDynamic.h"

AMediaDisplay::AMediaDisplay()
{
	PrimaryActorTick.bCanEverTick = true;
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Renderer"));

	Screen = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Screen"));
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ScreenPath(TEXT("StaticMesh'/Game/Developers/ZeCroque/Assets/Meshes/Static/Geometry/Plane.Plane'"));
	Screen->SetStaticMesh(ScreenPath.Object);

	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialPath(TEXT("Material'/Game/Developers/ZeCroque/Assets/Materials/Screen/ScreenMaterial.ScreenMaterial'"));
	Screen->SetMaterial(0, Cast<UMaterialInterface>(MaterialPath.Object));
}

void AMediaDisplay::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ScreenMaterial = Screen->CreateAndSetMaterialInstanceDynamic(0);
	ScreenMaterial->ShadingModels = FMaterialShadingModelField(EMaterialShadingModel::MSM_Unlit);
	
	RenderTarget = NewObject<UTextureRenderTarget2D>(this);
	RenderTarget->InitAutoFormat(512, 512); //TODO scale to mesh size
}

void AMediaDisplay::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> Cameras;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASceneCapture2D::StaticClass(), Cameras); //TODO find proper way for cameras

	Camera = Cast<ASceneCapture2D>(Cameras[0])->GetCaptureComponent2D();
	Camera->TextureTarget = RenderTarget;
	ScreenMaterial->SetTextureParameterValue(TEXT("RenderTexture"), Cast<UTexture>(RenderTarget));
}
