---
title: Basic Rendering Theory
depends: 
---

Basic Rendering Pipeline
---------------------
OpenGL is used to control a rendering pipeline, which is basically a series of standard steps that you make to turn some data into a rendered frame.  Below is a basic overview of a simple pipeline.  OpenGL allows you to do some more complicated things, but the stages below are enough to get some interesting things done for [most programs](# "Those of you familiar with things like tessellation should't worry. We'll get to that bit later").

```flow
[Vertex Data]->[<state> Vertex Shader]
[Vertex Shader]->[Rasterization]
[Rasterization]->[<state> Fragment Shader]
[Fragment Shader]->[Framebuffer]
```

#### Vertex Data
The vertex data is supplied by your program to OpenGL. It generally represents the positions of each vertex for a group of triangles.

#### Vertex Shader
The vertex shader is the first programmable part of the OpenGL pipeline. It lets you do things like move each vertex around and pass any data needed to later stages.  You generally use this stage to put your triangles in front of a camera.

#### Rasterization
This is a fixed-function step that OpenGL performs for you. It finds all of the [fragments](# "Pixels") that need to be processed in order to fill in your triangles. That said, we have rolled up a few distinct steps into one for the sake of simplicity.  There are also distinct steps here for turning your vertices into triangles (Primitive Assembly), then throwing away any triangles or pieces of triangles that are outside of the area that you are rendering, such as pieces of triangles that are off-screen (Clipping).

#### Fragment Shader
The fragment shader is the last programmable step in the pipeline.  This is where you get to do things like set the color of the fragment (EX: lighting, texturing).

#### Framebuffer
The framebuffer is where the final result is stored.  This framebuffer can then be put on the screen, or used as an input for further operations.  Applications generally employ a scheme called Double-Buffering, where one framebuffer is supplied to the screen for display while the application works on rendering the next frame to a different framebuffer, which is swapped with the first when it is complete.

OpenGL Particulars
------------------
Up until now, nothing we have covered has been OpenGL-specific.  We're now going to through this on a high level to give you some more context about what is happening over the course of the next few tutorials.

#### Shaders
Shaders are programs for OpenGL written to perform a specific task on the GPU.  Shaders in OpenGL are written in a language called GLSL, which should look vaguely familiar to anybody who has written a program in C.  We are responsible for passing our shader code to the graphics driver to be compiled, as well as selecting the pairs of Vertex/Fragment shader to link together and make into a shader "program".  There are also optional shader types that can be added to this program, but we will cover them at a later time.

Shaders are passed two general categories of parameter.  First, there are "Uniforms" which are essentially "extern const" variables for GLSL.  These are the same across all shaders in the program for the entire invocation.  There are then parameters passed in to one stage from the previous stage.  They can be passed from vertex to fragment shader, or from buffers on the GPU into the vertex shader.

#### Vertex Data
OpenGL stores vertex data in several different kinds of buffer, each of which is stored on the GPU.  Your most basic buffers (we'll call them vertex buffers), store a set of data that represents triangles.  For example, we could store the following data in a vertex buffer.

$$$

    \left(
        \begin{matrix}
            X & Y & Z \\
            -1 & -1 & 0 \\
			1 & -1 & 0 \\
			0 & 1 & 0
        \end{matrix}
    \right)

$$$

This data (x, y, z labels excluded), represents a triangle.  We can have any number of these vertex buffers defining different properties to be passed to the vertex shader (EX: position, color, texture coordinates).  For this, we have vertex arrays.  Vertex arrays give us a central place to define which buffer corresponds to which shader input, and how that data is logically arranged within that buffer.  This gives us the bare minimum needed to render with OpenGL.  Element array buffers are technically optional, but we will be using them throughout the series.  They allow us to define a list of vertex indices in the other buffers that correspond to triangles.  In the case above, our element array buffer would be (0, 1, 2), which is fairly pointless.  Once our triangle becomes part of a larger surface, the benefits become apparent. Adding a second adjacent triangle would get us the following without element array buffers.

$$$

    \left(
        \begin{matrix}
            X & Y & Z \\
            -1 & -1 & 0 \\
			1 & -1 & 0 \\
			0 & 1 & 0 \\
			1 & -1 & 0 \\
			0 & 1 & 0 \\
			1 & 1 & 0
        \end{matrix}
    \right)

$$$

With element array buffers, we can reduce this to...

$$$

    \left(
        \begin{matrix}
            X & Y & Z \\
            -1 & -1 & 0 \\
			1 & -1 & 0 \\
			0 & 1 & 0 \\
			1 & 1 & 0
        \end{matrix}
    \right)

$$$

The corresponding element array buffer would be (0, 1, 2, 1, 2 3).  This can save us memory, but more importantly processing time.  The duplicate data means that OpenGL will run our vertex shader six times without the element array buffer, but only four times with it.  Further optional buffers allow us to move the information about how to draw each object to the GPU as well as sets of objects to draw.  Once we have a vertex array, each further buffer type introduces a different draw command, the most important of which will be covered in our Hello Triangle tutorial.
