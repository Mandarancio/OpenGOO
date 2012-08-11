##OpenGOO##
A free and open clone of [World of Goo](http://www.worldofgoo.com/)
__This software is released under the GPLv3 license__


* [Home page](http://mandarancio.github.com/OpenGOO/)
* [Levels](http://opengoolevels.comeze.com/)
* [Wiki](https://github.com/Mandarancio/OpenGOO/wiki)
* [Issues](https://github.com/Mandarancio/OpenGOO/issues)

# Credits

* Adam 'Etam' Mizerski        (Coder)
* Dmitry Marakasov            (Coder)
* Ferrari Martino             (Founder and coder)
* Groppi Filippo              (Musician)
* Oleksiy Slyshyk             (Coder)
* Signoretti Fabrizio         (Coder)
* Torsello Marco              (Coder)

__Let us write your name!__


# News


__9/August/2012__
Now I'm working at [CERN](www.cern.ch) so I don't have all the time I'd like to have to work on this project.
I hope that at the end of the summer I could be operative again.

__Martino__



# DEPENDENCIES

* Box2d 2.2.1
* OpenAL
* Freealut
* Qt4 and QT4 opengl module
* Gcc
* QMake
* Make

# TO COMPILE

Go to the folder containing the source code [example OpenGOO-src]:

cd ~/OpenGOO/OpenGOO-src
qmake
make

# TO RUN

##Make sure the required files are in the same folder of the executable:##

* menu.index
* the .svg level files (not in a subfolder)
* the 'resources' folder.

Then go to the folder containing the executable:

(if you are on OpenGOO/OpenGOO-src folder)

cd ..
./OpenGOO

If you want to more debug output run the game with:

./OpenGOO -Debug
