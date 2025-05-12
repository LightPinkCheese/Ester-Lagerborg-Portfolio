// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayer.h"

AMyPlayer::AMyPlayer()
{
	CurrentHealth = MaxHealth;
}

void AMyPlayer::BeginPlay()
{	
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
}

void AMyPlayer::GetDamaged()
{
	CurrentHealth -= 1;

	if (CurrentHealth <= 0)
	{
		OnDeath();
	}
	UpdateHealth();
}


void AMyPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//here to se if movement now works
}

