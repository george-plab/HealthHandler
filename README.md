# Health Component Replicated plugin


![Alt Text - How To setup](/Doc/Slide1_title.jpg)

# Features
This component updates health and it calculates if the player is dead, each me the player 
takes damage. You only need to set up the damage source such as Projec le, damage zone  
or item pickup, and make cast to this component or Character class to get target actor  
damage (or actor to heal). This component does the rest. Furthermore, it is ready for online  
replica on.   
includes a lots of sample in demo.

If you need any custom feature please let me know in my discord server:
[VicAgent Discord request channel](https://discord.gg/Vcp8pPQ2) 

## Table of Contents
- [Installation](#installation)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)
- [Contact](#contact)


## Installation
### From Engine
1. In Epic Launcher Search in your Fab Library Health Component Plugin and click Install engine.

2. This is all.

### From Github
1. Select Release Branch.
2. Download ZIP from Code Button and unzip in /Plugins/HealthHandler/ Folder
3. Create Unreal C++ Project or add a c++ actor to your BP Project
4. Paste  /Plugins/HealthHandler/  forlder into the your root project folder.
5. The folder Plugins  estructure: 

- Plugins
    - |--OtherPlugins
    - |-- ...
    - |-- ...
    - |--HealthHandler (name equal descriptor name)
        - |--Binaries
        - |--config
        - |--Content
        - |--intermediate
        - |--Resosurces
        - |--Sources
        - |--HealthHandler (module maybe any name)
            - |--Private
            - |--Public
            - |-- HealthHandler.build.cs (Equal module name)
        - |--other modules…
        - |-- HealthHandler.uplugin  (descriptor )
## Usage
### Technical details
1. Health component for any actor, pawn or character
2. Getters:
    - GetHealth, GetHealthMax, GetArmor, GetArmorMax 
    - GetHealthBarPercent, GetArmorBarPercent: Fast access percent to widget for health and armor. 
    - CalculeIsDead: return a boolean (Health<=0)
    - CalculeHasArmor: return a boolean (Armor>0) 
3. Functions:
    - UpdateHealth(delta):  Health+=delta in server. Clamp to MaxHealth
    - UpdateMaxHealth(delta):  MaxHealth+=delta    
    - UpdateArmor(delta):  Armor+=delta in Server . Clamp to MaxArmor
    - UpdateMaxArmor(delta): MaxArmor+=delta
    - Heal(delta): Call UpdateHealth, not Allows negatives Deltas. (Recomended use for healig pickups)
    - FixArmor(float delta) idem for armor (Recomended use this for Fix armor pickup o fix armor zone)
    - SetupAutoHealthRegen(Period, healamt): Setup healamt every Period seconds when call AutoHealthRegen()
    - AutoHealthRegen(): Call a timer in loop, it'll call UpdateHealth(Healamt) until health>=HealthMax
   
   
4. Event Dispatchers:
    - OnHealthChanged: This Event fire every time The owner takes damage from "Apply damage" interface,
    this event fires when the health changed for any razon and any source. Useful for Widget Health
    - OnDeath: This event fires when Health reachs to zero. Useful to call a multicast RPC for death anim.
    (this multicast RPC for death animation is included in demo )
    - OnWounds: This event fires whenever take damage from any source. Useful for RPC Effects Multicast.
    - OnArmorChanged: This event fires whenever the armor take damages or it is fixed. useful for widget armor.
    - onHealing: This event fires whenever the owner heals from any source. Useful for RPC Effects Multicast.
5. 

![Event and Functions](/Doc/Slide5_EventFunc.jpg)

### Features 
![Features 2](/Doc/Slide2_Features.jpg)
![Features 3](/Doc/Slide3_Features.jpg)
### Setup
In your Actor (or PAwn or Character),  component section search Health...Add Health component (Named Health).
You can setup in detail panels at healthcomponent category the next properties:

1. Health: current Health value
2. HealthMax: max value, You can modify this value in game like a RPG  
3. Armor
4. ArmorMax
5. Default max Value: ( to clamp ArmorMax and Healthmax for safety )
6. HasAutoHealig:  if true auto healthregen is enable and you can setup the below properties:
7. Delay To Auto Health Regen: A delay after no damage is aplied, the health regen Start
8. Health Regen Period: Health regen is a loop with this period
9. Health Regen Amount: every period health regen apply this heal amount until Health>=HealthMax


![How To setup](/Doc/Slide4_Setup.jpg)



![UI](/Doc/Slide6_UI.jpg)

![Deathevent](/Doc/Slide7_Deathevent.jpg)


### Demo

![Demo Description](/Doc/Slide8_Demo.jpg)
![Pickups](/Doc/Slide9_Pickups.jpg)
![Damage Zone](/Doc/Slide10_DPS.jpg)

please if you see any issue please let me know about in DM on my discord chanel
You can contact me  in my discord server [VicAgent Discord issues channel](https://discord.gg/TGQzgzC6) 

### Contact

You can contact me  in my discord server [VicAgent Discord ](https://discord.gg/Kkn9bJKr) 


## Contributing
1. Fork the repository.
2. Create a new branch: `git checkout -b feature-name`.
3. Make your changes.
4. Push your branch: `git push origin feature-name`.
5. Create a pull request.

## License
This project is licensed under the [MIT License](https://mit-license.org/).

