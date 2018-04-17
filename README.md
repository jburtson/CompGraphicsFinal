# CompGraphicsFinal
A small game made in xcode using openGL and written in C++ as a final project for a Computer Graphics course. Player must dodge boulders and a zombie for as long as possible.

In order to get running properly in your version of xcode you may need a few extra steps:
In Build Phases, under Link Binary With Libraries, add GLUT.framwork and OpenGL.framework
Then under project, in the preprocessing section, remove "DEBUG=1"
Then in product-> Scheme -> edit Scheme -> Run, unlcick the Debug executable option, and in the Options tab, set the working directory, use custom directory tm_src

Demo of project:
https://www.youtube.com/watch?v=do7bu9C6oys
