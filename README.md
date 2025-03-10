# Project Title
Health Component Replicated plugin

![Alt Text - How To setup](/Doc/Slide1_title.JPG)

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
### Features 
![Features 2](/Doc/Slide2_Features.JPG)
![Features 3](/Doc/Slide3_Features.JPG)
### Setup
![How To setup](/Doc/Slide4_Setup.JPG)

![Event and Functions](/Doc/Slide5_EventFunc.JPG)

![UI](/Doc/Slide6_UI.JPG)

![Deathevent](/Doc/Slide7_Deathevent.JPG)


### Demo

![Demo Description](/Doc/Slide8_Demo.JPG)
![Pickups](/Doc/Slide9_Pickups.JPG)
![Damage Zone](/Doc/Slide10_DPS.JPG)

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

