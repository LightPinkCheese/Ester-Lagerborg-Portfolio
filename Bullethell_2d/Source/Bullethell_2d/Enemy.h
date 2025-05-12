// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemy.generated.h"


UCLASS()
class BULLETHELL_2D_API AEnemy : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEnemy();

	UPROPERTY(EditAnywhere, meta = (Category = "Enemy Movement"))
	float speed = 1000.f;

	//if enemy not visible how long until it disapears
	UPROPERTY(EditAnywhere, meta = (Category = "Enemy Movement"))
	float HowLongOutsideUntilDeleted = 1.0f;

	UPROPERTY(EditAnywhere, meta = (Category = "Enemy Health"))
	float MaxHealth = 5.f;

	UFUNCTION(BlueprintCallable, meta = (Category = "Enemy Health"))
	void GetDamaged(float damage);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (Category = "Enemy Health"))
	void EnemyDeath();

	UPROPERTY(EditAnywhere, meta = (Category = "Enemy Health"))
	float TimeHealthIsVisible = 1.f;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (Category = "Enemy Attack"))
	void DealDamage();

	UPROPERTY(EditAnywhere, meta = (Category = "Enemy Attack"))
	float Range = 150.f;


	UPROPERTY(EditAnywhere, meta = (Category = "Enemy Attack"))
	float FireRate = 2.f;


	

protected:
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime);
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

	UFUNCTION(BlueprintCallable, meta = (Category = "Collision Stuff"))
	void Overlapstarts(AActor* collision);

	void OnDeath();



private:
	float Distance;
	FTimerHandle TimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (Category = "Enemy Health", AllowPrivateAccess = "true"))
	float CurrentHealth = 0.f;

	UPROPERTY(BlueprintReadOnly, meta = (Category = "Enemy Attack", AllowPrivateAccess = "true"))
	FVector TargetDirection;

	


	UPROPERTY(VisibleAnywhere, meta = (Category = "Enemy Attack"))
	bool WithinRangeOfTarget = false;

	UPROPERTY(VisibleAnywhere, meta = (Category = "Enemy Attack"))
	float falserange;
	float LastFireTime = 0.f;

	UPROPERTY(VisibleAnywhere, meta = (Category = "EnemyComponents"))
	USceneComponent* Root = nullptr;

	UPROPERTY(EditAnywhere, meta = (Category = "EnemyComponents"))
	class AMyPlayer* Player;

	//too avoid collision with eachother, just guess
	UPROPERTY(EditAnywhere, meta = (Category = "Collision Stuff", AllowPrivateAccess = "true"))
	float EnemyWidth = 100.f;

	UPROPERTY(VisibleAnywhere, meta = (Category = "Collision Stuff"))
	int EnemyNumber;
	TArray<AEnemy*> previuscollisions;

	float DissapearTimer;
	float healthBarTimer;
	
	UPROPERTY(VisibleAnywhere, meta = (Category = "EnemyComponents"))
	class UWidgetComponent* WidgetComponent;

};
