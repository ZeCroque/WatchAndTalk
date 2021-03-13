// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniGameMouseWidget.h"

#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Canvas.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Components/Image.h"
#include "wat/Core/WatPlayerController.h"
#include "wat/GameplayActors/Interactables/Objectives/Objective.h"


void UMiniGameMouseWidget::Start()
{
	Super::Start();

	PointList.Empty();
	LocalSize = Image->Brush.ImageSize;
	RenderTarget = NewObject<UTextureRenderTarget2D>(this);
	RenderTarget->InitAutoFormat(LocalSize.X, LocalSize.Y);
	ParentInteractable->PlayerController->bShowMouseCursor = true;
	SetUserFocus(ParentInteractable->PlayerController);

	Image->Brush.SetResourceObject(RenderTarget);


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

void UMiniGameMouseWidget::Stop()
{
	ParentInteractable->PlayerController->bShowMouseCursor = false;
	Super::Stop();
}

FReply UMiniGameMouseWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bIsClicking)
	{
		LastMouseMove = (InMouseEvent.GetScreenSpacePosition() - MousePosition) / 250;
	}

	return FReply::Handled();
}

FReply UMiniGameMouseWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	bIsClicking = true;
	MousePosition = InMouseEvent.GetScreenSpacePosition();
	return FReply::Handled();
}

FReply UMiniGameMouseWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	bIsClicking = false;
	return FReply::Handled();
}

void UMiniGameMouseWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);


	if (bIsClicking)
	{
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
		Canvas->K2_DrawLine(FirstDrawPointList[i - 1], FirstDrawPointList[i], 10, FLinearColor::Red);
	}

	for (int i = 1; i < DrawPointList.Num(); ++i)
	{
		Canvas->K2_DrawLine(DrawPointList[i - 1], DrawPointList[i], WeldingRange, FLinearColor::White);
	}

	Canvas->K2_DrawTexture(
		WeldingGun, OldMouseMove - FVector2D(WeldingGun->GetSurfaceWidth(), WeldingGun->GetSurfaceHeight()),
		FVector2D(WeldingGun->GetSurfaceWidth(), WeldingGun->GetSurfaceHeight()), FVector2D(0, 0));

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
		if (FMath::Abs(PointList[0].Y - OldMouseMove.Y) <= WeldingRange)
		{
			bHasReached = true;
			break;
		}
		if (OldMouseMove.Y - OldPoint.Y > WeldingRange || FMath::Abs(PointList[0].X - OldMouseMove.X) > WeldingRange)
			OnFail();
		break;
	case EDirection::Down:
		if (FMath::Abs(PointList[0].Y - OldMouseMove.Y) <= WeldingRange)
		{
			bHasReached = true;
			break;
		}
		if (OldPoint.Y - OldMouseMove.Y > WeldingRange || FMath::Abs(PointList[0].X - OldMouseMove.X) > WeldingRange)
			OnFail();
		break;
	case EDirection::Forward:
		if (FMath::Abs(PointList[0].X - OldMouseMove.X) <= WeldingRange)
		{
			bHasReached = true;
			break;
		}
		if (OldPoint.X - OldMouseMove.X > WeldingRange || FMath::Abs(PointList[0].Y - OldMouseMove.Y) > WeldingRange)
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
