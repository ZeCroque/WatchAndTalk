// Fill out your copyright notice in the Description page of Project Settings.


#include "wat/AI/GridClearComponent.h"

#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "wat/AI/GridManager.h"
#include "Containers/Array.h"
#include "wat/Core/WatGameState.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
UGridClearComponent::UGridClearComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGridClearComponent::BeginPlay()
{
	Super::BeginPlay();

	Timer = TracePeriod;
}


// Called every frame
void UGridClearComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!GridManager)
	{
		GridManager = Cast<AWatGameState>(UGameplayStatics::GetGameState(GetWorld()))->GridManager;
	}

	Timer += DeltaTime;
	if (Timer >= TracePeriod && GridManager)
	{
		Timer = 0;

		auto const Start = GetOwner()->GetActorLocation() + (FVector::UpVector * ViewUpOffset) + (GetOwner()->
			GetActorForwardVector() * ViewForwardOffset);

		const auto AngleBetweenRows = (RaysRowMaximumAngle - RaysRowMinimumAngle) / static_cast<float>(RaysRowCount);
		
		for(int j = 0; j < RaysRowCount; j++)
		{
			TArray<FVector> Polygon;
			Polygon.Add(Start);
			for (int i = 0; i < RayCountByRow; ++i)
			{
				const float Angle = (i * (FieldOfView / RayCountByRow)) - (FieldOfView / 2);

				const auto Rotator = FRotator(GetComponentRotation().Pitch * -1 + RaysRowMinimumAngle + AngleBetweenRows * j,
				                            GetComponentRotation().Yaw + Angle,
				                            GetComponentRotation().Roll );
				FVector Direction = Rotator.Vector() * RayLength;
				auto const End = Start + Direction;

				if (bShowLineTraces)
				{
					DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, TracePeriod, 0, 1);
				}

				FHitResult Result;
				GetWorld()->LineTraceSingleByChannel(Result, Start, End, ECC_Visibility);

				if (Result.bBlockingHit)
				{
					Polygon.Add(Result.Location);
					if (bShowLineTraces)
					{
						DrawDebugSphere(GetWorld(), Result.Location, 20, 16, FColor::Purple, false, TracePeriod, 0, 1);
					}
				}
			}

			GridManager->SetValueInPolygon(Polygon);
		}
	}
}
