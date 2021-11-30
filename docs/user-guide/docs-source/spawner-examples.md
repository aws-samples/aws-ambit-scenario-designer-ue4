# Examples

<TODO: Update all images to reflect new behavior and interfaces>

## Spawn on Surface Examples

The following examples help illustrate the effect of each of the **Spawn on Surface** parameter. Each example uses the scene detailed below.

![Annotated scene](./images/ambit-spawner-examples/Scene_Annotation.jpg)

### Example #1

Placement by surface name only.

![Example 1](./images/ambit-spawner-examples/Example_1.png)


### Example #2

This example is the same as above with only the **Random seed** value changed. Not only does this result in different placement of the boxes, it also results in a different density, randomly chosen from within the specified range.

![Example 2](./images/ambit-spawner-examples/Example_2.png)

### Example #3

Placement by tag only.

![Example 3](./images/ambit-spawner-examples/Example_3.png)

### Example #4

Placement requiring both surface name **and** tag to match the provided values.

![Example 4](./images/ambit-spawner-examples/Example_4.png)

### Example #5

Placement requiring either surface name **or** tag to match the provided values.

![Example 5](./images/ambit-spawner-examples/Example_5.png)

### Example #6

Placement requiring both surface name **or** tag to match the provided values, and with rotation restricted to one rotation (90 degrees).

![Example 6](./images/ambit-spawner-examples/Example_6.png)

### Example #7

Placement requiring either surface name **or** tag to match the provided values, and with rotation restricted to a range (45-90 degrees).

![Example 7](./images/ambit-spawner-examples/Example_7.png)

### Example #8

Placement requiring either surface name **and** tag to match the provided values, and with two Unreal Engine actor types to spawn.

![Example 8](./images/ambit-spawner-examples/Example_8.png)

## Spawn in Volume Examples

### Example 1

Placement by bounding box only, without _Snap to Surface Below_ or specified surfaces.

![Example 1](./images/ambit-spawner-examples/bounding-1.png)

### Example 2

Placement by bounding box and _Snap to Surface Below_, without specified surfaces.

![Example 2](./images/ambit-spawner-examples/bounding-2.png)

### Example 3

Placement by bounding box and specified surfaces.

![Example 3](./images/ambit-spawner-examples/bounding-3.png)

## Spawn on Path Examples

### Example 1

Placement by spline only, without _Snap to Surface Below_ or specified surfaces.

![Example 1](./images/ambit-spawner-examples/spline-1.png)

### Example 2

Placement by spline and _Snap to Surface Below_, without specified surfaces.

![Example 2](./images/ambit-spawner-examples/spline-2.png)

### Example 3

Placement by spline and specified surfaces (name pattern _Road_).

![Example 3](./images/ambit-spawner-examples/spline-3.png)

### Example 4

Placement by spline, _Snap to Surface Below_, and _Follow Spline Rotation_. The spline was edited to include a curve.

![Example 4](./images/ambit-spawner-examples/spline-4.png)