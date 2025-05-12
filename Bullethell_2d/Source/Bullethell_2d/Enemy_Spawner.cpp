// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_Spawner.h"
#include "Enemy.h"
#include "MyPlayer.h"
#include "Kismet/GameplayStatics.h" 
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AEnemy_Spawner::AEnemy_Spawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemy_Spawner::BeginPlay()
{
	Super::BeginPlay();
	Player = Cast<AMyPlayer>(UGameplayStatics::GetActorOfClass(GetWorld(), AMyPlayer::StaticClass()));

}

// Called every frame
void AEnemy_Spawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsSpawnReady)
	{
		SetActorLocation(Player->GetPawn()->GetActorLocation());

		//Where enemies spawn
		int negativeorpositiveX = FMath::RandRange(0, 1);
		if (negativeorpositiveX == 1)
		{
			Spawnlocation.X = GetActorLocation().X + FMath::RandRange(RadiusOfNoSpawn, RadiusOfSpawn);;
		}
		else
		{
			Spawnlocation.X = GetActorLocation().X - FMath::RandRange(RadiusOfNoSpawn, RadiusOfSpawn);;
		}
		int negativeorpositiveY = FMath::RandRange(0, 1);
		if (negativeorpositiveY == 1)
		{
			Spawnlocation.Y = GetActorLocation().Y + FMath::RandRange(RadiusOfNoSpawn, RadiusOfSpawn);;
		}
		else
		{
			Spawnlocation.Y = GetActorLocation().Y - FMath::RandRange(RadiusOfNoSpawn, RadiusOfSpawn);;
		}
		Spawnlocation.Z = GetActorLocation().Z;
		

		//Choose which enemy is spawned
		float WhichEnemy = FMath::RandRange(0.f, 100.f);
		if (WhichEnemy <= NormalEnemyChance)
		{
			SpawnNormalEnemy();
			//made into blueprint event cuz can't figure out spwanactor function
		}
		else if (WhichEnemy <= NormalEnemyChance + RangedEnemyChance)
		{
			SpawnRangedEnemy();
		}
		else if (WhichEnemy <= 100 - ScoutEnemyChance)
		{
			SpawnTankEnemy();
		}
		else if (WhichEnemy <= 100)
		{
			SpawnScoutEnemy();
		}
		IsSpawnReady = false;
	}
	else
	{
		EnemySpawnTimer -= DeltaTime;
		if (EnemySpawnTimer <= 0)
		{
			IsSpawnReady = true;
			EnemySpawnTimer = TimeUntilNextEnemy;
		}

	}
}

