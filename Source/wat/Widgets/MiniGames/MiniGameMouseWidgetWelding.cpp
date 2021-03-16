// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniGameMouseWidgetWelding.h"

#include "Engine/Canvas.h"
#include "Kismet/KismetRenderingLibrary.h"


void UMiniGameMouseWidgetWelding::Start()
{
	Super::Start();

	StartPoint = FVector2D(LocalSize.X * 0.25, FMath::RandRange(LocalSize.Y * 0.33f, LocalSize.Y * 0.66f));
	const auto EndOfCourse = LocalSize * 0.75f;

	bool Vertical = false;
	PointList.Add(StartPoint);
	DrawPointList.Add(StartPoint);
	FVector2D LastPoint = StartPoint;


	do
	{
		if (Vertical)
		{
			const auto PosCursor = LastPoint.Y;
			const auto Distance = FMath::RandRange(LocalSize.Y * 0.10f, LocalSize.Y * 0.20f);

			float Y;
			if (FMath::RandBool())
			{
				Y = FMath::Clamp(PosCursor - Distance, LocalSize.Y * 0.20f, PosCursor);
			}
			else
			{
				Y = FMath::Clamp(PosCursor + Distance, PosCursor, EndOfCourse.Y);
			}

			LastPoint = FVector2D(LastPoint.X, Y);
			PointList.Add(LastPoint);
		}
		else
		{
			const auto PosCursor = LastPoint.X;
			const auto Distance = FMath::RandRange(LocalSize.X * 0.10f, LocalSize.X * 0.20f);
			const auto X = FMath::Clamp(PosCursor + Distance, PosCursor, EndOfCourse.X);

			LastPoint = FVector2D(X, LastPoint.Y);
			PointList.Add(LastPoint);
		}
		Vertical = !Vertical;
	}
	while (LastPoint.X < EndOfCourse.X);

	bIsClicking = false;
	LastMouseMove.Set(0, 0);
	OldMouseMove = StartPoint;
	FirstDrawPointList = PointList;
}

void UMiniGameMouseWidgetWelding::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
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
		Canvas->K2_DrawLine(FirstDrawPointList[i - 1], FirstDrawPointList[i], 10, WayColor);
	}

	for (int i = 1; i < DrawPointList.Num(); ++i)
	{
		Canvas->K2_DrawLine(DrawPointList[i - 1], DrawPointList[i], ProgressRange, ProgressColor);
	}

	Canvas->K2_DrawTexture(
		ToolTexture,
		OldMouseMove - FVector2D(ToolTexture->GetSurfaceWidth(), ToolTexture->GetSurfaceHeight()) - TmpWeldingGunOffset,
		FVector2D(ToolTexture->GetSurfaceWidth(), ToolTexture->GetSurfaceHeight()), FVector2D(0, 0));

	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, Context);


	if (PointList.Num() == 1 || OldPoint.X < PointList[0].X)
	{
		DirectionState = EDirection::Forward;
	}
	else if (OldPoint.Y < PointList[0].Y)
	{
		DirectionState = EDirection::Down;
	}
	else if (OldPoint.Y > PointList[0].Y)
	{
		DirectionState = EDirection::Up;
	}


	bool bHasReached = false;
	switch (DirectionState)
	{
	case EDirection::Up:
		if (FMath::Abs(PointList[0].Y - OldMouseMove.Y) <= ProgressRange)
		{
			bHasReached = true;
			break;
		}
		if (OldMouseMove.Y - OldPoint.Y > ProgressRange || FMath::Abs(PointList[0].X - OldMouseMove.X) > ProgressRange)
			OnFail();
		break;
	case EDirection::Down:
		if (FMath::Abs(PointList[0].Y - OldMouseMove.Y) <= ProgressRange)
		{
			bHasReached = true;
			break;
		}
		if (OldPoint.Y - OldMouseMove.Y > ProgressRange || FMath::Abs(PointList[0].X - OldMouseMove.X) > ProgressRange)
			OnFail();
		break;
	case EDirection::Forward:
		if (FMath::Abs(PointList[0].X - OldMouseMove.X) <= ProgressRange)
		{
			bHasReached = true;
			break;
		}
		if (OldPoint.X - OldMouseMove.X > ProgressRange || FMath::Abs(PointList[0].Y - OldMouseMove.Y) > ProgressRange)
			OnFail();
		break;
	}

	if (bHasReached)
	{
		OldPoint = PointList[0];
		PointList.RemoveAt(0);
	}

	if (PointList.Num() == 0)
	{
		OnComplete();
	}
}
