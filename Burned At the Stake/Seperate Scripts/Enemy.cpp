#include "Enemy.h"
#include "../dev-Arystos/ObjectPool.h"
#include "../dev-sean/PlayerCarriage.h"
#include "../dev-sean/PlayerSpline.h"
#include "../GameMode/CarriageGameInstance.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h" 
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundBase.h"

#include "Components/WidgetComponent.h"
#include "HealthBar.h"

AEnemy::AEnemy()
{
	// Comment here !
	CurrentHealth = MaxHealth;

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthValue"));
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;

	SecondRoot = CreateDefaultSubobject<USceneComponent>("SecondRoot");
	SecondRoot->SetupAttachment(Root);

	PrimaryActorTick.bCanEverTick = true;
	if (WidgetComponent)
	{
		WidgetComponent->SetupAttachment(RootComponent);
		WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
		static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClass{ TEXT("/Game/inherited_Actors/dev-Ester/MyHealthBar") };
		if (WidgetClass.Succeeded())
		{
			WidgetComponent->SetWidgetClass((WidgetClass.Class));
		}
	}

	Player = Cast<APlayerCarriage>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCarriage::StaticClass()));
	SplineMovement = Cast<APlayerSpline>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerSpline::StaticClass()));

	statusEffectManager = new StatusEffectManager(this);
}


void AEnemy::BeginPlay()
{
	WidgetComponent->SetVisibility(false);
	Super::BeginPlay();



	TArray<AActor*> enemiesamount;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(), enemiesamount);
	EnemyNumber = enemiesamount.Num();


	speed = FMath::RandRange(Minspeed, Maxspeed);

	int HowManyUpgradesAquired = Cast<UCarriageGameInstance>(GetGameInstance())->GetNumOfUpgrades();
	if (HowManyUpgradesAquired >= 1)
	{
		ChanceOfRanged += RangedChanceIncreasePerUpgrade;
	}
	if (HowManyUpgradesAquired >= 2)
	{
		ChanceOfRanged += RangedChanceIncreasePerUpgrade;
		speed *= SpeedMultiplier;
	}
	if (HowManyUpgradesAquired >= 3)
	{
		ChanceOfRanged += RangedChanceIncreasePerUpgrade;
		MaxHealth += HealthIncrease;
	}

	rangecheck = FMath::RandRange(0, 100);
	if (rangecheck <= ChanceOfRanged)
	{
		Range = RangedRange;
		isRangedEnemy = true;
		FireRate = RangedFireRate;
		StaggerTime = 0;
		WhichModel = 0;
	}
	else
	{
		WhichModel = FMath::RandRange(1, 5);
	}

	if (WhichModel == 2 || WhichModel == 4)
	{
		bIsMale = false;
	}
	else
	{
		bIsMale = true;
	}
	SkeletalMesh = Cast<USkeletalMeshComponent>(SecondRoot->GetChildComponent(WhichModel));

	DynMaterial = UMaterialInstanceDynamic::Create(SkeletalMesh->GetMaterial(0), this);
	SkeletalMesh->SetMaterial(0, DynMaterial);

	

	falserange = Range;
	LastFireTime = GetWorld()->GetTimeSeconds();

	CurrentHealth = MaxHealth;

}

void AEnemy::ApplyFreezeEffect() {
	movementRestricted = true;
	FActorSpawnParameters spawnInfo;
	GetWorld()->SpawnActor<AActor>(FreezeEffectSubclass, GetActorTransform(), spawnInfo);
}

void AEnemy::RemoveFreezeEffect() {
	movementRestricted = false;
}

void AEnemy::ApplyBurnEffect() {
	
}

void AEnemy::RemoveBurnEffect() {

}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	statusEffectManager->StatusEffectsTick(DeltaTime);

	if (!movementRestricted && !IsDying && GetWorld()->TimeSince(LastFireTime) > StaggerTime)
	{
		FVector CurrentDirection = Root->GetForwardVector();
		TargetDirection = Player->GetActorLocation() - GetActorLocation();
		TargetDirection.Z = 0;
		Distance = TargetDirection.Length();
		TargetDirection.Normalize();

		FVector PlayerDirection = GetActorLocation() - Player->GetActorLocation();
		PlayerDirection.Normalize();
		FVector PlayerForward = Player->GetActorForwardVector();
		float AheadOfPlayer = PlayerForward.Dot(PlayerDirection);

		//CurrentDirection = UKismetMathLibrary::Vector_SlerpVectorToDirection(CurrentDirection, TargetDirection, 5.f * DeltaTime);

		FRotator TargetRotation = UKismetMathLibrary::MakeRotFromX(CurrentDirection);
		Root->SetWorldRotation(FRotator(TargetRotation.Pitch, TargetRotation.Yaw, TargetRotation.Roll));
		if (Distance <= falserange)
		{
			if (GetWorld()->TimeSince(LastFireTime) > FireRate && Distance <= Range && bIsActive)
			{
				WithinRangeOfTarget = true;

				if (!isRangedEnemy)
				{
					DealDamage();

					if (Player->CarriageMeleeDamageSounds.Num() != 0)
					{
						int rand = FMath::RandRange(0, Player->CarriageMeleeDamageSounds.Num() - 1);
						Player->TakeDamage(MeleeDamage, Player->CarriageMeleeDamageSounds[rand]);
					}

					// Melee attack sound
					if (MeleeAttackSound != nullptr)
					{
						UGameplayStatics::PlaySoundAtLocation(this, MeleeAttackSound, GetActorLocation());
					}
				}
				else
				{
					ShootRanged();
				}
				LastFireTime = GetWorld()->GetTimeSeconds();
			}

		}
		else
		{
			AddActorWorldOffset(TargetDirection * speed * DeltaTime);
			WithinRangeOfTarget = false;
		}

		if (AheadOfPlayer < 0 && bIsActive && !SmoothKnockBackActive)
		{
			AddActorWorldOffset(SplineMovement->DeltaSpline * SlowdoneSpline);
		}
		SlowdoneSpline -= DeltaTime * SlowdownMultiplier;

	}

	DamageFlashLogic(DeltaTime);

	if (auto const widget = Cast<UHealthBar>(WidgetComponent->GetUserWidgetObject()))
	{
		widget->SetBarValuePercent(CurrentHealth / MaxHealth);
	}

	if (!previuscollisions.IsEmpty())
	{
		for (int i = 0; i < previuscollisions.Num(); i++)
		{
			if (!previuscollisions[i]->bIsActive)
			{
				falserange = Range;
				previuscollisions.RemoveAt(i);
				i--;
			}

		}
	}

	if (WidgetComponent->IsWidgetVisible()) {
		healthBarTimer -= DeltaTime;
		if (healthBarTimer < 0) {
			WidgetComponent->SetVisibility(false);
		}
	}

}

void AEnemy::DamageFlashLogic(float DeltaTime) {
	if (flashing) {
		currentFlashIntensity -= DeltaTime;

		if (currentFlashIntensity <= 0) {
			currentFlashIntensity = 0;
			flashing = false;
		}

		//SkeletalMesh->SetScalarParameterValueOnMaterials("Emissive", currentFlashIntensity);
		DynMaterial->SetScalarParameterValue("Emissive", currentFlashIntensity);
	}
}

void AEnemy::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void AEnemy::TakeDamage(float damage) {
	// Damage sound
	if (EnemyDamageSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, EnemyDamageSound, GetActorLocation());
	}

	CurrentHealth -= damage;

	WidgetComponent->SetVisibility(true);
	healthBarTimer = TimeHealthIsVisible;

	if (CurrentHealth <= 0)
	{
		HandleDeath();
	}

	flashing = true;
	currentFlashIntensity = flashIntensity;
}


void AEnemy::Overlapstarts(AActor* collision) {
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
	else if (collision->IsA(APlayerCarriage::StaticClass()) && !SmoothKnockBackActive)
	{
		SmoothKnockBackActive = true;
		TakeDamage(DamageOnCollsionWithCart);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AEnemy::SmoothKnockback, KnockBackTimer, false);
	}
}


void AEnemy::HandleDeath()
{
	
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	IsDying = true;
	EnemyDeath();


	// Play death sound
	if (EnemyMaleDeathSounds.Num() != 0 && EnemyFemaleDeathSounds.Num() != 0)
	{
		int rand1 = FMath::RandRange(0, EnemyMaleDeathSounds.Num() - 1);
		int rand2 = FMath::RandRange(0, EnemyFemaleDeathSounds.Num() - 1);
		if (bIsMale)
		{
			UGameplayStatics::PlaySoundAtLocation(this, EnemyMaleDeathSounds[rand1], GetActorLocation());
		}
		else
		{
			UGameplayStatics::PlaySoundAtLocation(this, EnemyFemaleDeathSounds[rand2], GetActorLocation());
		}
	}

	// Remove the enemy from the object pool after death animation
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AEnemy::Deactivate, 35 / 30, false);
	SetActorEnableCollision(false);

	statusEffectManager->CleanseStatusEffects();

	//Fix other variables
	falserange = Range;
	previuscollisions.Empty();
	SlowdoneSpline = 1;

	// Set the torch invisible
	SetIfFireOnTorch(false);

	// DEPRECATED: Destroy the enemy
	//Destroy();
}

void AEnemy::SmoothKnockback()
{
	SmoothKnockBackActive = false;
}

// Pool Functions

void AEnemy::Deactivate()
{	
	//Setting here for delay
	IsDying = false;
	WidgetComponent->SetVisibility(false);
	CurrentHealth = MaxHealth;

	SetActive(false);
	OnPooledObjectDestroyed.Broadcast(this); // Broadcast that this object has been destroyed
}

void AEnemy::SetActive(bool Active)
{

	if (SkeletalMesh != nullptr) {
		// Set the visibility of the enemy
		SkeletalMesh->SetVisibility(Active);
		
		// Activate the torch randomly on the enemy based on Active bool
		if (Active)
		{
			SetIfFireOnTorch(FMath::RandBool());
		}
		else
		{
			SetIfFireOnTorch(false);
		}
		
		//SetActorHiddenInGame(!Active);
		SetActorEnableCollision(Active);

		// Set the active state of the enemy
		bIsActive = Active;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SkeletalMesh is null"));
		bIsActive = false;
	}

}

void AEnemy::SetPoolIndex(int Index)
{
	PoolIndex = Index;
}

bool AEnemy::IsActive()
{
	return bIsActive;
}

int AEnemy::GetPoolIndex()
{
	return PoolIndex;
}
