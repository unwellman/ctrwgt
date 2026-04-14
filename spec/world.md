# Counterweight World Representation

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

### File formats

World geometry assets will be specified in SVG.


### Dependencies

Parsing SVG world specifications will be done with
[nanosvg](https://github.com/memononen/nanosvg),
which turns every piece of geometry into a 3rd-degree Bézier curve.
Because Counterweight's world geometry will rely on *very* large
(tens to hundreds of thousands of pixels) circles,
some modifications will need to be made to keep the approximation error not
larger than 1 pixel.

Modifying the internal nanosvg function `nsvg__parseCircle` to reflect
Bézier control points computed in
[this](https://spencermortensen.com/articles/bezier-circle/)
article will decrease the approximation error considerably without needing to
change the strictly cubic Bézier approach. Further error reduction may need to
be done on a case-by-case basis, including workarounds like chopping up massive
circles into arcs smaller than 90°.





