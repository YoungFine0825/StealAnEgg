#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayAbilities/Public/AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "SAEPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class USAEPlayerAttributeComponent;
class UAbilitySystemComponent;
class UInputAction;
class USAEHeldItemPresentationComponent;


struct FOnAttributeChangeData;
struct FInputActionValue;

USTRUCT(BlueprintType)
struct FSAECharacterAttachmentSlot
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FGameplayTag SlotTag;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FName SocketName = NAME_None;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FTransform RelativeTransform;
};

UCLASS()
class STEALANEGG_API ASAEPlayerCharacter : public ACharacter,public IAbilitySystemInterface
{
	GENERATED_BODY()
public:

	ASAEPlayerCharacter();
	
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void PossessedBy(AController* NewController) override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void OnRep_PlayerState() override;

	UFUNCTION(BlueprintCallable)
	bool ResolveHeldItemSlot(FGameplayTag SlotTag,USceneComponent*& OutComponent,FName& OutSocketName,FTransform& OutRelativeTransform) const;

	UFUNCTION(BlueprintPure)
	USAEHeldItemPresentationComponent* GetHeldItemPresentationComponent() const
	{
		return HeldItemPresentationComponent;
	}

	UFUNCTION(BlueprintPure)
	USceneComponent* GetFrontCarryAnchor() const
	{
		return FrontCarryAnchor;
	}
	
protected:
	void InitializeAbilitySystem();

	void UninitializeAbilitySystem();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void HandleMoveInput(const FInputActionValue& MoveInput );

	void HandleLookInput(const FInputActionValue& LookInput );

	void HandleInteractionStarted();

	void HandleInteractionReleased();
	
private:

	void BindMovementAttributes(UAbilitySystemComponent* ASC);

	void UnbindMovementAttributes();

	void HandleMovementAttributeChanged(const FOnAttributeChangeData& Data);

	void RefrehsMovementSpeed();

	TWeakObjectPtr<UAbilitySystemComponent> BoundASC;

	FDelegateHandle BaseMoveSpeedChangedHandle;
	FDelegateHandle MoveSpeedBounsChangedHandle;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AbilitySystem", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USAEPlayerAttributeComponent> PlayerAttribute;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Held Item",meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USAEHeldItemPresentationComponent> HeldItemPresentationComponent;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Held Item",meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> FrontCarryAnchor;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Held Item",meta = (AllowPrivateAccess = "true"))
	TArray<FSAECharacterAttachmentSlot> AttachmentSlots;
	
	UPROPERTY(EditDefaultsOnly,Category="Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly,Category="Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly,Category="Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly,Category="Input")
	TObjectPtr<UInputAction> MouseLookAction;

	UPROPERTY(EditDefaultsOnly,Category="Input|Interaction")
	TObjectPtr<UInputAction> InteractionAction;

};
