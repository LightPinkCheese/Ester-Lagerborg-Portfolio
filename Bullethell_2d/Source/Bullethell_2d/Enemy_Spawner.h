// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemy_Spawner.generated.h"

UCLASS()
class BULLETHELL_2D_API AEnemy_Spawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemy_Spawner();

	UPROPERTY(EditAnywhere, meta = (Category = "Refrences"))
	class AMyPlayer* Player;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "SpawnStuff"))
	float TimeUntilNextEnemy;

	UPROPERTY(EditAnywhere, meta = (Category = "SpawnStuff"))
	float RadiusOfSpawn = 5000;

	UPROPERTY(EditAnywhere, meta = (Category = "SpawnStuff"))
	float RadiusOfNoSpawn = 1000;

	//Make sure odds equals 100 or chance of no enemy spawn, also if something is higher the lower down types get less chance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "EnemyOdds"))
	float NormalEnemyChance = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "EnemyOdds"))
	float RangedEnemyChance = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "EnemyOdds"))
	float TankEnemyChance = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "EnemyOdds"))
	float ScoutEnemyChance = 20.f;

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime);

	UFUNCTION(BlueprintImplementableEvent, meta = (Category = "SpawnEvents"))
	void SpawnNormalEnemy();

	UFUNCTION(BlueprintImplementableEvent, meta = (Category = "SpawnEvents"))
	void SpawnTankEnemy();

	UFUNCTION(BlueprintImplementableEvent, meta = (Category = "SpawnEvents"))
	void SpawnScoutEnemy();

	UFUNCTION(BlueprintImplementableEvent, meta = (Category = "SpawnEvents"))
	void SpawnRangedEnemy();

	

private:	
	// Called every frame
	UPROPERTY(VisibleAnywhere, meta = (Category = "SpawnStuff"))
	bool IsSpawnReady = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (Category = "SpawnStuff", AllowPrivateAccess = "true"))
	FVector Spawnlocation;

	float EnemySpawnTimer = 0;

};
