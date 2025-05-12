// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayer.generated.h"


UCLASS()
class BULLETHELL_2D_API AMyPlayer : public APlayerController
{
	GENERATED_BODY()

public:
	AMyPlayer();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Health"))
	int MaxHealth = 1.f;

	UFUNCTION(BlueprintCallable, meta = (Category = "Health"))
	void GetDamaged();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (Category = "Health"))
	int CurrentHealth = 1.f;


	UFUNCTION(BlueprintImplementableEvent, meta = (Category = "Health"))
	void UpdateHealth();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Attack"))
	float PlayerDamage = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "LVL"))
	float XP = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "LVL"))
	float LVLsReady = 0;
	
	//changed to blueprint for crashing reasons
	UFUNCTION(BlueprintImplementableEvent, meta = (Category = "LVL"))
	void XPGained();

	
	//for death screen
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (Category = "UI"))
	void OnDeath();

protected:
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (Category = "LVL", AllowPrivateAccess = "true"))
	float XPNeededForNextLevel = 10;
};
