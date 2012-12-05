"""Provides a way to describe a graph of elements, that may be linked together.

Objects live in a given world. Worlds are organized as a hierarchy: world children may see their parent elements,
but the parent world can not see the child elements.

Typically, their is a global world with resources common to all levels, and each level has its own world.
This allows each level to define elements with identifiers that would conflict with element defined in other
levels if world were not used.

While elements live in a world, they are attached to a given "file" in that world. A world can contain multiple
files, but each file can only have one root element.

The structure of the graph of elements is defined statically:
- structure of the world kind hierarchy
- kind of file attached to each kind of world
- root element description attached to each file
- for each element description, its possible child element description, and its attribute description.

Object description can define constraint such as the minimum or maximum number of occurrences of the element in its parent.
Attribute description can indicate if the attribute is mandatory, its value domain, typical initial value, type...
"""
import xml.etree.ElementTree
# Publish/subscribe framework
# See http://louie.berlios.de/ and http://pydispatcher.sf.net/
import os
import louie
import math
import wogfile
from PyQt4 import QtCore

BALL_STATES=['attached','climbing','detaching','dragging','falling',
             'pipe','sleeping','standing','stuck','stuck_attached','stuck_detaching',
             'tank','walking' ]

BALL_STATES_PARTICLES=['attached','climbing','detaching','dragging','falling','onfire',
             'pipe','sleeping','standing','stuck','stuck_attached','stuck_detaching',
             'tank','walking' ]


BALL_NAMES = []

WOG_PATH = ''
PLATFORM_WIN=0
PLATFORM_LINUX=1
PLATFORM_MAC=2
# Different type of attributes

BOOLEAN_TYPE = 'boolean'
INTEGER_TYPE = 'integer'
REAL_TYPE = 'real'
RGB_COLOR_TYPE = 'rgb_color'
ARGB_COLOR_TYPE = 'argb_color'
REALLIST_TYPE="reallist"
XY_TYPE = 'xy'
SCALE_TYPE = 'scalewh'
DXDY_TYPE = 'dxdy'
SIZE_TYPE = 'wh'
RADIUS_TYPE = 'radius'
ENUMERATED_TYPE = 'enumerated'
STRING_TYPE = 'string'
TEXT_TYPE='text'
ANGLE_DEGREES_TYPE = 'angle.degrees'
ANGLE_RADIANS_TYPE = 'angle.radians'
REFERENCE_TYPE = 'reference'
IDENTIFIER_TYPE = 'identifier'
PATH_TYPE = 'path'

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

class AttributeMeta(object):
    def __init__( self, name, attribute_type, init = None, default = None, 
                  allow_empty = False, mandatory = False, display_id = False,
                  map_to = None, remove_empty=False, read_only=False, tooltip=None, min_length=None, category=None):
        """display_id: if True indicates that the attribute may be used to 
                       visually identify the element (typically name).
        """
        self.name = name
        self.type = attribute_type
        if init is not None:
            init = str(init)
        self.init = init
        self.default = default
        self.allow_empty = allow_empty
        self.remove_empty = remove_empty
        self.read_only = read_only
        self.mandatory = mandatory
        self.min_length = min_length
        self.element_meta = None
        self.display_id = display_id
        self.map_to = map_to or self.name
        self.tooltip = tooltip
        self.category = category


    def attach_to_element_meta( self, element_meta ):
        self.element_meta = element_meta
        if self.display_id:
            self.element_meta.display_id_attributes.add( self )

    def from_xml(self, xml_element, attributes_by_name):
        """Set attributes values in attributes_by_name using xml_element attributes
           as input."""
        if self.map_to == 'value':
            value = None
        else:
            value = xml_element.get(self.map_to)
        if value is not None:
            attributes_by_name[self.name] = value
    
    def to_xml(self, element, attributes_by_name):
        if self.name == 'value':
            value = None
        else:
            value = element.get(self.name)
        if value is not None:
            attributes_by_name[self.map_to] = value

    def get( self, element ):
        return element.get( self.name )

    def set( self, element, value ):
        return element.set( self.name, value )

    def get_native( self, element, default = None ):
        """Gets the value in python native type: float for real, int for integer...
           Returns default if the value is not convertible in the native type or not
           set.
        """
        raw = self.get( element )
        if raw is None:
            return default
        return raw

    def set_native(self, element, value):
        """Sets the value from the python native type: float for real..."""
        assert value is not None
        self.set( element, str(value) )

    def is_valid_value( self, value, world ): #IGNORE:W0613
        """Checks if the specified attribute is valid on this element.
           Returns tuple (message, args) if the value is not valid. 
           The message should be formatted as follow: message % args.
           Returns None if the value is valid.
        """
        if value is None:
            if self.mandatory:
                return ('mandatory attribute is missing',())
            return None
        if not value and not self.allow_empty:
            return ('empty value not allowed',())
        if not self.min_length is None:
            if len(value)<self.min_length:
                return ('minimum length = '+`self.min_length`,())

        return None

    def __repr__( self ):
        return '%s(name=%s, type=%s, mandatory=%s)' % (self.__class__.__name__, self.name, self.type, self.mandatory)

class ComponentsAttributeMeta(AttributeMeta):
    def __init__(self, name, attribute_type, 
                 min_components = None, max_components = None, components = None,
                 error_message = None,
                 error_messages = None,
                 **kwargs):
        """Comma separated value attributes.
           map_to if defined may be a list of attribute. In that case, each component
           will be mapped to the corresponding attribute in the XML document.
        """
        AttributeMeta.__init__( self, name, attribute_type, **kwargs )
        if components is not None:
            self.min_components = components
            self.max_components = components
        else:
            self.min_components = min_components
            self.max_components = max_components
        assert max_components is None or max_components >= min_components
        if error_message is not None:
            assert isinstance(error_message, (str, unicode))
            self.error_messages = {'missing': error_message, 'extra': error_message} 
        else:
            self.error_messages = error_messages

    def get_native(self, element, default = None):
        raw = self.get(element)
        if raw is None:
            return default
        components = raw.split(',')
        try:
            native_values = [ self._get_native_component(component) 
                              for component in components ]
            if len(native_values) < self.min_components:
                return default
            return tuple(native_values)
        except ValueError:
            return default
        
    def _get_native_component(self, component):
        """Returns a component converted to its python native type.
           The caller handle ValueError conversion error.
        """
        return component

    def set_native(self, element, value):
        """Sets the value from the python native type: float for real..."""
        assert value is not None
        try:
            values = [ self._component_from_native(component) for component in value ]
        except TypeError:
            #if only a single value is passed
            values = [self._component_from_native(value)]
        self.set( element, ','.join( values ) )

    def _component_from_native(self, component):
        return str(component)

    def from_xml(self, xml_element, attributes_by_name):
        """Set attributes values in attributes_by_name using xml_element attributes
           as input.
           If map_to is a list, then map each component to the corresponding attribute
           specified in map_to. For example if map_to is ('x','y'), then the first
           component of the attribute is obtains from the 'x' attribute of the
           XML element, and the second component of the attribute from the 'y'
           attribute of the XML element."""
        if isinstance(self.map_to, (str,unicode)):
            AttributeMeta.from_xml(self, xml_element, attributes_by_name)
        else:
            values = []
            defined = False
            for name in self.map_to:
                value = xml_element.get(name)
                values.append( value or '' )
                defined = defined or value is not None
            if defined:
                attributes_by_name[self.name] = ','.join( values )
    
    def to_xml(self, element, attributes_by_name):
        """Set attribute values in attributes_by_name using the world Element
           attributes' as input."""
        if isinstance(self.map_to, (str,unicode)):
            AttributeMeta.to_xml(self, element, attributes_by_name)
        else:
            values = element.get(self.name)
            if values is not None:
                values = values.split(',')
                for index, name in enumerate(self.map_to):
                    if index >= len(values):
                        break
                    attributes_by_name[name] = values[index]

    def  is_valid_value( self, text, world ): #IGNORE:W0613
        status = AttributeMeta.is_valid_value(self, text, world)
        if status is None and text:
            values = text.split(',')
            nb_components = len(values)
            if nb_components < self.min_components:
                message = self.error_messages.get('missing')
                if not message:
                    message = 'Value must have at least %(nb)d components. ' \
                              'Components are separated by a comma: ",".' 
                return message, {'nb':self.min_components}
            if self.max_components is not None and nb_components > self.max_components: 
                if self.error_messages:
                    message = self.error_messages.get('extra')
                else:
                    message = None
                if not message:
                    if self.max_components==1:
                        message = 'Value must be a single item, it cannot be a list.'
                    else:
                        message = 'Value must have no more than %(nb)d components. ' \
                              'Components are separated by a comma: ",".'
                return message, {'nb':self.min_components}
            for index, component in enumerate(values):
                status = self._is_component_valid( index, component, world )
                if status is not None:
                    break
        return status
    
    def _is_component_valid( self, index, component, world ):
        raise NotImplemented()


class NumericAttributeMeta(AttributeMeta):
    def __init__( self, name, attribute_type, value_type,
                  value_type_error,
                  min_value = None, max_value = None, **kwargs ):
        AttributeMeta.__init__( self, name, attribute_type, **kwargs )
        self.min_value = min_value
        self.max_value = max_value
        self.value_type = value_type # python type of the value
        self.value_type_error = value_type_error # error message on bad value type

    def get_native(self, element, default=None):
        try:
            raw = self.get(element)
            if raw is not None:
                return self.value_type(raw)
            return default
        except ValueError:
            return default

    def  is_valid_value( self, value, world ): #IGNORE:W0613
        status = AttributeMeta.is_valid_value(self, value, world)
        if status is None and value:
            try:
                value = self.value_type(str(value))
                if self.min_value is not None and value < self.min_value:
                    return 'Value must be >= %(v)s', {'v':str(self.min_value)}
                if self.max_value is not None and value > self.max_value:
                    return 'Value must be <= %(v)s', {'v':str(self.max_value)}
            except ValueError:
                return self.value_type_error, ()
        return status

class addinidAttributeMeta(AttributeMeta):
    def  is_valid_value( self, text, world ): #IGNORE:W0613
        status = AttributeMeta.is_valid_value(self, text, world)
        if status is None and text:
           rx = QtCore.QRegExp("^([a-zA-Z0-9]([a-zA-Z0-9]+)?\.)+[a-zA-Z0-9]+$")
           if not rx.exactMatch(text):
                message = 'Addin Id is not valid. It can only contain letters,numbers and . (dots) No Spaces or other symbols!  It should be something like  com.goofans.YourName.LevelName'
                return message, {}
        return status

class OCDAttributeMeta(AttributeMeta):
    def  is_valid_value( self, text, world ): #IGNORE:W0613
        status = AttributeMeta.is_valid_value(self, text, world)
        if status is None and text:
            values = text.split(',')
            nb_components = len(values)
            if nb_components != 2:
                message = 'Value must have %(nb)s components separated by a comma '
                return message, {'nb': '2'}
            valid_ocd = ['balls','moves','time']
            if values[0] not in valid_ocd:
                message = 'First Value must be balls or moves or time'
                return message, {}
            try:
                value = int(str(values[1]))
                if value <= 0:
                    return 'Second Value must be >= %(v)s', {'v':'0'}
            except ValueError:
                return 'Second Value must be a number', ()
        return status

class RadiansAngleAttributeMeta(NumericAttributeMeta):

    def from_xml(self, xml_element, attributes_by_name):
        """Convert radians to degree."""
        value = xml_element.get(self.map_to)
        if value is not None:
            try:
                attributes_by_name[self.name] = str(math.degrees(float(value)))
            except ValueError:
                attributes_by_name[self.name] = float(value)
    
    def to_xml(self, element, attributes_by_name):
        """Convert degree to radians."""
        value = element.get(self.name)
        if value is not None:
            try:
                attributes_by_name[self.map_to] = str(math.radians(float(value)))
            except ValueError:
                attributes_by_name[self.map_to] = value
    

class ColorAttributeMeta(ComponentsAttributeMeta):
    def __init__( self, name, attribute_type, components, **kwargs ):
        message = 'RGB color must be of the form "Red,Green,Blue" range [0-255].'
        if components == 4:
            message = 'ARGB color must be of the form "Alpha,Red,Green,Blue" range [0-255].'
        ComponentsAttributeMeta.__init__( self, name, attribute_type, 
            error_message = message, components = components, **kwargs )
        
    def _get_native_component(self, component):
        """Returns a component converted to its python native type.
           The caller handle ValueError conversion error.
        """
        return int(component)

    def  _is_component_valid( self, index, component, world ): #IGNORE:W0613
        components = self.min_components == 3 and 'RGB' or 'ARGB'
        try:
            value = float(component)
            if value < 0 or value > 255:
                return '%(type)s color component "%(c)s" must be in range [0-255].', {
                    'c':components[index], 'type':components}
        except ValueError:
            return self.error_messages['missing'], {}
        return None

class Vector2DAttributeMeta(ComponentsAttributeMeta):
    def __init__( self, name, attribute_type, min_value = None, position = False, **kwargs ):
        ComponentsAttributeMeta.__init__( self, name, attribute_type, error_message =
            'Value must be of the form "X,Y" were X and Y are real number',
            components = 2, **kwargs )
        self.min_value = min_value
        self.position = position
        
    def _get_native_component(self, component):
        """Returns a component converted to its python native type.
           The caller handle ValueError conversion error.
        """
        return float(component)

    def  _is_component_valid( self, index, component, world ): #IGNORE:W0613
        try:
            value = float(component)
            if self.min_value and value < self.min_value:
                return 'Component must be >= %(min_value)g', {'min_value':self.min_value} 
            return None
        except ValueError:
            return self.error_messages['missing'], {}

class RealListAttributeMeta(ComponentsAttributeMeta):
    def __init__( self, name, attribute_type, min_value = None, position = False, error_message='', **kwargs ):
        ComponentsAttributeMeta.__init__( self, name, attribute_type, error_message = error_message, **kwargs )
        self.min_value = min_value
        self.position = position

    def _get_native_component(self, component):
        """Returns a component converted to its python native type.
           The caller handle ValueError conversion error.
        """
        return float(component)

    def  _is_component_valid( self, index, component, world ): #IGNORE:W0613
        try:
            value = float(component)
            if self.min_value and value < self.min_value:
                return 'Component must be >= %(min_value)g', {'min_value':self.min_value}
            return None
        except ValueError:
            return self.error_messages['missing'], {}

class EnumeratedAttributeMeta(ComponentsAttributeMeta):
    def __init__( self, name, values, attribute_type = ENUMERATED_TYPE, allow_any=False,
                  is_list = False, **kwargs ):
        if is_list and 'allow_empty' not in kwargs:
            kwargs['allow_empty'] = True
        min_components = not is_list and 1 or 0 
        max_components = not is_list and 1 or None 
        ComponentsAttributeMeta.__init__( self, name, attribute_type, 
            min_components = min_components, max_components = max_components,
            **kwargs )
        self.values = values
        self.is_list = is_list
        self.allow_any=allow_any

    def  _is_component_valid( self, index, component, world ): #IGNORE:W0613
        if self.allow_any:
            return None
        if component not in self.values:
            return 'Invalid %(enum)s value: "%(values)s"', {
                'enum':self.name,
                'values':','.join(self.values) }

class BooleanAttributeMeta(EnumeratedAttributeMeta):
    def __init__( self, name, **kwargs ):
        EnumeratedAttributeMeta.__init__( self, name, ('true','false'), 
                                          attribute_type = BOOLEAN_TYPE, **kwargs )

    def get_native(self, element, default=None):
        raw = self.get(element)
        if raw == 'true':
            return True
        elif raw == 'false':
            return False
        return default

class ReferenceAttributeMeta(ComponentsAttributeMeta):
    def __init__( self, name, reference_family, reference_world, is_list = False,
                  **kwargs ):
        if is_list and 'allow_empty' not in kwargs:
            kwargs['allow_empty'] = True
        min_components = not is_list and 1 or 0 
        max_components = not is_list and 1 or None 
        ComponentsAttributeMeta.__init__( self, name, REFERENCE_TYPE, 
            min_components = min_components, max_components = max_components, **kwargs )
        self.reference_family = reference_family
        self.reference_world = reference_world
        self.is_list = is_list

    def attach_to_element_meta( self, element_meta ):
        AttributeMeta.attach_to_element_meta( self, element_meta )
        element_meta._add_reference_attribute( self )

    def  _is_component_valid( self, index, component, world ): #IGNORE:W0613
        if not world.is_valid_attribute_reference( self, component ):
            return ('"%(v)s" is not a valid reference to an element of type %(family)s',
                     { 'v':component,
                       'family':self.reference_family } )
        return None

class IdentifierAttributeMeta(AttributeMeta):
    def __init__( self, name, reference_family, reference_world, **kwargs ):
        AttributeMeta.__init__( self, name, IDENTIFIER_TYPE, **kwargs )
        self.reference_family = reference_family
        self.reference_world = reference_world

    def attach_to_element_meta( self, element_meta ):
        AttributeMeta.attach_to_element_meta( self, element_meta )
        element_meta._set_identifier_attribute( self )


class PathAttributeMeta(AttributeMeta):
    def __init__( self, name, strip_extension = None, **kwargs ):
        AttributeMeta.__init__( self, name, PATH_TYPE, **kwargs )
        self.strip_extension = strip_extension
	#@DaB - Converts \ to /  and // into /
    def set( self, element, value ):
         if ON_PLATFORM==PLATFORM_WIN:
            filename = os.path.normpath(os.path.join(WOG_PATH,self._clean_path(value)+self.strip_extension))
            if os.path.exists(filename):
                #confirm extension on drive is lower case
                len_wogdir = len(os.path.normpath(WOG_PATH))+1
                real_filename = os.path.normpath(_getRealFilename(filename))
                value = os.path.splitext(real_filename)[0][len_wogdir:]

         return element.set( self.name, self._clean_path(value) )

    def _clean_path(self,path):
        path = path.replace('\\','/').replace('//','/')
        basename,extension= os.path.splitext(path)
        if extension.lower() in ['',self.strip_extension]:
            return basename
        else:
            return path

    def  is_valid_value( self, text, world ): #IGNORE:W0613
        status = AttributeMeta.is_valid_value(self, text, world)
        if status is None and text:
            filename = os.path.normpath(os.path.join(WOG_PATH,self._clean_path(text)+self.strip_extension))
            if not os.path.isfile(filename):
                message = 'File not found : ' + filename
                return message, {}
        return status

def bool_attribute( name, **kwargs ):
    return BooleanAttributeMeta( name, **kwargs )

def int_attribute( name, min_value = None, **kwargs ):
    return NumericAttributeMeta( name, INTEGER_TYPE, int,
                                 'value must be an integer', 
                                 min_value = min_value, **kwargs )

def real_attribute( name, min_value = None, max_value = None, **kwargs ):
    return NumericAttributeMeta( name, REAL_TYPE, float, 
                                 'value must be a real number',
                                 min_value = min_value, max_value = max_value, **kwargs )

def rgb_attribute( name, **kwargs ):
    return ColorAttributeMeta( name, RGB_COLOR_TYPE, components = 3, **kwargs )

def argb_attribute( name, **kwargs ):
    return ColorAttributeMeta( name, ARGB_COLOR_TYPE, components = 4, **kwargs )

def radius_attribute( name, max_value = None, **kwargs ):
    return NumericAttributeMeta( name, RADIUS_TYPE, float, 
                                 'value must be a real number',
                                 min_value = 0, max_value = max_value, **kwargs )

def reallist_attribute( name, **kwargs ):
    return RealListAttributeMeta( name, REALLIST_TYPE, **kwargs )

def xy_attribute( name, **kwargs ):
    return Vector2DAttributeMeta( name, XY_TYPE, **kwargs )

def dxdy_attribute( name, **kwargs ):
    return Vector2DAttributeMeta( name, DXDY_TYPE, **kwargs )

def size_attribute( name, **kwargs ):
    return Vector2DAttributeMeta( name, SIZE_TYPE, **kwargs )

def scale_attribute( name, **kwargs ):
    return Vector2DAttributeMeta( name, SCALE_TYPE, **kwargs )

def enum_attribute( name, values, is_list = False, **kwargs ):
    return EnumeratedAttributeMeta( name, values, is_list = is_list, **kwargs )

def string_attribute( name, **kwargs ):
    return AttributeMeta( name, STRING_TYPE, **kwargs )

def text_attribute( name, **kwargs ):
    return AttributeMeta( name, TEXT_TYPE, **kwargs )

def ocd_attribute( name, **kwargs ):
    return OCDAttributeMeta( name, STRING_TYPE, **kwargs )

def addinid_attribute( name, **kwargs ):
    return addinidAttributeMeta( name, STRING_TYPE, **kwargs )


def angle_degrees_attribute( name, min_value = None, max_value = None, **kwargs ):
    return NumericAttributeMeta( name, ANGLE_DEGREES_TYPE, float,
                                 'value must be a real number',
                                 min_value = min_value, max_value = max_value, **kwargs )

def angle_radians_attribute( name, min_value = None, max_value = None, **kwargs ):
    return RadiansAngleAttributeMeta( name, ANGLE_RADIANS_TYPE, float,
                                 'value must be a real number', 
                                 min_value = min_value, max_value = max_value, **kwargs )

def reference_attribute( name, reference_family, reference_world, **kwargs ):
    return ReferenceAttributeMeta( name, reference_family = reference_family, 
                                   reference_world = reference_world, **kwargs )

def identifier_attribute( name, reference_family, reference_world, **kwargs ):
    return IdentifierAttributeMeta( name, reference_family, reference_world, **kwargs )

def path_attribute( name, **kwargs ):
    return PathAttributeMeta( name, **kwargs )

unknown_attribute = string_attribute # to help with generated model

class ObjectsMetaOwner:
    def __init__( self, elements_meta = None ):
        elements_meta = elements_meta or []
        self.__world = None
        self.elements_by_tag = {}
        self.add_elements( elements_meta )

    @property
    def world( self ):
        return self.__world

    def _set_world( self, parent_world ):
        if self.__world is not parent_world: # avoid cycle
            self.__world = parent_world
            for element_meta in self.elements_by_tag.itervalues():
                element_meta._set_world( parent_world )

    def add_elements( self, elements_meta ):
        for element_meta in elements_meta:
            assert element_meta.tag not in self.elements_by_tag, element_meta.tag
            self.elements_by_tag[element_meta.tag] = element_meta
            element_meta._set_world( self.__world )
            self._element_added( element_meta )

    def find_element_meta_by_tag( self, tag ):
        """Returns the ElementMeta corresponding to the specified tag if found in the owner or its descendant.
           None if not found.
        """
        found_element_meta = self.elements_by_tag.get( tag )
        if not found_element_meta:
            for element_meta in self.elements_by_tag.itervalues():
                found_element_meta = element_meta.find_element_meta_by_tag( tag )
                if found_element_meta:
                    break
        return found_element_meta

    def immediate_child_elements(self):
        return self.elements_by_tag.values()

    def immediate_child_tags(self):
        return self.elements_by_tag.keys()

    def find_immediate_child_by_tag( self, tag ):
        """Returns the ElementMeta corresponding to the specified tag if found, otherwise returns None.
           Notes: only direct child are inspected. Grand-children will not be examined.
        """
        return self.elements_by_tag.get( tag )

    def all_descendant_element_metas( self ):
        """Returns a dict of all element desc found in the owner and all its descendant keyed by tag."""
        element_metas_by_tag = self.elements_by_tag.copy()
        for element_meta in self.elements_by_tag.itervalues():
            element_metas_by_tag.update( element_meta.all_descendant_element_metas() )
        return element_metas_by_tag

    def _element_added( self, element_meta ):
        raise NotImplemented()


class ElementMeta(ObjectsMetaOwner):
    """A element description represents a tag that belong in a given file. Its main features are:
       - a tag name
       - a list of attribute description
       - zero or more parent element description
       - a minimum number of occurrences when it occurs in a parent element
       - a conceptual file it may appears in
    """
    def __init__( self, tag, elements_meta = None, attributes = None,
                  min_occurrence = None, max_occurrence = None,
                  read_only = False, groups = None ):
        ObjectsMetaOwner.__init__( self, elements_meta = elements_meta or [] )
        self.tag = tag
        attributes = attributes or []
        self.attributes_order = []
        self.attributes_by_name = {}
        self.parent_elements = set() # An element may be added as child of multiple elements if they are in the same file
        self.identifier_attribute = None
        self.reference_attributes = set()
        self.display_id_attributes = set()
        self.file = None # initialized when element or parent element is added to a file
        self.child_elements_by_tag = {}
        self.min_occurrence = min_occurrence or 0
        self.max_occurrence = max_occurrence or 2**32
        assert self.min_occurrence <= self.max_occurrence
        self.read_only = read_only
        if isinstance( groups, (unicode,str) ):
            groups = (groups,) 
        self.groups = set( groups or () )
        self.main_group = groups and list(groups)[0] or None
        self.add_attributes( attributes )

    def add_attributes( self, attributes ):
        for attribute in attributes:
            assert attribute.name not in self.attributes_by_name, attribute.name
            self.attributes_by_name[attribute.name] = attribute
            attribute.attach_to_element_meta( self )
        self.attributes_order.extend( attributes )

    def _add_reference_attribute( self, attribute_meta ):
        assert attribute_meta not in self.reference_attributes
        self.reference_attributes.add( attribute_meta )

    def _set_identifier_attribute( self, attribute_meta ):
        assert self.identifier_attribute is None
        self.identifier_attribute = attribute_meta

    def _set_file( self, tree_meta ):
        if self.file is not tree_meta: # avoid cycle
            self.file = tree_meta
            for element_meta in self.elements_by_tag.itervalues():
                element_meta._set_file( tree_meta )

    def _element_added( self, element_meta ):
        element_meta.parent_elements.add( self )

    def attribute_by_name( self, attribute_name ):
        """Retrieves the attribute description for the specified attribute_name.
           @exception KeyError if the element has no attribute named attribute_name.
        """ 
        return self.attributes_by_name[attribute_name]

    @property
    def attributes(self):
        """Returns the elements attribute ordered from most important to least one.""" 
        return self.attributes_order[:]
    
    def make_element_from_xml_element( self, xml_element, warning = None ):
        """Create an Element from a xml.tree.ElementTree.Element.
           Returns the created element. The element tag must match this meta element tag.
           warning: callable(message,arguments) => formatted using message % arguments
        """
        assert self.tag == xml_element.tag
        # Map element attributes
        known_attributes = {}
        for attribute_meta in self.attributes_by_name.itervalues():
            attribute_meta.from_xml( xml_element, known_attributes )

        # Map element children
        children = []
        for xml_element_child in xml_element:
            child_meta = self.find_immediate_child_by_tag( xml_element_child.tag )
            if child_meta:
                element = child_meta.make_element_from_xml_element( xml_element_child,
                                                                    warning )
                children.append( element )
            elif warning is not None:
                warning( u'Element %(tag)s, the following child tag missing in the element description: %(child)s.',
                         tag = xml_element.tag,
                         child = xml_element_child.tag )
        return Element( self, attributes = known_attributes, children = children, text=xml_element.text )

    def __repr__( self ):
        return '%s(tag=%s, attributes=[%s], elements=[%s])' % (
            self.__class__.__name__, self.tag, ','.join([a.name for a in self.attributes_order]),
            ','.join(self.elements_by_tag.keys()))

def describe_element( tag, attributes = None, elements = None, groups = None,
                     min_occurrence = None, max_occurrence = None, exact_occurrence = None,
                     read_only = False ):
    if exact_occurrence is not None:
        min_occurrence = exact_occurrence
        max_occurrence = exact_occurrence
    return ElementMeta( tag, attributes = attributes, elements_meta = elements,
                       min_occurrence = min_occurrence, max_occurrence = max_occurrence,
                       read_only = read_only, groups = groups, )

class TreeMeta(ObjectsMetaOwner):
    def __init__( self, conceptual_file_name, elements = None ):
        ObjectsMetaOwner.__init__( self, elements_meta = elements or [] )
        self.name = conceptual_file_name
        assert len(self.elements_by_tag) <= 1

    def _element_added( self, element_meta ):
        assert len(self.elements_by_tag) <= 1
        element_meta._set_file( self )

    @property
    def root_element_meta( self ):
        """Returns the root element description of the file."""
        assert len(self.elements_by_tag) == 1
        return self.elements_by_tag.values()[0]

    def __repr__( self ):
        return '%s(name=%s, elements=[%s])' % (self.__class__.__name__, self.name, ','.join(self.elements_by_tag.keys()))

def describe_tree( conceptual_file_name, elements = None ):
    return TreeMeta( conceptual_file_name, elements = elements )

class WorldMeta(object):
    def __init__( self, name, trees_meta = None, child_worlds = None ):
        child_worlds = child_worlds or []
        self.name = name
        self.parent_world = None
        self.child_worlds = []
        self.trees_meta_by_name = {}
        self.__elements_by_tag = None
        self.add_child_worlds( child_worlds )
        self.add_trees_meta( trees_meta )

    @property
    def trees(self):
        return self.trees_meta_by_name.values()

    @property
    def elements_by_tag( self ):
        if self.__elements_by_tag is None:
            self.__elements_by_tag = {}
            for tree_meta in self.trees_meta_by_name.itervalues():
                self.__elements_by_tag.update( tree_meta.all_descendant_element_metas() )
        return self.__elements_by_tag

    def add_child_worlds( self, child_worlds ):
        self.child_worlds.extend( child_worlds )
        for world in child_worlds:
            world.parent_world = self

    def add_trees_meta( self, trees_meta ):
        for tree_meta in trees_meta:
            assert tree_meta.name not in self.trees_meta_by_name
            self.trees_meta_by_name[tree_meta.name] = tree_meta
            tree_meta._set_world( self )

    def __repr__( self ):
        return '%s(name=%s, files=[%s])' % (self.__class__.__name__, self.name, ','.join(self.trees_meta_by_name.keys()))

def describe_world( name, trees_meta = None, child_worlds = None ):
    return WorldMeta( name, trees_meta = trees_meta, child_worlds = child_worlds )


def print_world_meta( world ):
    """Diagnostic function that print the full content of a World, including its files and elements."""
    print '* World:', world.name
    for child_world in world.child_worlds:
        print '  has child world:', child_world.name
    for tree in world.trees_meta_by_name:
        print '  contained file:', tree
    print '  contains element:', ', '.join( sorted(world.elements_by_tag) )
    for child_world in world.child_worlds:
        print_world_meta( child_world )
        print
    for tree_meta in world.trees_meta_by_name.itervalues():
        print_tree_meta( tree_meta )
        print

def print_tree_meta( tree_meta ):
    """Diagnostic function that print the full content of a TreeMeta, including its elements."""
    print '* File:', tree_meta.name
    print '  belong to world:', tree_meta.world.name
    print '  root element:', tree_meta.root_element_meta.tag
    print '  contains element:', ', '.join( sorted(tree_meta.all_descendant_element_metas()) )
    print '  element tree:'
    print_element_meta_tree( tree_meta.root_element_meta, '        ' )

def print_element_meta_tree( element, indent ):
    """Diagnostic function that print the hierarchy of an ElementMeta and its children."""
    suffix = ''
    if element.min_occurrence == element.max_occurrence:
        if element.min_occurrence > 1:
            suffix = '{%d}' % element.min_occurrence
    elif element.min_occurrence == 0:
        if element.max_occurrence == 1:
            suffix = '?'
        else:
            suffix = '*'
    elif element.min_occurrence == 1:
        suffix = '+'
    else:
        suffix = '{%d-%d}' % (element.min_occurrence, element.max_occurrence)
    print indent + element.tag + suffix
    for child_element in element.elements_by_tag.itervalues():
        print_element_meta_tree( child_element, indent + '    ' )




# SIGNALS
ELEMENT_ADDED = 'added'
ELEMENT_ABOUT_TO_BE_REMOVED = 'about_to_be_removed'
ELEMENT_ATTRIBUTE_UPDATED = 'updated'

class ElementAdded(louie.Signal):
    """Signal emitted when one element has been inserted into a tree or another element.
       The parent element/tree must be connected connected to a tree for
       the signal to be emitted.
       Signature: (parent_element, element, index_in_parent)
       parent_element: new parent Element of the element that has been added.
                       None if element is the root of the tree.
       element: Element that has been added.
       index_in_parent: zero-based index of insertion element in parent_element.
                        Always 0 if element is the root of the tree.
       sender: tree that owns the element
       Can retrieve the element tree via element.tree.   
    """

class ElementAboutToBeRemoved(louie.Signal):
    """Signal emitted when an element connected to a tree is about to be removed.
       Signature: (parent_element, element, index_in_parent)
       parent_element: parent Element of the element that will be removed.
                       None if element is the root of the tree.
       element: Element that will be removed.
       index_in_parent: zero-based index of insertion element in parent_element.
                        Always 0 if element is the root of the tree.  
       sender: tree that owns the element
    """

class AttributeUpdated(louie.Signal):
    """Signal emitted when an attribute of an element connected to a tree has been modified.
       Signature: (element, attribute_name, new_value, old_value )
       element: element that has been modified
       attribute_name: name of the attribute of the element that has been modified
       new_value: new value of the attribute. None if the attribute has been removed.
       old_value: old value of the attribute. None if the attribute has been added.
       sender: tree that owns the element
    """
    
class TreeAdded(louie.Signal):
    """Signal emitted when a tree has been added to a world connected to the universe.
       Signature: (tree)
       tree: tree that has been added to a world.
             Can retrieve the tree world & universe using tree.world and tree.universe.
       sender: parent world of the tree 
    """
    
class TreeAboutToBeRemoved(louie.Signal):
    """Signal emitted when a tree is about to be removed from a world connected to the universe.
       Signature: (tree)
       tree: tree that is about to be removed from a world.
             Can retrieve the tree world & universe using tree.world and tree.universe.
       sender: parent world of the tree 
    """

class WorldAdded(louie.Signal):
    """Signal emitted when a world is added to another world or the universe.
       Signature: (world)
       world: World that has been added. Can retrieve is parent world and universe from
              attributes.
    """
    
class WorldAboutToBeRemoved(louie.Signal):
    """Signal emitted when a world is about to be removed from another world or the universe.
       Signature: (world)
       world: World that is about to be removed. Can retrieve is parent world and universe from
              attributes.
    """
    

class WorldsOwner:
    def __init__( self ):
        self._worlds = {} # dict(desc: dict(key: world) )

    def all_child_worlds(self):
        worlds = []
        for world_data in self._worlds.itervalues():
            worlds.append( world_data.values() )
        return worlds
        
    def make_world( self, world_meta, world_key = None, factory = None, *args, **kwargs ):
        """Creates a child World using the specified world_meta description and associating it with world_key.
           world_meta: description of the world to instantiate.
           workd_key: a unique identifier for the world within the world for worlds of the same kind.
           factory: Type to instantiate. Must be a subclass of World. Default is World.
                    Factory parameters are: (universe, world_meta, key)
                    It will also be passed any extra parameters provided to the function.
        """
        #@todo check that world_meta is an allowed child world

        factory = factory or World
        world = factory( self.universe, world_meta, world_key, #IGNORE:E1101 
                         *args, **kwargs ) 
        if world_meta not in self._worlds:
            self._worlds[world_meta] = {}
        
        assert world.key not in self._worlds[world_meta]
        self._worlds[world_meta][world.key] = world
        parent_world = self.universe != self and self or None #IGNORE:E1101
        world._attached_to_parent_world(parent_world)
        return world

    def remove_world(self, world):
        """Removes the specified child World from the World or Universe.
           exception: KeyError if the World does not belong.
        """
        # @todo
        assert world.key in self._worlds[world.meta]
        parent_world = self.universe != self and self or None #IGNORE:E1101
        world._about_to_be_detached_from_parent_world(parent_world)
        del self._worlds[world.meta][world.key]

    def find_world( self, world_meta, world_key ):
        worlds_by_key = self._worlds.get( world_meta, {} )
        return worlds_by_key.get( world_key )

    def list_worlds_of_type( self, world_meta ):
        worlds_by_key = self._worlds.get( world_meta, {} )
        return worlds_by_key.values()

    def list_world_keys( self, world_meta ):
        worlds_by_key = self._worlds.get( world_meta, {} )
        return worlds_by_key.keys()

class ElementEventsSynthetizer(object):
    """Generates element events for all tree root from world or tree events 
       when they are attached/detached from the universe.
    """
    def __init__(self, universe, 
                 added_handler = None, 
                 updated_handler = None, 
                 removed_handler = None ):
        self.__universe = universe
        self.__added_handler = added_handler
        self.__updated_handler = updated_handler
        self.__removed_handler = removed_handler
        if added_handler is not None:
            louie.connect( self.__on_tree_added, TreeAdded )
            louie.connect( self.__on_world_added, WorldAdded )
        if removed_handler is not None:
            louie.connect( self.__on_tree_about_to_be_removed, TreeAboutToBeRemoved )
            louie.connect( self.__on_world_about_to_be_removed, WorldAboutToBeRemoved )
            
    def __on_world_added(self, world):
        if world.universe == self.__universe:
            for tree in world.trees:
                self.__on_tree_added(tree)
            for world in world.all_child_worlds():
                self.__on_world_added(world)
    
    def __on_world_about_to_be_removed(self, world):
        if world.universe == self.__universe:
            for tree in world.trees:
                self.__on_tree_about_to_be_removed(tree)
            for world in world.all_child_worlds():
                self.__on_world_about_to_be_removed(world)

    def __on_tree_added(self, tree):
        if tree.universe == self.__universe:
            self.__manage_tree_connections( tree, louie.connect )
            if tree.root is not None:
                self.__added_handler( tree.root, 0 )

    def __on_tree_about_to_be_removed(self, tree):
        if tree.universe == self.__universe:
            self.__manage_tree_connections( tree, louie.disconnect )
            if tree.root is not None:
                self.__removed_handler( tree.root, 0 ) 
            
    def __manage_tree_connections(self, tree, connection_manager):
        if self.__added_handler is not None:
            connection_manager( self.__added_handler, ElementAdded, tree )
        if self.__removed_handler is not None:
            connection_manager( self.__removed_handler, 
                                ElementAboutToBeRemoved, tree )
        if self.__updated_handler is not None:
            connection_manager( self.__updated_handler, AttributeUpdated, tree )

class Universe(WorldsOwner):
    """Represents the universe where all elements, worlds and trees live in.
    """
    def __init__( self ):
        WorldsOwner.__init__( self )
        self.ref_by_world_and_family = {} # dict( (world,family): dict(id: element) )
        self.back_references = {} # dict( (family,identifier) : set(world,element,attribute_meta)] )
        self.__event_synthetizer = ElementEventsSynthetizer(self,
            self._on_element_added,
            self._on_element_updated, 
            self._on_element_about_to_be_removed )

    @property
    def universe( self ):
        return self

    def _on_element_added(self, element, index_in_parent): #IGNORE:W0613
        assert isinstance(element,Element)
        assert index_in_parent >=0, index_in_parent
        
        # Checks if the element has any identifier attribute
        identifier_meta = element.meta.identifier_attribute
        if identifier_meta:
            identifier_value = identifier_meta.get( element )
            self._register_element_identifier( element, identifier_meta, identifier_value )

        # Checks element for all reference attributes
        for attribute_meta in element.meta.reference_attributes:
            reference_value = attribute_meta.get( element )
            self._register_element_reference( element, attribute_meta, reference_value )

        for index, child_element in enumerate( element ):
            self._on_element_added(child_element, index)

    def _register_element_identifier( self, element, id_meta, identifier_value ):
        ##print '=> registering "%s" : "%s"' % (element.tag, repr(identifier_value))
        assert element is not None
        if identifier_value is not None:
            # walk parents worlds until we find the right one.
            world = element.world
            while world.meta != id_meta.reference_world:
                world = world.parent_world
            id_world_key = (world,id_meta.reference_family)
            references = self.ref_by_world_and_family.get( id_world_key )
            if references is None:
                references = {}
                self.ref_by_world_and_family[ id_world_key ] = references
            references[identifier_value] = element

    def _register_element_reference( self, element, attribute_meta, reference_value ):
        if reference_value is not None:
            back_reference_key = (attribute_meta.reference_family, reference_value)
            back_references = self.back_references.get( back_reference_key )
            if back_references is None:
                back_references = set()
                self.back_references[back_reference_key] = back_references
            back_references.add( (element, attribute_meta) )

    def _on_element_about_to_be_removed(self, element, index_in_parent): #IGNORE:W0613
        assert isinstance(element,Element)
        assert index_in_parent >= 0, index_in_parent

        # Checks if the element has any identifier attribute
        id_meta = element.meta.identifier_attribute
        if id_meta:
            identifier_value = id_meta.get( element )
            self._unregister_element_identifier( element, id_meta, identifier_value )
            
        # Checks element for all reference attributes
        for attribute_meta in element.meta.reference_attributes:
            reference_value = attribute_meta.get( element )
            self._unregister_element_reference( element, attribute_meta, reference_value )

        for index, child_element in enumerate( element ):
            self._on_element_about_to_be_removed(child_element, index)

    def _unregister_element_identifier( self, element, id_meta, identifier_value ):
        ##print '=> unregistering "%s" : "%s"' % (element.tag, repr(identifier_value))
        if identifier_value is not None:
            # walk parents worlds until we find the right one.
            world = element.world
            while world.meta != id_meta.reference_world:
                world = world.parent_world
            # unregister the reference
            id_world_key = (world,id_meta.reference_family)
            references = self.ref_by_world_and_family.get( id_world_key )
            if references:
                try:
                    del references[identifier_value]
                except KeyError:    # IGNORE:W0704 May happens in case of multiple image with same identifier (usually blank)
                    pass            # since unicity is not validated yet

    def _unregister_element_reference( self, element, attribute_meta, reference_value ):
        if reference_value is not None:
            back_reference_key = (attribute_meta.reference_family, reference_value)
            back_references = self.back_references.get( back_reference_key )
            if back_references:
                back_references.remove( (element, attribute_meta) )

    def _on_element_updated(self, element, name, new_value, old_value): #IGNORE:W0613
        assert isinstance(element,Element) 
#        print 'Element updated', element
        attribute_meta = element.meta.attribute_by_name( name )
        id_meta = element.meta.identifier_attribute
        if id_meta is attribute_meta:
            self._unregister_element_identifier( element, id_meta, old_value )
            self._register_element_identifier( element, id_meta, new_value )
        if attribute_meta in element.meta.reference_attributes:
            self._unregister_element_reference( element, attribute_meta, old_value )
            self._register_element_reference( element, attribute_meta, new_value )
    
    def _warning( self, message, **kwargs ):
        print message % kwargs

    # Identifier/Reference queries
    
    def resolve_reference(self, world, reference_world_meta, family, id_value ):
        """Returns the element corresponding to the specified reference in the world.
           Resolution only consider identifiers defined in world at the level or above 
           the level of reference_world_meta in the hierarchy of world.
           @exception ValueError if world has no world matching 
                      reference_world_meta in its hierarchy. 
        """
        # walk parents worlds until we find the right one.
        assert world is not None
        initial_world = world
        while world.meta != reference_world_meta:
            world = world.parent_world
            if world is None:
                raise ValueError( "World '%(world)s' as no meta world '%(scope)s' in its hierarchy" % 
                                  {'world':initial_world, 
                                   'scope':reference_world_meta } )
        return self.__resolve_reference_in_world_or_parent( world, 
                                                            family, 
                                                            id_value )

    def __resolve_reference_in_world_or_parent(self, world, family, id_value ):
        """Returns the element an id_value identifier for the specified family
           in world or one of its parent world.
           Implementation detail of resolve_reference.
        """
        id_scope_key = (world,family)
        references = self.ref_by_world_and_family.get( id_scope_key )
        if references is not None:
            resolved_element = references.get(id_value)
            if resolved_element is not None:
                return resolved_element
        world = world.parent_world
        if world is not None:
            return self.__resolve_reference_in_world_or_parent( world, 
                                                                family, 
                                                                id_value )
        return None

    
    def is_valid_attribute_reference( self, world, attribute_meta, id_value ):
        """Checks if id_value is valid in world for the family specified 
           by attribute_meta.
           Resolution only consider identifiers defined in world at the level or above 
           the level of reference_world_meta in the hierarchy of world.
           @exception ValueError if world has no world matching 
                      attribute_meta.reference_world in its hierarchy. 
        """
        return self.resolve_reference(world, 
                                      attribute_meta.reference_world, 
                                      attribute_meta.reference_family, 
                                      id_value) is not None

    
    def is_valid_reference( self, world, reference_world, family, id_value ):
        """Checks if id_value is a valid reference in world of type family.
           Resolution only consider identifiers defined in world at the level or above 
           the level of reference_world_meta in the hierarchy of world.
           @exception ValueError if world has no world matching 
                      attribute_meta.reference_world in its hierarchy. 
        """
        return self.resolve_reference(world, reference_world, 
                                      family, id_value) is not None

    def list_identifiers( self, world, family ):
        """Returns a list all identifiers for the specified family in the specified world and its parent worlds."""
        id_scope_key = (world, family)
        identifiers = set(self.ref_by_world_and_family.get( id_scope_key, {} ).keys())
        if world.parent_world is not None:
            identifiers |= self.list_identifiers( world.parent_world, family )
        return identifiers

    def list_world_identifiers( self, world,family):
        id_scope_key = (world, family)
        identifiers = set(self.ref_by_world_and_family.get( id_scope_key, {} ).keys())
        return identifiers
    
    def list_references( self, family, identifier_value ):
        """Returns a list of (element,attribute_meta) element attributes 
           that reference the specified identifier.
        """
        back_reference_key = (family, identifier_value)
        return list( self.back_references.get( back_reference_key, [] ) )

    def make_unattached_tree_from_xml( self, tree_meta, xml_data ):
        """Makes a tree from the provided xml data for the specified kind of tree.
           The tree is NOT attached to any world. Use World.add_tree to do so.
           Returns the created tree if successful (root was successfully parsed),
           otherwise raise the exception WorldException.
           Warning may be raised at the universe level.
           tree_meta: description of the kind of tree to load. Used to associated xml tag to element description.
           xml_data: raw XML data.
        """
        try:
            xml_element = xml.etree.ElementTree.fromstring( xml_data )
        except xml.parsers.expat.ExpatError,e:
            raise IOError(u'XML Parse Error:'+unicode(e))
        
        if tree_meta.root_element_meta.tag != xml_element.tag:
            raise WorldException( u'Expected root tag "%(root)s", but got "%(actual)s" instead.' % {
                'root': tree_meta.root_element_meta.tag, 'actual': xml_element.tag } )
        root_meta = tree_meta.root_element_meta
        root_element = root_meta.make_element_from_xml_element( xml_element,
                                                                self._warning )
        return Tree( self, tree_meta, root_element = root_element )


class WorldException(Exception):
    pass

class World(WorldsOwner):
    """Represents a part of the universe unknown to other worlds, described by a WorldMeta.

       The elements attached to a world are unknown to other World.    
    """
    def __init__( self, universe, world_meta, key = None ):
        WorldsOwner.__init__( self )
        self._universe = universe
        self._world_meta = world_meta
        self._trees = {}
        self._key = key
        self._parent_world = None

    def __repr__( self ):
        trees = ', '.join( tree.meta.name for tree in self._trees.values() )
        return 'World(key="%s",meta="%s",trees=[%s])' % (self.key,self.meta,trees)

    @property
    def key( self ):
        return self._key 

    @property
    def parent_world( self ):
        return self._parent_world

    @property
    def universe( self ):
        return self._universe

    @property
    def meta( self ):
        return self._world_meta

    @property
    def trees(self):
        return self._trees.values()

    def refreshFromFiles(self):
        refreshed=self.key+' refreshing\n'
        for treemeta,tree in self._trees.items():
            if tree.filename!='':
              if os.path.isfile(tree.filename):
                current_filetime = os.path.getmtime(tree.filename)
                if current_filetime > tree.filetime:
                    self._refreshTree(tree)
                    refreshed+= 'refreshed : ' + tree.filename +'  '+`tree.filetime`+'  '+`current_filetime`+"\n"
        return refreshed

    def _refreshTree(self,tree):
        tree_meta = tree.meta
        tree_filename = tree.filename
        #tree_path = os.path.split(tree_filename)
        tree_ext = os.path.splitext(tree_filename)[1]
        #self.remove_tree(tree)
        if tree_ext=='.bin':
            #new_tree = self._loadTree( self, tree_meta,  tree_path[0], tree_path[1])
            xml_data = wogfile.decrypt_file_data( tree_filename )
        else:
            #new_tree = self._loadUnPackedTree( self, tree_meta,  tree_path[0], tree_path[1])
            xml_data = file( tree_filename, 'rb' ).read()
        #new_tree =  self.make_tree_from_xml( tree_meta, xml_data )
        new_tree = self.universe.make_unattached_tree_from_xml( tree_meta, xml_data )
        if new_tree.root.tag=='ResourceManifest':
            self._processSetDefaults(new_tree)

        old_tree = self.find_tree(tree_meta)
        old_tree.set_root(new_tree.root)
        old_tree.setFilename(tree_filename)
        #bit hacky using the tag but...
        #trying to use meta type cause nasty recursive python import problems with metawog

    def _processSetDefaults(self,resource_tree):
        #Unwraps the SetDefaults "processing instruction"
        #updates all paths and ids to full
        
        for resource_element in resource_tree.root.findall('.//Resources'):
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

    def list_identifiers( self, family ):
        """Returns a list all identifiers for the specified family in the specified world and its parent worlds."""
        return self.universe.list_identifiers(self, family)

    def list_world_identifiers( self, family ):
        """Returns a list all identifiers for the specified family in the specified world BUT NOT the parent world."""
        return self.universe.list_world_identifiers(self, family)

    def resolve_reference(self, reference_world_meta, family, id_value ):
        """Returns the element corresponding to the specified reference in this world.
           Resolution only consider identifiers defined in world at the level or above 
           the level of reference_world_meta in the hierarchy of world.
           @exception ValueError if world has no world matching 
                      reference_world_meta in its hierarchy. 
        """
        return self.universe.resolve_reference(self, reference_world_meta, family, id_value)

    def is_valid_attribute_reference( self, attribute_meta, attribute_value ):
        """Checks if the specified attribute reference is valid in the world scope
           specified by attribute_meta.
           @exception ValueError if world has no world matching 
                      attribute_meta.reference_world in its hierarchy. 
        """
        return self.universe.is_valid_attribute_reference( self, attribute_meta, attribute_value )

    def is_valid_reference( self, reference_world, family, id_value ):
        """Checks if id_value is a valid reference in this world of type family.
           Resolution only consider identifiers defined in this world 
           at the level or above the level of reference_world_meta in 
           the hierarchy of world.
           @exception ValueError if world has no world matching 
                      attribute_meta.reference_world in its hierarchy. 
        """
        return self.universe.is_valid_reference( self, reference_world, 
                                                 family, id_value )

    def _attached_to_parent_world( self, parent_world ):
        """Called when a sub-world is attached to the world."""
        self._parent_world = parent_world
        louie.send( WorldAdded, parent_world, self )

    def _about_to_be_detached_from_parent_world(self, parent_world ):
        """Called when a world is removed from the owner."""
        louie.send( WorldAboutToBeRemoved, parent_world, self )
        self._parent_world = None

    def make_tree( self, tree_meta, root_element = None ):
        tree = Tree( self.universe, tree_meta, root_element = root_element )
        self.add_tree( [tree] )
        return tree

    def make_tree_from_xml( self, tree_meta, xml_data ):
        """Makes a tree from the provided xml data for the specified kind of tree.
           The tree is automatically attached to the world.
           Returns the created tree if successful (root was successfully parsed),
           otherwise raise the exception WorldException.
           Warning may be raised at the universe level.
           tree_meta: description of the kind of tree to load. Used to associated xml tag to element description.
           xml_data: raw XML data.
        """

        tree = self.universe.make_unattached_tree_from_xml( tree_meta, xml_data )
        self.add_tree( [tree] )
        return tree

    def find_tree( self, tree_meta ):
        return self._trees.get( tree_meta )

    def add_tree( self, trees ):
        for tree in trees:
            assert tree._world is None
            assert isinstance(tree, Tree)
            tree._world = self
            assert tree._tree_meta not in self._trees 
            self._trees[ tree._tree_meta ] = tree
            louie.send( TreeAdded, self, tree )

    def remove_tree( self, *trees ):
        for tree in trees:
            assert isinstance(tree, Tree)
            assert self._trees.get(tree._tree_meta) == tree
            louie.send( TreeAboutToBeRemoved, self, tree )
            del self._trees[ tree._tree_meta ]
            tree._world = None

    def generate_unique_identifier(self, id_meta ):
        """Generates an identifier value that is unique for the specified attribute
           id_meta in this world.
        """
        assert id_meta.type == IDENTIFIER_TYPE
        prefix = id_meta.reference_family.replace("LEVELNAME",self._key.upper())
        serial_number = 1
        while True:
            id_value = '%s%d' % (prefix,serial_number)
            if not self.is_valid_reference( id_meta.reference_world,
                                            id_meta.reference_family, 
                                            id_value ):
                return id_value
            serial_number += 1

    def _warning( self, message, **kwargs ):
        self._universe._warning( message, **kwargs )
        

class Tree:
    """Represents a part of the world elements live in, described by a TreeMeta.
    """
    def __init__( self, universe, tree_meta, root_element = None ):
        self._universe = universe
        self._tree_meta = tree_meta
        self._root_element = root_element
        self._world = None
        self.set_root( root_element )
        self._filename = ''
        self._filetime = 0

    def setFilename(self,path):
        self._filename=path
        if os.path.isfile(path):
            self._filetime = os.path.getmtime(path)
        else:
            self._filetime = 0

    @property
    def filename( self ):
        return self._filename
    @property
    def filetime( self ):
        return self._filetime

    def set_root( self, root_element ):
        assert root_element is None or isinstance(root_element, Element), type(root_element)
        if self._root_element is not None:  # detach old root
            louie.send( ElementAboutToBeRemoved, self, self._root_element, 0 )
            self._root_element._tree = None
        if root_element is not None: # attach new root
            root_element._tree = self
            louie.send( ElementAdded, self, root_element, 0 )
        self._root_element = root_element

    def __repr__( self ):
        return 'Tree(meta="%s",id="%s",root="%s")' % (self.meta,id(self),self.root)

    @property
    def universe( self ):
        return self._universe

    @property
    def world( self ):
        return self._world

    @property
    def root( self ):
        return self._root_element

    @property
    def meta( self ):
        return self._tree_meta

    def connect_to_element_events(self,
                                  added_handler = None,
                                  updated_handler = None,
                                  removed_handler = None ):
        """Connects to ElementAdded, AttributeUpdated and ElementAboutToBeRemoved
           events related to elements of this tree.
        """
        self._manage_element_events_connection( louie.connect, added_handler, 
                                                updated_handler, removed_handler )

    def disconnect_from_element_events(self,
                                       added_handler = None,
                                       updated_handler = None,
                                       removed_handler = None ):
        """Disconnects to ElementAdded, AttributeUpdated and ElementAboutToBeRemoved
           events related to elements of this tree.
        """
        self._manage_element_events_connection( louie.disconnect, added_handler, 
                                                updated_handler, removed_handler )
        
        
    def _manage_element_events_connection(self, connection_manager,
                                          added_handler, updated_handler, removed_handler ):
        if added_handler is not None:
            connection_manager( added_handler, ElementAdded, self )
        if updated_handler is not None:
            connection_manager( updated_handler, AttributeUpdated, self )
        if removed_handler is not None:
            connection_manager( removed_handler, ElementAboutToBeRemoved, self )

    def to_xml( self, encoding = None ):
        """Outputs a XML string representing the tree.
           The XML is encoded using the specified encoding, or UTF-8 if none is specified.
        """
        assert self.root is not None
        return self.root.to_xml( encoding )

    def clone( self ):
        """Makes a deep clone of the tree root element.
           The returned tree is not attached to a world.
        """
        cloned_root = self.root and self.root.clone() or None
        return Tree( self, self.meta, cloned_root )


# Provides support for attributes dict, children list and path look-up
_ElementBase = xml.etree.ElementTree._ElementInterface

class Element(_ElementBase):
    """Represents a tree that live in a World on a given Tree, described by an ElementMeta.
       The Element's description associates it with a given kind of Tree and restricts
       the kind of parent and child elements it may have.
    """
    def __init__( self, element_meta, attributes = None, children = None , text = None):
        """Initializes the element of type element_meta with the specified attributes.
           element_meta: an ElementMeta instance
           attributes: a dictionary of (name, value) of attributes values
           children: an iterable (list) of child elements not attached to any tree to be attached as child of this element.
        """
        _ElementBase.__init__( self, element_meta.tag, {} )

        # rather than init the xml base element with the given attributes....
        # wait till it's init'd and has meta info
        # then attribute set each one...
        # forces any processing / coersion of entered values to be performed


        self.text = text
        assert element_meta is not None
        self._element_meta = element_meta
        self._parent = None
        self._tree = None # only set for the root element
        for attribute,value in attributes.items():
            self._element_meta.attributes_by_name[attribute].set(self,value)

        for child in children or ():
            self.append( child )

    def is_root( self ):
        return self._parent is None

    @property
    def universe( self ):
        world = self.world
        if world is None:
            return None
        return world.universe

    @property
    def world( self ):
        tree = self.tree
        if tree is None:
            return None
        return tree.world

    @property
    def tree( self ):
        if self._parent is None:
            return self._tree
        return  self._parent.tree

    @property
    def parent( self ):
        """Returns the parent element. None if the element is a root."""
        return self._parent

    @property
    def meta( self ):
        return self._element_meta

    @property
    def attributes(self):
        return self.attrib.copy()

    def make_child( self, element_meta, attributes = None, children = None ):
        """Makes a new child element and append it to the element."""
        child_element = Element( element_meta, attributes, children )
        self.append( child_element )
        return child_element

    def attribute_meta( self, attribute_name ):
        """Returns the AttributeMeta for the specified attribute.
           @exception KeyError if attribute not found.
        """
        return self._element_meta.attributes_by_name[attribute_name]
    @property
    def has_attribute( self, attribute_name ):
        """Returns the AttributeMeta for the specified attribute.
           @exception KeyError if attribute not found.
        """

        return self._element_meta.attributes_by_name[attribute_name]

    def get(self,attribute_name,default=None):
        if attribute_name=='' or attribute_name=='value':
            return self.text or default
        return _ElementBase.get(self,attribute_name,default)
    
    def get_native(self, attribute_name, default=None):
        """Returns the specified attribute as its python type value.
           Returns default if the attribute is not defined or not convertible to
           its python type."""
        return self.attribute_meta(attribute_name).get_native(self, default)

    def to_xml( self, encoding = None ):
        """Outputs a XML string representing the element and its children.
           The XML is encoded using the specified encoding, or UTF-8 if none is specified.
        """
        encoding = encoding or 'utf-8'
        xml_element = self._to_xml_element()
        return xml.etree.ElementTree.tostring( xml_element, encoding )
    
    def _to_xml_element( self ):
        """Returns the element and its children as a xml.etree element for serialization.
           Takes care of expanding mapped attribute (center => x,y).
        """
        attributes_by_name = {}
        for attribute in self.meta.attributes:
            attribute.to_xml( self, attributes_by_name )
        element = xml.etree.ElementTree.Element( self.meta.tag, attributes_by_name )
        if self.text is not None:
            element.text = self.text
        for child in self:
            element.append( child._to_xml_element() )
        return element
        

    def is_detached(self):
        """Indicates if the element does not belong to a tree. 
           Can be used to detect a deleted element.           
        """
        return self.parent is None and self.tree is None

    def xpath(self):
        """Returns the path in XPATH format to access this element.
        """
        # element related paths
        xpath = []
        element = self
        while element is not None:
            # generate xpath component
            xpath_node = u'/' + element.meta.tag
            id_meta = element.meta.identifier_attribute
            identifier = id_meta and id_meta.get( element ) or ''
            if identifier:
                xpath_node += u"[@%s='%s']" % (id_meta.name, identifier)
            elif element.parent:
                elements_before = element.parent[0:element.index_in_parent()]
                tag_position = sum( [ 1 for child in elements_before
                                      if child.meta.tag == element.meta.tag ] )
                xpath_node += u'[%d]' % (tag_position+1)
            xpath.insert(0, xpath_node )
            element = element.parent
        return ''.join( xpath )

    def to_xml_with_meta(self, encoding = None, meta_attributes = None):
        """Outputs a XML string representing the element and its children with meta information
           for this element (Tree, type...).
           The XML is encoded using the specified encoding, or UTF-8 if none is specified.
           
           The following meta-information are provided in the root XML 'MetaWorldElement':
           world_meta_path: '/' separated list of the name of the meta world of the element
                            Example: 'game/level'
           world_path: '/' separated list of the name of the world of the element
                       Example: 'game/GoingUp'
           tree_meta: name of tree meta. Example: 'level.scene'
           element_meta_path:  '/' separated list of the name of the meta element of the element
                               Example: 'scene/SceneLayer'
           element_xpath: XPATH path of the element from the root of the tree.
                         Example: "/scene[0]/BallInstance[@id='BallInstance1']"
           Notes: world & tree meta attributes are only set if available.
           element_path and element_meta_path are always set.
        """
        encoding = encoding or 'utf-8'
        meta_attributes = meta_attributes or {}
        # world related paths
        world_meta_path, world_path = [], []
        world = self.world
        while world is not None:
            world_path.insert( 0, world.key )
            world_meta_path.insert( 0, world.meta.name )
            world = world.parent_world
        # element related paths
        element_meta_path = []
        element = self
        while element is not None:
            element_meta_path.insert( 0, element.meta.tag )
            element = element.parent
        # build final element
        if world_meta_path:
            meta_attributes['world_meta_path'] = '/'.join( world_meta_path )
            meta_attributes['world_path'] = '/'.join( world_path )
        if self.tree is not None:
            meta_attributes['tree_meta'] = self.tree.meta.name
        meta_attributes['element_meta_path'] = '/'.join( element_meta_path )
        meta_attributes['element_xpath'] = self.xpath()
        meta_element = xml.etree.ElementTree.Element( 'MetaWorldElement', 
                                                      meta_attributes )
        #@DaB - Fixes incorrectly pasted combined properties (x,y)
	meta_element.append( self._to_xml_element() )
        return xml.etree.ElementTree.tostring( meta_element, encoding )

    def can_add_child_from_xml(self, xml_data):
        """Tests if the an XML string generated by to_xml_with_meta() can be added as a child.
           Returns True if xml_data contains an element that can be a child of this element.
        """
        child_elements = self._make_element_from_xml( xml_data )
        if child_elements is None:
            return False
        child_tags = self.meta.immediate_child_tags()
        # check if there is at least one compatible child
        return len( [ element for element in child_elements 
                      if element.tag in child_tags ] ) > 0 

    def _make_element_from_xml(self, xml_data):
        try:
            metaworld_element = xml.etree.ElementTree.fromstring( xml_data )
        except xml.parsers.expat.ExpatError:
            return None
        if metaworld_element is None:
            return None
        # skip root node if MetaWorldElement
        child_elements = [metaworld_element]
        if  metaworld_element.tag == 'MetaWorldElement':
            child_elements = metaworld_element[:]
        return child_elements

    def make_detached_child_from_xml( self, xml_data, warning = None ):
        """Creates Elements from a XML string for insertion has a child of this element.
           Returns a list of the created Element or an empty list on failure.
           For an element to be created, it must be a suitable child of this element.
          warning: callable(message,arguments) => formatted using message % arguments
        """
        xml_elements = self._make_element_from_xml( xml_data )
        if xml_elements is None:
            return []
        child_tags = self.meta.immediate_child_tags()
        valid_elements = []
        for xml_element in xml_elements:
            if xml_element.tag in child_tags:
                child_meta = self.meta.find_immediate_child_by_tag( xml_element.tag )
                valid_elements.append(
                    child_meta.make_element_from_xml_element( xml_element, warning ) )
        return valid_elements
        

    def safe_identifier_insert(self, index, element):
        """Insert the child element at the specified index generating new identifier
           if required.
           This element must belong to a world.
        """
        assert self.world 
        id_meta = element.meta.identifier_attribute
        if id_meta is not None:
            id_value = id_meta.get( element )
            # Generate a new identifier if already exist or none is present.
            is_valid = self.world.is_valid_reference( id_meta.reference_world,
                                                      id_meta.reference_family,
                                                      id_value ) 
            if is_valid or not id_value:
                id_meta.set( element, self.world.generate_unique_identifier( id_meta ) )
        # Remove the children, they will be added one by one later to make sure
        # they each have a unique id (otherwise they could end up with the 
        # same identifier if they were in the same family
        old_children = element[:]
        del element[:]
        self.insert( index, element )
        for child_element in old_children:
            # Reinsert the child in its original parent
            element.safe_identifier_insert( len(element), child_element )

    def previous_element(self):
        """Returns the previous element in the tree.
           If the element is at index 0 in its parent, then returns its parent,
           otherwise returns its sibling at the previous index.
        """
        index = self.index_in_parent()
        if index == 0:
            return self.parent
        return self.parent[index-1]
    
    def get_display_id(self):
        """Returns the display attribute value of the element.
           Returns an empty string if the element has no display id attribute.
        """
        for attribute in self.meta.display_id_attributes:
            value = attribute.get(self)
            if value:
                return value
        return ''

    def is_attribute_valid(self, attribute_meta, world):
        """Checks if the specified attribute is valid on this element.
           Returns tuple (is_valid, message)
        """
        value = attribute_meta.get(self)
        return attribute_meta.is_valid_value( value, world )

    def append( self, element ):
        """Adds a subelement to the end of this element.
           @param element The element to add.
           @exception AssertionError If a sequence member is not a valid element.
        """
        index = len(self)
        self._children.append( element )
        self._parent_element( element )
        tree = self.tree
        if tree:
            louie.send( ElementAdded, tree, element, index )

    def insert( self, index, element ):
        """Inserts a subelement at the given position in this element.
           @param index Where to insert the new subelement.
           @exception AssertionError If the element is not a valid element.
        """
        self._children.insert( index, element )
        self._parent_element( element )
        tree = self.tree
        if tree:
            louie.send( ElementAdded, tree, element, index )

    def __setitem__( self, index, element ):
        """Replaces the given subelement.
           @param index What subelement to replace.
           @param element The new element value.
           @exception IndexError If the given element does not exist.
           @exception AssertionError If element is not a valid element.
        """
        tree = self.tree
        old_element = self._children[index]
        if tree:
            louie.send( ElementAboutToBeRemoved, tree, old_element, index )
        self._children[index] = element
        old_element._parent = None
        self._parent_element( element )
        if tree:
            louie.send( ElementAdded, tree, element, index )

    def __delitem__( self, index ):
        """Deletes the given subelement.
           @param index What subelement to delete.
           @exception IndexError If the given element does not exist.
        """
        if index < 0:
            index += len(self)
        if index < 0 or index >= len(self):
            raise IndexError( "Index %(i)d is not in range[0,%(len)d[" % {'i':index,'len':len(self)} )
        tree = self.tree
        element = self._children[index]
        if tree:
            louie.send( ElementAboutToBeRemoved, tree, element, index )
        del self._children[index]
        element._parent = None

    def __setslice__( self, start, stop, elements ):
        """Replaces a number of subelements with elements from a sequence.
           @param start The first subelement to replace.
           @param stop The first subelement that shouldn't be replaced.
           @param elements A sequence element with zero or more elements.
           @exception AssertionError If a sequence member is not a valid element.
        """
        if start < 0:
            start += len(self)
        if start < 0 or start > len(self):
            raise IndexError( "Start index %(i)d is not in range[0,%(len)d[" % {'i':start,'len':len(self)} )
        if stop < 0:
            stop += len(self)
        if stop < start:
            stop = start
        stop = min(len(self), stop)
        tree = self.tree
        for index in xrange(start,stop):
            element = self._children[start]
            if tree:
                louie.send( ElementAboutToBeRemoved, tree, element, start )
            element._parent = None
            del self._children[start]
        for offset, element in enumerate(elements):
            index = start + offset
            self._parent_element( element )
            self._children.insert( index, element )
            louie.send( ElementAdded, tree, element, index )

    def __delslice__( self, start, stop ):
        """Deletes a number of subelements.
           @param start The first subelement to delete.
           @param stop The first subelement to leave in there.
        """
        if start < 0:
            start += len(self)
        if start < 0 or start > len(self):
            raise IndexError( "Start index %(i)d is not in range[0,%(len)d[" % {'i':start,'len':len(self)} )
        if stop < 0:
            stop += len(self)
        if stop < start:
            stop = start
        stop = min(len(self), stop)
        tree = self.tree
        for index in xrange(start,stop):
            element = self._children[start]
            if tree:
                louie.send( ElementAboutToBeRemoved, tree, element, start )
            element._parent = None
            del self._children[start]

    def index_in_parent( self ):
        """Returns the index of the element in its parent.
           Returns 0 if the element has no parent.
        """
        if self._parent is None:
            return 0
        return self._parent._children.index( self )

    def remove( self, element ):
        """Removes a matching subelement.  Unlike the <b>find</b> methods,
           this method compares elements based on identity, not on tag
           value or contents.
           @param element What element to remove.
           @exception ValueError If a matching element could not be found.
           @exception AssertionError If the element is not a valid element.
        """
        index = self._children.index( element )
        tree = self.tree
        if tree:
            louie.send( ElementAboutToBeRemoved, tree, element, index )
        del self._children[index]
        element._parent = None

    def clear( self ):
        """Resets an element.  This function removes all subelements, clears
           all attributes, and sets the text and tail attributes to None.
        """
        tree = self.tree
        for index in xrange(0,len(self)):
            element = self._children[0]
            if tree:
                louie.send( ElementAboutToBeRemoved, tree, element, 0 )
            element._parent = None
            del self._children[0]
        _ElementBase.clear( self )

    def set(self, key, new_value):
        """Sets an element attribute.
           @param key What attribute to set.
           @param value The attribute value.
           @exception KeyError if the element has no attribute with the specified name in its description.
        """
        # @todo check that value is string-like
        assert new_value is not None
        if key not in self._element_meta.attributes_by_name:
            raise KeyError( 'element %(tag)s has no attribute %(name)s' % {
                'tag': self.meta.tag,
                'name': key } )
        tree = self.tree
        if tree:
            if key=='value':
                old_value = self.text
                self.text = new_value
            else:
                old_value = self.attrib.get(key)
                self.attrib[key] = new_value
            louie.send( AttributeUpdated, tree, self, key, new_value, old_value )
        else:
            self.attrib[key] = new_value

    def unset( self, attribute_name ):
        """Removes the specified attribute from the element.
        """
        tree = self.tree
        if tree:
            try:
                old_value = self.attrib[attribute_name]
            except KeyError:
                #wasn't set , so can't be unset..
                return
            del self.attrib[attribute_name]
            louie.send( AttributeUpdated, tree, self, attribute_name, None, old_value )
        else:
            del self.attrib[attribute_name]


    def _parent_element( self, element ):
        assert isinstance(element, Element)
        assert element._parent is None
        element._parent = self

    def clone( self ):
        """Makes a deep clone of the element.
           The returned element is not attached to a tree or parented.
        """
        element = Element( self.meta, attributes = self.attrib.copy() )
        for child in self:
            element.append( child.clone() )
        return element


if __name__ == "__main__":
    import unittest

    TREE_TEST_GLOBAL = describe_tree( 'testglobal' )
    TREE_TEST_VALIDATION  = describe_tree( 'testvalidation' )
    TREE_TEST_LEVEL = describe_tree( 'testlevel' )

    WORLD_TEST_LEVEL = describe_world( 'testworld.level', trees_meta = [TREE_TEST_LEVEL] )

    WORLD_TEST_GLOBAL = describe_world( 'testworld',
                                        trees_meta = [TREE_TEST_GLOBAL, TREE_TEST_VALIDATION],
                                        child_worlds = [WORLD_TEST_LEVEL] )

    GLOBAL_TEXT = describe_element( 'text', attributes = [
        identifier_attribute( 'id', mandatory = True, reference_family = 'text',
                              reference_world = WORLD_TEST_GLOBAL ),
        string_attribute( 'fr' )
        ] )

    GLOBAL_VALIDATION = describe_element( 'validatin', attributes = [
            string_attribute( 'mandatory', mandatory = True ),
            string_attribute( 'mandatory_empty', mandatory = True, allow_empty = True ),
            string_attribute( 'empty', allow_empty = True ),
            int_attribute( 'empty_int', allow_empty = True ),
            int_attribute( 'empty_int_min', allow_empty = True, min_value = 10 ),
            int_attribute( 'empty_int_max', allow_empty = True, max_value = 20 ),
            int_attribute( 'int_bounded', min_value = 10, max_value = 20),
            real_attribute( 'empty_real', allow_empty = True ),
            real_attribute( 'real', allow_empty = True ),
            rgb_attribute( 'rgb' ),
            argb_attribute( 'argb' ),
            enum_attribute('enumyesno', ('yes','no')),
            enum_attribute('enumlist', ('blue','red','green','yelllow'), is_list = True ),
            bool_attribute('bool'),
            xy_attribute('xy')
        ] )

    TREE_TEST_GLOBAL.add_elements( [ GLOBAL_TEXT ] )
    TREE_TEST_VALIDATION.add_elements( [ GLOBAL_VALIDATION ] )

    LEVEL_TEXT = describe_element( 'text', attributes = [
        identifier_attribute( 'id', mandatory = True, reference_family = 'text',
                              reference_world = WORLD_TEST_LEVEL ),
        string_attribute( 'fr' )
        ] )
    
    LEVEL_SIGN = describe_element( 'sign', attributes = [
        reference_attribute( 'text', reference_family = 'text',
                             reference_world = WORLD_TEST_LEVEL, init = '', mandatory = True ),
        reference_attribute( 'alt_text', reference_family = 'text',
                             reference_world = WORLD_TEST_LEVEL ),
        xy_attribute( 'pos', map_to = ('x','y') )
        ], elements = [ LEVEL_TEXT ] )

    LEVEL_INLINE = describe_element( 'inline', elements= [ LEVEL_SIGN, LEVEL_TEXT ] )

    TREE_TEST_LEVEL.add_elements( [ LEVEL_INLINE ] )


    class MetaTest(unittest.TestCase):

        def test_descriptions( self ):
            self.assertEqual( sorted(['text', 'sign', 'inline']), sorted(WORLD_TEST_LEVEL.elements_by_tag.keys()) )
            for world in (WORLD_TEST_LEVEL, WORLD_TEST_GLOBAL):
                for element_meta in world.elements_by_tag.itervalues():
                    self.assertEqual( world, element_meta.world )
#                for tree_meta in world.trees_meta_by_name.itervalues():
#                    self.assertEqual( world, element_meta.world )
            self.assertEqual( sorted([LEVEL_SIGN, LEVEL_INLINE]), sorted(LEVEL_TEXT.parent_elements) )
            for tree, elements in { TREE_TEST_GLOBAL: [GLOBAL_TEXT],
                                   TREE_TEST_LEVEL: [LEVEL_TEXT, LEVEL_SIGN] }.iteritems():
                for element in elements:
                    self.assertEqual( tree, element.file )
                    self.assert_( element in tree.all_descendant_element_metas().values() )
                    self.assert_( element.tag in tree.all_descendant_element_metas() )
                    self.assert_( element.tag in tree.world.elements_by_tag )

    class UniverseTest(unittest.TestCase):


        def setUp( self ):
            self.universe = Universe()
            self.world = self.universe.make_world( WORLD_TEST_GLOBAL, 'global' )
            self.world_level1 = self.world.make_world( WORLD_TEST_LEVEL, 'level1' )
            self.level1 = self.world_level1.make_tree( TREE_TEST_LEVEL )
            self.world_level2 = self.world.make_world( WORLD_TEST_LEVEL, 'level2' )
            self.level2 = self.world_level2.make_tree( TREE_TEST_LEVEL )

        def _make_element(self, element_meta, **attributes ):
            return Element( element_meta, attributes = attributes )

        def test_identifiers(self):
            universe = self.universe
            self.assertEqual( set([]), universe.list_identifiers(self.world, 'text') )
            self.failIf(universe.list_references('text', 'TEXT_HI'))
            # add elements
            gt1 = self._make_element( GLOBAL_TEXT, id = 'TEXT_HI', fr = 'Salut' )
            global_tree = self.world.make_tree( TREE_TEST_GLOBAL, gt1 )
            assert global_tree.universe == universe
            # check reference resolution to global world from global or level worlds
            def check_text_ids( world, *args ):
                self.assertEqual( set(args), 
                                  universe.list_identifiers(world, 'text') )
            check_text_ids( self.world, 'TEXT_HI' )
            check_text_ids( self.world_level1, 'TEXT_HI' )
            check_text_ids( self.world_level2, 'TEXT_HI' )
            def check_valid_sign_reference(world, value):
                attribute_meta = LEVEL_SIGN.attributes_by_name['text']
                assert world is not None
                self.assert_( world.is_valid_attribute_reference(attribute_meta, value) ) 
            def check_invalid_sign_reference(world, value):
                attribute_meta = LEVEL_SIGN.attributes_by_name['text']
                assert world is not None
                self.failIf( world.is_valid_attribute_reference(attribute_meta, value) ) 
            check_valid_sign_reference( self.world_level1, 'TEXT_HI')
            check_valid_sign_reference( self.world_level2, 'TEXT_HI')
            check_invalid_sign_reference( self.world_level1, 'TEXT_HO')
            check_invalid_sign_reference( self.world_level2, 'TEXT_HO')
            # add identifiers specific to level1 world
            l1root = self._make_element( LEVEL_INLINE )
            l2root = self._make_element( LEVEL_INLINE )
            self.level1.set_root( l1root )
            self.level2.set_root( l2root )
            l1_ho = l1root.make_child(LEVEL_TEXT, {'id':'TEXT_HO', 'fr':'Oh'})
            # check that reference resolution keep level worlds identifiers independent
            check_valid_sign_reference( self.world_level1, 'TEXT_HO')
            check_invalid_sign_reference( self.world_level2, 'TEXT_HO')
            check_text_ids( self.world, 'TEXT_HI' )
            check_text_ids( self.world_level1, 'TEXT_HI', 'TEXT_HO' )
            check_text_ids( self.world_level2, 'TEXT_HI' )
            # add identifier specified to level2 world
            l2_ho = l2root.make_child(LEVEL_TEXT, {'id':'TEXT_HO', 'fr':'Oooh'})
            check_valid_sign_reference( self.world_level2, 'TEXT_HO')
            check_text_ids( self.world_level1, 'TEXT_HI', 'TEXT_HO' )
            check_text_ids( self.world_level2, 'TEXT_HI', 'TEXT_HO' )
            #
            # check back references
            #
            def check_references( identifier_value, *args ):
                expected = set([ (element,element.meta.attribute_by_name(name))
                                 for element, name in args ] )
                actual = set( universe.list_references( 'text', identifier_value ) )
                self.assertEqual( expected, actual )
            check_references( 'TEXT_HI' )    
            l1s1 = l1root.make_child( LEVEL_SIGN, 
                                      {'text':'TEXT_HI', 'alt_text':'TEXT_HO'})
            check_references( 'TEXT_HI', (l1s1,'text') )    
            check_references( 'TEXT_HO', (l1s1,'alt_text') )
            # check after update    
            l1s1.set( 'text', 'TEXT_HO' )
            check_references( 'TEXT_HI' )    
            check_references( 'TEXT_HO', (l1s1,'text'), (l1s1,'alt_text') )
            # check after unset attribute
            l1s1.unset( 'alt_text' )
            check_references( 'TEXT_HI' )    
            check_references( 'TEXT_HO', (l1s1,'text') )
            # remove element with reference
            l1s1.parent.remove( l1s1 )
            check_references( 'TEXT_HI' )    
            check_references( 'TEXT_HO' )
            # remove element with identifier
            l1_ho.parent.remove( l1_ho )
            check_invalid_sign_reference( self.world_level1, 'TEXT_HO')

        def test_world( self ):
            self.assertEqual( self.universe, self.universe.universe )
            self.assertEqual( self.universe, self.world.universe )
            self.assertEqual( self.universe, self.world_level1.universe )
            self.assertEqual( self.universe, self.level1.universe )
            # Global
            self.assertEqual( sorted( ['global'] ),
                              sorted( self.universe.list_world_keys( WORLD_TEST_GLOBAL ) ) )
            self.assertEqual( sorted( [self.world] ),
                              sorted( self.universe.list_worlds_of_type( WORLD_TEST_GLOBAL ) ) )
            self.assertEqual( self.world, self.universe.find_world( WORLD_TEST_GLOBAL, 'global' ) )
            # Levels
            self.assertEqual( sorted( ['level1', 'level2'] ),
                              sorted( self.world.list_world_keys( WORLD_TEST_LEVEL ) ) )
            self.assertEqual( sorted( [self.world_level1, self.world_level2] ),
                              sorted( self.world.list_worlds_of_type( WORLD_TEST_LEVEL ) ) )
            self.assertEqual( self.world_level1, self.world.find_world( WORLD_TEST_LEVEL, 'level1' ) )
            self.assertEqual( self.world, self.world_level1.parent_world )
            # Missing
            self.assertEqual( sorted( [] ),
                              sorted( self.world.list_world_keys( WORLD_TEST_GLOBAL ) ) )
            self.assertEqual( sorted( [] ),
                              sorted( self.world.list_worlds_of_type( WORLD_TEST_GLOBAL ) ) )
            self.assertEqual( None, self.world.find_world( WORLD_TEST_LEVEL, 'level_unknown' ) )
            self.assertEqual( None, self.world.find_world( WORLD_TEST_GLOBAL, 'level_unknown' ) )
    
        def test_element(self):
            s1 = self._make_element( LEVEL_SIGN )
            class ChangeListener:
                def __init__(self,test):
                    self.test = test
                    self._events = []
                    self._expectations = []
                def expect_event(self, signal, parent_element, element, index ):
                    self._events.append( (signal,parent_element, element, index) )
                def on_event(self, element, index, signal=None):
                    expected = self._events.pop(0)  # if failure there, then unexpected extra events...
                    actual = (signal,element.parent, element, index)
                    self.test.assertEqual( expected, actual )
                def expect_attribute(self, element, name, value, old_value):
                    self._events.append( (AttributeUpdated, element, name, value, old_value) )
                def on_attribute_change(self, element, name, value, old_value,signal=None ):
                    expected = self._events.pop(0)  # if failure there, then unexpected extra events...
                    actual = (signal,element,name,value,old_value)
                    self.test.assertEqual( expected, actual )
                def check(self):
                    self.test.assertEqual( [], self._events )
            def check_list( *args ):
                elements = s1[:]
                self.assertEquals( list(args), list(elements) ) 
            t1 = self._make_element( LEVEL_TEXT )
            t2 = self._make_element( LEVEL_TEXT )
            t3 = self._make_element( LEVEL_TEXT )
            t4 = self._make_element( LEVEL_TEXT )
            t5 = self._make_element( LEVEL_TEXT )
            # Test: append() & insert()
            s1.append( t3 )    # t3
            s1.insert( 0, t1 ) # t1, t3
            s1.insert( 1, t2 ) # t1, t2, t3
            s1.append( t4 )    # t1, t2, t3, t4
            s1.insert( 4, t5 ) # t1, t2, t3, t4, t5
            # Checks that child are in s1 and correctly parent
            self.assertEqual( 5, len(s1) )
            self.assertEqual( LEVEL_SIGN, s1.meta  )
            self.assertEqual( LEVEL_TEXT, t2.meta  )
            self.assertEqual( None, t3.universe )
            self.assertEqual( None, s1.parent )
            self.assertEqual( s1, t2.parent )
            self.assertEqual( s1, t3.parent )
            self.assertEqual( None, s1.tree )
            self.assertEqual( None, s1.world )
            self.assertEqual( None, s1.universe )
            self.assertEqual( None, t2.tree )
            self.assertEqual( None, t2.world )
            self.assertEqual( None, t2.universe )
            check_list( t1, t2, t3, t4, t5 )

            # Attach s1 to a root
            events_checker = ChangeListener( self )
            louie.connect( events_checker.on_event, ElementAdded )
            louie.connect( events_checker.on_event, ElementAboutToBeRemoved )
            louie.connect( events_checker.on_attribute_change, AttributeUpdated )
            events_checker.expect_event(ElementAdded, None, s1, 0)
            self.level1.set_root( s1 )
            events_checker.check()
            events_checker.expect_event(ElementAboutToBeRemoved, None, s1, 0)
            self.level1.set_root( None )
            events_checker.expect_event(ElementAdded, None, s1, 0)
            self.level1.set_root( s1 )
            events_checker.check()

            # Checks that universe... is correctly propagated to all children
            self.assertEqual( self.level1, s1.tree )
            self.assertEqual( self.level1, t2.tree )
            self.assertEqual( self.world_level1, s1.world )
            self.assertEqual( self.world_level1, t2.world )
            self.assertEqual( self.universe, s1.universe )
            self.assertEqual( self.universe, t2.universe )
            check_list( t1, t2, t3, t4, t5 )
            # setitem
            t6 = self._make_element( LEVEL_TEXT )
            events_checker.expect_event(ElementAboutToBeRemoved, s1, t1, 0)
            events_checker.expect_event(ElementAdded, s1, t6, 0)
            s1[0] = t6
            self.assertEqual( t6, s1[0] )
            self.assertEqual( s1, t6.parent )
            self.assertEqual( None, t1.parent )
            check_list( t6, t2, t3, t4, t5 )
            events_checker.check()
            # delitem
            events_checker.expect_event(ElementAboutToBeRemoved, s1, t6, 0)
            del s1[0]
            self.assertEqual( t2, s1[0] )
            self.assertEqual( None, t6.parent )
            check_list( t2, t3, t4, t5 )
            events_checker.check()
            # setslice
            events_checker.expect_event(ElementAdded, s1, t1, 0)
            s1[0:0] = [t1]
            self.assertEqual( t1, s1[0] )
            check_list( t1, t2, t3, t4, t5 )
            events_checker.expect_event(ElementAboutToBeRemoved, s1, t2, 1)
            events_checker.expect_event(ElementAboutToBeRemoved, s1, t3, 1)
            s1[1:3] = []
            check_list( t1, t4, t5 )
            self.assertEqual( None, t2.parent )
            self.assertEqual( None, t3.parent )
            self.assertEqual( 5-2, len(s1) )
            events_checker.expect_event(ElementAdded, s1, t2, 1)
            events_checker.expect_event(ElementAdded, s1, t3, 2)
            s1[1:1] = [t2,t3]
            check_list( t1, t2, t3, t4, t5 )
            self.assertEqual( s1, t2.parent )
            self.assertEqual( s1, t3.parent )
            self.assertEqual( 5, len(s1) )
            events_checker.check()
            # delslice
            events_checker.expect_event(ElementAboutToBeRemoved, s1, t2, 1)
            events_checker.expect_event(ElementAboutToBeRemoved, s1, t3, 1)
            del s1[1:3]
            check_list( t1, t4, t5 )
            self.assertEqual( None, t2.parent )
            self.assertEqual( None, t3.parent )
            self.assertEqual( 5-2, len(s1) )
            events_checker.expect_event(ElementAdded, s1, t2, 1)
            events_checker.expect_event(ElementAdded, s1, t3, 2)
            s1[1:1] = [t2,t3]
            check_list( t1, t2, t3, t4, t5 )
            self.assertEqual( s1, t2.parent )
            self.assertEqual( s1, t3.parent )
            events_checker.check()
            # remove
            events_checker.expect_event(ElementAboutToBeRemoved, s1, t2, 1)
            s1.remove(t2)
            check_list( t1, t3, t4, t5 )
            self.assertEqual( None, t2.parent )
            self.assertEqual( 5-1, len(s1) )
            events_checker.expect_event(ElementAdded, s1, t2, 1)
            s1[1:1] = [t2]
            check_list( t1, t2, t3, t4, t5 )
            events_checker.check()
            # clear
            events_checker.expect_event(ElementAboutToBeRemoved, s1, t1, 0)
            events_checker.expect_event(ElementAboutToBeRemoved, s1, t2, 0)
            events_checker.expect_event(ElementAboutToBeRemoved, s1, t3, 0)
            events_checker.expect_event(ElementAboutToBeRemoved, s1, t4, 0)
            events_checker.expect_event(ElementAboutToBeRemoved, s1, t5, 0)
            s1.clear()
            check_list()
            self.assertEqual( 0, len(s1) )
            self.assertEqual( None, t1.parent )
            self.assertEqual( None, t2.parent )
            self.assertEqual( None, t3.parent )
            self.assertEqual( None, t4.parent )
            self.assertEqual( None, t5.parent )
            self.assertEqual( 0, len(s1.keys()) )
            events_checker.check()
            # set attribute
            events_checker.expect_event(ElementAdded, s1, t1, 0)
            s1.append( t1 )
            events_checker.expect_attribute( t1, 'id', 'TEXT_HI', None )
            t1.set( 'id', 'TEXT_HI' )
            events_checker.expect_attribute( t1, 'id', 'TEXT_HO', 'TEXT_HI' )
            t1.set( 'id', 'TEXT_HO' )
            try:
                t1.set( '_bad_attribute', 'dummy')
                self.fail()
            except KeyError: #IGNORE:W0704 
                pass
            events_checker.check()

        def test_from_to_xml_clone( self ):
            xml_data = """<inline>
<text id ="TEXT_HI" fr="Salut" />
<text id ="TEXT_HO" fr="Oh" />
<sign text="TEXT_HI" alt_text="TEXT_HO" x="1234" y="4567">
  <text id="TEXT_CHILD" fr="Enfant" />
</sign>
</inline>
"""
            def check( xml_data ):
                world_level = self.world.make_world( WORLD_TEST_LEVEL, 'levelxml' )
                level_tree = world_level.make_tree_from_xml( TREE_TEST_LEVEL, xml_data )
                self.assertEqual( TREE_TEST_LEVEL, level_tree.meta )
                self.assertEqual( self.universe, level_tree.universe )
                self.assertEqual( world_level, level_tree.world )
                self.assertEqual( level_tree, world_level.find_tree( TREE_TEST_LEVEL ) )
                self.assertEqual( world_level, level_tree.root.world )
                # content            
                inline = level_tree.root
                self.assertEqual( LEVEL_INLINE, inline.meta )
                self.assertEqual( 3, len(inline) )
                self.assertEqual( LEVEL_TEXT, inline[0].meta )
                self.assertEqual( LEVEL_TEXT, inline[1].meta )
                self.assertEqual( LEVEL_SIGN, inline[2].meta )
                self.assertEqual( LEVEL_TEXT, inline[2][0].meta )
                self.assertEqual( 1, len(inline[2]) )
                self.assertEqual( sorted( [('fr','Salut'),('id','TEXT_HI')] ), sorted(inline[0].items()) )
                self.assertEqual( sorted( [('fr','Oh'),('id','TEXT_HO')] ), sorted(inline[1].items()) )
                self.assertEqual( sorted( [('alt_text','TEXT_HO'),('pos','1234,4567'),('text','TEXT_HI')] ), sorted(inline[2].items()) )
                self.assertEqual( sorted( [('fr','Enfant'),('id','TEXT_CHILD')] ), sorted(inline[2][0].items()) )
                self.world.remove_world( world_level )
                return level_tree

            level_tree = check( xml_data )
            xml_data = level_tree.to_xml()
            check( xml_data )
            level_tree = check( xml_data )
            xml_data = level_tree.to_xml()
            check( xml_data )
            # clone
            cloned_tree = level_tree.clone()
            xml_data = cloned_tree.to_xml()
            check( xml_data )

        def test_from_xml2( self ):
            xml_data = """<inline></inline>"""
            world_level = self.world.make_world( WORLD_TEST_LEVEL, 'levelxml' )
            level_tree = world_level.make_tree_from_xml( TREE_TEST_LEVEL, xml_data )
            self.assertEqual( world_level, level_tree.world )
            self.assertEqual( world_level, level_tree.root.world )

# Hmm, can not figure out how to match the root with ElementTree
#        def test_element_xpath(self):
#            xml_data = """<inline>
#<text id ="TEXT_HI" fr="Salut" />
#<text id ="TEXT_HO" fr="Oh" />
#<sign text="TEXT_HI" alt_text="TEXT_HO">
#  <text id="TEXT_CHILD" fr="Enfant" />
#</sign>
#</inline>
#"""
#            world_level = self.world.make_world( WORLD_TEST_LEVEL, 'levelxml' )
#            level_tree = world_level.make_tree_from_xml( TREE_TEST_LEVEL, xml_data )
#            root_path = level_tree.root.xpath()
#            def check_path( path, expected_element ):
#                assert path.startswith('/')
##                path = '.' + path
#                tree = xml.etree.ElementTree.ElementTree( level_tree.root )
#                self.assertEqual( expected_element, tree.find( path ) )
#            check_path( root_path, level_tree.root )

        def test_validation(self):
            root = self._make_element( GLOBAL_VALIDATION )
            tree = self.world.make_tree( TREE_TEST_VALIDATION, root )
            assert tree.universe == self.universe
            # check missing mandatory
            def validate( attribute_name, value ):
                if value is None:
                    if root.get(attribute_name) is not None:
                        root.unset(attribute_name)
                else:
                    root.set(attribute_name, value)
                attribute_meta = root.meta.attribute_by_name( attribute_name )
                assert attribute_meta is not None
                return root.is_attribute_valid(attribute_meta, self.world)

            def check_not_valid( attribute_name, value ):
                status = validate( attribute_name, value )
                self.assert_( status is not None )
                message, args = status
                return message % args # check formatting

            def check_valid( attribute_name, value ):
                status = validate( attribute_name, value )
                self.assert_( status is None, status )

            # mandatory check
            check_not_valid( 'mandatory', None )
            check_not_valid( 'mandatory_empty', None )
            check_not_valid( 'mandatory', None )
            check_not_valid( 'mandatory', '' )
            check_valid( 'mandatory', 'abc' )
            # allow empty check
            check_valid( 'mandatory_empty', '' )
            check_valid( 'empty', None )
            check_valid( 'empty', '' )
            # numeric check
            check_valid( 'empty_int', None )
            check_valid( 'empty_int', '' )
            check_valid( 'empty_int', '-12345678' )
            check_valid( 'empty_int', '12345678' )
            check_not_valid( 'empty_int', '1234.5678' )
            check_not_valid( 'empty_int', '1e10' )
            check_not_valid( 'empty_int', '0x1234' )
            check_not_valid( 'empty_int', 'abc' )
            check_not_valid( 'empty_int_min', '9' )
            check_not_valid( 'empty_int_min', '-15' )
            check_valid( 'empty_int_min', '10' )
            check_valid( 'empty_int_min', '15' )
            check_not_valid( 'empty_int_max', '21' )
            check_valid( 'empty_int_max', '-21' )
            check_valid( 'empty_int_max', '20' )
            check_valid( 'empty_int_max', '15' )
            check_valid( 'int_bounded', '10' )
            check_valid( 'int_bounded', '15' )
            check_valid( 'int_bounded', '20' )
            check_not_valid( 'int_bounded', '9' )
            check_not_valid( 'int_bounded', '21' )
            # numeric real
            check_valid( 'empty_real', None )
            check_valid( 'empty_real', '' )
            check_valid( 'empty_real', '1.234' )
            check_valid( 'real', '1.234' )
            check_valid( 'real', '1.234e10' )
            check_valid( 'real', '1' )
            check_valid( 'real', '-4.1' )
            # rgb
            check_valid( 'rgb', None )
            check_not_valid( 'rgb', '' )
            check_valid( 'rgb', '0,0,0' )
            check_valid( 'rgb', '255,255,255' )
            check_not_valid( 'rgb', '0' )
            check_not_valid( 'rgb', '0,,' )
            check_not_valid( 'rgb', '0,0,' )
            check_not_valid( 'rgb', '0,0,0,' )
            check_not_valid( 'rgb', ',0,0,' )
            check_not_valid( 'rgb', '0,0,0,0' )
            # argb
            check_valid( 'argb', None )
            check_not_valid( 'argb', '' )
            check_valid( 'argb', '0,0,0,0' )
            check_valid( 'argb', '255,255,255,0' )
            check_not_valid( 'argb', '0,0,0' )
            check_not_valid( 'argb', '0,0,0,0,0' )
            check_valid( 'argb', '255,76.0000030696392,49.0000008791685,15.0000000558794' )
            
            # check enum
            check_valid( 'enumyesno', None )
            check_not_valid( 'enumyesno', '' )
            check_valid( 'enumyesno', 'yes' )
            check_valid( 'enumyesno', 'no' )
            check_not_valid( 'enumyesno', 'NO' )
            check_not_valid( 'enumyesno', 'true' )
            # check enumlist
            check_valid( 'enumlist', None )
            check_valid( 'enumlist', '' )
            check_valid( 'enumlist', 'blue' )
            check_valid( 'enumlist', 'blue,red' )
            check_valid( 'enumlist', 'blue,red,green' )
            check_not_valid( 'enumlist', ',red,green' )
            # check bool
            check_valid( 'bool', None )
            check_not_valid( 'bool', '' )
            check_valid( 'bool', 'true' )
            check_valid( 'bool', 'false' )
            check_not_valid( 'bool', 'False' )
            # check xy
            check_valid( 'xy', None )
            check_not_valid( 'xy', '' )
            check_valid( 'xy', '0,0' )
            check_not_valid( 'xy', '0,' )
            check_not_valid( 'xy', ',0' )
            check_not_valid( 'xy', '0' )
            check_not_valid( 'xy', '0,0,0' )

# to test:

    unittest.main()
