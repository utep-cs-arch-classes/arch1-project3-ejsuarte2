SAVE THE SHIP GAME--> README/INSTRUCTIONS
BY: Erick Duarte

PLEASE READ BEFORE PLAYING THE GAME!!!!!!!!!!!!!! (IMPORTANT INSTRUCTIONS)


-------------->SOURCES USED<-----------------
All of Dr. Freudenthal's library files for timmer, watchdog interrupt, lcdUtils, p2switch files,
lcddraw, shapeLib files, files in h folder, abCircle files and Buzzer files from lab 2 assignment.
Also shape-motion-demo (what my game is based off of modified this project to my needs), p2sw-demo, and shape demo. ALL of Dr. Freudenthal's code was recycled and used by me in my implelentation
of this game, along with my code from lab 2.


-------------->STARTING THE GAME<--------------
To run the game first go into directory save_the_ship, once inside run the make command.
Once this command is run and all files are compiled simply run make load command.


---------------->GAME RULES/ HOW TO PLAY<-------------------
Save the ship game consists of 2 paddels one red on the left hand side and one green on the right
hand side. It is also contains a pink rectangle shape and a yellow ball, wich moves around the
arena or white frame along with the panels inside this white frame. On the top left of the white
frame the button that was pressed will be displayed while on the top right of the white frame the
Score is dispayed as the game progresses. In the middle of the arena there will be a light blue rectangle that is simply for decorative purposes as it does not interfere with the game in a ny way.
Anyhow the rules of the game are simple and as follows:

0. The green and red panels initially move up and down at a slow paced speed. However the player
can move THEM faster and in ANY direction. (using the 4 provided buttons s1..s3 explained later)

1. The player will lose the game IF either the pink sliced rectangle OR the yellow ball touch
either the left white wall behind the red paddel or the right white wall behind the green paddel.

2. The player can ALSO loose if EITHER the red panel OR green panel touch either one of these
walls. (left white wall or right white wall as expained in rule 1.)

3. The pink sliced rectangle, AND the yellow ball move freely inside the white frame in different
directions for as long as the game is active.

4. ONLY the yellow ball reacts to a collision between the red and green panel the pink rectangle
does NOT.! (purposely done so for higher dificulty)

5. When the ball is hit by either panel, its Speed increases by 8 times the speed it was already
going, and the ship's speed increases by 2 EVERY time it either touches the TOP white wall
OR BOTTOM white wall as it does not react to panel collision only to the collision of these walls.

6.The only way to save the ship (Keep the pink rectangle from touching the white left wall or the
right one) is by hitting the YELLOW ball with EITHER the red or green paddel. This will cause the
ship to ALWAYS go in the opposite direction. So if it's headed toward either one of these walls
(left or right white walls) and the yellow ball is hit before the ship reaches any of these walls,
the ship will start moving in the exact opposite direction avoiding collision with either one of
these walls.

7. The Objective of the game is to attain the highest score possible until the game is lost.

---------------->USING THE PANELS<-------------------
As stated earlier both the red and green panels can move in ANY direction with increasing speed.
This can be done using the 4 buttons provided on the green board of the msp430  s1, s2, s3 and s4.
The game will play a sound every time any one of these buttons is presses and it will be a
different sound each time this happens.

Notice that there are four directions to move the panels and they are either Up, Down, Left,
or right and 2 panels , both of which the player has control over. These panels move at the same
time meaning that if the player moved the position of the red panel that he/she also moved the
position of the green panel. (Done for higher dificulty)
Let's begin,  the commands of each button are as follows:

*******IMPORTANT**********
********NOTE: To increase speed of pannel movement simply press the button of desired direction
consecutively (multiple times) for movement at higher speed.

s1. Moves Both the RED and Green panel UP Regardles of what direction the panels are already going.

s2. Moves red and green panel DOWN regardless of what direction they were previously moving at

s3. (CAREFULL) This button NARROWS the distance between the red and green panel brigning them a
distance closer to each other, if used to frequently it can cause the panels to collide with each
other bringing about the opposite effect and throwing the ball or object out of bounds.

s4. (EXTRA CAREFULL) This button does the exact opposite, it WIDENS the distance between the green and red panel regardless of what position they are in. SO if at the start of the game this button is presses the player has already lost as the red panel ends up touching the left white wall and the
green panel ends up touching the right white wall. (I always fall for this..) How ever it is useful
for moving back to a previous position.

------------------->PLAY THE GAME<------------------
You are now ready to play a game of your own!! run make load on your msp430 and let the games below. Whenever you loose a blue Screen will show up with a message stating the game is over all sounds
will be turned off CPU will be off and your score will be displayed on the blue screen.