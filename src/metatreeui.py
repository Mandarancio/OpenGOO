from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import Qt
import qthelper
import louie
import metaworldui
import metaworld

class MetaWorldTreeModel(QtGui.QStandardItemModel):
    def __init__( self, meta_tree, icons_by_group, *args ):
        QtGui.QStandardItemModel.__init__( self, 0, 2, *args ) # nb rows, nb columns
        self._metaworld_tree = None
        self._meta_tree = meta_tree
        self._setHeaders()
        self._issue_tracker = None
        self._icons_by_group = icons_by_group 
        self._issue_icons = {
            None: icons_by_group, # map of QIcon by group name
            metaworldui.CRITICAL_ISSUE: {},
            metaworldui.WARNING_ISSUE: {} 
            }
        for level, icon_path in ( (metaworldui.CRITICAL_ISSUE, ":/images/stop16.png"),
                                  (metaworldui.WARNING_ISSUE, ":/images/warning16.png") ):
            issue_icon = QtGui.QIcon( icon_path )
            for group, group_icon in icons_by_group.iteritems():
                size = QtCore.QSize(16,16)
                overlay_size = QtCore.QSize(14,14)
                icon = qthelper.make_icon_overlay( group_icon, issue_icon, 
                                                   size, overlay_size )
                self._issue_icons[level][group] = icon
                

    @property
    def metaworld_tree( self ):
        return self._metaworld_tree

    @property
    def meta_tree(self):
        return self._meta_tree

    def set_metaworld_tree( self, tree ):
        #assert tree is not None
        #assert tree.meta == self._meta_tree, (tree.meta, self._meta_tree)
        # setup event listener for the tree
# Notes: somehow the tree remain empty if we do not allow setting twice. To investigate...
#        if tree == self._metaworld_tree:
#            return
        if self._metaworld_tree is not None:
            self._metaworld_tree.disconnect_from_element_events( 
                self._onElementAdded, self._onElementUpdated,
                self._onElementAboutToBeRemoved )
            louie.disconnect( self._on_element_issues_updated, 
                              metaworldui.ElementIssuesUpdated,
                              self._metaworld_tree.world )
        self._metaworld_tree = tree
        self._issue_tracker = tree is not None and tree.world or None
        if tree is not None:
            self._metaworld_tree.connect_to_element_events( 
                self._onElementAdded, self._onElementUpdated,
                self._onElementAboutToBeRemoved )
            louie.connect( self._on_element_issues_updated, 
                           metaworldui.ElementIssuesUpdated,
                           self._metaworld_tree.world )
            self._refreshTreeRoot()
        else:
            self.clear()

    def _refreshTreeRoot(self):
        # refresh items
        self.clear()
        self._setHeaders()
        #if self._metaworld_tree.root:
        self._insertElementTreeInTree( self, self._metaworld_tree.root )

    def _setHeaders(self):
        self.setHorizontalHeaderLabels( [self.tr('Element'),
                                         self.tr('Id or Name')] )

    def _onElementAdded(self, element, index_in_parent ):
        if element.parent is None:
            self._refreshTreeRoot()
        else:
            parent_item = self._findItemByElement( element.parent )
            if parent_item is not None:
                self._insertElementTreeInTree( parent_item, element, index_in_parent )
                self._issue_tracker._check_element(element.parent,set())
                self._refresh_item(parent_item,element.parent)
            else:
                print 'Warning: parent_element not found in tree view', element.parent, "parent",element.parent.get('name'), "child",element.tag,element.get('name'),self.meta_tree
                

    def _onElementUpdated(self, element, attribute_name, new_value, old_value): #IGNORE:W0613
        """Updates id/name column if one of those attributes changed."""
        if element.tree == self.metaworld_tree:
            attribute_meta = element.meta.attribute_by_name( attribute_name )
            assert attribute_meta is not None
            if attribute_meta in element.meta.display_id_attributes:
                item = self._findItemByElement( element )
                name_item = qthelper.get_row_item_sibling( item, 1 )
                if name_item is not None:
                    name_item.setText( new_value or '' )

    def _onElementAboutToBeRemoved(self, element, index_in_parent ): #IGNORE:W0613
        item = self._findItemByElement( element )
        if item is not None:
            item_row = item.row()
            if item.parent() is not None:
                item.parent().removeRow( item_row )

        # Notes: selection will be automatically switched to the previous row in the tree view.

    def get_item_element(self, item):
        data = item.data( Qt.UserRole )
        if data.isValid():
            return data.toPyObject()
        else:
            return None

    def get_index_element(self, index):
        return index.data( Qt.UserRole ).toPyObject()

    def _findItemByElement( self, element ):
        """Returns the tree view item corresponding to the specified element.
           None if the element is not in the tree.
        """
        for item in qthelper.standardModelTreeItems( self ):
            telement = self.get_item_element( item )
            if telement is not None:
                if telement is element:
                    return item
        return None

    def _insertElementTreeInTree( self, item_parent, element, index = None ):
        """Inserts a sub-tree of item in item_parent at the specified index corresponding to the tree of the specified element.
           Returns the new root item of the sub-tree.
           index: if None, append the new sub-tree after all the parent chidlren.
        """
        items_to_process = [ (item_parent, element, index) ]
        while items_to_process:
            item_parent, element, index = items_to_process.pop(0)
            item = self._insertElementNodeInTree( item_parent, 
                                                  element, 
                                                  index )
            for child_element in element:
                items_to_process.append( (item, child_element, None) )

    def _insertElementNodeInTree( self, item_parent, element, index = None ):
        """Inserts a single child node in item_parent at the specified index corresponding to the specified element and returns item.
           index: if None, append the new child item after all the parent children.
        """
        if index is None:
            index = item_parent.rowCount()
        display_name = element.get_display_id()
        items = []
        for position, text in enumerate((element.tag, display_name)):
            item = QtGui.QStandardItem( text )
            item.setData( QtCore.QVariant( element ), Qt.UserRole )
            item.setFlags( item.flags() & ~Qt.ItemIsEditable )
            if position == 0:
                self._refresh_item( item, element )
            items.append( item )
        item_parent.insertRow( index, items )
        return items[0]

    def _refresh_item( self, item, element ):
        group = element.meta.main_group
        issue = self._issue_tracker.element_issue_level( element )
        icons_by_group = self._issue_icons.get( issue )
        if icons_by_group:
            icon = icons_by_group.get( group )
            if icon:
                item.setIcon( icon )
        if issue is not None:
            item.setToolTip( self._issue_tracker.element_issue_report(element) )
        else:
            item.setToolTip('')

    def _on_element_issues_updated( self, elements ):
        for item in qthelper.standardModelTreeItems( self ):
            element = self.get_item_element( item )
            if element is not None:
              if element in elements:
                self._refresh_item( item, element )

class MetaWorldTreeView(QtGui.QTreeView):
    def __init__( self, common_actions, group_icons, *args ):
        QtGui.QTreeView.__init__( self, *args )
        self._common_actions = common_actions
        # Hook context menu popup signal
        self._active_world = None
        self.setSelectionMode(QtGui.QAbstractItemView.ExtendedSelection)
        self.setSelectionBehavior( QtGui.QAbstractItemView.SelectRows )
        self.setContextMenuPolicy( Qt.CustomContextMenu )
        self.setRootIsDecorated( False )
        self.setSortingEnabled(True)
        self.setIconSize( QtCore.QSize(16,16) )
        self.group_icons = group_icons
        self.connect( self, QtCore.SIGNAL("customContextMenuRequested(QPoint)"),
                      self._onContextMenu )
        louie.connect( self._on_active_world_change, metaworldui.ActiveWorldChanged )

    @property
    def metaworld_tree(self):
        model = self.model()
        return model and model.metaworld_tree or None 

    def setModel(self, model):
        QtGui.QTreeView.setModel(self, model)
        assert self.selectionModel() is not None
        self.connect( self.selectionModel(), 
                      QtCore.SIGNAL("selectionChanged(QItemSelection,QItemSelection)"),
                      self._onTreeViewSelectionChange )
        
    def _on_active_world_change(self, active_world):
        """Refresh a tree view with the new root element."""
        if active_world!=self._active_world:
            self._active_world = active_world
            model = self.model()
            # disconnect for previous world selection events
            if model is not None and model.metaworld_tree is not None:
                old_world = model.metaworld_tree.world
                louie.disconnect( self._on_selection_change, metaworldui.WorldSelectionChanged,
                                  old_world )
            # connect to new world selection events & refresh tree view
            if model is None or active_world is None:
                model.set_metaworld_tree( None )
                #self.clear()
            else:
                model_tree = active_world.find_tree( model.meta_tree )
                if model_tree is not None:
                    louie.connect( self._on_selection_change, metaworldui.WorldSelectionChanged,
                                   active_world )
                    model.set_metaworld_tree( model_tree )
                    self._expand_roots()
                    self.resizeColumnToContents(0)
                    self.show()
                else: # the new world has no tree of the type of the view
                    self.hide()

    def _expand_roots(self):
        """Expands all parents with a single child starting with the root.
        """
        root_index = self.model().index(0,0)
        self.setExpanded( root_index, True )
        while self.model().rowCount( root_index ) == 1:
            root_index = root_index.child(0,0)
            self.setExpanded( root_index, True )

    def _on_selection_change(self, selection, #IGNORE:W0613
                             selected_elements, deselected_elements): 
        """Called when elements are selected in the world.
           Notes: reflect the selection on the tree view. Element may not belong to
           the tree of this view.
        """
        meta_trees = set( [element.tree.meta for element in selection if element.tree is not None] )
        model = self.model()
        selection_model = self.selectionModel()
        if model is None or selection_model is None:
            return
        if model.meta_tree not in meta_trees: # clear all selection in tree ?
            selection_model.clear()
        elif len(selection) > 1: # @todo handle multiple selection
            random_element = list(selection)[0]
            #random_element.world.set_selection( random_element )
        else: # there is a single element on this tree
            element = list(selection)[0] 
            selected_item = model._findItemByElement( element )
            if selected_item is not None:
                selected_index = selected_item.index()
                selection_model.select( selected_index, 
                                        QtGui.QItemSelectionModel.ClearAndSelect |
                                        QtGui.QItemSelectionModel.Rows )
                self.setExpanded( selected_index, True )
                self.parent().raise_() # Raise the dock windows associated to the tree view
                self.scrollTo( selected_index )
            else:
                print 'Warning: selected item not found in tree view.', element
                
        
    def _onTreeViewSelectionChange( self, selected, deselected ): #IGNORE:W0613
        """Called whenever the scene tree selection change."""
        model = self.model()
        if model is not None:
            selected_elements = [ model.get_index_element(index) 
                                  for index in selected.indexes() ]
            deselected_elements = [ model.get_index_element(index) 
                                    for index in deselected.indexes() ]
            assert None not in selected_elements
            assert None not in deselected_elements
            world = None
            if selected_elements:
                world = selected_elements[0].world
            elif deselected_elements:
                world = deselected_elements[0].world
            if world is not None:
                current_elements = world.selected_elements
                if None in current_elements:
                    current_elements.remove( None )
                meta_trees = set([element.tree.meta for element in current_elements if element.tree is not None])
                if model.meta_tree not in meta_trees:
                    # replace selection to avoid selection spanning multiple tree views
                    world.set_selection( selected_elements )
                else: # update selection in tree view
                    #print "Trew View On Selection",selected_elements, deselected_elements
                    world.update_selection( selected_elements, deselected_elements )

    def _get_selected_elements(self):
        """Returns the list of selected Element in the world corresponding to this tree.
        """
        if self.model() is not None:
            tree = self.model().metaworld_tree
            if tree is not None:
                return [ element for element in tree.world.selected_elements
                         if element.tree == tree ]
        return []

    def _onContextMenu( self, menu_pos ):
        # Select the right clicked item
        index = self.indexAt(menu_pos)
        if index.isValid() and self.model():
            element = self.model().get_index_element(index)
            if element is None:
                print 'Warning: somehow managed to activate context menu on non item???'
            else:
                element.world.set_selection( element )
                # Notes: a selectionChanged signal may have been emitted due to selection change.
                # Check out FormWindow::initializePopupMenu in designer, it does plenty of interesting stuff...
                menu = QtGui.QMenu( self )
                if not element.is_root() and not element.meta.read_only:
                    menu.addAction( self._common_actions['cut'] )
                menu.addAction( self._common_actions['copy'] )
                menu.addAction( self._common_actions['paste'] )
                menu.addSeparator()
                if not element.is_root() and not element.meta.read_only:
                    menu.addSeparator()
                    menu.addAction( self._common_actions['delete'] )
                self._menu_add_child_actions( element.meta, menu )
                
                for action in menu.actions():
                    action.setShortcutContext( Qt.ApplicationShortcut )
                
                menu.exec_( self.viewport().mapToGlobal(menu_pos) )

    def _menu_add_child_actions(self, element_meta, menu):
        class AddChildAction(QtCore.QObject):
            def __init__(self, tree_view, element_meta, parent):
                QtCore.QObject.__init__( self, parent )
                self.__tree_view = tree_view
                self.__element_meta = element_meta
            def __call__(self):
                elements = self.__tree_view._get_selected_elements()
                if len(elements) == 1:
                    self.__tree_view._appendChildTag( elements[0], 
                                                      self.__element_meta )
        has_action = False
        for tag in sorted(element_meta.immediate_child_tags()):
            child_element_meta = element_meta.find_immediate_child_by_tag(tag)
            if not child_element_meta.read_only:
                action = menu.addAction( self.tr("Add %1").arg(tag) )
                icon = self.group_icons.get( child_element_meta.main_group )
                if icon:
                    action.setIcon( icon )
                handler = AddChildAction( self, child_element_meta, menu )
                self.connect( action, QtCore.SIGNAL("triggered()"), handler )
                has_action = True
        return has_action
        
    def _appendChildTag( self, parent_element, new_element_meta ):
        """Adds the specified child tag to the specified element and update the tree view."""
        assert parent_element is not None
        # build the list of attributes with their initial values.
        mandatory_attributes = {}
        for attribute_meta in new_element_meta.attributes:
            if attribute_meta.mandatory:
                init_value = attribute_meta.init
                if init_value is None:
                    init_value = ''
                if attribute_meta.type == metaworld.IDENTIFIER_TYPE:
                    init_value = parent_element.world.generate_unique_identifier(
                        attribute_meta )
                mandatory_attributes[attribute_meta.name] = init_value
			#@DaB Apply "default" attributes to new items
            if (attribute_meta.default is not None and not attribute_meta.mandatory):
                init_value = attribute_meta.default
                mandatory_attributes[attribute_meta.name] = init_value

        # Notes: when the element is added, the ElementAdded signal will cause the
        # corresponding item to be inserted into the tree.
        child_element = parent_element.make_child( new_element_meta, 
                                                   mandatory_attributes )

        # Select new item in tree view
        child_element.world.set_selection( child_element )
        item_child = self.model()._findItemByElement( child_element )
        self.scrollTo( item_child.index() )
