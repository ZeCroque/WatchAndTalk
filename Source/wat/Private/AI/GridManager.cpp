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

	Grid.Init(0.f, CellPerSide * CellPerSide * 2);
}

// Called when the game starts or when spawned
void AGridManager::BeginPlay()
{
	Super::BeginPlay();

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

	for (int i = 0; i < CellPerSide; i++)
	{
		for (int j = CellPerSide; j < CellPerSide * 2; j++)
		{
			DecrementValueAt(j, i, CoolingStrength * DeltaTime);
		}
	}
}


float AGridManager::GetValueAt(const int X, const int Y)
{
	return Grid[X + Y * CellPerSide];
}

void AGridManager::SetValueAt(const int X, const int Y, const float Value)
{
	Grid[X + Y * CellPerSide] = Value;
}

void AGridManager::DecrementValueAt(const int X, const int Y, const float Value)
{
	Grid[X + Y * CellPerSide] = FMath::Max(0.0f, Grid[X + Y * CellPerSide] - Value);
}

void AGridManager::SetValueAround(const FVector Center, const int Ray)
{
	// j -> X
	// i -> y

	const FVector2D GridCoordinate = GetGridCoordinate(Center);
	const int Start_X = GridCoordinate.X;
	const int Start_Y = GridCoordinate.Y;

	int JStart, JEnd;
	if (Center.Z < FloorHeight)
	{
		JStart = 0;
		JEnd = CellPerSide;
	}
	else
	{
		JStart = CellPerSide;
		JEnd = CellPerSide * 2;
	}

	for (int i = 0; i < CellPerSide; i++)
	{
		for (int j = JStart; j < JEnd; j++)
		{
			if (((j - Start_X) * (j - Start_X) + (i - Start_Y) * (i - Start_Y)) <= Ray * Ray)
			{
				SetValueAt(j, j, 1.0f);
			}
		}
	}
}

FVector2D AGridManager::GetGridCoordinate(const FVector WorldCoordinate) const
{
	const int GridY = static_cast<int>((WorldCoordinate.Y - MinWorld.Y) / CellHeight);

	int Offset;
	if (WorldCoordinate.Z < FloorHeight)
	{
		Offset = 0;
	}
	else
	{
		Offset = CellPerSide;
	}

	const int GridX = static_cast<int>((WorldCoordinate.X - MinWorld.X) / CellWidth) + Offset;

	return FVector2D(GridX, GridY);
}

FVector AGridManager::GetWorldCoordinate(const FVector2D GridCoordinate) const
{
	const float WorldY = GridCoordinate.Y * CellHeight + MinWorld.Y + CellHeight / 2.f;
	float WorldX, WorldZ;

	if (GridCoordinate.X < CellPerSide)
	{
		WorldZ = GroundHeight;
		WorldX = GridCoordinate.X * CellWidth + MinWorld.X + CellWidth / 2.f;
	}
	else
	{
		WorldZ = FloorHeight;
		WorldX = (GridCoordinate.X - CellPerSide) * CellWidth + MinWorld.X + CellWidth / 2.f;
	}

	return FVector(WorldX, WorldY, WorldZ);
}
