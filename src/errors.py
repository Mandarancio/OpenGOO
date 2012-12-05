# To change this template, choose Tools | Templates
# and open the template in the editor.
ISSUE_LEVEL_NONE = 0
ISSUE_LEVEL_ADVICE = 1
ISSUE_LEVEL_WARNING = 2
ISSUE_LEVEL_CRITICAL =4
ERROR_URL = 'http://goofans.com/developers/world-of-goo-level-editor/reference-guide/errors-and-warnings#%d'
ERROR_MORE_INFO = '&nbsp; <a href="'+ERROR_URL+'"><small><i>[more info]</i></small></a>'
ERROR_FRONT=[''
,'Advice: '
,'<b>Warning:</b> '
,''
,'<font color="#BF0000"><b>CRITICAL:</b></font> ']

ERROR_INFO={0:[ISSUE_LEVEL_NONE,''],
#Scene Tree Errors
    1:[ISSUE_LEVEL_CRITICAL,'<tt>%s</tt> is not static and has no mass'],
    2:[ISSUE_LEVEL_ADVICE,' <tt>%s</tt> not static and rotated. It won\'t be where you think!'],
    3:[ISSUE_LEVEL_CRITICAL,'<tt>%s</tt> is not static and child <tt>%s</tt> has no mass'],
    4:[ISSUE_LEVEL_ADVICE,'<tt>%s</tt> : Images on compgeom children are ignored in the game.'], #redundant?
    5:[ISSUE_LEVEL_CRITICAL,'<tt>%s</tt> is non-static and has no children'],
    6:[ISSUE_LEVEL_WARNING,'<tt>%s</tt> is static and has no children.'],
    7:[ISSUE_LEVEL_CRITICAL,'<tt>%s</tt> is static and has a motor'],
    8:[ISSUE_LEVEL_CRITICAL,'<tt>%s</tt> is at the centre of rff %s'],
    9:[ISSUE_LEVEL_ADVICE,'<tt>%s</tt> is spinning but has no hinge!'],
    10:[ISSUE_LEVEL_WARNING,'<tt>%s</tt> is a compgeom child so cannot have a hinge!'],
    11:[ISSUE_LEVEL_CRITICAL,'linearforcefield <tt>%s</tt> has size but no center'],

#Level Tree Errors
    101:[ISSUE_LEVEL_ADVICE,'<tt>%s</tt> poi traveltime will cause a delay starting the level'],
    102:[ISSUE_LEVEL_WARNING,'Level has no <tt>normal</tt> camera'],
    103:[ISSUE_LEVEL_WARNING,'Level has no <tt>widescreen</tt> camera'],
    104:[ISSUE_LEVEL_CRITICAL,'has <tt>levelexit</tt> but no <tt>pipe</tt>'],
    105:[ISSUE_LEVEL_ADVICE,'The pipe is not at levelexit!'],
    106:[ISSUE_LEVEL_ADVICE,'Pipe vertexes are too close together'],
    107:[ISSUE_LEVEL_CRITICAL,'There is a pipe vertex that does not turn'],
    108:[ISSUE_LEVEL_CRITICAL,'There is a pipe vertex that turns 180deg'],
    109:[ISSUE_LEVEL_CRITICAL,'You can\'t connect <tt>%s (%s)</tt> to <tt>%s (%s)</tt>'],
    110:[ISSUE_LEVEL_CRITICAL,'<tt>%s (%s)</tt> cannot have any strands!'],
    111:[ISSUE_LEVEL_ADVICE,'Multiple end conditions <tt>%s</tt>'],
    112:[ISSUE_LEVEL_WARNING,'Gooballs start above the <tt>targetheight</tt>'],
    113:[ISSUE_LEVEL_CRITICAL,'Signpost %s uses an Ambient Particle effect <tt>%s</tt>'],
    114:[ISSUE_LEVEL_CRITICAL,'Fire object uses an Ambient Particle effect <tt>%s</tt>'],

#Resource Errors
    201:[ISSUE_LEVEL_WARNING,'Image file extensions must be png (lowercase) %s'],
    202:[ISSUE_LEVEL_ADVICE,'%s unused'],
    203:[ISSUE_LEVEL_WARNING,'Sound file extensions must be ogg (lowercase) %s'],
    204:[ISSUE_LEVEL_ADVICE,'%s unused'],
    205:[ISSUE_LEVEL_ADVICE,'Text resource %s unused'],

#Dependancy Errors and Notifications
    301:[ISSUE_LEVEL_CRITICAL,'Ball Recursion <tt>%s</tt>'],
    302:[ISSUE_LEVEL_ADVICE,'Custom Balls used <tt>%s</tt>'],
    303:[ISSUE_LEVEL_CRITICAL,'Ball <tt>%s</tt> is missing'],
    304:[ISSUE_LEVEL_ADVICE,'Custom Materials used <tt>%s</tt>'],
    305:[ISSUE_LEVEL_CRITICAL,'Material <tt>%s</tt> is missing'],
    306:[ISSUE_LEVEL_ADVICE,'Custom Particles used <tt>%s</tt>'],
    307:[ISSUE_LEVEL_CRITICAL,'Particle Effect <tt>%s</tt> is missing'],
    308:[ISSUE_LEVEL_ADVICE,'Custom Animtaions used <tt>%s</tt>'],
    309:[ISSUE_LEVEL_CRITICAL,'Animation file <tt>%s.anim.binltl</tt> is missing'],

#Global Level Errors
    401:[ISSUE_LEVEL_WARNING,'the levelexit is outside scene bounds'],
    402:[ISSUE_LEVEL_WARNING,'Custom Pipe resource %s is missing'],

#Ball Tree Errors
    501:[ISSUE_LEVEL_WARNING,'Ball is <tt>detachable</tt> but has no <tt>detachstrand</tt>'],
    502:[ISSUE_LEVEL_CRITICAL,'Ball <tt>contains</tt> balls but has no <tt>popparticles</tt>'],
    503:[ISSUE_LEVEL_CRITICAL,'Ball <tt>contains</tt> balls but has no <tt>popsound</tt>'],
    504:[ISSUE_LEVEL_CRITICAL,'<tt>towermass</tt> is between -1 and 1.'],
    505:[ISSUE_LEVEL_CRITICAL,'<tt>stacking</tt> set true on <tt>circle</tt> ball'],
    506:[ISSUE_LEVEL_ADVICE,'Ball has a very low <tt>dragmass</tt>, causes glitches near an open pipe'],
    507:[ISSUE_LEVEL_WARNING,'<tt>%s</tt> has only 2 arguments and the first is not 0'],
    508:[ISSUE_LEVEL_CRITICAL,'Part <tt>%s</tt> is an <tt>eye</tt> but has no <tt>pupil</tt> image' ],
    509:[ISSUE_LEVEL_ADVICE,'No part called "body", this ball will not display in WooGLE correctly'],
    510:[ISSUE_LEVEL_ADVICE,'Strand <tt>maxlen2</tt> is 0, this ball will not form any strands.'],
    511:[ISSUE_LEVEL_WARNING,'Strand <tt>minlen</tt> is greater than <tt>maxlen</tt>'],
    512:[ISSUE_LEVEL_ADVICE,'Strand has no <tt>fireparticles</tt> it will not burn.'],
    513:[ISSUE_LEVEL_ADVICE,'Strand has a large <tt>ignitedelay</tt> so fire may not spread'],
    514:[ISSUE_LEVEL_ADVICE,'<tt>throw</tt> sound not last in list. %s sounds will play instead'],
    515:[ISSUE_LEVEL_ADVICE,'<tt>throw</tt> sound not found. %s sounds will play instead']
}
