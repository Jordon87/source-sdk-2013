// Created by Crowbar 0.74

$modelname "weapons/v_1187beretta_dual.mdl"

$bodygroup "1187_Weapon"
{
	studio "reference.smd"
}


$surfaceprop "default"

$contents "solid"

$illumposition 12.453 -3.224 -9.997

$cdmaterials "models\Weapons\v_firstperson\"
$cdmaterials "models\Weapons\v_beretta\"

$attachment "muzzleright" "gun1" 0 -3.55 1.35 rotate 0 -90 90
$attachment "muzzleleft" "gun" 0 -4 1.35 rotate 0 -90 90
$attachment "eject2" "gun" 0.4 2 1.5 rotate -15 0 180
$attachment "eject3" "gun1" -0.4 2 1.5 rotate -15 -180 -180
$attachment "1187eyefix" "viewbone" 0 0 0 rotate 0 -90 0

$cbox 0 0 0 0 0 0

$bbox -7.476 -12.97 -19.209 31.011 12.261 2.765

$definebone "BASE_dummy" "" 1.337008 5.708662 -9.448819 0 0 0 0 0 0 0 0 0
$definebone "L.arm_dummy" "BASE_dummy" 4.533402 -2.752522 11.95603 -6.98109 0 0 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Clavicle" "L.arm_dummy" 0 0 0 60.380471 -89.545112 5.202399 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_UpperArm" "ValveBiped.Bip01_L_Clavicle" 6.028145 0.000001 0 -18.548477 1.442593 -74.568857 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Forearm" "ValveBiped.Bip01_L_UpperArm" 11.692559 0.000006 0 -12.582037 -70.003756 6.778264 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Ulna" "ValveBiped.Bip01_L_Forearm" 5.740851 0 0 0.000001 0.000004 0.043314 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Wrist" "ValveBiped.Bip01_L_Forearm" 11.481703 -0.000002 -0.000002 0.000001 0.000004 0.08004 0 0 0 0 0 0
$definebone "Dummy02" "ValveBiped.Bip01_L_Forearm" 11.481693 -0.000039 -0.000024 -9.313771 20.540594 64.795163 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Hand" "Dummy02" -0.000006 0 0.000003 0.000001 -0.000002 0.000002 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Finger4" "ValveBiped.Bip01_L_Hand" 4.035851 0.29576 -1.32189 -0.276966 -110.262804 -16.764976 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Finger41" "ValveBiped.Bip01_L_Finger4" 1.312562 0 0 0 -37.819058 -0.000001 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Finger42" "ValveBiped.Bip01_L_Finger41" 0.729362 0.000002 0 0 -17.119806 -0.000002 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Finger3" "ValveBiped.Bip01_L_Hand" 4.032049 0.427219 -0.376758 5.53065 -85.821177 -15.912012 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Finger31" "ValveBiped.Bip01_L_Finger3" 1.539093 0.000002 0.000001 -0.000002 -67.001683 -0.000004 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Finger32" "ValveBiped.Bip01_L_Finger31" 1.196314 -0.000003 0.000002 7.333455 -38.699695 -0.000001 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Finger2" "ValveBiped.Bip01_L_Hand" 4.192043 0.645181 0.516859 5.643694 -84.782455 -14.434754 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Finger21" "ValveBiped.Bip01_L_Finger2" 1.719576 0 0 0 -64.923632 -0.000003 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Finger22" "ValveBiped.Bip01_L_Finger21" 1.209185 -0.000003 0.000001 -0.000001 -33.60519 -0.000003 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Finger1" "ValveBiped.Bip01_L_Hand" 3.650038 0.146238 1.26871 4.024573 -15.018771 3.21968 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Finger11" "ValveBiped.Bip01_L_Finger1" 1.719421 -0.000002 0 -0.504258 -20.986407 -9.285524 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Finger12" "ValveBiped.Bip01_L_Finger11" 1.099659 -0.000004 0 0.186728 -21.900394 -4.791017 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Finger0" "ValveBiped.Bip01_L_Hand" 0.936251 -0.656 0.984589 -57.610508 -60.343109 -50.836364 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Finger01" "ValveBiped.Bip01_L_Finger0" 1.789782 -0.000004 0 -10.221742 68.957993 2.687454 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Finger02" "ValveBiped.Bip01_L_Finger01" 1.206997 -0.000001 -0.000002 9.669693 18.521436 -0.68246 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Elbow" "ValveBiped.Bip01_L_UpperArm" 11.692557 0.000006 -0.000001 0.000001 -0.000001 -0.000002 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_L_Bicep" "ValveBiped.Bip01_L_UpperArm" 5.56 0.000002 0 0.000001 -0.000001 0.640911 0 0 0 0 0 0
$definebone "R.arm_dummy" "BASE_dummy" -7.866306 -0.1518 10.831763 19.079609 0 0 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Clavicle" "R.arm_dummy" 0 0 0 34.660222 -70.22125 -175.461563 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_UpperArm" "ValveBiped.Bip01_R_Clavicle" 6.028143 -0.000002 0 -5.795636 12.474608 93.530316 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Forearm" "ValveBiped.Bip01_R_UpperArm" 11.692552 0 -0.000001 1.640318 -67.184507 0.833366 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Ulna" "ValveBiped.Bip01_R_Forearm" 5.740849 0.000002 -0.000001 -0.000002 0.000007 0.174121 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Wrist" "ValveBiped.Bip01_R_Forearm" 11.481705 0.000007 -0.000002 -0.000002 0.000007 0.322001 0 0 0 0 0 0
$definebone "r.arm.wrist_dummy" "ValveBiped.Bip01_R_Forearm" 11.481701 -0.000004 0.00001 -71.355526 -52.697102 168.78299 0 0 0 0 0 0
$definebone "R.hand_control" "r.arm.wrist_dummy" 0 -0.000002 0 -6.225051 -91.475276 179.749965 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Finger4" "R.hand_control" 3.735474 -0.021699 1.4323 1.889269 -71.933469 4.632369 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Finger41" "ValveBiped.Bip01_R_Finger4" 1.312563 0.000004 0 3.996668 -86.766331 1.062093 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Finger42" "ValveBiped.Bip01_R_Finger41" 0.729361 0 0.000001 0.253534 -17.181457 0.162262 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Finger3" "R.hand_control" 3.78648 -0.0191 0.5177 -2.311542 -67.244613 7.860528 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Finger31" "ValveBiped.Bip01_R_Finger3" 1.5391 0 0 4.200808 -99.532119 -3.403255 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Finger32" "ValveBiped.Bip01_R_Finger31" 1.196321 0 0 0.602863 -29.065805 5.731185 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Finger2" "R.hand_control" 3.89917 0.157001 -0.360001 0.316271 -68.134873 -4.383009 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Finger21" "ValveBiped.Bip01_R_Finger2" 1.719576 0 0.000002 15.112907 -94.153598 16.968085 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Finger22" "ValveBiped.Bip01_R_Finger21" 1.209177 0 0 -8.207566 -44.070874 -3.090532 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Finger1" "R.hand_control" 3.650072 0.146201 -1.268701 1.01264 -1.758237 -0.493603 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Finger11" "ValveBiped.Bip01_R_Finger1" 1.719423 0 0 0.421978 -29.182905 0.636674 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Finger12" "ValveBiped.Bip01_R_Finger11" 1.099669 0 -0.000001 2.022138 -41.183677 1.038491 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Finger0" "R.hand_control" 0.936268 -0.656 -0.9846 40.07231 -35.168832 57.392324 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Finger01" "ValveBiped.Bip01_R_Finger0" 1.789785 -0.00001 0 -5.962421 51.264763 2.864152 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Finger02" "ValveBiped.Bip01_R_Finger01" 1.207001 0 0 3.615428 26.817445 11.861026 0 0 0 0 0 0
$definebone "ValveBiped.Rthumb_muscle4" "ValveBiped.Bip01_R_Finger02" 0.517803 0.000155 -0.000001 -9.885752 27.818461 3.63192 0 0 0 0 0 0
$definebone "ValveBiped.Rthumb_web_bulge" "ValveBiped.Bip01_R_Finger0" 1.112814 0.956167 -0.926425 -7.999979 19.500002 37.300008 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Elbow" "ValveBiped.Bip01_R_UpperArm" 11.692553 0.000001 -0.000002 -0.000002 -0.000003 -0.000002 0 0 0 0 0 0
$definebone "ValveBiped.Bip01_R_Bicep" "ValveBiped.Bip01_R_UpperArm" 5.559999 0 0 -0.000002 -0.000003 -2.615269 0 0 0 0 0 0
$definebone "gun.wrist.arms.control1" "BASE_dummy" -7.065176 -23.929838 4.354649 0 0 0 0 0 0 0 0 0
$definebone "gun.control1" "gun.wrist.arms.control1" 0.571858 -6.154856 2.219772 0 0 0 0 0 0 0 0 0
$definebone "gun1" "gun.control1" 0.240664 -2.525093 1.030116 0 1.145686 0 0 0 0 0 0 0
$definebone "Trigger.control1" "gun.control1" 0.044815 0.241676 0.800555 0 0 0 0 0 0 0 0 0
$definebone "Trigger1" "Trigger.control1" -0.320618 -0.095657 -0.432226 0 0 0 0 0 0 0 0 0
$definebone "catch.control1" "gun.control1" 0.604239 0.616493 1.228888 0 0 0 0 0 0 0 0 0
$definebone "catch1" "catch.control1" -0.202633 0.489361 -0.053002 0 2.121032 0 0 0 0 0 0 0
$definebone "Hammer.control1" "gun.control1" 0.060318 3.781357 1.22594 0 0 -60.000029 0 0 0 0 0 0
$definebone "hammer1" "Hammer.control1" -0.60808 0.860507 0.87945 0 2.202622 0 0 0 0 0 0 0
$definebone "Slide.control1" "gun.control1" 0.06357 1.742668 2.239693 0 0 0 0 0 0 0 0 0
$definebone "Slide1" "Slide.control1" 0.096938 -0.134209 0.18529 0 -1.060889 0 0 0 0 0 0 0
$definebone "Safety.control1" "gun.control1" 0.057673 3.302483 2.258661 0 0 0 0 0 0 0 0 0
$definebone "safety1" "Safety.control1" 0.49723 0.055584 -0.128004 0 3.179801 0 0 0 0 0 0 0
$definebone "clip.control1" "gun.control1" 0.030602 3.404495 -1.94367 0 0 13.77912 0 0 0 0 0 0
$definebone "clip1" "clip.control1" -0.77613 0.602617 0.115579 0.243679 0.993623 -13.777 0 0 0 0 0 0
$definebone "bullet.control1" "gun.control1" 0.042182 2.447716 1.975793 0 0 -0.000057 0 0 0 0 0 0
$definebone "bullet1" "bullet.control1" -0.432352 -0.252565 -0.020719 0 2.202622 0.000115 0 0 0 0 0 0
$definebone "gun.wrist.arms.control02" "BASE_dummy" 4.152193 -24.46451 4.276392 -11.841319 -83.814672 -4.253983 0 0 0 0 0 0
$definebone "gun.control02" "gun.wrist.arms.control02" 8.657696 -1.611799 2.798935 -2.963396 83.938606 12.223196 0 0 0 0 0 0
$definebone "Safety.control02" "gun.control02" 0.057673 5.237534 -0.068726 -0.000001 0 0.000001 0 0 0 0 0 0
$definebone "safety" "Safety.control02" 0.344049 0.481468 -0.071636 -0.000001 3.179802 0.000001 0 0 0 0 0 0
$definebone "Slide.control02" "gun.control02" 0.06357 4.103603 -0.087694 -0.000001 0 0.000001 0 0 0 0 0 0
$definebone "Slide" "Slide.control02" -0.056243 -0.134209 0.241658 -0.000001 -1.060889 0.000001 0 0 0 0 0 0
$definebone "catch.control02" "gun.control02" 0.604239 2.493399 -1.0985 -0.000001 0 0.000001 0 0 0 0 0 0
$definebone "catch" "catch.control02" -0.355815 0.973387 0.003366 -0.000001 2.121032 0.000001 0 0 0 0 0 0
$definebone "Hammer.control02" "gun.control02" 0.060318 5.790943 -1.101447 -0.000001 0 -60.000029 0 0 0 0 0 0
$definebone "hammer" "Hammer.control02" -0.761261 0.98737 1.211908 0 2.202621 0.000001 0 0 0 0 0 0
$definebone "Trigger.control02" "gun.control02" 0.044815 2.293377 -1.547667 -0.000001 0 0.000001 0 0 0 0 0 0
$definebone "Trigger" "Trigger.control02" -0.473799 0.213581 -0.355022 -0.000001 0 9.546852 0 0 0 0 0 0
$definebone "clip.control02" "gun.control02" 0.030601 5.202883 -4.271057 -0.000001 0 13.77912 0 0 0 0 0 0
$definebone "clip" "clip.control02" -0.92931 1.162403 0.036339 0.243678 0.993624 -13.777 0 0 0 0 0 0
$definebone "bullet.control02" "gun.control02" 0.042182 4.808651 -0.303473 -0.000001 0 -0.000056 0 0 0 0 0 0
$definebone "bullet" "bullet.control02" -0.043759 -0.939428 -0.150366 -0.000058 86.077753 0 0 0 0 0 0 0
$definebone "gun" "gun.control02" 0.087483 -0.164158 -1.240902 -0.000001 1.145687 0.000001 0 0 0 0 0 0
$definebone "viewbone" "" 0 0 0 0 0 0 0 0 0 0 0 0


$sectionframes 30 120

$sequence "idle01" {
	"idle01.smd"
	activity "ACT_VM_IDLE" 1
	fadein 0.2
	fadeout 0.2
	node "Ready"
	fps 30
	loop
}

$sequence "fire_right" {
	"fire_right.smd"
	activity "ACT_VM_PRIMARYATTACK" 1
	{ event AE_MUZZLEFLASH 0 "PISTOL MUZZLERIGHT" }
	{ event 10001 0 "0 EJECT3" }
	fadein 0.2
	fadeout 0.2
	snap
	fps 35
}

$sequence "fire_left" {
	"fire_left.smd"
	activity "ACT_VM_SECONDARYATTACK" 1
	{ event AE_MUZZLEFLASH 0 "PISTOL MUZZLELEFT" }
	{ event 10001 0 "0 EJECT2" }
	fadein 0.2
	fadeout 0.2
	snap
	fps 35
}

$sequence "fire_both1" {
	"fire_both1.smd"
	activity "ACT_VM_RECOIL1" 1
	{ event AE_MUZZLEFLASH 0 "PISTOL MUZZLERIGHT" }
	{ event AE_MUZZLEFLASH 0 "PISTOL MUZZLELEFT" }
	{ event 10001 0 "0 EJECT3" }
	{ event 10001 0 "0 EJECT2" }
	fadein 0.2
	fadeout 0.2
	snap
	fps 35
}

$sequence "fire_both2" {
	"fire_both2.smd"
	activity "ACT_VM_RECOIL1" 1
	{ event AE_MUZZLEFLASH 0 "PISTOL MUZZLERIGHT" }
	{ event AE_MUZZLEFLASH 0 "PISTOL MUZZLELEFT" }
	{ event 10001 0 "0 EJECT3" }
	{ event 10001 0 "0 EJECT2" }
	fadein 0.2
	fadeout 0.2
	snap
	fps 35
}

$sequence "draw" {
	"draw.smd"
	activity "ACT_VM_DRAW" 1
	{ event AE_CL_PLAYSOUND 24 "Weapon_Pistol.hammer" }
	fadein 0.2
	fadeout 0.2
	node "Ready"
	fps 30
}

$sequence "reload" {
	"reload.smd"
	activity "ACT_VM_RELOAD" 1
	{ event AE_CL_PLAYSOUND 9 "Weapon_Pistol.remove_empty" }
	{ event AE_CL_PLAYSOUND 30 "Weapon_Pistol.Clipin" }
	{ event AE_CL_PLAYSOUND 68 "Weapon_Pistol.Clipin2" }
	fadein 0.2
	fadeout 0.2
	node "Ready"
	fps 30
}

$sequence "idletolow" {
	"idletolow.smd"
	activity "ACT_VM_LOWERED_TO_IDLE" 1
	fadein 0.2
	fadeout 0.2
	transition "Ready" "Low"
	fps 40
}

$sequence "lowtoidle" {
	"lowtoidle.smd"
	activity "ACT_VM_IDLE_TO_LOWERED" 1
	fadein 0.2
	fadeout 0.2
	transition "Low" "Ready"
	fps 40
}

$sequence "lowidle" {
	"lowidle.smd"
	activity "ACT_VM_IDLE_LOWERED" 1
	fadein 0.2
	fadeout 0.2
	node "Low"
	fps 30
	loop
}

$sequence "melee" {
	"melee.smd"
	activity "ACT_VM_SWINGHARD" 1
	{ event AE_WPN_MELEEATTACK 6 "" }
	fadein 0.2
	fadeout 0.2
	node "Melee"
	snap
	fps 30
}

$sequence "melee_hack" {
	"melee_hack.smd"
	activity "ACT_VM_HAULBACK" 1
	{ event AE_WPN_FRAGATTACK 6 "" }
	fadein 0.2
	fadeout 0.2
	node "MeleeHack"
	snap
	fps 30
}

$sequence "jump" {
	"jump.smd"
	activity "ACT_JUMP" 1
	fadein 0.2
	fadeout 0.2
	node "Ready"
	fps 35
}

$sequence "crouch" {
	"crouch.smd"
	activity "ACT_CROUCH" 1
	fadein 0.2
	fadeout 0.2
	node "Ready"
	fps 35
}

$sequence "idletosprint" {
	"idletosprint.smd"
	activity "ACT_VM_SPRINT_ENTER" 1
	fadein 0.2
	fadeout 0.2
	transition "Ready" "Sprint"
	fps 35
}

$sequence "sprinttoidle" {
	"sprinttoidle.smd"
	activity "ACT_VM_SPRINT_LEAVE" 1
	fadein 0.2
	fadeout 0.2
	transition "Sprint" "Ready"
	fps 35
}

$sequence "sprintidle" {
	"sprintidle.smd"
	activity "ACT_VM_SPRINT_IDLE" 1
	fadein 0.2
	fadeout 0.2
	node "Sprint"
	fps 35
	loop
}
