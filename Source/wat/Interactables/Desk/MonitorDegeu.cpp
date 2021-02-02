// Fill out your copyright notice in the Description page of Project Settings.


#include "MonitorDegeu.h"


#include "SecurityCamera.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Core/MainGameState.h"
#include "Engine/Canvas.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"

AMonitorDegeu::AMonitorDegeu()
{
      ScreenMesh1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ScreenMesh1"));
      ScreenMesh1->SetupAttachment(MonitorMesh);
      ScreenMesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ScreenMesh2"));
      ScreenMesh2->SetupAttachment(MonitorMesh);
      ScreenMesh3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ScreenMesh3"));
      ScreenMesh3->SetupAttachment(MonitorMesh);
}

void AMonitorDegeu::BeginPlay()
{
    Super::BeginPlay();

    auto* GameState = Cast<AMainGameState>(GetWorld()->GetGameState());

    ScreenMaterial1 = ScreenMesh1->CreateAndSetMaterialInstanceDynamic(0);
    ScreenMaterial1->ShadingModels = FMaterialShadingModelField(EMaterialShadingModel::MSM_Unlit);

    RenderTarget1 = NewObject<UTextureRenderTarget2D>(this);
    RenderTarget1->InitAutoFormat(512, 512);

    RenderSourceCamera1 = GameState->Cameras[DebugZoneIndex].ZoneCameras[1]->GetCaptureComponent2D();
    RenderSourceCamera1->TextureTarget = RenderTarget1;
    RenderSourceCamera1->SetVisibleFlag(false);
   // ScreenMaterial1->SetTextureParameterValue(TEXT("RenderTexture"), Cast<UTexture>(RenderTarget1));


  
    ScreenMaterial2 = ScreenMesh2->CreateAndSetMaterialInstanceDynamic(0);
    ScreenMaterial2->ShadingModels = FMaterialShadingModelField(EMaterialShadingModel::MSM_Unlit);

    RenderTarget2 = NewObject<UTextureRenderTarget2D>(this);
    RenderTarget2->InitAutoFormat(512, 512);

    RenderSourceCamera2 = GameState->Cameras[DebugZoneIndex].ZoneCameras[2]->GetCaptureComponent2D();
    RenderSourceCamera2->TextureTarget = RenderTarget2;
    RenderSourceCamera2->SetVisibleFlag(false);
    ScreenMaterial2->SetTextureParameterValue(TEXT("RenderTexture"), Cast<UTexture>(RenderTarget2));



    ScreenMaterial3 = ScreenMesh3->CreateAndSetMaterialInstanceDynamic(0);
    ScreenMaterial3->ShadingModels = FMaterialShadingModelField(EMaterialShadingModel::MSM_Unlit);

    RenderTarget3 = NewObject<UTextureRenderTarget2D>(this);
    RenderTarget3->InitAutoFormat(512, 512);

    RenderSourceCamera3 = GameState->Cameras[DebugZoneIndex].ZoneCameras[3]->GetCaptureComponent2D();
    RenderSourceCamera3->TextureTarget = RenderTarget3;
    RenderSourceCamera3->SetVisibleFlag(false);
    ScreenMaterial3->SetTextureParameterValue(TEXT("RenderTexture"), Cast<UTexture>(RenderTarget3));

  
    NewRenderTarget1 = NewObject<UTextureRenderTarget2D>(this);
    NewRenderTarget1->InitAutoFormat(1024, 1024);

    auto* Canvas  = GetWorld()->GetCanvasForDrawMaterialToRenderTarget();
    auto CanvasSize = FVector2D();
    auto Context = FDrawToRenderTargetContext();
    UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(this, NewRenderTarget1, Canvas, CanvasSize, Context);
    Canvas->K2_DrawMaterial(ScreenMaterial1, FVector2D(), FVector2D(512.f,512.f),FVector2D());
    Canvas->K2_DrawMaterial(ScreenMaterial1, FVector2D(512.f,0.f), FVector2D(512.f,512.f),FVector2D());
    Canvas->K2_DrawMaterial(ScreenMaterial1, FVector2D(0.f,512.f), FVector2D(512.f,512.f),FVector2D());
    Canvas->K2_DrawMaterial(ScreenMaterial1, FVector2D(512.f,512.f), FVector2D(512.f,512.f),FVector2D());
    //TODO actually draw
    
    UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, Context);
    ScreenMaterial1->SetTextureParameterValue(TEXT("RenderTexture"), Cast<UTexture>(NewRenderTarget1));
}
