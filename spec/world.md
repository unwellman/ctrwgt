# Counterweight World Representation

Brainstorm techniques for world design

## Geometry

Being a space game with a rotatable camera, Counterweight will need to have
continuously defined geometry (e.g. circles which are smooth, polygons which
have flat colliders at arbitrary angles). Axis-aligned rectangles will have
basically no place because (in principle) the whole world will be constantly
rotating to provide artificial gravity in a physically realistic way.

### Vector graphics

One way of implementing this would be using some standard vector graphics format,
enabling the use of visual editors. Collision geometry and graphical properties
(like materials, which can be filled in with a pixel renderer) could be
separate layers on a vector graphics file, keeping related data together.

    - Tooling would take a great deal of effort
    - Enables the use of visual tools like Blender
        - Faster iteration with higher upfront cost
        - Feasible to onboard collaborators to the workflow

The intimidation factor of venturing outside the terminal workflow is
significant, but in the interest of bringing in collaborators, this seems to be
the way to go.

### Text-based specification

The approach in the Python m\_ake system was to use YAML to specify the positions
of various collision primitives, e.g. circles, splines in a text-only format.

    - Tooling is easy to set up
    - Lack of visual tools means:
        - Slower iteration and feedback time
        - Much less accessible workflow for collaborators
    - YAML's tree-like maps lend themselves cleanly to the mathematical
    properties of a scene graph
    - Designing a system from the ground up means:
        - High likelihood for design debt and breaking changes

The biggest problem with this approach is that the specification for how data
will be laid out is too large of a design project to undertake before even
creating any assets. The design will inevitably undergo many breaking changes
and ultimately outprice the initial ease of tooling.

### The secret third option: tile geometry

    - Ease of implementation, workflow, and collision handling
    - Pixel art assets don't lend themselves to being stretched like this tile
    system would demand



## In-world actors

