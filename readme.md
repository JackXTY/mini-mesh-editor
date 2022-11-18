# mini-mesh-editor

This is project with various functions about editing mesh, such as editing vertexs position of mesh in different ways, binding skeleton and deforming mesh according to skeleton transformation.

It comes from my coourse project for my UPenn master course.

## Some special features:

### Skin weight editor:
After json and obj are loaded, and skinning is done, when the user select a vertex, the combo box will show 2 related joints of this vertex.
Then the user could select which joint to be replaced in combo box, select new joint in the joint QTreeWidgetList, and input the weight.
The last step is to click the "Replace Joint" button, after clicking that, the joint would be replaced.
### Extruding Faces:
A GUI button "Extrude" is added. After user selects a face, if the user pushes this button, the selcted face will be extruded.
### Sharp edges and vertices:
A GUI button "Sharp" is added. Can set selected vertex/edge/face to be sharp.
### Selection via ray casting
User could select vertex/edge/faces via directly clicking the GL Window.


(According to my personal approximation, it might reach 0.1% ability of Maya. What's a pity is that, I didn't and maybe will never implement the function to export an obj file.)
