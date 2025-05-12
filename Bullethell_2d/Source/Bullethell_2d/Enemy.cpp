// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "MyPlayer.h"
#include "Bullet.h"
#include "Kismet/GameplayStatics.h" 
#include "Kismet/KismetMathLibrary.h"
#include "Components/WidgetComponent.h"
#include "EnemyHealthBar2.h"


// Sets default values
AEnemy::AEnemy()
{
 
	CurrentHealth = MaxHealth;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthValue"));
	Root = CreateDefaultSubobject<USceneComponent>("Root");


	
	if (WidgetComponent)
	{
		WidgetComponent->SetupAttachment(Root);
		WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
		static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClass{ TEXT("/Game/EnemyStuff/MyEnemyHealthBar2") };
		if (WidgetClass.Succeeded() )
		{
			WidgetComponent->SetWidgetClass((WidgetClass.Class));
		}
	}
	
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{	
	WidgetComponent->SetVisibility(false);
	Super::BeginPlay();
	Player = Cast<AMyPlayer>(UGameplayStatics::GetActorOfClass(GetWorld(), AMyPlayer::StaticClass()));

	TArray<AActor*> enemiesamount;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(), enemiesamount);
	EnemyNumber = enemiesamount.Num();

	falserange = Range;
	LastFireTime = GetWorld()->GetTimeSeconds();

	DissapearTimer = HowLongOutsideUntilDeleted;
	CurrentHealth = MaxHealth;
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector CurrentDirection = Root->GetForwardVector();
	if (Player != nullptr)
	{
		TargetDirection = Player->GetPawn()->GetActorLocation() - GetActorLocation(); 
	}
	if (!WasRecentlyRendered())
	{
		DissapearTimer -= DeltaTime;
		if (DissapearTimer <= 0)
		{	
			Destroy(); //no nedd to do OnDeath since no rendered
		}
	}
	else
	{
		DissapearTimer = HowLongOutsideUntilDeleted;
	}
	TargetDirection.Z = 0;
	Distance = TargetDirection.Length();
	TargetDirection.Normalize();

	FRotator TargetRotation = UKismetMathLibrary::MakeRotFromX(CurrentDirection);
	Root->SetWorldRotation(FRotator(TargetRotation.Pitch, TargetRotation.Yaw, TargetRotation.Roll));
	if (Distance <= falserange)
	{
		if (GetWorld()->TimeSince(LastFireTime) > FireRate && Distance <= Range)
		{
			WithinRangeOfTarget = true;
			DealDamage();
			Player->GetDamaged();

			LastFireTime = GetWorld()->GetTimeSeconds();
		}
	}
	else
	{
		AddActorWorldOffset(TargetDirection * speed * DeltaTime);
		WithinRangeOfTarget = false;
	}


	if (auto const widget = Cast<UEnemyHealthBar2>(WidgetComponent->GetUserWidgetObject()))
	{
		widget->SetBarValuePercent(CurrentHealth / MaxHealth);
	}

	if (WidgetComponent->IsWidgetVisible()) {
		healthBarTimer -= DeltaTime;
		if (healthBarTimer < 0) {
			WidgetComponent->SetVisibility(false);
		}
	}
	
	if (!previuscollisions.IsEmpty())
	{
		for (int i = 0; i < previuscollisions.Num(); i++)
		{
			if (previuscollisions[i]!= nullptr)
			{
				falserange = Range;
				previuscollisions.RemoveAt(i);
				i--;
			}

		}
	}
}

void AEnemy::EndPlay(const EEndPlayReason::Type EndPlayReason) 
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void AEnemy::OnDeath()
{
	EnemyDeath();
	Player->XP += 1;
	Player->XPGained();
	Destroy();
}

void AEnemy::GetDamaged(float damage) {

	CurrentHealth -= damage;

	WidgetComponent->SetVisibility(true);
	healthBarTimer = TimeHealthIsVisible;

	if (CurrentHealth <= 0)
	{	
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
		OnDeath();
	}


}

void AEnemy::Overlapstarts(AActor* collision) 
{
	if (collision->IsA(AEnemy::StaticClass()))
	{
		AEnemy* Otherone = Cast<AEnemy>(collision);

		if (EnemyNumber > Otherone->EnemyNumber)
		{
			if (Distance <= falserange + EnemyWidth + speed)
			{
				falserange += EnemyWidth;
				FVector anticollisiondirection(-TargetDirection.X - EnemyWidth, -TargetDirection.Y - EnemyWidth, 0);
				AddActorWorldOffset(anticollisiondirection * UGameplayStatics::GetWorldDeltaSeconds(this));
				previuscollisions.Add(Otherone);
			}
			else
			{
				AddActorWorldOffset(TargetDirection * UGameplayStatics::GetWorldDeltaSeconds(this) * speed);
			}

		}
		
	} 
	else if (collision->IsA(ABullet::StaticClass()))
	{
		GetDamaged(Player->PlayerDamage);
	}
}
