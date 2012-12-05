"""Describes the structure and constraints of elements used in data file of WOG."""
from metaworld import *
from PyQt4 import QtCore, QtGui

# Declares all file types
TREE_GLOBAL_FX = describe_tree( 'game.fx' )
TREE_GLOBAL_MATERIALS = describe_tree( 'game.materials' )
TREE_GLOBAL_RESOURCE = describe_tree( 'game.resources' )
TREE_GLOBAL_TEXT = describe_tree( 'game.text' )
TREE_GLOBAL_FILES = describe_tree ('game.files')

TREE_ISLAND = describe_tree( 'island' )

TREE_BALL_MAIN = describe_tree( 'ball.main' )
TREE_BALL_RESOURCE = describe_tree( 'ball.resource' )

TREE_LEVEL_GAME = describe_tree( 'level.game' )
TREE_LEVEL_SCENE = describe_tree( 'level.scene' )
TREE_LEVEL_RESOURCE = describe_tree( 'level.resource' )
#@DaB
TREE_LEVEL_ADDIN = describe_tree ('level.addin')
TREE_LEVEL_TEXT = describe_tree ('level.text')

TREE_LEVEL_DEPENDANCY = describe_tree('level.dep')

# Declares the world hierarchy
WORLD_LEVEL = describe_world( 'level', trees_meta = [
    TREE_LEVEL_GAME,
    TREE_LEVEL_SCENE,
    TREE_LEVEL_RESOURCE,
    TREE_LEVEL_ADDIN,
    TREE_LEVEL_TEXT,
    TREE_LEVEL_DEPENDANCY
    ] )
WORLD_ISLAND = describe_world( 'island', trees_meta = [
    TREE_ISLAND
    ] )
WORLD_BALL = describe_world( 'ball', trees_meta = [
    TREE_BALL_MAIN,
    TREE_BALL_RESOURCE,
    ] )
WORLD_GLOBAL = describe_world( 'game',
                               child_worlds = [ WORLD_ISLAND, WORLD_LEVEL, WORLD_BALL ],
                               trees_meta = [
    TREE_GLOBAL_RESOURCE,
    TREE_GLOBAL_FX,
    TREE_GLOBAL_MATERIALS,
    TREE_GLOBAL_TEXT,
    TREE_GLOBAL_FILES
    ] )

LEVELS_ORIGINAL = set(['AB3','BeautyAndTheTentacle','BeautySchool','BlusteryDay',
                   'BulletinBoardSystem','BurningMan','Chain','Deliverance',
                   'Drool','EconomicDivide','FistyReachesOut','FlyAwayLittleOnes',
                   'FlyingMachine','GeneticSortingMachine','GoingUp','GracefulFailure',
                   'GrapeVineVirus','GraphicProcessingUnit','HangLow','HelloWorld',
                   'HTInnovationCommittee','ImmigrationNaturalizationUnit','ImpaleSticky',
                   'IncinerationDestination','InfestyTheWorm',
                   'island1','island2','island3','island4','island5','IslandUi',
                   'IvyTower','LeapHole','MapWorldView','MistysLongBonyRoad','MOM',
                   'ObservatoryObservationStation','OdeToBridgeBuilder','ProductLauncher',
                   'RedCarpet','RegurgitationPumpingStation','RoadBlocks','SecondHandSmoke',
                   'SuperFuseChallengeTime','TheServer','ThirdWheel','ThrusterTest','TowerOfGoo',
                   'Tumbler','UpperShaft','VolcanicPercolatorDaySpa','WaterLock','WeatherVane',
                   'Whistler','wogc','wogc3d','wogcd','YouHaveToExplodeTheHead'])

LEVELS_ORIGINAL_LOWER=[level_name.lower() for level_name in LEVELS_ORIGINAL]

 #DaB: I'd hoped to do this by checking the attributes of the ball type,
 # So that this could also correctly handle custom balls as well as "original" balls.
 # However, it is not possible to accurately determine "No Strands" from just the balls attributes.
 # Anchor's and Beauty's (and others) have exactly the same attributes but one can
 # have strands and one can't / shouldn't
 # So we're stuck with a hard-coded list of names that can't have strands
BALLS_NO_STRANDS = ['Beauty','BeautyProduct','BeautyProductEye','Bit',
                        'BlockHead','Hex','RectHead','Spam','TimeBug',
                        'Ugly','UglyProduct','UndeletePill','UndeletePillFizz',
                        'UtilGooGlobber','UtilProductLauncherScreamer',
                        'WindowRect','WindowSquare','ZBomb','ZBombMom']

#mustbe_gb1 is necessary since these Goos do not have a <strand> entry
# 2 mustbe_gb1's can't be connected together - causes WOG to crash.
#BALLS_MUST_BE_GB1 = ('Anchor','AnchorFriendly','AnchorSticky','AnchorStickyInvisible')
BALLS_MUST_BE_GB1 = []

#shouldbe_gb2 is subjective  (what looks best, what it looked like in the original)
BALLS_SHOULD_BE_GB2 = ['balloon','EvilEye','Fish','Pokey','BombMini','BombShaft','BombSticky','Bone']

BALLS_SHAPES={}
BALLS_IMAGES={}
BALLS_SUCKABLE=set()
BALLS_INVISIBLE=set()
BALLS_STATIC=set()

BALLS_ORIGINAL = ['Anchor','AnchorFriendly','AnchorSticky','AnchorStickyInvisible',
                 'balloon','Beauty','BeautyProduct','BeautyProductEye','Bit','BlockHead',
                'BombMini','BombShaft','BombSticky','Bone','common','common_albino','common_black','Distant',
                'Drained','DrainedIsh','EvilEye','Fish','Fuse','GooProduct','Hex','IconWindowRect','IconWindowSquare',
                'Ivy','Pilot','Pixel','PixelProduct','Pokey','RectHead','SimCommon','Spam','TimeBug',
                'Ugly','UglyProduct','UndeletePill','UndeletePillFizz',
                'UtilAttachUnwalkable','UtilAttachWalkable','UtilChapter2',
                'UtilGooGlobber','UtilGooGlobberMom','UtilGooGlobberMomQuiet',
                'UtilNoAttachUnwalkable','UtilNoAttachWalkableSmall','UtilProductLauncherScreamer',
                'water','WindowRect','WindowSquare','ZBomb','ZBombMom']

MATERIALS_ORIGINAL = ["ice","rock","grass","fruit",
                      "sticky","verysticky","kindasticky" ,"slightlysticky",
                      "slosh","swamp","throat","machine","strand","IvyTower",
                      "volcanicPercolatorSides","ServerPlatform",
                      "BeautyBall","UglyBall","BlockBall","BoneBall"]

PARTICLEEFFECTS_ORIGINAL =['unlockfuse','unlockburst','snowStormFromPoint','splash','gooFallWide2','gooSplatter',
                          'gooSplatterSubtle','mistUp','fireStack1','fireStackSmaller1','fireStackSmaller2',
                          'fireRobotHead','fireStackSquat','fireStackLanternBw','fireBallBurn','thruster',
                          'fireArmBurn','poisonBallBurn','poisonArmBurn','blackSmokeRising','bubblesRisingFromPoint',
                          'bubblesRisingFromPointSlow','wogcSmoke','wogcSmokeIsh','whiteSmokeRising','sleepyZzz',
                          'ish_sleepyZzz','ishr_sleepyZzz','signpostAlert','signpostAlertMom','whistle',
                          'worldMap_FactorySmoke','BurningManSmoke','RobotHeadSmoke','distantSmokestack',
                          'worldMap_FactorySmokeWhite','cigSmoke','gentleFactorySmoke','puffyFactorySmoke',
                          'gentleFactorySmokeSepia','matchSmoke','gooTankStream','gooDrips','polFountain1','polFountain2',
                          'gooMist','timebugFizz','flashes','tubeAirFlowUp','tubeAirFlowLeft','geomExplosionDust',
                          'ish_smallfire','ish_FlingTrail','ishr_FlingTrail','ish_gpu_bitspew','ish_bitPop','beautypop',
                          'flowerDust','OOS_gooDrips','OOS_gooGlobs','BallExplode_Fuse','BallExplode_Bomb','BallExplode_ISH',
                          'ISH_undeleteFizz','ish_bubbles']

AMBIENTEFFECTS_ORIGINAL=['bigleaves1','smallleaves1','rainingleaves','rainingleavesRight','leavesRight',
                         'snowSparse','snowDense','snowStorm','snowStormC3',
                         'blackBallsRising','blackBallsRight','blackLeaves','blackLeavesHeavy',
                         'rainStreaksHeavy','rainStreaksHeavyDistant','rainStreaksDown',
                         'mistRight','breezeRight','breezeUpSlow','breezeDownSlow','breezeUp','mistUpSepia',
                         'ish_BigLeaves','ish_SmallLeaves','ish_RainLeavesLeft','ish_RainLeavesUp','ish_RainLeavesUpRed',
                         'ish_BreezeRight','ish_HeavyBreezeLeft','ish_HeavyBreezeUp','OOS_breezeRight']

ANIMATIONS_ORIGINAL = ['ball_counter','ball_counter_ocd','blink','closer','discovery','hairblow',
                     'happyTreeDance','islandhairblow','island_name_in','island_name_loop',
                     'island_name_out','level_name_popup','ocdFlagWave','rot_1rps','treeBlow_leaf1',
                     'treeBlow_leaf2','treeBlow_leaf3','treeBlow_trunk']
ANIMATIONS_GLOBAL = []

PIPE_RESOURCE = 'IMAGE_GLOBAL_PIPE'
PIPE_RESOURCE_STUBS=['','_BEND_TL','_BEND_TR','_BEND_BL','_BEND_BR','_CAP_OPEN','_CAP_CLOSED']
PIPES_STANDARD = {'default':QtGui.QColor( 64, 64, 64 ),
                  '':QtGui.QColor( 64, 64, 64 ),
                  'BLACK' :QtGui.QColor( 0, 0, 0 ),
                  'BEAUTY':QtGui.QColor( 255, 128, 128 ),
                  'ISH':QtGui.QColor( 32, 255, 32 ),
                  'custom':QtGui.QColor( 255, 0,0 )}
#@DaB
FILE_ELEMENT= describe_element('file', attributes = [
                        string_attribute('name',mandatory = True),
                        string_attribute('type',mandatory = True) ])
FOLDER_ELEMENT = describe_element('folder', attributes = [
        string_attribute('name',mandatory = True)],elements=[
        FILE_ELEMENT])
FOLDER_ELEMENT.add_elements([FOLDER_ELEMENT])
TREE_GLOBAL_FILES.add_elements ([
    describe_element('folder', exact_occurrence = 1,
      attributes = [ string_attribute('name',mandatory = True)],
      elements=[FILE_ELEMENT,FOLDER_ELEMENT])])

LANGUAGE_ATTRIBUTES=[]
for lang in ['de','es','fr','it','nl','pl']:
    LANGUAGE_ATTRIBUTES.append(string_attribute(lang,allow_empty=True,remove_empty=True))

ADDIN_LEVEL_NAME_ATTRIBUTES=[ string_attribute ('text',init="Level Name", mandatory=True, display_id=True,tooltip='Level Name to display in the Game')]
ADDIN_LEVEL_NAME_ATTRIBUTES.extend(LANGUAGE_ATTRIBUTES)
ADDIN_LEVEL_NAME_ELEMENT= describe_element ('name', groups = 'info', exact_occurrence=1, attributes=ADDIN_LEVEL_NAME_ATTRIBUTES)

ADDIN_LEVEL_SUBTITLE_ATTRIBUTES=[ string_attribute ('text',init="subtitle", mandatory=True,display_id=True,tooltip='Subtitle to show on Level Start screen\nEasy as Goo pie!')]
ADDIN_LEVEL_SUBTITLE_ATTRIBUTES.extend(LANGUAGE_ATTRIBUTES)
ADDIN_LEVEL_SUBTITLE_ELEMENT = describe_element ('subtitle', groups = 'info', exact_occurrence=1, attributes=ADDIN_LEVEL_SUBTITLE_ATTRIBUTES)

TREE_LEVEL_ADDIN.add_elements ( [
    describe_element('addin', groups = 'info', exact_occurrence = 1,attributes = [
        real_attribute('spec-version',init="1.1",mandatory = True, read_only=True,tooltip='Version of the Addin file\nAlways 1.1 (read only)')],
        elements = [
        describe_element( 'id', groups = 'info', exact_occurrence = 1, read_only=True, attributes = [
        addinid_attribute('value',init="unique.addin.id",display_id=True, mandatory = True, tooltip='Unique Addin Id\ncom.goofans.{yourname}.levels.{levelname}')]),
        describe_element( 'name',groups = 'info', read_only=True, exact_occurrence = 1, attributes = [
        string_attribute('value',init="Addin Name", min_length=2, display_id=True ,mandatory = True, tooltip='Level/Addin Name shown in GooTool')]),
        describe_element( 'type',groups = 'info', read_only=True, exact_occurrence = 1,attributes = [
        string_attribute('value',init="level",mandatory = True,display_id=True, read_only=True, tooltip='Always "level" (read-only)')]),
        describe_element( 'thumbnail',groups = 'image', attributes = [
           string_attribute('value',display_id=True,init="thumbnail filename",mandatory = True),
           enum_attribute( 'type', values = ('image/jpeg','image/png'), mandatory=True ),
           int_attribute ('width', mandatory=True,init='200'),
           int_attribute ('height',mandatory=True,init='150') ]),
        describe_element( 'version', groups = 'info',read_only=True, exact_occurrence = 1,attributes = [
           string_attribute('value',init="0.1",mandatory = True,display_id=True,tooltip='Version Number of your level\n Valid Formats: 1  |  0.1  |  1.0.2  |  1.5.0.1')]),
        describe_element( 'description', read_only=True, groups = 'info',exact_occurrence = 1,attributes = [
            text_attribute('value',init="Addin Description", display_id=True, mandatory = True,tooltip='Description of the level shown in GooTool')]),
        describe_element( 'author', groups = 'info',read_only=True, exact_occurrence = 1,attributes = [
            string_attribute('value',init="Your Name Here",display_id=True,mandatory = True, tooltip='Your name here')]),
        describe_element( 'dependencies', max_occurrence=1, groups='info', elements = [
            describe_element( 'depends', groups='info', min_occurrence = 1, attributes =[
                string_attribute ('ref',init="unique.addin.id",mandatory=True),
                real_attribute ('min-version'),
                real_attribute ('max-version') ])
        ]),        
        describe_element( 'levels', read_only=True, exact_occurrence=1, groups = 'info',elements = [
            describe_element( 'level', groups = 'info',min_occurrence = 1, elements =[
                describe_element ('dir', groups = 'info', exact_occurrence=1,attributes = [
                    string_attribute('value',init="LevelFolder",mandatory = True,display_id=True,tooltip='Level Folder Name')]),
                ADDIN_LEVEL_NAME_ELEMENT,
                ADDIN_LEVEL_SUBTITLE_ELEMENT,
                describe_element ('ocd',max_occurrence=1,groups = 'info',attributes = [
                    ocd_attribute('value',default="",allow_empty = True,display_id=True,tooltip='OCD requirement. Valid Format\nballs,10\nmoves,23\ntime,30\nLeave blank for no OCD')]),
                describe_element ('skipeolsequence',groups = 'info',max_occurrence=1),
                describe_element ('cutscene',groups = 'info',max_occurrence=1, attributes = [
                    string_attribute('value',init="x,x,x",mandatory = True)])
            ])
        ])
        ]
        )
])

TREE_LEVEL_GAME.add_elements( [
    describe_element( 'level', exact_occurrence = 1, groups = 'game', attributes = [
        int_attribute( 'ballsrequired', default = 1, min_value = 1, tooltip="Number of Goos needed to finished the level" ,allow_empty = True,remove_empty=True),
        bool_attribute( 'letterboxed', init = False, allow_empty = True,remove_empty=True ),
        bool_attribute( 'visualdebug', init = False, allow_empty = True,remove_empty=True),
        bool_attribute( 'autobounds', init = False, tooltip='If true, the camera is restricted to the explored area\nIf false, the camera can move anywhere',allow_empty = True,remove_empty=True),
        rgb_attribute( 'textcolor', init = (255,255,255), allow_empty = True,remove_empty=True ),
        real_attribute( 'timebugprobability', init = 0, min_value = 0,max_value = 1,allow_empty=True,remove_empty=True ,tooltip='Set 0 for No Timebugs\n0.1 for some\n0.5 for LOTS!'),
        bool_attribute( 'strandgeom', default = False,allow_empty = True,remove_empty=True ),
        bool_attribute( 'allowskip', default = True, allow_empty = True,remove_empty=True ),
        bool_attribute( 'texteffects', default = False ,allow_empty = True,remove_empty=True),
        rgb_attribute( 'cursor1color', default = (255,255,255) ),
        rgb_attribute( 'cursor2color', default = (255,255,255) ),
        rgb_attribute( 'cursor3color', default = (255,255,255) ),
        rgb_attribute( 'cursor4color', default = (255,255,255) ),
        ],
        elements = [
        describe_element( 'camera', exact_occurrence = 2, groups = 'camera', attributes = [
            enum_attribute( 'aspect', values = ('widescreen', 'normal'), default = 'normal', display_id=True ),
            xy_attribute( 'endpos' ),
            real_attribute( 'endzoom', min_value = 0.00001 )
            ], elements = [
            describe_element( 'poi', min_occurrence = 1, groups = 'camera', attributes = [
                xy_attribute( 'pos', init = '0,0', mandatory = True ),
                real_attribute( 'zoom', min_value = 0.00001, default = 1 ,init = 1 , mandatory = True),
                real_attribute( 'pause', min_value = 0, default = 0 ,init = 0 , mandatory = True, tooltip='Wait here for n seconds\n'),
                real_attribute( 'traveltime', min_value = 0, default = 0, init = 3 , mandatory = True, tooltip='Time taken to travel TO this position\nIgnored on first POI')
                ] )
            ] ),
        describe_element( 'signpost', groups = 'sign', attributes = [
            string_attribute( 'name', init = '', mandatory = 'True', display_id=True ),
            xy_attribute( 'center', init = '0,0', mandatory = True, map_to = ('x','y'),position=True ),
            scale_attribute( 'scale', init = '1,1', min_value = 0.0000001, mandatory = True,
                             map_to = ('scalex', 'scaley') ),
            angle_degrees_attribute( 'rotation', init = 0, mandatory = True ),
            real_attribute( 'depth', init = 0, mandatory = True ),
            # @todo makes x,y a composite attribute
            reference_attribute( 'text', reference_family = 'TEXT_LEVELNAME_STR', reference_world = WORLD_LEVEL, init = '', mandatory = True ),
            reference_attribute( 'image', reference_family = 'image', reference_world = WORLD_LEVEL,
                                 init = '', mandatory = True ),
            real_attribute( 'alpha', min_value = 0, max_value = 1, init = 1, mandatory = True ),
            rgb_attribute( 'colorize', init = '255,255,255', mandatory = True ),
            reference_attribute( 'particles', reference_family = 'effect', reference_world = WORLD_GLOBAL ),
            bool_attribute( 'pulse',allow_empty=True,remove_empty=True )
            ] ),
        describe_element( 'pipe', groups = 'pipe', max_occurrence=1, attributes = [
            #identifier_attribute( 'id', mandatory = True, reference_family = 'pipe',
            #    display_id = True, reference_world = WORLD_LEVEL, init ='exitPipe' ),
            enum_attribute( 'type', values = ('BEAUTY', 'BLACK', 'ISH') , allow_any=True, allow_empty=True, remove_empty=True, tooltip='Sets Pipe Colour\nDoes NOT affect which Goos can get in' )
            ],
            elements = [
            describe_element( 'Vertex', min_occurrence = 2, attributes = [
                xy_attribute( 'pos', init = '0,0', mandatory = True, map_to = ('x','y') )
                ] ),
            ] ),
        describe_element( 'BallInstance', groups = 'ball', attributes = [
            identifier_attribute( 'id', mandatory = True, reference_family = 'goo',
                display_id = True, reference_world = WORLD_LEVEL ),
            reference_attribute( 'type', mandatory = True,
                                 reference_family = 'ball', reference_world = WORLD_GLOBAL ),
            # @todo makes x,y a composite attribute
            xy_attribute( 'pos', init = '0,0', mandatory = True, map_to = ('x','y'),position=True ),
            angle_degrees_attribute( 'angle', init = 0, mandatory = True ),
            #real_attribute( 'depth',allow_empty=True,remove_empty=True ),
            bool_attribute( 'discovered',allow_empty=True,remove_empty=True,tooltip='If false, this Goo will start asleep' )
            ] ),
        describe_element( 'Strand', groups = 'strand', attributes = [
            reference_attribute( 'gb1', reference_family = 'goo', reference_world = WORLD_LEVEL,
                                 init = '', mandatory = True ),
            reference_attribute( 'gb2', reference_family = 'goo', reference_world = WORLD_LEVEL,
                                 init = '', mandatory = True )
            ] ),
        describe_element( 'music', groups = 'resource', max_occurrence=1,attributes = [
            reference_attribute( 'sound', map_to='id',display_id = True, mandatory = True,
                reference_family = 'sound', reference_world = WORLD_LEVEL )
            ] ),
        describe_element( 'loopsound', groups = 'resource', max_occurrence=1,attributes = [
            reference_attribute( 'sound', map_to='id',display_id = True, mandatory = True,
                reference_family = 'sound', reference_world = WORLD_LEVEL )
            ] ),
        describe_element( 'levelexit', groups = 'game', attributes = [
            string_attribute( 'id', display_id = True, allow_empty=True, remove_empty=True ),
            xy_attribute( 'pos', mandatory = True, init = '0,0' ,position=True),
            radius_attribute( 'radius', mandatory = True, init = '75' ),
            reference_attribute( 'filter', init = '', allow_empty = True, remove_empty=True,
                                 is_list = True, 
                                 reference_family = 'ball', reference_world = WORLD_GLOBAL ,tooltip='Controls which Goos can get in the pipe.\nLeave Blank for Any\nSet multiple types -> common,Ivy,Bit')
            ] ),
        describe_element( 'endoncollision', groups = 'game',attributes = [
            reference_attribute( 'id1', reference_family = 'geometry', reference_world = WORLD_LEVEL, mandatory = True ),
            reference_attribute( 'id2', reference_family = 'geometry', reference_world = WORLD_LEVEL, mandatory = True ),
            real_attribute( 'delay', mandatory = True, init = '1' )
            ] ),
        describe_element( 'endonmessage', groups = 'game',attributes = [
            string_attribute( 'id', display_id = True, mandatory = True )  # values seems to be hard-coded
            ] ),

        #DaB End on no Geom functions fine...
        describe_element( 'endonnogeom', groups = 'game',attributes = [] ),

        describe_element( 'fire', groups = 'fire', attributes = [
            xy_attribute( 'center', mandatory = True, init = '0,0', map_to = ('x','y') ,position=True),
            radius_attribute( 'radius', mandatory = True, init = '50' ),
            real_attribute( 'depth', mandatory = True, init = '0' ),
            reference_attribute( 'particles', reference_family = 'effect',
                                 reference_world = WORLD_GLOBAL, mandatory=True)
            ] ),
        describe_element( 'targetheight', groups = 'game', max_occurrence=1,attributes = [
            real_attribute( 'y', mandatory = True, init = '300' ),
            ] )
        ] )
    ] )

def _describe_resource_file( tree_meta, resource_world, is_global = False ):
    if is_global:
        resources_element = describe_element( 'Resources', min_occurrence = 1 )
    else:
        resources_element = describe_element( 'Resources', exact_occurrence = 1, read_only = True )
    resources_element.add_attributes( [
        identifier_attribute( 'id', mandatory = True, read_only = True, tooltip="Resource Id for this level\nMust be scene_{Levelname} (read only)",
                              reference_family = 'resources',
                              reference_world = resource_world ),
        ] )
    resources_element.add_elements( [
        describe_element( 'Image', groups = 'image', attributes = [
            identifier_attribute( 'id', mandatory = True, reference_family = 'image',
                display_id = True, reference_world = resource_world ),
            path_attribute( 'path', strip_extension = '.png', mandatory = True )
            ] ),
        describe_element( 'Sound', groups = 'resource', attributes = [
            identifier_attribute( 'id', mandatory = True, reference_family = 'sound',
                display_id = True, reference_world = resource_world ),
            path_attribute( 'path', strip_extension = '.ogg', mandatory = True )
            ] ),
        describe_element( 'SetDefaults', read_only = True, groups = 'resource', 
                          attributes = [
            string_attribute( 'path', mandatory = True, read_only = True ),
            string_attribute( 'idprefix', mandatory = True, allow_empty = True, read_only = True )
            ] )
        ] )
    if is_global:
        resources_element.add_elements( [
            describe_element( 'font', groups = 'resource', attributes = [
                identifier_attribute( 'id', mandatory = True, reference_family = 'font',
                    display_id = True, reference_world = resource_world ),
                path_attribute( 'path', strip_extension = '.png', mandatory = True ) # @todo also check existence of .txt
                ] )
        ] )
    
    tree_meta.add_elements( [
        # DUPLICATED FROM GLOBAL SCOPE => makes FACTORY function ?
        describe_element( 'ResourceManifest', exact_occurrence = 1, groups = 'resource',
                          attributes = [], elements = [
            resources_element
            ] )
        ] )

_describe_resource_file( TREE_LEVEL_RESOURCE, WORLD_LEVEL )


# Values for Tag attribute
#v0.70 - kindasticky tag removed - came from a mistake by 2dBoy in Second Hand Smoke
_TAG_VALUES = ('ballbuster', 'break=1', 'break=2', 'deadly', 'detaching', 'geomkiller',
      'mostlydeadly', 'stopsign', 'unwalkable', 'walkable' )

ELEMENT_BUTTON = describe_element( 'button', groups = 'image', attributes = [
        string_attribute( 'id', display_id = True, mandatory = True ),
        xy_attribute( 'center', mandatory = True, init = '0,0', map_to = ('x','y') ,position=True),
        real_attribute( 'depth', mandatory = True, init = '10' ),
        real_attribute( 'alpha', mandatory = True, init = '1' ),
        real_attribute( 'rotation', mandatory = True, init = '0' ),
        scale_attribute( 'scale', init = '1,1', min_value = 0.0000001, mandatory = True,
                         map_to = ('scalex', 'scaley') ),
        rgb_attribute( 'colorize', mandatory = True, init = '255,255,255' ),
        reference_attribute( 'up', reference_family = 'image', reference_world = WORLD_LEVEL,
                             init = '', mandatory = True ),
        reference_attribute( 'over', reference_family = 'image', reference_world = WORLD_LEVEL,
                             init = '', mandatory = True ),
        enum_attribute( 'context', ('screen') ),
        reference_attribute( 'disabled', reference_family = 'image', reference_world = WORLD_LEVEL ),
        reference_attribute( 'font', reference_family = 'font', reference_world = WORLD_GLOBAL ),
        string_attribute( 'onclick' ),
        string_attribute( 'onmouseenter' ),
        string_attribute( 'onmouseexit' ),
        bool_attribute( 'overlay' ),
        bool_attribute( 'screenspace' ),
        reference_attribute( 'text', reference_family = 'text', reference_world = WORLD_GLOBAL ),
        argb_attribute( 'textcolorup' ),
        argb_attribute( 'textcolorupover' ),
        reference_attribute( 'tooltip', reference_family = 'text', reference_world = WORLD_GLOBAL )
        ] )

ELEMENT_RECTANGLE = describe_element( 'rectangle', groups = 'rect', attributes = [
    identifier_attribute( 'id', display_id = True, mandatory = True,  allow_empty = True,
        reference_family = 'geometry', reference_world = WORLD_LEVEL ),
    xy_attribute( 'center', mandatory = True, init = '0,0', map_to = ('x','y') ,position=True),
    size_attribute( 'size', mandatory = True, init = '100,100', map_to = ('width', 'height') ),
    angle_radians_attribute( 'rotation', mandatory = True, init = '0' ),
    bool_attribute( 'static', default='true', tooltip="If static=true this object cannot move\nIf static=false or empty then a mass is required." ),
    real_attribute( 'mass', tooltip="Mass of this item.\nRequired if static=false or empty" ),
    reference_attribute( 'material', reference_family = 'material', reference_world = WORLD_GLOBAL,
                          init = '' ,allow_empty=True,remove_empty=True),
    enum_attribute( 'tag', _TAG_VALUES, is_list = True, allow_empty=True, remove_empty=True ),
    reference_attribute( 'image', reference_family = 'image', reference_world = WORLD_LEVEL,
                         init = '', allow_empty=True,remove_empty=True ),
    xy_attribute( 'imagepos', allow_empty=True,remove_empty=True ),
    angle_radians_attribute( 'imagerot',allow_empty=True,remove_empty=True ),
    scale_attribute( 'imagescale', allow_empty=True,remove_empty=True ),
    real_attribute( 'rotspeed',allow_empty=True,remove_empty=True ),
    bool_attribute( 'contacts' ,allow_empty=True,remove_empty=True),
    bool_attribute( 'collide',allow_empty=True,remove_empty=True ),
    bool_attribute( 'nogeomcollisions',allow_empty=True,remove_empty=True)
    ] )

ELEMENT_CHILD_RECTANGLE = describe_element( 'rectangle', groups = 'rect', attributes = [
    string_attribute( 'id', display_id = True,  allow_empty = True ),
    xy_attribute( 'center', mandatory = True, init = '0,0', map_to = ('x','y'),position=True ),
    size_attribute( 'size', mandatory = True, init = '100,100', map_to = ('width', 'height') ),
    angle_radians_attribute( 'rotation', mandatory = True, init = '0' ),
    real_attribute( 'mass', tooltip="Mass of this child" )
] )



ELEMENT_CIRCLE = describe_element( 'circle', groups = 'circle', attributes = [
    identifier_attribute( 'id', display_id = True, mandatory = True, allow_empty = True, 
        reference_family = 'geometry', reference_world = WORLD_LEVEL ),
    xy_attribute( 'center', mandatory = True, init = '0,0', map_to = ('x','y') ,position=True),
    radius_attribute( 'radius', mandatory = True, init = '75' ),
    bool_attribute( 'static', default='true',tooltip="If static=true this object cannot move\nIf static=false or empty then a mass is required." ),
    real_attribute( 'mass',tooltip="Mass of this item.\nRequired if static=false or empty" ,allow_empty=True,remove_empty=True ),
    reference_attribute( 'material', reference_family = 'material', reference_world = WORLD_GLOBAL,
                          init = '' , allow_empty=True,remove_empty=True),
    enum_attribute( 'tag', _TAG_VALUES, is_list = True, allow_empty=True, remove_empty=True),
    reference_attribute( 'image', reference_family = 'image', reference_world = WORLD_LEVEL,
                         init = '', allow_empty=True,remove_empty=True ),
    xy_attribute( 'imagepos',allow_empty=True,remove_empty=True ),
    angle_radians_attribute( 'imagerot',allow_empty=True,remove_empty=True ),
    scale_attribute( 'imagescale',allow_empty=True,remove_empty=True ),
    real_attribute( 'rotspeed',allow_empty=True,remove_empty=True ),
    bool_attribute( 'contacts' ),
    bool_attribute( 'nogeomcollisions',tooltip="If true, Goo strands cannot go through this object")
    ] )

ELEMENT_CHILD_CIRCLE = describe_element( 'circle', groups = 'circle', attributes = [
    string_attribute( 'id', display_id = True, allow_empty = True ),
    xy_attribute( 'center', mandatory = True, init = '0,0', map_to = ('x','y'),position=True ),
    radius_attribute( 'radius', mandatory = True, init = '75' ),
    real_attribute( 'mass',tooltip="Mass of this child" ,allow_empty=True,remove_empty=True )
    ] )

TREE_LEVEL_SCENE.add_elements( [
    describe_element( 'scene', exact_occurrence = 1, groups = 'image', attributes = [
        rgb_attribute( 'backgroundcolor', mandatory = True, init = '0,0,0' ),
        real_attribute( 'minx', init='-500', tooltip="Left edge of playing area\nCamera will not show past here.",allow_empty=True,remove_empty=True ),
        real_attribute( 'miny', init='-500', tooltip="Bottom edge of playing area\nCamera will not show past here.",allow_empty=True,remove_empty=True),
        real_attribute( 'maxx', init='500', tooltip="Right edge of playing area\nCamera will not show past here.",allow_empty=True,remove_empty=True),
        real_attribute( 'maxy', init='500',tooltip="Top edge of playing area\nCamera will not show past here." ,allow_empty=True,remove_empty=True)
        ],
        elements = [
        describe_element( 'SceneLayer', groups = 'image', attributes = [
            string_attribute( 'name', display_id = True),
            reference_attribute( 'image', reference_family = 'image', reference_world = WORLD_LEVEL,
                                 init = '', mandatory = True ),
            xy_attribute( 'center', mandatory = True, init='0,0', map_to = ('x','y') ,position=True),
            scale_attribute( 'scale', default = '1,1',min_value = 0.000001,  map_to = ('scalex', 'scaley'),allow_empty=True,remove_empty=True ),
            angle_degrees_attribute( 'rotation', default = '0',allow_empty=True,remove_empty=True),
            real_attribute( 'depth', mandatory = True, init='0' ),
            bool_attribute( 'tilex', default='false', allow_empty=True,remove_empty=True),
            bool_attribute( 'tiley', default='false',allow_empty=True,remove_empty=True),
            int_attribute ( 'tilecountx',allow_empty=True,remove_empty=True),
            int_attribute ( 'tilecounty',allow_empty=True,remove_empty=True),
            real_attribute( 'alpha', min_value = 0, max_value = 1, default = '1' , allow_empty=True,remove_empty=True),
            rgb_attribute( 'colorize', init = '255,255,255', allow_empty=True,remove_empty=True),
            enum_attribute( 'anim',ANIMATIONS_GLOBAL, allow_empty=True, remove_empty=True ),     # @todo Animation are filename in res/anim/*.bintl
            real_attribute( 'animspeed',allow_empty=True,remove_empty=True),
            real_attribute( 'animdelay', min_value = 0.0001, default = '1',allow_empty=True,remove_empty=True ),
            identifier_attribute( 'id', allow_empty = True, remove_empty=True,
                reference_family = 'image', reference_world = WORLD_LEVEL ),
            enum_attribute( 'context', ('screen'),allow_empty=True,remove_empty=True ),
            ] ),
        ELEMENT_BUTTON,
        describe_element( 'buttongroup', groups = 'image', attributes = [
            string_attribute( 'id', mandatory = True ),
            xy_attribute( 'osx', mandatory = True )
            ],
            elements = [
                ELEMENT_BUTTON
            ] ),
        describe_element( 'label', groups = 'text', attributes = [
            string_attribute( 'id', display_id = True, mandatory = True,init = '', allow_empty = True ),
            xy_attribute( 'position', mandatory = True, init = '0,0', map_to = ('x','y') ,position=True),
            angle_degrees_attribute( 'rotation', mandatory = True, init = '0' ),
            real_attribute( 'scale', mandatory = True, init = '1' ),
            reference_attribute( 'text', reference_family = 'TEXT_LEVELNAME_STR', reference_world = WORLD_LEVEL, mandatory = True),
            reference_attribute( 'font', reference_family = 'font', reference_world = WORLD_GLOBAL,
                                 mandatory = True ),
            enum_attribute( 'align', ('right', 'center', 'left'), mandatory = True, init = 'center' ),
            real_attribute( 'depth', mandatory = True, init = '10' ),
            bool_attribute( 'overlay', init = 'false', allow_empty=True, remove_empty=True ),
            bool_attribute( 'screenspace', init = 'false',allow_empty=True, remove_empty=True )
            ] ),
        ELEMENT_RECTANGLE,
        ELEMENT_CIRCLE,
        describe_element( 'compositegeom', groups = 'compgeom', attributes = [
            identifier_attribute( 'id', display_id = True, 
                mandatory = True, allow_empty = True, 
                reference_family = 'geometry', reference_world = WORLD_LEVEL ),
            xy_attribute( 'center', mandatory = True, init = '0,0', map_to = ('x','y') ,position=True),
            angle_radians_attribute( 'rotation', mandatory = True, init = '0' ),
            bool_attribute( 'static', default = 'true' ,tooltip="If static=true this object and its children cannot move\nIf static=false then all children require a mass."),
            reference_attribute( 'material', reference_family = 'material', reference_world = WORLD_GLOBAL,
                                 init = '' ,allow_empty=True,remove_empty=True),
            enum_attribute( 'tag', _TAG_VALUES, is_list = True,allow_empty=True,remove_empty=True ),
            reference_attribute( 'image', reference_family = 'image', reference_world = WORLD_LEVEL,allow_empty=True,remove_empty=True ),
            xy_attribute( 'imagepos',allow_empty=True,remove_empty=True),
            angle_radians_attribute( 'imagerot',allow_empty=True,remove_empty=True ),
            scale_attribute( 'imagescale',allow_empty=True,remove_empty=True ),
            real_attribute( 'rotspeed' ,allow_empty=True,remove_empty=True),
            bool_attribute( 'contacts' ),
            bool_attribute( 'nogeomcollisions' )
            ],
            elements = [
                ELEMENT_CHILD_RECTANGLE,
                ELEMENT_CHILD_CIRCLE
            ] ),
        describe_element( 'line', groups = 'line', attributes = [
            string_attribute( 'id', display_id = True, mandatory = True, 
                              allow_empty = True, init='' ),
            xy_attribute( 'anchor', mandatory = True, init = '0,0' ,position=True),
            dxdy_attribute( 'normal', mandatory = True, init = '1,0' ),
            reference_attribute( 'material', reference_family = 'material', reference_world = WORLD_GLOBAL,
                                 init = '', allow_empty=True,remove_empty=True ),
           # bool_attribute( 'static', init = 'true' ),
            enum_attribute( 'tag', _TAG_VALUES, is_list = True , allow_empty=True, remove_empty=True )
            ] ),
        describe_element( 'linearforcefield', groups = 'physic', attributes = [
            string_attribute( 'id', display_id = True, allow_empty = True ),
            xy_attribute( 'center', init = '0,0' , allow_empty=True,remove_empty=True,position=True),
            size_attribute( 'size', map_to = ('width', 'height') , allow_empty=True,remove_empty=True),
            enum_attribute( 'type', ('force', 'gravity'), init = 'gravity', mandatory = True ),
            dxdy_attribute( 'force', mandatory = True, init = '0,-10' ),
            real_attribute( 'dampeningfactor', mandatory = True, init = '0' ),
            bool_attribute( 'antigrav', mandatory = True, init = 'false' ),
            real_attribute( 'depth' ,allow_empty=True,remove_empty=True),
            argb_attribute( 'color' ,allow_empty=True,remove_empty=True),
            bool_attribute( 'enabled' ,allow_empty=True,remove_empty=True),
            bool_attribute( 'water', default = 'false' ,allow_empty=True,remove_empty=True),
            bool_attribute( 'geomonly' ,allow_empty=True,remove_empty=True)
            ] ),
        describe_element( 'radialforcefield', groups = 'physic', attributes = [
            string_attribute( 'id', display_id = True, allow_empty = True ),
            xy_attribute( 'center', mandatory = True, init = '0,0' ,position=True),
            radius_attribute( 'radius', mandatory = True, init = '100'),
#            enum_attribute( 'type', ('force', 'gravity'), init = 'gravity', mandatory = True ), # @todo in game, only gravity
            real_attribute( 'forceatcenter', mandatory = True, init = '10' ),
            real_attribute( 'forceatedge', mandatory = True, init = '0' ),
            real_attribute( 'dampeningfactor', mandatory = True, init = '0' ),
            bool_attribute( 'antigrav', mandatory = True, init = 'false' ),
#            real_attribute( 'depth' ),
#            bool_attribute( 'enabled',allow_empty=True,remove_empty=True )
#            bool_attribute( 'geomonly' )
            ] ),
        describe_element( 'hinge', groups = 'physic', attributes = [
            xy_attribute( 'anchor', mandatory = True ,position=True),
            reference_attribute( 'body1', mandatory = True, 
                reference_family = 'geometry', reference_world = WORLD_LEVEL ),
            reference_attribute( 'body2', allow_empty=True,remove_empty=True,
                reference_family = 'geometry', reference_world = WORLD_LEVEL ),
            real_attribute( 'bounce' ,allow_empty=True,remove_empty=True),
            real_attribute( 'histop' ,allow_empty=True,remove_empty=True),
            real_attribute( 'lostop' ,allow_empty=True,remove_empty=True),
            real_attribute( 'stopcfm' ,allow_empty=True,remove_empty=True),
            real_attribute( 'stoperp' ,allow_empty=True,remove_empty=True)
            ] ),
        describe_element( 'slider', groups = 'physic', attributes = [
            reference_attribute( 'body1', mandatory = True,
                reference_family = 'geometry', reference_world = WORLD_LEVEL ),
            reference_attribute( 'body2',mandatory = True,
                reference_family = 'geometry', reference_world = WORLD_LEVEL ),
            dxdy_attribute('axis', mandatory=True),
            real_attribute( 'bounce', allow_empty=True,remove_empty=True),
            real_attribute( 'histop', allow_empty=True,remove_empty=True),
            real_attribute( 'lostop', allow_empty=True,remove_empty=True),
            real_attribute( 'stopcfm' ,allow_empty=True,remove_empty=True),
            real_attribute( 'stoperp' ,allow_empty=True,remove_empty=True)
            ] ),

        describe_element( 'motor', groups = 'physic', attributes = [
            reference_attribute( 'body', reference_family = 'geometry', reference_world = WORLD_LEVEL, mandatory = True ),
            real_attribute( 'maxforce', mandatory = True, init = '20' ),
            real_attribute( 'speed', mandatory = True, init = '-0.01' )
            ] ),
        describe_element( 'particles', groups = 'particles', attributes = [
            reference_attribute( 'effect', mandatory = True, display_id = True,
                reference_family = 'effect', reference_world = WORLD_GLOBAL ),
            xy_attribute( 'pos' ,position=True),
            real_attribute( 'depth', mandatory = True, init = '-20' ),
            real_attribute( 'pretick', default = '0' )
            ] )
        ] )
    ] )

LEVEL_TREE_TEXT = describe_element( 'strings', groups = 'text', exact_occurrence = 1, attributes = [],
        elements = [
        describe_element( 'string', groups = 'text', attributes = [
            identifier_attribute( 'id', mandatory = True, display_id = True,
                reference_family = 'TEXT_LEVELNAME_STR', reference_world = WORLD_LEVEL ),
            text_attribute( 'text', mandatory = True,tooltip="Use | symbol (pipe) to get new pages on signs, and new lines in labels" ),
            text_attribute( 'de' ,allow_empty=True,remove_empty=True),
            text_attribute( 'es' ,allow_empty=True,remove_empty=True),
            text_attribute( 'fr' ,allow_empty=True,remove_empty=True),
            text_attribute( 'it' ,allow_empty=True,remove_empty=True),
            text_attribute( 'nl' ,allow_empty=True,remove_empty=True),
            text_attribute( 'pt' ,allow_empty=True,remove_empty=True)
            ] )
        ] )

GLOBAL_TREE_TEXT = describe_element( 'strings', exact_occurrence = 1, attributes = [], elements = [
        describe_element( 'string',min_occurrence=1 , attributes = [
            identifier_attribute( 'id', mandatory = True, display_id = True,
                reference_family = 'text', reference_world = WORLD_GLOBAL ),
            string_attribute( 'text', mandatory = True ),
            string_attribute( 'de' ),
            string_attribute( 'es' ),
            string_attribute( 'fr' ),
            string_attribute( 'it' ),
            string_attribute( 'nl' ),
            string_attribute( 'pt' )
            ] )
        ] )

TREE_GLOBAL_TEXT.add_elements( [GLOBAL_TREE_TEXT] )
TREE_LEVEL_TEXT.add_elements( [LEVEL_TREE_TEXT] )

DEP_IMAGE = describe_element('image',groups = 'image',read_only = True,attributes=[
      string_attribute('id',read_only = True),
      identifier_attribute('path',read_only = True,display_id=True,reference_family="imagedep",reference_world=WORLD_LEVEL)
      ,bool_attribute('found',read_only = True,mandatory=True,default=False)
])
DEP_SOUND = describe_element('sound',groups = 'resource',read_only = True, attributes=[
      string_attribute('id',read_only = True),
      identifier_attribute('path',read_only = True,display_id=True,reference_family="sounddep",reference_world=WORLD_LEVEL)
      ,bool_attribute('found',read_only = True,mandatory=True,default=False)
])

DEP_PARTICLES = describe_element('effect',read_only = True,groups='particles',attributes=[
      identifier_attribute('name',read_only = True,display_id=True,reference_family="effectdep",reference_world=WORLD_LEVEL)
     ,bool_attribute('found',read_only = True,mandatory=True,default=False)
],
      elements=[DEP_IMAGE])

DEP_MATERIAL = describe_element('material',groups='material', read_only = True,attributes=[
    identifier_attribute('id',read_only = True,display_id=True,reference_family="materialdep",reference_world=WORLD_LEVEL)
    ,bool_attribute('found',read_only = True,mandatory=True,default=False)
])

DEP_ANIM = describe_element('anim',groups='anim', read_only = True,attributes=[
    enum_attribute( 'id',ANIMATIONS_GLOBAL, read_only=True, display_id=True  ),
    bool_attribute('found',read_only = True,mandatory=True,default=False)
])

DEP_BALL = describe_element('ball',read_only = True, groups = 'ball',attributes=[
    identifier_attribute('name',read_only = True,display_id=True,reference_family="balldep",reference_world=WORLD_LEVEL)
    ,bool_attribute('found',read_only = True,mandatory=True,default=False) ],
    elements=[ DEP_IMAGE,DEP_SOUND,DEP_PARTICLES,DEP_MATERIAL])

DEP_BALL.add_elements([DEP_BALL])

TREE_LEVEL_DEPENDANCY.add_elements([describe_element( 'dependancy',groups="goomod",
            read_only = True,exact_occurrence = 1, attributes = [],
            elements=[DEP_BALL,DEP_IMAGE,DEP_SOUND,DEP_PARTICLES,DEP_MATERIAL,DEP_ANIM])])

_describe_resource_file( TREE_GLOBAL_RESOURCE, WORLD_GLOBAL, is_global = True )

ELEMENT_PARTICLE = describe_element( 'particle', groups = 'image', 
                                     min_occurrence = 1, attributes = [
    dxdy_attribute( 'acceleration', mandatory = True, init = '0,0.1' ),
    bool_attribute( 'directed', mandatory = True, init = 'false' ),
    reference_attribute( 'image', is_list=True, reference_family = 'image', reference_world = WORLD_GLOBAL,
                         mandatory = True ),
    angle_degrees_attribute( 'movedir', mandatory = True, init = '0' ),
    angle_degrees_attribute( 'movedirvar', mandatory = True, init = '0' ), # ?
    scale_attribute( 'scale', mandatory = True, init = '1,1' ),  
    dxdy_attribute( 'speed', mandatory = True, init = '1,1' ),  
    bool_attribute( 'additive' ),
    real_attribute( 'dampening', min_value = 0, max_value = '1' ),
    bool_attribute( 'fade' ),
    real_attribute( 'finalscale', min_value = 0 ),
    xy_attribute( 'lifespan' ), # @todo TYPE OPTIONAL INTERVAL (e.g. 1 or 1,2 are ok)?
    xy_attribute( 'rotation' ), # @todo TYPE OPTIONAL INTERVAL (e.g. 1 or 1,2 are ok)?
    xy_attribute( 'rotspeed' ) # @todo TYPE OPTIONAL INTERVAL (e.g. 1 or 1,2 are ok)?
    ],
    elements = [
        describe_element( 'axialsinoffset', min_occurrence = 1, max_occurrence = 2, attributes = [
            xy_attribute( 'amp', mandatory = True, init = '5,10' ), # @todo just 2 reals (interval)
            enum_attribute( 'axis', ('x','y'), mandatory = True, init = 'x' ),
            xy_attribute( 'freq', mandatory = True, init = '5,10' ),# @todo just 2 reals (interval)
            xy_attribute( 'phaseshift', mandatory = True, init = '0.2,0.4' ),# @todo just 2 reals (interval)
        ] )
    ] )
    
TREE_GLOBAL_FX.add_elements( [
    describe_element( 'effects', groups = 'image', 
                      exact_occurrence = 1, attributes = [], elements = [
        describe_element( 'ambientparticleeffect', attributes = [
            identifier_attribute( 'name', display_id = True, mandatory = True, 
                reference_family = 'effect', reference_world = WORLD_GLOBAL ),
            int_attribute( 'maxparticles', min_value = 1, mandatory = True, init = '1' ),
            int_attribute( 'margin' ) # ???
            ],
            elements = [
                ELEMENT_PARTICLE
            ] ),
        describe_element( 'particleeffect', groups = 'image', 
                          attributes = [
            identifier_attribute( 'name', display_id = True, mandatory = True, 
                reference_family = 'effect', reference_world = WORLD_GLOBAL ),
            int_attribute( 'maxparticles', min_value = 1, mandatory = True, init = '1' ),
            real_attribute( 'rate', min_value = 0.00001 ),
            int_attribute( 'margin' ) # ???
            ],
            elements = [
                ELEMENT_PARTICLE
            ] )
        ])
    ] )



TREE_GLOBAL_MATERIALS.add_elements( [
    describe_element( 'materials', groups = 'physic', 
                      exact_occurrence = 1, attributes = [], elements = [
        describe_element( 'material', attributes = [
            identifier_attribute( 'id', display_id = True, mandatory = True, 
                reference_family = 'material', reference_world = WORLD_GLOBAL ),
            real_attribute( 'bounce', min_value = 0, mandatory = True, init = '0' ),
            real_attribute( 'friction', min_value = 0, mandatory = True, init = '0' ),
            real_attribute( 'minbouncevel', min_value = 0, mandatory = True, init = '100' ),
            real_attribute( 'stickiness', min_value = 0 )
            ] )
        ] )
    ] )


_describe_resource_file( TREE_BALL_RESOURCE, WORLD_BALL )

BALL_EVENTS=['attach','attachcloser','bounce',
             'collidediff','collidegeom','collidesame',
             'death','deathfall',
             'detached','detaching',
             'detonate','drop','exit','extinguish','ignite','land',
             'marker','pickup','snap','suction','throw' ]

BALL_PROPS =['name','shape','mass','strands','material','towermass','dragmass',
        'walkspeed','climbspeed','dampening','walkforce','speedvariance',
        'draggable','detachable','hingedrag',
        'invulnerable','suckable','autobounds','autoboundsunattached','sticky','stickyattached','stickyunattached','grumpy','collidewithattached','collideattached','stacking',
        'jump','climber','wakedist','antigrav','isantigravunattached','static','staticwhensleeping','jumponwakeup','autoattach','maxattachspeed',
        'burntime','detonateforce','detonateradius','explosionparticles','flammable',
        'contains','popduration','popparticles','popsound','popdelay',
        'spawn',
        'blinkcolor','hideeyes','alwayslookatmouse','statescales','isbehindstrands','fling','distantsounds',
        'attenuationdeselect','attenuationdrag','attenuationdrop','attenuationselect','autodisable','fallingattachment','stuckattachment'
        ]

# NOTES: this has been generated from scanxmlfile with -w options. Need a lot of clean up.
TREE_BALL_MAIN.add_elements( [
        describe_element( 'ball', groups = 'game', 
                          min_occurrence=1, max_occurrence=1, attributes = [
            identifier_attribute( 'name', display_id = True, mandatory = True, 
                reference_family = 'ball', reference_world = WORLD_GLOBAL ),
            int_attribute( 'mass', min_value=0.00001, mandatory = True, init = '20'),  # [8-600] Median:20 Samples: 20 | 30 | 10 | 60 | 600
            unknown_attribute( 'shape', mandatory = True, init = 'circle,30'),  # Samples: circle,30 | circle,30,0.25 | circle,24,0.1 | circle,50,0 | rectangle,50,50
            real_attribute( 'speedvariance', min_value=0, mandatory = True, init = '0.2'),  # [0.0-0.3] Median:0.2 Samples: 0.2 | 0 | 0.3 | 0.0
            int_attribute( 'strands', min_value=0, mandatory = True, init = '0'),  # [0-4] Median:1 Samples: 0 | 2 | 1 | 4 | 3
            real_attribute( 'walkspeed', min_value=0, mandatory = True, init = '0'),  # [0.0-0.15] Median:0.1 Samples: 0 | 0.1 | 0.0 | 0.05 | 0.15
            real_attribute( 'climbspeed', min_value=0, init = '0'),  # [0.0-2.8] Median:1.0 Samples: 0 | 2.0 | 0.0 | 1.8 | 0.9
            real_attribute( 'towermass', min_value=0, init = '5'),  # [0.0-200.0] Median:5.0 Samples: 5 | 3.0 | 10 | 200 | 20
            unknown_attribute( 'jump', init = '0.0,0.0'),  # Samples: 0.0,0.0 | 0,0 | 0.0,0.3 | 0.4,1.2 | 0.0,0.4
            bool_attribute( 'detachable', init = 'false'),
            bool_attribute( 'draggable', init = 'false'),
            bool_attribute( 'grumpy', init = 'true'),
            bool_attribute( 'invulnerable', init = 'true'),
            rgb_attribute( 'blinkcolor', init = '0,0,0'),  # Samples: 0,0,0 | 0,255,0
            bool_attribute( 'suckable', init = 'false'),
            int_attribute( 'walkforce', min_value=0, init = '0'),  # [0-3000] Median:0 Samples: 0 | 500 | 3000
            bool_attribute( 'autoboundsunattached', init = 'true'),
            bool_attribute( 'hideeyes', init = 'false'),
            real_attribute( 'burntime', min_value=0.00001, init = '2.0'),  # [0.1-7.0] Median:2.0 Samples: 2.0 | 3.0 | 5 | 0.1 | 7.0
            bool_attribute( 'collidewithattached', init = 'true'),
            real_attribute( 'detonateforce', min_value=0, init = '500'),  # [0.0-1000.0] Median:300.0 Samples: 500 | 300 | 0.01 | 0 | 2
            int_attribute( 'detonateradius', min_value=0, init = '100'),  # [0-500] Median:100 Samples: 100 | 200 | 0 | 350 | 10
            bool_attribute( 'climber', init = 'false'),
            bool_attribute( 'collideattached', init = 'true'),
            bool_attribute( 'alwayslookatmouse', init = 'true'),
#            unknown_attribute( 'material', init = 'BlockBall'),  # Samples: BlockBall | rock | BoneBall | UglyBall | BeautyBall
            reference_attribute( 'material', reference_family = 'material', reference_world = WORLD_GLOBAL),
            bool_attribute( 'stuckattachment', init = 'false'),
            int_attribute( 'wakedist', min_value=0.00001, init = '30'),  # [30-600] Median:30 Samples: 30 | 200 | 60 | 600
            bool_attribute( 'autobounds', init = 'false'),
            unknown_attribute( 'attenuationdeselect', init = '0, 1'),  # Samples: 0, 1 | 0.05, 1.2, 1.0 | 0.1, 1.1, 1.0
            unknown_attribute( 'attenuationdrag', init = '0, 1'),  # Samples: 0, 1 | 0.05, 1.2, 1.0 | 0.1, 1.1, 1.0
            unknown_attribute( 'attenuationdrop', init = '0, 1'),  # Samples: 0, 1 | 0.05, 1.2, 1.0 | 0.1, 1.1, 1.0
            unknown_attribute( 'attenuationselect', init = '0, 1'),  # Samples: 0, 1 | 0.05, 1.0, 1.2 | 0.1, 1.0, 1.1
            reference_attribute( 'contains', reference_family = 'ball', reference_world = WORLD_GLOBAL,is_list=True, init = '10,UtilGooGlobber'),  # Samples: 10,UtilGooGlobber | 42,ZBomb | 1,Pilot | 80,UndeletePillFizz | 4,Spam
            real_attribute( 'popduration', min_value=0.00001, init = '0.25'),  # [0.1-4.0] Median:0.25 Samples: 0.25 | 1.0 | 0.1 | 4
#           unknown_attribute( 'popparticles', init = 'beautypop'),  # Samples: beautypop | OOS_gooGlobs | ISH_undeleteFizz | ish_bitPop
            reference_attribute( 'popparticles', reference_family = 'effect', reference_world = WORLD_GLOBAL ),
#            unknown_attribute( 'popsound', init = 'SOUND_BALL_BIT_POP1'),  # Samples: SOUND_BALL_BIT_POP1 | SOUND_BALL_ZBOMBMOM_POP | SOUND_BALL_UNDELETEPILLFIZZ_POP4 | SOUND_BALL_UNDELETEPILL_POP | SOUND_BALL_UTILGOOGLOBBERMOM_POP
            reference_attribute( 'popsound', reference_family = 'sound', reference_world = WORLD_BALL ),
            unknown_attribute( 'statescales', init = 'tank,0.12'),  # Samples: tank,0.12 | attached,1.25 | attached,1.65, detaching,1.3 | attached,1.75, detaching,1.3, tank,1.0
            real_attribute( 'antigrav', min_value=0.00001, init = '1.0'),  # [0.5-14.0] Median:1.0 Samples: 1.0 | 4.5 | 14 | 0.5 | 3
#            unknown_attribute( 'explosionparticles', init = 'BallExplode_ISH'),  # Samples: BallExplode_ISH | BallExplode_Bomb | BallExplode_Fuse
            reference_attribute( 'explosionparticles', reference_family = 'effect', reference_world = WORLD_GLOBAL ),
            bool_attribute( 'static', init = 'true'),
            bool_attribute( 'sticky', init = 'false'),
            bool_attribute( 'isbehindstrands', init = 'false'),
            bool_attribute( 'stacking', init = 'true'),
            real_attribute( 'dampening', min_value=0.00001, init = '0.1'),  # [0.005-0.115] Median:0.1 Samples: 0.1 | 0.005 | 0.115
            bool_attribute( 'hingedrag', init = 'true'),
            bool_attribute( 'jumponwakeup', init = 'true'),
            int_attribute( 'maxattachspeed', min_value=0.00001, init = '1000'),  # [1000-1000] Median:1000 Samples: 1000
            bool_attribute( 'staticwhensleeping', init = 'true'),
            bool_attribute( 'autodisable', init = 'true'),
            int_attribute( 'dragmass', min_value=0.00001, init = '100'),  # [40-100] Median:40 Samples: 100 | 40
            bool_attribute( 'isantigravunattached', init = 'true'),
            bool_attribute( 'stickyattached', init = 'true'),
            bool_attribute( 'stickyunattached', init = 'false'),
            unknown_attribute( 'fling', init = '200,2.5'),  # Samples: 200,2.5
            unknown_attribute( 'popdelay', init = '2,12'),  # Samples: 2,12 | 2,2
#            unknown_attribute( 'spawn', init = 'WindowRect'),  # Samples: WindowRect | WindowSquare
            reference_attribute( 'spawn',  reference_family = 'ball', reference_world = WORLD_GLOBAL ),
            bool_attribute( 'autoattach', init = 'true'),
            bool_attribute( 'distantsounds', init = 'false'),
            bool_attribute( 'fallingattachment', init = 'false'),
            bool_attribute( 'flammable', init = 'false'),
        ], elements = [
            describe_element( 'detachstrand', min_occurrence=0, max_occurrence=1, attributes = [
                int_attribute( 'maxlen', min_value=0.00001, mandatory = True, init = '60'),  # [60-160] Median:60 Samples: 60 | 160 | 70
#                unknown_attribute( 'image', init = 'IMAGE_BALL_BALLOON_SPLAT1'),  # Samples: IMAGE_BALL_BALLOON_SPLAT1 | IMAGE_BALL_COMMON_BLACK_DSTRAND | IMAGE_BALL_WATER_DSTRAND | IMAGE_BALL_POKEY_DSTRAND | IMAGE_BALL_PILOT_ARROW
                reference_attribute( 'image', reference_family = 'image', reference_world = WORLD_BALL)
            ] ),
            describe_element( 'marker', min_occurrence=0, max_occurrence=1, attributes = [
#                unknown_attribute( 'detach', mandatory = True, init = 'IMAGE_BALL_POKEY_DETACHMARKER_P1'),  # Samples: IMAGE_BALL_POKEY_DETACHMARKER_P1 | IMAGE_BALL_COMMON_DRAGMARKER_P1 | IMAGE_BALL_IVY_DETACHMARKER_P1 | IMAGE_BALL_UTILATTACHWALKABLE_DRAGMARKER_P1 | IMAGE_BALL_RECTHEAD_DRAGMARKER
#                unknown_attribute( 'drag', mandatory = True, init = 'IMAGE_BALL_COMMON_DRAGMARKER_P1'),  # Samples: IMAGE_BALL_COMMON_DRAGMARKER_P1 | IMAGE_BALL_UTILATTACHWALKABLE_DRAGMARKER_P1 | IMAGE_BALL_DRAINED_DRAGMARKER_P1 | IMAGE_BALL_BOMBSTICKY_DRAGMARKER | IMAGE_BALL_RECTHEAD_DRAGMARKER
                reference_attribute( 'detach', reference_family = 'image', reference_world = WORLD_BALL),
                reference_attribute( 'drag', reference_family = 'image', reference_world = WORLD_BALL),
                int_attribute( 'rotspeed', mandatory = True, init = '2'),  # [-2-2] Median:2 Samples: 2 | -2 | 0
            ] ),
            describe_element( 'part', min_occurrence=0, attributes = [
#                unknown_attribute( 'image', mandatory = True, init = 'IMAGE_BALL_GENERIC_EYE_GLASS_1,IMAGE_BALL_GENERIC_EYE_GLASS_2,IMAGE_BALL_GENERIC_EYE_GLASS_3'),  # Samples: IMAGE_BALL_GENERIC_EYE_GLASS_1,IMAGE_BALL_GENERIC_EYE_GLASS_2,IMAGE_BALL_GENERIC_EYE_GLASS_3 | IMAGE_BALL_GENERIC_EYE_GLASS_1,IMAGE_BALL_GENERIC_EYE_GLASS_2 | IMAGE_BALL_GENERIC_EYE_GLASS_1 | IMAGE_BALL_GENERIC_HILITE2 | IMAGE_BALL_GENERIC_HILITE1
                reference_attribute( 'image', is_list=True, reference_family = 'image', reference_world = WORLD_BALL),
                int_attribute( 'layer', min_value=0, mandatory = True, init = '2'),  # [0-4] Median:1 Samples: 2 | 1 | 0 | 3 | 4
#                unknown_attribute( 'name', mandatory = True, init = 'body'),  # Samples: body | lefteye | righteye | lips | hilite1
                identifier_attribute( 'name', display_id = True, mandatory = True,  reference_family = 'part', reference_world = WORLD_BALL ),
                real_attribute( 'scale', min_value=0.00001, mandatory = True, init = '0.5'),  # [0.25-1.45] Median:0.71875 Samples: 0.5 | 1 | 0.6 | 0.5390625 | 0.75
                unknown_attribute( 'x', mandatory = True, init = '0'),  # Samples: 0 | -12,-8 | 8,12 | -10,-6 | -5,0
                unknown_attribute( 'y', mandatory = True, init = '0'),  # Samples: 0 | 0,0 | -5,5 | 0,7 | 6,10
                bool_attribute( 'rotate', init = 'true'),
               # unknown_attribute( 'state', init = 'attached'),  # Samples: attached | climbing,walking,falling,dragging,detaching,standing,tank | climbing,walking,falling,attached,dragging,detaching,standing,tank,stuck,stuck_attached,stuck_detaching | climbing,walking,falling,dragging,detaching,standing,tank,sleeping,stuck,stuck_attached,stuck_detaching | climbing,walking,falling,dragging,detaching,standing,tank,sleeping,stuck,stuck_attached,stuck_detaching,pipe
                enum_attribute( 'state', BALL_STATES, mandatory=True, is_list = True,allow_empty=True ),
                unknown_attribute( 'stretch', init = '16,2,0.5'),  # Samples: 16,2,0.5 | 24,1.2,0.9 | 32,2,0.5 | 24,1.75,0.65 | 12,2,0.5
                bool_attribute( 'eye', init = 'true'),
#                unknown_attribute( 'pupil', init = 'IMAGE_BALL_GENERIC_PUPIL1'),  # Samples: IMAGE_BALL_GENERIC_PUPIL1 | IMAGE_BALL_BEAUTY_PUPIL | IMAGE_BALL_UGLY_PUPIL | IMAGE_BALL_BIT_PUPIL | IMAGE_BALL_BEAUTYPRODUCTEYE_PUPIL
                reference_attribute( 'pupil', reference_family = 'image', reference_world = WORLD_BALL),
                int_attribute( 'pupilinset', min_value=0.00001, init = '13'),  # [10-116] Median:13 Samples: 13 | 12 | 10 | 14 | 50
                unknown_attribute( 'xrange', init = '-18,0'),  # Samples: -18,0 | 0,18 | -20,-10 | 10,20 | -90,-70,
                unknown_attribute( 'yrange', init = '-12,12'),  # Samples: -12,12 | 20,40 | -6,6 | -8,8 | -115,-35
            ] ),
            describe_element( 'particles', min_occurrence=0, attributes = [
#                unknown_attribute( 'id', mandatory = True, init = 'sleepyZzz'),  # Samples: sleepyZzz | ish_smallfire | ish_sleepyZzz | poisonBallBurn | fireRobotHead
                reference_attribute( 'id', reference_family = 'effect', reference_world = WORLD_GLOBAL ),
                bool_attribute( 'overball', mandatory = True, init = 'true'),
#                unknown_attribute( 'states', mandatory = True, init = 'sleeping'),  # Samples: sleeping | onfire | falling
                enum_attribute( 'states', BALL_STATES, is_list = True,allow_empty=True,mandatory = True ),
            ] ),
            describe_element( 'shadow', min_occurrence=0, max_occurrence=1, attributes = [
#                unknown_attribute( 'image', mandatory = True, init = 'IMAGE_BALL_GENERIC_SHADOW0'),  # Samples: IMAGE_BALL_GENERIC_SHADOW0 | IMAGE_BALL_GENERIC_SHADOW1 | IMAGE_BALL_BEAUTY_SHADOW | IMAGE_BALL_COMMON_ALBINO_SHADOWGLOW | IMAGE_BALL_UGLY_SHADOW
                reference_attribute( 'image', reference_family = 'image', reference_world = WORLD_BALL),
                bool_attribute( 'additive', init = 'true'),
            ] ),
            describe_element( 'sinvariance', min_occurrence=0, attributes = [
                real_attribute( 'amp', min_value=0, mandatory = True, init = '0.1'),  # [0.0-1.0] Median:0.1 Samples: 0.1 | 0.02 | 0.03 | 0.5 | 0.05
                real_attribute( 'freq', min_value=0, mandatory = True, init = '1.2'),  # [0.0-2.0] Median:0.8 Samples: 1.2 | 0.3 | 1.5 | 0.4 | 0.8
                real_attribute( 'shift', min_value=0, mandatory = True, init = '0.0'),  # [0.0-0.8] Median:0.0 Samples: 0.0 | 0 | 0.5 | 0.8
            ], elements = [
                describe_element( 'sinanim', min_occurrence=1, attributes = [
                    real_attribute( 'amp', mandatory = True, init = '0.1'),  # [-3.0-12.0] Median:0.1 Samples: 0.1 | 2 | 0.5 | 0.2 | 0.06
#                   unknown_attribute( 'axis', mandatory = True, init = 'y'),  # Samples: y | x
                    enum_attribute( 'axis',('x','y'),mandatory=True,init = 'y'),
                    real_attribute( 'freq', min_value=0.00001, mandatory = True, init = '2.0'),  # [0.2-22.0] Median:2.0 Samples: 2.0 | 1.0 | 0.25 | .20 | 1.2
#                    unknown_attribute( 'part', mandatory = True, init = 'body'),  # Samples: body | lefteye | righteye | lefteye,righteye | body,shine
                    reference_attribute( 'part', reference_family = 'part', reference_world = WORLD_BALL,mandatory = True, is_list=True),
                    real_attribute( 'shift', min_value=0, mandatory = True, init = '0'),  # [0.0-0.8] Median:0.0 Samples: 0 | 0.5 | 0.33 | 0.1 | 0.66
#                   unknown_attribute( 'state', mandatory = True, init = 'walking'),  # Samples: walking | climbing | falling | dragging | attached
                    enum_attribute( 'state', BALL_STATES, mandatory=True, is_list = True,allow_empty=True ),
#                    unknown_attribute( 'type', mandatory = True, init = 'scale'),  # Samples: scale | translate
                    enum_attribute( 'type', ('scale','translate'), mandatory=True )
                ] ),
            ] ),
            describe_element( 'sound', min_occurrence=0, attributes = [
#                unknown_attribute( 'event', mandatory = True, init = 'death'),  # Samples: death | land | bounce | pickup | marker
                enum_attribute('event',BALL_EVENTS,mandatory = True),
#                unknown_attribute( 'id', mandatory = True, allow_empty = True, init = 'SOUND_BALL_GENERIC_STICK1,SOUND_BALL_GENERIC_STICK2,SOUND_BALL_GENERIC_STICK3,SOUND_BALL_GENERIC_STICK4,SOUND_BALL_GENERIC_STICK5,SOUND_BALL_GENERIC_STICK6'),  # Samples: SOUND_BALL_GENERIC_STICK1,SOUND_BALL_GENERIC_STICK2,SOUND_BALL_GENERIC_STICK3,SOUND_BALL_GENERIC_STICK4,SOUND_BALL_GENERIC_STICK5,SOUND_BALL_GENERIC_STICK6 | SOUND_BALL_GENERIC_MUMBLE1,SOUND_BALL_GENERIC_MUMBLE2,SOUND_BALL_GENERIC_MUMBLE3,SOUND_BALL_GENERIC_MUMBLE4,SOUND_BALL_GENERIC_MUMBLE5,SOUND_BALL_GENERIC_MUMBLE6,SOUND_BALL_GENERIC_MUMBLE7 | SOUND_BALL_GENERIC_BOUNCE1,SOUND_BALL_GENERIC_BOUNCE2,SOUND_BALL_GENERIC_BOUNCE3,SOUND_BALL_GENERIC_BOUNCE4 | SOUND_BALL_GENERIC_DEATH1,SOUND_BALL_GENERIC_DEATH2,SOUND_BALL_GENERIC_DEATH3,SOUND_BALL_GENERIC_DEATH4,SOUND_BALL_GENERIC_DEATH5 | SOUND_BALL_GENERIC_DETACHED1
                reference_attribute( 'id', is_list=True,  reference_family = 'sound', reference_world = WORLD_BALL )

            ] ),
            describe_element( 'splat', min_occurrence=0, max_occurrence=1, attributes = [
#               unknown_attribute( 'image', mandatory = True, init = 'IMAGE_FX_SMOKEBLACK'),  # Samples: IMAGE_FX_SMOKEBLACK | IMAGE_BALL_WATER_SPLAT1,IMAGE_BALL_WATER_SPLAT2 | IMAGE_BALL_TIMEBUG_SPLAT1,IMAGE_BALL_TIMEBUG_SPLAT2 | IMAGE_BALL_FISH_SPLAT1,IMAGE_BALL_FISH_SPLAT2 | IMAGE_BALL_PILOT_SPLAT1,IMAGE_BALL_PILOT_SPLAT2
                reference_attribute( 'image', reference_family = 'image', reference_world = WORLD_BALL, is_list=True)
            ] ),
            describe_element( 'strand', min_occurrence=0, max_occurrence=1, attributes = [
                real_attribute( 'dampfac', min_value=0.00001, mandatory = True, init = '0.9'),  # [0.002-1.9] Median:0.9 Samples: 0.9 | 0.002 | 1.9 | 0.1 | 0.2
#                unknown_attribute( 'image', mandatory = True, init = 'IMAGE_BALL_FUSE_STRAND'),  # Samples: IMAGE_BALL_FUSE_STRAND | IMAGE_BALL_UTILGOOGLOBBERMOM_STRAND | IMAGE_BALL_DRAINEDISH_STRAND | IMAGE_BALL_FISH_STRING3 | IMAGE_BALL_WATER_STRAND
                reference_attribute( 'image', reference_family = 'image', reference_world = WORLD_BALL),
#                unknown_attribute( 'inactiveimage', mandatory = True, init = 'IMAGE_BALL_GENERIC_ARM_INACTIVE'),  # Samples: IMAGE_BALL_GENERIC_ARM_INACTIVE | IMAGE_BALL_BALLOON_INACTIVE | IMAGE_BALL_GOOPRODUCT_STRAND | IMAGE_BALL_UTILNOATTACHUNWALKABLE_STRAND | IMAGE_BALL_BOMBSHAFT_STRAND
                reference_attribute( 'inactiveimage', reference_family = 'image', reference_world = WORLD_BALL),
                int_attribute( 'maxforce', min_value=0.00001, mandatory = True, init = '600'),  # [200-2000] Median:600 Samples: 600 | 800 | 1000 | 200 | 300
                int_attribute( 'maxlen2', min_value=0.00001, mandatory = True, init = '140'),  # [50-380] Median:140 Samples: 140 | 50 | 380 | 300 | 120
                int_attribute( 'minlen', min_value=0.00001, mandatory = True, init = '100'),  # [10-150] Median:100 Samples: 100 | 130 | 10 | 150 | 40
                int_attribute( 'springconstmax', min_value=0.00001, mandatory = True, init = '9'),  # [1-10] Median:9 Samples: 9 | 10 | 1 | 2 | 6
                int_attribute( 'springconstmin', min_value=0.00001, mandatory = True, init = '9'),  # [1-10] Median:9 Samples: 9 | 10 | 2 | 1 | 6
                enum_attribute( 'type', ('spring','rope'), mandatory = True, init = 'spring'),  # Samples: spring | rope
                int_attribute( 'maxlen1', min_value=0.00001, init = '200'),  # [80-500] Median:200 Samples: 200 | 300 | 80 | 180 | 130
                bool_attribute( 'walkable', init = 'false'),
                int_attribute( 'shrinklen', min_value=0.00001, init = '90'),  # [80-160] Median:90 Samples: 90 | 100 | 120 | 130 | 160
                int_attribute( 'burnspeed', min_value=0.00001, init = '3'),  # [2-3] Median:3 Samples: 3 | 2
#                unknown_attribute( 'burntimage', init = 'IMAGE_BALL_BOMBMINI_STRAND_BURNT'),  # Samples: IMAGE_BALL_BOMBMINI_STRAND_BURNT | IMAGE_BALL_PIXEL_ARM | IMAGE_BALL_BOMBSTICKY_STRAND_BURNT | IMAGE_BALL_PIXELPRODUCT_ARM | IMAGE_BALL_FUSE_STRAND_BURNT
                reference_attribute( 'burntimage', reference_family = 'image', reference_world = WORLD_BALL),
#                unknown_attribute( 'fireparticles', init = 'fireArmBurn'),  # Samples: fireArmBurn | ish_smallfire | poisonArmBurn
                reference_attribute( 'fireparticles', reference_family = 'effect', reference_world = WORLD_GLOBAL ),
                int_attribute( 'ignitedelay', min_value=0, init = '0'),  # [0-0] Median:0 Samples: 0
                int_attribute( 'thickness', min_value=0.00001, init = '40'),  # [10-40] Median:20 Samples: 40 | 10 | 20
                bool_attribute( 'rope', init = 'true'),
                bool_attribute( 'geom', init = 'false'),
            ] )
        ] )
    ] )

TREE_ISLAND.add_elements( [
        describe_element( 'island', exact_occurrence = 1, attributes = [
            reference_attribute( 'icon', reference_family = 'image', reference_world = WORLD_GLOBAL, mandatory = True ),
            string_attribute( 'map', mandatory = True, init = 'island5'),
            string_attribute( 'name', display_id = True, mandatory = True, 
                init = 'Cog in the Machine'),  # Samples: Cog in the Machine | The Goo Filled Hills | Information Superhighay | Little Miss World of Goo | End of the World
        ], elements = [
            describe_element( 'level', min_occurrence=1, attributes = [
                identifier_attribute( 'id', display_id = True, mandatory = True, 
                    reference_family = 'level', reference_world = WORLD_ISLAND ),
                reference_attribute( 'name', reference_family = 'text', reference_world = WORLD_ISLAND, init = '', mandatory = True ),
                reference_attribute( 'text', reference_family = 'text', reference_world = WORLD_ISLAND, init = '', mandatory = True ),
                reference_attribute( 'depends', reference_family = 'level', reference_world = WORLD_ISLAND),
                string_attribute( 'ocd', init = 'balls,10'),
                string_attribute( 'cutscene'),  # Samples: levelFadeOut,Chapter5End,gooTransition_out | x,whistleUnlock,gooTransition_out | levelFadeOut,Chapter4End,gooTransition_out | x,Chapter2Mid,gooTransition_out | levelFadeOut,Chapter1End,gooTransition_out
                string_attribute( 'oncomplete'),  # Samples: expandchapter4 | unlockwogcorp | unlockwhistle
                bool_attribute( 'skipeolsequence', init = 'true')
            ] )
        ] )
    ] )


LEVEL_GAME_TEMPLATE = """\
<level ballsrequired="1" letterboxed="false" visualdebug="false" autobounds="false" textcolor="255,255,255" timebugprobability="0" strandgeom="false" allowskip="true" >

	<!-- Camera -->
	<camera aspect="normal" endpos="0,0" endzoom="1">
		<poi pos="0,0" traveltime="0" pause="0" zoom="1" />
	</camera>
	<camera aspect="widescreen" endpos="0,0" endzoom="1">
		<poi pos="0,0" traveltime="0" pause="0" zoom="1" />
	</camera>

	<!-- Level Exit -->
	<levelexit id="theExit" pos="0,0" radius="75" filter="" >
	</levelexit>

</level>
"""

LEVEL_SCENE_TEMPLATE = """\
<scene minx="-500" miny="0" maxx="500" maxy="1000" backgroundcolor="0,0,0" >
	<linearforcefield type="gravity" force="0,-10" dampeningfactor="0" antigrav="true"  />

	<line id="right" tag="detaching" anchor="500,300" normal="-1,0" />
	<line id="left" tag="detaching" anchor="-500,300" normal="1,0" />
	<line id="ground" anchor="0,20" normal="0,1" />
</scene>"""

LEVEL_RESOURCE_TEMPLATE = """\
<ResourceManifest>
	<Resources id="scene_NewTemplate" >
		<SetDefaults path="./" idprefix="" />
	</Resources>
</ResourceManifest>
"""
LEVEL_ADDIN_TEMPLATE ="""\
<addin spec-version="1.1">
  <id></id>
  <name>LevelName</name>
  <type>level</type>
  <version>0.1</version>
  <description></description>
  <author></author>
  <levels>
    <level>
      <dir>LevelName</dir>
      <name text="LevelName"/>
      <subtitle text=""/>
      <ocd></ocd>
    </level>
  </levels>
</addin>
"""

LEVEL_TEXT_TEMPLATE ="""\
<strings spec-version="1.1"/>
"""

LEVEL_DEPENDANCY_TEMPLATE ="""\
<dependancy/>
"""

BALL_BALL_TEMPLATE = """\
<ball name="new_ball_name"
      shape="circle,30"
      mass="30"
  >
</ball>
"""

BALL_RESOURCE_TEMPLATE = """\
<ResourceManifest>
	<Resources id="ball_NewTemplate" >
		<SetDefaults path="./" idprefix="" />
	</Resources>
</ResourceManifest>
"""

BALL_ADDIN_TEMPLATE ="""\
<addin spec-version="1.1">
  <id></id>
  <name>BallName</name>
  <type>mod</type>
  <version>0.1</version>
  <description></description>
  <author></author>
</addin>
"""

BALL_DEPENDANCY_TEMPLATE ="""\
<dependancy/>
"""


XSL_ADD_TEMPLATE="""\
<xsl:transform version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <!-- Copy everything not matched by another rule -->
  <xsl:template match="* | comment()">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

  <!-- Append -->
  <xsl:template match="%(path)s">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates/>
      %(xml_data)s
    </xsl:copy>
  </xsl:template>
</xsl:transform>"""

#@DaB - New Resource Tree - Only has Fonts in
# to eliminate all the useless images and sounds appearing in the completer boxes.
GLOBAL_FONT_RESOURCES = """\
<ResourceManifest>
<Resources id="init">
<font  id="FONT_OUTLINE_18" path="res/fonts/TwCenMTCondensedExtraBold18"/>
<font  id="FONT_LOADING"                 path="res/fonts/TwCenMTCondensedExtraBold18"/>
<font  id="FONT_OUTLINE_26"              path="res/fonts/TwCenMTCondensedExtraBold26"/>
<font  id="FONT_SIGNPOST"                path="res/fonts/TwCenMTCondensedExtraBold36"/>
<font  id="FONT_BIGWHITE_52"             path="res/fonts/TwCenMTCondensedExtraBold52"/>
<font  id="FONT_INGAME36"                path="res/fonts/wogSmall"/>
<font  id="FONT_TITLE"                   path="res/fonts/wogBig"/>
<font  id="FONT_STAT"                    path="res/fonts/wog150numbers"/>
<font  id="FONT_CONSOLE"                    path="res/fonts/console"/>
</Resources>
</ResourceManifest>
"""

if __name__ == "__main__":
    print_world_meta( WORLD_GLOBAL )

