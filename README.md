## OpenGOO

A free and open clone of [World of Goo](http://www.worldofgoo.com/)

* [Home page](http://mandarancio.github.com/OpenGOO/)
* [Wiki](https://github.com/Mandarancio/OpenGOO/wiki)
* [Levels](http://opengoolevels.comeze.com/)
* [Issues](https://github.com/Mandarancio/OpenGOO/issues)
* [Contribute](https://github.com/Mandarancio/OpenGOO/wiki/How-to-collaborate)

__This software is released under the [GPLv3](http://www.gnu.org/licenses/gpl-3.0.html) license__


## Credits

* Adam 'Etam' Mizerski          (Coder)
* Ferrari Martino               (Founder and coder)
* Groppi Filippo *              (Musician)
* Batarov Dmitry *              (Coder)
* Marakasov Dmitry              (Coder)
* Signoretti Fabrizio *         (Coder)
* Slyshyk Oleksiy *             (Coder)
* Torsello Marco                (Coder)

(*) Active.


[**Let us write your name!**](https://github.com/Mandarancio/OpenGOO/wiki/How-to-collaborate) We need sketchers for new artworks!

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

* Qt4 and QT4 opengl module
* Gcc
* QMake
* Make

## COMPILE AND RUN

In the folder containing the source code, where this README is located, type:

    qmake
    make

then execute the game with:

    ./OpenGOO

There are some flags that you can activate:
  
    -debug  (activates the debug output)
    -fps    (shows the framerate per second)
    -opengl (opengl mode)

    example:
    ./Opengoo -opengl -debug

## CUSTOM LEVELS

Put your own levels in the 'Levels' folder and execute the game.