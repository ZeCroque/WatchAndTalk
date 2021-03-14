// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniGameMouseWidgetWelding.h"


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
