#Provides a visual representation of a LevelWorld using a QGraphicView

# Notes of caution:
# QRectF right edge is x + width, but QRect right edge is x + width -1


from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import Qt
import math
import louie
import metaworld
import metaworldui
import qthelper
import metawog

ROUND_DIGITS = 2 #Rounds Sizes, Positions, Radii and Angles to 2 Decimal Places
Z_TOOL_ITEMS = 1000000 # makes sure always on top
Z_LEVEL_ITEMS = 10000.0
Z_PHYSIC_ITEMS = 9000.0

TOOL_SELECT = 'select'
TOOL_PAN = 'pan'
TOOL_MOVE = 'move'

# x coordinate of the unit vector of length = 1
UNIT_VECTOR_COORDINATE = math.sqrt(0.5)

KEY_ELEMENT = 0
KEY_TOOL = 1
#@DaB
#Added to allow storage of pre-computed item area to item.data
KEY_AREA = 2
KEY_TYPE = 3
KEY_EXTRA = 4

ELEMENT_STATE_NONE = 0
ELEMENT_STATE_LOCKED = 1
ELEMENT_STATE_INVISIBLE = 2

# Rectangle corner/middle position identifiers
# Each position is made of a tuple (weight0, weight1, weight2, weight3) were
# each weight correspond respectively to the weight of corner
# top-left, top-right, bottom-right and bottom-left respectively.
# The sum of the weight must always be equal to 1.0
POS_TOP_LEFT = (1,0, 0,0)
POS_TOP_CENTER = (0.5,0.5, 0,0)
POS_TOP_RIGHT = (0,1, 0,0)
POS_CENTER_LEFT = (0.5,0, 0,0.5)
POS_CENTER = (0.25,0.25, 0.25,0.25)
POS_CENTER_RIGHT = (0,0.5, 0.5,0)
POS_BOTTOM_LEFT = (0,0, 0,1)
POS_BOTTOM_CENTER = (0,0, 0.5,0.5)
POS_BOTTOM_RIGHT = (0,0, 1,0)
#@DaB
#Positions for Rotate Handles
POS_MID_RIGHT = (0.15,0.35,0.35,0.15)
POS_MID_LEFT = (0.35,0.15,0.15,0.35)

def poly_weighted_pos(rect, weights):
    """Given a rectangle, computes the specified position. 
       @param pos tuple (xmin_weigth,xmax_weigth,ymin_weight,ymax_weight)
    """
    assert sum(weights) == 1.0
    weighted_pos = [rect[index] * weight 
                    for index,weight in enumerate(weights)]
    pos = weighted_pos[0]
    for weigthed in weighted_pos[1:]:
        pos += weigthed
    return pos

traced_level = 0

TRACED_ACTIVE = False

def traced(f):
    """A decorator that print the method name when it is entered and exited."""
    if not TRACED_ACTIVE:
        return f
    if hasattr(f,'im_class'):
        name = '%s.%s' % (f.im_class.__name__,f.__name__)
    else:
        name = '%s' % (f.__name__)
    def traced_call( *args, **kwargs ):
        global traced_level
        print '%sEnter %s' % (traced_level*'  ', name)
        traced_level += 1
        result = f(*args, **kwargs)
        traced_level -= 1
        print '%sExit %s' % (traced_level*'  ', name)
        return result
    return traced_call

def toQPointF(pos):
    x,y = pos
    return QtCore.QPointF(x,y)

def vector2d_length(x, y):
    """Computes the magnitude of vector (x,y)."""
    return math.sqrt(x*x + y*y)

def vector2d_angle(u, v):
    """Computes the angle required to rotate 'u' around the Z axis counter-clockwise
       to be in the direction as 'v'.
       @param u tuple (x,y) representing vector U
       @param v tuple (x,y) representing vector V
       @exception ValueError is raise if either u or v is the null vector.
       @returns Angle between vector 'u' and 'v' in degrees, in range ]-180,180]
    """
    # We have: cos_uv = U.V / (|U|*|V|), where U.V is the scalar product of U and V, 
    # and |U| the magnitude of U.
    # and we have: sin_uv = |U*V| / (|U|*|V|), where U*V is the cross product of U and V
    # U.V = Ux * Vx + Uy * Vy
    # U*V = (0,0,Ux * Vy - Uy * Vx)
    # |U*V| = Ux * Vy - Uy * Vx
    length_uv = vector2d_length(*u) * vector2d_length(*v)
    if length_uv == 0.0:
        raise ValueError( "Can not computed angle between a null vector and another vector." )
    cos_uv = (u[0] * v[0] + u[1] * v[1]) / length_uv
    sign_sin_uv = u[0] * v[1] - u[1] * v[0]
    angle = math.acos( cos_uv )
    if sign_sin_uv < 0:
        angle = -angle
    return math.degrees( angle )  

# Actions:
# left click: do whatever the current tool is selected to do.
#             for move tool, determine move/resize based on click location
# middle click or left+right click: start panning
# right click: context menu selection


class BasicTool(object):
    def __init__(self, view):
        self._view = view
        self._last_event = None
        self._press_event = None
        self.activated = False
        self.in_use = False
        self.override_tool = None
        self._downat = QtCore.QPoint(0,0) #@DaB Added to store original Mouse_Press Location
        self._moveto = QtCore.QPoint(0,0)

    def on_mouse_press_event(self, event):
        """Handles tool overriding with mouse button and dispatch press event.
           Middle click or left+right mouse button override the current tool
           with the PanTool.
        """
        self._downat = event.pos() #@DaB Store Press Location
        if ( (event.buttons() == Qt.MidButton) or 
             (event.buttons() == (Qt.LeftButton|Qt.RightButton)) ):
            if self.override_tool is None:
                self.stop_using_tool()
                self.override_tool = PanTool(self._view)
                self.override_tool._handle_press_event(event)
        elif self.override_tool is None: #@DaB Removed LeftButton Filter
            self._handle_press_event(event)
    
    def _handle_press_event(self, event):
        """Handle press event for the tool."""
        if not self.in_use:
            self.start_using_tool()
        self._press_event = qthelper.clone_mouse_event(event)
        self._last_event = self._press_event 
        self.activated = True
    
    def on_mouse_release_event(self, event):
        if self.override_tool is not None:
            self.override_tool.on_mouse_release_event(event)
            self.override_tool.stop_using_tool()
            self.override_tool = None
        self.start_using_tool()
        self.activated = False
        self._handle_release_event(event)
        self._last_event = qthelper.clone_mouse_event(event)
    
    def _handle_release_event(self, event):
        pass
    
    def on_mouse_move_event(self, event):
        self._moveto = event.pos()
        if self.override_tool is not None:
            self.override_tool.on_mouse_move_event(event)
        self.start_using_tool()
        self._handle_move_event(event)
        self._last_event = qthelper.clone_mouse_event(event)
        

    def _handle_move_event(self, event):
        pass


    def start_using_tool(self):
        if not self.in_use:
            self.in_use = True
            self._on_start_using_tool()

    def stop_using_tool(self):
        if self.in_use:
            self._on_stop_using_tool()
            self._last_event = None
            self.in_use = False

    def _on_start_using_tool(self):
        pass
    
    def _on_stop_using_tool(self):
        pass

    def downat(self):
        return _downat()
    def moveto(self):
        return _moveto()

#@DaB - Used for StrandMode
class SelectTool(BasicTool):
    def __init__(self,view):
        BasicTool.__init__( self, view )
        self._press_element = None
        self._release_element = None

    def _on_start_using_tool(self):
        self._view.viewport().setCursor( Qt.ArrowCursor )
        self._p1 = QtCore.QPointF(0,0)

    def _handle_press_event(self, event):
        BasicTool._handle_press_event( self, event )
        self._press_element = None
        self._release_element = None
        clicked_items = self._view.items( event.pos() )
        if len(clicked_items) > 0:
            selected_index = -1
            for index, item in enumerate(clicked_items):
                if item.data(KEY_TYPE).toString()=='BallInstance':
                    if selected_index==-1:
                        minarea = item.data(KEY_AREA).toFloat()[0]
                        selected_index=index
                    else:
                        itemarea = item.data(KEY_AREA).toFloat()[0]
                        if itemarea<minarea:
                            minarea=itemarea
                            selected_index=index
                
            if selected_index != -1:
                data = clicked_items[selected_index].data( KEY_ELEMENT )
                if data.isValid():
                    self._press_element = data.toPyObject()
                    if self._press_element.get('id')=='' or self._press_element.get('type') in metawog.BALLS_NO_STRANDS:
                        self._press_element = None
                    else:
                        #@DaB: Changed to ensure strands come from the center of Rectangle Goos too!
                        self._p1 = poly_weighted_pos(clicked_items[selected_index].mapToScene( clicked_items[selected_index].boundingRect() ),POS_CENTER)
                        #self._p1 = clicked_items[selected_index].pos()
                        p2 = self._view.mapToScene( event.pos() )
                        self._view._show_band(self._p1,p2)
                        self._view.select_item_element( clicked_items[selected_index] )

    def _handle_move_event(self,event):
        BasicTool._handle_move_event(self, event)
        if self._press_element is not None:
            p2 = self._view.mapToScene( event.pos() )
            self._view._show_band(self._p1,p2)

    def _handle_release_event(self, event):
        BasicTool._handle_release_event( self, event )
        self._view._remove_band()
        clicked_items = self._view.items( event.pos() )
        if len(clicked_items) > 0:
            selected_index = -1
            for index, item in enumerate(clicked_items):
              if item.data(KEY_TYPE).toString()=='BallInstance':
                if selected_index==-1:
                    minarea = item.data(KEY_AREA).toFloat()[0]
                    selected_index=index
                else:
                    itemarea = item.data(KEY_AREA).toFloat()[0]
                    if itemarea<minarea:
                        minarea=itemarea
                        selected_index=index

            if selected_index != -1:
                data = clicked_items[selected_index].data( KEY_ELEMENT )
                if data.isValid():
                    self._release_element = data.toPyObject()
                self._act_on_press_release(self._press_element,self._release_element)
        self._press_element = None
        self._release_element = None

    def _act_on_press_release(self, p_element, r_element):
        if p_element is None or r_element is None:
            return
        #Only Strands at the mo
        if (p_element.tag == 'BallInstance') and (r_element.tag=='BallInstance'):
             gb1 = p_element.get('id')
             gb2 = r_element.get('id')

             if (gb1 == '') or (gb2 ==''):
                QtGui.QMessageBox.warning(self._view, self._view.tr("Empty GooBall Id!"),
                                          self._view.tr('GooBalls must have an id to connect a strand') )
                return

             if (gb1 == gb2):
                return

             for child_element in self._view.world.level_root:
                if child_element.tag=='Strand':
                    egb = child_element.get('gb1')
                    if egb==gb1 or egb==gb2:
                        egb = child_element.get('gb2')
                        if egb==gb1 or egb==gb2:
                            QtGui.QMessageBox.warning(self._view, self._view.tr("Duplicate Strand!"),
                                                      self._view.tr('You can\'t have 2 strands between the same balls') )
                            return

             gt1 = p_element.get('type')
             gt2 = r_element.get('type')
             if gt2 in metawog.BALLS_NO_STRANDS:
                 QtGui.QMessageBox.warning(self._view, self._view.tr("Invalid Strand!"),
                                           self._view.tr('You can\'t connect a strand to a ' + gt2) )
                 return

             if (gt1 in metawog.BALLS_MUST_BE_GB1) and (gt2 in metawog.BALLS_MUST_BE_GB1):
                 QtGui.QMessageBox.warning(self._view, self._view.tr("Invalid Strand!"),
                                           self._view.tr('You can\'t connect a ' + gt1 + ' to a ' + gt2) )
                 return
             elif (gt1 in metawog.BALLS_SHOULD_BE_GB2) or (gt2 in metawog.BALLS_MUST_BE_GB1):
                 # swap 'em
                 tb = gb1
                 gb1 = gb2
                 gb2 = tb
             self._addStrand(gb1,gb2)

    def _addStrand(self, gb1, gb2):
            lr = self._view.world.level_root
            attrib = {'gb1':gb1,'gb2':gb2}
            new_strand = lr.make_child( lr.meta.find_immediate_child_by_tag('Strand'), attrib )
            new_strand.world.set_selection( new_strand )            

class PanTool(BasicTool):
    def _on_start_using_tool(self):
        self._view.viewport().setCursor( Qt.OpenHandCursor )

    def _handle_press_event(self, event):
        BasicTool._handle_press_event(self, event)
        self._view.viewport().setCursor( Qt.ClosedHandCursor )
        return True
#@DaB
#Rewritten to allow Zoom in "Pan" Mode
#Click-Release in same location (<5px delta)
# Left = Zoom-in  Right=Zoom-out    
    def _handle_release_event(self, event):
        delta = event.pos() - self._downat
        if vector2d_length(delta.x(),delta.y())<5:
           factor=0
           if (self._press_event.buttons() == Qt.LeftButton):
               # zoomin
               factor = 1.5
           elif (self._press_event.buttons() == Qt.RightButton):
               #zoomout
               factor = 0.6666
           if factor!=0:
                self._view.zoomView( factor, event.pos().x(), event.pos().y() )

        self._view.viewport().setCursor( Qt.OpenHandCursor )
        return True

#@DaB
#Modified to only Pan at delta >5px
    def _handle_move_event(self, event):
        BasicTool._handle_move_event(self, event)
        if self._last_event and self.activated:
            delta = event.pos() - self._downat
            if vector2d_length(delta.x(),delta.y())>5:
                view = self._view
                h_bar = self._view.horizontalScrollBar()
                v_bar = self._view.verticalScrollBar()
                delta = event.pos() - self._last_event.pos()
                x_value = h_bar.value()
                if view.isRightToLeft():
                    x_value += delta.x()
                else:
                    x_value -= delta.x()
                h_bar.setValue( x_value )
                v_bar.setValue( v_bar.value() - delta.y() )
        return True


class MoveOrResizeTool(BasicTool):
    """
Need to get current selected item to:
- apply transformation to
- detect tool to activate on click (translate, rotate, resize)
    """
    def __init__(self, view):
        BasicTool.__init__( self, view )
        self._active_tool = None


    def _get_tool_for_event_location(self,event):
        item_at_pos = self._view.itemAt( event.pos() )
        if item_at_pos is not None:
            data = item_at_pos.data( KEY_TOOL )
            activated_tool = None
            if data.isValid():
                activated_tool = data.toPyObject()
                return activated_tool
		#@DaB - Allow Move only if the press was ON the item
        return None

    def _showContextMenu(self,event):
        if len(self._view.world.selected_elements)>0:
            for selected_element in self._view.world.selected_elements:
                top_item = selected_element.tag
                break
        else:
            selected_element = None
            top_item = None

        menu = QtGui.QMenu( self._view )
        if top_item:
           menu.addAction( qthelper.action( menu, enabled=False,  text = top_item  ) )
           menu.addSeparator()

        menu.addAction( self._view.common_actions['cut'] )
        menu.addAction( self._view.common_actions['copy'] )
        menu.addAction(  self._view.common_actions['pastehere'] )
        menu.addSeparator()
        menu.addAction( self._view.common_actions['delete'] )

        is_selected = (selected_element is not None)
        self._view.common_actions['cut'].setEnabled(is_selected)
        self._view.common_actions['copy'].setEnabled(is_selected)
        self._view.common_actions['delete'].setEnabled(is_selected)

        # sort out paste to
        menu.exec_( self._view.mapToGlobal(event.pos() ))

    def _handle_press_event(self, event):
        BasicTool._handle_press_event(self, event)
        # Commit previous tool if any
        if self._active_tool:
            self._active_tool.cancel()
            self._active_tool = None
        if event.buttons() != Qt.LeftButton:
            return
        # Find if any tool handle was clicked by user
        activated_tool = self._get_tool_for_event_location( event )
        if activated_tool is None: #@DaB - Missed all the handles
            # compare previous and newly selected element
            if self._new_select_tool(event): #@DaB - Try a Selec
               activated_tool = self._view.get_current_inner_tool()


        if activated_tool is not None:
            self._active_tool = activated_tool 
            scene_pos = self._view.mapToScene( event.pos() )
            self._active_tool.activated( scene_pos.x(), scene_pos.y(),event.modifiers() )
                
    def _new_select_tool(self, event):
        clicked_items = self._view.items( event.pos() )
        if len(clicked_items) > 0:
            selected_index = -1
            for index, item in enumerate(clicked_items):
                area = item.data(KEY_AREA).toFloat()[0]
                if area>0:
                  if selected_index==-1:
                    minarea = item.data(KEY_AREA).toFloat()[0]
                    selected_index=index
                  else:
                    itemarea = item.data(KEY_AREA).toFloat()[0]
                    if itemarea<minarea:
                        minarea=itemarea
                        selected_index=index
            if selected_index >= 0:
                # something was clicked
                oldsel = self._view.world.selected_elements
                data = clicked_items[selected_index].data( KEY_ELEMENT )
                if data.isValid():
                    clicked_element = data.toPyObject()
                else:
                    clicked_element = None
                if clicked_element is not None:
                    if (event.modifiers() & Qt.ControlModifier)==Qt.ControlModifier:
                        self._view.modify_selection( clicked_items[selected_index] )
                    else:
                        if clicked_element in oldsel:
                            return True
                        self._view.select_item_element( clicked_items[selected_index] )
                return False


        self._view.clear_selection()
        return False
        
    def _handle_release_event(self, event):
        if event.button()==Qt.RightButton:
            self._new_select_tool(event) #@DaB - Try a Selec
            self._showContextMenu(event)
            return

        if self._active_tool is not None:
            scene_pos = self._view.mapToScene( event.pos() )
            self._active_tool.commit( scene_pos.x(), scene_pos.y(),event.modifiers() )
            self._active_tool = None
            self._view.viewport().setCursor( Qt.ArrowCursor )
        else:
            self._handle_move_event(event)

    def _handle_move_event(self, event):
        BasicTool._handle_move_event(self, event)
        # If a tool delegate has been activated, then forward all events
        if self._active_tool is not None:
            scene_pos = self._view.mapToScene( event.pos() )
            self._active_tool.on_mouse_move( scene_pos.x(), scene_pos.y() , event.modifiers())
        else:
            # Otherwise try to find if one would be activated and change mouse cursor
            tool = self._get_tool_for_event_location( event )
            if tool is None: # If None, then go back to selection
                self._view.viewport().setCursor( Qt.ArrowCursor )
            else:
                tool.set_activable_mouse_cursor()

# ###################################################################
# ###################################################################
# Tool Delegates
# ###################################################################
# ###################################################################

class ToolDelegate(object):
    """A tool delegate operate on a single attribute of the object. 
       It provides the following features:
       - set mouse icon corresponding to the tool when mouse is hovering over 
         the tool activation location
       - set mouse icon to activated icon when the user press the left mouse button
       - modify the item to match user action when the mouse is moved
       - cancel or commit change to the underlying element attribute.
    """
    def __init__(self, view, element, item, attribute_meta, state_handler,
                 position_is_center = False, activable_cursor = None, 
                 activated_cursor = None):
        self.view = view
        self.element = element
        self.item = item
        self.attribute_meta = attribute_meta
        self.state_handler = state_handler
        self.activable_cursor = activable_cursor
        self.activated_cursor = activated_cursor or activable_cursor
        self.position_is_center = position_is_center
        self._reset()
        
    def _reset(self): 
        self.activation_pos = None
        self.activation_value = None
        self.activation_item_state = None
        
    def set_activable_mouse_cursor(self):
        if self.activable_cursor is not None:
            self.view.viewport().setCursor( self.activable_cursor )
    
    def set_activated_mouse_cursor(self):
        if self.activated_cursor is not None:
            self.view.viewport().setCursor( self.activated_cursor )

    def activated(self, scene_x, scene_y, modifiers):
     #   print 'Activated:', self
        self.set_activated_mouse_cursor()
        item_pos = self.item.mapFromScene( scene_x, scene_y )
#        print 'Activated:', self, item_pos.x(), item_pos.y()
        self.activation_pos = item_pos
        self.last_pos = self.activation_pos
        self.activation_value = self._get_activation_value()
        self.activation_item_state = self.state_handler.get_item_state(self.item)
        self.on_mouse_move( scene_x, scene_y, modifiers, is_activation = True )

    def _get_activation_value(self): #@todo do we really need this ? We never modify the element until commit!
        """Returns the activation value of the element."""
        return self.attribute_meta.get_native( self.element )
        
    def cancel(self):
#        print 'Cancelled:', self
        if self.activation_item_state is not None:
            self.restore_activation_state()
        self._reset()
    
    def restore_activation_state(self):
        assert self.activation_item_state is not None
        self.state_handler.restore_item_state( self.item, self.activation_item_state )
    
    def commit(self, scene_x, scene_y, modifiers):
        attribute_value = self.on_mouse_move( scene_x, scene_y , modifiers)
#        print 'Committed:', self, attribute_value
        if attribute_value is not None:
            attributes_to_update = self._list_attributes_to_update( attribute_value )
            for attribute_meta, value in attributes_to_update:
                # Delay until next event loop: destroying the scene while in event
                # handler makes the application crash
                self.view.delayed_element_property_update( self.element, 
                                                           attribute_meta,
                                                           value )
        self._reset()
        
    def _list_attributes_to_update(self, attribute_value):
        """Returns a list of tuple(attribute_meta, attribute_value).
           Called on commit to update the element attributes.
        """
        return [ (self.attribute_meta, attribute_value) ]
    
    def on_mouse_move(self, scene_x, scene_y, modifiers, is_activation = False):
        item_pos = self.item.mapFromScene( scene_x, scene_y )
        if is_activation:
            self._on_activation( item_pos )
        result = self._on_mouse_move( item_pos , modifiers)
        self.last_pos = item_pos
        return result

    def _on_activation(self, item_pos):
        pass
    
    def _on_mouse_move(self, item_pos,modifiers):
        raise NotImplemented()
        
    def get_item_bound(self):
        if isinstance(self.item, QtGui.QGraphicsPixmapItem):
            width, height = self.item.pixmap().width(), self.item.pixmap().height()
            return QtCore.QRectF( 0, 0, width, height )
		#@DaB
		#boundingRect includes LineWidth so center was wrong by linewidth/2
        elif isinstance(self.item, QtGui.QGraphicsRectItem):
            return self.item.rect()  #Use .rect instead
        return self.item.boundingRect()
        
    def get_item_center_offset(self):
        """Returns the offset of the center in the item coordinates.
           Some item such as pixmap have their position in the top-left corner.
        """
       #@DaB
       #position_is_center is set for Rects and PixMaps (but position is NOT center)
       # 2 Lines - Temporarily Commented out until I can decide if I can unset the flag
       # Without Harming Anything Else

       # if self.position_is_center:
       #     return QtCore.QPointF()
        bounding_rect = self.get_item_bound()
        return bounding_rect.center() - bounding_rect.topLeft()
        
    def get_item_center_offset_in_parent(self):
        """Returns the offset of the center in the parent coordinates.
           Some item such as pixmap have their position in the top-left corner.
        """
        #if self.position_is_center:
        #    return QtCore.QPointF()
        bounding_rect = self.item.mapToParent( self.get_item_bound() )
        center = poly_weighted_pos( bounding_rect, POS_CENTER )
        offset = center - self.item.pos()
#        print 'Pos: %.f,%.f Center: %.f,%.f, Offset: %.f,%.f' % (
#            self.item.pos().x(), self.item.pos().y(),
#            center.x(), center.y(), 
#            offset.x(), offset.y() )
        return offset

    def get_item_center_pos(self):
        return self.item.pos() + self.get_item_center_offset_in_parent()

class MoveToolDelegate(ToolDelegate):
    """This tool allow the user to move the element in its parent (scene or compositegeom).
    """
    def __init__(self, view, element, item, state_handler, position_is_center, attribute_meta, pos_delegate=None):
        ToolDelegate.__init__( self, view, element, item, state_handler, 
                               position_is_center, attribute_meta,
                               activable_cursor = Qt.SizeAllCursor )
        self.rect_position = None
        self.pos_delegate = pos_delegate

    def set_rect_position(self,rect_position):
        assert self.rect_position is None
        self.rect_position = rect_position

    def _get_activation_value(self): #@todo do we really need this ? We never modify the element until commit!
        """Returns the activation value of the element."""
        av = ToolDelegate._get_activation_value(self)
        if av is None and self.pos_delegate is not None:
            av = self.pos_delegate.get_native( self.element )
        return av


    def _on_mouse_move(self, item_pos,modifiers):
        if self.activation_pos is None:
            return None
        # Compute delta between current position and activation click in parent coordinate
        # then impact position with the delta (position is always in parent coordinate).
        parent_pos = self.item.mapToParent(item_pos)
        self.restore_activation_state()
        activation_parent_pos = self.item.mapToParent( self.activation_pos )
        delta_pos = parent_pos - activation_parent_pos
#       
#        print 'Delta: %.2f, %.2f, New pos: %.2f, %.2f' % (delta_pos.x(), delta_pos.y(),
#                                                          pdelta.x(), pdelta.y())
        if (modifiers & Qt.ControlModifier)==Qt.ControlModifier:
            if abs(delta_pos.x())>abs(delta_pos.y()):
                delta_pos = QtCore.QPointF(delta_pos.x(),0)
            else:
                delta_pos = QtCore.QPointF(0,delta_pos.y())
                
        new_pos = self.item.pos() + delta_pos
        #Special Case for handling Pipe Vertexes since they need to be
        #exactly Horizontal or Vertical to be display correctly in the game
        if self.item.data(KEY_TYPE).toString()=='Vertex':
            data = self.item.data( KEY_ELEMENT )
            if data.isValid():
                velement = data.toPyObject()
                ei = velement.index_in_parent()
                # First Vertex (Pipe End) allow to move freely.. otherwise
                if ei!=0:
                    return self._moveVertex(new_pos,velement)

        self.item.setPos( new_pos )
        self.view._update_tools_handle()        
        if self.activation_value is None: # Default value to (0,0)
            return delta_pos.x(), -delta_pos.y()
        #@DaB - Round position atrributes
	element_pos_x = round((self.activation_value[0] + delta_pos.x()),ROUND_DIGITS)
        element_pos_y = round((self.activation_value[1] - delta_pos.y()),ROUND_DIGITS)
        return element_pos_x, element_pos_y 

    def _moveVertex(self,new_pos, velement):
          pchild = velement.previous_element()
          prefx, prefy = pchild.get_native( 'pos', (0.0,0.0) )
          ppchild = pchild.previous_element()
          ddx = 1
          ddy = 1
          if ppchild!=velement.parent:
              # limit to a single dimension
              pprefx,pprefy = ppchild.get_native('pos',(0.0,0.0))
              if abs(prefx-pprefx) >abs(prefy-pprefy):
                ddx = 0
              else:
                ddy = 0

          dx = abs(new_pos.x()- prefx)*ddx
          dy = abs(new_pos.y()+ prefy)*ddy
          if dx >= dy:
              new_pos = QtCore.QPointF(new_pos.x(),-prefy)
          else:
              new_pos = QtCore.QPointF(prefx,new_pos.y())

          #Offset center of Item because of Line Width
          new_item_pos = new_pos + QtCore.QPointF(-5,-5)

          self.item.setPos( new_item_pos )
          self.view._update_tools_handle()

          if self.activation_value is None: # Default value to (0,0)
               return new_pos.x(), -new_pos.y()
        #@DaB - Round position atrributes
          element_pos_x = round(new_pos.x(),ROUND_DIGITS)
          element_pos_y = -round(new_pos.y(),ROUND_DIGITS)
          return element_pos_x, element_pos_y

class MultiMoveToolDelegate(ToolDelegate):
    """This tool allow the user to move the element in its parent (scene or compositegeom).
    """
    def __init__(self, view, items, position_is_center):
        self.rect_position = None
        self.view = view
        self.items = set()
        self.element = {}
        self.attribute_meta = {}
        self.state_handler = {}
        for item in items:
         if item is not None:
          if item.data(KEY_ELEMENT).isValid():
            element = item.data(KEY_ELEMENT).toPyObject()
            attrib = []
            for attribute_meta in element.meta.attributes:
              if attribute_meta.type == metaworld.XY_TYPE:
                 if attribute_meta.position:
                   attrib.append(attribute_meta)
                 elif attribute_meta.name=='imagepos':
                   attrib.append(attribute_meta)

            if len(attrib)>0:
                if item not in self.items:
                    self.items.add(item)
                    self.attribute_meta[item] = []
                    self.attribute_meta[item].extend(attrib)
                    self.element[item] = element
                    self.state_handler[item]=self._get_state_manager(item)

        for item in list( self.items ):
            if self.element[item].parent in self.element.values():
                # this items parent was also selected...
                # remove this item
                self.items.remove(item)
        
        self.activable_cursor = Qt.SizeAllCursor
        self.activated_cursor = Qt.SizeAllCursor
        self.position_is_center = position_is_center
        self._reset()

    def _get_state_manager(self,item):
        """Returns the state manager used to save and restore the state of the item."""
        if isinstance( item, QtGui.QGraphicsEllipseItem ):
                return EllipseStateManager()
        elif isinstance( item, QtGui.QGraphicsPixmapItem ):
                return PixmapStateManager()
        elif isinstance( item, QtGui.QGraphicsRectItem):
                return RectStateManager()
        elif isinstance( item, QtGui.QGraphicsLineItem):
                return LineStateManager()
        elif isinstance( item, QtGui.QGraphicsTextItem):
                return TextStateManager()
        elif isinstance( item, QtGui.QGraphicsItemGroup):
                return GroupStateManager()
        return None

    def _reset(self):
        self.activation_pos = {}
        self.activation_value = {}
        self.activation_item_state = {}

    def set_activable_mouse_cursor(self):
        if self.activable_cursor is not None:
            self.view.viewport().setCursor( self.activable_cursor )

    def set_activated_mouse_cursor(self):
        if self.activated_cursor is not None:
            self.view.viewport().setCursor( self.activated_cursor )

    def activated(self, scene_x, scene_y, modifiers):
        self.set_activated_mouse_cursor()
        for item in self.items:
            self.activation_pos[item] = item.mapFromScene( scene_x, scene_y )
            self.activation_item_state[item] = self.state_handler[item].get_item_state(item)
        self.on_mouse_move( scene_x, scene_y, modifiers, is_activation = True )

    def cancel(self):
#        print 'Cancelled:', self
        for item in self.items:
            if self.activation_item_state[item] is not None:
                self.restore_activation_state(item)
        self._reset()

    def restore_activation_state(self,item):
        assert self.activation_item_state[item] is not None
        self.state_handler[item].restore_item_state( item, self.activation_item_state[item] )

    def commit(self, scene_x, scene_y, modifiers):
        for item in self.items:
          attribute_value = self._on_mouse_move( item, scene_x, scene_y , modifiers)
          if attribute_value is not None:

             for attribute in self.attribute_meta[item]:
                activation_value = attribute.get_native( self.element[item], None )
                if activation_value:
                    newvalue = (round(activation_value[0]+attribute_value[0],ROUND_DIGITS),
                                round(activation_value[1]+attribute_value[1],ROUND_DIGITS))

                    self.view.delayed_element_property_update( self.element[item],
                                                        attribute,
                                                        newvalue )
        self._reset()


    def on_mouse_move(self, scene_x, scene_y, modifiers, is_activation = False):
        for item in self.items:
           result = self._on_mouse_move( item, scene_x, scene_y , modifiers)
        return

    def _on_mouse_move(self, item, scene_x,scene_y,modifiers):
        # Compute delta between current position and activation click in parent coordinate
        # then impact position with the delta (position is always in parent coordinate).
        parent_pos = item.mapToParent(item.mapFromScene(scene_x,scene_y))
        self.restore_activation_state(item)
        delta_pos = parent_pos - item.mapToParent( self.activation_pos[item] )
        if (modifiers & Qt.ControlModifier)==Qt.ControlModifier:
            if abs(delta_pos.x())>abs(delta_pos.y()):
                delta_pos = QtCore.QPointF(delta_pos.x(),0)
            else:
                delta_pos = QtCore.QPointF(0,delta_pos.y())
        item.setPos( item.pos() + delta_pos )
        return delta_pos.x(), -delta_pos.y()

class LineToolDelegate(ToolDelegate):
    def __init__(self, view, element, item, state_handler, position_is_center,
                 attribute_meta):
        ToolDelegate.__init__( self, view, element, item, state_handler,
                               position_is_center, attribute_meta,
                               activable_cursor = Qt.SizeBDiagCursor )

    def set_rect_position(self,position):
        pass

    def _on_mouse_move(self, item_pos,modifiers):
        #item will be combined item
        dl = item_pos-self.activation_pos
        dx = self.activation_value[0] + dl.x()*0.02
        dy = self.activation_value[1] - dl.y()*0.02
        oline = self.item.childItems()[1].line()
        self.item.childItems()[1].setLine(oline.x1(),oline.y1(), item_pos.x(),item_pos.y() )

        try:
            angle = vector2d_angle( (self.activation_value[0], self.activation_value[1]),
                                    (dx, dy) )
        except ValueError:
            return None # Current mouse position is the Null vector @todo makes this last value

        transform = QtGui.QTransform()
        transform.translate( oline.x1(), oline.y1() )
        transform.rotate( -angle )
        transform.translate(-oline.x1(), -oline.y1() )
        self.item.childItems()[0].setTransform( transform )

        # return stuff

        self.view._update_tools_handle()
        l = vector2d_length(dx,dy)
        if l==0:
            dx = 1
            dy = 0
        else:
            dx = dx / l
            dy = dy / l
    
        return round(dx,ROUND_DIGITS+3),round(dy,ROUND_DIGITS+3)


class RotateToolDelegate(ToolDelegate):
    """This tool allow the user to rotate the element around its center.
    """
    def __init__(self, view, element, item, state_handler, position_is_center, 
                 attribute_angle, attribute_scale):
        ToolDelegate.__init__( self, view, element, item, state_handler, 
                               position_is_center, attribute_angle,
                               activable_cursor = Qt.ArrowCursor )
        self.attribute_scale = attribute_scale

    def _get_activation_value(self): #@todo do we really need this ? We never modify the element until commit!
        """Returns the activation value of the element."""
        if self.element.tag=='BallInstance':
           #hack to keep ball images the same size... during rotate
           #since BallInstance doesn't have a scale attribute
           #the scale is stored in the body part
           try:
              image_info = metawog.BALLS_IMAGES[self.element.get('type')]
              scale_attrib = (image_info[1],image_info[1])
           except KeyError:
              scale_attrib = None
        else:
          try:
            scale_attrib = self.attribute_scale.get_native( self.element )
          except:
            scale_attrib = None
        return ( ToolDelegate._get_activation_value(self) , scale_attrib )

    def _on_mouse_move(self, item_pos,modifiers):
        # Map current, activation position in parent coordinate
        # Compute angle vectors based on the item center in parent coordinate
        # The angle between both vectors give the angle of rotation to apply
        parent_pos = self.item.mapToParent( item_pos )
        self.restore_activation_state()
        center_pos = self.get_item_center_pos()
        activation_vector = self.item.mapToParent( self.activation_pos ) - center_pos
        new_vector = parent_pos - center_pos 
        if activation_vector.isNull():
            activation_vector = QtCore.QPointF( 1.0, 0 ) # arbitrary move it
        try:
            angle = vector2d_angle( (activation_vector.x(), activation_vector.y()),
                                    (new_vector.x(), new_vector.y()) )
        except ValueError:
            return None # Current mouse position is the Null vector @todo makes this last value
#        print '  Activation: %.2f, %.2f Current: %.2f, %.2f Parent: %.f, %.f Center: %.f, %.f' % (
#            activation_vector.x(), activation_vector.y(), 
#            new_vector.x(), new_vector.y(),
#            parent_pos.x(), parent_pos.y(),
#            center_pos.x(), center_pos.y() )
        # Rotates around the item center
        center_offset = self.get_item_center_offset()
        if self.activation_value[1] is not None: #item has scale attribute
            center_offset = QtCore.QPointF( center_offset.x() * self.activation_value[1][0], center_offset.y() * self.activation_value[1][1])

        # for this to work right....
        # the next bit of code
        # has to EXACTLY match the code / transform sequence that generated the rect / pixmap in the first place
        # took me a while to figure out that it didn't!... does now!
        transform = QtGui.QTransform()
        transform.translate( center_offset.x(), center_offset.y() )
        transform.rotate( -((self.activation_value[0] or 0.0) -angle) )
        transform.translate( -center_offset.x(), -center_offset.y() )
        if self.activation_value[1] is not None:
            transform.scale(self.activation_value[1][0],self.activation_value[1][1])
        self.item.setTransform( transform )

        finalx = center_pos.x() - center_offset.x()
        finaly = center_pos.y() - center_offset.y()
        self.item.setPos( finalx, finaly )
        
        self.view._update_tools_handle()
        return round((self.activation_value[0] or 0.0) -angle,ROUND_DIGITS)

class MoveAndScaleToolDelegate(ToolDelegate):
    """This tool allow the user to move the corner of a pixmap.
       It will automatically center position and scale factor accordingly.
    """
    def __init__(self, view, element, item, state_handler, position_is_center, 
                 attribute_center, attribute_scale, center_delegate=None):
        ToolDelegate.__init__( self, view, element, item, state_handler, 
                               position_is_center, attribute_center,
                               activable_cursor = Qt.SizeBDiagCursor )
        self.attribute_scale = attribute_scale
        self.center_delegate = center_delegate
        self.rect_position = None 
        
    def set_rect_position(self,rect_position):
        assert self.rect_position is None
        self.rect_position = rect_position 

    def _get_activation_value(self): #@todo do we really need this ? We never modify the element until commit!
        """Returns the activation value of the element."""
        av = ToolDelegate._get_activation_value(self)
        if av is None and self.center_delegate is not None:
            av = self.center_delegate.get_native( self.element )
        return ( av,
                 self.attribute_scale.get_native( self.element ) )
        
    def _list_attributes_to_update(self, attribute_value):
        """Returns a list of tuple(attribute_meta, attribute_value).
           Called on commit to update the element attributes.
        """
        return [(self.attribute_meta, attribute_value[0]),
                (self.attribute_scale, attribute_value[1])]
        
    def _on_mouse_move(self, item_pos,modifiers):
        if self.activation_pos is None:
            return None
        if self.activation_value is None:
            return [(0,0),(scale_x, scale_y)]

        activation_pos, activation_scale = self.activation_value
        if activation_scale is None:
            activation_scale = (1,1)

        # item pos is in coordinate system at scale 1 (= to image size)
        # First, map the position to parent coordinate. Restore original item
        # state and map the position back to item coordinate.
        # Then in item coordinate, compare new size to original size
        # => this give us the scale factor
        # From the new item bound, compute the center and map it to parent coordinate.
        # Then determine if the item position needs to be modified (
        # center is center position, or dragging top or left corners).
        parent_item_pos = self.item.mapToParent( item_pos )
        self.restore_activation_state()
        item_pos = self.item.mapFromParent( parent_item_pos )
        
        # Computes new item bounds
        bound_rect = self.get_item_bound()
        if not bound_rect.isValid(): # has 0 width or height ?
            return None
        xminmax = [bound_rect.x(), bound_rect.right()]  
        yminmax = [bound_rect.y(), bound_rect.bottom()]

        oldcenter = self.item.mapToParent(QtCore.QPointF((bound_rect.left()+bound_rect.right())*0.5,(bound_rect.top()+bound_rect.bottom())*0.5))
        # x/y minmax impacted indexes by position
        old_width, old_height = xminmax[1] - xminmax[0], yminmax[1] - yminmax[0]

        impacts_by_position = { # tuple(xindex,yindex)
            POS_TOP_LEFT: (0,0,2,2),
            POS_TOP_CENTER: (None,0,1,2),
            POS_TOP_RIGHT: (1,0,0,2),
            POS_CENTER_LEFT: (0,None,2,1),
            POS_CENTER: (None,None,1,1),
            POS_CENTER_RIGHT: (1,None,0,1),
            POS_BOTTOM_LEFT: (0,1,2,0),
            POS_BOTTOM_CENTER: (None,1,1,0),
            POS_BOTTOM_RIGHT: (1,1,0,0),
            }
        impact = impacts_by_position[self.rect_position]
        assert impact is not None
        if impact[0] is not None:
            xminmax[ impact[0] ] = item_pos.x()
        if impact[1] is not None:
            yminmax[ impact[1] ] = item_pos.y()
        new_width, new_height = xminmax[1] - xminmax[0], yminmax[1] - yminmax[0]
        if new_width <= 0.0 or new_height <= 0.0:
            return None
        # Computes scale factor
        scale_x = new_width / bound_rect.width()
        scale_y = new_height / bound_rect.height()
        if (modifiers & Qt.AltModifier)==Qt.AltModifier:
            if scale_x<scale_y:
               # use scale_x
               scale_y = scale_x
               if impact[1]==0: #moving top
                    yminmax[0] = yminmax[1]-old_height*scale_y
               else: # moving bottom
                    yminmax[1] = yminmax[0]+old_height*scale_y
            else:
               # use scale_y
               scale_x = scale_y
               if impact[0]==0: #moving top
                    xminmax[0] = xminmax[1]-old_width*scale_x
               else: # moving bottom
                    xminmax[1] = xminmax[0]+old_width*scale_x

        elif (modifiers & Qt.ControlModifier)==Qt.ControlModifier:
            real_scalex = activation_scale[0]*scale_x
            real_scaley = activation_scale[1]*scale_y

            if real_scalex<real_scaley:
               # use scale_x
               scale_y = real_scalex/activation_scale[1]
               if impact[1]==0: #moving top
                    yminmax[0] = yminmax[1]-old_height*scale_y
               else: # moving bottom
                    yminmax[1] = yminmax[0]+old_height*scale_y
            else:
               # use scale_y
               scale_x = real_scaley/activation_scale[0]
               if impact[0]==0: #moving top
                    xminmax[0] = xminmax[1]-old_width*scale_x
               else: # moving bottom
                    xminmax[1] = xminmax[0]+old_width*scale_x

        newcenter = self.item.mapToParent(QtCore.QPointF((xminmax[0]+xminmax[1])*0.5,(yminmax[0]+yminmax[1])*0.5))
        deltacenter = newcenter-oldcenter


        newpos = self.item.pos()
        newposx = newpos.x() + impact[2] * deltacenter.x()
        newposy = newpos.y() + impact[3] * deltacenter.y()
        self.item.setPos(QtCore.QPointF(newposx,newposy))
        self.item.scale( scale_x, scale_y )
        self.view._update_tools_handle()

        #@DaB - Round position and scale atrributes
        new_scale_x = round(activation_scale[0] * scale_x,ROUND_DIGITS+1)
        new_scale_y = round(activation_scale[1] * scale_y,ROUND_DIGITS+1)
        new_pos_x = round((activation_pos[0] + deltacenter.x()),ROUND_DIGITS)
        new_pos_y = round((activation_pos[1] - deltacenter.y()),ROUND_DIGITS)
        return [(new_pos_x,new_pos_y), (new_scale_x, new_scale_y)]
        
class DXDYToolDelegate(ToolDelegate):
    def __init__(self, view, element, item, state_handler, position_is_center,
                 attribute_meta):
        ToolDelegate.__init__( self, view, element, item, state_handler,
                               position_is_center, attribute_meta,
                               activable_cursor = Qt.SizeBDiagCursor )

    def set_rect_position(self,position):
        pass

    def _on_mouse_move(self, item_pos,modifiers):
        dl = item_pos-self.activation_pos
        dx = self.activation_value[0] + dl.x()*0.05
        dy = self.activation_value[1] - dl.y()*0.05
        oline = self.item.line()
        self.item.setLine(oline.x1(),oline.y1(), item_pos.x(),item_pos.y() )
        self.view._update_tools_handle()
        return round(dx,ROUND_DIGITS),round(dy,ROUND_DIGITS)

class ResizeToolDelegate(ToolDelegate):
    def __init__(self, view, element, item, state_handler, position_is_center, attribute_center, attribute_size):
        ToolDelegate.__init__( self, view, element, item, state_handler,
                               position_is_center, attribute_center,
                               activable_cursor = Qt.SizeBDiagCursor )
        self.attribute_size = attribute_size
        self.rect_position = None

    def set_rect_position(self,rect_position):
        assert self.rect_position is None
        self.rect_position = rect_position

    def _get_activation_value(self): #@todo do we really need this ? We never modify the element until commit!
        """Returns the activation value of the element."""
        return ( ToolDelegate._get_activation_value(self),
                 self.attribute_size.get_native( self.element ) )

    def _list_attributes_to_update(self, attribute_value):
        """Returns a list of tuple(attribute_meta, attribute_value).
           Called on commit to update the element attributes.
        """
        return [(self.attribute_meta, attribute_value[0]),
                (self.attribute_size, attribute_value[1])]

    def _on_mouse_move(self, item_pos,modifiers):
        if self.activation_pos is None:
            return None
        # item pos is in coordinate system at scale 1 (= to image size)
        # First, map the position to parent coordinate. Restore original item
        # state and map the position back to item coordinate.
        # Then in item coordinate, compare new size to original size
        # => this give us the scale factor
        # From the new item bound, compute the center and map it to parent coordinate.
        # Then determine if the item position needs to be modified (
        # center is center position, or dragging top or left corners).
        parent_item_pos = self.item.mapToParent( item_pos )
        self.restore_activation_state()
        item_pos = self.item.mapFromParent( parent_item_pos )

        # Computes new item bounds
        bound_rect = self.get_item_bound()
        if not bound_rect.isValid(): # has 0 width or height ?
            return None
        xminmax = [bound_rect.x(), bound_rect.right()]
        yminmax = [bound_rect.y(), bound_rect.bottom()]
        oldcenter = self.item.mapToParent(QtCore.QPointF((bound_rect.left()+bound_rect.right())*0.5,(bound_rect.top()+bound_rect.bottom())*0.5))
        # x/y minmax impacted indexes by position
        impacts_by_position = { # tuple(xindex,yindex)
            POS_TOP_LEFT: (0,0,2,2),
            POS_TOP_CENTER: (None,0,1,2),
            POS_TOP_RIGHT: (1,0,0,2),
            POS_CENTER_LEFT: (0,None,2,1),
            POS_CENTER: (None,None,1,1),
            POS_CENTER_RIGHT: (1,None,0,1),
            POS_BOTTOM_LEFT: (0,1,2,0),
            POS_BOTTOM_CENTER: (None,1,1,0),
            POS_BOTTOM_RIGHT: (1,1,0,0),
            }
        impact = impacts_by_position[self.rect_position]
        assert impact is not None

        if impact[0] is not None:
            xminmax[ impact[0] ] = item_pos.x()
        if impact[1] is not None:
            yminmax[ impact[1] ] = item_pos.y()

        new_width = xminmax[1] - xminmax[0]
        new_height = yminmax[1] - yminmax[0]
        if new_width <= 0.0 or new_height <= 0.0:
            return None

        scale_x = new_width / bound_rect.width()
        scale_y = new_height / bound_rect.height()


        newcenter = self.item.mapToParent(QtCore.QPointF((xminmax[0]+xminmax[1])*0.5,(yminmax[0]+yminmax[1])*0.5))
        deltacenter = newcenter-oldcenter

        # Computes scale factor

        self.item.scale( scale_x, scale_y )
        #self.item.rect().setWidth(new_width)
        #self.item.rect().setHeight(new_height)

        newpos = self.item.pos()
        newposx = newpos.x() + impact[2] * deltacenter.x()
        newposy = newpos.y() + impact[3] * deltacenter.y()
        self.item.setPos(QtCore.QPointF(newposx,newposy))
        self.view._update_tools_handle()

        if self.activation_value is None:
            return [(0,0),(scale_x, scale_y)]
        activation_pos, activation_size = self.activation_value
        if activation_pos is None:
            activation_pos = (0,0)
        if activation_size is None:
            activation_size = (5,5)
        new_size_x = round(activation_size[0] * scale_x,ROUND_DIGITS)
        new_size_y = round(activation_size[1] * scale_y,ROUND_DIGITS)
        new_pos_x = round(activation_pos[0] + deltacenter.x(),ROUND_DIGITS)
        new_pos_y = round(activation_pos[1] - deltacenter.y(),ROUND_DIGITS)
        return [(new_pos_x,new_pos_y), (new_size_x, new_size_y)]

class RadiusToolDelegate(ToolDelegate):
    def __init__(self, view, element, item, state_handler, position_is_center, 
                 attribute_meta):
        ToolDelegate.__init__( self, view, element, item, state_handler, 
                               position_is_center, attribute_meta,
                               activable_cursor = Qt.SizeBDiagCursor )
        
    def set_rect_position(self,position):
        pass
        
    def _on_mouse_move(self, item_pos,modifiers):
        # Circle are always zero centered, hence we can just get the current distance
        # from the origin to get the current length.
        # The difference between the current length and the activation length is
        # used to impact the radius as it was at the time of the activation
        r_pos = vector2d_length( item_pos.x(), item_pos.y() )
        r_activation = vector2d_length( self.activation_pos.x(),
                                        self.activation_pos.y() )
        r = round(abs(self.activation_value + r_pos - r_activation),ROUND_DIGITS)
        self.item.setRect( -r, -r, r*2, r*2 )
        self.view._update_tools_handle()
        return r


# ###################################################################
# ###################################################################
# Tools Factories
# ###################################################################
# ###################################################################
# Tool selector needs to handle:
# Move: inside
# Resize: 
# - on rectangle: 4 corners, rely on shift modifier to force horizontal/vertical only
# - on circle: 4 middle crossing the axis
# Rotate: 
# - on rectangle: 4 middle handles
# - on circle: 4 handles spread over at 45 degrees
# Using scene item has handle implies:
# -> Creates/destroy item on selection change, hide them during operation
# -> associates them with selected item
# -> compute handle position in item coordinate and map to scene
# -> handle should show no direction (avoid rotation transformation issue)
#
class ToolsFactory(object):
    """Responsible for creating and positioning the "handle" items used to
       activate tools (rotate, resize...) when clicked.
    """  
    
    def get_pixel_length(self, view):
        """Returns the length of a pixel in scene unit."""
        origin_pos = view.mapToScene( QtCore.QPoint() )
        f = 10000
        unit_pos = view.mapToScene( QtCore.QPoint( UNIT_VECTOR_COORDINATE*f,
                                                   UNIT_VECTOR_COORDINATE*f ) )
        unit_vector = (unit_pos - origin_pos) / f
        unit_length = vector2d_length( unit_vector.x(), unit_vector.y() )
        return unit_length

    
    def create_tools(self, item, element, view ):
        """Creates graphic scene item representing the tool handle.
           Returns: tuple(move_tool, list of items)
        """
        self._make_tools( item, element, view, self._get_state_manager() )
        resize_tool_pos, rotate_tool_pos = self._get_tools_positions()
        bouding_poly = item.mapToScene( item.boundingRect() )
#        for index in range(0,4):
#            pos = bouding_poly[index]
#            print 'Bounding poly [%d] = %.2f, %.2f' % (index, pos.x(), pos.y())
        items = []
        pixel_length = self.get_pixel_length( view )
        for tool_index, positions in enumerate( (resize_tool_pos, rotate_tool_pos) ):
            for pos_index, position in enumerate( positions ):
                pos = poly_weighted_pos( bouding_poly, position )
#                print 'Tool %d, index %d %s: %.2f,%.2f' % (tool_index, pos_index, position,
#                                                           pos.x(), pos.y() ) 
                size = QtCore.QPointF( 3*pixel_length, 3*pixel_length )
                bound = QtCore.QRectF( pos - size, pos + size )
                if tool_index == 0 and self.resize_tools is not None:
                    tool_item = view.scene().addRect( bound )
                    tool = self.resize_tools[pos_index]
                    tool.set_rect_position( position )
                elif tool_index == 1 and self.rotate_tools is not None:
                    tool_item = view.scene().addEllipse( bound )
                    tool = self.rotate_tools[pos_index]
                else:
                    tool_item = None
                if tool_item is not None:
                    tool_item.setZValue( Z_TOOL_ITEMS )
                    tool_item.setData( KEY_TOOL, QtCore.QVariant( tool ) )
                    tool_item.setData(KEY_TYPE,QtCore.QVariant('TOOL'))
                    items.append( tool_item )
        return self.move_tool, items

    def _make_tools(self, item, element, view, state_manager):
        attribute_radius = None
        attribute_center = None
        attribute_size = None
        attribute_scale = None
        attribute_angle = None
        attribute_dxdy = None
        for attribute_meta in element.meta.attributes:
            if attribute_meta.type == metaworld.RADIUS_TYPE:
                attribute_radius = attribute_meta
            elif attribute_meta.type == metaworld.XY_TYPE:
                attribute_center = attribute_center or attribute_meta
            elif attribute_meta.type == metaworld.SIZE_TYPE:
                attribute_size = attribute_size or attribute_meta
            elif attribute_meta.type == metaworld.SCALE_TYPE:
                attribute_scale = attribute_scale or attribute_meta
            elif attribute_meta.type == metaworld.DXDY_TYPE:
                attribute_dxdy = attribute_dxdy or attribute_meta
            elif attribute_meta.type in (metaworld.ANGLE_DEGREES_TYPE, 
                                         metaworld.ANGLE_RADIANS_TYPE):
                attribute_angle = attribute_angle or attribute_meta
        position_is_center = not self._center_is_top_left()
        def make_delegate( type, attribute, *args ):
            if attribute is not None:
                return type( view, element, item, attribute, state_manager, 
                             position_is_center, *args )
            return None
        self.move_tool = make_delegate( MoveToolDelegate, attribute_center )
        self.resize_tools = None
        if attribute_radius is not None:
            self.resize_tools = [ make_delegate( RadiusToolDelegate, 
                                                 attribute_radius )
                                  for i in range(0,4) ]
# @todo Restore this once resizing is implemented
        elif attribute_size is not None:
            self.resize_tools =  [ make_delegate( ResizeToolDelegate, attribute_center,
                                                 attribute_size )   for i in range(0,4) ]
        elif attribute_scale is not None:
            self.resize_tools = [ make_delegate( MoveAndScaleToolDelegate, attribute_center,
                                                 attribute_scale ) 
                                  for i in range(0,4) ]
        elif attribute_dxdy is not None:
            self.resize_tools = [ make_delegate( DXDYToolDelegate, attribute_dxdy )
                                   for i in range(0,4) ]

        if attribute_angle is not None:
            self.rotate_tools = [ make_delegate(RotateToolDelegate, attribute_angle, attribute_scale)
                                  for i in range(0,4) ]
        else:
            self.rotate_tools = None 

    def _get_tools_positions(self):
        """Returns the of positions for the resize tools and the rotate tools."""
        raise NotImplemented()

    def _get_state_manager(self):
        """Returns the state manager used to save and restore the state of the item."""
        raise NotImplemented()

    def _center_is_top_left(self):
        """Indicates if the item position represents the center or the top-left corner of
           the bounding box.
        """
        return False

class CircleToolsFactory(ToolsFactory):

    def _get_tools_positions(self):
        """Returns the of positions for the resize tools and the rotate tools."""
        resize_tool_pos = ( POS_TOP_CENTER, POS_BOTTOM_CENTER,
                            POS_CENTER_LEFT, POS_CENTER_RIGHT ) 
#        rotate_tool_pos = ( POS_TOP_LEFT, POS_TOP_RIGHT,
#                            POS_BOTTOM_RIGHT, POS_BOTTOM_LEFT )
        return  resize_tool_pos, ()

    def _get_state_manager(self):
        """Returns the state manager used to save and restore the state of the item."""
        return EllipseStateManager()

class PixmapToolsFactory(ToolsFactory):
    """Pixmap may be either circle with image, rectangle with image, SceneLayer or
       part of a compositgeom.
    """

    def _get_tools_positions(self):
        """Returns the of positions for the resize tools and the rotate tools."""
        rotate_tool_pos = ( POS_CENTER_LEFT, POS_CENTER_RIGHT )
        resize_tool_pos = ( POS_TOP_LEFT,POS_BOTTOM_RIGHT )
        return  resize_tool_pos, rotate_tool_pos 

    def _get_state_manager(self):
        """Returns the state manager used to save and restore the state of the item."""
        return PixmapStateManager()

    def _center_is_top_left(self):
        return True

class BallToolsFactory(ToolsFactory):
    """Pixmap may be either circle with image, rectangle with image, SceneLayer or
       part of a compositgeom.
    """
    def create_tools(self, item, element, view ):
        self.ball_type = element.get('type')
        return ToolsFactory.create_tools(self,item,element,view)

    def _get_tools_positions(self):
        """Returns the of positions for the resize tools and the rotate tools."""
        if self.ball_type is None or self.ball_type=='':
            rotate_tool_pos=()
        else:
            try:
                ball_shape = metawog.BALLS_SHAPES[self.ball_type]
                if ball_shape[0]=='circle':
                    rotate_tool_pos=()
                else:
                    rotate_tool_pos = ( POS_CENTER_LEFT, POS_CENTER_RIGHT )
            except KeyError:
                rotate_tool_pos=()

        return (), rotate_tool_pos

    def _get_state_manager(self):
        """Returns the state manager used to save and restore the state of the item."""
        return PixmapStateManager()

    def _center_is_top_left(self):
        return True

class MultiMoveToolsFactory(ToolsFactory):

    def create_tools(self, items, view ):
        """Creates graphic scene item representing the tool handle.
           Returns: tuple(move_tool, list of items)
        """
        self.move_tool = MultiMoveToolDelegate(view,items,False)
        return self.move_tool, []


class RectangleToolsFactory(ToolsFactory):

    def _get_tools_positions(self):
        """Returns the of positions for the resize tools and the rotate tools."""
        rotate_tool_pos = ( POS_CENTER_LEFT, POS_CENTER_RIGHT )
        resize_tool_pos = ( POS_TOP_LEFT,POS_BOTTOM_RIGHT )
        return  resize_tool_pos, rotate_tool_pos

    def _get_state_manager(self):
        """Returns the state manager used to save and restore the state of the item."""
        return RectStateManager()

class LineToolsFactory(ToolsFactory):

    def _get_tools_positions(self):
        """Returns the of positions for the resize tools and the rotate tools."""
        rotate_tool_pos = ( POS_CENTER_LEFT, POS_CENTER_RIGHT )
        resize_tool_pos = ( )
        return  resize_tool_pos, rotate_tool_pos

    def _get_state_manager(self):
        """Returns the state manager used to save and restore the state of the item."""
        return LineStateManager()

class TextToolsFactory(ToolsFactory):     

    def _get_tools_positions(self):
        """Returns the of positions for the resize tools and the rotate tools."""
        rotate_tool_pos = ( POS_MID_RIGHT, POS_MID_RIGHT )
        return  (), rotate_tool_pos

    def _get_state_manager(self):
        """Returns the state manager used to save and restore the state of the item."""
        return TextStateManager()

#@DaB Different framwork for Group Items... because...
# Different SubItems require different tools
# Tool Positions need to based on subitem bounding rect, not combined item

# returns the 1 default movetool and list of other tools
class GroupToolsFactory(ToolsFactory):

    def _build_tool(self,view,pos,position,size,type,tool, fillcolor=QtGui.QColor(0,0,0)):
        bound = QtCore.QRectF( pos - size, pos + size )
        if type == 'rect':
            tool_item = view.scene().addRect( bound )
            tool.set_rect_position( position )
        elif type == 'rectfill':
            pen = QtGui.QPen(QtGui.QColor(255,255,255))
            xbrush = QtGui.QBrush( fillcolor,Qt.SolidPattern )
            tool_item = view.scene().addRect( bound ,pen, xbrush)
            tool.set_rect_position( position )
        elif type=='circle':
            tool_item = view.scene().addEllipse( bound )
        elif type=='circlefill':
            pen = QtGui.QPen(QtGui.QColor(255,255,255))
            xbrush = QtGui.QBrush( fillcolor,Qt.SolidPattern )
            tool_item = view.scene().addEllipse( bound,pen,xbrush )
        else:
            tool_item = None
        if tool_item is not None:
            tool_item.setZValue( Z_TOOL_ITEMS )
            tool_item.setData( KEY_TOOL, QtCore.QVariant( tool ) )
            tool_item.setData(KEY_TYPE,QtCore.QVariant('TOOL'))
        return tool_item

class RectangleGroupToolFactory(GroupToolsFactory):

    def create_tools(self, item, element, view ):
        """Creates graphic scene item representing the tool handle.
           Returns: tuple(move_tool, list of items)
        """
        items = []
        pixel_length = self.get_pixel_length( view )
        size = QtCore.QPointF( 4*pixel_length, 4*pixel_length )

        bounding_poly= []
        for subitem in item.childItems():
            bounding_poly.append(subitem.mapToScene( subitem.boundingRect() ))

        self.move_tool = MoveToolDelegate(view,element,item.childItems()[1],
                                            element.meta.attribute_by_name('center'),
                                            RectStateManager(),False)
        self.resize_tools = []
        newtool = ResizeToolDelegate(view,element, item.childItems()[1],
                                                 element.meta.attribute_by_name('center'),
                                                 RectStateManager(),
                                                 False,
                                                 element.meta.attribute_by_name('size'))
        self.resize_tools.append(newtool)
        pos = poly_weighted_pos(bounding_poly[1],POS_TOP_LEFT)
        items.append(self._build_tool(view,pos,POS_TOP_LEFT,size,'rect',newtool))

        newtool = ResizeToolDelegate(view,element, item.childItems()[1],
                                                 element.meta.attribute_by_name('center'),
                                                 RectStateManager(),
                                                 False,
                                                 element.meta.attribute_by_name('size'))
        self.resize_tools.append(newtool)
        pos = poly_weighted_pos(bounding_poly[1],POS_BOTTOM_RIGHT)
        items.append(self._build_tool(view,pos,POS_BOTTOM_RIGHT,size,'rect',newtool))

        newtool = MoveAndScaleToolDelegate(view,element, item.childItems()[0],
                                                 element.meta.attribute_by_name('imagepos'),
                                                 PixmapStateManager(),
                                                 True,
                                                 element.meta.attribute_by_name('imagescale')
                                                 ,element.meta.attribute_by_name('center'))
        self.resize_tools.append(newtool)
        pos = poly_weighted_pos(bounding_poly[0],POS_TOP_LEFT)
        items.append(self._build_tool(view,pos,POS_TOP_LEFT,size,'rectfill',newtool))

        newtool = MoveAndScaleToolDelegate(view,element, item.childItems()[0],
                                                 element.meta.attribute_by_name('imagepos'),
                                                 PixmapStateManager(),
                                                 True,
                                                 element.meta.attribute_by_name('imagescale')
                                                 ,element.meta.attribute_by_name('center'))
        self.resize_tools.append(newtool)
        pos = poly_weighted_pos(bounding_poly[0],POS_BOTTOM_RIGHT)
        items.append(self._build_tool(view,pos,POS_BOTTOM_RIGHT,size,'rectfill',newtool))

        newtool = MoveToolDelegate(view,element, item.childItems()[0],
                                                 element.meta.attribute_by_name('imagepos'),
                                                 PixmapStateManager(),
                                                 True,
                                                 element.meta.attribute_by_name('center'))
        self.resize_tools.append(newtool)
        pos = poly_weighted_pos(bounding_poly[0],POS_CENTER)
        items.append(self._build_tool(view,pos,POS_CENTER,size,'rectfill',newtool))

        newtool = RotateToolDelegate(view,element, item.childItems()[1],
                                                 element.meta.attribute_by_name('rotation'),
                                                 RectStateManager(),
                                                 True,None)
        self.resize_tools.append(newtool)
        pos = poly_weighted_pos(bounding_poly[1],POS_CENTER_RIGHT)
        items.append(self._build_tool(view,pos,POS_CENTER_RIGHT,size,'circle',newtool))

        newtool = RotateToolDelegate(view,element, item.childItems()[0],
                                                 element.meta.attribute_by_name('imagerot'),
                                                 PixmapStateManager(),
                                                 True,element.meta.attribute_by_name('imagescale'))
        self.resize_tools.append(newtool)
        pos = poly_weighted_pos(bounding_poly[0],POS_CENTER_LEFT)
        items.append(self._build_tool(view,pos,POS_CENTER_LEFT,size,'circlefill',newtool))

        return self.move_tool, items

class LineGroupToolFactory(GroupToolsFactory):

    def create_tools(self, item, element, view ):
        """Creates graphic scene item representing the tool handle.
           Returns: tuple(move_tool, list of items)
        """
        items = []
        pixel_length = self.get_pixel_length( view )
        size = QtCore.QPointF( 4*pixel_length, 4*pixel_length )

        bounding_poly= []
        for subitem in item.childItems():
            bounding_poly.append(subitem.mapToScene( subitem.boundingRect() ))

        self.move_tool = MoveToolDelegate(view,element,item.childItems()[0],
                                            element.meta.attribute_by_name('anchor'),
                                            LineStateManager(),False)
        self.resize_tools = []
        newtool = LineToolDelegate(view,element, item,
                                                 element.meta.attribute_by_name('normal'),
                                                 GroupStateManager(),
                                                 False)
        self.resize_tools.append(newtool)
        pos = item.childItems()[1].mapToScene( item.childItems()[1].line().p2() )
        items.append(self._build_tool(view,pos,POS_BOTTOM_RIGHT,size,'circle',newtool))

        return self.move_tool, items


class CircleGroupToolFactory(GroupToolsFactory):
    def create_tools(self, item, element, view ):
        """Creates graphic scene item representing the tool handle.
           Returns: tuple(move_tool, list of items)
        """
        items = []
        pixel_length = self.get_pixel_length( view )
        size = QtCore.QPointF( 4*pixel_length, 4*pixel_length )

        bounding_poly= []
        for subitem in item.childItems():
            bounding_poly.append(subitem.mapToScene( subitem.boundingRect() ))

        self.move_tool = MoveToolDelegate(view,element,item.childItems()[1],
                                            element.meta.attribute_by_name('center'),
                                            EllipseStateManager(),False)
        self.resize_tools = [RadiusToolDelegate(view,element, item.childItems()[1],
                                                 element.meta.attribute_by_name('radius'),
                                                 EllipseStateManager(),
                                                 False)  for i in range(0,4)]

        for index, position in enumerate( (POS_TOP_CENTER,POS_CENTER_LEFT,POS_CENTER_RIGHT,POS_BOTTOM_CENTER)):
          pos = poly_weighted_pos(bounding_poly[1],position)
          items.append(self._build_tool(view,pos,position,size,'rect',self.resize_tools[index]))

        newtool = MoveAndScaleToolDelegate(view,element, item.childItems()[0],
                                                 element.meta.attribute_by_name('imagepos'),
                                                 PixmapStateManager(),
                                                 True,
                                                 element.meta.attribute_by_name('imagescale'),
                                                 element.meta.attribute_by_name('center'))
        self.resize_tools.append(newtool)
        pos = poly_weighted_pos(bounding_poly[0],POS_TOP_LEFT)
        items.append(self._build_tool(view,pos,POS_TOP_LEFT,size,'rectfill',newtool))

        newtool = MoveAndScaleToolDelegate(view,element, item.childItems()[0],
                                                 element.meta.attribute_by_name('imagepos'),
                                                 PixmapStateManager(),
                                                 True,
                                                 element.meta.attribute_by_name('imagescale'),
                                                 element.meta.attribute_by_name('center'))
        self.resize_tools.append(newtool)
        pos = poly_weighted_pos(bounding_poly[0],POS_BOTTOM_RIGHT)
        items.append(self._build_tool(view,pos,POS_BOTTOM_RIGHT,size,'rectfill',newtool))

        newtool = MoveToolDelegate(view,element, item.childItems()[0],
                                                 element.meta.attribute_by_name('imagepos'),
                                                 PixmapStateManager(),
                                                 True,
                                                 element.meta.attribute_by_name('center'))
        self.resize_tools.append(newtool)
        pos = poly_weighted_pos(bounding_poly[0],POS_CENTER)
        items.append(self._build_tool(view,pos,POS_CENTER,size,'rectfill',newtool))

        newtool = RotateToolDelegate(view,element, item.childItems()[0],
                                                 element.meta.attribute_by_name('imagerot'),
                                                 PixmapStateManager(),
                                                 True,element.meta.attribute_by_name('imagescale'))
        self.resize_tools.append(newtool)
        pos = poly_weighted_pos(bounding_poly[0],POS_CENTER_LEFT)
        items.append(self._build_tool(view,pos,POS_CENTER_LEFT,size,'circlefill',newtool))

        return self.move_tool, items

class LinearFFGroupToolFactory(GroupToolsFactory):
    def create_tools(self, item, element, view ):
        """Creates graphic scene item representing the tool handle.
           Returns: tuple(move_tool, list of items)
        """
        items = []
        pixel_length = self.get_pixel_length( view )
        size = QtCore.QPointF( 4*pixel_length, 4*pixel_length )

        bounding_poly= []
        for subitem in item.childItems():
            bounding_poly.append(subitem.mapToScene( subitem.boundingRect() ))

        self.move_tool = MoveToolDelegate(view,element,item.childItems()[0],
                                            element.meta.attribute_by_name('center'),
                                            RectStateManager(),False)
        self.resize_tools = []
        newtool = ResizeToolDelegate(view,element, item.childItems()[0],
                                                 element.meta.attribute_by_name('center'),
                                                 RectStateManager(),
                                                 False,
                                                 element.meta.attribute_by_name('size'))
        self.resize_tools.append(newtool)
        pos = poly_weighted_pos(bounding_poly[0],POS_TOP_LEFT)
        items.append(self._build_tool(view,pos,POS_TOP_LEFT,size,'rect',newtool))

        newtool = ResizeToolDelegate(view,element, item.childItems()[0],
                                                 element.meta.attribute_by_name('center'),
                                                 RectStateManager(),
                                                 False,
                                                 element.meta.attribute_by_name('size'))
        self.resize_tools.append(newtool)
        pos = poly_weighted_pos(bounding_poly[0],POS_BOTTOM_RIGHT)
        items.append(self._build_tool(view,pos,POS_BOTTOM_RIGHT,size,'rect',newtool))

        newtool = DXDYToolDelegate(view,element, item.childItems()[1],
                                                 element.meta.attribute_by_name('force'),
                                                 LineStateManager(),
                                                 False)
        self.resize_tools.append(newtool)
        pos = item.childItems()[1].mapToScene( item.childItems()[1].line().p2() )
        items.append(self._build_tool(view,pos,POS_BOTTOM_RIGHT,size,'rect',newtool))


        return self.move_tool, items

class RadialFFGroupToolFactory(GroupToolsFactory):
    def create_tools(self, item, element, view ):
        """Creates graphic scene item representing the tool handle.
           Returns: tuple(move_tool, list of items)
        """
        items = []
        pixel_length = self.get_pixel_length( view )
        size = QtCore.QPointF( 4*pixel_length, 4*pixel_length )

        bounding_poly= []
        for subitem in item.childItems():
            bounding_poly.append(subitem.mapToScene( subitem.boundingRect() ))

        self.move_tool = MoveToolDelegate(view,element,item.childItems()[0],
                                            element.meta.attribute_by_name('center'),
                                            EllipseStateManager(),False)
        self.resize_tools = [RadiusToolDelegate(view,element, item.childItems()[0],
                                                 element.meta.attribute_by_name('radius'),
                                                 EllipseStateManager(),
                                                 False)  for i in range(0,4)]

        for index, position in enumerate( (POS_TOP_CENTER,POS_CENTER_LEFT,POS_CENTER_RIGHT,POS_BOTTOM_CENTER)):
          pos = poly_weighted_pos(bounding_poly[0],position)
          items.append(self._build_tool(view,pos,position,size,'rect',self.resize_tools[index]))

        return self.move_tool, items

class CompGeomGroupToolFactory(GroupToolsFactory):

    def create_tools(self, item, element, view ):
        """Creates graphic scene item representing the tool handle.
           Returns: tuple(move_tool, list of items)
        """
        items = []
        pixel_length = self.get_pixel_length( view )
        size = QtCore.QPointF( 4*pixel_length, 4*pixel_length )

        children_bounding_poly= None
        children_item = None
        pixmap_bounding_poly = None
        pixmap_item = None
        for subitem in item.childItems():
             if isinstance( subitem, QtGui.QGraphicsItemGroup):
                children_item = subitem
                children_bounding_poly = subitem.mapToScene( subitem.childrenBoundingRect() )
             elif isinstance( subitem, QtGui.QGraphicsPixmapItem):
                pixmap_item = subitem
                pixmap_bounding_poly= subitem.mapToScene( subitem.boundingRect() )

        self.move_tool = MoveToolDelegate(view,element,children_item,
                                            element.meta.attribute_by_name('center'),
                                            GroupStateManager(),False)
        self.resize_tools = []
        newtool = MoveToolDelegate(view,element, children_item,
                                                 element.meta.attribute_by_name('center'),
                                                 GroupStateManager(),
                                                 True)
        self.resize_tools.append(newtool)
        pos = poly_weighted_pos(children_item.mapToScene(QtCore.QRectF(-1,-1,2,2)),POS_CENTER)
        items.append(self._build_tool(view,pos,POS_CENTER,size,'rect',newtool))

        newtool = RotateToolDelegate(view,element, children_item,
                                                 element.meta.attribute_by_name('rotation'),
                                                 GroupStateManager(),
                                                 True,None)
        self.resize_tools.append(newtool)
        pos = poly_weighted_pos(children_bounding_poly,POS_CENTER_RIGHT)
        items.append(self._build_tool(view,pos,POS_CENTER_RIGHT,QtCore.QPointF( 6*pixel_length, 6*pixel_length ),'circlefill',newtool,QtGui.QColor(0,255,0)))

        if pixmap_item:
            newtool = MoveAndScaleToolDelegate(view,element, pixmap_item,
                                                 element.meta.attribute_by_name('imagepos'),
                                                 PixmapStateManager(),
                                                 True,
                                                 element.meta.attribute_by_name('imagescale')
                                                 ,element.meta.attribute_by_name('center'))
            self.resize_tools.append(newtool)
            pos = poly_weighted_pos(pixmap_bounding_poly,POS_TOP_LEFT)
            items.append(self._build_tool(view,pos,POS_TOP_LEFT,size,'rectfill',newtool))

            newtool = MoveAndScaleToolDelegate(view,element, pixmap_item,
                                                     element.meta.attribute_by_name('imagepos'),
                                                     PixmapStateManager(),
                                                     True,
                                                     element.meta.attribute_by_name('imagescale')
                                                     ,element.meta.attribute_by_name('center'))
            self.resize_tools.append(newtool)
            pos = poly_weighted_pos(pixmap_bounding_poly,POS_BOTTOM_RIGHT)
            items.append(self._build_tool(view,pos,POS_BOTTOM_RIGHT,size,'rectfill',newtool))

            newtool = MoveToolDelegate(view,element, pixmap_item,
                                                     element.meta.attribute_by_name('imagepos'),
                                                     PixmapStateManager(),
                                                     True,
                                                     element.meta.attribute_by_name('center'))
            self.resize_tools.append(newtool)
            pos = poly_weighted_pos(pixmap_bounding_poly,POS_CENTER)
            items.append(self._build_tool(view,pos,POS_CENTER,size,'rectfill',newtool))


            newtool = RotateToolDelegate(view,element, pixmap_item,
                                                     element.meta.attribute_by_name('imagerot'),
                                                     PixmapStateManager(),
                                                     True,element.meta.attribute_by_name('imagescale'))
            self.resize_tools.append(newtool)
            pos = poly_weighted_pos(pixmap_bounding_poly,POS_CENTER_LEFT)
            items.append(self._build_tool(view,pos,POS_CENTER_LEFT,size,'circlefill',newtool))

        return self.move_tool, items

GroupToolsFactorys = {'rectangle':RectangleGroupToolFactory,
                      'circle':CircleGroupToolFactory,
                      'compositegeom':CompGeomGroupToolFactory,
                      'linearforcefield':LinearFFGroupToolFactory,
                      'radialforcefield':RadialFFGroupToolFactory,
                      'line':LineGroupToolFactory}

# ###################################################################
# ###################################################################
# State Managers
# ###################################################################
# ###################################################################

class StateManager(object):
    def get_item_state(self, item):
        """Returns an object representing the current item state."""
        return (item.pos(), item.transform(), self._get_item_state(item))
    
    def _get_item_state(self, item): #IGNORE:W0613
        """Returns an object represent the state specific to the item type."""
        return None
    
    def restore_item_state(self, item, state):
        """Restore the item in the state capture by get_item_state."""
        pos, transform, specific_state = state
        item.setPos( pos )
        item.setTransform( transform ) 
        self._set_item_state( item, specific_state )
        
    def _set_item_state(self, item, state):
        """Restore the item specific state capture by _get_item_state()."""
        pass

PixmapStateManager = StateManager
TextStateManager = StateManager
GroupStateManager = StateManager

class RectStateManager(StateManager):
    def _get_item_state(self, item):
        return item.rect()
    
    def _restore_item_state(self, item, state):
        item.setRect( state )

class LineStateManager(StateManager):
    def _get_item_state(self, item):
        return item.line()

    def _restore_item_state(self, item, state):
        item.setLine( state )

class EllipseStateManager(StateManager):
    def _get_item_state(self, item):
        return (item.rect(), item.startAngle(), item.spanAngle())
    
    def _restore_item_state(self, item, state):
        rect, start_angle, span_angle = state
        item.setRect( rect )
        item.setStartAngle( start_angle )
        item.setSpanAngle( span_angle )


class LevelGraphicView(QtGui.QGraphicsView):
    """A graphics view that display scene and level elements.
       Signals:
       QtCore.SIGNAL('mouseMovedInScene(PyQt_PyObject,PyQt_PyObject)')
         => when the mouse mouse in the map. parameters: x,y in scene coordinate.
    """
    def __init__( self, world, tools_actions, common_actions ):
        QtGui.QGraphicsView.__init__( self )
        self.__world = world
        self.common_actions = common_actions
        self.setWindowTitle( self.tr( self.__world.key))
        self.setAttribute( Qt.WA_DeleteOnClose )
        self.__scene = QtGui.QGraphicsScene()
        self.__balls_by_id = {}
        self.__ball_shapes = {}
        self.__strands = []
        self.__lines = []
        self.__scene_elements = set()
        self.__level_elements = set()
        self.__tools_by_actions = {}
        self.__tools_group = None
        self._delayed_property_updates = []
        self._delayed_timer_id = None
        self._band_item = None

        for name, action in tools_actions.iteritems():
            self.__tools_by_actions[action] = name
            self.__tools_group = self.__tools_group or action.actionGroup()
        self._tools_by_name = {
            TOOL_SELECT: SelectTool(self),
            TOOL_PAN: PanTool(self),
            TOOL_MOVE: MoveOrResizeTool(self)
            }
        self._elements_to_skip = None
        self._active_tool = None
        self._tools_handle_items = []
        self._current_inner_tool = None
        self._items_by_element = {}
        self._selection_tool_degates_cache = (None,[])
        self.setScene( self.__scene )
        # Notes: we disable interactive mode. It is very easily to make the application
        # crash when interactive mode is allowed an mouse events are "sometimes" 
        # accepted by the overridden view. Instead, we handle selection and panning
        # ourselves.
        self.setInteractive( False )
        self._type_states = {}
        self._type_states['camera']=ELEMENT_STATE_INVISIBLE
        self._type_states['poi']=ELEMENT_STATE_INVISIBLE
        self.refreshFromModel()
        self.scale( 1.0, 1.0 )
#@DaB
        self.setVerticalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOn)
        self.setHorizontalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOn)
        self.setMouseTracking(True)
        self.setRenderHints( QtGui.QPainter.Antialiasing | QtGui.QPainter.SmoothPixmapTransform )
        self._last_press_at = QtCore.QPointF(0,0)
        self._last_release_at = QtCore.QPointF(0,0)
        # Subscribes to level element change to refresh the view
        for tree in self.__world.trees:
            tree.connect_to_element_events( self.__on_element_added,
                                            self.__on_element_updated,
                                            self.__on_element_about_to_be_removed )
        louie.connect( self._on_active_world_change, metaworldui.ActiveWorldChanged, 
                       self.__world.universe )
        louie.connect( self._on_selection_change, metaworldui.WorldSelectionChanged, 
                       self.__world )
        

    @property
    def world(self):
        return self.__world


    def is_selected_item(self, item):
        data = item.data( KEY_ELEMENT )
        if data.isValid():
            return data.toPyObject() in self.__world.selected_elements
        return False

    def get_enabled_view_tools(self):
        return set( [TOOL_PAN, TOOL_SELECT, TOOL_MOVE] )

    def get_current_inner_tool(self):
        """Returns the tool delegate to use when the user click inside a shape.
           This is usually the MoveToolDelegate.
        """
        return self._current_inner_tool

    def _show_band(self,p1,p2):
        if self._band_item is not None:
            self.__scene.removeItem(self._band_item )
        l = vector2d_length(p1.x()-p2.x(),p1.y()-p2.y())
        if l > 100:
           l = (l-100)*250/150
           if l>255:
               l=255
        else:
            l = 0
        pen = QtGui.QPen(QtGui.QColor(l,0,0))
        pen.setWidth( 3 )
        self._band_item = self.__scene.addLine( p1.x(), p1.y(), p2.x(), p2.y(), pen )
        self._band_item.setZValue(Z_TOOL_ITEMS)

    def _remove_band(self):
        if self._band_item is not None:
            self.__scene.removeItem( self._band_item )
            self._band_item = None

    def _update_tools_handle(self):
        """Updates tools' handle on current select item. 
           Must be called whenever the scale factory change or selection change.
        """
        # Removes any existing handle
        # Notes: we need to be very careful there, removing item that have been removed
        # by other means such as clear() cause the application to crash.
        for tool_item in self._tools_handle_items:
            self.scene().removeItem( tool_item )
        self._tools_handle_items = []
        self._current_inner_tool = None 
        # get selected elements, corresponding item and generate new handles
        elements = self.__world.selected_elements
        if len(elements) > 1: # @todo handle multiple selection
            items = [self._items_by_element.get( element ) for element in elements]
            self._current_inner_tool, self._tools_handle_items = MultiMoveToolsFactory().create_tools(items,self)
            return
        if len(elements)!=1:
            return
        if self._get_active_tool() != self._tools_by_name[TOOL_MOVE]:
            return # No handle for select or pan tool
        element = iter(elements).next()
        item = self._items_by_element.get( element )
        if item is None:
            return
        factory_type = None
        if isinstance( item, QtGui.QGraphicsEllipseItem ):
            factory_type = CircleToolsFactory
        elif isinstance( item, QtGui.QGraphicsPixmapItem ):
            if element.tag=='BallInstance':
                factory_type = BallToolsFactory
            else:
                factory_type = PixmapToolsFactory
        elif isinstance( item, QtGui.QGraphicsRectItem):
            factory_type = RectangleToolsFactory
        elif isinstance( item, QtGui.QGraphicsLineItem):
            factory_type = LineToolsFactory
        elif isinstance( item, QtGui.QGraphicsTextItem):
            factory_type = TextToolsFactory
        elif isinstance( item, QtGui.QGraphicsItemGroup)  and self.is_selected_item(item):
            factory_type = GroupToolsFactorys[element.tag]

        if factory_type is not None:
            self._current_inner_tool, self._tools_handle_items = factory_type().create_tools(item, element, self)

        if self._tools_handle_items:
          for tool_item in self._tools_handle_items:
            # Prevent the item from being selected
            tool_item.setAcceptedMouseButtons( Qt.NoButton ) 

    def delayed_element_property_update(self, element, attribute_meta, new_value):
        self._delayed_property_updates.append( (element, attribute_meta, new_value) )
        if self._delayed_timer_id is None:
            self._delayed_timer_id = self.startTimer(0)
        
    def timerEvent(self, event):
        if event.timerId() == self._delayed_timer_id:
            self.killTimer( self._delayed_timer_id )
            self._delayed_timer_id = None
            pending, self._delayed_property_updates = self._delayed_property_updates, []
            for element, attribute_meta, new_value in pending:
                attribute_meta.set_native( element, new_value )
            event.accept()
        else:
            QtGui.QGraphicsView.timerEvent(self, event)
        
#        if self._selection_tool_degates_cache[0] == element:
#            return self._selection_tool_degates_cache[1][:]
#        # Only handle plain rectangle, pixmap and circle at the current time
#        tool_factories = None
#        if isinstance( item, QtGui.QGraphicsRectItem, QtGui.QGraphicsPixmapItem ):
#            return RectangleToolSelector( self, element, item )
##            tool_factories = {
##                metaworld.XY_TYPE: RectangleMoveToolDelegate,
##                metaworld.SIZE_TYPE: RectangleResizeToolDelegate
##                }
#        elif isinstance( item, QtGui.QGraphicsEllipseItem ):
#            return 
##            tool_factories = {
##                metaworld.XY_TYPE: CircleMoveToolDelegate,
##                metaworld.RADIUS_TYPE: CircleRadiusToolDelegate
##                }
#        elif isinstance( item, QtGui.QGraphicsPixmapItem ):
#            tool_factories = {
#                metaworld.XY_TYPE: PixmapMoveToolDelegate,
#                metaworld.SCALE_TYPE: PixmapScaleToolDelegate
#                }
#        available_tools = []
#        for attribute in element.attributes:
#            factory = tool_factories.get(attribute.type)
#            if factory:
#                if factory == MoveOrRadiusToolDelegate:
#                    if available_tools and isinstance( available_tools[-1], 
#                                                       MoveToolDelegate):
#                        del available_tools[-1] # tool replace simple moving tool
#                        
#                tool = factory( self, element, item, attribute )
#                available_tools.append( tool )
#        self._selection_tool_degates_cache = (element, available_tools[:])
#        return available_tools

    def tool_activated( self, tool_name ):
        """Activates the corresponding tool in the view and commit any pending change.
        """
        if self._active_tool:
            self._active_tool.stop_using_tool()
        self._get_active_tool().start_using_tool()
        self._update_tools_handle()
#        if tool_name == TOOL_SELECT:
#            self.setDragMode( QtGui.QGraphicsView.NoDrag )
#        elif tool_name == TOOL_PAN:
#            self.setDragMode( QtGui.QGraphicsView.ScrollHandDrag )

    def selectLevelOnSubWindowActivation( self ):
        """Called when the user switched MDI window."""
        self.__world.game_model.selectLevel( self.__world.key )

    def clear_selection(self):
        self.__world.set_selection( [] )

    def select_item_element(self, item):
        """Selects the element corresponding to the specified item.
           Called when the user click on an item with the selection tool.
        """
        data = item.data( KEY_ELEMENT )
        if data.isValid():
            element = data.toPyObject()
            self.__world.set_selection( element )
        else:
            print "data is not valid for ",item

    def modify_selection(self, item):
        """Selects the element corresponding to the specified item.
           Called when the user click on an item with the selection tool.
        """
        data = item.data( KEY_ELEMENT )
        if data.isValid():
            element = data.toPyObject()
            return self.__world.modify_selection( element )
        else:
            print "data is not valid for ",item
        return False

    def _get_active_tool(self):
        name = self.__tools_by_actions.get( self.__tools_group.checkedAction() )
        tool = self._tools_by_name.get(name)
        if tool is None:
            tool =  self._tools_by_name[TOOL_SELECT]
        self._active_tool = tool
        return tool

    def mousePressEvent(self, event):
        self._last_press_at = self.mapToScene(event.pos())
        self._get_active_tool().on_mouse_press_event( event )
        event.accept()
	#@DaB - Ensure if Click then Press to move is quick enough to be consider a Double-click
	# that it still works.
    def mouseDoubleClickEvent(self, event):
        self._last_press_at = self.mapToScene(event.pos())
        self._get_active_tool().on_mouse_press_event( event )
        event.accept()

    def mouseReleaseEvent(self, event):
        self._last_release_at = self.mapToScene(event.pos())
        self._get_active_tool().on_mouse_release_event( event )
        event.accept()

    def mouseMoveEvent( self, event):
        self._last_pos = self.mapToScene( event.pos() )
        self.emit( QtCore.SIGNAL('mouseMovedInScene(PyQt_PyObject,PyQt_PyObject)'), self._last_pos.x(), self._last_pos.y() )
        self._get_active_tool().on_mouse_move_event( event )
        

    def closeEvent(self,event):
        # could check for unsaved change and ask here
        if not self.__world.isReadOnly and self.__world.is_dirty:
            # ask unsaved changes
            ret =  QtGui.QMessageBox.warning(self, "Save Changes to " + self.__world.name,
                            'There are unsaved changes to ' + self.__world.name,
                            QtGui.QMessageBox.Save | QtGui.QMessageBox.Discard | QtGui.QMessageBox.Cancel )
            if ret==QtGui.QMessageBox.Cancel:
                event.ignore()
                return
            if ret==QtGui.QMessageBox.Save:
                self.__world.saveModifiedElements()

        self.__world.parent_world.remove_world(self.__world)
        del self.__world.game_model.models_by_name[self.__world.name]
        if self._delayed_timer_id is not None:
            self.killTimer( self._delayed_timer_id )#
        if len(self.parent().mdiArea().subWindowList())==1:
            louie.send( metaworldui.ActiveWorldChanged, None, None )

        
        
        QtGui.QGraphicsView.closeEvent(self,event)
        event.accept()

    def wheelEvent(self, event):
        """Handle zoom when wheel is rotated."""
#@DaB
#Added to update displayed position on Zoom
        pos = self.mapToScene( event.pos() )
        self.emit( QtCore.SIGNAL('mouseMovedInScene(PyQt_PyObject,PyQt_PyObject)'), pos.x(), pos.y() )

        delta = event.delta()
        if delta != 0:
            small_delta = delta / 500.0
            factor = abs(small_delta)
            if small_delta < 0:
                factor = 1/(1+factor)
            else:
                factor = 1 + small_delta
#@DaB
#New function zoomView replaces scaleView
			#self.scaleView( factor ) 
            self.zoomView( factor, event.pos().x(), event.pos().y() )
            self.emit( QtCore.SIGNAL('mouseMovedInScene(PyQt_PyObject,PyQt_PyObject)'), pos.x(), pos.y() )

#@DaB
#New Zoom function which give a more "natural" zooming
#In the center of the window 0.2 -> 0.8 
#Keeps the part of the scene at the mouse pointer still (in the same place after zoom)
#At the edges (0->0.2  0.8>1) it scrolls the scene by an increasing amount
#to bring "off-screen" items inwards

    def zoomView(self, scaleFactor, orX, orY):
        factor = self.matrix().scale(scaleFactor, scaleFactor).mapRect(QtCore.QRectF(0, 0, 1, 1)).width()

        if factor < 0.07 or factor > 100:
            return
        self.scale(scaleFactor, scaleFactor)
        
        dx = (orX - self.width()*0.5)
        if scaleFactor > 1:
            llimit = self.width()*0.30
            if dx > llimit :
                dx = dx*4 - llimit*3
            elif dx<-llimit:
                dx = dx*4 + llimit*3

        dy = (self.height()*0.5 - orY)
        if scaleFactor > 1:
            llimit = self.height()*0.30
            if dy > llimit :
                dy = dy*4 - llimit*3
            elif dy<-llimit:
                dy = dy*4 + llimit*3

        h_bar = self.horizontalScrollBar()
        v_bar = self.verticalScrollBar()
        x_value = h_bar.value()
        if self.isRightToLeft():
            x_value -= dx * (scaleFactor-1)
        else:
            x_value += dx * (scaleFactor-1)
        h_bar.setValue( x_value )
        v_bar.setValue( v_bar.value() - dy * (scaleFactor-1) )
        self._update_tools_handle()


    def scaleView(self, scaleFactor):
        """Scales the view by a given factor."""
        factor = self.matrix().scale(scaleFactor, scaleFactor).mapRect(QtCore.QRectF(0, 0, 1, 1)).width()

        if factor < 0.07 or factor > 100:
            return

        self.scale(scaleFactor, scaleFactor)
        self._update_tools_handle()

    def _on_selection_change(self, selection, #IGNORE:W0613
                             selected_elements, deselected_elements,
                             **kwargs): 
        """Ensures that the selected element is seleted in the graphic view.
           Called whenever an element is selected in the tree view or the graphic view.
        """
        # Notes: we do not change selection if the item belong to an item group.
        # All selection events send to an item belonging to a group are forwarded
        # to the item group, which caused infinite recursion (unselect child,
        # then unselect parent, selection parent...)
        for item in self.__scene.items():
          data = item.data( KEY_ELEMENT )
          if not data.isValid():
             itemtype = item.data( KEY_TYPE ).toString()
             if itemtype!='TOOL':
                print "Data not valid in _on_selection_change",itemtype,item
          else:
            element = item.data(KEY_ELEMENT).toPyObject()
            if element in selection:
##                print 'Selecting', item, 'isSelected =', item.isSelected()
##                print '    Group is', item.group()
                if not item.isSelected() and item.group() is None:
                    item.setSelected( True )
            elif item.isSelected() and item.group() is None:
##                print 'Unselecting', item, 'isSelected =', item.isSelected()
##                print '    Group is', item.group()
                item.setSelected( False )
        self._update_tools_handle()

    def getModel( self ):
        return self.__world

    def __on_element_added(self, element, index_in_parent): #IGNORE:W0613
        self.refreshFromModel()

    def __on_element_updated(self, element, name, new_value, old_value): #IGNORE:W0613
        self.refreshFromModel()

    def __on_element_about_to_be_removed(self, element, index_in_parent): #IGNORE:W0613
        self.refreshFromModel( set([element]) )

    def _on_active_world_change(self, active_world):
        """Called when a new world becomes active (may be another one).
        """
        if active_world == self.__world:
            self.refreshFromModel()

    def refreshFromModel( self, elements_to_skip = None ):

        self._elements_to_skip = elements_to_skip or set()
        scene = self.__scene
        scene.clear()
        self._tools_handle_items = []
        self._current_inner_tool = None
        self._items_by_element = {}
        self.__balls_by_id = {}
        self.__strands = []
        self.__lines = []
        level_element = self.__world.level_root
        self._addElements( scene, level_element, self.__level_elements, self._elements_to_skip )
        self._addStrands( scene )

        scene_element = self.__world.scene_root
        self._addElements( scene, scene_element, self.__scene_elements, self._elements_to_skip )

        for element in self.__lines:
            item = self._sceneLineBuilder( scene, element )
            item.setData( KEY_ELEMENT, QtCore.QVariant( element ) )
            item.setFlag( QtGui.QGraphicsItem.ItemIsSelectable, True )
            self._items_by_element[element] = item
        
        # Select currently selected item if any
        self._on_selection_change( self.__world.selected_elements, set(), set() )

    def get_element_state(self,elementtype):
        try:
            estate = self._type_states[elementtype]
        except KeyError:
            estate = ELEMENT_STATE_NONE
        return estate
    
    def set_element_state(self,elementtype,estate):
        self._type_states[elementtype]=estate

    def _addElements( self, scene, element, element_set, elements_to_skip ):
        """Adds graphic item for 'element' to the scene, and add the element to element_set.
           Recurses for child element.
           Return the graphic item created for the element if any (None otherwise).
        """
        if element in elements_to_skip:
            return None
        if self.get_element_state(element.tag)==ELEMENT_STATE_INVISIBLE:
            return None
        builders = {
            # .level.xml builders
            'signpost': self._levelSignPostBuilder,
            'pipe': self._levelPipeBuilder,
            'Vertex': self._levelPipeVertex,
            'BallInstance': self._levelBallInstanceBuilder,
            'Strand': self._addlevelStrand,
            'fire': self._levelFireBuilder,
            'levelexit': self._levelExit,
            'camera': self._levelCameraBuilder,
            'poi': self._levelPoiBuilder,
            # .scene.xml builders
            'scene': self._sceneBuilder,
            'SceneLayer': self._sceneSceneLayerBuilder,
            'button': self._sceneButtonBuilder,
            'buttongroup': self._sceneButtonGroupBuilder,
            'circle': self._sceneCircleBuilder,
            'compositegeom': self._sceneCompositeGeometryBuilder,
            'rectangle': self._sceneRectangleBuilder,
            'hinge': self._sceneHingeBuilder,
            'label': self._sceneLabelBuilder,
            'line': self._addSceneLine,
            'linearforcefield': self._sceneLinearForceFieldBuidler,
            'motor': self._sceneMotorBuilder,
            'particles': self._sceneParticlesBuilder,
            'radialforcefield': self._sceneRadialForceFieldBuilder
            }
        builder = builders.get( element.tag )
        composite_item = None
        composite_child_holder = None
        item = None
        if builder:
            item = builder( scene, element )
            if item:
               item.setData( KEY_ELEMENT, QtCore.QVariant( element ) )
               item.setFlag( QtGui.QGraphicsItem.ItemIsSelectable, True )
               if element.tag == 'compositegeom':
                    composite_item = item
                    for child in composite_item.childItems():
                        child.setData( KEY_ELEMENT, QtCore.QVariant( element ) )
                        child.setFlag( QtGui.QGraphicsItem.ItemIsSelectable, True )
                        if isinstance( child, QtGui.QGraphicsItemGroup):
                            composite_child_holder=child
                            break
               self._items_by_element[element] = item

        for child_element in element:
            item = self._addElements( scene, child_element, element_set, elements_to_skip )
            if composite_child_holder and item:
               item.setParentItem( composite_child_holder )
               

        # finish off compoitegeom.. now all the children and everything are added
        if composite_item:
           brect= composite_item.childrenBoundingRect()
           composite_item.setData(KEY_AREA , QtCore.QVariant(brect.width()*brect.height()))
           self._items_by_element[element] = composite_item

        element_set.add( element )
        return item

    @staticmethod
    def _elementV2Pos( element, attribute, default_value = (0.0,0.0) ): # y=0 is bottom => Negate y
        x, y = element.get_native( attribute, default_value )
        return x, -y

    @staticmethod
    def _elementImageWithPosScaleRot( element ):
        image = element.get('image')
        if image is None:
            return None,None,None,None
        if element.get('imagepos') is not None:
            imagepos = LevelGraphicView._elementV2Pos( element, 'imagepos' )
        else:
            imagepos=None
        imagescale = element.get_native( 'imagescale', (1.0,1.0) )
        imagerot = element.get_native( 'imagerot',0.0 )
        return image, imagepos, imagescale, imagerot

    @staticmethod
    def _setLevelItemZ( item ):
        item.setZValue( Z_LEVEL_ITEMS )

    @staticmethod
    def _setLevelItemXYZ( item, x, y ):
        item.setZValue( Z_LEVEL_ITEMS )
        item.setPos( x, y )

    @staticmethod
    def _setSceneItemXYZ( item, x, y ):
        item.setZValue( Z_PHYSIC_ITEMS )
        item.setPos( x, y )

    def getImagePixmap( self, image_id ):
        """Returns the image pixmap for the specified image id."""
        if image_id is not None:
            return self.__world.getImagePixmap( image_id )
        return None

    def _levelSignPostBuilder( self, scene, element ):
        x, y = self._elementV2Pos( element, 'center' )
        depth = element.get_native( 'depth', 0.0 ) 
        image = element.get('image')
        rotation = element.get_native( 'rotation', 0.0 )
        scalex, scaley = element.get_native( 'scale', (1.0,1.0) )
        alpha = element.get_native('alpha',1.0)

	#@DaB If signpost image is valid, display it!
        pixmap = None
        if image:
           pixmap = self.getImagePixmap( image )

        if pixmap:
            item = scene.addPixmap( pixmap )
            item.setData(KEY_AREA , QtCore.QVariant( pixmap.height()*pixmap.width()*scalex*scaley ))
            item.setData(KEY_TYPE , QtCore.QVariant( element.tag ) )
            item.setTransformationMode(Qt.SmoothTransformation)
            self._applyPixmapTransform( item, pixmap, x, y, rotation, scalex, scaley, depth )
            item.setOpacity(alpha)
        else:
            pen = QtGui.QPen(QtGui.QColor(255,0,0))
            pen.setWidth( 4 )
            item = scene.addRect( 0, 0, 100, 100, pen )
            item.setData(KEY_AREA , QtCore.QVariant( 10000 ))
            item.setData(KEY_TYPE , QtCore.QVariant( element.tag ) )
            self._applyTransform( item, 50, 50, x, y, rotation, 1.0, 1.0, Z_PHYSIC_ITEMS )
        return item

    def _levelPipeBuilder( self, scene, element ):
        vertexes = []
        for vertex_element in element:
            if vertex_element not in self._elements_to_skip:
                pos = self._elementV2Pos( vertex_element, 'pos' )
                if pos is not None:
                    vertexes.append( pos )
        if vertexes:
            path = QtGui.QPainterPath()
            path.moveTo( *vertexes[0] )
            for vertex in vertexes[1:]:
                path.lineTo( *vertex )
            
            pipe_type = element.get('type')
            if pipe_type is None:
                pipe_type='default'
            if pipe_type not in metawog.PIPES_STANDARD.keys():
                pipe_type='custom'
            pen = QtGui.QPen(metawog.PIPES_STANDARD[pipe_type])
            pen.setWidth( 10 )
    ##        brush = QtGui.QBrush( Qt.SolidPattern )
    ##        item = scene.addPath( path, pen, brush )
            item = scene.addPath( path, pen )
            item.setData(KEY_AREA , QtCore.QVariant( item.boundingRect().width() * item.boundingRect().height() ))
            item.setData(KEY_TYPE , QtCore.QVariant( element.tag ) )
            return item

    def _levelPipeVertex(self, scene, element):
        x, y = self._elementV2Pos( element, 'pos' )
        rotation = 45
        size = 10
        pen = QtGui.QPen( QtGui.QColor( 255, 85, 153 ) )
        pen.setWidth( 5 )
        item = scene.addRect( 0, 0, size, size, pen )
        item.setData(KEY_AREA , QtCore.QVariant( size*size ))
        item.setData(KEY_TYPE , QtCore.QVariant( element.tag ) )


        self._applyTransform( item, size/2.0, size/2.0, x, y, rotation,
                              1.0, 1.0, Z_PHYSIC_ITEMS )
        return item

    def _levelCameraBuilder( self, scene, element ):

        x, y = self._elementV2Pos( element, 'endpos' )
        rotation = 0
        zoom = element.get_native('endzoom')
        cameraitem=None
        if zoom is not None:
          if element.get('aspect')=='widescreen':
            sizex = 1060 / zoom
            sizey = 600 / zoom
            pen = QtGui.QPen( QtGui.QColor( 0, 96, 48 ) )
            scalex = 10
          else:
            pen = QtGui.QPen( QtGui.QColor( 0, 64, 64 ) )
            sizex = 800 / zoom
            sizey = 600 / zoom
            scalex = 3
          sizex = sizex / scalex
          pen.setWidth( 2 )
          cameraitem = scene.addRect( 0, 0, sizex, sizey, pen )
          cameraitem.setData(KEY_AREA , QtCore.QVariant( sizex*sizey*scalex ))
          cameraitem.setData(KEY_TYPE , QtCore.QVariant( element.tag ) )
          self._applyTransform( cameraitem, sizex/2.0, sizey/2.0, x, y, rotation,
                              scalex, 1.0, Z_PHYSIC_ITEMS )
        return cameraitem

    def _levelPoiBuilder(self, scene, element):
        x, y = self._elementV2Pos( element, 'pos' )
        rotation = 0
        parent = element.parent
        nPois = len(parent.getchildren())
        myIndex = element.index_in_parent()
        zoom = element.get_native('zoom')
        if myIndex == nPois-1:
            scaley = 1
            rescaley = 1
            alpha = 1
        else:
            scaley = 80/zoom
            rescaley = 70/zoom
            alpha=0.5

        if parent.get('aspect')=='widescreen':
            sizex = 1060 / zoom
            sizey = 600 / zoom
            pen = QtGui.QPen( QtGui.QColor( 32, 255, 128 ) )
            scalex = 10
            rescalex = 9.5
            aspect=16/9
        else:
            pen = QtGui.QPen( QtGui.QColor( 64, 192, 192 ) )
            sizex = 800 / zoom
            sizey = 600 / zoom
            scalex = 1
            rescalex = 1
            aspect=4/3

        sizex = sizex / scalex
        sizey = sizey / scaley
        pen.setWidth( 2 )
        item = scene.addRect( 0, 0, sizex, sizey, pen )
        item.setData(KEY_AREA , QtCore.QVariant( sizex*sizey*scalex*scaley ))
        item.setData(KEY_TYPE , QtCore.QVariant( element.tag ) )
        item.setData(KEY_EXTRA , QtCore.QVariant( aspect ) )


        self._applyTransform( item, sizex/2.0, sizey/2.0, x, y, rotation,
                              rescalex, rescaley, Z_PHYSIC_ITEMS )
        item.setOpacity(alpha)
        return item

    def _getBallPixmap(self,balltype):
       try:
          ball_image = metawog.BALLS_IMAGES[balltype]
       except KeyError:
          ball_image = [None,0]
       pixmap = None
       if ball_image[0] is not None:
          pixmap = self.__world.game_model.pixmap_cache.get_pixmap( ball_image[0] )
          return [pixmap,ball_image[1]]
       return [None,0]

    def _outlineBallBuilder(self,scene,element,balltype):
        if balltype is None or balltype=='':
           ballshape = ('circle',50.0)
           pen = QtGui.QPen(QtGui.QColor(255,0,0))
           pen.setWidth(3)
        else:
           pen = QtGui.QPen(QtGui.QColor(0,0,0))
           pen.setWidth(1)
           try:
            ballshape = metawog.BALLS_SHAPES[balltype]
           except KeyError:
                print "ERROR: Unknown balltype=",balltype
                pen = QtGui.QPen(QtGui.QColor(255,0,0))
                pen.setWidth(3)
                ballshape = ('circle',50.0)
           
        x, y = self._elementV2Pos( element, 'pos' )
        if ballshape[0]=='circle':
            r = float(ballshape[1])
            item = scene.addEllipse( -r/2, -r/2, r, r ,pen)
            item.setZValue(0.002)
            self._setLevelItemXYZ( item, x, y )
            item.setData(KEY_AREA , QtCore.QVariant( 3.14 * r * r * 0.25 ))
        else:
            rotation = element.get_native( 'angle', 0.0 )
            width = float(ballshape[1])
            height = float(ballshape[2])
            item = scene.addRect( 0, 0, width, height ,pen)
            self._applyTransform( item, width*0.5, height*0.5, x, y, rotation, 1.0, 1.0, 0.002 )
            item.setData(KEY_AREA , QtCore.QVariant( width*height +1))

        item.setData(KEY_TYPE , QtCore.QVariant( element.tag ) )
        return item

    def _levelBallInstanceBuilder( self, scene, element ):
		#@DaB - Retreives 'shape' attribute from the correct Ball_World
		# and modifies the shape and size of item according
        balltype = element.get('type')
        pixmap,scale = self._getBallPixmap(balltype)
        if pixmap is None:
            item = self._outlineBallBuilder(scene,element,balltype)
        else:
            x, y = self._elementV2Pos( element, 'pos' )
            rotation = element.get_native( 'angle', 0.0 )
            item = scene.addPixmap( pixmap )
            item.setData(KEY_AREA , QtCore.QVariant( pixmap.height()*pixmap.width()*scale*scale ))
            item.setData(KEY_TYPE , QtCore.QVariant( element.tag ) )
            item.setTransformationMode(Qt.SmoothTransformation)
            self._applyPixmapTransform( item, pixmap, x, y, rotation, scale, scale, 0.002 )
        
        ball_id = element.get('id')
        self.__balls_by_id[ ball_id ] = item
        return item

    def _addlevelStrand( self, scene, element ): #IGNORE:W0613
        """Strands are rendered once everything has been rendered because they refer other items (balls)."""
        id1, id2 = element.get('gb1'), element.get('gb2')
        self.__strands.append( (id1, id2, element) )

    def _addStrands( self, scene ):
        """Render all the strands."""
        for id1, id2, element in self.__strands:
            item1, item2 = self.__balls_by_id.get(id1), self.__balls_by_id.get(id2)
            if item1 and item2:
                p1 = poly_weighted_pos(item1.mapToScene( item1.boundingRect() ),POS_CENTER)
                p2 = poly_weighted_pos(item2.mapToScene( item2.boundingRect() ),POS_CENTER)
                #@DaB: Changed to ensure strands come from the center of Rectangle Goos too!
                #p1, p2 = item1.pos(), item2.pos()
                l = vector2d_length(p1.x()-p2.x(),p1.y()-p2.y())
                if l > 100:
                    l = (l-100)*250/150
                    if l>255:
                        l=255
                else:
                    l = 0
                pen = QtGui.QPen(QtGui.QColor(l,0,0))
                pen.setWidth( 3 )

                strand_item = scene.addLine( p1.x(), p1.y(), p2.x(), p2.y(), pen )
                strand_item.setData(KEY_AREA , QtCore.QVariant( 100+20*vector2d_length(p1.x()-p2.x(),p1.y()-p2.y())))
                strand_item.setData(KEY_TYPE , QtCore.QVariant( element.tag ) )
                strand_item.setData( KEY_ELEMENT, QtCore.QVariant( element ) )
                strand_item.setZValue(-0.001)
                self._items_by_element[element] = strand_item

    def _levelFireBuilder( self, scene, element ):
        x, y = self._elementV2Pos( element, 'center' )
        r = element.get_native( 'radius', 1.0 )
        pen = QtGui.QPen( QtGui.QColor( 255, 64, 0 ) )
        pen.setWidth( 3 )
        #item = scene.addEllipse( -r/2, -r/2, r, r, pen )
        item = scene.addEllipse( -r, -r, r*2, r*2, pen )
        item.setData(KEY_AREA , QtCore.QVariant( 3.14 * r * r  ))
        item.setData(KEY_TYPE , QtCore.QVariant( element.tag ) )
        self._setLevelItemXYZ( item, x, y )
        return item

    def _levelExit(self, scene, element):
        x, y = self._elementV2Pos( element, 'pos' )
        r = element.get_native( 'radius', 1.0 )
        pen = QtGui.QPen( QtGui.QColor( 255, 85, 153 ) )
        pen.setWidth( 3 )
        item = scene.addEllipse( -r, -r, r*2, r*2, pen )
        item.setData(KEY_AREA , QtCore.QVariant( 3.14 * r * r  ))
        item.setData(KEY_TYPE , QtCore.QVariant( element.tag ) )
        self._setLevelItemXYZ( item, x, y )
        return item

    def _sceneSceneLayerBuilder( self, scene, element ):
        x, y = self._elementV2Pos( element, 'center' )
        depth = element.get_native( 'depth', 0.0 ) 
        image = element.get('image')
        if image!='':
            pixmap = self.getImagePixmap( image )
        else:
            pixmap = None
        rotation = element.get_native( 'rotation', 0.0 )
        scalex, scaley = element.get_native( 'scale', (1.0,1.0) )
        alpha = element.get_native('alpha',1.0)
        if pixmap is not None:
            item = scene.addPixmap( pixmap )
            item.setData(KEY_AREA , QtCore.QVariant( pixmap.height()*pixmap.width()*scalex*scaley ))
            item.setData(KEY_TYPE , QtCore.QVariant( element.tag ) )
            item.setTransformationMode(Qt.SmoothTransformation)
            self._applyPixmapTransform( item, pixmap, x, y, rotation, scalex, scaley, depth )
            item.setOpacity(alpha)
        else:
            pen = QtGui.QPen( QtGui.QColor( 255, 0, 0 ) )
            pen.setWidth( 4 )
            item = scene.addRect( 0, 0, 100, 100, pen )
            item.setData(KEY_AREA , QtCore.QVariant( 10000 ))
            item.setData(KEY_TYPE , QtCore.QVariant( element.tag ) )
            self._applyTransform( item, 50, 50, x, y, rotation, 1.0, 1.0, Z_PHYSIC_ITEMS )
        return item

    @staticmethod
    def _applyPixmapTransform( item, pixmap, x, y, rotation, scalex, scaley, depth ):
        LevelGraphicView._applyTransform( item, pixmap.width()/2.0, pixmap.height()/2.0,
                                          x, y, rotation, scalex, scaley, depth )

    @staticmethod
    def _applyTransform( item, xcenter, ycenter, x, y, rotation, scalex, scaley, depth ):
        """Rotate, scale and translate the item. xcenter, ycenter indicates the center of rotation.
        """
        # Notes: x, y coordinate are based on the center of the image, but Qt are based on top-left.
        # Hence, we adjust the x, y coordinate by half width/height.
        # But rotation is done around the center of the image, that is half width/height
        transform = QtGui.QTransform()
        xcenter, ycenter = xcenter * scalex, ycenter * scaley
        transform.translate( xcenter, ycenter )
        transform.rotate( -rotation )
        transform.translate( -xcenter, -ycenter )
        transform.scale(scalex,scaley)
        item.setTransform( transform )
        x -= xcenter
        y -= ycenter
        item.setPos( x, y )
        item.setZValue( depth )

    def _sceneButtonBuilder( self, scene, element ):
        x, y = self._elementV2Pos( element, 'center' )
        depth = element.get_native( 'depth', 0.0 )
        rotation = element.get_native( 'rotation', 0.0 )
        scalex, scaley = element.get_native( 'scale', (1.0,1.0) )
        pixmap = self.getImagePixmap( element.get('up') )
        if pixmap:
            item = scene.addPixmap( pixmap )
            item.setData(KEY_AREA , QtCore.QVariant( pixmap.height()*pixmap.width()*scalex*scaley ))
            item.setData(KEY_TYPE , QtCore.QVariant( element.tag ) )
            self._applyPixmapTransform( item, pixmap, x, y, rotation, scalex, scaley, depth )
            return item
        else:
            print 'Button image not found:', element.get('up')
        

    def _sceneButtonGroupBuilder( self, scene, element ):
        pass

    def _sceneLabelBuilder( self, scene, element ):
        x, y = self._elementV2Pos( element, 'position' )
        rotation = element.get_native( 'rotation', 0.0 )
        scale = element.get_native( 'scale', 1.0 )
        alignment = element.get('align',"left")
        font = QtGui.QFont()
        font.setPointSize( 16.0 )
        font.setBold( True )
        text = element.get('text')
        if text is None:
            text='!Set_Text!'
        item = scene.addText( text, font )
        item.setData(KEY_AREA , QtCore.QVariant( item.boundingRect().width() * item.boundingRect().height() ))
        item.setData(KEY_TYPE , QtCore.QVariant( element.tag ) )
        item.setDefaultTextColor( QtGui.QColor( 64, 255, 0 ) )
        offsety = item.boundingRect().height()/2
        if alignment=="left":
            offsetx = 0
        elif alignment=="right":
            offsetx = item.boundingRect().width()
        else:
            offsetx = item.boundingRect().width()/2

        self._applyTransform( item,offsetx, offsety, x, y, rotation, scale, scale, Z_PHYSIC_ITEMS )
        return item

    def _sceneCircleBuilder( self, scene, element ):
        x, y = self._elementV2Pos( element, 'center' )
        r = element.get_native( 'radius', 1.0 )
        image, imagepos, imagescale, imagerot = self._elementImageWithPosScaleRot( element )
        if image:
            pen = QtGui.QPen( QtGui.QColor( 128, 64, 192 ) )
            brush=QtGui.QBrush (QtGui.QColor( 192, 128, 255,64 ), Qt.SolidPattern)
        else:
            pen = QtGui.QPen( QtGui.QColor( 0, 64, 255 ) )
            brush = QtGui.QBrush (QtGui.QColor( 0, 128, 255,64 ), Qt.SolidPattern)

        pen.setWidth( 2 )
        item = scene.addEllipse( -r, -r, r*2, r*2, pen, brush)
        item.setData(KEY_AREA , QtCore.QVariant( 3.14 * r * r ))
        item.setData(KEY_TYPE , QtCore.QVariant( element.tag ) )
        self._applyTransform( item, 0,0, x, y, 0, 1.0, 1.0, Z_PHYSIC_ITEMS )

        if image and self.get_element_state('pixmap')!=ELEMENT_STATE_INVISIBLE:
            pixmap = self.getImagePixmap( image )
            if pixmap:
                if imagepos is None:
                    imagepos = (x,y)
                item.setData(KEY_ELEMENT , QtCore.QVariant( element ) )
                imgitem = scene.addPixmap( pixmap )
                imgitem.setData(KEY_AREA , QtCore.QVariant( 3.14 * r * r))
                imgitem.setData(KEY_TYPE , QtCore.QVariant( element.tag ) )
                imgitem.setData(KEY_ELEMENT , QtCore.QVariant( element ) )
                imgitem.setTransformationMode(Qt.SmoothTransformation)
                self._applyPixmapTransform( imgitem, pixmap, imagepos[0], imagepos[1], imagerot,
                                            imagescale[0], imagescale[1], -0.002 )
                combitem = scene.createItemGroup( [item, imgitem] )     
                combitem.setData(KEY_AREA , QtCore.QVariant( 0 ))
                combitem.setData(KEY_TYPE , QtCore.QVariant( element.tag ) )
                combitem.setZValue ( Z_PHYSIC_ITEMS )
                return combitem
            else:
                print 'Circle image not found:', image
                return item
        else: # "physic" circle
            return item
            
    def _sceneBuilder( self, scene, element ):
        minx,maxx = element.get_native('minx',0.0),element.get_native('maxx',0.0)
        miny,maxy = element.get_native('miny',0.0),element.get_native('maxy',0.0)
        x, y = (minx+maxx)*0.5,-(miny+maxy)*0.5
        rotation = 0
        width, height = abs(maxx-minx),abs(maxy-miny)
        if maxx<=minx or maxy<=miny:
            pen = QtGui.QPen( QtGui.QColor( 255,0,0 ) )
        else:
            pen = QtGui.QPen( QtGui.QColor( 0, 0, 0 ) )
        pen.setWidth( 2 )
        pen.setStyle( Qt.DotLine)
        item = scene.addRect( 0, 0, width, height, pen )
        item.setData(KEY_AREA , QtCore.QVariant( 10*width*height +1))
        item.setData(KEY_TYPE , QtCore.QVariant( 'scene' ) )
        self._applyTransform( item, width/2.0, height/2.0, x, y, rotation, 1.0, 1.0, Z_TOOL_ITEMS )
        return item

    def _sceneRectangleBuilder( self, scene, element ):
        x, y = self._elementV2Pos( element, 'center' )
        rotation = element.get_native( 'rotation', 0.0 )
        width, height = element.get_native( 'size', (1.0,1.0) )
        image, imagepos, imagescale, imagerot = self._elementImageWithPosScaleRot( element )
        if image:
            pen = QtGui.QPen( QtGui.QColor( 128, 64, 192 ) )
            brush=QtGui.QBrush (QtGui.QColor( 192, 128, 255,64 ), Qt.SolidPattern)
        else:
            pen = QtGui.QPen( QtGui.QColor( 0, 64, 255 ) )
            brush = QtGui.QBrush (QtGui.QColor( 0, 128, 255,64 ), Qt.SolidPattern)
        pen.setWidth( 2 )
        item = scene.addRect( 0, 0, width, height, pen, brush )
        item.setData(KEY_AREA , QtCore.QVariant( width*height))
        item.setData(KEY_TYPE , QtCore.QVariant( element.tag ) )
        self._applyTransform( item, width/2.0, height/2.0, x, y, rotation, 1.0, 1.0, Z_PHYSIC_ITEMS )

        if image and self.get_element_state('pixmap')!=ELEMENT_STATE_INVISIBLE:
            pixmap = self.getImagePixmap( image )
            if pixmap:
                if imagepos is None:
                    imagepos = (x,y)
                item.setData(KEY_ELEMENT , QtCore.QVariant( element ) )
                imgitem = scene.addPixmap( pixmap )
                imgitem.setData(KEY_AREA , QtCore.QVariant( width*height))
                imgitem.setData(KEY_TYPE , QtCore.QVariant( element.tag ) )
                imgitem.setData(KEY_ELEMENT , QtCore.QVariant( element ) )
                imgitem.setTransformationMode(Qt.SmoothTransformation)
                self._applyPixmapTransform( imgitem, pixmap, imagepos[0], imagepos[1], imagerot,
                                            imagescale[0], imagescale[1], -Z_PHYSIC_ITEMS )

                combitem = scene.createItemGroup( [item, imgitem] )
                combitem.setData(KEY_AREA , QtCore.QVariant( 0 ))
                combitem.setData(KEY_TYPE , QtCore.QVariant( element.tag ) )
                combitem.setZValue( Z_PHYSIC_ITEMS )
                return combitem
            else:
                print 'Rectangle image not found:', image
                return item
        else: # "physic" rectangle
            return item
        
    def _addSceneLine( self, scene, element ):
        """Delay line rendering after everything (line are unbounded, we limit them to the scene extend)."""
        self.__lines.append( element )

    def _sceneLineBuilder( self, scene, element ):
        """An unbounded physic line. We bound it to the scene bounding rectangle."""
        anchor = self._elementV2Pos( element, 'anchor' )
        normal = self._elementV2Pos( element, 'normal' )
        # Get scene bounds, compute center point
        scene_element = self.__world.scene_root
        scene_center = [(scene_element.get_native('minx',-10000)+scene_element.get_native('maxx',10000))*0.5,
                 (scene_element.get_native('miny',-10000)+scene_element.get_native('maxy',10001))*0.5]
        #Compare direction from anchor to scene_center
        #against Normal
        anchor2scene = [scene_center[0]-anchor[0],scene_center[1]+anchor[1]]
        a2slength=vector2d_length(anchor2scene[0],anchor2scene[1])
        n2length = vector2d_length(normal[0],normal[1])
        a2sdotn2 = (anchor2scene[0]*normal[0] - anchor2scene[1]*normal[1]) / (a2slength*n2length)
        #If Normal Points AWAY from scene_center draw in RED
        if a2sdotn2<0:
            pen = QtGui.QPen( QtGui.QColor( 255, 0, 0 ) )
        else:
            pen = QtGui.QPen( QtGui.QColor( 0, 64, 255 ) )
            
        pen.setWidth( 5 )
        direction = normal[1], -normal[0]
        scene_rect = scene.sceneRect()
        # The line is defined by: anchor + direction * t
        if abs(direction[0]) > abs(direction[1]):   # mostly horizontal, bound it min/max x scene
            # xl = anchor.x + direction.x * t => (xl - anchor.x)/direction.x
            # yl = anchor.y + direction.y * t => yl = anchor.y + direction.y * (xl - anchor.x)/direction.x
            minx, maxx = scene_rect.left(), scene_rect.right()
            ys = [ anchor[1] + direction[1] * ( xl - anchor[0] ) / direction[0]
                   for xl in (minx, maxx) ]
            item = scene.addLine( minx, ys[0], maxx, ys[1], pen )
        else:
            miny, maxy = scene_rect.top(), scene_rect.bottom()
            xs = [ anchor[0] + direction[0] * ( yl - anchor[1] ) / direction[1]
                   for yl in (miny, maxy) ]
            item = scene.addLine( xs[0], miny, xs[1], maxy, pen )

        item.setZValue( Z_PHYSIC_ITEMS )
        item.setData(KEY_AREA , QtCore.QVariant( item.line().length()*50))
        item.setData(KEY_TYPE , QtCore.QVariant( element.tag ) )
        item.setData(KEY_ELEMENT , QtCore.QVariant( element ) )

        pen.setWidth( 3 )
        normalitem = scene.addLine( anchor[0], anchor[1], anchor[0]+normal[0]*50,anchor[1] + normal[1]*50, pen )
        normalitem.setData(KEY_AREA , QtCore.QVariant( item.line().length()*50 ))
        normalitem.setData(KEY_TYPE , QtCore.QVariant( element.tag ) )
        normalitem.setZValue( Z_PHYSIC_ITEMS )
        normalitem.setData(KEY_ELEMENT , QtCore.QVariant( element ) )
        combitem = scene.createItemGroup([item,normalitem])
        combitem.setData(KEY_AREA , QtCore.QVariant( 0 ))
        combitem.setData(KEY_TYPE , QtCore.QVariant( element.tag ) )
        return combitem

    def _sceneCompositeGeometryBuilder( self, scene, element ):
        #@daB
        #New Structure for Composite Geom Items
        #ItemGroup (No Transform)
        # |- Geom Item Group (Children) Transform compgeon center and rotation
        # |    |- Rects and circles (Own center and rot)
        # |- Pixmap - Transform imagepos, imagerot, imagescale (no compensation for compgeom values)

        x, y = self._elementV2Pos( element, 'center' )
        rotation = element.get_native( 'rotation', 0.0 )
        image, imagepos, imagescale, imagerot = self._elementImageWithPosScaleRot( element )
        sub_items = []
        pen = QtGui.QPen(QtGui.QColor(32,255,32))
        pen.setWidth(10)
        center_item = scene.addEllipse(-6,-6,12,12,pen)
        center_item.setData(KEY_AREA , QtCore.QVariant( 100 ))
        center_item.setData(KEY_TYPE , QtCore.QVariant( element.tag ) )
        center_item.setData(KEY_ELEMENT , QtCore.QVariant( element ) )
        self._applyTransform( center_item, 0, 0, x, y, rotation, 1.0, 1.0, Z_PHYSIC_ITEMS )
        sub_items.append(center_item)
        childgroup = scene.createItemGroup([])
        self._applyTransform( childgroup, 0, 0, x, y, rotation, 1.0, 1.0,  Z_PHYSIC_ITEMS  )
        sub_items.append(childgroup)
        imageitem=None
        if image and self.get_element_state('pixmap')!=ELEMENT_STATE_INVISIBLE:
            pixmap = self.getImagePixmap( image )
            if pixmap:
                if imagepos is None:
                    imagepos = (x,y)
                imageitem = scene.addPixmap( pixmap )

                self._applyPixmapTransform( imageitem, pixmap, imagepos[0], imagepos[1],
                                            imagerot, imagescale[0], imagescale[1], -0.002 )
                imageitem.setData(KEY_AREA , QtCore.QVariant( pixmap.height()*pixmap.width()*imagescale[0]*imagescale[1] ))
                imageitem.setData(KEY_TYPE , QtCore.QVariant( element.tag ) )
                imageitem.setData(KEY_ELEMENT , QtCore.QVariant( element ) )
                imageitem.setTransformationMode(Qt.SmoothTransformation)
                sub_items.append(imageitem)

        item = scene.createItemGroup(sub_items)
        item.setData(KEY_TYPE , QtCore.QVariant( element.tag ) )
        item.setZValue( Z_PHYSIC_ITEMS )
        return item


    def _sceneLinearForceFieldBuidler( self, scene, element ):
        # @todo ? Should we bother: gravity field usually does not have center, width & height
        x, y = self._elementV2Pos( element, 'center' )
        width, height = element.get_native( 'size', (1.0,1.0) )
        forcex, forcey = self._elementV2Pos( element, 'force', (0, 0.1) )
        # force zone item
        pen = QtGui.QPen( QtGui.QColor( 255, 224, 0 ) )
        pen.setWidth( 5 )
        sub_item1 = scene.addRect( 0, 0, width, height, pen )
        sub_item1.setData(KEY_ELEMENT,QtCore.QVariant(element))
        sub_item1.setData(KEY_AREA , QtCore.QVariant( width * height ))
        sub_item1.setData(KEY_TYPE , QtCore.QVariant( element.tag ) )

        # force direction item
        sub_item2 = self._makeForceDirectionItem( scene, width/2.0, height/2.0, forcex, forcey )
        sub_item2.setData(KEY_ELEMENT,QtCore.QVariant(element))
        sub_item2.setData(KEY_AREA , QtCore.QVariant( vector2d_length(forcex,forcey)*20*5  ))
        sub_item2.setData(KEY_TYPE , QtCore.QVariant( element.tag ) )
        # item group with both force direction & force zone
        item = scene.createItemGroup( [sub_item1, sub_item2] )
        item.setData(KEY_AREA , QtCore.QVariant( 0 ))
        item.setData(KEY_TYPE , QtCore.QVariant( element.tag ) )

        self._applyTransform( item, width/2.0, height/2.0, x, y, 0.0,
                              1.0, 1.0, Z_PHYSIC_ITEMS )
        return item

    def _sceneRadialForceFieldBuilder( self, scene, element ):
        x, y = self._elementV2Pos( element, 'center' )
        r = element.get_native( 'radius', 1.0 )
        force_at_edge = element.get_native( 'forceatedge', 0.0 )
        force_at_center = element.get_native( 'forceatcenter', 0.0 )
        # circular zone item
        pen = QtGui.QPen( QtGui.QColor( 255, 224, 0 ) )
        pen.setWidth( 5 )
        sub_item1 = scene.addEllipse( -r, -r, r*2, r*2, pen )
        sub_item1.setData(KEY_AREA , QtCore.QVariant( 3.14 * r * r ))
        sub_item1.setData(KEY_TYPE , QtCore.QVariant( element.tag ) )
        sub_item1.setData(KEY_ELEMENT , QtCore.QVariant( element ) )

        # force at center item (from the center to down)
        sub_item2 = self._makeForceDirectionItem( scene, 0, 0, 0, force_at_center )
        sub_item2.setData(KEY_AREA , QtCore.QVariant( (3.14 * r * r )+2 ))
        sub_item2.setData(KEY_TYPE , QtCore.QVariant( element.tag ) )
        sub_item2.setData(KEY_ELEMENT , QtCore.QVariant( element ) )

        # force at edge item (from ledge side of the circle to down)
        sub_item3 = self._makeForceDirectionItem( scene, -r, 0, 0, force_at_edge )
        sub_item3.setData(KEY_AREA , QtCore.QVariant( (3.14 * r * r )+3 ))
        sub_item3.setData(KEY_TYPE , QtCore.QVariant( element.tag ) )
        sub_item3.setData(KEY_ELEMENT , QtCore.QVariant( element ) )
        # item group with both force direction & force zone
        item = scene.createItemGroup( [sub_item1, sub_item2, sub_item3] )
        item.setData(KEY_AREA , QtCore.QVariant( 0 ))
        item.setData(KEY_TYPE , QtCore.QVariant( element.tag ) )

        self._setSceneItemXYZ( item, x, y )
        return item

    def _makeForceDirectionItem( self, scene, x, y, forcex, forcey, force_factor = 20.0 ):
        forcex, forcey = forcex * force_factor, forcey * force_factor # to make direction more visible
        force_gradient = QtGui.QLinearGradient( x,y, x+forcex, y+forcey )
        force_gradient.setColorAt( 0.0, QtGui.QColor( 128, 32, 0 ) )
        force_gradient.setColorAt( 1.0, QtGui.QColor( 255, 192, 64 ) )
        force_pen = QtGui.QPen( QtGui.QBrush( force_gradient ), 4 )
        return scene.addLine( x, y, x+forcex, y+forcey, force_pen )

    def _sceneMotorBuilder( self, scene, element ):
        # Nothing to render...
        pass

    def _sceneHingeBuilder( self, scene, element ):
        x, y = self._elementV2Pos( element, 'anchor' )
        rotation = 45
        size = 10
        pen = QtGui.QPen( QtGui.QColor( 255, 255, 0 ) )
        pen.setWidth( 5 )
        item = scene.addRect( 0, 0, size, size, pen )
        item.setData(KEY_AREA , QtCore.QVariant( size*size ))
        item.setData(KEY_TYPE , QtCore.QVariant( element.tag ) )
        self._applyTransform( item, size/2.0, size/2.0, x, y, rotation,
                              1.0, 1.0, Z_LEVEL_ITEMS+10 )
        return item
       
    def _sceneParticlesBuilder( self, scene, element ):
        x, y = self._elementV2Pos( element, 'pos', (150,30) )
        font = QtGui.QFont()
        font.setPointSize( 24.0 )
        font.setBold( True )
        text = element.get('effect')
        if text is None or text=='':
            text = "!Set_effect!"
        item = scene.addText( text, font )
        item.setDefaultTextColor( QtGui.QColor( 168, 28, 255 ) )
        item.setData(KEY_AREA , QtCore.QVariant( item.boundingRect().width() * item.boundingRect().height() ))
        item.setData(KEY_TYPE , QtCore.QVariant( element.tag ) )
        self._setSceneItemXYZ( item, x,y )
        return item


if __name__ == "__main__":
    import unittest

    class VectorTest(unittest.TestCase):

        def test_angle(self):
            def check_unit_rotation(angle, f = 1):
                u = (1,0)
                v = (math.cos( math.radians(angle) )*f, 
                     math.sin( math.radians(angle) )*f)
                actual = vector2d_angle( u, v )
                self.assertAlmostEquals( angle, actual, 1 )
            for factor in (1,5): 
                check_unit_rotation( 0, factor )
                check_unit_rotation( 45, factor )
                check_unit_rotation( 10, factor )
                check_unit_rotation( 60, factor )
                check_unit_rotation( 89, factor )
                check_unit_rotation( 90, factor )
                check_unit_rotation( 135, factor )
                check_unit_rotation( 180, factor )
                check_unit_rotation( -170, factor )
                check_unit_rotation( -90, factor )
                check_unit_rotation( -45, factor )
                check_unit_rotation( -10, factor )
            a0 = (100,0)
            a45 = (10,10) 
            a90 = (0,11)
            am45 = (10,-10) 
            am90 = (0,-1)
            self.assertAlmostEquals( -45, vector2d_angle( a45, a0 ), 1 )
            self.assertAlmostEquals( 45, vector2d_angle( a45, a90 ), 1 )
            self.assertAlmostEquals( -90, vector2d_angle( a45, am45 ), 1 )
            self.assertAlmostEquals( 180, vector2d_angle( a90, am90 ), 1 )
    unittest.main()
