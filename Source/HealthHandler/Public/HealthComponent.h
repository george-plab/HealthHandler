// VícAgent CopyRights All right Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "HealthComponent.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedEventDispatcher, UHealthComponent*, OwningHealthComp, float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDeathEventDispatcher, float, LastHitDamage);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWoundEventDispatcher,bool, bIsWounded, float, DamageReceived);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FArmourChangedEventDispatcher, float, newArmor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRecoveryEventDispatcher);
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

	/** update the current health, this node is server only. */
	void UpdateHealth(float DeltaHealth);

	void UpdateArmor(float DeltaArmor);

	/** update the Max health, this node is server only. */
	void UpdateMaxHealth(float DeltaHealth);

	void UpdateMaxArmot(float DeltaArmor);

public:

	/** This Event is Fired when Owner  of Health component Death */
	UPROPERTY(BlueprintAssignable,  Category = "Health Component | Events")
	FDeathEventDispatcher OnDeathEvent;

	UFUNCTION(BlueprintCallable, Category = "Health Component")
	virtual void OnDeath(float LastHitDamage);

	/** This Event is Fired when Owner (normally a pawn)  of Health component Death */
	UPROPERTY(BlueprintAssignable,  Category = "Health Component | Events")
	FWoundEventDispatcher OnWoundEvent;


	UFUNCTION(BlueprintCallable, Category = "Health Component")
	virtual void OnWound(bool bWound, float Damage);

	FArmourChangedEventDispatcher onArmourChanged;
	
	UFUNCTION(BlueprintCallable, Category = "Health Component")
	virtual void OnArmor( float newAromourValue);

	/** This Event is Fired when Owner  of Health component Recovery from a wound before timetorevery seconds */
	UPROPERTY(BlueprintAssignable, Category = "Health Component | Events")
	FRecoveryEventDispatcher OnRecoveryEvent;

	/** Start health regeneration, Resets bIsWounded  */
	UFUNCTION(BlueprintCallable, Category = "Health Component")
	virtual void OnRecovery();

	UPROPERTY(BlueprintAssignable, Category = "Health Component | Events")
	FOnHealthChangedEventDispatcher OnHealthChanged;

	UFUNCTION()
	void HandledTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);



	/**Heal to Owner. Add DeltaHeal to Heath*/
	UFUNCTION(BlueprintCallable, Category = "Health Component")
	void Heal(float DeltaHeal);
	
	
	

	/**Fix to ArmorOwner. Add DeltaArmor to Armor*/
	UFUNCTION(BlueprintCallable, Category = "Health Component")
	void FixArmor(float DeltaArmor);

	/** To apply Mods to Health Regen, Heal by potions or Damage for Fire Efects */
	UFUNCTION(BlueprintCallable, Category = "Health Component")
	void RegenPerSec();
	
	/**Heal to Owner. Add DeltaHeal to Heath*/
	UFUNCTION(BlueprintCallable, Category = "Health Component")
	void DamagePerSec();
	

	/** Called in every client from Server Every time Healt change in Server */
	UFUNCTION(BlueprintCallable, Category = "Health Component")
	void OnRep_Health(float OldHealth);


	/** Called in every client from Server Every time Healt change in Server */
	UFUNCTION(BlueprintCallable, Category = "Health Component")
	void OnRep_Armor();

public:	
	// Called every frame
	
	

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
	UPROPERTY(Replicated, BlueprintReadOnly, EditDefaultsOnly, Category = "Health Component" )
		bool bIsDead;

	/**Is true whenever the owner receives damage , 
	You can use this for a stunned or Woinded state	*/
	UPROPERTY( Replicated,BlueprintReadOnly, EditDefaultsOnly, Category = "Health Component")
	bool bIsWounded;


	


	
	
	/** The amount of damage received, before recovery,
	You can use this to setup a threshold for stunned if bIsWounde=true*/
	UPROPERTY(Replicated, BlueprintReadOnly, VisibleAnywhere, Category = "Health Component")
	float DamageReceived = 0.f;

	FTimerHandle T_RecoveryFromWound;
	
	/*Time To reset bisWound and start health regeneration (onRecovery())
	if TimeToRecovery=0.f bIsWounded=false always and onRecovery execute immediately */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Health Component", meta = (ExposeOnSpawn = "true"))
	float TimeToRecovery = 1.2f;
	
	/**if false, After Recovery Health regenerates instantly, if true health needs a Time To Regenerate */
	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Health Component", meta = (ExposeOnSpawn = "true"))
	bool HealthRegen_HasDuration = false;

	FTimerHandle T_HealthRegen;
	
	/*Every "Time Health Regen" apply amtHealth Regen
	*/
	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Health Component", meta = (ExposeOnSpawn = "true", EditCondition = "HealthRegen_HasDuration"))
	float TimeHealthRegen = 0.01f;

	UPROPERTY( BlueprintReadWrite, EditAnywhere, Category = "Health Component", meta = (ExposeOnSpawn = "true", EditCondition = "HealthRegen_HasDuration"))
	float AmtHealthEffect = 0.01f;
	

	/** Get the max value health */
	UFUNCTION(BlueprintPure, Category = "Health Component")
	FORCEINLINE float GetMaxHealth()const { return MaxHealth; }

	/** Get the max value health */
	UFUNCTION(BlueprintPure, Category = "Health Component")
	FORCEINLINE float GetMaxArmor()const { return MaxArmor; }

	/** Get the health Percent for display in widget : Health / MaxHealth */
	UFUNCTION(BlueprintPure, Category = "Health Component")
	FORCEINLINE float GetHealth() const { return Health; }

	/** Get the health Percent for display in widget : Health / MaxHealth */
	UFUNCTION(BlueprintPure, Category = "Health Component")
	FORCEINLINE float GetArmor() const { return Armor; }
	
	/** Get the health Percent for display in widget : Health / MaxHealth */
	UFUNCTION(BlueprintPure, Category = "Health Component")
	float GetHealthBarPercent() const;

	
	/** Get the health Percent for display in widget : Health / MaxHealth */
	UFUNCTION(BlueprintPure, Category = "Health Component")
	float GetArmorBarPercent() const;

	
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
