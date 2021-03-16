// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniGameMouseWidgetAdn.h"

#include "Engine/Canvas.h"
#include "Kismet/KismetRenderingLibrary.h"

void UMiniGameMouseWidgetAdn::Start()
{
    Super::Start();

    StartPoint = FVector2D(LocalSize.X * 0.25, FMath::RandRange(LocalSize.Y * 0.33f, LocalSize.Y * 0.66f));
    const auto EndOfCourse = LocalSize * 0.75f;

    const int Size = EndOfCourse.X - StartPoint.X;
    const int Height = FMath::RandRange(LocalSize.Y * 0.05f, LocalSize.Y * 0.1f);
    const float RatioPeriod = Size * FMath::RandRange(0.09f, 0.13f);

    for (int i = 0; i < Size; ++i)
    {
        if (!(i % 3))
        {
            PointList.Add(StartPoint + FVector2D(i, Height * FMath::Cos(i / RatioPeriod)));
            ShadowPointList.Add(StartPoint + FVector2D(i, -(Height * FMath::Cos(i / RatioPeriod))));
        }
    }

    bIsClicking = false;
    LastMouseMove.Set(0, 0);
    OldMouseMove = PointList[0];
    FirstDrawPointList = PointList;
}

void UMiniGameMouseWidgetAdn::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    auto TmpWeldingGunOffset = ToolTextureOffset;
    if (bIsClicking)
    {
        TmpWeldingGunOffset = FVector2D(0, 0);
        DrawPointList.Add(OldMouseMove + LastMouseMove);
        OldMouseMove = OldMouseMove + LastMouseMove;
    }

    auto* Canvas = GetWorld()->GetCanvasForDrawMaterialToRenderTarget();
    FVector2D CanvasSize;
    FDrawToRenderTargetContext Context;

    UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(this, RenderTarget, Canvas, CanvasSize, Context);


    Canvas->K2_DrawTexture(BackGround, FVector2D(0, 0), LocalSize, FVector2D(0, 0));

    for (int i = 1; i < FirstDrawPointList.Num(); ++i)
    {
        if (i < FirstDrawPointList.Num() - PointList.Num())
        {
            if (!(i % StrandRatio))
            {
                Canvas->K2_DrawLine(ShadowPointList[i - 1], FirstDrawPointList[i - 1], 8, ProgressColor);
            }
            Canvas->K2_DrawLine(FirstDrawPointList[i - 1], FirstDrawPointList[i], 10, ProgressColor);
            Canvas->K2_DrawLine(ShadowPointList[i - 1], ShadowPointList[i], 10, ProgressColor);
        }
        else
        {
            Canvas->K2_DrawLine(FirstDrawPointList[i - 1], FirstDrawPointList[i], 10, WayColor);
            Canvas->K2_DrawLine(ShadowPointList[i - 1], ShadowPointList[i], 10, ProgressColor);
        }
    }

    Canvas->K2_DrawTexture(
        ToolTexture,
        OldMouseMove - FVector2D(ToolTexture->GetSurfaceWidth(), ToolTexture->GetSurfaceHeight()) - TmpWeldingGunOffset,
        FVector2D(ToolTexture->GetSurfaceWidth(), ToolTexture->GetSurfaceHeight()), FVector2D(0, 0));

    UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, Context);

    for (int i = 0; i < PointList.Num(); ++i)
    {
        if (FVector2D::Distance(OldMouseMove, PointList[i]) <= ProgressRange)
        {
            OldPoint = PointList[i];
            PointList.RemoveAt(i);
            i--;
        }
    }


    if (PointList.Num() == 0)
    {
        OnComplete();
    }
    else if (FVector2D::Distance(PointList[0], OldMouseMove) > ProgressRange * ErrorToleranceRatio)
    {
        OnFail();
    }
}
