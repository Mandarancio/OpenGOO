# The level editor GUI.
# The following workflow is expected:
# 1) User load a level
# 2) main windows display the scene layout
#    right-side-top-dock display:
#   - level scene tree
#   - level tree (a list in fact)
#   - level resources tree (a list in fact)
# 3) user select an element in one of the tree, related properties are displayed in
#    right-side-down-dock property list
# 4) user edit properties in property list
#
# Later on, provides property edition via scene layout display
# Add toolbar to create new element
#
# In memory, we keep track of two things:
# - updated level
# - specific text/fx resources
from __future__ import with_statement  # Makes "With" work in Python 2.5
import xml.etree.ElementTree
import sys
import os
import os.path
import glob
import subprocess
import louie
import wogfile
import metaworld
import metawog
import metaworldui
import metatreeui
import metaelementui
import levelview
from shutil import copy2
from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import Qt
import qthelper
import editleveldialog
import newleveldialog_ui
import wogeditor_rc #IGNORE:W0611
import shutil
import zipfile
import errors
from datetime import datetime
LOG_TO_FILE = False
APP_NAME_UPPER = 'WOOGLE'
APP_NAME_LOWER = 'woogle'
APP_NAME_PROPER = 'WooGLE'
GAME_NAME = 'OpenGOO'
STR_DIR_STUB='levels'
CURRENT_VERSION = ""#"v0.77 Final"
CREATED_BY = '<!-- Created by ' + APP_NAME_PROPER + ' ' + CURRENT_VERSION + ' -->\n'
ISSUE_LEVEL_NONE = 0
ISSUE_LEVEL_ADVICE = 1
ISSUE_LEVEL_WARNING = 2
ISSUE_LEVEL_CRITICAL =4
PLATFORM_WIN=0
PLATFORM_LINUX=1
PLATFORM_MAC=2
MAXRECENTFILES=4
#print "platform=",sys.platform

if sys.platform=='win32' or sys.platform=='cygwin':
    ON_PLATFORM = PLATFORM_WIN
elif sys.platform=='darwin':
    ON_PLATFORM = PLATFORM_MAC
else:
    ON_PLATFORM = PLATFORM_LINUX
metaworld.ON_PLATFORM = ON_PLATFORM

def app_path():
    if hasattr(sys, 'frozen'):
        return os.path.dirname(sys.executable)
    #elif __file__:
    #   return os.path.dirname(__file__)
    else:
        return os.path.dirname(sys._getframe(1).f_code.co_filename)

def _getRealFilename(path):
    # Only required on Windows
    # will return the filename in the AcTuaL CaSe it is stored on the drive
    # ensure "clean" split
    path_bits = path.replace('\\','/').replace('//','/').split('/')
    real_path_bits=[]
    currentpath=path_bits.pop(0)+"\\"
    for path_bit in path_bits:
        insensitive_match=''
        sensitive_match=''
        for entry in os.listdir(currentpath):
            if entry == path_bit:
                # case senstive match - we can bail
                sensitive_match=entry
                break
            elif entry.lower()== path_bit.lower():
                # case insenstive match
                insensitive_match=entry
                break
        else:
            print "File not Found!", path
            return ''
        if sensitive_match!='':
           currentpath = os.path.join(currentpath,entry)
        elif insensitive_match!='':
            currentpath = os.path.join(currentpath,insensitive_match)
    return currentpath


#@DaB New actions for Add item toolbar
def _appendChildTag( parent_element, new_element_meta , mandatory_attributes, keepid=False ):
                    """Adds the specified child tag to the specified element and update the tree view."""
                    assert parent_element is not None
                    # build the list of attributes with their initial values.
                    for attribute_meta in new_element_meta.attributes:
                        if attribute_meta.mandatory:
                          if attribute_meta.type == metaworld.IDENTIFIER_TYPE:
                              try:
                                given_id = mandatory_attributes[attribute_meta.name]
                              except KeyError:
                                given_id = None
                              if given_id is None or not keepid:
                                  init_value = parent_element.world.generate_unique_identifier(
                                                      attribute_meta )
                                  mandatory_attributes[attribute_meta.name] = init_value
                          else:
                            init_value = attribute_meta.init
                            if init_value is not None:
                              if attribute_meta.name not in mandatory_attributes:
                                mandatory_attributes[attribute_meta.name] = init_value

                        if (attribute_meta.default is not None and not attribute_meta.mandatory):
                            if attribute_meta.name not in mandatory_attributes:
                                init_value = attribute_meta.default
                                mandatory_attributes[attribute_meta.name] = init_value
                    # Notes: when the element is added, the ElementAdded signal will cause the
                    # corresponding item to be inserted into the tree.
                    child_element = parent_element.make_child( new_element_meta,
                                                               mandatory_attributes )
                    # Select new item in tree view
                    if not keepid:
                        child_element.world.set_selection( child_element )
                    return child_element

class AddItemFactory(object):
                def __init__( self, window, parent,itemtag,attrib):
                    self.window = window
                    self.itemtag = itemtag
                    self.attrib = attrib
                    self.parent = parent

                def __call__( self ):
                    assert self.parent is not None
                    model = self.window.getCurrentModel()
                    if model:
                        window = self.window.mdiArea.activeSubWindow()
                        if window:
                            cview = window.widget()
                            cp = cview.mapToScene(cview.width()*0.5,cview.height()*0.5)
                            offsetx,offsety = 0,0
                            if self.parent=='level':
                               root = model.level_root
                            elif self.parent=='scene':
                                root = model.scene_root
                            elif self.parent=='resource':
                                root = model.resource_root
                            elif self.parent=='addin':
                                root = model.addin_root
                            elif self.parent=='text':
                                root = model.text_root
                            elif self.parent=='compositegeom':
                                thisworld = cview.world
                                selected_elements=thisworld.selected_elements
                                cgparent = None
                                for element in selected_elements:
                                    if element.tag== 'compositegeom':
                                        cgparent = element
                                        break
                                    elif element.tag=='rectangle' or element.tag=='circle':
                                        # check to see if they are part of a cg
                                        pelement = element.parent
                                        if pelement is not None:
                                            if pelement.tag=='compositegeom':
                                                cgparent = pelement
                                                break
                                if cgparent is None:
                                    QtGui.QMessageBox.warning(window,'No composite geometry parent','You must select a CompositeGeom item to add this child to')
                                    return
                                root = cgparent
                                offsetx,offsety = root.get_native('center')
                            else:
                                print "Unknown Parent in AddItemFactory", self.parent
                            rootmbt = root.meta.find_immediate_child_by_tag(self.itemtag)
                            if rootmbt is not None:
                                for attribute_meta in rootmbt.attributes:
                                    if attribute_meta.type == metaworld.XY_TYPE:
                                        self.attrib[attribute_meta.name]= str(cp.x()-offsetx)+","+str(-(cp.y()+offsety))
                                        break
                                _appendChildTag(root,rootmbt,self.attrib)

def tr( context, message ):
    return QtCore.QCoreApplication.translate( context, message )

def find_element_in_tree( root_element, element ):
    """Searchs the specified element in the root_element children and returns all its parent, and its index in its immediate parent.
       Returns None if the element is not found, otherwise returns a tuple ([parent_elements], child_index)
       root_element, element: must provides the interface xml.etree.ElementTree.
    """
    for index, child_element in enumerate(root_element):
        if child_element is element:
            return ([root_element], index)
        found = find_element_in_tree( child_element, element )
        if found is not None:
            found_parents, found_index = found
            found_parents.insert( 0, root_element )
            return found_parents, found_index
    return None

def flattened_element_children( element ):
    """Returns a list of all the element children, including its grand-children..."""
    children = []
    for child_element in element:
        children.append( child_element )
        children.extend( flattened_element_children( child_element ) )
    return children

class GameModelException(Exception):
    pass

class PixmapCache(object):
    """A global pixmap cache the cache the pixmap associated to each element.
       Maintains the cache up to date by listening for element events.
    """
    def __init__(self, wog_dir, universe):
        self._wog_dir = wog_dir
        self._pixmaps_by_path = {}
        self._filedate_by_path = {}
        self.__event_synthetizer = metaworld.ElementEventsSynthetizer(universe,
            None,
            self._on_element_updated,
            self._on_element_about_to_be_removed )

    def get_pixmap(self, image_element):
        """Returns a pixmap corresponding to the image_element.
           The pixmap is loaded if not present in the cache.
           None is returned on failure to load the pixmap.
        """
        assert image_element.tag == 'Image'
        image_path = image_element.get('path','')
        pixmap = self._pixmaps_by_path.get( image_path )
        if pixmap:
            return pixmap
        path = os.path.join( self._wog_dir, image_path + '.png' )
        if not os.path.isfile(path):
            print 'Warning: invalid image path for "%(id)s": "%(path)s"' % \
                image_element.attributes
        else:
            return self._addToCache(path,image_element.attributes)
        return None

    def _addToCache(self,path, image_attrib):
            pixmap = QtGui.QPixmap()
            image_path = image_attrib['path']
            if pixmap.load( path ):
                #print "plain loaded:",path
                self._pixmaps_by_path[image_path] = pixmap
                self._filedate_by_path[image_path] = os.path.getmtime(path)
                return pixmap
            else:
                data = file( path, 'rb' ).read()
                if pixmap.loadFromData( data ):
                    #print "loadFromData:",path
                    self._pixmaps_by_path[image_path] = pixmap
                    self._filedate_by_path[image_path] = os.path.getmtime(path)
                    return pixmap
                else:
                   if image_path in self._pixmaps_by_path.keys():
                       del self._pixmaps_by_path[image_path]
                       del self._filedate_by_path[image_path]
                   print 'Warning: failed to load image "%(id)s": "%(path)s"' % \
                        image_attrib
            return None

    def refresh(self):
        # check each file in the cache...
        # if it's out of date then reload
        for image_path,filedate in self._filedate_by_path.items():
            path = os.path.normpath(os.path.join( self._wog_dir, image_path + '.png' ))
            if not os.path.isfile(path):
                if image_path in self._pixmaps_by_path.keys():
                    del self._pixmaps_by_path[image_path]
                    del self._filedate_by_path[image_path]
                print 'Warning: File is missing %s' % path
            elif os.path.getmtime(path)>filedate:
                # refresh
                self._addToCache(path,{'id':path,'path':image_path})


    def _on_element_about_to_be_removed(self, element, index_in_parent): #IGNORE:W0
        if element.tag == 'Image':
          if element.get('path','') in self._pixmaps_by_element:
            del self._pixmaps_by_element[element.get('path','')]

    def _on_element_updated(self, element, name, new_value, old_value): #IGNORE:W0613
        if element.tag == 'Image':
          if old_value in self._pixmaps_by_element:
            del self._pixmaps_by_element[old_value]


class GameModel(QtCore.QObject):
    def __init__( self, wog_path,window):
        """Loads FX, material, text and global resources.
           Loads Balls
           Loads Levels

           The following signals are provided:
           QtCore.SIGNAL('selectedObjectChanged(PyQt_PyObject,PyQt_PyObject,PyQt_PyObject)')
        """
        QtCore.QObject.__init__( self )
        self._window = window
        self._wog_path = wog_path

        if ON_PLATFORM==PLATFORM_MAC:
            # on Mac
            # wogdir is Contents\esources\game\
            self._wog_dir = os.path.join(self._wog_path,u'Contents',u'Resources',u'game')
        else:
            self._wog_dir = wog_path

        metaworld.WOG_PATH = self._wog_dir
        self._properties_dir = os.path.join( self._wog_dir, u'properties' )
        self._res_dir = os.path.join( self._wog_dir, u'res' )

        # On MAC
        # enumerate all files in res folder
        # convert all .png.binltl to .png
        if ON_PLATFORM==PLATFORM_MAC:
            window.statusBar().showMessage(self.tr("Checking graphics files..."))
            skipped,processed,found=0,0,0
            lresdir = len(self._res_dir)
            toconvert=[]
            for (path, dirs, files) in os.walk(self._res_dir):
             for name in files:
              if name.endswith('.png.binltl'):
                found+=1
                output_path=os.path.join(path, name[:-11]) + '.png'
                if not os.path.isfile( output_path ):
                    toconvert.append([os.path.join(path, name),output_path,os.path.join(path, name)[lresdir:]])
                    processed+=1
                else:
                    skipped+=1
            #print "png.binltl found",found,'processed',processed,'skipped',skipped
            if processed>0:
                progress=QtGui.QProgressDialog("", QtCore.QString(), 0, processed, window);
                progress.setWindowTitle(window.tr("Converting PNG.BINLTL files to PNG..."));
                progress.setWindowModality(Qt.WindowModal);
                progress.setMinimumWidth(300)
                progress.forceShow()
                for filepair in toconvert:
                    if progress.wasCanceled():
                        break
                    progress.setValue(progress.value()+1);
                    progress.setLabelText(filepair[2])
                    width,height=wogfile.pngbinltl2png(filepair[0],filepair[1])
                progress.setValue(progress.value()+1);

        window.statusBar().showMessage(self.tr("Game Model : Initializing"))
        self._universe = metaworld.Universe()
        self.global_world = self._universe.make_world( metawog.WORLD_GLOBAL, 'game' )
        window.statusBar().showMessage(self.tr("Game Model : Loading Properties XMLs"))

        self._loadTree( self.global_world, metawog.TREE_GLOBAL_FX,
                                             self._properties_dir, 'fx.xml' )

        self._loadTree( self.global_world, metawog.TREE_GLOBAL_MATERIALS,
                                               self._properties_dir, 'materials.xml' )

        self._loadUnPackedTree( self.global_world, metawog.TREE_GLOBAL_FILES,
                                               app_path(), 'files.xml.xml' ,'')

	self._loadTree( self.global_world, metawog.TREE_GLOBAL_RESOURCE,
                                               self._properties_dir, 'resources.xml' )

        self._readonly_resources = set()    # resources in resources.xml that have expanded defaults idprefix & path
        self._loadTree( self.global_world, metawog.TREE_GLOBAL_TEXT,
                                           self._properties_dir, 'text.xml' )

        self._levels = self._loadDirList( os.path.join( self._res_dir, 'levels' ),
                                          filename_filter = '%s.scene.xml' )

        anim_dir = os.path.join( self._res_dir, 'anim' )

        if not os.path.exists(anim_dir):
            os.mkdir(anim_dir)

        anims = self._loadFileList( anim_dir, filename_filter = '.anim.binltl' )
        metawog.ANIMATIONS_GLOBAL.extend(anim[:len(anim)-12] for anim in anims )

        self.models_by_name = {}
        self.__is_dirty = False
        self._initializeGlobalReferences()

        try:
            window.statusBar().showMessage(self.tr("Game Model : Loading Goo Ball Info"))
            self._loadBalls()
        except GameModelException,e:
            QtGui.QMessageBox.warning(window,self.tr("Errors found Loading Balls ("+APP_NAME_PROPER+" "+CURRENT_VERSION+")"), unicode(e))

        self.modified_worlds_to_check = set()
        louie.connect( self._onElementAdded, metaworld.ElementAdded )
        louie.connect( self._onElementAboutToBeRemoved, metaworld.ElementAboutToBeRemoved )
        louie.connect( self._onElementUpdated, metaworld.AttributeUpdated )
        self.pixmap_cache = PixmapCache( self._wog_dir, self._universe )
        window.statusBar().showMessage(self.tr("Game Model : Complete"))

    @property
    def _effects_tree( self ):
        return self.global_world.find_tree( metawog.TREE_GLOBAL_FX )

    @property
    def _materials_tree( self ):
        return self.global_world.find_tree( metawog.TREE_GLOBAL_MATERIALS )

    @property
    def _resources_tree( self ):
        return self.global_world.find_tree( metawog.TREE_GLOBAL_RESOURCE )

    @property
    def _files_tree( self ):
        return self.global_world.find_tree( metawog.TREE_GLOBAL_FILES )


    @property
    def _texts_tree( self ):
        return self.global_world.find_tree( metawog.TREE_GLOBAL_TEXT )


    @property
    def is_dirty(self):
        worlds = self.modified_worlds_to_check
        self.modified_worlds_to_check = set()
        for world in worlds:
            if world:
                self.__is_dirty = self.__is_dirty or world.is_dirty
        return self.__is_dirty

    def getResourcePath( self, game_dir_relative_path ):
        return os.path.join( self._wog_dir, game_dir_relative_path )

    def _loadTree( self, world, meta_tree, directory, file_name ):
        path = os.path.join( directory, file_name )
        if not os.path.isfile( path ):
            raise GameModelException( tr( 'LoadData',
                'File "%1" does not exist. You likely provided an incorrect OpenGOO directory.' ).arg( path ) )
        xml_data = file( path, 'rb' ).read()
        try:
            new_tree =  world.make_tree_from_xml( meta_tree, xml_data )
        except IOError,e:
            raise GameModelException(unicode(e)+u' in file '+file_name )
        new_tree.setFilename(path)
        return new_tree

    def _loadUnPackedTree( self, world, meta_tree, directory, file_name, template ):
        input_path = os.path.join( directory, file_name )
        if not os.path.isfile( input_path ):
            xml_data = template
        else:
            xml_data = file( input_path, 'rb' ).read()
        try:
            new_tree =  world.make_tree_from_xml( meta_tree, xml_data )
        except IOError,e:
            raise GameModelException(unicode(e)+u' in file '+file_name )
        new_tree.setFilename(input_path)
        return new_tree

    def _loadUnPackedXML( self, directory, file_name, template ):
        input_path = os.path.join( directory, file_name )
        if not os.path.isfile( input_path ):
            return template
        else:
            return file( input_path, 'rb' ).read()

    def _savePackedXML( self, directory, file_name, xml_data ):
        if not os.path.isdir( directory ):
            os.makedirs( directory )
        path = os.path.join( directory, file_name )
        xml_data = xml_data.replace('><','>\n<')
        file( path, 'wb' ).write( xml_data )

    def _saveUnPackedData(self,directory,file_name,tree):
        if not os.path.isdir( directory ):
            os.makedirs( directory )
        output_path = os.path.join( directory, file_name )
        xml_data = tree.to_xml()
        xml_data = CREATED_BY + xml_data.replace('><','>\n<')
        file( output_path, 'wb' ).write( xml_data )
        tree.setFilename(output_path)

    def _savePackedData( self, directory, file_name, tree ):
        if not os.path.isdir( directory ):
            os.makedirs( directory )
        path = os.path.join( directory, file_name )
        xml_data = tree.to_xml()
        xml_data = xml_data.replace('><','>\n<')
        file( path, 'wb' ).write( xml_data )
        tree.setFilename(path)

    def _loadDirList( self, directory, filename_filter ):
        if not os.path.isdir( directory ):
            raise GameModelException( tr('LoadLevelList',
                'Directory "%1" does not exist. You likely provided an incorrect OpenGOO directory.' ).arg( directory ) )
        def is_valid_dir( entry ):
            """Accepts the directory only if it contains a specified file."""
            dir_path = os.path.join( directory, entry )
            if os.path.isdir( dir_path ):
                try:
                    filter_file_path = filename_filter % entry
                except TypeError:
                    filter_file_path = filename_filter
                if os.path.isfile( os.path.join( dir_path, filter_file_path ) ):
                    return True
            return False
        dirs = [ entry for entry in os.listdir( directory ) if is_valid_dir( entry ) ]
        dirs.sort(key=unicode.lower)
        return dirs

    def _loadFileList( self, directory, filename_filter ):
        if not os.path.isdir( directory ):
            raise GameModelException( tr('LoadFileList',
                'Directory "%1" does not exist. You likely provided an incorrect OpenGOO directory.' ).arg( directory ) )
        def is_valid_file( entry ):
            """Accepts the directory only if it contains a specified file."""
            if entry.endswith(filename_filter):
                file_path = os.path.join( directory, entry )
                return os.path.isfile( file_path )
            return False
        files = [ entry for entry in os.listdir( directory ) if is_valid_file( entry ) ]
        files.sort(key=unicode.lower)
        return files

    def _loadBalls( self ):
        """Loads all ball models and initialize related identifiers/references."""
        ball_dir = os.path.join( self._res_dir, 'balls' )

        if not os.path.exists(ball_dir):
            os.mkdir(ball_dir)

        ball_names = self._loadDirList( ball_dir, filename_filter = 'balls.xml' )
        ball_name_map = {}
        for ball_name in ball_names:
            #WoG Editor was having trouble when the Ball Folder name
            #wasn't a 100% case-sensitive match to <ball name="..."> in the balls.xml
            #but WOG was fine with it, even on Linux.

            #read in the ball_tree (balls.xml) first...
            #get name attribute out of it, and use that to create the Ball World
            xml_data = file( os.path.join(ball_dir, ball_name, 'balls.xml'), 'rb' ).read()
            try:
                ball_tree = self._universe.make_unattached_tree_from_xml( metawog.TREE_BALL_MAIN, xml_data )
            except IOError,e:
                raise GameModelException(unicode(e)+u' in '+ball_name+u'/balls.xml')

            real_ball_name = ball_tree.root.get('name')
            if real_ball_name not in self.global_world.list_world_keys(metawog.WORLD_BALL):
                ball_world = self.global_world.make_world( metawog.WORLD_BALL, real_ball_name, BallModel, self )
                #then append the ball_tree to the ball world... rather than loading the file again.
                ball_tree.setFilename(os.path.join(ball_dir, ball_name, 'balls.xml'))
                ball_world.add_tree([ball_tree])
                #ball_tree = self._loadTree( ball_world, metawog.TREE_BALL_MAIN,
                     #                       os.path.join(ball_dir, ball_name), 'balls.xml.bin' )
                try:
                    ball_res_tree = self._loadTree( ball_world, metawog.TREE_BALL_RESOURCE,
                            os.path.join(ball_dir, ball_name), 'resources.xml' )
                    self._processSetDefaults(ball_res_tree)
                except IOError,e:
                    raise GameModelException(unicode(e)+u' in '+ball_name+u'/resources.xml')
        #        print ball_name,real_ball_name,ball_res_tree.root.find('Resources').get('id')
                assert ball_tree.world == ball_world
                assert ball_tree.root.world == ball_world, ball_tree.root.world

                ball_name_map[real_ball_name]=ball_name

                # look at ball_tree
                # cache the shape, and suckable
                metawog.BALLS_SHAPES[real_ball_name] = str(ball_tree.root.get('shape')).split(',')
                if ball_tree.root.get_native('suckable',True):
                    metawog.BALLS_SUCKABLE.add(real_ball_name)
                if ball_tree.root.get_native('static',False):
                    metawog.BALLS_STATIC.add(real_ball_name)

                #cache the body image_element and scale factor
                for part in ball_tree.root.findall('part'):
                    if part.get('name').lower()=='body':
                        ball_image = part.get('image').split(',')[0] #can be comma-sep list, use first!
                        image_element = ball_world.resolve_reference( metawog.WORLD_BALL, 'image', ball_image )
                        scale = part.get_native('scale',1)
                        metawog.BALLS_IMAGES[real_ball_name] = [image_element,scale]

                if real_ball_name not in metawog.BALLS_IMAGES.keys():
                    metawog.BALLS_INVISIBLE.add(real_ball_name)

                # flag if this ball has no strand entry
                if ball_tree.root.find('strand') is None:
                    metawog.BALLS_MUST_BE_GB1.append(real_ball_name)

            else:
                 QtGui.QMessageBox.warning(self._window,self.tr("Errors found Loading Balls ("+APP_NAME_PROPER+" "+CURRENT_VERSION+")"),
                                            'Duplicate ball name <b>'+real_ball_name+'</b> found in folder <b>' + ball_name_map[real_ball_name] + '</b>  and  <b>' + ball_name + '</b><br>'+
                                            'Only the first one has been loaded!')

    def _processSetDefaults(self,resource_tree):
        #Unwraps the SetDefaults "processing instruction"
        #updates all paths and ids to full
        resource_element = resource_tree.root.find('Resources')
        idprefix = ''
        pathprefix=''
        for element in resource_element:
            if element.tag =='SetDefaults':
                idprefix = element.get('idprefix','')
                pathprefix = element.get('path').strip().replace("\\","/")
                if not pathprefix.endswith('/'):
                      pathprefix += '/'
                pathprefix = pathprefix.replace("./","")

                element.set('idprefix',"")
                element.set('path',"./")
            else:
                element.set('path',pathprefix+element.get('path').replace('\\','/'))
                element.set('id',idprefix+element.get('id'))

    def _initializeGlobalReferences( self ):
        """Initialize global effects, materials, resources and texts references."""
        self._expandResourceDefaultsIdPrefixAndPath()

    def _expandResourceDefaultsIdPrefixAndPath( self ):
        """Expands the default idprefix and path that are used as short-cut in the XML file."""
        # Notes: there is an invalid global resource:
        # IMAGE_GLOBAL_ISLAND_6_ICON res/images/islandicon_6
        resource_manifest = self._resources_tree.root
        default_idprefix = ''
        default_path = ''
        for resources in resource_manifest:
            for element in resources:
                if element.tag == 'SetDefaults':
                    default_path = element.get('path','').strip()
                    if not default_path.endswith('/'):
                       default_path += '/'
                    default_path = default_path.replace("./","")
                    default_idprefix = element.get('idprefix','')
                elif element.tag in ('Image', 'Sound', 'font'):
                    new_id = default_idprefix + element.get('id')
                    new_path = default_path + element.get('path')
##                    if element.tag == 'Sound':
##                        full_path = os.path.join( self._wog_dir, new_path + '.ogg' )
##                    else:
##                        full_path = os.path.join( self._wog_dir, new_path + '.png' )
##                    if not os.path.isfile( full_path ):
##                        print 'Invalid resource:', element.get('id'), element.get('path'), new_id, full_path
                    element.set( 'id', new_id )
                    element.set( 'path', new_path )
                self._readonly_resources.add( element )


    @property
    def names( self ):
        return self._levels

    def getModel( self, name ):
        if name not in self.models_by_name:
            dir = os.path.join( self._res_dir, STR_DIR_STUB, name )

            world = self.global_world.make_world( metawog.WORLD_LEVEL,
                                                        name,
                                                        LevelWorld,
                                                        self )
            #@DaB Prepare addin template
            addin_template = metawog.LEVEL_ADDIN_TEMPLATE.replace("LevelName",name)
            self._loadUnPackedTree (world, metawog.TREE_LEVEL_ADDIN,
                            dir, name + '.addin.xml', addin_template)

            self._loadUnPackedTree (world, metawog.TREE_LEVEL_TEXT,
                            dir, name + '.text.xml', metawog.LEVEL_TEXT_TEMPLATE)

            self._loadTree( world, metawog.TREE_LEVEL_GAME,
                            dir, name + '.level.xml' )
            self._loadTree( world, metawog.TREE_LEVEL_SCENE,
                            dir, name + '.scene.xml' )
            self._loadTree( world, metawog.TREE_LEVEL_RESOURCE,
                            dir, name + '.resrc.xml' )

            self._processSetDefaults(world.find_tree(metawog.TREE_LEVEL_RESOURCE))

#            #clean up resource paths for Linux
# *** Done in _processSetDefaults

#            for resource in world.resource_root.findall('.//Image'):
#                res_path = resource.get('path')
#                clean_res_path = res_path.replace('\\','/')
#                if res_path != clean_res_path:
#                    print "Normalizing Path:",res_path,"->",clean_res_path
#                    resource.set( 'path', clean_res_path )
#            for resource in world.resource_root.findall('.//Sound'):
#                res_path = resource.get('path')
#                clean_res_path = res_path.replace('\\','/')
#                if res_path != clean_res_path:
#                    print "Normalizing Path:",res_path,"->",clean_res_path
#                    resource.set( 'path', clean_res_path )

            #Find any Global Strings and Localize them
            root = world.text_root
            rootmbt = root.meta.find_immediate_child_by_tag('string')
            elements_with_text=[]
            for element_with_text in world.level_root.findall('.//signpost'):
                elements_with_text.append(element_with_text)
            for element_with_text in world.scene_root.findall('.//label'):
                elements_with_text.append(element_with_text)

            for element_with_text in elements_with_text:
                tid = element_with_text.get('text')
                if tid is not None:
                  if not world.is_valid_reference(metawog.WORLD_LEVEL, 'TEXT_LEVELNAME_STR',tid):
                    if  world.is_valid_reference( metawog.WORLD_GLOBAL, 'text',tid):
                        global_text_element = world.resolve_reference( metawog.WORLD_GLOBAL, 'text' ,tid)
                        local_attrib = global_text_element.attrib.copy()
                        new_text = _appendChildTag(root,rootmbt,local_attrib,keepid=True)
                        #print tid,"is Global Text ... Localized as ",new_text.get('id')
                        element_with_text.set('text',new_text.get('id'))
                    else:
                        print "Text resource ",tid,"not found locally or Globally."

            world._buildDependancyTree()
            #world.make_tree_from_xml( metawog.TREE_LEVEL_DEPENDANCY, metawog.LEVEL_DEPENDANCY_TEMPLATE )

            if world.isReadOnly:
               world.clean_dirty_tracker()
            world.clear_undo_queue()
            self.models_by_name[name] = world

        return self.models_by_name[name]

    def selectLevel( self, name ):
        """Activate the specified level and load it if required.
           Returns the activated LevelWorld.
        """
        model = self.getModel(name)
        assert model is not None
        louie.send( metaworldui.ActiveWorldChanged, self._universe, model )
        return model

    def _onElementAdded(self, element, index_in_parent): #IGNORE:W0613
        self.modified_worlds_to_check.add( element.world )

    def _onElementUpdated(self, element, attribute_name, new_value, old_value): #IGNORE:W0613
        self.modified_worlds_to_check.add( element.world )

    def _onElementAboutToBeRemoved(self, element, index_in_parent): #IGNORE:W0613
        self.modified_worlds_to_check.add( element.world )

    def hasModifiedReadOnly( self ):
        """Checks if the user has modified read-only """
        for model in self.models_by_name.itervalues():
            if model.is_dirty and model.isReadOnly:
                return True
        return False



    def playLevel( self, level_model ):
        """Starts WOG to test the specified level."""
        # Check for level_text
        # Update GLOBAL_TEXT_TREE  (self._texts_tree)
        local_text_ids = ['WOOGLE_TEST_NAME','WOOGLE_TEST_TEXT']
        for text_element in level_model.text_root.findall('string'):
            local_text_ids.append(text_element.get('id'))

        for text_element in self._texts_tree.root.findall('string'):
            if text_element.get('id') in local_text_ids:
                text_element.parent.remove(text_element)
#                    print "removing",text_element.get('id'),text_element,"from",text_element.parent

        # Because GLOBAL_TEXT and LOCAL_TEXT are different meta+types
        # we cannot just clone the local element and appned it to the global tree
        # we need to add a new child to the global_tree with the attributes of the local element
        root = self._texts_tree.root
        rootmbt = root.meta.find_immediate_child_by_tag('string')
        for text_element in level_model.text_root.findall('string'):
            #print "appending",text_element.get('id'),text_element
            local_attrib = text_element.attrib.copy()
            _appendChildTag(root,rootmbt,local_attrib,keepid=True)

        #if ON_PLATFORM==PLATFORM_MAC:
        #ADD WOOGLE_TEST_ENTRIES
        if ON_PLATFORM==PLATFORM_MAC:
            local_attrib={'id':'WOOGLE_TEST_NAME','text':"WooGLE : Save n Play|"+level_model.name}
            _appendChildTag(root,rootmbt,local_attrib,keepid=True)
            local_attrib={'id':'WOOGLE_TEST_TEXT','text':'(testing 1 2 3)'}
            _appendChildTag(root,rootmbt,local_attrib,keepid=True)
        # Save it
        self._savePackedData(self._properties_dir, 'text.xml', self._texts_tree )

        #On Mac
        if ON_PLATFORM==PLATFORM_MAC:
            # Also Add this level as Chapter 1 Button
            #print "ON MAC - Save and Play"
            if self.addLevelButton(level_model.name):
                #Then run the program file itself with no command-line parameters
                #print "launch ",os.path.join(self._wog_path,u'Contents',u'MacOS',u'World of Goo')
                pid = subprocess.Popen( os.path.join(self._wog_path,u'Contents',u'MacOS',u'World of Goo'), cwd = self._wog_dir ).pid
        else:
            #self.addLevelButton(level_model.name)
            #pid = subprocess.Popen( self._wog_path, cwd = self._wog_dir ).pid
            pid = subprocess.Popen( [os.path.join(self._wog_path, GAME_NAME)
                                    , '--level'
                                    , level_model.name]
                                    , cwd = self._wog_dir ).pid
            # Don't wait for process end...
            # @Todo ? Monitor process so that only one can be launched ???


    def addLevelButton(self,level_name):
        #print "addLevelButton"
            # To prevent "problems" we need to remove
            # a) Old / Last WOOGLE Test level entries
            # and
            # b) Entries for the Level to be tested, if it's been added by GooTool already.
            # then add our new entries
        button_x,button_y = -660,680
        label_x,label_y = button_x+30,button_y
        scenelayer_x,scenelayer_y=button_x,button_y+20
        #load res/islands/island1.xml.bin
        path = os.path.join( self._res_dir,u'islands',u'island1.xml' )
        if not os.path.isfile( path ):
            print "File not found:",path
            return false
        else:
        #    print "Doing",path
            xml_data = file( path, 'rb' ).read()
            tree = self._universe.make_unattached_tree_from_xml( metawog.TREE_ISLAND, xml_data )
            root = tree.root
            rootmbt = root.meta.find_immediate_child_by_tag('level')
            # seek WooGLE_Test element

            for level in root.findall('level'):
                if level.get('name')==APP_NAME_UPPER+'_TEST_NAME':
                    # remove if found
                    level.parent.remove(level)
                #elif level.get('id')==level_name:
                #    level.parent.remove(level)

            #add new WooGLE_Test element
            attrib={'id':level_name,'name':APP_NAME_UPPER+"_TEST_NAME",'text':APP_NAME_UPPER+"_TEST_TEXT"}
            _appendChildTag(root,rootmbt,attrib,keepid=True)

            #save file
            xml_data = tree.to_xml()
            xml_data = xml_data.replace('><','>\n<')
            file( path, 'wb' ).write( xml_data )
         #   print "Done",path

        #load res/levels/island1/island1.scene.bin
        path = os.path.join( self._res_dir,u'levels',u'island1',u'island1.scene.xml' )
        if not os.path.isfile( path ):
            print "File not found:",path
            return false
        else:
          #  print "Doing",path
            xml_data = file( path, 'rb' ).read()
            tree = self._universe.make_unattached_tree_from_xml( metawog.TREE_LEVEL_SCENE, xml_data )
            root = tree.root
            # seek WooGLE_Test Label element
          #  print "seeking label"
            for label in root.findall('label'):
                x,y = label.get_native('position')
                if abs(x-label_x)<1:
                 if abs(y-label_y)<1:
          #          print "Removing old label",label.get('id')
                    label.parent.remove(label)


            attrib={'id':'txt_'+level_name, 'text':APP_NAME_UPPER+"_TEST_NAME",
            'depth':"0.1", 'position':`label_x`+","+`label_y`,
            'align':"left", 'rotation':"6.337", 'scale':"0.7",
            'overlay':"false",'screenspace':"false", 'font':"FONT_INGAME36"}
            rootmbt = root.meta.find_immediate_child_by_tag('label')
            _appendChildTag(root,rootmbt,attrib,keepid=True)

           # print "seek scenelayer"
            for scenelayer in root.findall('SceneLayer'):
                x,y = scenelayer.get_native('center')
                if abs(x-scenelayer_x)<1:
                 if abs(y-scenelayer_y)<1:
           #         print "removing old scenelayer",scenelayer.get('id')
                    scenelayer.parent.remove(scenelayer)

           # print "create scenelayer",'ocd_'+level_name
            attrib={ 'id':'ocd_'+level_name, 'name':"OCD_flag1",
                    'depth':"-0.1", 'center':`scenelayer_x`+","+`scenelayer_y`,
                    'scale':"0.7,0.7", 'rotation':"17.59",
                    'alpha':"1",'colorize':"255,255,255",
                    'image':"IMAGE_SCENE_ISLAND1_OCD_FLAG1",
                    'anim':"ocdFlagWave",'animspeed':"1"}
            rootmbt = root.meta.find_immediate_child_by_tag('SceneLayer')
            _appendChildTag(root,rootmbt,attrib,keepid=True)

            # seek WooGLE_Test Button element
           # print "seeking button"
            for buttongroup in root.findall('buttongroup'):
                if buttongroup.get('id')=='levelMarkerGroup':
                    buttongroupelement=buttongroup
                for button in buttongroup.findall('button'):
                    x,y = button.get_native('center')
                    if abs(x-button_x)<1:
                     if abs(y-button_y)<1:
            #            print "removing button",button.get('id')
                        button.parent.remove(button)

           # print "create button","lb_"+level_name
            rootmbt = buttongroupelement.meta.find_immediate_child_by_tag('button')
            attrib={'id':"lb_"+level_name,'onclick':"pl_"+level_name,
                    'depth': "0", 'center':`button_x`+","+`button_y`,
                    'scale':"1.008,0.848",'rotation':"-0.5",
                    'alpha':"1", 'colorize':"255,255,255",
                    'up':"IMAGE_SCENE_ISLAND1_LEVELMARKERA_UP",
                    'over':"IMAGE_SCENE_ISLAND1_LEVELMARKERA_OVER"}
            _appendChildTag(buttongroupelement,rootmbt,attrib,keepid=True)

            #save file
            xml_data = tree.to_xml()
            xml_data = xml_data.replace('><','>\n<')
            file( path, 'wb' ).write( xml_data )
           # print "Done",path
        return True

    def newLevel( self, name ):
        """Creates a new blank level with the specified name.
           May fails with an IOError or OSError."""
        return self._addNewLevel( name,
            self._universe.make_unattached_tree_from_xml( metawog.TREE_LEVEL_GAME,
                                                          metawog.LEVEL_GAME_TEMPLATE ),
            self._universe.make_unattached_tree_from_xml( metawog.TREE_LEVEL_SCENE,
                                                          metawog.LEVEL_SCENE_TEMPLATE ),
            self._universe.make_unattached_tree_from_xml( metawog.TREE_LEVEL_RESOURCE,
                                                          metawog.LEVEL_RESOURCE_TEMPLATE ),
            self._universe.make_unattached_tree_from_xml( metawog.TREE_LEVEL_ADDIN,
                                                          metawog.LEVEL_ADDIN_TEMPLATE.replace("LevelName",name)),
            self._universe.make_unattached_tree_from_xml( metawog.TREE_LEVEL_TEXT,
                                                          metawog.LEVEL_TEXT_TEMPLATE ) ,
            self._universe.make_unattached_tree_from_xml( metawog.TREE_LEVEL_DEPENDANCY,
                                                          metawog.LEVEL_DEPENDANCY_TEMPLATE ) )


    def cloneLevel( self, cloned_name, new_name ):
        #Clone an existing level and its resources.
           model = self.getModel( cloned_name )
           dir = os.path.join( self._res_dir, STR_DIR_STUB, new_name )
           if not os.path.isdir( dir ):
                os.mkdir( dir )

           #new cloning method... #2
           # worked for balls... might be going back to the old Nitrozark way..
           # which didn't work right... Hmmm.!

            #get xml from existing
            #make unattached trees from it
           new_level_tree = self._universe.make_unattached_tree_from_xml( metawog.TREE_LEVEL_GAME,
                                                                        model.level_root.tree.to_xml() )

           new_scene_tree = self._universe.make_unattached_tree_from_xml( metawog.TREE_LEVEL_SCENE,
                                                                        model.scene_root.tree.to_xml() )

           new_res_tree = self._universe.make_unattached_tree_from_xml( metawog.TREE_LEVEL_RESOURCE,
                                                                        model.resource_root.tree.to_xml() )

           new_addin_tree = self._universe.make_unattached_tree_from_xml( metawog.TREE_LEVEL_ADDIN,
                                                                        model.addin_root.tree.to_xml() )
           new_text_tree = self._universe.make_unattached_tree_from_xml( metawog.TREE_LEVEL_TEXT,
                                                                        model.text_root.tree.to_xml() )

            #change stuff
           for resource_element in new_res_tree.root.findall( './/Resources' ):
                resource_element.set( 'id', 'scene_%s' % new_name )

           for resource_element in new_res_tree.root.findall( './/Image' ):
                resid = resource_element.get('id')
                resource_element.set('id',resid.replace('_'+cloned_name.upper()+'_','_'+new_name.upper()+'_',1))

           for resource_element in new_res_tree.root.findall( './/Sound' ):
                resid = resource_element.get('id')
                resource_element.set('id',resid.replace('_'+cloned_name.upper()+'_','_'+new_name.upper()+'_',1))

           for resource_element in new_text_tree.root.findall( './/string' ):
                resid = resource_element.get('id')
                resource_element.set('id',resid.replace('_'+cloned_name.upper()+'_','_'+new_name.upper()+'_',1))

           #also rewrite important things in the addin xml
           dir_element = new_addin_tree.root.find('.//dir')
           #print ".dir element",dir_element
           dir_element.set('value', new_name)

           name_element = new_addin_tree.root.find('name')
           #print "addin.name element",name_element
           name_element.set('value', new_name)

           name_element = new_addin_tree.root.find('levels/level/name')
           #print "addin.levels.level.name element",name_element
           name_element.set('text', new_name)


           id_element = new_addin_tree.root.find('.//id')
           id_element.set('value', "")

           self._res_swap(new_level_tree.root,'_'+cloned_name.upper()+'_','_'+new_name.upper()+'_')
           self._res_swap(new_scene_tree.root,'_'+cloned_name.upper()+'_','_'+new_name.upper()+'_')

            #save out new trees
           self._saveUnPackedData( dir, new_name + '.addin.xml', new_addin_tree )
           self._saveUnPackedData( dir, new_name + '.text.xml', new_text_tree )
           self._savePackedData( dir, new_name+'.level.xml', new_level_tree )
           self._savePackedData( dir, new_name+'.scene.xml', new_scene_tree )
           self._savePackedData( dir, new_name+'.resrc.xml', new_res_tree )

           self._levels.append( unicode(new_name) )
           self._levels.sort(key=unicode.lower)
           self.__is_dirty = True

    def _res_swap(self,element,find,replace):
           for attribute in element.meta.attributes:
                if attribute.type==metaworld.REFERENCE_TYPE:
                    if attribute.reference_family in ['image','sound','TEXT_LEVELNAME_STR']:
                        value = element.get(attribute.name,None)
                        if value is not None:
                            rv = ','.join([v.replace(find,replace,1) for v in value.split(',')])
                            element.set(attribute.name,rv)
           for child in element.getchildren():
                self._res_swap(child,find,replace)

    def creategoomod( self, name ,goomod_filename, include_deps = False):
           model = self.getModel( name )
           dir = os.path.join( self._res_dir, STR_DIR_STUB, name )
           goomod_dir = os.path.join(dir,"goomod")
           stub = os.path.join('res',STR_DIR_STUB,name)
           compile_dir = os.path.join(goomod_dir,'compile', stub)
           if not os.path.isdir( compile_dir ):
                os.makedirs( compile_dir)
           files_to_goomod=[os.path.join(goomod_dir,'addin.xml')]
           self._saveUnPackedData( goomod_dir, 'addin.xml', model.addin_root.tree )
           #@DaB Check if there's text and write file if needed.
           textfile = os.path.join(goomod_dir, 'text.xml')
           if model.hasText:
               files_to_goomod.append(textfile)
               self._saveUnPackedData( goomod_dir, 'text.xml', model.text_root.tree )
           else:
               # if no text required, remove the goomod text.xml file if it exists.
               if os.path.exists(textfile):
                   os.remove(textfile)

           files_to_goomod.append(os.path.join(compile_dir, name + '.level.xml'))
           self._saveUnPackedData( compile_dir, name + '.level.xml', model.level_root.tree )
           files_to_goomod.append(os.path.join(compile_dir, name + '.resrc.xml'))
           self._saveUnPackedData( compile_dir, name + '.resrc.xml', model.resource_root.tree )
           files_to_goomod.append(os.path.join(compile_dir, name + '.scene.xml'))
           self._saveUnPackedData( compile_dir, name + '.scene.xml', model.scene_root.tree )

           #That's the level xml taken care of... now lets see if there are any resources to copy
           # The dependancy tree is our friend here.
           # it already contains elements for every (non-original) image and sound
           # as well as entries for balls, particles and materials.
           # it's kinda why I did it.

           # if they've said include_all_dep then we include "everything" in the dep_tree
           # if not, we only do Root Level images and sounds (immediate child of root)
           global_resources = {}
           files_to_copy = set()
           self._add_dep_to_goomod(model.dependancy_root,files_to_copy,files_to_goomod,global_resources,compile_dir,goomod_dir,full_dep=include_deps)

           if len(files_to_copy)>0:
               override_dir = os.path.join(goomod_dir,'override')
               if not os.path.isdir( override_dir ):
                    os.makedirs( override_dir)
               for file in files_to_copy:

                   file_bits = os.path.split(file)
                   dest_dir = os.path.join(override_dir,file_bits[0])
                   if not os.path.isdir( dest_dir ):
                      os.makedirs( dest_dir)
                   src_file = os.path.join(self._wog_dir,file)
                   dest_file = os.path.join(override_dir,file)
                   copy2(src_file,dest_file)
                   files_to_goomod.append(dest_file)
                   #print "Copied",src_file," to ",dest_file

           #Now create the zip file.
           len_gf = len(goomod_dir)+1
           zout = zipfile.ZipFile(goomod_filename, "w")
           for file in files_to_goomod:
                file_in_zip = file[len_gf:].replace("\\","/")
                #print "goomoding...",file,"as",file_in_zip
                zout.write(file,file_in_zip)
           zout.close()

    def _add_dep_to_goomod(self,element,files_to_copy,files_to_goomod,global_resources,compile_dir,goomod_dir,full_dep=False):
        # first an any 'image' or 'sound' children at this level

        for child in element.getchildren():
            if child.tag == "image":
                files_to_copy.add(child.get('path')+".png")
                if element.tag in ['effect']:
                    #print "effect image found ",child,child.get('id'),child.get('path')
                    global_resources[child.get('id')]=child.get('path')
            elif child.tag=="sound":
                files_to_copy.add(child.get('path')+".ogg")

        if full_dep:
        # findall balls and call with full_dep false
            ball_path = os.path.join(goomod_dir,'compile','res','balls')
            for ball in element.findall('.//ball'):
                # locate ball world
                ball_world = self.global_world.find_world(metawog.WORLD_BALL,ball.get('name'))
                ball_dir = os.path.join(ball_path, ball.get('name'))
                # ENSURE PATH EXISTS
                if not os.path.isdir( ball_dir ):
                   os.makedirs( ball_dir )

                filename =os.path.join(ball_dir,'balls.xml')
                files_to_goomod.append(filename)
                xml_data = file( ball_world.find_tree(metawog.TREE_BALL_MAIN).filename, 'rb' ).read()
                file( filename, 'wb' ).write( xml_data )
                #self._saveUnPackedData( ball_dir, 'balls.xml.xml', ball_tree)

                filename =os.path.join(ball_dir,'resources.xml')
                files_to_goomod.append(filename)
                xml_data = file( ball_world.find_tree(metawog.TREE_BALL_RESOURCE).filename, 'rb' ).read()
                file( filename, 'wb' ).write( xml_data )
#                self._saveUnPackedData( ball_dir, 'resources.xml.xml', ball_tree)

                self._add_dep_to_goomod(ball,files_to_copy,files_to_goomod,global_resources,compile_dir,goomod_dir,full_dep = False)

            for anim in element.findall('anim'):
                files_to_copy.add("res/anim/" + anim.get('id','')+".anim.binltl")

        # findall particles (store xml) and call with fulldep=false
            particle_xml = ''
            for effect in element.findall('.//effect'):
               res_element = self.global_world.resolve_reference( metawog.WORLD_GLOBAL, 'effect', effect.get('name') )
               particle_xml+=res_element.to_xml()+"\n"
               self._add_dep_to_goomod(effect,files_to_copy,files_to_goomod,global_resources,compile_dir,goomod_dir,full_dep = False)

        # findall materials (store xml)
            material_xml=''
            for material in element.findall('.//material'):
               res_element = self.global_world.resolve_reference( metawog.WORLD_GLOBAL, 'material', material.get('id') )
               material_xml+=res_element.to_xml()+"\n"

            # output xsl for particles, and add for goomod
            merge_path = os.path.join(goomod_dir,'merge','properties')
            if particle_xml!='':
               # print "particle_xml=",particle_xml
                params = {}
                params['path']='/effects'
                params['xml_data'] = particle_xml
                files_to_goomod.append(os.path.join(merge_path,'fx.xml.xsl'))
                self._output_xsl(metawog.XSL_ADD_TEMPLATE,params,merge_path,'fx.xml.xsl')

            # output xsl for materials, and add for goomod
            if material_xml!='':
               # print "material_xml=",material_xml
                params = {}
                params['path']='/materials'
                params['xml_data'] = material_xml
                files_to_goomod.append(os.path.join(merge_path,'materials.xml.xsl'))
                self._output_xsl(metawog.XSL_ADD_TEMPLATE,params,merge_path,'materials.xml.xsl')

            # custom resources required for particles effect need to go in
            # the global  resources.xml  so require a merge xsl
            if len(global_resources)>0:
               resource_xml= '<SetDefaults path="./" idprefix=""/>\n'
               for id,path in global_resources.items():
                    resource_xml+='<Image id="%(id)s" path="%(path)s" />\n' % {'id':id,'path':path}
               params = {}
               params['path']="/ResourceManifest/Resources[@id='common']"
               params['xml_data'] = resource_xml
               files_to_goomod.append(os.path.join(merge_path,'resources.xml.xsl'))
               self._output_xsl(metawog.XSL_ADD_TEMPLATE,params,merge_path,'resources.xml.xsl')

            # done?

    def _output_xsl(self,template,params,directory,filename):
        if not os.path.isdir( directory  ):
            os.makedirs( directory )
        output_path = os.path.join( directory, filename )
        xsl = template % params
        output_data = CREATED_BY + xsl.replace('><','>\n<')
        file( output_path, 'wb' ).write( output_data )

    def _isOriginalFile(self, filename,extension):

        path_bits = filename.replace('\\','/').split("/")
        if len(path_bits)==1:
            print filename,path_bits
            return False
        path_bits.pop(0)
        file = path_bits.pop(len(path_bits)-1)
        root_element = self._files_tree.root
        return self._seekFile(root_element,path_bits,file,extension)

    def _seekFile(self,element,path,file,ext):

        if path==[]:
            for fileitem in element.findall('file'):
                if fileitem.get('name')==file:
                    if fileitem.get('type')==ext:
                        return True
            return False
        else:
            for folder in element.findall('folder'):
                if folder.get('name')==path[0]:
                    path.pop(0)
                    return self._seekFile(folder,path,file,ext)
            return False

    def _addNewLevel( self, name, level_tree, scene_tree, resource_tree, addin_tree=None,text_tree=None, dependancy_tree=None ):
        """Adds a new level using the specified level, scene and resource tree.
           The level directory is created, but the level xml files will not be saved immediately.
        """
        dir_path = os.path.join( self._res_dir, STR_DIR_STUB, name )
        if not os.path.isdir( dir_path ):
             os.mkdir( dir_path )


        # Fix the hard-coded level name in resource tree: <Resources id="scene_NewTemplate" >
        for resource_element in resource_tree.root.findall( './/Resources' ):
            resource_element.set( 'id', 'scene_%s' % name )
        # Creates and register the new level
        world = self.global_world.make_world( metawog.WORLD_LEVEL, name,
                                                    LevelWorld, self, is_dirty = True )
        treestoadd = [level_tree,scene_tree,resource_tree]
        if addin_tree is not None:
            treestoadd.append(addin_tree)
        if text_tree is not None:
            treestoadd.append(text_tree)
        if dependancy_tree is not None:
            treestoadd.append(dependancy_tree)

        world.add_tree( treestoadd )

        self.models_by_name[name] = world
        self._levels.append( unicode(name) )
        self._levels.sort(key=unicode.lower)
        self.__is_dirty = True

class BallModel(metaworld.World):
    def __init__( self, universe, world_meta, ball_name, game_model ):
        metaworld.World.__init__( self, universe, world_meta, ball_name )
        self.game_model = game_model
        self.is_dirty = False

class ThingWorld(metaworld.World,
                 metaworldui.SelectedElementsTracker,
                 metaworldui.ElementIssueTracker,
                 metaworldui.UndoWorldTracker):
    def __init__( self, universe, world_meta, name, game_model, is_dirty = False ):
        metaworld.World.__init__( self, universe, world_meta, name )
        metaworldui.SelectedElementsTracker.__init__( self, self )
        metaworldui.ElementIssueTracker.__init__( self,self )
        metaworldui.UndoWorldTracker.__init__(self,self,100)
        self.game_model = game_model

    @property
    def name( self ):
        return self.key

class LevelWorld(ThingWorld):
    def __init__( self, universe, world_meta, name, game_model, is_dirty = False ):
        ThingWorld.__init__(self, universe, world_meta, name, game_model, is_dirty = is_dirty )
        self.__dirty_tracker = metaworldui.DirtyWorldTracker( self, is_dirty )
        self._importError = None
        self._sceneissues = ''
        self._levelissues = ''
        self._resrcissues = ''
        self._globalissues = ''
        self._scene_issue_level = ISSUE_LEVEL_NONE
        self._level_issue_level = ISSUE_LEVEL_NONE
        self._resrc_issue_level = ISSUE_LEVEL_NONE
        self._global_issue_level = ISSUE_LEVEL_NONE
        self._dependancyissues = ''
        self._dependancy_issue_level = ISSUE_LEVEL_NONE
        self._view = None

    @property
    def level_root( self ):
        return self.find_tree( metawog.TREE_LEVEL_GAME ).root

    @property
    def scene_root( self ):
        return self.find_tree( metawog.TREE_LEVEL_SCENE ).root

    @property
    def resource_root( self ):
        return self.find_tree( metawog.TREE_LEVEL_RESOURCE ).root

    @property
    def addin_root( self ):
        return self.find_tree( metawog.TREE_LEVEL_ADDIN ).root

    @property
    def text_root( self ):
        return self.find_tree( metawog.TREE_LEVEL_TEXT ).root

    @property
    def dependancy_root( self ):
        return self.find_tree( metawog.TREE_LEVEL_DEPENDANCY ).root

    @property
    def is_dirty( self ):
        return self.__dirty_tracker.is_dirty

    @property
    def isReadOnly( self ):
        return self.name.lower() in metawog.LEVELS_ORIGINAL_LOWER

    @property
    def hasText(self):
       return self.find_tree( metawog.TREE_LEVEL_TEXT ).root.find('string') is not None

    @property
    def view( self ):
        return self._view

    def setView (self, newview):
        self._view = newview

    #@DaB - Issue checking used when saving the level, or making a goomod
    def hasIssues (self):
		#Checks all 3 element trees for outstanding issues
		# Returns True if there are any.
        self._buildDependancyTree()
        tIssue = ISSUE_LEVEL_NONE
        if self.element_issue_level(self.scene_root):
            tIssue |= ISSUE_LEVEL_CRITICAL
        if self.element_issue_level(self.level_root):
            tIssue |= ISSUE_LEVEL_CRITICAL
        if self.element_issue_level(self.resource_root):
            tIssue |= ISSUE_LEVEL_CRITICAL
        if self.element_issue_level(self.text_root):
            tIssue |= ISSUE_LEVEL_CRITICAL
        #If we have a tree Issue.. don't perform the extra checks
        #because that can cause rt errors (because of the tree issues)
        #and then we don't see a popup.
        if tIssue==ISSUE_LEVEL_CRITICAL:
            #ensure old issues don't get redisplayed is we do "bail" here
            self._sceneissues = ''
            self._levelissues = ''
            self._resrcissues = ''
            self._globalissues = ''
            return tIssue
        if self.haslevel_issue():
            tIssue |= self._level_issue_level
        if self.hasscene_issue():
            tIssue |= self._scene_issue_level
        if self.hasresrc_issue():
            tIssue |= self._resrc_issue_level
        if self.hasglobal_issue():
            tIssue |= self._global_issue_level
        if self.hasdependancy_issue():
            tIssue |= (self._dependancy_issue_level & 6)

        return tIssue

    def getIssues (self):
		#Get a 'report' of outstanding Issues
		#Used for Popup Message
        txtIssue = ''
        if self.element_issue_level(self.scene_root):
            txtIssue = txtIssue + '<p>Scene Tree:<br>' + self.element_issue_report(self.scene_root) + '</p>'
        if self.scene_issue_report!='':
            txtIssue+= '<p>Scene Checks:<br>' + self.scene_issue_report + '</p>'
        if self.element_issue_level(self.level_root):
            txtIssue = txtIssue + '<p>Level Tree:<br>' + self.element_issue_report(self.level_root) + '</p>'
        if self.level_issue_report!='':
            txtIssue+= '<p>Level Checks:<br>' + self.level_issue_report + '</p>'
        if self.element_issue_level(self.resource_root):
            txtIssue = txtIssue + '<p>Resource Tree:<br>' + self.element_issue_report(self.resource_root) + '</p>'
        if self.resrc_issue_report!='':
            txtIssue+= '<p>Resource Checks:<br>' + self.resrc_issue_report + '</p>'
        if self.global_issue_report!='':
            txtIssue+= '<p>Global Checks:<br>' + self.global_issue_report + '</p>'
        if self.element_issue_level(self.text_root):
            txtIssue = txtIssue + '<p>Text Tree:<br>' + self.element_issue_report(self.text_root) + '</p>'
        if self.dependancy_issue_report!='':
            txtIssue+= '<p>Dependancy Checks:<br>' + self.dependancy_issue_report + '</p>'

        return txtIssue

    #@DaB Additional Checking Level,Scene,Resource (at tree level)
    def hasglobal_issue(self):
        # check for issues across trees
        #if there's a levelexit it must be within the scene bounds
        self._globalissues = ''
        self._global_issue_level = ISSUE_LEVEL_NONE
        levelexit = self.level_root.find('levelexit')
        if levelexit is not None:
            exit_posx, exit_posy = levelexit.get_native('pos')
            minx,maxx = self.scene_root.get_native('minx'),self.scene_root.get_native('maxx')
            miny,maxy = self.scene_root.get_native('miny'),self.scene_root.get_native('maxy')
            if exit_posx > maxx or exit_posx<minx or exit_posy>maxy or exit_posy<miny:
                # exit outside scene bounds warning
                self.addGlobalError(401,None)

        #Custom Pipe checks
        pipe = self.level_root.find('pipe')
        if pipe is not None:
            pipe_type = pipe.get('type','default')
            if pipe_type not in metawog.PIPES_STANDARD.keys():
                # it's a custom pipe
                # Check the necessary resources exist
                for resource_id_stub in metawog.PIPE_RESOURCE_STUBS:
                    image_id = metawog.PIPE_RESOURCE + resource_id_stub + "_" + pipe_type
                    image_element = self.resolve_reference(metawog.WORLD_LEVEL, 'image', image_id )
                    if image_element is None:
                        self.addGlobalError(401,image_id)

        return self._global_issue_level!=ISSUE_LEVEL_NONE

    def haslevel_issue(self):
        # rules for "DUMBASS" proofing (would normally use a much ruder word)

        root = self.level_root
        self._levelissues=''
        self._level_issue_level = ISSUE_LEVEL_NONE
        normal_camera = False
        widescreen_camera = False

        #must have 1 normal camera and 1 widescreen camera
        for camera in root.findall('camera'):
            c_aspect = camera.get('aspect')
            if c_aspect == 'normal':
                normal_camera=True
            elif c_aspect == 'widescreen':
                widescreen_camera = True

            #only Single poi travel time check
            if len(camera._children)==1:
                if camera._children[0].get_native('traveltime',0)>1:
                   self.addLevelError(101,c_aspect)

        if not normal_camera:
            self.addLevelError(102,None)

        if not widescreen_camera:
            self.addLevelError(103,None)

        end_conditions = []
        targetheight = root.find('targetheight')
        if targetheight is not None:
            end_conditions.append('targetheight')
        endoncollision = root.find('endoncollision')
        if endoncollision is not None:
            end_conditions.append('endoncollision')

        #if there's a levelexit there must be a pipe
        levelexit = root.find('levelexit')
        pipe = root.find('pipe')
        if levelexit is not None:
            end_conditions.append('levelexit')
            if pipe is None:
                self.addLevelError(104,None)

            else:
                pipex,pipey = pipe.getchildren()[0].get_native('pos')
                levelx,levely = levelexit.get_native('pos')
                levelr = levelexit.get_native('radius')/2
                if levelview.vector2d_length(pipex-levelx, pipey-levely)>levelr:
                    self.addLevelError(105,None)

        if pipe is not None:
            vertexes = pipe.getchildren()
            if len(vertexes)>2:
                tvertex = [levelview.toQPointF(vertexes[0].get_native('pos')),
                           levelview.toQPointF(vertexes[0].get_native('pos')),
                           levelview.toQPointF(vertexes[1].get_native('pos'))]
                for vertex in vertexes[2:]:
                    tvertex.pop(0)
                    tvertex.append(levelview.toQPointF(vertex.get_native('pos')))
                    # normalized dot product
                    v1,v2= tvertex[1]-tvertex[0],tvertex[2]-tvertex[1]
                    v1l = levelview.vector2d_length(v1.x(), v1.y())
                    v2l = levelview.vector2d_length(v2.x(), v2.y())
                    if v1l<=40 or v2l<=40:
                        self.addLevelError(106,None)
                        break
                    dp = ((v1.x()*v2.x())+(v1.y()*v2.y()))/(v1l*v2l)
                    if dp>=0.95:
                        self.addLevelError(107,None)
                        break
                    elif dp<=-0.95:
                        self.addLevelError(108,None)
                        break

        #Final Ball Checks
        #Need to check that no "funny business" has gone on since the strands were added
        #also get the max height (y) of any ball
        # Build Dict of Ball Ids and Types
        balls={}
        ball = root.find('BallInstance')
        if ball is not None:
            x,max_ball_height = ball.get_native('pos')
        else:
            max_ball_height = 0
        for ball in root.findall('BallInstance'):
            balls[ball.get('id')]=ball.get('type')
            x,y = ball.get_native('pos')
            if y>max_ball_height:
                max_ball_height = y

        problem_balls=set()
        for strand in root.findall('Strand'):
            gt1,gt2 = balls[strand.get('gb1')],balls[strand.get('gb2')]
            if gt1 in metawog.BALLS_NO_STRANDS:
               problem_balls.add(strand.get('gb1'))
            if gt2 in metawog.BALLS_NO_STRANDS:
               problem_balls.add(strand.get('gb2'))
            if (gt1 in metawog.BALLS_MUST_BE_GB1) and (gt2 in metawog.BALLS_MUST_BE_GB1):
               self.addLevelError(109,(gt1,strand.get('gb1'),gt2,strand.get('gb1')))

        for problem_ball in problem_balls:
               self.addLevelError(110,(balls[problem_ball],problem_ball))

        if len(end_conditions) > 1:
               self.addLevelError(111,','.join(end_conditions))

        if targetheight is not None:
            th = targetheight.get_native('y')
            if max_ball_height >= th:
               self.addLevelError(112,None)

        #Ambient Particles on "Local Effect Only" object (fire or signpost)
        ambient_effects = []
        fx_tree = self.game_model._effects_tree
        for effect in fx_tree.root.findall('ambientparticleeffect'):
            ambient_effects.append(effect.get('name'))

        for element in root.findall('signpost'):
            particles = element.get('particles')
            if particles is not None:
                if particles in ambient_effects:
                   self.addLevelError(113,(element.get('name',''),particles))

        for element in root.findall('fire'):
            particles = element.get('particles')
            if particles is not None:
                if particles in ambient_effects:
                   self.addLevelError(114,particles)

        return self._level_issue_level!=ISSUE_LEVEL_NONE


    def addSceneError(self,error_num,subst):
        error = errors.ERROR_INFO[error_num]
        self._scene_issue_level,self._sceneissues = self.addError(self._scene_issue_level,self._sceneissues,error,error_num,subst)

    def addLevelError(self,error_num,subst):
        error = errors.ERROR_INFO[error_num]
        self._level_issue_level,self._levelissues = self.addError(self._level_issue_level,self._levelissues,error,error_num,subst)

    def addResourceError(self,error_num,subst):
        error = errors.ERROR_INFO[error_num]
        self._resrc_issue_level,self._resrcissues = self.addError(self._resrc_issue_level,self._resrcissues,error,error_num,subst)

    def addGlobalError(self,error_num,subst):
        error = errors.ERROR_INFO[error_num]
        self._global_issue_level,self._globalissues = self.addError(self._global_issue_level,self._globalissues,error,error_num,subst)

    def addDependancyError(self,error_num,subst):
        error = errors.ERROR_INFO[error_num]
        self._dependancy_issue_level,self._dependancyissues = self.addError(self._dependancy_issue_level,self._dependancyissues,error,error_num,subst)


    def addError(self,err_level,err_message,error,error_num,err_subst):
        err_level|=error[0]
        err_message+=errors.ERROR_FRONT[error[0]]
        if err_subst is not None:
            err_message+=error[1] % err_subst
        else:
            err_message+=error[1]
        err_message+=errors.ERROR_MORE_INFO % error_num
        err_message+="<br>"
        return err_level,err_message



    def hasscene_issue(self):
        #rules
        root = self.scene_root
        self._scene_issue_level = ISSUE_LEVEL_NONE
        self._sceneissues = ''
        #motor attached to static body
        motorbodys = set()
        for motor in root.findall('motor'):
            motorbodys.add(motor.get('body'))
        hingebodys = set()
        for hinge in root.findall('hinge'):
            hingebodys.add(hinge.get('body1'))
            body2 = hinge.get('body2','')
            if body2 !='':
                hingebodys.add(hinge.get('body2'))

        rotspeedbodys=set()

        geomitems=[]
        for geomitem in root.findall('rectangle'):
            geomitems.append(geomitem)
        for geomitem in root.findall('circle'):
            geomitems.append(geomitem)

        # mass checks on rectangle and circles
        for geomitem in geomitems:
            geomstatic = geomitem.get_native('static',False)
            #static / masscheck!
            if not geomstatic:
                if geomitem.get_native('mass',0)<=0:
                    self.addSceneError(1,geomitem.get('id',''))
        # check on composite geoms
        geomchildren = set()
        for geomitem in root.findall('compositegeom'):
            geomitems.append(geomitem)
            geomstatic = geomitem.get_native('static',False)
            if not geomstatic:
             if geomitem.get_native('rotation',0)!=0:
                   self.addSceneError(2,geomitem.get('id',''))
            nchildren = 0
            for geomchild in geomitem.getchildren():
                nchildren+=1
                geomchildren.add(geomchild.get('id',''))
                if not geomstatic:
                   if geomchild.get_native('mass',0.0)<=0:
                       self.addSceneError(3,(geomitem.get('id',''),geomchild.get('id','')))
                if geomchild.get('image'):
                   self.addSceneError(4,geomchild.get('id',''))
            if nchildren==0:
                   if not geomstatic:
                        self.addSceneError(5,geomitem.get('id',''))
                   else:
                        self.addSceneError(6,geomitem.get('id',''))

        # Get any radial forcefields.. ready for next check
        rfflist = {}
        for rff in root.findall('radialforcefield'):
            rffid = rff.get('id',len(rfflist))
            rfflist[rffid] = rff.get_native('center')

        # check on ALL geometry bodies
        for geomitem in geomitems:
            id = geomitem.get('id','')
            if geomitem.get_native('rotspeed',0)!=0:
                rotspeedbodys.add(id)
            geomstatic = geomitem.get_native('static',False)
            #static vs motor check
            if geomstatic and id in motorbodys:
               self.addSceneError(7,id)

            if not geomstatic:
              gx,gy = geomitem.get_native('center',(0,0))
              for rffid,rffpos in rfflist.items():
                  if abs(gx-rffpos[0]+gy-rffpos[1])<0.001:
                      self.addSceneError(8,(id,rffid))

        # finally some checks on unfixed spinning things
        spinning = motorbodys | rotspeedbodys
        spinningnohinge = spinning - hingebodys
        for body in spinningnohinge:
           self.addSceneError(9,body)

        hingedchildren = hingebodys & geomchildren
        for hingedchild in hingedchildren:
           self.addSceneError(10,hingedchild)

        #linearforcefield can have center but no size
        #but CANNOT have size, but no center
        for lff in root.findall('linearforcefield'):
            if lff.get('size') is not None:
               if lff.get('center','')=='':
                   self.addSceneError(11,lff.get('id',''))

        return self._scene_issue_level != ISSUE_LEVEL_NONE

    def _get_all_resource_ids(self,root,tag):
        resource_ids = set()
        for resource in root.findall('.//'+tag):
            resource_ids.add(resource.get('id'))
        return resource_ids

    def _get_unused_resources(self):
        used = self._get_used_resources()
        resources={}
        resources['image'] = self._get_all_resource_ids(self.resource_root,"Image")
        resources['sound'] = self._get_all_resource_ids(self.resource_root,"Sound")
        resources['TEXT_LEVELNAME_STR'] =  self._get_all_resource_ids(self.text_root,"string")
        unused = {'image':set(),'sound':set(),'TEXT_LEVELNAME_STR':set()}
        for restype in unused.keys():
            unused[restype] = resources[restype]-used[restype]
        return unused

    def _remove_unused_resources(self,unused):
        self.suspend_undo()
        for family,unusedset in unused.items():
          for unusedid in unusedset:
            element = self.resolve_reference(metawog.WORLD_LEVEL, family, unusedid)
            if element is not None:
                element.parent.remove(element)
        self.activate_undo()

    def _get_used_resources(self):
        used = {'image':set(),'sound':set(),'TEXT_LEVELNAME_STR':set()}
        oftype = ['image','sound','TEXT_LEVELNAME_STR']
        #go through scene and level root
        #store the resource id of any that do
        for element in self.scene_root:
            for attribute_meta in element.meta.attributes:
                if attribute_meta.type == metaworld.REFERENCE_TYPE:
                    if attribute_meta.reference_family in oftype:
                        if element.get(attribute_meta.name):
                            if attribute_meta.is_list:
                                for res in element.get(attribute_meta.name).split(','):
                                    used[attribute_meta.reference_family].add(res)
                            else:
                                used[attribute_meta.reference_family].add(element.get(attribute_meta.name))

        for element in self.level_root:
          for attribute_meta in element.meta.attributes:
            if attribute_meta.type == metaworld.REFERENCE_TYPE:
                if attribute_meta.reference_family in oftype:
                    if element.get(attribute_meta.name):
                        if attribute_meta.is_list:
                            for res in element.get(attribute_meta.name).split(','):
                                used[attribute_meta.reference_family].add(res)
                        else:
                            used[attribute_meta.reference_family].add(element.get(attribute_meta.name))

        # Also if there's a custom pipe
        # Add the required resource names to used... so it thinks they are
        pipe = self.level_root.find('pipe')
        if pipe is not None:
            pipe_type = pipe.get('type','default')
            if pipe_type not in metawog.PIPES_STANDARD.keys():
                # it's a custom pipe
                for resource_id_stub in metawog.PIPE_RESOURCE_STUBS:
                    image_id = metawog.PIPE_RESOURCE + resource_id_stub + "_" + pipe_type
                    used['image'].add(image_id)
        return used

    def hasresrc_issue(self):
        root = self.resource_root
        self._resrcissues = ''
        self._resrc_issue_level = ISSUE_LEVEL_NONE
        # confirm every file referenced exists
        used_resources = self._get_used_resources()
        image_resources = set()
        for resource in root.findall('.//Image'):
            image_resources.add(resource.get('id'))
            full_filename = os.path.join(self.game_model._wog_dir,resource.get('path')+".png")
            if ON_PLATFORM == PLATFORM_WIN:
                #confirm extension on drive is lower case
                real_filename= _getRealFilename(full_filename)
                real_ext = os.path.splitext(real_filename)[1]
                if real_ext != ".png":
                  self.addResourceError(201,resource.get('path')+real_ext)

        unused_images = image_resources.difference(used_resources['image'])
        if len(unused_images)!=0:
            for unused in unused_images:
               self.addResourceError(202,unused)

        sound_resources = set()
        for resource in root.findall('.//Sound'):
            sound_resources.add(resource.get('id'))
            full_filename = os.path.join(self.game_model._wog_dir,resource.get('path')+".ogg")

            if ON_PLATFORM == PLATFORM_WIN:
                #confirm extension on drive is lower case
                real_filename=_getRealFilename(full_filename)
                real_ext = os.path.splitext(real_filename)[1]
                if real_ext != ".ogg":
                    self.addResourceError(203,resource.get('path')+real_ext)


        unused_sounds = sound_resources.difference(used_resources['sound'])
        if len(unused_sounds)!=0:
            for unused in unused_sounds:
               self.addResourceError(204,unused)

        text_resources = set()
        for resource in self.text_root.findall('.//string'):
            text_resources.add(resource.get('id'))

        unused_texts = text_resources.difference(used_resources['TEXT_LEVELNAME_STR'])
        if len(unused_texts)!=0:
            for unused in unused_texts:
               self.addResourceError(205,unused)

        return self._resrc_issue_level != ISSUE_LEVEL_NONE

    @property
    def scene_issue_report(self):
        return self._sceneissues
    @property
    def level_issue_report(self):
        return self._levelissues
    @property
    def resrc_issue_report(self):
        return self._resrcissues
    @property
    def global_issue_report(self):
        return self._globalissues

    #@DaB Additional Issue checking when trying to produce a goomod
    def hasAddinIssues (self):
        if self.element_issue_level(self.addin_root):
            return True
        return False

    def getAddinIssues (self):
        txtIssue = ''
        if self.element_issue_level(self.addin_root):
            txtIssue = txtIssue + 'Addin : ' + self.element_issue_report(self.addin_root) + '<br>'
        return txtIssue

    def _buildDependancyTree(self):
        self.suspend_undo()
        dependancy_tree = self.find_tree(metawog.TREE_LEVEL_DEPENDANCY)
        if dependancy_tree is not None:
            self.remove_tree(dependancy_tree)
        dependancy_tree =  self.make_tree_from_xml( metawog.TREE_LEVEL_DEPENDANCY, metawog.LEVEL_DEPENDANCY_TEMPLATE )
        current={'imagedep':set(),'sounddep':set(),'effectdep':set(),'materialdep':set(),'balldep':set(),'animdep':set()}
        ball_trace=''
        self._recursion = []

        self.game_model.global_world.refreshFromFiles()

        self._addDependancies(self.level_root,dependancy_tree.root,current,ball_trace)
        self._addDependancies(self.scene_root,dependancy_tree.root,current,ball_trace)

        #additional non-recursive stuff
        for element in self.scene_root.findall('.//SceneLayer'):
            anim = element.get('anim','')
            if anim!='':
               if anim not in metawog.ANIMATIONS_ORIGINAL:
                  if anim not in current['animdep']:
                    child_attrib = {'id':anim}
                    if anim in metawog.ANIMATIONS_GLOBAL:
                        child_attrib['found']="true"
                    child_element = metaworld.Element( metawog.DEP_ANIM, child_attrib)
                    dependancy_tree.root._children.append( child_element )
                    child_element._parent = dependancy_tree.root
                    current['animdep'].add(child_attrib['id'])

        for element in self.resource_root.findall('.//Image'):
            child_attrib = {'found':"true"}
            for attribute in element.meta.attributes:
               child_attrib[attribute.name]=element.get(attribute.name)
            if child_attrib['path'] not in current['imagedep']:
                child_element = metaworld.Element( metawog.DEP_IMAGE, child_attrib)
                dependancy_tree.root._children.append( child_element )
                child_element._parent = dependancy_tree.root
                current['imagedep'].add(child_attrib['path'])

        for element in self.resource_root.findall('.//Sound'):
            child_attrib = {'found':"true"}
            for attribute in element.meta.attributes:
               child_attrib[attribute.name]=element.get(attribute.name)
            if child_attrib['path'] not in current['sounddep']:
                child_element = metaworld.Element( metawog.DEP_SOUND, child_attrib)
                dependancy_tree.root._children.append( child_element )
                child_element._parent = dependancy_tree.root
                current['sounddep'].add(child_attrib['path'])

        self._removeOriginalDependancies(dependancy_tree.root)
        self.activate_undo()
        self.__dirty_tracker.clean_tree(metawog.TREE_LEVEL_DEPENDANCY)
        return dependancy_tree

    def _isNumber(self,input):
        try:
            f = float(input)
            return True
        except ValueError:
            return False

    def _removeOriginalDependancies(self,element):
        children = []
        children.extend(element.getchildren())
        for child in children:
            self._removeOriginalDependancies(child)

        remove = False
        if (element.tag=='image') or (element.tag=='sound'):
          extensions={'image':'png','sound':'ogg'}
          #print "checking : ",element.tag,element.get('path')
          if self.game_model._isOriginalFile(element.get('path'),extensions[element.tag]):
             remove = True
          if not remove:
             if element.get('id','')=='':
                # path but no id, swap em
                element.set('found','* resource id not found *')
             elif element.get_native('found',False):
                fullfilename = os.path.normpath(os.path.join(self.game_model._wog_dir,element.get('path')+'.'+extensions[element.tag]))
                if not os.path.exists(fullfilename):
                     element.set('found','* file not found *')
             #print "remove",element.tag, element.get('path')
        elif element.tag=='material':
            if element.get('id') in metawog.MATERIALS_ORIGINAL:
                 remove = True
            if not remove:
              if not element.get_native('found',False):
                  element.set('found','* custom material not found *')
             #    print "remove",element.tag, element.get('id')
        elif element.tag=='effect':
            if (element.get('name') in metawog.PARTICLEEFFECTS_ORIGINAL) or (element.get('name') in metawog.AMBIENTEFFECTS_ORIGINAL):
                 remove = True
            if not remove:
              if not element.get_native('found',False):
                  element.set('found','* particle effect not found *')
        elif element.tag=='ball':
            if element.get('name') in metawog.BALLS_ORIGINAL:
                 remove = True
            if not remove:
              if not element.get_native('found',False):
                  element.set('found','* ball not found *')
        elif element.tag=='anim':
            if not element.get_native('found',False):
                 element.set('found','* anim file not found *')
            remove=False
        elif element.tag in ['dependancy']:
                remove=False
        else:
            print "Unknown Dependancy Tag", element.tag

        if remove and len(element.getchildren())==0 and element.get_native('found',False):
#            print "Actually Removing",element.tag
             index = element.parent._children.index( element )
             del element.parent._children[index]
             element._parent = None
             del element

    def _addDependancies(self,element,dep_element,current, ball_trace):
        #run through the attributes of the element
        # add nodes at this level for any direct deps
        if element.tag=='ball':
            if element.get('name') in ball_trace.split(','):
                self._recursion.append(ball_trace+element.get('name'))
                return
            else:
                ball_trace+=element.get('name')+","
            #only bother refreshing custom balls
            if element.get('name') not in metawog.BALLS_ORIGINAL:
                world = element.world
                element.world.refreshFromFiles()
                element = world.find_tree(metawog.TREE_BALL_MAIN).root

        for attribute_meta in element.meta.attributes:
            if attribute_meta.type == metaworld.REFERENCE_TYPE:
             if attribute_meta.reference_family in ['image','sound','effect','material','ball']:
              if attribute_meta.name!='filter':
                attribute_value = attribute_meta.get(element)
                if attribute_value is not None:
                  if attribute_meta.is_list:
                      references=attribute_value.split(',')
                  else:
                      references=[attribute_value]
                  for reference in references:
                   if reference.strip()!='' and not self._isNumber(reference):
                    try:
                        res_element = self.resolve_reference( attribute_meta.reference_world, attribute_meta.reference_family, reference )
                    except ValueError:
                        res_element = None
                    if res_element is None:
                        ball_element = element
                        for i in range(4):
                          if ball_element.tag=="ball":
                             ball_world = ball_element.world
                             if ball_world is not None:
                                res_element = ball_world.resolve_reference( attribute_meta.reference_world, attribute_meta.reference_family, reference )
                             break
                          elif ball_element.parent is None:
                              print "ball_element no parent",ball_element.tag,i
                              break
                          ball_element = ball_element.parent

                    new_dep_meta = dep_element.meta.find_immediate_child_by_tag(attribute_meta.reference_family)
                    child_attrib = {}
                    id_attribute = None
                    if res_element is None:
                        #print "Empty res_element",element.tag, attribute_meta.name, attribute_meta.reference_world,attribute_meta.reference_family, reference
                        for dep_attribute in new_dep_meta.attributes:
                            if dep_attribute.type == metaworld.IDENTIFIER_TYPE:
                                child_attrib[dep_attribute.name]=reference
                                id_attribute = dep_attribute
                    else:
                        child_attrib['found']="true"
                        for dep_attribute in new_dep_meta.attributes:
                            if dep_attribute.name!='found':
                             child_attrib[dep_attribute.name]=res_element.get(dep_attribute.name)
                             if dep_attribute.type == metaworld.IDENTIFIER_TYPE:
                                id_attribute = dep_attribute

                    if id_attribute is None or res_element is None:
                        if reference not in current[id_attribute.reference_family]:
                            child_element = metaworld.Element( new_dep_meta, child_attrib)
                            dep_element._children.append( child_element )
                            child_element._parent = dep_element
                            current[id_attribute.reference_family].add(reference)
                    elif res_element.get(id_attribute.name) not in current[id_attribute.reference_family]:
                        child_element = metaworld.Element( new_dep_meta, child_attrib)
                        dep_element._children.append( child_element )
                        child_element._parent = dep_element
                        current[id_attribute.reference_family].add(res_element.get(id_attribute.name))
                        self._addDependancies(res_element,child_element,current,ball_trace)


        #now run through child elements
        for child_element in element.getchildren():
            self._addDependancies(child_element,dep_element,current,ball_trace)

    def hasDependancies(self):
        return len(self.dependancy_root.getchildren())>0

    def hasdependancy_issue(self):
        # things to check
        self._dependancyissues = ''
        self._dependancy_issue_level = ISSUE_LEVEL_NONE

        if len(self._recursion)>0:
            for recurse in self._recursion:
                self.addDependancyError(301,recurse.replace(',',' --> '))

        # Custom Balls
        ball_dep = {}
        for ball in self.dependancy_root.findall(".//ball"):
            ball_dep[ball.get('name')]=ball.get_native('found',False)

        if len(ball_dep)!=0:
            self.addDependancyError(302,','.join(ball_dep.keys()))
            for ball,found in ball_dep.items():
                if not found:
                    self.addDependancyError(303,ball)

        # Custom Materials
        material_dep = {}
        for material in self.dependancy_root.findall(".//material"):
            material_dep[material.get('id')]=(material.get_native('found',False)==True)

        if len(material_dep)!=0:
            self.addDependancyError(304,','.join(material_dep.keys()))
            for material,found in material_dep.items():
                if not found:
                    self.addDependancyError(305,material)

        # Custom Animations
        anim_dep = {}
        for animation in self.dependancy_root.findall(".//anim"):
            anim_dep[animation.get('id')]=animation.get_native('found',False)

        if len(anim_dep)!=0:
            self.addDependancyError(308,','.join(anim_dep.keys()))
            for anim,found in anim_dep.items():
                if not found:
                    self.addDependancyError(309,anim)

        # Custom Particles
        particles_dep = {}
        for effect in self.dependancy_root.findall(".//effect"):
            particles_dep[effect.get('name')]=effect.get_native('found',False)

        if len(particles_dep)!=0:
            self.addDependancyError(306,','.join(particles_dep.keys()))
            for effect,found in particles_dep.items():
                if not found:
                    self.addDependancyError(307,effect)

        return self._dependancy_issue_level != ISSUE_LEVEL_NONE

    @property
    def dependancy_issue_report(self):
        return self._dependancyissues

    def _cleanleveltree(self):
        self.suspend_undo()
        for strand in self.level_root.findall('Strand'):
            self.level_root.remove(strand)
            self.level_root.append(strand)
        self.activate_undo()

    def _cleanscenetree(self):
        self.suspend_undo()
        for hinge in self.scene_root.findall('hinge'):
            self.scene_root.remove(hinge)
            self.scene_root.append(hinge)
        for motor in self.scene_root.findall('motor'):
            self.scene_root.remove(motor)
            self.scene_root.append(motor)
        self.activate_undo()

    def _cleanresourcetree(self):
        #removes any unused resources from the resource and text resource trees
        self.suspend_undo()
        root = self.resource_root

        #ensure cAsE sensitive path is stored in resource file
        #Only required on windows...
        #If path was not CaSe SenSitivE match on Linux / Mac would be File not found earlier
        if ON_PLATFORM==PLATFORM_WIN:
          for resource in root.findall('.//Image'):
            full_filename = os.path.normpath(os.path.join(self.game_model._wog_dir,resource.get('path')+".png"))
            if os.path.exists(full_filename):
                #confirm extension on drive is lower case
                len_wogdir = len(os.path.normpath(self.game_model._wog_dir))+1
                real_filename = os.path.normpath(_getRealFilename(full_filename))
                real_file = os.path.splitext(real_filename)[0][len_wogdir:]
                full_file = os.path.splitext(full_filename)[0][len_wogdir:]
                if real_file != full_file:
                    print "Correcting Path",resource.get('id'),full_file,"-->",real_file
                    resource.attribute_meta('path').set(resource, real_file)

          for resource in root.findall('.//Sound'):
            full_filename = os.path.normpath(os.path.join(self.game_model._wog_dir,resource.get('path')+".ogg"))
            if os.path.exists(full_filename):
                #confirm extension on drive is lower case
                len_wogdir = len(os.path.normpath(self.game_model._wog_dir))
                real_filename = os.path.normpath(_getRealFilename(full_filename))
                real_file = os.path.splitext(real_filename)[0][len_wogdir:]
                full_file = os.path.splitext(full_filename)[0][len_wogdir:]
                if real_file != full_file:
                    print "Correcting Path",resource.get('id'),full_file,"-->",real_file
                    resource.attribute_meta('path').set(resource, real_file)

        self.activate_undo()

    def saveModifiedElements( self ):
        """Save the modified scene, level, resource tree."""
        if not self.isReadOnly:  # Discards change made on read-only level
            name = self.name
            dir = os.path.join( self.game_model._res_dir, STR_DIR_STUB, name )
            if not os.path.isdir( dir ):
                os.mkdir( dir )
            if self.__dirty_tracker.is_dirty_tree( metawog.TREE_LEVEL_ADDIN):
                self.game_model._saveUnPackedData( dir, name + '.addin.xml',
                                                 self.addin_root.tree )

            if self.__dirty_tracker.is_dirty_tree( metawog.TREE_LEVEL_TEXT):
                self.game_model._saveUnPackedData( dir, name + '.text.xml',
                                                 self.text_root.tree )

            if self.__dirty_tracker.is_dirty_tree( metawog.TREE_LEVEL_GAME):

                if not self.element_issue_level(self.level_root):
                  #clean tree caused an infinite loop when there was a missing ball
                  # so only clean trees with no issues
                  self._cleanleveltree()

                self.game_model._savePackedData( dir, name + '.level.xml',
                                                 self.level_root.tree )
            if self.__dirty_tracker.is_dirty_tree( metawog.TREE_LEVEL_RESOURCE):
                self.game_model._savePackedData( dir, name + '.resrc.xml',
                                                 self.resource_root.tree )

            # ON Mac
            # Convert all "custom" png to .png.binltl
            # Only works with REAL PNG
            if ON_PLATFORM==PLATFORM_MAC:
                for image in self.resource_root.findall('.//Image'):
                  if not self.game_model._isOriginalFile(image.get('path'),'png'):
                    in_path=os.path.join(self.game_model._wog_dir,image.get('path'))
                    out_path=in_path+'.png.binltl'
                    in_path+='.png'
                    wogfile.png2pngbinltl(in_path,out_path)


            if self.__dirty_tracker.is_dirty_tree( metawog.TREE_LEVEL_SCENE):
                if not self.element_issue_level(self.scene_root):
                    # so only clean trees with no issues
                    self._cleanscenetree()
                self.game_model._savePackedData( dir, name + '.scene.xml',
                                                 self.scene_root.tree )

        self.__dirty_tracker.clean()

    def clean_dirty_tracker(self):
        self.__dirty_tracker.clean()

    def getImagePixmap( self, image_id ):
        image_element = self.resolve_reference( metawog.WORLD_LEVEL, 'image', image_id )
        pixmap = None
        if image_element is not None:
            pixmap = self.game_model.pixmap_cache.get_pixmap( image_element )
        else:
            print 'Warning: invalid image reference:|',image_id,'|'
        return pixmap

    def updateResources( self ):
        """Ensures all image/sound resource present in the level directory
           are in the resource tree.
           Adds new resource to the resource tree if required.
        """

        game_dir = os.path.normpath( self.game_model._wog_dir )
        dir = os.path.join( game_dir, 'res', STR_DIR_STUB, self.name )
        if not os.path.isdir( dir ):
            print 'Warning: level directory does not exist'
            return []

        resource_element = self.resource_root.find( './/Resources' )
        if resource_element is None:
            print 'Warning: root element not found in resource tree'
            return []
        added_elements = []
        for tag, extension, id_prefix in ( ('Image','png', 'LEVEL_IMAGE_'), ('Sound','ogg', 'LEVEL_SOUND_') ):
            known_paths = set()
            for element in self.resource_root.findall( './/' + tag ):
                path = os.path.normpath( os.path.splitext( element.get('path','').lower() )[0] )
                # known path are related to wog top dir in unix format & lower case without the file extension
                known_paths.add( path )
            existing_paths = glob.glob( os.path.join( dir, '*.' + extension ) )
            for existing_path in existing_paths:
                existing_path = existing_path[len(game_dir)+1:] # makes path relative to wog top dir
                existing_path = os.path.splitext(existing_path)[0] # strip file extension
                path = os.path.normpath( existing_path ).lower()
                if path not in known_paths:
                    existing_path = os.path.split( existing_path )[1]
                    ALLOWED_CHARS = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789'
                    resource_id = id_prefix + ''.join( c for c in existing_path
                                                       if c.upper() in ALLOWED_CHARS )
                    resource_path = 'res/levels/%s/%s' % (self.name,existing_path)
                    meta_element = metawog.TREE_LEVEL_RESOURCE.find_element_meta_by_tag( tag )
                    new_resource = metaworld.Element( meta_element, {'id':resource_id.upper(),
                                                                     'path':resource_path} )
                    resource_element.append( new_resource )
                    added_elements.append( new_resource )
        return added_elements

	#@DaB New Functionality - Import resources direct from files
    def importError(self):
        return self._importError

    def importResources( self, importedfiles, res_dir ):
        """Import Resources direct from files into the Resource Tree
           If files are located outside the Wog/res folder it copies them
           png -> res/levels/{name}
           ogg -> res/music/{name}  for compatability with Soultaker's Volume control add-in
        """
        self._importError = None
        res_dir = os.path.normpath(res_dir)
        game_dir = os.path.split( res_dir )[0]
        resource_element = self.resource_root.find( './/Resources' )
        if resource_element is None:
            print 'Warning: root element not found in resource tree'
            return []
        all_local = True
        includesogg = False
        for file in importedfiles:
            file = os.path.normpath(file)
            # "Are you Local?"
			# Check if the files were imported from outside the Wog/Res folder
            fileext = os.path.splitext(file)[1][1:4]
            if fileext.lower()=="ogg":
                includesogg = True
            if file[:len(res_dir)]!=res_dir:
                all_local = False

        if not all_local and self.isReadOnly:
            self._importError = ["Cannot import external files...!","You cannot import external files into levels that come with OpenGOO.\nIf you really want to do this... Clone the level first!"]
            return []

        if not all_local:
            level_path = os.path.join( res_dir, STR_DIR_STUB, self.name )
            if not os.path.isdir( level_path ):
                os.mkdir( level_path )

            if includesogg:
                #' confirm / create import folder'
                music_path = os.path.join( res_dir, 'music', self.name )
                if not os.path.isdir( music_path ):
                    os.mkdir( music_path )

        localfiles = []
        resmap = {'png':('Image','IMAGE_',STR_DIR_STUB),'ogg':('Sound','SOUND_','music')}
        for file in importedfiles:
            # "Are you Local?"
            fileext = os.path.splitext(file)[1][1:4]
            if file[:len(res_dir)]!=res_dir:
                #@DaB - Ensure if the file is copied that it's new extension is always lower case
                fname = os.path.splitext(os.path.split( file )[1])[0]
                fileext = fileext.lower()
                newfile = os.path.join(res_dir,resmap[fileext][2],self.name,fname+"."+fileext)
                copy2(file, newfile)
                localfiles.append(newfile)
            else:
                #@DaB - File Extension Capitalization Check
                if fileext!=fileext.lower():
                    #Must be png or ogg to be compatible with LINUX and MAC
                    self._importError = ["File Extension CAPITALIZATION Warning!","To be compatible with Linux and Mac - All file extensions must be lower case.\nYou should rename the file below, and then import it again.\n\n" +file + " skipped!"]
                else:
                    localfiles.append(file)

        added_elements = []

        known_paths = {'Image':set(),'Sound':set()}
        for ext in resmap:
          for element in self.resource_root.findall( './/' + resmap[ext][0] ):
            path = os.path.normpath( os.path.splitext( element.get('path','').lower() )[0] )
            # known path are related to wog top dir in unix format & lower case without the file extension
            known_paths[resmap[ext][0]].add( path )
        for file in localfiles:
            file = file[len(game_dir)+1:] # makes path relative to wog top dir            print file
            filei = os.path.splitext(file)
            path = os.path.normpath( filei[0] ).lower()
            ext = filei[1][1:4]
            if path not in known_paths[resmap[ext][0]]:
                ALLOWED_CHARS = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789'
                pathbits = os.path.split( path )
                nextdir = os.path.split( pathbits[0])[1]
                resource_id = resmap[ext][1] + ''.join( c for c in nextdir.upper() if c in ALLOWED_CHARS ) + '_'+ ''.join( c for c in pathbits[1].upper() if c in ALLOWED_CHARS )
                resource_path = filei[0].replace("\\","/")
                meta_element = metawog.TREE_LEVEL_RESOURCE.find_element_meta_by_tag( resmap[ext][0] )
                new_resource = metaworld.Element( meta_element, {'id':resource_id.upper(),
                                                                 'path':resource_path} )
                resource_element.append( new_resource )
                added_elements.append( new_resource )
        return added_elements

class MainWindow(QtGui.QMainWindow):
    def __init__(self, parent=None):
        QtGui.QMainWindow.__init__(self, parent)
        self.setWindowIcon( QtGui.QIcon(":/images/icon.png" ))
        self.setAttribute( Qt.WA_DeleteOnClose )
        self.actionTimer = None
        self.statusTimer = None
        self._wog_path = None # Path to worl of goo executable
        self.recentfiles = None
        self.createMDIArea()
        self.createActions()
        self.createMenus()
        self.createToolBars()
        self.createStatusBar()
        self.createDockWindows()
        self.setWindowTitle(self.tr("OpenGOO Level Editor"))

        self._readSettings()

        self._game_model = None
        if self._wog_path:
	   #Check that the stored path is still valid
            if not os.path.exists(self._wog_path):
                self.changeWOGDir()
            else:
                self._reloadGameModel()
        else:
            # if wog_path is missing, prompt for it.
            self.changeWOGDir()

    def changeWOGDir(self):
        wog_path = QtGui.QFileDialog.getExistingDirectory(self,
             self.tr( 'Select the OpenGOO folder' ) )
##        wog_path =  QtGui.QFileDialog.getOpenFileName( self,
##             self.tr( 'Select WorldOfGoo program in the folder you want to edit' ),
##             r'',
##             self.tr( 'OpenGOO (OpenGOO*)' ) )
        if wog_path.isEmpty(): # user canceled action
            #wog_path="D:\World of Goo.app"
            return
        self._wog_path = os.path.normpath(unicode(wog_path))
        #print "_wog_path=",self._wog_path

        self._reloadGameModel()

    def _reloadGameModel( self ):
        try:
            self._game_model = GameModel( self._wog_path,self)
        except GameModelException, e:
            QtGui.QMessageBox.warning(self, self.tr("Loading OpenGOO levels ("+APP_NAME_PROPER+" "+CURRENT_VERSION+")"),
                                      unicode(e))
    def _updateRecentFiles(self):
        if self.recentFiles is None:
            numRecentFiles =0
        else:
            numRecentFiles = min(len(self.recentFiles),MAXRECENTFILES)
        for i in range(0,numRecentFiles):
            self.recentfile_actions[i].setText(self.recentFiles[i])
            self.recentfile_actions[i].setVisible(True)
        for i in range(numRecentFiles,MAXRECENTFILES):
            self.recentfile_actions[i].setVisible(False)
        self.separatorRecent.setVisible(numRecentFiles > 0);

    def _setRecentFile(self,filename):
        self.recentFiles.removeAll(filename)
        self.recentFiles.prepend(filename)
        if len(self.recentFiles) > MAXRECENTFILES:
           self.recentFiles = self.recentFiles[:MAXRECENTFILES]
        self._updateRecentFiles()

    def on_recentfile_action(self):
        action = self.sender()
        name = unicode(action.text())
        if self.open_level_view_by_name( name ):
            self._setRecentFile( name )

    def editLevel( self ):
        if self._game_model:
            dialog = QtGui.QDialog()
            ui = editleveldialog.Ui_EditLevelDialog()
            ui.setupUi( dialog , set(self._game_model.names), metawog.LEVELS_ORIGINAL)
            if dialog.exec_() and ui.levelList.currentItem:
                settings = QtCore.QSettings()
                settings.beginGroup( "MainWindow" )
                settings.setValue( "level_filter", ui.comboBox.currentIndex())
                settings.endGroup( )
                name = unicode( ui.levelList.currentItem().text() )
                if self.open_level_view_by_name( name ):
                    self._setRecentFile( name )

    def open_level_view_by_name( self, name ):
        try:
            world = self._game_model.selectLevel( name )
        except GameModelException, e:
            QtGui.QMessageBox.warning(self, self.tr("Failed to load level! ("+APP_NAME_PROPER+" "+CURRENT_VERSION+")"),
                      unicode(e))
        else:
            sub_window = self._findWorldMDIView( world )
            if sub_window:
                self.mdiArea.setActiveSubWindow( sub_window )
            else:
                self._addGraphicView( world )
            return True
        return False

    def _addGraphicView( self, world ):
        """Adds a new MDI GraphicView window for the specified level."""
        level_view = levelview.LevelGraphicView( world, self.view_actions, self.common_actions )
        sub_window = self.mdiArea.addSubWindow( level_view )
        self.connect( level_view, QtCore.SIGNAL('mouseMovedInScene(PyQt_PyObject,PyQt_PyObject)'),
                      self._updateMouseScenePosInStatusBar )
        self.connect( sub_window, QtCore.SIGNAL('aboutToActivate()'),
                      level_view.selectLevelOnSubWindowActivation )
	world.set_selection(world.scene_root)
        world.setView(level_view)
        level_view.show()

    def _updateMouseScenePosInStatusBar( self, x, y ):
        """Called whenever the mouse move in the LevelView."""
		# Round displayed coordinate to 2dp (0.01)
        x = round(x,2)
        y = -round(y,2) # Reverse transformation done when mapping to scene (in Qt 0 = top, in WOG 0 = bottom)
        self._mousePositionLabel.setText( self.tr('x: %1 y: %2').arg(x).arg(y) )

    def _findWorldMDIView( self, world ):
        """Search for an existing MDI window for level name.
           Return the LevelGraphicView widget, or None if not found."""
        for window in self.mdiArea.subWindowList():
            sub_window = window.widget()
            if sub_window.world == world:
                return window
        return None

    def get_active_view(self):
        """Returns the view of the active MDI window.
           Returns None if no view is active.
        """
        window = self.mdiArea.activeSubWindow()
        if window:
            return window.widget()
        return None

    def getCurrentModel( self ):
        """Returns the level model of the active MDI window."""
        window = self.mdiArea.activeSubWindow()
        if window:
            return window.widget().getModel()
        return None

        #@DaB - New save routines to save ONLY the current Level

    def saveCurrent(self):
       if self._game_model:
            model = self.getCurrentModel()
            if model is not None:
                if model.isReadOnly:
                    if model.is_dirty:
                        QtGui.QMessageBox.warning(self, self.tr("Can not save OpenGOO standard levels!"),
                              self.tr('You can not save changes made to levels that come with OpenGOO.\n'
                                      'Instead, clone the level using the "Clone selected level" tool.\n'
                                      'Do so now, or your change will be lost once you quit the editor' ) )
                        return False
                    return True
                else:
                    #Check for issues
                    try:
                        model.saveModifiedElements()
                        self.statusBar().showMessage(self.tr("Saved " + model.name), 2000)
                        return True
                    except (IOError,OSError), e:
                        QtGui.QMessageBox.warning(self, self.tr("Failed saving levels ("+APP_NAME_PROPER+" "+CURRENT_VERSION+")"), unicode(e))

       return False

    def saveIT(self):
       if self.saveCurrent():
         QtGui.QApplication.setOverrideCursor(Qt.WaitCursor)
         model = self.getCurrentModel()
         issue_level = model.hasIssues()
         QtGui.QApplication.restoreOverrideCursor()
         if issue_level>=ISSUE_LEVEL_WARNING:
            txtIssue = self.tr("""<p>There are unresolved issues with this level that may cause problems.<br>
                                    You should fix these before you try to play or make a goomod.</p>""")
            txtIssue = txtIssue + self.tr(model.getIssues())
            txtIssue = txtIssue + self.tr( '<br>The level has been saved!')
            QtGui.QMessageBox.warning(self, self.tr("This level has issues!"),
                  txtIssue )

    def saveAndPlayLevel(self):
		#@DaB only save current level, and don't "play" if it has "Issues"
        if self.saveCurrent(): #returns false if there are issues
            model = self.getCurrentModel()
            if model:
                issue_level = model.hasIssues()
                if issue_level>=ISSUE_LEVEL_CRITICAL:
                    txtIssue = self.tr("""<p>There are CRITICAL issues with this level that will cause World of Goo to crash.<br>
                                       You must fix these before you try to play the level.</p>""")
                    txtIssue = txtIssue + self.tr(model.getIssues())
                    txtIssue = txtIssue + self.tr( '<br>The level has been saved!')
                    QtGui.QMessageBox.warning(self, self.tr("This level has CRITICAL issues!"),
                          txtIssue )
                elif issue_level>ISSUE_LEVEL_NONE:
                    txtIssue = self.tr("""<p>There are Advice/Warnings for this level that may cause problems.<br>
                                        You should fix these before you try to play the level.</p>""")
                    txtIssue = txtIssue + self.tr(model.getIssues())
                    txtIssue = txtIssue + self.tr( '<br>Click OK to Play anyway, or click Cancel to go back.')
                    ret = QtGui.QMessageBox.warning(self, self.tr("This level has warnings!"),
                        txtIssue,QtGui.QMessageBox.Ok | QtGui.QMessageBox.Cancel )
                    if ret==QtGui.QMessageBox.Ok:
                        self._game_model.playLevel( model )
                else:
                    self._game_model.playLevel( model )
            else:
                self.statusBar().showMessage(self.tr("You must select a level to play"), 2000)

    def makegoomod(self):
      if self.saveCurrent(): #returns false if it failed
        if self._game_model:
            model = self.getCurrentModel()
            if model is not None:
                self.statusBar().showMessage(self.tr("Checking for Issues : " + model.name), 2000)
                QtGui.QApplication.setOverrideCursor(Qt.WaitCursor)
                issue_level = model.hasIssues()
                QtGui.QApplication.restoreOverrideCursor()

                if issue_level==ISSUE_LEVEL_ADVICE:
                    txtIssue = self.tr("""<p>There are Advice issues for this level.<br>
                               You should probably fix these before you upload the goomod</p>""")
                    txtIssue = txtIssue + self.tr(model.getIssues())
                    txtIssue = txtIssue + self.tr( '<br>Click OK to make the goomod anyway, or click Cancel to go back.')
                    ret = QtGui.QMessageBox.warning(self, self.tr("This level has advice!"),
                        txtIssue,QtGui.QMessageBox.Ok | QtGui.QMessageBox.Cancel )
                    if ret==QtGui.QMessageBox.Cancel:
                        return False
                elif issue_level>=ISSUE_LEVEL_WARNING:
                        txtIssue = self.tr("""<p>There are unresolved issues with this level.<br>
                                           You must fix these before you can produce a .goomod</p>""")
                        txtIssue = txtIssue + self.tr(model.getIssues())
                        QtGui.QMessageBox.warning(self, self.tr("This level has issues!"), txtIssue )
                        return False

                if model.hasAddinIssues():
                        txtIssue = self.tr("""<p>There are unresolved issues with the Addin info.<br>
                                                You must fix these before you can produce a .goomod</p>""")
                        txtIssue = txtIssue + self.tr(model.getAddinIssues())
                        QtGui.QMessageBox.warning(self, self.tr("This level has Addin issues!"), txtIssue )
                        return False

                include_all_dep = False
                if model.hasDependancies():
                    # level has some dependancies (might just be custom gfx or Sounds
                    if model.hasdependancy_issue():
                        # has more deps than just GFX or Sound (Balls or Materials or Particles)
                        if model._dependancy_issue_level>ISSUE_LEVEL_ADVICE:
                            #Theres a problem and we can't build the goomod
                            txtIssue = self.tr("""<p>You must fix these before you can produce a .goomod</p>""")
                            txtIssue+= model.dependancy_issue_report
                            QtGui.QMessageBox.warning(self, self.tr("This level has dependancy issues!"), txtIssue )
                            return False
                        else:
                            #There's no "problem" we need just to ask if they want to localize all
                            txtIssue = self.tr("""<p>This level depends on several custom resources.</p>""")
                            txtIssue+= model.dependancy_issue_report
                            txtIssue+= self.tr('<br>Would you like to include ALL these resources in the goomod?')
                            ret = QtGui.QMessageBox.warning(self, self.tr("This level has dependancies!"),
                                txtIssue,QtGui.QMessageBox.Yes|QtGui.QMessageBox.No | QtGui.QMessageBox.Cancel )
                            if ret==QtGui.QMessageBox.Cancel:
                                return False
                            elif ret==QtGui.QMessageBox.Yes:
                                include_all_dep = True

                #maybe actually think about doing it!
                #ask here instead?
                dir = os.path.normpath(os.path.join( os.path.split( self._wog_path )[0],'res', STR_DIR_STUB, model.name ))
                goomod_dir = os.path.join(dir,u'goomod')
                if not os.path.isdir( goomod_dir ):
                    os.mkdir( goomod_dir )
                id_element = model.addin_root.find('id')

                if self._goomod_path=='':
                    goomod_filename = os.path.join(goomod_dir,id_element.text) + ".goomod"
                else:
                    goomod_filename = os.path.join(self._goomod_path,id_element.text) + ".goomod"

                filename = QtGui.QFileDialog.getSaveFileName(self, self.tr("Save goomod File"),
                        goomod_filename,
                        self.tr("goomod (*.goomod)"));
                if filename!='':
                    self.statusBar().showMessage(self.tr("Creating goomod : " + model.name), 2000)

                    filename = os.path.normpath(str(filename))
                    if filename[:len(dir)] == dir:
                        self._goomod_path = u''
                    else:
                        self._goomod_path = os.path.split(filename)[0]
                    self._game_model.creategoomod(model.name,filename,include_deps = include_all_dep)
                    self.statusBar().showMessage(self.tr("goomod Created: " + filename), 2000)

      return False

    def newLevel( self ):
        """Creates a new blank level."""
        new_name = self._pickNewName( is_cloning = False )
        if new_name:
            try:
                self._game_model.newLevel( new_name )
                world = self._game_model.selectLevel( new_name )
                self._addGraphicView( world )
            except (IOError,OSError), e:
                QtGui.QMessageBox.warning(self, self.tr("Failed to create the new level! ("+APP_NAME_PROPER+" "+CURRENT_VERSION+")"),
                                          unicode(e))

    def _pickNewName( self, is_cloning = False ):
        if self._game_model:
            dialog = QtGui.QDialog()
            ui = newleveldialog_ui.Ui_NewLevelDialog()
            ui.setupUi( dialog )
            reg_ex = QtCore.QRegExp( '[A-Za-z][0-9A-Za-z][0-9A-Za-z]+' )
            validator = QtGui.QRegExpValidator( reg_ex, dialog )
            ui.levelName.setValidator( validator )
            if is_cloning:
                dialog.setWindowTitle(tr("NewLevelDialog", "Cloning Level"))

            if dialog.exec_():
                new_name = str(ui.levelName.text())
                existing_names = [name.lower() for name in self._game_model.names]
                if new_name.lower() not in existing_names:
                    return new_name
                QtGui.QMessageBox.warning(self, self.tr("Can not create level!"),
                    self.tr("There is already a level named '%1'").arg(new_name))
        return None

    def cloneLevel( self ):
        """Clone the selected level."""
        current_model = self.getCurrentModel()
        if current_model:
            new_name = self._pickNewName( is_cloning = True )
            if new_name:
                try:
                    self._game_model.cloneLevel( current_model.name, new_name )
                    world = self._game_model.selectLevel( new_name )
                    self._addGraphicView( world )
                    self._setRecentFile( new_name )
                except (IOError,OSError), e:
                    QtGui.QMessageBox.warning(self, self.tr("Failed to create the new cloned level! ("+APP_NAME_PROPER+" "+CURRENT_VERSION+")"),unicode(e))



    def updateResources( self ):
        """Adds the required resource in the level based on existing file."""
        model = self.getCurrentModel()
        if model:
            model.game_model.pixmap_cache.refresh()
            added_resource_elements = model.updateResources()
            if added_resource_elements:
                model.set_selection( added_resource_elements )
            model._view.refreshFromModel()

    def setMusic(self):
        model = self.getCurrentModel()
        if model:
            #game_dir = os.path.normpath( os.path.split( self._wog_path )[0] )
            #res_dir =  os.path.join( game_dir, 'res' )
            music_dir = os.path.join( self._game_model._res_dir, 'music' )
            file =  QtGui.QFileDialog.getOpenFileName( self,
                        self.tr( 'Select the file to use for music...' ),
                        music_dir,
                        self.tr( 'Sound File (*.ogg)' ) )

            if file=='': # user canceled action
                return
            safefiles = []
            safefiles.append( os.path.normpath(str(file)))

            added_resource_elements = model.importResources(safefiles,self._game_model._res_dir)
            if added_resource_elements:
                music_id = added_resource_elements[0].get('id')
                music_element=model.level_root.find('music')
                if music_element is not None:
                    old_music_id = music_element.get('sound')
                    music_element.set('sound',music_id)
                    if old_music_id is not None:
                        if old_music_id!='':
                           for soundelement in model.resource_root.findall('.//Sound'):
                               if soundelement.get('id')==old_music_id:
                                   soundelement.parent.remove( soundelement )
                                   break

                else:
                    AddItemFactory(self, 'level','music',{'sound':music_id})()
            else:
                ie = model.importError()
                if ie is not None:
                    QtGui.QMessageBox.warning(self, self.tr(ie[0]),
                              self.tr(ie[1] ) )

    def setLoopSound(self):
        model = self.getCurrentModel()
        if model:
            #game_dir = os.path.normpath( os.path.split( self._wog_path )[0] )
            #res_dir =  os.path.join( game_dir, 'res' )
            sound_dir = os.path.join( self._game_model._res_dir, 'sounds' )
            file =  QtGui.QFileDialog.getOpenFileName( self,
                        self.tr( 'Select the file to use for LoopSound...' ),
                        sound_dir,
                        self.tr( 'Sound File (*.ogg)' ) )

            if file=='': # user canceled action
                return
            safefiles = []
            safefiles.append( os.path.normpath(str(file)))

            added_resource_elements = model.importResources(safefiles,self._game_model._res_dir)
            if added_resource_elements:
                loopsound_id = added_resource_elements[0].get('id')
                loopsound_element=model.level_root.find('loopsound')
                if loopsound_element is not None:
                    old_loopsound_id = loopsound_element.get('sound')
                    loopsound_element.set('sound',loopsound_id)
                    if old_loopsound_id is not None:
                        if old_loopsound_id!='':
                           for soundelement in model.resource_root.findall('.//Sound'):
                               if soundelement.get('id')==old_loopsound_id:
                                   soundelement.parent.remove( soundelement )
                                   break
                else:
                    AddItemFactory(self, 'level','loopsound',{'sound':loopsound_id})()
            else:
                ie = model.importError()
                if ie is not None:
                    QtGui.QMessageBox.warning(self, self.tr(ie[0]),
                              self.tr(ie[1] ) )
    def cleanResources(self):
        model = self.getCurrentModel()
        if model:
            unused = model._get_unused_resources()
            unusedlist = ''
            for family,unusedset in unused.items():
                for id in unusedset:
                    unusedlist+=id+'\n'
            if unusedlist!='':
                unusedlist="The following resources are unused\n"+unusedlist+"\nAre you sure you want to remove them?"
                ret = QtGui.QMessageBox.warning(self, self.tr("Remove unused resources"),
                        unusedlist,QtGui.QMessageBox.Ok | QtGui.QMessageBox.Cancel )
                if ret==QtGui.QMessageBox.Ok:
                    model._remove_unused_resources(unused)

            else:
                 QtGui.QMessageBox.warning(self, self.tr("Remove unused resources"),
                        self.tr("There are no unused resources\n") )



    def importResources( self ):
        """Adds the required resource in the level based on existing file."""
        model = self.getCurrentModel()
        if model:
            #game_dir = os.path.normpath( os.path.split( self._wog_path )[0] )
            #res_dir =  os.path.join( game_dir, 'res' )
            dir = os.path.join( self._game_model._res_dir, STR_DIR_STUB )
            files =  QtGui.QFileDialog.getOpenFileNames( self,
                        self.tr( 'Select the Images to import...' ),
                        dir,
                        self.tr( 'Images (*.png)' ) )

            if files.isEmpty(): # user canceled action
                return
            safefiles = []
            for file in files:
                safefiles.append( os.path.normpath(str(file)))

            added_resource_elements = model.importResources(safefiles,self._game_model._res_dir)
            if added_resource_elements:
                model.set_selection( added_resource_elements )
            else:
                ie = model.importError()
                if ie is not None:
                    QtGui.QMessageBox.warning(self, self.tr(ie[0]),
                              self.tr(ie[1] ) )

    def autoPipe(self):
        model = self.getCurrentModel()
        if model:
            exit_element = model.level_root.find('levelexit')
            if exit_element is None:
               QtGui.QMessageBox.warning(self, self.tr("Level Exit Required!"),
                              self.tr('You cannot use Auto Pipe without a Level Exit\n'
                                      'Add and position an exit, and try Auto Pipe again.' ) )
               return
            exit_posx, exit_posy = exit_element.get_native('pos')
            minx,maxx = model.scene_root.get_native('minx'),model.scene_root.get_native('maxx')
            miny,maxy = model.scene_root.get_native('miny'),model.scene_root.get_native('maxy')
            #find closest "wall"
            if exit_posx > maxx or exit_posx<minx or exit_posy>maxy or exit_posy<miny:
                # exit outside scene bounds warning
               QtGui.QMessageBox.warning(self, self.tr("Level Exit Outside Scene Bounds!"),
                              self.tr('The Level Exit is outside the limits of the scene\n'
                                      'You should correct this, by moving the exit or changing the scene bounds.' ) )
               return

            pipe_element = model.level_root.find('pipe')
            if pipe_element is None:
                # add one
                pipe_meta = model.level_root.meta.find_immediate_child_by_tag('pipe')
                attrib = {}
                pipe_element= _appendChildTag(model.level_root,pipe_meta,attrib)
            else:
                ret =  QtGui.QMessageBox.warning(self, self.tr("Existing Pipe will be reset!"),
                              self.tr('Auto Pipe will replace the current pipe with a simple default pipe.\n\n'
                                      'Are you sure you want to do this?' ),QtGui.QMessageBox.Ok | QtGui.QMessageBox.Cancel )
                if ret==QtGui.QMessageBox.Cancel:
                    return
                #remove all existing vertexes
                for vertex in pipe_element.findall('Vertex'):
                    pipe_element.remove(vertex)

            attrib={}
            vertex_meta = pipe_element.meta.find_immediate_child_by_tag('Vertex')
            attrib['pos']= exit_element.get('pos')
            _appendChildTag(pipe_element,vertex_meta,attrib)

            #get scene bounds...
            dx1,dx2 = abs(minx-exit_posx),abs(maxx-exit_posx)
            if dx1<dx2:
                dx = dx1
                xd = minx - 100
            else:
                dx = dx2
                xd = maxx + 100

            dy1,dy2 = abs(miny-exit_posy),abs(maxy-exit_posy)
            if dy1<dy2:
                dy = dy1
                yd = miny - 100
            else:
                dy = dy2
                yd = maxy + 100
            if dx<dy:
                v2pos_x = xd
                v2pos_y = exit_posy
            else:
                v2pos_x = exit_posx
                v2pos_y = yd

            attrib={}
            attrib['pos']= str(v2pos_x)+","+str(v2pos_y)
            _appendChildTag(pipe_element,vertex_meta,attrib)



    def addPipeVertex(self):
        model = self.getCurrentModel()
        if model:
          window = self.mdiArea.activeSubWindow()
          if window:
            cview = window.widget()
            cp = cview.mapToScene(cview.width()*0.5,cview.height()*0.5)
            attrib={}
            pipe_element = model.level_root.find('pipe')
            if pipe_element is not None:
                vertex_meta = pipe_element.meta.find_immediate_child_by_tag('Vertex')
                for attribute_meta in vertex_meta.attributes:
                    if attribute_meta.type == metaworld.XY_TYPE:
                        attrib[attribute_meta.name]= str(cp.x())+","+str(-cp.y())
                        break
                _appendChildTag(pipe_element,vertex_meta,attrib)


    def about(self):
        QtGui.QMessageBox.about(self, self.tr("About OpenGOO Level Editor " + CURRENT_VERSION),
            self.tr("""<p>OpenGOO Level Editor <b>(OGLE)</b> helps you create new levels for OpenGOO.<p>
            <p>Based on WooGLE 077 for World of Goo</p>
            <p>Download Page:<br>
            <a href="http://goofans.com/download/utility/world-of-goo-level-editor">http://goofans.com/download/utility/world-of-goo-level-editor</a></p>
            <p>FAQ, Tutorial and Reference Guide:<br>
            <a href="http://goofans.com/developers/world-of-goo-level-editor">http://goofans.com/developers/world-of-goo-level-editor</a></p>
            <p>Copyright 2013-, The OpenGOO Team</p>
            <p>Copyright 2010-, DaftasBrush</p>
            <p>&nbsp;<br>Original Sourceforge project: (v0.5)
            <a href="http://www.sourceforge.net/projects/wogedit">http://www.sourceforge.net/projects/wogedit</a><br>
            Copyright 2008-2009, NitroZark &lt;nitrozark at users.sourceforget.net&gt;</p>"""))

    def on_cut_action(self):
        elements = self.on_copy_action( is_cut_action=True )
        if elements:
            for element in elements:
                if element.meta.read_only:
                    #Messagebox
                    QtGui.QMessageBox.warning(self, self.tr("Cannot Cut read only element!"),
                              self.tr('This element is read only.\n'
                                      'It cannot be cut' ) )
                    return
            self.on_delete_action( is_cut_action=True )
            self.statusBar().showMessage(
                self.tr('Element "%s" cut to clipboard' %
                        elements[0].tag), 1000 )

    def on_copy_action(self, is_cut_action = False):
        world = self.getCurrentModel()
        if world:
            elements = list(world.selected_elements)
            on_clipboard = set()
            clipboard_element = xml.etree.ElementTree._ElementInterface('WooGLEClipboard',{})
            for element in elements:
                on_clipboard.add(element.tag)
                xml_data= element.to_xml_with_meta()
                clipboard_element.append(xml.etree.ElementTree.fromstring(xml_data))
            clipboard = QtGui.QApplication.clipboard()
            if len(on_clipboard)==1:
                clipboard_element.set('type', list(on_clipboard)[0])
            else:
                clipboard_element.set('type',"Various")
            scene = self.get_active_view().scene()
            # bounding rect of selected items
            i=0
            for item in scene.selectedItems():
                if i==0:
                    brect = item.mapToScene(item.boundingRect()).boundingRect()
                    mybrect=[brect.left(),brect.right(),brect.bottom(),brect.top()]
                else:
                    brect = item.mapToScene(item.boundingRect()).boundingRect()
                    if brect.left()<mybrect[0]:
                        mybrect[0]=brect.left()
                    if brect.right()>mybrect[1]:
                        mybrect[1]=brect.right()
                    if brect.bottom()<mybrect[2]:
                        mybrect[2]=brect.bottom()
                    if brect.top()>mybrect[3]:
                        mybrect[3]=brect.top()
                i+=1

            clipboard_element.set('posx',str((mybrect[0]+mybrect[1])*0.5))
            clipboard_element.set('posy',str(-(mybrect[2]+mybrect[3])*0.5))
            xml_data =  xml.etree.ElementTree.tostring(clipboard_element,'utf-8')
            clipboard.setText( xml_data )
            if not is_cut_action:
                self.statusBar().showMessage(
                    self.tr('%d Element "%s" copied to clipboard' %
                            (len(elements),clipboard_element.get('type'))), 1000 )
            self.common_actions['paste'].setText("Paste In Place ("+clipboard_element.get('type')+")")
            self.common_actions['pastehere'].setText("Paste Here ("+clipboard_element.get('type')+")")
            return elements


    def on_pasteto_action(self):
        clipboard = QtGui.QApplication.clipboard()
        xml_data = unicode(clipboard.text())
        world = self.getCurrentModel()
        if world is None or not xml_data:
            return
        clipboard_element = xml.etree.ElementTree.fromstring(xml_data)
        view = self.get_active_view()
        paste_posx,paste_posy = view._last_pos.x(),-view._last_pos.y()
        copy_posx,copy_posy = float(clipboard_element.get('posx',0)),float(clipboard_element.get('posy',0))
        pasted_elements = []
        for clip_child in clipboard_element.getchildren():
          xml_data = xml.etree.ElementTree.tostring(clip_child,'utf-8')
          for element in [tree.root for tree in world.trees]:
                child_elements = element.make_detached_child_from_xml( xml_data )
                if child_elements:
                    pasted_elements.extend(child_elements)
                    for child_element in child_elements:
                        # find the pos attribute in the meta
                        # set it to view._last_release_at
                        pos_attribute = self._getPositionAttribute(child_element)
                        imagepos_attribute = self._getImageposAttribute(child_element)
                        if pos_attribute is not None:
                            old_pos = pos_attribute.get_native(child_element,(0,0))
                            if clipboard_element.__len__()==1:
                                pos_attribute.set_native(child_element,[view._last_pos.x(),-view._last_pos.y()])
                            else:
                                pos_attribute.set_native(child_element,[old_pos[0]+paste_posx-copy_posx,old_pos[1]+paste_posy-copy_posy])

                            if imagepos_attribute is not None:
                                old_imagepos = imagepos_attribute.get_native(child_element,None)
                                if old_imagepos is not None:
                                    if clipboard_element.__len__()==1:
                                        new_posx = old_imagepos[0]+view._last_pos.x()-old_pos[0]
                                        new_posy = old_imagepos[1]-view._last_pos.y()-old_pos[1]
                                    else:
                                        new_posx = old_imagepos[0]+paste_posx-copy_posx
                                        new_posy = old_imagepos[1]+paste_posy-copy_posy
                                    imagepos_attribute.set_native(child_element,[new_posx,new_posy])

                        element.safe_identifier_insert( len(element), child_element )
                    break
        if len(pasted_elements)>=1:
            world.set_selection( pasted_elements )


    def _getPositionAttribute(self,element):
        for attribute_meta in element.meta.attributes:
            if attribute_meta.type == metaworld.XY_TYPE:
                if attribute_meta.position:
                    return attribute_meta
        return None

    def _getImageposAttribute(self,element):
        image= element.get('image',None)
        if image is None:
            return None
        for attribute_meta in element.meta.attributes:
            if attribute_meta.type == metaworld.XY_TYPE:
                if attribute_meta.name=="imagepos":
                    return attribute_meta
        return None

    def on_paste_action(self):
        clipboard = QtGui.QApplication.clipboard()
        xml_data = unicode(clipboard.text())
        world = self.getCurrentModel()
        if world is None or not xml_data:
            return
        elements = list(world.selected_elements)
        if len(elements) == 0: # Allow pasting to root when no selection
            elements = [tree.root for tree in world.trees]
        # Try to paste in one of the selected elements. Stop when succeed
        clipboard_element = xml.etree.ElementTree.fromstring(xml_data)
        pasted_elements = []
        for clip_child in clipboard_element.getchildren():
          xml_data = xml.etree.ElementTree.tostring(clip_child,'utf-8')
          for element in elements:
             while element is not None:
                child_elements = element.make_detached_child_from_xml( xml_data )
                if child_elements:
                    for child_element in child_elements:
                        element.safe_identifier_insert( len(element), child_element )
                    pasted_elements.extend(child_elements)
                    break
                element = element.parent
        if len(pasted_elements)>=1:
            element.world.set_selection( pasted_elements )

    def on_undo_action(self):
        world = self.getCurrentModel()
        if world is None:
            return
        world.undo()

    def on_redo_action(self):
        world = self.getCurrentModel()
        if world is None:
            return
        world.redo()


    def on_delete_action(self, is_cut_action = False):
        world = self.getCurrentModel()
        if world is None:
            return
        deleted_elements = []
        previous_element = None
        for element in list(world.selected_elements):
            if element.meta.read_only:
                #messagebox
                QtGui.QMessageBox.warning(self, self.tr("Cannot delete read only element!"),
                              self.tr('This element is read only.\n'
                                      'It cannot be deleted' ) )

                return 0
            elif not element.is_root():
                if element.previous_element() not in list(world.selected_elements):
                    previous_element = element.previous_element()

                deleted_elements.append( element.tag )
                element.parent.remove( element )

        if is_cut_action:
            return len(deleted_elements)
        if deleted_elements:
            self.statusBar().showMessage(
                self.tr('Deleted %d element(s)' % len(deleted_elements)), 1000 )
            world.set_selection( previous_element )

    def _on_view_tool_actived( self, tool_name ):
        active_view = self.get_active_view()
        if active_view is not None:
            active_view.tool_activated( tool_name )

    def on_select_tool_action(self):
        self._on_view_tool_actived( levelview.TOOL_SELECT )

    def on_pan_tool_action(self):
        self._on_view_tool_actived( levelview.TOOL_PAN )

    def on_move_tool_action(self):
        self._on_view_tool_actived( levelview.TOOL_MOVE )

    def onRefreshAction( self ):
        """Called multiple time per second. Used to refresh enabled flags of actions."""
        has_wog_dir = self._game_model is not None
        #@DaB - Now that save and "save and play" only act on the
		# current level it's better if that toolbars buttons
 		# change state based on the current level, rather than all levels
	currentModel = self.getCurrentModel()
        is_selected = currentModel is not None
        can_select = is_selected and self.view_actions[levelview.TOOL_MOVE].isChecked()

        if is_selected:
            can_save = has_wog_dir and currentModel.is_dirty
            element_is_selected = can_select and len(currentModel.selected_elements)>0
            can_import = is_selected and not currentModel.isReadOnly
            can_undo = currentModel.can_undo
            can_redo = currentModel.can_redo
            if currentModel.is_dirty:
                if currentModel.isReadOnly:
                    self.mdiArea.activeSubWindow().setWindowIcon(  QtGui.QIcon ( ':/images/nosave.png' ) )
                else:
                    self.mdiArea.activeSubWindow().setWindowIcon(  QtGui.QIcon ( ':/images/dirty.png' ) )
            else:
                self.mdiArea.activeSubWindow().setWindowIcon(  QtGui.QIcon ( ':/images/clean.png' ) )
        else:
            can_save = False
            element_is_selected = False
            can_import = False
            can_undo = False
            can_redo = False


        self.editLevelAction.setEnabled( has_wog_dir )
        self.newLevelAction.setEnabled( has_wog_dir )
        self.cloneLevelAction.setEnabled( is_selected )
        self.saveAction.setEnabled( can_save and True or False )
        self.playAction.setEnabled( is_selected )
        self.goomodAction.setEnabled (can_import)

        #Edit Menu / ToolBar

        self.common_actions['cut'].setEnabled (element_is_selected)
        self.common_actions['copy'].setEnabled (element_is_selected)
        self.common_actions['paste'].setEnabled (is_selected)
        self.common_actions['delete'].setEnabled (element_is_selected)
        self.undoAction.setEnabled (can_undo)
        self.redoAction.setEnabled (can_redo)

        #Resources
        self.importResourcesAction.setEnabled (can_import  )
        self.cleanResourcesAction.setEnabled (can_import  )
        self.updateResourcesAction.setEnabled( can_import )
        self.additem_actions['text'].setEnabled (can_import)
        self.setMusicAction.setEnabled( can_import )
        self.setLoopSoundAction.setEnabled( can_import )


        self.addItemToolBar.setEnabled( can_select )
        self.addGooToolBar.setEnabled( can_select )

        self.showhideToolBar.setEnabled( is_selected )


        active_view = self.get_active_view()
        enabled_view_tools = set()
        if active_view:
            enabled_view_tools = active_view.get_enabled_view_tools()
        for name, action in self.view_actions.iteritems():
            is_enabled = name in enabled_view_tools
            action.setEnabled( is_enabled )
        if self.view_action_group.checkedAction() is None:
            self.view_actions[levelview.TOOL_MOVE].setChecked( True )

    def _on_refresh_element_status(self):
        # broadcast the event to all ElementIssueTracker
        louie.send_minimal( metaworldui.RefreshElementIssues )

    def createMDIArea( self ):
        self.mdiArea = QtGui.QMdiArea()
        self.mdiArea.setViewMode(QtGui.QMdiArea.TabbedView)
        for thing in self.mdiArea.findChildren(QtGui.QTabBar):
            thing.setTabsClosable(True)
            self.connect ( thing, QtCore.SIGNAL("tabCloseRequested(int)"), self.on_closeTab )
        self.setCentralWidget(self.mdiArea)

    def on_closeTab(self,index):
        sub = self.mdiArea.subWindowList()[index]
        sub.close()

    def createActions(self):
        self.changeWOGDirAction = qthelper.action( self, handler = self.changeWOGDir,
            icon = ":/images/open.png",
            text = "&Change OpenGOO directory...",
            shortcut = QtGui.QKeySequence.Open,
            status_tip = "Change OpenGOO top-directory" )

        self.editLevelAction = qthelper.action( self, handler = self.editLevel,
            icon = ":/images/icon-wog-level.png",
            text = "&Edit existing level...",
            shortcut = "Ctrl+L",
            status_tip = "Select a level to edit" )

        self.newLevelAction = qthelper.action(self, handler = self.newLevel,
            icon = ":/images/icon-wog-new-level2.png",
            text = "&New level...",
            shortcut = QtGui.QKeySequence.New,
            status_tip = "Creates a new level" )

        self.cloneLevelAction = qthelper.action( self, handler = self.cloneLevel,
            icon = ":/images/icon-wog-clone-level.png",
            text = "&Clone selected level...",
            shortcut = "Ctrl+D",
            status_tip = "Clone the selected level" )

        self.saveAction = qthelper.action( self, handler = self.saveIT,
            icon = ":/images/save.png",
            text = "&Save...",
            shortcut = QtGui.QKeySequence.Save,
            status_tip = "Saves the Level" )

        self.playAction = qthelper.action( self, handler = self.saveAndPlayLevel,
            icon = ":/images/play.png",
            text = "&Save and play Level...",
            shortcut = "Ctrl+P",
            status_tip = "Save and play the selected level" )

        self.goomodAction = qthelper.action( self, handler = self.makegoomod,
            icon = ":/images/goomod.png",
            text = "&Make .goomod",
            shortcut = "Ctrl+G",
            status_tip = "Make a .goomod file for this level" )


        self.updateResourcesAction = qthelper.action( self,
            handler = self.updateResources,
            icon = ":/images/update-level-resources.png",
            text = "&Update level resources...",
            shortcut = "Ctrl+U",
            status_tip = "Adds automatically all .png & .ogg files in the level directory to the level resources" )

        self.cleanResourcesAction = qthelper.action( self,
            handler = self.cleanResources,
            icon = ":/images/cleanres.png",
            text = "&Clean Resources",
            status_tip = "Removes any unused resource from the level." )

        self.importResourcesAction = qthelper.action( self,
            handler = self.importResources,
            icon = ":/images/importres.png",
            text = "&Import images...",
            shortcut = "Ctrl+I",
            status_tip = "Adds images (png) to the level resources" )

        self.setMusicAction = qthelper.action( self,
            handler = self.setMusic,
            icon = ":/images/music.png",
            text = "&Set Music...",
            shortcut = "Ctrl+M",
            status_tip = "Select the ogg file you want as the Music" )

        self.setLoopSoundAction = qthelper.action( self,
            handler = self.setLoopSound,
            icon = ":/images/loopsound.png",
            text = "&Set Loop Sound...",
            shortcut = "Ctrl+L",
            status_tip = "Select the ogg file you want as the Loop Sound" )

        self.quitAct = qthelper.action( self, handler = self.close,
            text = "&Quit",
            shortcut = "Ctrl+Q",
            status_tip = "Quit the application" )

        self.aboutAct = qthelper.action( self, handler = self.about,
            icon = ":/images/icon.png",
            text = "&About",
            status_tip = "Show the application's About box" )

        self.recentfile_actions = [qthelper.action( self, handler = self.on_recentfile_action, visible=False)
                                    for i in range(0,MAXRECENTFILES)]

        self.common_actions = {
            'cut': qthelper.action( self, handler = self.on_cut_action,
                    icon = ":/images/cut.png",
                    text = "Cu&t",
                    shortcut = QtGui.QKeySequence.Cut ),
            'copy': qthelper.action( self, handler = self.on_copy_action,
                    icon = ":/images/copy.png",
                    text = "&Copy",
                    shortcut = QtGui.QKeySequence.Copy ),
            'paste': qthelper.action( self, handler = self.on_paste_action,
                    icon = ":/images/paste.png",
                    text = "Paste &In Place",
                    shortcut ="Ctrl+Shift+V" ),
            'pastehere': qthelper.action( self, handler = self.on_pasteto_action,
                    icon = ":/images/paste.png",
                    text = "&Paste Here",shortcut = QtGui.QKeySequence.Paste ),

            'delete': qthelper.action( self, handler = self.on_delete_action,
                    icon = ":/images/delete.png",
                    text = "&Delete",
                    shortcut = QtGui.QKeySequence.Delete )

        }
        self.undoAction = qthelper.action( self, handler = self.on_undo_action,
                    icon = ":/images/undo.png",
                    text = "&Undo",
                    shortcut = QtGui.QKeySequence.Undo )

        self.redoAction = qthelper.action( self, handler = self.on_redo_action,
                    icon = ":/images/redo.png",
                    text = "&Redo",
                    shortcut = QtGui.QKeySequence.Redo )


        class ShowHideFactory(object):
                def __init__( self, window, elements ):
                    self.window = window
                    self.elements = elements
                def __call__( self ):
                    lv = self.window.get_active_view()
                    if lv is not None:
                      for elementtype in self.elements:
                        currentstate = lv.get_element_state(elementtype)
                        newstate = 2 - currentstate
                        lv.set_element_state(elementtype,newstate)
                      lv.refreshFromModel()

        self.showhide_actions = {
            'camera': qthelper.action( self, handler = ShowHideFactory( self ,['camera','poi'] ),
                    text = "Show/Hide Camera" ,icon = ":/images/show-camera.png"),
            'fields': qthelper.action( self, handler = ShowHideFactory( self ,['linearforcefield','radialforcefield']),
                    text = "Show/Hide Forcefields",icon = ":/images/show-physic.png" ),
            'geom': qthelper.action( self, handler = ShowHideFactory( self ,['rectangle','circle','compositegeom','levelexit','pipe','Vertex','line','hinge','fire']),
                    text = "Show/Hide Geometry" ,icon = ":/images/show-geom.png"),
            'gfx': qthelper.action( self, handler = ShowHideFactory( self ,['SceneLayer','signpost','pixmap']),
                    text = "Show/Hide Graphics" ,icon = ":/images/show-gfx.png" ),
            'goos': qthelper.action( self, handler = ShowHideFactory( self ,['BallInstance','Strand']),
                    text = "Show/Hide Goos" ,icon = ":/images/show-goo.png"),
            'particles': qthelper.action( self, handler = ShowHideFactory( self ,['particles']),
                    text = "Show/Hide Particles" ,icon = ":/images/show-particles.png"),
            'labels': qthelper.action( self, handler = ShowHideFactory( self ,['label']),
                    text = "Show/Hide Labels" ,icon = ":/images/show-label.png")


        }

        self.view_action_group = QtGui.QActionGroup(self)
        self.view_actions = {
            levelview.TOOL_SELECT: qthelper.action( self,
                    handler = self.on_select_tool_action,
                    icon = ":/images/strand.png",
                    text = "&Strand Mode",
                    shortcut = QtGui.QKeySequence( Qt.Key_Space),
                    checkable = True,
                    status_tip = "Click a Goo, hold, move to another Goo and release to connect them." ),
            levelview.TOOL_PAN: qthelper.action( self,
                    handler = self.on_pan_tool_action,
                    icon = ":/images/zoom.png",
                    text = "&Zoom and Pan view (F)",
                    shortcut = 'F',
                    checkable = True ),
            levelview.TOOL_MOVE: qthelper.action( self,
                    handler = self.on_move_tool_action,
                    icon = ":/images/tool-move.png",
                    text = "&Select, Move and Resize",
                    shortcut = 'T',
                    checked = True,
                    checkable = True )
            }

        for action in self.view_actions.itervalues():
            self.view_action_group.addAction( action )

        self.additem_actions = {
        'line':qthelper.action( self,
                    handler = AddItemFactory(self, 'scene','line',{'static':'true'}),
                    icon = ":/images/addline.png",
                    text = "&Add a Line" ),

        'rectangle':qthelper.action( self,
                    handler = AddItemFactory(self, 'scene','rectangle',{'static':'true'}),
                    icon = ":/images/addrect.png",
                    text = "&Add Rectangle" ),

        'circle':   qthelper.action( self,
                    handler = AddItemFactory(self, 'scene','circle',{'static':'true'}),
                    icon = ":/images/addcircle.png",
                    text = "&Add Circle"),

        'image':    qthelper.action( self,
                    handler = AddItemFactory(self, 'scene','SceneLayer',{}),
                    icon = ":/images/group-image.png",
                    text = "&Add Image (SceneLayer)"),

        'compgeom': qthelper.action( self,
                    handler = AddItemFactory(self, 'scene','compositegeom',{}),
                    icon = ":/images/compgeom.png",
                    text = "&Add Composite Geometry (Parent)"),

        'childrect':qthelper.action( self,
                    handler = AddItemFactory(self, 'compositegeom','rectangle',{'mass':'1'}),
                    icon = ":/images/childrect.png",
                    text = "&Add Child Rectangle"),

        'childcircle':qthelper.action( self,
                    handler = AddItemFactory(self,'compositegeom', 'circle',{'mass':'1'}),
                    icon = ":/images/childcircle.png",
                    text = "&Add Child Circle"),

        'hinge':    qthelper.action( self,
                    handler = AddItemFactory(self,'scene', 'hinge',{}),
                    icon = ":/images/hinge.png",
                    text = "&Add Hinge"),

        'autopipe': qthelper.action( self,
                    handler = self.autoPipe,
                    icon = ":/images/pipe.png",
                    text = "&Auto Pipe"),

        'vertex':   qthelper.action( self,
                    handler = self.addPipeVertex,
                    icon = ":/images/pipebend.png",
                    text = "&Add Pipe Vertex"),

        'fire':     qthelper.action( self,
                    handler = AddItemFactory(self, 'level','fire',{}),
                    icon = ":/images/fire.png",
                    text = "&Add Fire"),

        'lff':      qthelper.action( self,
                    handler = AddItemFactory(self, 'scene','linearforcefield',{'size':'100,100'}),
                    icon = ":/images/lff.png",
                    text = "&Add Linear force Field"),

        'rff':      qthelper.action( self,
                    handler = AddItemFactory(self, 'scene','radialforcefield',{}),
                    icon = ":/images/rff.png",
                    text = "&Add Radial force Field"),

        'particles':qthelper.action( self,
                    handler = AddItemFactory(self, 'scene','particles',{}),
                    icon = ":/images/particles.png",
                    text = "&Add Particles"),

        'text':     qthelper.action( self,
                    handler=AddItemFactory(self,'text', 'string',{}),
                    icon = ":/images/text.png",
                    text = "&Add Text Resource"),

        'sign':     qthelper.action( self,
                    handler=AddItemFactory(self,'level', 'signpost',{}),
                    icon = ":/images/sign32.png",
                    text = "&Add Sign"),

        'label':    qthelper.action( self,
                    handler=AddItemFactory(self,'scene', 'label',{}),
                    icon = ":/images/label.png",
                    text = "&Add Label")

        }




        self.addgoo_actions = [
            qthelper.action( self,
                    handler = AddItemFactory(self,'level', 'BallInstance',{'type':'common'}),
                    icon = ":/images/goos/common32.png",
                    text = "&Add common Goo"),

            qthelper.action( self, handler = AddItemFactory(self,'level', 'BallInstance',{'type':'common_albino'}),
                    icon = ":/images/goos/albino32.png", text = "&Add Albino Goo"),

            qthelper.action( self, handler = AddItemFactory(self,'level', 'BallInstance',{'type':'common_black'}),
                    icon = ":/images/goos/black32.png", text = "&Add Black Goo"),

            qthelper.action( self, handler = AddItemFactory(self,'level', 'BallInstance',{'type':'Ivy'}),
                    icon = ":/images/goos/ivy32.png", text = "&Add Ivy Goo"),

            qthelper.action( self, handler = AddItemFactory(self,'level', 'BallInstance',{'type':'water'}),
                    icon = ":/images/goos/water32.png", text = "&Add Water Goo"),

            qthelper.action( self, handler = AddItemFactory(self,'level', 'BallInstance',{'type':'Fuse'}),
                    icon = ":/images/goos/fuse32.png", text = "&Add Fuse Goo"),

            qthelper.action( self, handler = AddItemFactory(self,'level', 'BallInstance',{'type':'balloon'}),
                    icon = ":/images/goos/balloon32.png", text = "&Add Balloon"),

            qthelper.action( self, handler = AddItemFactory(self,'level', 'BallInstance',{'type':'Bone'}),
                    icon = ":/images/goos/bone32.png", text = "&Add Bone Goo"),

            qthelper.action( self, handler = AddItemFactory(self,'level', 'BallInstance',{'type':'Pokey'}),
                    icon = ":/images/goos/pokey32.png", text = "&Add Pokey Goo"),

            qthelper.action( self, handler = AddItemFactory(self,'level', 'BallInstance',{'type':'GooProduct'}),
                    icon = ":/images/goos/gooproduct32.png", text = "&Add Goo Product"),

            qthelper.action( self, handler = AddItemFactory(self,'level', 'BallInstance',{'type':'Beauty'}),
                    icon = ":/images/goos/beauty32.png", text = "&Add Beauty"),

            qthelper.action( self, handler = AddItemFactory(self,'level', 'BallInstance',{'type':'BlockHead'}),
                    icon = ":/images/goos/blockhead32.png", text = "&Add BlockHead"),

            qthelper.action( self, handler = AddItemFactory(self,'level', 'BallInstance',{'type':'RectHead'}),
                    icon = ":/images/goos/recthead32.png", text = "&Add RectHead"),

            qthelper.action( self, handler = AddItemFactory(self,'level', 'BallInstance',{'type':'Pixel'}),
                    icon = ":/images/goos/pixel32.png", text = "&Add Pixel Goo"),

            qthelper.action( self, handler = AddItemFactory(self,'level', 'BallInstance',{'type':'Bit'}),
                    icon = ":/images/goos/bit32.png", text = "&Add Bit Goo"),

            qthelper.action( self, handler = AddItemFactory(self,'level', 'BallInstance',{'type':'Pilot'}),
                    icon = ":/images/goos/pilot32.png", text = "&Add Pilot Goo"),

            qthelper.action( self, handler = AddItemFactory(self,'level', 'BallInstance',{'type':'BombSticky'}),
                    icon = ":/images/goos/bombsticky32.png", text = "&Add Sticky Bomb"),

            qthelper.action( self, handler = AddItemFactory(self,'level', 'BallInstance',{'type':'Anchor'}),
                    icon = ":/images/goos/anchor32.png", text = "&Add Anchor"),

            qthelper.action( self, handler = AddItemFactory(self,'level', 'BallInstance',{'type':'AnchorFriendly'}),
                    icon = ":/images/goos/anchorfriendly32.png", text = "&Add Anchor Friendly"),

            qthelper.action( self, handler = AddItemFactory(self,'level', 'BallInstance',{'type':'AnchorSticky'}),
                    icon = ":/images/goos/anchorsticky32.png", text = "&Add Anchor Sticky"),

        ]
        self.actionTimer = QtCore.QTimer( self )
        self.connect( self.actionTimer, QtCore.SIGNAL("timeout()"), self.onRefreshAction )
        self.actionTimer.start( 250 )    # Refresh action enabled flag every 250ms.

        self.statusTimer = QtCore.QTimer( self )
        self.connect( self.statusTimer, QtCore.SIGNAL("timeout()"),
                      self._on_refresh_element_status )
        self.statusTimer.start( 300 )    # Refresh element status every 300ms.

    def createMenus(self):
        self.fileMenu = self.menuBar().addMenu(self.tr("&File"))
        self.fileMenu.addAction(self.newLevelAction)
        self.fileMenu.addAction(self.editLevelAction)
        self.fileMenu.addAction(self.cloneLevelAction)
        self.fileMenu.addAction(self.saveAction)
        self.fileMenu.addAction(self.playAction)
        self.fileMenu.addSeparator()
        self.fileMenu.addAction(self.changeWOGDirAction)
        self.separatorRecent = self.fileMenu.addSeparator()
        for recentaction in self.recentfile_actions:
            self.fileMenu.addAction(recentaction)
        self.fileMenu.addSeparator()
        self.fileMenu.addAction(self.quitAct)

        self.editMenu = self.menuBar().addMenu(self.tr("&Edit"))
        self.editMenu.addAction( self.undoAction)
        self.editMenu.addAction( self.redoAction)
        self.editMenu.addSeparator()
        self.editMenu.addAction( self.common_actions['cut'] )
        self.editMenu.addAction( self.common_actions['copy'] )
        self.editMenu.addAction( self.common_actions['paste'] )
        self.editMenu.addAction( self.common_actions['pastehere'] )
        self.editMenu.addSeparator()
        self.editMenu.addAction( self.common_actions['delete'] )

        self.menuBar().addSeparator()
        self.resourceMenu = self.menuBar().addMenu(self.tr("&Resources"))
        self.resourceMenu.addAction( self.updateResourcesAction )
        self.resourceMenu.addAction( self.importResourcesAction )
        self.resourceMenu.addAction( self.additem_actions['text'] )
        self.resourceMenu.addSeparator()
        self.resourceMenu.addAction( self.cleanResourcesAction )
        self.resourceMenu.addSeparator()
        self.resourceMenu.addAction( self.setMusicAction )
        self.resourceMenu.addAction( self.setLoopSoundAction )

        self.menuBar().addSeparator()


        # @todo add Windows menu. Take MDI example as model.

        self.helpMenu = self.menuBar().addMenu(self.tr("&Help"))
        self.helpMenu.addAction(self.aboutAct)

    def createToolBars(self):
        self.fileToolBar = self.addToolBar(self.tr("File"))
        self.fileToolBar.setObjectName("fileToolbar")
       # self.fileToolBar.addAction(self.changeWOGDirAction)
        self.fileToolBar.addAction(self.newLevelAction)
        self.fileToolBar.addAction(self.editLevelAction)
        self.fileToolBar.addAction(self.cloneLevelAction)
        self.fileToolBar.addSeparator()
        self.fileToolBar.addAction(self.saveAction)
        self.fileToolBar.addAction(self.playAction)
        self.fileToolBar.addSeparator()
        self.fileToolBar.addAction(self.goomodAction)


        self.editToolbar = self.addToolBar(self.tr("Edit"))
        self.editToolbar.setObjectName("editToolbar")
        self.editToolbar.addAction( self.undoAction)
        self.editToolbar.addAction( self.redoAction)
        self.editToolbar.addSeparator()
        self.editToolbar.addAction( self.common_actions['cut'] )
        self.editToolbar.addAction( self.common_actions['copy'] )
        self.editToolbar.addAction( self.common_actions['paste'] )
        self.editToolbar.addSeparator()
        self.editToolbar.addAction( self.common_actions['delete'] )

        self.resourceToolBar = self.addToolBar(self.tr("Resources"))
        self.resourceToolBar.setObjectName("resourceToolbar")
        self.resourceToolBar.addAction( self.updateResourcesAction )
        self.resourceToolBar.addAction( self.importResourcesAction )
        self.resourceToolBar.addAction( self.additem_actions['text'] )
        self.resourceToolBar.addSeparator()
        self.resourceToolBar.addAction( self.cleanResourcesAction )
        self.resourceToolBar.addSeparator()
        self.resourceToolBar.addAction( self.setMusicAction )
        self.resourceToolBar.addAction( self.setLoopSoundAction )

        self.levelViewToolBar = self.addToolBar(self.tr("Level View"))
        self.levelViewToolBar.setObjectName("levelViewToolbar")

        for name in ('move', 'pan', 'select'):
            action = self.view_actions[name]
            self.levelViewToolBar.addAction( action )

        self.addItemToolBar = QtGui.QToolBar(self.tr("Add Item"))
        self.addItemToolBar.setObjectName("addItemToolbar")
        self.addToolBar(Qt.LeftToolBarArea, self.addItemToolBar)

        additem_action_list=['line','rectangle','circle','image','compgeom','childrect','childcircle','hinge',
                            'sep1',
                            'autopipe','vertex',
                            'sep2',
                            'fire','lff','rff','particles',
                            'sep3',
                            'sign','label'
                            ]

        for name in additem_action_list:
            if name not in self.additem_actions:
                self.addItemToolBar.addSeparator()
            else:
                self.addItemToolBar.addAction( self.additem_actions[name] )

        self.addToolBarBreak(Qt.LeftToolBarArea)
        self.addGooToolBar = QtGui.QToolBar(self.tr("Add Goo"))
        self.addGooToolBar.setObjectName("addGooToolbar")
        self.addToolBar(Qt.LeftToolBarArea, self.addGooToolBar)
        for action in self.addgoo_actions:
            self.addGooToolBar.addAction( action )

        self.showhideToolBar = self.addToolBar(self.tr("Show/Hide"))
        self.showhideToolBar.setObjectName("showhideToolbar")

        for elementtype in ('camera','fields','geom','gfx','goos','particles','labels'):
            self.showhideToolBar.addAction( self.showhide_actions[elementtype] )

    def createStatusBar(self):
        self.statusBar().showMessage(self.tr("Ready"))
        self._mousePositionLabel = QtGui.QLabel()
        self.statusBar().addPermanentWidget( self._mousePositionLabel )

    def createElementTreeView(self, name, tree_meta, sibling_tabbed_dock = None ):
        dock = QtGui.QDockWidget( self.tr( name ), self )
        dock.setObjectName(name+'_tab')
        dock.setAllowedAreas( Qt.RightDockWidgetArea )
        element_tree_view = metatreeui.MetaWorldTreeView( self.common_actions,self.group_icons, dock  )
        tree_model = metatreeui.MetaWorldTreeModel( tree_meta, self.group_icons,
                                                    element_tree_view )
        element_tree_view.setModel( tree_model )
        dock.setWidget( element_tree_view )
        self.addDockWidget( Qt.RightDockWidgetArea, dock )
        if sibling_tabbed_dock: # Stacks the dock widget together
            self.tabifyDockWidget( sibling_tabbed_dock, dock )
        dock.setFeatures(QtGui.QDockWidget.NoDockWidgetFeatures)
        self.tree_view_by_element_world[tree_meta] = element_tree_view
        return dock, element_tree_view

    def createDockWindows(self):
        self.group_icons = {}
        for group in 'camera game image physic resource shape text info ball particles strand fire goomod material rect circle compgeom pipe line sign anim'.split():
            self.group_icons[group] = QtGui.QIcon( ":/images/group-%s.png" % group )
        self.tree_view_by_element_world = {} # map of all tree views
        scene_dock, self.sceneTree = self.createElementTreeView( 'Scene', metawog.TREE_LEVEL_SCENE )
        level_dock, self.levelTree = self.createElementTreeView( 'Level', metawog.TREE_LEVEL_GAME, scene_dock )
        resource_dock, self.levelResourceTree = self.createElementTreeView( 'Resource',
                                                                            metawog.TREE_LEVEL_RESOURCE,
                                                                            level_dock )
        addin_dock, self.addinTree = self.createElementTreeView( 'Addin',
                                                                            metawog.TREE_LEVEL_ADDIN,
                                                                            resource_dock )

        text_dock, self.textTree = self.createElementTreeView( 'Text',
                                                                            metawog.TREE_LEVEL_TEXT,
                                                                            addin_dock )

        dep_dock, self.depTree = self.createElementTreeView( 'Depends',
                                                                            metawog.TREE_LEVEL_DEPENDANCY,
                                                                            text_dock )

        scene_dock.raise_() # Makes the scene the default active tab

        dock = QtGui.QDockWidget(self.tr("Properties"), self)
        dock.setAllowedAreas( Qt.RightDockWidgetArea )
        dock.setFeatures(QtGui.QDockWidget.NoDockWidgetFeatures)
        dock.setObjectName('properties')

        self.propertiesList = metaelementui.MetaWorldPropertyListView( self.statusBar(),
                                                                       dock )

        self.propertiesListModel = metaelementui.MetaWorldPropertyListModel(0, 2,
            self.propertiesList)  # nb rows, nb cols
        self.propertiesList.setModel( self.propertiesListModel )
        dock.setWidget(self.propertiesList)
        self.addDockWidget(Qt.RightDockWidgetArea, dock)

    def _readSettings( self ):
        """Reads setting from previous session & restore window state."""
        settings = QtCore.QSettings()
        settings.beginGroup( "MainWindow" )

        self._wog_path = unicode( settings.value( "wog_path", QtCore.QVariant(u'') ).toString() )
        if self._wog_path==u'.':
            self._wog_path=u''
        elif self._wog_path!=u'':
            self._wog_path=os.path.normpath(self._wog_path)
        self._goomod_path = unicode( settings.value( "goomod_path", QtCore.QVariant(u'') ).toString() )
        if self._goomod_path==u'.':
            self._goomod_path=u''
        elif self._goomod_path!=u'':
            self._goomod_path=os.path.normpath(self._goomod_path)

        if settings.value( "wasMaximized", False ).toBool():
            self.showMaximized()
        else:
            self.resize( settings.value( "size", QtCore.QVariant( QtCore.QSize(640,480) ) ).toSize() )
            self.move( settings.value( "pos", QtCore.QVariant( QtCore.QPoint(200,200) ) ).toPoint() )
        windowstate = settings.value("windowState",None);
        if windowstate is not None:
           self.restoreState(windowstate.toByteArray())

        self.recentFiles = settings.value( "recent_files" ).toStringList()
        self._updateRecentFiles()
        settings.endGroup()

    def _writeSettings( self ):
        """Persists the session window state for future restoration."""
        # Settings should be stored in HKEY_CURRENT_USER\Software\WOGCorp\WOG Editor
        settings = QtCore.QSettings() #@todo makes helper to avoid QVariant conversions
        settings.beginGroup( "MainWindow" )
        settings.setValue( "wog_path", QtCore.QVariant( QtCore.QString(self._wog_path or u'') ) )
        settings.setValue( "goomod_path", QtCore.QVariant( QtCore.QString(self._goomod_path or u'') ) )
        settings.setValue( "wasMaximized",QtCore.QVariant( self.isMaximized()))
        settings.setValue( "size", QtCore.QVariant( self.size() ) )
        settings.setValue( "pos", QtCore.QVariant( self.pos() ) )
        settings.setValue("windowState", self.saveState())
        settings.setValue("recent_files",self.recentFiles)
        settings.endGroup()

    def closeEvent( self, event ):
        """Called when user close the main window."""
        #@todo check if user really want to quit

        for subwin in self.mdiArea.subWindowList():
            if not subwin.close():
                event.ignore()
                return

        self._writeSettings()
        self.actionTimer.stop
        self.statusTimer.stop
        QtGui.QMainWindow.closeEvent(self,event)
        event.accept()

if __name__ == "__main__":
    app = QtGui.QApplication(sys.argv)
    # Set keys for settings
    app.setOrganizationName( "WOGCorp" )
    app.setOrganizationDomain( "goofans.com" )
    app.setApplicationName( "WoG Editor" )

    if LOG_TO_FILE:
        saveout = sys.stdout
        saveerr = sys.stderr
        fout = open(APP_NAME_LOWER+'.log', 'a')
        sys.stdout = fout
        sys.stderr = fout
        print ""
        print "------------------------------------------------------"
        print APP_NAME_PROPER+" started ",datetime.now(), "File Logging Enabled"


    mainwindow = MainWindow()
    mainwindow.show()
    appex = app.exec_()

    if LOG_TO_FILE:
        sys.stdout = saveout
        sys.stderr = saveerr
        fout.close()

    sys.exit(appex)
