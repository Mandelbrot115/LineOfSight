# LineOfSight
Line Of Sight game implemented by Alex Sadier-Gane in C++ using OneLoneCoder's "PixelGameEngine".
The engine can be found [here](https://github.com/OneLoneCoder/olcPixelGameEngine) and provides useful functions such as drawing to the windows console or detecting when a particular key is pressed. 

Descripion: 
You find yourself being hunted by a UFO on an alien planet. The gravity is weird there, and your ship is unnable to escape the sky's limits as a result. You are stuck with your ship's two thrusters as the only defence against the UFO's deadly laser.

The laser rotates about a fixed point, and can randomly switch direction at anytime (with small probability). The rotation speed will gradually increase upto a maximum.

If your ship becomes collinear with the laser, ie. it has a line of sight on the ship, the laser will fire automatically and inflict damage on the ship.
If the damage is large enough, one of your thrusters will break and reduce the maneuverability of your ship. If both thrusters break, you fail.

Goal: Avoid being in a line of sight with the laser. Survive as long as possible. 

My C++ code for the game, and a few screenshots are available.
