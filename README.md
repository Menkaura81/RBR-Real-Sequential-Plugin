# RBR Real Sequential Plugin
Plugin for providing realistic sequential shifter logic to RBR. Neutral and Reverse are locked, you can only engage them if you press the safety trigger in your sequiential stick.
It uses some code from RBRHShift (original version by Racer_S)

## **THERE IS A BUG CONFIGURING YOUR CONTROLS!!: 
In the ingame config menu, you must have Enable = False while configuring your controls, then you can set it to True and save settings

### How to build it yourself:

You can build the dll yourself. Open the RBRTestPlugin.sln with Visual Studio 2022 and compile it. You must have a directory in C: called "C:\Games\Richard Burns Rally Original\Plugins\". There you will find the compiled dll wich will be called RealSequentialPlugin.dll, copy it to your RBR plugin directory and you are ready to go.
