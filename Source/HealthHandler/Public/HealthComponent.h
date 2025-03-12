// License
// This Plugin is created @Author: VicAgent Marz 2025
// This project is licensed under the [MIT License] (https://mit-license.org/).

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "HealthComponent.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedEventDispatcher, UHealthComponent*, OwningHealthComp, float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDeathEventDispatcher, float, LastHitDamage);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWoundEventDispatcher,bool, bIsWounded, float, DamageReceived);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FArmorChangedEventDispatcher, float, newArmor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHealingEventDispatcher, float , HealAmount);
//
//

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class HEALTHHANDLER_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


public:


	/** This Event is Fired when Owner reduce health to zero and dies */
	UPROPERTY(BlueprintAssignable,  Category = "Health Component | Events")
	FDeathEventDispatcher OnDeathEvent;

	/** Fire Death Event */
	UFUNCTION(BlueprintCallable, Category = "Health Component")
	virtual void OnDeath(float LastHitDamage);

	/** This Event is Fired when Owner (normally a pawn) 
	take damages of any "apply damage" interface */
	UPROPERTY(BlueprintAssignable,  Category = "Health Component | Events")
	FWoundEventDispatcher OnWoundEvent;

	/** Fire Wound Event */
	UFUNCTION(BlueprintCallable, Category = "Health Component")
	virtual void OnWound(bool bWound, float Damage);

	/** This Event is Fired when Owner (normally a pawn)  
	take damages of any "apply damage" interface and armor is damaged */
	UPROPERTY(BlueprintAssignable, Category = "Health Component | Events")
	FArmorChangedEventDispatcher onArmorChanged;
	
	/** Fire Armor Event */
	UFUNCTION(BlueprintCallable, Category = "Health Component")
	virtual void OnArmor(float newAromourValue);

	/** This Event is Fired when Owner Get healed from any source */
	UPROPERTY(BlueprintAssignable, Category = "Health Component | Events")
	FHealingEventDispatcher OnHealingEvent;

	/** Fire Healing Event */
	UFUNCTION(BlueprintCallable, Category = "Health Component")
	virtual void OnHealing(float HealAmount);

	/** This Event is Fired when Owner (normally a pawn)
	take damages of any "apply damage" interface, or  heal or healtregen execute   */
	UPROPERTY(BlueprintAssignable, Category = "Health Component | Events")
	FOnHealthChangedEventDispatcher OnHealthChanged;

	
	UFUNCTION()
	void HandledTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	
	/** update the current health, this node is server only. */
	UFUNCTION(BlueprintCallable, Category = "Health Component")
	void UpdateHealth(float DeltaHealth);


	UFUNCTION(BlueprintCallable, Category = "Health Component")
	void UpdateArmor(float DeltaArmor);

	/** update the Max health, this node is server only. */
	UFUNCTION(BlueprintCallable, Category = "Health Component")
	void UpdateMaxHealth(float DeltaHealth);

	/** update the Max Armor, this node is server only. */
	UFUNCTION(BlueprintCallable, Category = "Health Component")
	void UpdateMaxArmor(float DeltaArmor);

	/**Heal to Owner. Add DeltaHeal to Heath*/
	UFUNCTION(BlueprintCallable, Category = "Health Component")
	void Heal(float DeltaHeal);	

	/**Fix to ArmorOwner. Add DeltaArmor to Armor*/
	UFUNCTION(BlueprintCallable, Category = "Health Component")
	void FixArmor(float DeltaArmor);

	/** To apply Mods to Health Regen, Heal by potions or Damage for Fire Efects */
	UFUNCTION(BlueprintCallable, Category = "Health Component")
	void AutoHealthRegen();

	/** Custom your HealthRegenPeriod  and HealthRegenAmount Use this before call AutohealthRegen or any "Apply damage" interface */
	UFUNCTION(BlueprintCallable, Category = "Health Component")
	void SetupAutoHealthRegen(float RegenPeriod, float HealAmount);

	/** Called in every client from Server Every time Healt change in Server */
	UFUNCTION(BlueprintCallable, Category = "Health Component")
	void OnRep_Health(float OldHealth);


	/** Called in every client from Server Every time Healt change in Server */
	UFUNCTION(BlueprintCallable, Category = "Health Component")
	void OnRep_Armor();

protected:
	
	FTimerHandle T_AutoHeal;

public:	
		

	/** Current Health */
	UPROPERTY(ReplicatedUsing = "OnRep_Health", BlueprintReadWrite, EditAnywhere, Category = "Health Component")
		float Health = 100.f;

	/** Current Max Health in Match Its posible Rise to DefaultMaxValueStat */
	UPROPERTY(Replicated, BlueprintReadWrite, EditDefaultsOnly, Category = "Health Component")
		float MaxHealth = 100.f;

			/** Current Armor */
	UPROPERTY(ReplicatedUsing = "OnRep_Armor", BlueprintReadWrite, EditAnywhere, Category = "Health Component")
		float Armor = 100.f;
		/** Current Max Armor in Match Its posible Rise to DefaultMaxValueStat */
	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Health Component")
		float MaxArmor = 100.f;

		/** Default ClampMax to MaxHealth or MaxArmor in Game.
		MaxHealh <= DefaultMaxValueStat  or MaxArmor <= DefaultMaxValueStat  */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Health Component")
		float DefaultMaxValueStat = 500.f;
	/** is True when current health is zero */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Health Component" )
		bool bIsDead=false;

	/**Is true whenever the owner receives damage , 
	You can use this for a stunned or Woinded state	*/
	UPROPERTY( Replicated,BlueprintReadOnly, Category = "Health Component")
		bool bIsWounded;
	
	/**if false, After Recovery Health regenerates instantly, if true health needs a Time To Regenerate */
	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Health Component", meta = (ExposeOnSpawn = "true"))
	bool HasAutoHealthRegen = false;

	/*Time To reset bisWound and start health regeneration
	if TimeToRecovery=0.f bIsWounded=false always and onRecovery execute immediately */
	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Health Component", meta = (ExposeOnSpawn = "true",  EditCondition = "HasAutoHeathRegen"))
	float DealyToAutoHealthRegen = 1.2f;	
	
	/*Every "Time Health Regen" seconds apply Heal( amtHealth )  until Health == MaxHealth  */
	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Health Component", meta = (ExposeOnSpawn = "true", EditCondition = "HasAutoHeathRegen"))
	float HealthRegenPeriod = 0.05f;

	/** Amount of Health added every TimeHealthRegen */
	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Health Component", meta = (ExposeOnSpawn = "true", EditCondition = "HasAutoHeathRegen"))
	float HealthRegenAmount = 1.f;
	

	/** Get the max value health */
	UFUNCTION(BlueprintPure, Category = "Health Component")
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

	/** Get the max value health */
	UFUNCTION(BlueprintPure, Category = "Health Component")
	FORCEINLINE float GetMaxArmor()const { return MaxArmor; }

	/** Get the current health  */
	UFUNCTION(BlueprintPure, Category = "Health Component")
	FORCEINLINE float GetHealth() const { return Health; }

	/** Get the Current Armor */
	UFUNCTION(BlueprintPure, Category = "Health Component")
	FORCEINLINE float GetArmor() const { return Armor; }
	
	
	/** Calcule if player is Dead*/
	UFUNCTION(BlueprintPure, Category = "Health Component")
	FORCEINLINE bool CalculeIsDead() { return bIsDead = (Health <= 0) ? true : false; }
	
	/**Calcule if player has Armor*/
	UFUNCTION(BlueprintPure, Category = "Health Component")
	FORCEINLINE bool CalculeHasArmor() const { return (Armor > 0) ? true : false; }

	/** Get the health Percent for display in widget : Health / MaxHealth */
	UFUNCTION(BlueprintPure, Category = "Health Component")
	FORCEINLINE float GetHealthBarPercent() const { return (Health / MaxHealth); };

	
	/** Get the health Percent for display in widget : Health / MaxHealth */
	UFUNCTION(BlueprintPure, Category = "Health Component")
	FORCEINLINE float GetArmorBarPercent() const { return (Armor / MaxArmor); };

	
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
