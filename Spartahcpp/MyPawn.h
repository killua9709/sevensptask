#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MyPawn.generated.h"

struct FInputActionValue;
UCLASS()
class TWOSPPROJECT_API AMyPawn : public APawn
{
	GENERATED_BODY()

public:
	AMyPawn();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCapsuleComponent* CapsuleComponent;	// 전방선언을 이렇게도 표현이 가능하넹

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USpringArmComponent* SpringArmComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCameraComponent* CameraComponent;

	void Move(const FInputActionValue& value);
	void Look(const FInputActionValue& value);
	void StopMove(const FInputActionValue& Value);
	void StopLook(const FInputActionValue& Value);

	// --- 이동 관련 설정 ---
		// 최대 이동 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MaxWalkSpeed = 600.f;

	// Pawn 회전 감도 (Yaw)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float TurnRate = 70.f;

	// 카메라 상하 회전 감도 (Pitch)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float LookUpRate = 70.f;

	// 카메라 Pitch 제한값 (최소, 최대)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MinPitch = -80.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MaxPitch = 0.f;

	// 이동 시 속도 보간 계수 (가속/감속 효과)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float VelocityInterpSpeed = 10.f;

	// 최종적으로 적용되는 이동 속도 (AnimBP에서 활용)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	FVector Velocity;

	// --- 입력 값 저장 ---
	FVector2D MovementInput; // 이동 입력 (X: 전후, Y: 좌우)
	FVector2D LookInput; // Look 입력 (X: Yaw, Y: Pitch)

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
