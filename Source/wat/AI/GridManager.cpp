// Fill out your copyright notice in the Description page of Project Settings.


#include "wat/AI/GridManager.h"


#include "NavModifierVolume.h"
#include "Containers/Array.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AGridManager::AGridManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Grid Component"));
	BoxComponent->SetupAttachment(RootComponent);
	BoxComponent->SetHiddenInGame(true);
}

// Called when the game starts or when spawned
void AGridManager::BeginPlay()
{
	Super::BeginPlay();

	Grid.Init(0.0f, CellPerSide * CellPerSide);

	MinWorld = FVector2D(BoxComponent->GetNavigationBounds().Min);
	MaxWorld = FVector2D(BoxComponent->GetNavigationBounds().Max);

	CellHeight = (MaxWorld.Y - MinWorld.Y) / CellPerSide;
	CellWidth = (MaxWorld.X - MinWorld.X) / CellPerSide;

	Timer = CoolingInterval;


	TArray<AActor*> NavModifierVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANavModifierVolume::StaticClass(), NavModifierVolumes);

	for (auto NavModifierVolume : NavModifierVolumes)
	{
		FVector Origin;
		FVector BoxExtent;
		NavModifierVolume->GetActorBounds(false, Origin, BoxExtent);

		FVector A = FVector((Origin - BoxExtent).X, (Origin - BoxExtent).Y, (Origin - BoxExtent).Z);
		FVector B = FVector((Origin + BoxExtent).X, (Origin - BoxExtent).Y, (Origin - BoxExtent).Z);
		FVector C = FVector(B.X, Origin.Y + BoxExtent.Y, (Origin - BoxExtent).Z);
		FVector D = FVector(A.X, C.Y, (Origin - BoxExtent).Z);

		TArray<FVector> NavModifierPolygon;
		NavModifierPolygon.Add(A);
		NavModifierPolygon.Add(B);
		NavModifierPolygon.Add(C);
		NavModifierPolygon.Add(D);

		SetValueInPolygon(NavModifierPolygon, -1);
	}
}

// Called every frame
void AGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Timer += DeltaTime;
	if (Timer >= CoolingInterval)
	{
		Timer = 0;

		for (int i = 0; i < CellPerSide; i++)
		{
			for (int j = 0; j < CellPerSide; j++)
			{
				DecrementValueAt(j, i, CoolingStrength);
			}
		}
	}
}


float AGridManager::GetValueAt(const int X, const int Y)
{
	return Grid[X + Y * CellPerSide];
}

void AGridManager::SetValueAt(const int X, const int Y, const float Value)
{
	if (Grid[X + Y * CellPerSide] != -1)
	{
		Grid[X + Y * CellPerSide] = Value;
	}
}

void AGridManager::DecrementValueAt(const int X, const int Y, const float Value)
{
	if (Grid[X + Y * CellPerSide] != -1)
	{
		const auto ClampValue = FMath::Max(0.0f, Grid[X + Y * CellPerSide] - Value);
		Grid[X + Y * CellPerSide] = ClampValue;
	}
}

void AGridManager::SetValueAround(const FVector& Center, const int Ray)
{
	// j -> X
	// i -> y

	const FVector2D GridCoordinate = GetGridCoordinate(Center);

	for (int i = 0; i < CellPerSide; i++)
	{
		for (int j = 0; j < CellPerSide; j++)
		{
			if (((j - GridCoordinate.X) * (j - GridCoordinate.X) + (i - GridCoordinate.Y) * (i - GridCoordinate.Y)) <=
				Ray * Ray)
			{
				SetValueAt(j, i, 1.0f);
			}
		}
	}
}

float AGridManager::GetValueAround(const FVector& Center, const int Ray)
{
	// j -> X
	// i -> y

	const FVector2D GridCoordinate = GetGridCoordinate(Center);

	float AverageValue = 0;
	int ValueCount = 0;
	for (int i = 0; i < CellPerSide; ++i)
	{
		for (int j = 0; j < CellPerSide; ++j)
		{
			if (((j - GridCoordinate.X) * (j - GridCoordinate.X) + (i - GridCoordinate.Y) * (i - GridCoordinate.Y)) <=
				Ray * Ray)
			{
				++ValueCount;
				AverageValue = AverageValue + GetValueAt(j, i) != -1 ? GetValueAt(j, i) : 0;
			}
		}
	}
	return AverageValue / static_cast<float>(ValueCount);
}

void AGridManager::SetValueOutside(const FVector& Center, const int Ray)
{
	// j -> X
	// i -> y

	const FVector2D GridCoordinate = GetGridCoordinate(Center);

	for (int i = 0; i < CellPerSide; i++)
	{
		for (int j = 0; j < CellPerSide; j++)
		{
			if (!(((j - GridCoordinate.X) * (j - GridCoordinate.X) + (i - GridCoordinate.Y) * (i - GridCoordinate.Y)) <=
				Ray * Ray))
			{
				SetValueAt(j, i, 0.0f);
			}
		}
	}
}

void AGridManager::SetValueInPolygon(const TArray<FVector>& WorldPolygon, const float Value)
{
	int XMin = CellPerSide + 1;
	int XMax = -1;
	int YMin = CellPerSide + 1;
	int YMax = -1;

	TArray<FVector2D> GridPolygon;
	for (auto WorldPoint : WorldPolygon)
	{
		auto GridPoint = GetGridCoordinate(WorldPoint);

		XMin = GridPoint.X < XMin ? GridPoint.X : XMin;
		XMax = GridPoint.X > XMax ? GridPoint.X : XMax;
		YMin = GridPoint.Y < YMin ? GridPoint.Y : YMin;
		YMax = GridPoint.Y > YMax ? GridPoint.Y : YMax;

		GridPolygon.Add(GridPoint);
	}

	for (int i = FMath::Max(YMin, 0); i < FMath::Min(YMax, CellPerSide); i++)
	{
		for (int j = FMath::Max(XMin, 0); j < FMath::Min(XMax, CellPerSide); j++)
		{
			if (IsInsidePolygon(GridPolygon, FVector2D(j, i)))
			{
				SetValueAt(j, i, Value);
			}
		}
	}
}

bool AGridManager::RandomHotWorldCoordinate(FVector& Vector)
{
	float SumOfWeight = 0;
	for (auto Element : Grid)
	{
		SumOfWeight += FMath::Max(Element, 0.0f);
	}

	if (SumOfWeight > 0)
	{
		float Rnd;
		do
		{
			Rnd = FMath::RandRange(0.0f, SumOfWeight);
		}
		while (Rnd == 0.f);


		float Cursor = 0;
		for (int i = 0; i < CellPerSide; i++)
		{
			for (int j = 0; j < CellPerSide; j++)
			{
				Cursor += GetValueAt(j, i) != -1 ? GetValueAt(j, i) : 0;
				if (Cursor >= Rnd)
				{
					Vector = GetWorldCoordinate(FVector2D(j, i));
					return true;
				}
			}
		}
	}

	return false;
}

bool AGridManager::SmartRandomHotWorldCoordinate(FVector& Vector, const int Try, const int Ray)
{
	float BestWeight = 0;
	bool bHaveFindCoordinate = false;

	for (int i = 0; i < Try; i++)
	{
		FVector ActualCoordinate;
		bHaveFindCoordinate = RandomHotWorldCoordinate(ActualCoordinate);
		if (bHaveFindCoordinate)
		{
			const float ActualWeight = GetValueAround(ActualCoordinate, Ray);
			if (ActualWeight > BestWeight)
			{
				BestWeight = ActualWeight;
				Vector = ActualCoordinate;
			}
		}
	}

	return bHaveFindCoordinate;
}

FVector2D AGridManager::GetGridCoordinate(const FVector& WorldCoordinate) const
{
	const int GridX = static_cast<int>((WorldCoordinate.X - MinWorld.X) / CellWidth);
	const int GridY = static_cast<int>((WorldCoordinate.Y - MinWorld.Y) / CellHeight);

	return FVector2D(GridX, GridY);
}

FVector AGridManager::GetWorldCoordinate(const FVector2D& GridCoordinate) const
{
	const float WorldX = GridCoordinate.X * CellWidth + MinWorld.X + CellWidth / 2.f;
	const float WorldY = GridCoordinate.Y * CellHeight + MinWorld.Y + CellHeight / 2.f;
	const float WorldZ = GroundHeight;

	return FVector(WorldX, WorldY, WorldZ);
}


bool AGridManager::IsInsidePolygon(const TArray<FVector2D>& Polygon, const FVector2D& Point)
{
	int i, j;
	auto bIsInside = false;
	const int Size = Polygon.Num();

	for (i = 0, j = Size - 1; i < Size; j = i++)
	{
		if (((Polygon[i].Y >= Point.Y) != (Polygon[j].Y >= Point.Y)) &&
			(Point.X <= (Polygon[j].X - Polygon[i].X) * (Point.Y - Polygon[i].Y) / (Polygon[j].Y - Polygon[i].Y) +
				Polygon[i].X)
		)
			bIsInside = !bIsInside;
	}

	return bIsInside;
};
