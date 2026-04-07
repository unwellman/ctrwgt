# File I/O

## Images, audio
Rely on built-in SDL functionality


## Sprite animation behavior trees
Previously implemented using YAML, which would add a dependency on LibYAML.


## Keybind representations
YAML maps are a natural pick, using C enums where applicable


## World representation
Tentatively:
    - Collision geometry will be provided by Bezier curves from an SVG parser
    - Materials and visuals TBD; probably a second layer of SVG

