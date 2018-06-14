You should be using the github repository for coordinating the work amongst your team. As such, you should be regularly checking in and out code. The final commit of your code must be received by 11:59 pm on Tue June 12.

Create a short writeup about your project and place it in README.md. If you have used code or ideas from other people, please attribute their work. It’s fine to use code from online, but you must reference where you got the code, and what changes you made. It should be clear what you did, and what others did.

The writeup should also include a short description of what work was performed by each member of the team.

**GOAL AND DESCRIPTION OF PROJECT**

The goal of this project is to be able to brute-force unlock any master-lock. We aimed to achieve this goal by using a stepper motor to turn the dials of the lock and a solenoid to open the shackle. We were able to use the stepper motor to precisely turn the dials of the lock to specific increments on the lock, but we found that the solenoid was too weak to lift the shackle. Therefore, we decided to use a second stepper motor with a pulley mechanism to open the shackle, but ultimately this too was insufficient. Without the ability to mechanically open the shackle, we were not able to effectively brute-force unlock the master lock since brute-forcing requires the ability to check if the combination is correct by attempting to open the shackle. However, we were able to implement code to be able to brute force a lock although realistically, the code would require a lot of time.

Additionally, we researched methods on how to brute-force a master lock efficiently. Some [methods](https://samy.pl/master/master.html) advertised the ability to "crack any master lock in eight tries or less". However we found that these methods required the ability to control force on the shackle which we were not able to implement in this project.

**MATERIALS USED**

- 1 x [4-Phase 12VDC 0.4 A Unipolar Stepper Motor](https://bit.ly/2xFiOii)

- 2 x [L298N Motor Drive Controller Board](https://www.amazon.com/gp/product/B014KMHSW6/ref=oh_aui_detailpage_o01_s00?ie=UTF8&psc=1)

- Additional Stepper Motor obtained from Chris Gregg (unknown model)

- 3D Prints (see details below)

- Generic chain and hook from Home Depot


**3D PRINTS**

The 3D prints used in this project can be found under the directory "3d-stl-files". We used Samy Kamkar's 3D prints for project [Combo Breaker](http://samy.pl/combobreaker/) as a baseline, and we made further personal modifications to fit the scope and physical dimensions of the components in our project. 

**DEMO VIDEO**

See below for a video of the demo. As seen in the video, we were able to configure our stepper motor with code to turn the lock dial to exact combinations. We also set up a mechanism with the second stepper motor with the use of the hook and chain to pull the lock shackle upwards. However, as seen in the video, this was unsuccessful because not enough force (torque) was generated from the stepper motor. 

[![Alt text](https://img.youtube.com/vi/DToVxfMl1qU/0.jpg)](https://www.youtube.com/watch?v=DToVxfMl1qU)

**WORK DONE BY EACH MEMBER OF THE TEAM**

*ZAYA*

Zaya found, modified, and printed all the 3D parts required for this project. She also wrote code to precisely turn the stepper motor to the exact increments of the lock. 

*JOSH*

Josh did work on trying to use the second stepper motor with a pulley mechanism to lift up the shackle. He also wrote code for brute-forcing the lock. 


