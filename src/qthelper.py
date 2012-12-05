"""Some helpers to work around some ackward aspect of the pyqt API.
"""
from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import Qt
import xml

def implicit( type, value ):
    """Simulates implicit C++ constructor.
       If value is an instance of type, then returns value, otherwise call type constructor
       with value as parameter.
    """
    if isinstance(value, type):
        return value
    return type(value)

def key_sequence( value, translate_context ):
    """Constructs a QtGui.QKeySequence simulating the implicit C++ constructor.
       value: may be a QtGui.QKeySequence, QtGui.QKeySequence.StandardKey or a string.
       If a string is provided, it will be translated. 
    """
    if isinstance(value, QtGui.QKeySequence):
        return value
    if isinstance(value, QtGui.QKeySequence.StandardKey):
        return QtGui.QKeySequence(value)
    assert isinstance(translate_context,QtCore.QObject), translate_context
    return QtGui.QKeySequence( translate_context.tr( value ) )

def action( parent, icon = None, text = None, 
            shortcut = None, shortcut_context = None, status_tip = None,
            enabled = None, checkable = None, checked = None,
            receiver = None, handler = None, translate_context = None, visible=None ):
    """Creates a QtGui.QAction with the specified attributes.
       text, shortcut, status_tip are automatically translated using translate_context,
       or parent if not specified.
       If handler is specified, then the triggered() signal is connected to handler
       using receiver as receiver or parent if no receiver is specified.
    """
    tr_context = translate_context or parent
    action = QtGui.QAction( parent )
    if icon:
        action.setIcon( implicit( QtGui.QIcon, icon ) )
    if text:
        action.setText( tr_context.tr(text) )
    if shortcut is not None:
        action.setShortcut( key_sequence( shortcut, tr_context ) )      
    if shortcut_context is not None:
        action.setShortcutContext( shortcut_context )
    if status_tip:
        action.setStatusTip( tr_context.tr(status_tip) )
    if enabled is not None:
        action.setEnabled( enabled )
    if checked is not None:
        action.setChecked( checked )
    if checkable is not None:
        action.setCheckable( checkable )
    if visible is not None:
        action.setVisible ( visible )
    if handler is not None:
        if receiver is None:
            receiver = parent 
        receiver.connect( action, QtCore.SIGNAL("triggered()"), handler )
    return action

def iterQTreeWidget( tree_or_item, flag = QtGui.QTreeWidgetItemIterator.All ):
    iterator = QtGui.QTreeWidgetItemIterator(tree_or_item, flag)
    while True:
        iterator.__iadd__(1)
        value = iterator.value()
        if value is None:
            break
        yield value

def standardModelTreeItems( model, root_index = None ):
    """Returns all the row items (at column 0) of the specified index recursively. If no index is specified,
       returns all row items of the model.
       model: a QStandardItemModel
       root_index: a QModelIndex.
    """
    if root_index is None or not root_index.isValid():
        parent_indexes = [ model.index(row,0) for row in xrange(0,model.rowCount()) ]
    else:
        parent_indexes = [ root_index ]
    items = []
    while parent_indexes:
        index = parent_indexes.pop()
        parent_indexes.extend( [ index.child(row,0) for row in xrange(0,model.rowCount(index)) ] )
        items.append( model.itemFromIndex(index) )
    return items

def get_row_item_sibling( item, column ):
    """Returns the item corresponding to the column column on the same row as item.
    """
    column_index = item.index().sibling( item.row(), column )
    return item.model().itemFromIndex(column_index)

def index_path( index ):
    """Returns a list of tuple (row,column) corresponding to the index path starting from
       the root.
       index: instance of QModelIndex
    """
    path = []
    if not index.isValid():
        return [(-1,-1)]
    if index.parent().isValid():
        path.extend( index_path(index.parent()) )
    path.append( (index.row(),index.column()) )
    return path

def index_path_str(index, separator = None):
    """Returns a string representing the index path.
       index: instance of QModelIndex
    """
    separator = separator or ' / '
    return separator.join( ['%d:%d' % xy for xy in index_path(index) ] )


def make_icon_overlay( source_icon, overlay_icon, size, overlay_size ):
    """Makes a new icon by applying overlay_icon over source_icon."""
    source_pixmap = source_icon.pixmap( size )
    overlay_pixmap = overlay_icon.pixmap( overlay_size )
    offset = source_pixmap.size() - overlay_pixmap.size()
    painter = QtGui.QPainter( source_pixmap )
    painter.drawPixmap( offset.width(), offset.height(), overlay_pixmap )
    painter.end()
    return QtGui.QIcon( source_pixmap )

def clone_mouse_event( event ):
    return QtGui.QMouseEvent( QtCore.QEvent.MouseMove, event.pos(), event.globalPos(),
                              event.button(), event.buttons(), event.modifiers() )

def itemonclipboard():
    clipboard = QtGui.QApplication.clipboard()
    xml_data = unicode(clipboard.text())
    if not xml_data:
       return None
    try:
        metaworld_element = xml.etree.ElementTree.fromstring( xml_data )
    except xml.parsers.expat.ExpatError:
       return None
    if metaworld_element is None:
       return None
    if metaworld_element.tag == 'MetaWorldElement':
        return metaworld_element[0].tag
    return None

