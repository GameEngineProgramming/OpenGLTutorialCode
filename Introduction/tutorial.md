---
title: Introduction and Build Environment
depends:
---

Welcome to our [AZDO](http://gdcvault.com/play/1020791/ "Approaching Zero Driver Overhead") tutorial series.  Here, we will be covering the most recent and efficient subset of OpenGL 4.5.  This is specifically targeted at the latest subset because it is not well covered in other tutorial series, and it provides some new features that make it an easier first subset to learn than the old-style OpenGL.  During this series, we will be rather prescriptive about how you should do things.  We think that this is important to keeping people's code efficient and reducing confusion.  If the only reason to use a feature is compatibility, we won't mention it.  If there are genuine trade-offs, we'll let you know about alternatives, then try to teach what is most commonly needed.  If we have a complicated method in mind as an optimal solution, we will give a minimal set of stepping-stones if needed for comprehension.  All of the code is in on Github along with the markdown for these tutorials.  If you don't like something, let us know (preferably by pull-request).

Libraries
---------
We will be using a number of libraries to assist us.  All of them are permissively licensed, and you should be able to use them in your commercial projects.

#### [OpenGL](https://www.opengl.org/wiki/Getting_Started "OpenGL Getting Started Guide")
The most important library we will be using is OpenGL.  Your OpenGL implementation is controlled by your graphics driver. That said, go install the latest graphics driver for your system before proceeding. Seriously, some of the things covered are extremely new and may not be supported in your drivers unless you updated them already this month.

#### [SDL2](http://libsdl.org/ "Simple DirectMedia Layer")
We will be using SDL2 for handling the creation of our window and OpenGL [rendering context](# "The place in the window where OpenGL draws all of the pretty pictures. Yes, 'Pretty Pictures' is a technical term.").  SDL2 will also handle our [input events](# "key presses, mouse movement, etc") for us.  Basically, SDL2 is our way of abstracting away [OS](# "Operating system")-specific things so that we can get on to the meat of the tutorial.

#### [GLEW](http://glew.sourceforge.net/ "The OpenGL Extension Wrangler Library")
OpenGL organizes itself into a series of extensions, which may or may not be supported by any particular driver depending on which version of OpenGL they support and which extra extensions that vendor chooses to support beyond that.  GLEW binds the functions that you call in your program to the correct implementation of that function in the driver on the system running your program.  In essence, GLEW makes sure that when you call a function supported by your driver, it actually get called.  Most people programming in OpenGL use GLEW.

#### [GLM](http://glm.g-truc.net/ "OpenGL Mathematics")
GLM provides us with efficient implementations for most of the math that we will be using in this series. We chose this one largely because the naming conventions match what we'll be using on the [GPU](# "Graphics Processing Unit. IE: graphics card. I admit it, I'm just having fun with hover-text.").

Setting Up your Build Environment
---------------------------------
Tim: you should write this part using CMake
