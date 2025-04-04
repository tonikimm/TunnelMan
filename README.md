Actor Class
-Serves as a base class for all game objects, encapsulating shared properties such as visibility, position, and alive status
-doSomething() (pure virtual): Enforces implementation in derived classes because each game object needs unique behavior during its turn. It is pure virtual since every derived class has unique behavior.
-moveTo(int x, int y): Moves the actor while verifying coordinates to valid bounds. Defined here because all actors may move.
-isAlive() / setDead(): Provides status management for all game objects. Defined here as all objects must handle liveliness.
-The Actor class was tested to verify its foundational behaviors as a base class. Tests confirmed that the constructor properly initializes attributes such as position, visibility, and liveliness (isAlive). The moveTo function was tested with edge cases to ensure it verified coordinates within the game boundaries. Additionally, since doSomething is pure virtual, the tests ensured derived classes properly implement it. Attempts to instantiate or call doSomething on an abstract Actor resulted in compilation or runtime errors, confirming the class enforces the intended inheritance structure.
 
Human Class
-A specialized base class for entities with health, like the TunnelMan and Protesters
-moveInDirection(Direction): Handles directional movement while avoiding obstacles. Defined here since movement mechanics are common to humans.
-isAnnoyed(int hitpoint) (pure virtual): Forces derived classes to implement health-specific behaviors. It is pure virtual to ensure specific annoyance handling with tunnelman vs protestor
-getHealth() / setHealth(int amt): health-related accessor and mutator for derived classes
-The Human class was tested for its shared behaviors among human-like actors. Tests confirmed the constructor correctly initializes attributes such as health. Movement behavior via moveInDirection was tested in scenarios involving obstacles like boulders and Earth, ensuring proper interaction. The isAnnoyed function, being pure virtual, was tested indirectly by ensuring derived classes override it and handle health reduction appropriately. The health getter (getHealth) and setter (setHealth) was validated for accuracy and boundary cases, such as health reduction to zero.

TunnelMan Class
-Represents the player character with additional inventory (squirts, sonar, gold).
-doSomething(): Handles player actions (movement, firing, using items). Defined here as it is player-specific logic.
-shoot(): adds a new squirt object to the list of actors in the world.
-inc(int id): Updates inventory based on item pickups
-isAnnoyed(int hitpoint): Overrides Human's virtual function to define TunnelMan-specific reactions to being yelled at
-The TunnelMan class was tested as the player character. Its doSomething method requires extensive testing to cover all possible actions, including movement in all directions, interactions with Earth, boulders, and items, as well as the use of tools like squirts and sonar. Tests for shoot confirmed proper squirt instantiation and behavior in various scenarios, such as hitting obstacles or Protesters. Inventory management through inc was validated by ensuring the player's inventory updates correctly upon collecting items. The isAnnoyed function handles health reduction accurately and transitions the player to a dead state when health reaches zero.

Earth Class
-Represents destructible terrain.
-doSomething(): defined but does nothing because Earth doesn't act after being initialized,  trivial override

Boulder Class
-Represents falling boulder that can interact with other actors.
-doSomething(): Implements the logic for boulder stability, falling, and interactions.
-hitMan(): Applies damage to actors in the path of the fall
-The Boulder class was tested for its lifecycle, from the stable state to falling and interaction with actors. Tests verified that stable boulders transition to falling correctly after losing ground support. Ensure falling boulders interact properly with other objects, such as damaging TunnelMan or Protesters, and stop falling when they reach an obstacle or the game boundary. The hitMan function was tested for correct damage application to TunnelMan and Protesters.

Squirt Class
-Represents projectiles fired by TunnelMan.
-doSomething(): Manages movement and interaction logic for the projectile.
-annoyProtesters(): Damages nearby protesters.
-Tests confirmed that squirts travel correctly in their given direction and stop upon hitting an obstacle, Earth, or Boulder. The doSomething function was tested to ensure squirts properly reduce travel distance and disappear after traveling their maximum range. The annoyProtesters function was validated by confirming that Protesters within the squirt’s radius are affected correctly, with their health reduced.

Item Class
-Abstract base class that represents collectible or interactive items.
-doSomething() (pure virtual): Enforces item-specific logic. It is pure virtual to enforce unique item behavior in derived classes.
-disapTimer(int ticks): Manages lifespan for temporary items.
-The Item class and its derived classes was tested for their lifecycle and interaction behaviors. For Barrel, I tested that it becomes visible when TunnelMan approaches within range and is collected correctly. Gold was tested for both player pickup and interaction with Protesters as bribes. Temporary items like Goodie, Sonar, and WaterPool were tested for proper lifespan management with the disapTimer function and correct interactions with TunnelMan.

Barrel & Gold & Goodie & Sonar & WaterPool Classes
-Represents specific items with unique behaviors
-doSomething(): Implements behavior like player pickup, visibility toggling, or interactions.
-All doSomething() implementations override the pure virtual function in Item

Protester Class
-Represents enemies with complex movement, health, and interaction logic.
-doSomething(): Implements protester actions, including movement and reactions to player actions.
-isAnnoyed(int hitpoint): Handles health reduction and state changes.
-beBribed(): Handles gold-based interactions.
-beStunned(): Implements stun mechanics.
-The Protester class and derived classes were tested for movement, health, and interactions with TunnelMan and the environment. Pathfinding behavior, including straight-line movement and navigation to the exit, was validated under different scenarios. The doSomething method was tested for actions like yelling at TunnelMan, receiving bribes, and getting stunned. Health management through isAnnoyed and interactions like turning corners or detecting the player was thoroughly tested. 

Regular Protestor Class
-Derived from protestor class
-Has smaller radius to detect tunnelman

Hardcore Protestor Class
-Derived from protestor class
-Has bigger radius to detect tunnelman

StudentWorld Class
-Manages game state, interactions, and overall game logic
-init(), move(), cleanUp(): Implements core game lifecycle.
-Utility functions (e.g., addActor, detectNearActors) manage actors and interactions.
-Initialization tests confirmed that all actors and game elements are created correctly. The move function was tested for its ability to update game state, including calling doSomething for each actor and handling interactions, collisions, and game-ending conditions. Tests for addProtesters and addGoodies confirmed that the correct number and type of actors are added based on game level. Utility functions, such as removeDirt, isWalkable, and interaction detection (playerTooClose or protesterTooClose), were validated under edge cases and typical scenarios. Finally, proper cleanup of resources was ensured after each level.
