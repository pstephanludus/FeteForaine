# Template_VRPlugin_UE4_NL_V2
Template vr plugin cpp/blueprint UE4 NL_V2

# 24/07/2017<br/>
First Commit<br/>
Base UE4 Template VR project<br/>
Config editor preferences (window browser conent, input viewport..)<br/>
Export .ini input editor viewport<br/>

# 24/07/2017 11:15
Create vrMap(Based of vr map template UE4)


# 24/07/2017 13:00
Create VRPawn.h/VRPawn.cpp

VRPawn.h
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "MotionControllerComponent.h"

VRPawn.cpp
#include "IHeadMountedDisplay.h"

Use HeadMountedDisplay and set MotionController add in Template_VRPlugNL_V2.Build.cs file
 PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
Set struct FGravityProperty
Set class Property
Set VRPawn Constructor (init class Property)

unit test :

Set gamemode_BP
Set defaultPawn =>VRPawn

# 24/07/2017 16:00
Finalise VRPawn
Set HMD "Head Mounted Display"
(Htc Vive/rift)

//forward declaration
class UMotionControllerComponent;
include UMotionControllerComponent in cpp

Create Pawn and set Fcts

UFUNCTION(BlueprintCallable, Category = "VR")
  void OverridePawnValues(float PawnBaseEyeHeight = 0.f, float FOV = 110.f, float CapsuleHalfHeight = 96.f, float CapsuleRadius = 22.f,
   FVector CapsuleRelativeLocation = FVector(0.f, 0.f, -110.f),
   FVector SceneLocation = FVector(0.f, 0.f, -110.f), FVector LeftControllerLocation = FVector(0.f, 0.f, 110.f),
   FVector RightControllerLocation = FVector(0.f, 0.f, 110.f));

UFUNCTION(BlueprintCallable, Category = "VR")
  void RotatePawn(float RotationRate = 1.f, float XAxisInput = 0.f, float YAxisInput = 0.f);

UFUNCTION(BlueprintCallable, Category = "VR")
  bool IsHMDWorn();

UFUNCTION(BlueprintCallable, Category = "VR")
  void PrintDebugMessage(FString Message, bool OverwriteExisting = false, float Duration = 5.f, FColor Color = FColor::Red);

unit test :

Set gamemode_BP
Set defaultPawn =>VRPawn

# 24/07/2017 16:45
Modification licence projet
GNU Gpl V3

# 09/02/2018
Update 
VRPawn
VRGrabber
VRTeleporter
