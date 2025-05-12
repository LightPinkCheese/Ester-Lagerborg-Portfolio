// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyHealthBar2.h"

void UEnemyHealthBar2::NativeConstruct()
{
	Super::NativeConstruct();
}

void UEnemyHealthBar2::SetBarValuePercent(float const value)
{
	HealthValue->SetPercent(value);
}