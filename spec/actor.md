## Actor: game object primitive

Attributes:
    - Message queue
    - Position in game world
    - (optional physics component
    - (optional) sprite
    - (optional) audio component


Functions:
    - Update all components (with time-step)
    - Receive a message from a component
    - Receive a message from another Actor
    - Broadcast message to all components


## Actor\_component: uniform requirements for components

Attributes:
    - A reference to the actor

Functions:
    - Receive a broadcast from actor

## Actor\_message: standard messaging interface

Attributes:
    - Enum describing type of message (i.e. which component it came from)
