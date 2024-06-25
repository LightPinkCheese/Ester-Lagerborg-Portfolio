#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../dev-Ludvig/StatusEffectManager.h"
#include "Enemy.generated.h"

// Create a delegate where can pass one parameter of type AEnemy* 
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPooledObjectDestroyed, AEnemy*, Enemy);

UCLASS()
class AEnemy: public AActor
{
	// Comment here !

	GENERATED_BODY()

public:
	AEnemy();
	UPROPERTY(EditAnywhere, meta = (Category = "Enemy Movement"))
	float Maxspeed = 5000.f;
	UPROPERTY(EditAnywhere, meta = (Category = "Enemy Movement"))
	float Minspeed = 1000.f;

	//how fast the enemies slow down (spline movement)
	UPROPERTY(EditAnywhere, meta = (UIMin = "0.0", UIMax = "1", Category = "Enemy Movement"))
	float SlowdownMultiplier = 1.0f;

	//if enemy not visible how long until it disapears
	UPROPERTY(EditAnywhere, meta = (Category = "Enemy Movement"))
	float HowLongOutsideUntilDeleted = 1.0f;

	UPROPERTY(EditAnywhere, meta = (Category = "Enemy Health"))
	float MaxHealth = 5.f;

	UPROPERTY(EditAnywhere, meta = (Category = "Enemy Health"))
	float TimeHealthIsVisible = 1.f;

	// Getter and setter for IfFireOnTorch
	UFUNCTION(BlueprintCallable)
	bool GetIfFireOnTorch() const { return IfFireOnTorch; }
	
	UFUNCTION(BlueprintCallable)
	void SetIfFireOnTorch(bool Value) { IfFireOnTorch = Value; }

	UFUNCTION(BlueprintCallable, meta = (Category = "Enemy Health"))
	void TakeDamage(float damage);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (Category = "Enemy Health"))
	void EnemyDeath();

	UPROPERTY(EditAnywhere, meta = (Category = "Enemy Attack"))
	float MeleeDamage;

	UPROPERTY(EditAnywhere, meta = (Category = "Enemy Attack"))
	float Range = 150.f;

	UPROPERTY(EditAnywhere, meta = (Category = "Enemy Attack"))
	float FireRate = 2.f;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (Category = "Enemy Attack"))
	void DealDamage();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (Category = "Enemy Attack"))
	void ShootRanged();

	//After Enemy attacks how long until they can move again
	UPROPERTY(EditAnywhere, meta = (Category = "Enemy Attack"))
	float StaggerTime = 0.1f;

	//percentage chance of an enemy being a ranged enemy, this is base chance of ranged enemy will increase with RangedChanceIncreasePerUpgrade
	UPROPERTY(EditAnywhere, meta = (UIMin = "0.0", UIMax = "100", Category = "Ranged Enemy"))
	float ChanceOfRanged;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (Category = "Ranged Enemy"))
	bool isRangedEnemy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Category = "Ranged Enemy"))
	float RangedDamage;

	UPROPERTY(EditAnywhere, meta = (Category = "Ranged Enemy"))
	float RangedRange;

	UPROPERTY(EditAnywhere, meta = (Category = "Ranged Enemy"))
	float RangedFireRate = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Category = "Ranged Enemy"))
	float RangedProjectileSpeed;

	UPROPERTY(EditAnywhere, meta = (Category = "Vehicular Manslaughter"))
	float DamageOnCollsionWithCart = 1.f;

	UPROPERTY(EditAnywhere, meta = (Category = "Vehicular Manslaughter"))
	float KnockBackTimer = 0.5f;

	//how much the speed should be multiplied on second upgrade
	UPROPERTY(EditAnywhere, meta = (Category = "Enemy Improvments"))
	float SpeedMultiplier;

	//how much extra health on 3rd upgrade
	UPROPERTY(EditAnywhere, meta = (Category = "Enemy Improvments"))
	float HealthIncrease;

	//the percentile units of increase in chance of ranged enemy spawning, stops increaseing after 3 upgrades
	UPROPERTY(EditAnywhere, meta = (UIMin = "0", UIMax = "100", Category = "Enemy Improvments"))
	float RangedChanceIncreasePerUpgrade;

	//too avoid collision with eachother, just guess
	UPROPERTY(EditAnywhere, meta = (Category = "Collision Stuff"))
	float EnemyWidth = 100.f;

	//1 is 100%, 2 is 50% etc...
	UPROPERTY(EditAnywhere, meta = (Category = "EnemyComponents"))
	int ChanceOfTorchLit;

	UPROPERTY(EditAnywhere, meta = (Category = "Effects"))
	TSubclassOf<class AActor> FreezeEffectSubclass;

	void ApplyFreezeEffect();

	void RemoveFreezeEffect();

	void ApplyBurnEffect();

	void RemoveBurnEffect();

	class StatusEffectManager* statusEffectManager;

	// Object Pool Functions
	FOnPooledObjectDestroyed OnPooledObjectDestroyed;

	UFUNCTION(BlueprintCallable, Category = "Pooled Object")
	void Deactivate();
	
	void SetActive(bool Active);
	void SetPoolIndex(int Index);

	UFUNCTION(BlueprintCallable)
	bool IsActive();
	int GetPoolIndex();


protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, meta = (Category = "Collision Stuff"))
	void Overlapstarts(AActor* collision);

	UPROPERTY(EditAnywhere, Category = "Enemy sounds")
	TArray<USoundBase*> EnemyMaleDeathSounds;

	UPROPERTY(EditAnywhere, Category = "Enemy sounds")
	TArray<USoundBase*> EnemyFemaleDeathSounds;

	UPROPERTY(EditAnywhere, Category = "Enemy sounds")
	USoundBase* EnemyDamageSound;

	UPROPERTY(EditAnywhere, Category = "Enemy sounds")
	USoundBase* MeleeAttackSound;

	// Object Pool Variables
	bool bIsActive;
	int PoolIndex;

private:
	FTimerHandle TimerHandle;
	//Movement Variables
	float speed;
	float Distance;
	float SlowdoneSpline = 1;
	UPROPERTY(BlueprintReadOnly, meta = (Category = "Enemy Attack", AllowPrivateAccess = "true"))
	FVector TargetDirection;

	bool movementRestricted = false;

	UPROPERTY(BlueprintReadOnly, meta = (Category = "Enemy Health", AllowPrivateAccess = "true"))
	bool IsDying = false;

	UPROPERTY(VisibleAnywhere, meta = (Category = "Enemy Health"))
	float CurrentHealth = 0.f;
	float healthBarTimer;

	UMaterialInstanceDynamic* DynMaterial;

	void DamageFlashLogic(float DeltaTime);

	float currentFlashIntensity = 0.f;
	UPROPERTY(EditAnywhere, Category = "Damage Flash")
	float flashIntensity = 0.4f;
	UPROPERTY(EditAnywhere, Category = "Damage Flash")
	float flashFalloffSpeed = 0.05f;

	bool flashing = false;
	

	UPROPERTY(VisibleAnywhere, meta = (Category = "Enemy Attack"))
	bool WithinRangeOfTarget = false;

	UPROPERTY(VisibleAnywhere, meta = (Category = "Enemy Attack"))
	float falserange;
	float LastFireTime = 0.f;
	//Ranged Enemy Variables
	float rangecheck;

	UPROPERTY(VisibleAnywhere, meta = (Category = "Collision Stuff"))
	int EnemyNumber;
	TArray<AEnemy*> previuscollisions;
	

	//Vechicular Manslaughter Variables
	void SmoothKnockback();
	bool SmoothKnockBackActive = false;

	//mesh variables
	int WhichModel;
	TArray<USceneComponent*> EnemyMeshes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (Category = "EnemyComponents", AllowPrivateAccess = "true"))
	USkeletalMeshComponent* SkeletalMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (Category = "EnemyComponents", AllowPrivateAccess = "true"))
	bool IfFireOnTorch = false;

	UFUNCTION(BlueprintCallable)
	void HandleDeath();

	UPROPERTY(VisibleAnywhere, meta = (Category = "EnemyComponents"))
	USceneComponent* Root = nullptr;

	//Where the meshes are, one mesh is selected randomly (melee and ranged seperatly) also very important: MAKE THE MESHES NOT VISIBLE!!!
	UPROPERTY(VisibleAnywhere, meta = (Category = "EnemyComponents"))
	USceneComponent* SecondRoot = nullptr;

	UPROPERTY(VisibleAnywhere, meta = (Category = "EnemyComponents"))
	class UWidgetComponent* WidgetComponent;
	class APlayerCarriage* Player;
	class APlayerSpline* SplineMovement;
	class UCarriageGameInstance* upgradesthingy;

	bool bIsMale;

};
