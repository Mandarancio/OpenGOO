## OpenGOO

A free and open clone of [World of Goo](http://www.worldofgoo.com/)

* [Home page](http://mandarancio.github.com/OpenGOO/)
* [Wiki](https://github.com/Mandarancio/OpenGOO/wiki)
* [Levels](http://opengoolevels.comeze.com/)
* [Issues](https://github.com/Mandarancio/OpenGOO/issues)
* [Contribute](https://github.com/Mandarancio/OpenGOO/wiki/How-to-collaborate)

__This software is released under the [GPLv3](http://www.gnu.org/licenses/gpl-3.0.html) license__


## Credits

* Adam 'Etam' Mizerski        (Coder)
* Ferrari Martino             (Founder and coder)
* Groppi Filippo              (Musician)
* Marakasov Dmitry            (Coder)
* Signoretti Fabrizio         (Coder)
* Slyshyk Oleksiy             (Coder)
* Torsello Marco              (Coder)

**Let us write your name!**


## News

__11/August/2012__

We are competing in the [LPC](lpc.opengameart.org), check our [entry](http://opengameart.org/lpc-code-entries)!
The results of the competition will be released at the end of August!

_Martino_

* * *

__09/August/2012__

Now I'm working at [CERN](www.cern.ch) so I don't have all the time I'd like to have to work on this project.
I hope that at the end of the summer I could be operative again.

_Martino_



## DEPENDENCIES

* Box2d 2.2.1
* OpenAL
* Freealut
* Qt4 and QT4 opengl module
* Gcc
* QMake
* Make

## TO COMPILE

Go to the folder containing the source code [example OpenGOO-src]:

    cd ~/OpenGOO/OpenGOO-src
    qmake
    make

## TO RUN

##Make sure the required files are in the same folder of the executable:##

* menu.index
* the .svg level files (not in a subfolder)
* the 'resources' folder.

Then go to the folder containing the executable:

(if you are on OpenGOO/OpenGOO-src folder)

    cd ..
    ./OpenGOO

There are some flag that you can activate:
  
    -debug  (for more debug output)
    -fps    (to know the fps)
    -opengl (use opengl for more performances)
    example:
    ./Opengoo -opengl -debug
    