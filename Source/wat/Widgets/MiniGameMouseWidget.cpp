// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniGameMouseWidget.h"

#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Canvas.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Components/Image.h"
#include "Brushes/SlateImageBrush.h"
#include "Kismet/GameplayStatics.h"

void UMiniGameMouseWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PointList.Empty();
	LocalSize = Image->Brush.ImageSize;
	RenderTarget = NewObject<UTextureRenderTarget2D>(this);
	RenderTarget->InitAutoFormat(LocalSize.X, LocalSize.Y);


	Image->Brush.SetResourceObject(RenderTarget);


	StartPoint = FVector2D(LocalSize.X * 0.1, FMath::RandRange(LocalSize.Y * 0.33f, LocalSize.Y * 0.66f));
	const auto EndOfCourse = LocalSize * 0.90f;

	bool Vertical = false;
	PointList.Add(StartPoint);
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
				Y = FMath::Clamp(PosCursor - Distance, LocalSize.Y * 0.10f, PosCursor);
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
}

void UMiniGameMouseWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	auto* Canvas = GetWorld()->GetCanvasForDrawMaterialToRenderTarget();
	FVector2D CanvasSize;
	FDrawToRenderTargetContext Context;;
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(this, RenderTarget, Canvas, CanvasSize, Context);


	for (int i = 1; i < PointList.Num(); ++i)
	{
		Canvas->K2_DrawLine(PointList[i - 1], PointList[i], 10, FLinearColor::Red);
	}

	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, Context);
}
