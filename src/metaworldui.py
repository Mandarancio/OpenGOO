import louie
import metaworld


class ActiveWorldChanged(louie.Signal):
    """Emitted when the current level MDI change.
       Signature: (new_active_world)
       sender: universe the world belong to
    """

class WorldSelectionChanged(louie.Signal):
    """Emitted when the selected elements change.
       Signature: (selection,selected,unselected)
       selected: set of Element that are now selected
       unselected: set of Element that are no longer selected, but were previously selected
       sender: world the element belong to
    """

class RefreshElementIssues(louie.Signal):
    """Emitted periodically from a timer to refresh element issue status.
       Signature: (), sender: Anonymous
    """ 

class ElementIssuesUpdated(louie.Signal):
    """Emitted when existing element issues have been updated
       Signature: (elements), sender: world
       elements: list of Element with modified issue
    """ 

class UndoWorldTracker(object):
    def __init__(self, world, queue_depth = 0):
        """world: world that is tracked for change.
           is_dirty: is the world initially considered has dirty (new world for example).
        """
        self.__world = world
        self.__queue_depth = queue_depth
        self.__undo_queue = []
        self.__redo_queue = []
        self.__active = True
        louie.connect( self.__on_tree_added, metaworld.TreeAdded, world )

    def __on_tree_added(self, tree):
        for tree_meta in self.__world.meta.trees:
            if self.__world.find_tree(tree_meta) is None: # level not ready yet
                return
        # all tree are available, setup the level
        for tree in self.__world.trees:
            tree.connect_to_element_events( self.__on_element_added,
                                            self.__on_element_updated,
                                            self.__on_element_about_to_be_removed )

    def __on_element_added(self, element, index_in_parent): #IGNORE:W0613
        undo_action = [metaworld.ELEMENT_ADDED,element]
        self.__add_to_undo_queue(undo_action)

    def __on_element_about_to_be_removed(self, element, index_in_parent): #IGNORE:W0613
        undo_action = [metaworld.ELEMENT_ABOUT_TO_BE_REMOVED,element,element.parent,index_in_parent]
        self.__add_to_undo_queue(undo_action)

    def __on_element_updated(self, element, name, new_value, old_value): #IGNORE:W0613
        if new_value != old_value:
            undo_action = [metaworld.ELEMENT_ATTRIBUTE_UPDATED,element,name,new_value,old_value]
            self.__add_to_undo_queue(undo_action)

    def __add_to_undo_queue(self, undo_action):
        if self.__active:
            if self.__queue_depth>0:  # 0 = infinite
                if len(self.__undo_queue)==self.__queue_depth:
                    #print "queue full - popping"
                    self.__undo_queue.pop(0)
            self.__undo_queue.append(undo_action)
            self.clear_redo_queue()  #clear redo queue when new undo action is added
            #print "undo count=",len(self.__undo_queue)

    def suspend_undo(self):
        self.__active=False

    def activate_undo(self):
        self.__active=True

    @property
    def undo_status(self):
        return self.__active

    @property
    def can_undo(self):
        # could add extra check here
        return len(self.__undo_queue)>0

    @property
    def can_redo(self):
        # could add extra check here
        return len(self.__redo_queue)>0

    def clear_undo_queue(self):
        self.__undo_queue = []

    def clear_redo_queue(self):
        self.__redo_queue = []

    def undo(self):
        initial_queue_length=len(self.__undo_queue)
        if initial_queue_length>0:
            undo_action = self.__undo_queue.pop(initial_queue_length-1)

            # Undo-ing would add a new action into the undo queue.
            # so suspend undo, while we are undoing
            self.__active = False
            if undo_action[0]==metaworld.ELEMENT_ADDED:
                # action, element
                element = undo_action[1]
                redo_action = [undo_action[0],element,
                                element.parent,element.index_in_parent()]
                element.parent.remove(element)

            elif undo_action[0]==metaworld.ELEMENT_ATTRIBUTE_UPDATED:
                # action, element, attributename, newvalue, oldvalue
                element = undo_action[1]
                redo_action = undo_action
                if undo_action[4] is None:
                    element.unset(undo_action[2])
                else:
                    element.set(undo_action[2],undo_action[4])
            elif undo_action[0]==metaworld.ELEMENT_ABOUT_TO_BE_REMOVED:
                # action, element, parent, index in parent
                redo_action = undo_action
                element = undo_action[1]
                parent = undo_action[2]
                parent.insert(undo_action[3],element)
            else:
                print "Unknown Undo Action",undo_action
            
            # put the redo_action in the redo stack
            self.__redo_queue.append(redo_action)
            # reactivate undo now that we've undone
            self.__active = True

       # print "undo count=",len(self.__undo_queue)

    def redo(self):
        initial_queue_length=len(self.__redo_queue)
        if initial_queue_length>0:
            redo_action = self.__redo_queue.pop(initial_queue_length-1)

            # redo-ing would add a new action into the undo queue.
            # we're going to have to do that anyway...
            # but doing it automatically would reset the redo queue...
            # so suspend undo, while we are redoing
            self.__active = False
            if redo_action[0]==metaworld.ELEMENT_ADDED:
                # action, element, parent, index in parent
                element = redo_action[1]
                parent = redo_action[2]
                parent.insert(redo_action[3],element)

            elif redo_action[0]==metaworld.ELEMENT_ATTRIBUTE_UPDATED:
                # action, element, attributename, newvalue, oldvalue
                element = redo_action[1]
                if redo_action[3] is None:
                    element.unset(undo_action[2])
                else:
                    element.set(redo_action[2],redo_action[3])
            elif redo_action[0]==metaworld.ELEMENT_ABOUT_TO_BE_REMOVED:
                # action, element, parent, index in parent
                element = redo_action[1]
                element.parent.remove(element)
            else:
                print "Unknown Redo Action",redo_action
            
            # put this action back in the undo stack
            self.__undo_queue.append(redo_action)
            # reactivate undo now that we've undone
            self.__active = True

        #print "redo count=",len(self.__redo_queue)

class DirtyWorldTracker(object):
    """Provides the list of tree that have been modified in a world.
       Use element events to track change. Starts tracking for change
       only once all tree from the world description have been added 
       to the world. 
    """
    def __init__(self, world, is_dirty = False):
        """world: world that is tracked for change.
           is_dirty: is the world initially considered has dirty (new world for example).
        """
        self.__world = world
        if is_dirty:
            self.__dirty_tree_metas = set( self.__world.meta.trees )
        else:
            self.__dirty_tree_metas = set()
        louie.connect( self.__on_tree_added, metaworld.TreeAdded, world )

    def __on_tree_added(self, tree):
        for tree_meta in self.__world.meta.trees:
            if self.__world.find_tree(tree_meta) is None: # level not ready yet
                return
        # all tree are available, setup the level
        for tree in self.__world.trees:
            tree.connect_to_element_events( self.__on_element_added,
                                            self.__on_element_updated,
                                            self.__on_element_about_to_be_removed )

    def __on_element_added(self, element, index_in_parent): #IGNORE:W0613
        self.__dirty_tree_metas.add( element.tree.meta )

    def __on_element_about_to_be_removed(self, element, index_in_parent): #IGNORE:W0613
        self.__dirty_tree_metas.add( element.tree.meta )

    def __on_element_updated(self, element, name, new_value, old_value): #IGNORE:W0613
        if new_value != old_value:
            self.__dirty_tree_metas.add( element.tree.meta )

    @property
    def is_dirty(self):
        """Returns True if one of the world tree has been modified."""
        return len(self.__dirty_tree_metas) > 0

    @property
    def dirty_trees(self):
        """Returns the list of modified world trees."""
        return [ self.__world.find_tree(tree_meta) 
                 for tree_meta in list(self.__dirty_tree_metas) ]
    
    @property
    def dirty_tree_metas(self):
        """Returns the types of the modified world tree."""
        return list(self.__dirty_tree_metas)

    def is_dirty_tree(self, tree_meta):
        """Return True if the specified type of world tree has been modified."""
        return tree_meta in self.dirty_tree_metas 

    def clean(self):
        """Forget any change made to the trees so that is_dirty returns True."""
        self.__dirty_tree_metas = set()

    def clean_tree(self, tree_meta):
        """Forget any change made to the specified tree type."""
        if tree_meta in self.__dirty_tree_metas:
            self.__dirty_tree_metas.remove( tree_meta )

class SelectedElementsTracker(object):
    def __init__(self, world):
        self.__selection = set() # set of selected elements
        self.__world = world

    @property
    def selected_elements(self):
        """List of selected Elements.""" 
        return self.__selection.copy()
        
    def _check_selected_elements( self, selected_elements ):
        for element in selected_elements:
            assert element.tree is not None
            assert element.tree.world == self.__world
        
    def set_selection(self, selected_elements ):
        """Set the list of selected Elements."""
        if isinstance(selected_elements, metaworld.Element):
            selected_elements = [selected_elements]
        self._check_selected_elements( selected_elements )
        old_selection = self.__selection.copy()
        self.__selection = set(selected_elements)  
        self._send_selection_update( old_selection )

    def modify_selection(self,elements):
        if isinstance(elements, metaworld.Element):
            elements = [elements]
        old_selection = self.__selection.copy()
        elements = set(elements)
#        print old_selection,elements
        self._check_selected_elements(elements)
        self.__selection = self.__selection ^ elements
#        print self.__selection
        self._send_selection_update( old_selection )
#        print self.__selection
        return len(self.__selection)>0
        
    def update_selection( self, selected_elements, deselected_elements ):
        """Adds and remove some Element from the selection."""
        selected_elements = set(selected_elements)
        deselected_elements = set(deselected_elements)
        self._check_selected_elements(selected_elements)
        old_selection = self.__selection.copy()
        self.__selection = self.__selection | selected_elements
        self.__selection = self.__selection - deselected_elements
        self._send_selection_update( old_selection )

    def _send_selection_update(self, old_selection):
        """Broadcast the selection change to the world if required."""
        if None in self.__selection:
            self.__selection.remove( None )
        selected_elements = self.__selection - old_selection
        deselected_elements = old_selection - self.__selection
        if selected_elements or deselected_elements: 
            #print 'Selection changed:',self
            #print '  Selection:', self.__selection
            #print '  Selected:',  selected_elements
            #print '  Unselected:',  deselected_elements
            louie.send( WorldSelectionChanged, self.__world, 
                        self.__selection.copy(), 
                        selected_elements, deselected_elements )
        
        
CRITICAL_ISSUE = 'critical'
WARNING_ISSUE = 'warning'

class ElementIssueTracker(object):
    """Track a list of issue for all elements.
       Tracked issues concerns invalid element property, incorrect number of children,
       other constraints (missing mass for dynamic object...) 
    """
    def __init__(self, world):
        """world: world that is tracked for change.
        """
        self.__world = world
        louie.connect( self.__on_tree_added, metaworld.TreeAdded, world )
        louie.connect( self.__on_refresh_element_status, RefreshElementIssues )
        self._issues_by_element = {} #dict element:(child,attribute,node)
        self._pending_full_check = set()
        self._pending_updated = set()
        self._modified_issues = set()

    def element_issue_level(self, element):
        """Returns the most critical level of issue for the element.
           None if there is no pending issue.
        """
        if element in self._issues_by_element:
            return CRITICAL_ISSUE
        return None

    def element_attribute_issue(self, element, attribute_name):
        """Returns the issue associated to the specified element atttribute if any.
           Returns None if there is no issue.
        """
        element_issues = self._issues_by_element.get( element )
        if element_issues:
            attributes = element_issues[1]
            issue = attributes.get( attribute_name )
            if issue:
                format, args = issue
                return format % args 
        return None

    def element_issue_report(self, element):
        """Returns a small report of all the element issues.
        """
        issues = self._issues_by_element[element]
        if issues:
            nodes, attributes, occurrences = issues
            report = []
            if occurrences:
                for format, args in occurrences.itervalues():
                    report.append( format % args )
            if attributes:
                report.append( 'Attribute issue:')
                for name in sorted(attributes):
                    format, args = attributes[name]
                    report.append( '- "%(name)s": %(message)s' % {
                        'name':name,'message':format % args } )
            if nodes:
                report.append( 'Has %(childcount)d child element(s) with issue' % {
                    'childcount':len(nodes)} )
            return '\n'.join( report )
        return ''

    def __on_tree_added(self, tree):
        for tree_meta in self.__world.meta.trees:
            if self.__world.find_tree(tree_meta) is None: # level not ready yet
                return
        # all tree are available, setup the level
        for tree in self.__world.trees:
            tree.connect_to_element_events( self.__on_element_added,
                                            self.__on_element_updated,
                                            self.__on_element_about_to_be_removed )
            self.__on_element_added( tree.root, 0 )

    def __on_element_added(self, element, index_in_parent): #IGNORE:W0613
        self._pending_full_check.add( element )

    def __on_element_about_to_be_removed(self, element, index_in_parent): #IGNORE:W0613
        parent = element.parent
        if parent is not None:
            self._pending_full_check.add(parent)
        self._schedule_back_references( element )
    
    def _schedule_back_references( self, element ):
        """Schedule any back-references for check as they may have become invalid."""
        id_meta = element.meta.identifier_attribute
        if id_meta is not None:
            id_value = id_meta.get( element )
            if id_value:
                family = id_meta.reference_family
                references = self.__world.universe.list_references( family, id_value )
                for element, attribute_meta in references: #IGNORE:W0612
                    if element.world == self.__world:
                        self._pending_full_check.add(element) 

    def __on_element_updated(self, element, name, new_value, old_value): #IGNORE:W0613
        # Schedule non-recursive check
        self._pending_updated.add(element) 
        # if the identifier was updated, then also check all back-references
        id_meta = element.meta.identifier_attribute
        if id_meta is not None and id_meta.name == name:
            self._schedule_back_references( element )

    def __on_refresh_element_status(self):
#        import time
#        start_time = time.clock()

        checked_elements = set()
        # Scan added or parent of removed elements and all their children
        for element in self._pending_full_check:
            self._check_element( element, checked_elements, recurse = True )
        self._pending_full_check.clear()
        # Scan updated elements, but not their children
        for element in self._pending_updated:
            self._check_element( element, checked_elements, recurse = False )
        self._pending_updated.clear()

        # Scan parents elements of any modified issue (need to escalate warning)
        all_issue_elements = set()
        while self._modified_issues:
            self._modified_issues, modified_issues = set(), self._modified_issues
            all_issue_elements |= modified_issues
            parents_to_check = set( [ element.parent for element in modified_issues
                                      if element.parent is not None ] )
            for element in parents_to_check:
                if element not in checked_elements:
                    self._check_element( element, checked_elements, recurse = False )
        
        if  all_issue_elements:
#            print 'Refreshed element status: %.3fs' % (time.clock()-start_time)
#            start_time = time.clock()
#            print 'Broadcasting issues:', all_issue_elements
            louie.send( ElementIssuesUpdated, self.__world, all_issue_elements )
#            print 'Broadcast modified element issues: %.3fs' % (time.clock()-start_time)
        
         
    def _check_element(self, element, checked_elements, recurse = True):
        checked_elements.add( self )
        if element.is_detached():  # deleted element
            return
        # check child for issues
        child_issues = {}
        children_by_meta = {}
        for child in element:
            if recurse:
                self._check_element(child, checked_elements)
            issue = self._issues_by_element.get(child)
            if issue is not None:
                child_issues[child] = issue
            if child.meta not in children_by_meta: 
                children_by_meta[child.meta] = []
            children_by_meta[child.meta].append(child)
        # check attribute for issues
        attribute_issues = {}
        for attribute_meta in element.meta.attributes:
            status = element.is_attribute_valid( attribute_meta, self.__world )
            if status is not None:
                attribute_issues[attribute_meta.name] = status 
        # check node issues (mandatory children...)
        node_issues = {}
        for child_meta in element.meta.immediate_child_elements():
            status = self._check_child_occurrences( child_meta, children_by_meta )
            if status is not None:
                node_issues[child_meta] = status
        # synthesis of issues
        if child_issues or attribute_issues or node_issues:
            self._issues_by_element[element] = (child_issues, 
                                                attribute_issues, 
                                                node_issues)
            self._modified_issues.add( element )
#            print 'Issue found on', element
        elif element in self._issues_by_element:
#            print 'Removing issue on ', element
            del self._issues_by_element[element]
            self._modified_issues.add( element ) 
            
    def _check_child_occurrences(self, meta, children_by_meta):
        occurrences = len(children_by_meta.get(meta,()))
        if ( meta.min_occurrence is not None 
             and occurrences < meta.min_occurrence ):
            if meta.min_occurrence == meta.max_occurrence:
                if meta.min_occurrence == 1:
                    return 'Element must have one %(type)s child', {'type':meta.tag}
                return 'Element must have exactly %(count)d %(type)s children', {
                    'type':meta.tag,'count':meta.min_occurrence}
            if meta.min_occurrence == 1:
                return 'Element must have at least one %(type)s child', {'type':meta.tag}
            return 'Element must have at least %(count)d %(type)s children', {
                'type':meta.tag,'count':meta.min_occurrence}
        if ( meta.max_occurrence is not None 
             and occurrences > meta.max_occurrence ):
            if meta.max_occurrence == 1:
                return 'Element must have no more than one %(type)s child', {'type':meta.tag}
            return 'Element must have no more than %(count)d %(type)s children', {
                'type':meta.tag,'count':meta.max_occurrence}
        return None
        
