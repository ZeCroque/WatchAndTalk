// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GridManager.h"

// Sets default values
AGridManager::AGridManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Grid Component"));
	BoxComponent->SetupAttachment(RootComponent);
	BoxComponent->SetHiddenInGame(true);

	SumOfWeight = 0;
}

// Called when the game starts or when spawned
void AGridManager::BeginPlay()
{
	Super::BeginPlay();

	Grid.Init(0.f, CellPerSide * CellPerSide * 2);

	MinWorld = FVector2D(BoxComponent->GetNavigationBounds().Min);
	MaxWorld = FVector2D(BoxComponent->GetNavigationBounds().Max);

	CellHeight = (MaxWorld.Y - MinWorld.Y) / CellPerSide;
	CellWidth = (MaxWorld.X - MinWorld.X) / CellPerSide;
}

// Called every frame
void AGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (int i = 0; i < CellPerSide; i++)
	{
		for (int j = 0; j < CellPerSide; j++)
		{
			DecrementValueAt(j, i, CoolingStrength * DeltaTime);
		}
	}

	for (int i = CellPerSide; i < CellPerSide * 2; i++)
	{
		for (int j = 0; j < CellPerSide; j++)
		{
			DecrementValueAt(j, i, CoolingStrength * DeltaTime);
		}
	}
	// GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Blue, FString::FromInt(SumOfWeight));
}


float AGridManager::GetValueAt(const int X, const int Y)
{
	return Grid[X + Y * CellPerSide];
}

void AGridManager::SetValueAt(const int X, const int Y, const float Value)
{
	SumOfWeight -= Grid[X + Y * CellPerSide];
	SumOfWeight += Value;
	Grid[X + Y * CellPerSide] = Value;
}

void AGridManager::DecrementValueAt(const int X, const int Y, const float Value)
{
	const auto ClampValue = FMath::Max(0.0f, Grid[X + Y * CellPerSide] - Value);
	SumOfWeight -= Grid[X + Y * CellPerSide];
	SumOfWeight += ClampValue;
	Grid[X + Y * CellPerSide] = ClampValue;
}

void AGridManager::SetValueAround(const FVector& Center, const int Ray)
{
	// j -> X
	// i -> y

	const FVector2D GridCoordinate = GetGridCoordinate(Center);

	int IStart = 0;
	int IEnd = CellPerSide;
	if (Center.Z >= FloorHeight)
	{
		IStart = CellPerSide;
		IEnd = CellPerSide * 2;
	}

	for (int i = IStart; i < IEnd; i++)
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

bool AGridManager::RandomHotWorldCoordinate(FVector& Vector) //TODO : Test
{
	if (SumOfWeight > 0)
	{
		float Rnd;
		do
		{
			Rnd = FMath::RandRange(0.0f, SumOfWeight);
		}
		while (Rnd == 0.f);

		for (int i = 0; i < Grid.Num(); i++)
		{
			if (Rnd < Grid[i])
			{
				const int x = i % CellPerSide;
				const int y = i / CellPerSide;

				Vector = GetWorldCoordinate(FVector2D(x, y));
				return true;
			}
			Rnd -= Grid[i];
		}
	}

	return false;
}

FVector2D AGridManager::GetGridCoordinate(const FVector& WorldCoordinate) const
{
	const int ZOffset = WorldCoordinate.Z < FloorHeight ? 0 : CellPerSide;

	const int GridX = static_cast<int>((WorldCoordinate.X - MinWorld.X) / CellWidth);
	const int GridY = static_cast<int>((WorldCoordinate.Y - MinWorld.Y) / CellHeight) + ZOffset;

	return FVector2D(GridX, GridY);
}

FVector AGridManager::GetWorldCoordinate(const FVector2D& GridCoordinate) const
{
	const float WorldX = GridCoordinate.X * CellWidth + MinWorld.X + CellWidth / 2.f;
	float WorldY, WorldZ;

	if (GridCoordinate.Y < CellPerSide)
	{
		WorldZ = GroundHeight;
		WorldY = GridCoordinate.Y * CellHeight + MinWorld.Y + CellHeight / 2.f;
	}
	else
	{
		WorldZ = FloorHeight;
		WorldY = (GridCoordinate.Y - CellPerSide) * CellHeight + MinWorld.Y + CellHeight / 2.f;
	}

	return FVector(WorldX, WorldY, WorldZ);
}
