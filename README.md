

Path Tracer
======================

Xi Yang


Path Tracing with Participating Media
--------

See volumeintegrator.h/.cpp. 

Add medium in Camera, ray and medium interface in Intersection. 
Add medium intersection features in intersection class. 
Add homogeneous medium implementation.

Attention: If there is build error in toggled check box, please just comment that error line.
(Actually I don't know why there is a build error after I add the check box for lens camera.)

Renderings 
--------

glassballbox(default setting)

![](./glassball_box.png)


spotlight(sample 200, recursion limit 5)

![](./spotlight.png)

wolf(sample 200, recursion limit 5)

![](./wolf.png)


======================

Lens-based camera (REQUIRED) (10 points)
--------

Please use the check box on GUI to test Lens-based Camera!

(PT_CameraTestScene.json)


Without lens-based camera:

![](./CameraTest1.png)

With lens-based camera:

![](./CameraTest2.png)


======================
----------------

* Complex implicit surfaces

+A new class Surface inherited from Shape. 

+Change of Jason file, you can specify the type of implicit surface.
 
+Now supported type: sphere(default), Ellipsoid, Torus, Heart, ToothSurface.

Renders(full lighting integrator,default settings):

torus:

![](./torus.png)

ellipsoid:

![](./Ellipsoid.png)

tooth surface:

![](./ToothSurface.png)

heart:

![](./Heart.png)



-----------
Point Light:

rendered with direct lighting integrator(default settings), the light is at the up-right corner of the box.

![](./direct_point_intensity5_posmiddle3.png)

Spot Light:

rendered with direct lighting integrator(default settings), the light is at the top of the box.

![](./direct_spot_falloff0.1_width50.png)

Texture Projection Light:

rendered with direct lighting integrator(default settings), the light is very far top of the box.

PT_projectionLight.json

![](./projectionLight.png)

PT_cornellBox_projectionLight.json

![](./projectlightBox.png)

BVH Tree:
------------
wahoo(100 samples, 5 recursion limit)

Milliseconds to build BVH tree: 52

![](./rendered_images_full2.png)



screw.jason

(400 samples, 8 recursion limit)

Milliseconds to build BVH tree: 111

![](./bvh111_screw_20-8.png)


stairs.jason

(400 samples, 8 recursion limit)

Milliseconds to build BVH tree: 127

![](./bvh127_20-8.png)


