# Interactive-Interior-Design-Project
Computer Graphics Course Project- An interactive interior design program for a multi-room environment
This is a Visual Studio C++ Project

•	The program has 3 rooms. Adjacent rooms are connected by sliding doors.
•	There are total 4 different types of furniture – Sofa, Table, Chair, Shelf.
•	Avatar has a simple cylindrical body, a sphere as head and 2 small spheres - on head and on body respectively (to identify the front side), with head turning capability.
•	The walls and doors are thick; walls, doors, floor, furniture are all texture mapped. 
•	Furniture can be positioned both in world view mode and Avatar view mode: Translate, Rotate, Scale, Extrude
•	Collision detection is implemented for all objects restricting movement, scale etc. upon colliding with walls/ other objects.

Interface:

Movement of Avatar:
•	When in world view mode, use up-down arrow keys to move avatar forward or backward, right-left keys to turn the Avatar right or left.
•	Use ‘F4’ key to switch to Avatar’s first person view. Arrow keys control the movement as similar to above.
•	In first person view mode, hold ‘Left mouse button’ and move the mouse to turn only the head of the avatar. The Avatar can move only when its head returns to forward position. Press ‘F3’ to bring the head to face forward.
•	Mouse is used to only turn head, while arrow keys turn the whole body of Avatar.
•	Collision detection is implemented such that Avatar cannot penetrate walls, doors and floor. Use ‘F7’ key to allow or disallow Avatar to penetrate the furniture in the rooms.
•	The Avatar cannot be moved if any of the furniture is selected. Press ‘F2’ key to deselect all furniture (if any of them is selected), so that arrow keys now can be used to move Avatar.

Furniture Control:
•	Furniture can be positioned both in world view mode and Avatar view mode.
•	Only furniture in the room where Avatar is present can be positioned. Move Avatar to the room where furniture is to be modified.
•	Press ‘F1’ key to enter select mode and select a furniture in the room, Use ‘F1’ again to select next furniture in the room. Then use one of the following on the selected furniture:
		‘t’: Translate mode- Use arrow keys to move the furniture in x-z plane.
		‘s’: Scale mode- Use arrow keys to scale in x-z plane.
		‘r’: Rotate mode- Use right-left arrow keys
		‘e’: Extrude mode- Use up down arrow keys.
•	Press ‘F2’ key to deselect all furniture.
•	Collision detection is implemented- no penetration with walls and other furniture.


Doors:
There are two doors. The Avatar can move from one room to other only through these doors.
•	Use ‘F5’ key to open/close door1
•	Use ‘F6’ key to open/close door2

Camera Control:
Use ‘F4’ to move to Avatar view mode or back to world view mode.
•	World View Mode:
o	The camera is looking at the center of the room which has the Avatar.
o	Hold ‘Left mouse button’ and move the mouse change the look from position- up/down to move along longitude, right/left to move along latitude.
o	Hold ‘Right mouse button’ and move the mouse up/down to zoom in/ zoom out.
o	When the Avatar moves from one room to other, the lookAt position is updated to the center of the new room which contains Avatar.
•	Avatar view mode:
o	The camera is attached to the eyes of the Avatar.
o	Control is explained in Movement of Avatar above.

Control Summary:

F1: Select furniture mode/ select next furniture in the room. Use ‘t’, ‘s’, ‘r’, ‘e’ for ‘Translate’, ‘Scale’, ‘Rotate’, ‘Extrude’ respectively.
F2: Deselect all furniture.
F3: Bring head of Avatar to face forward (if head is turned in Avatar view mode)
F4: Switch camera view between Avatar view mode and world view mode.
F5: Open/ close door 1
F6: Open/close door 2
F7: Enable/ disable penetration of furniture by Avatar and vice versa.

Mouse - to control the camera in world view mode, and head turning in Avatar view mode.

