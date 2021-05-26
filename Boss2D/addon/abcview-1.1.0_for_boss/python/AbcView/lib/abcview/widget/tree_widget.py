#-******************************************************************************
#
# Copyright (c) 2012-2014,
#  Sony Pictures Imageworks Inc. and
#  Industrial Light & Magic, a division of Lucasfilm Entertainment Company Ltd.
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
# *       Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
# *       Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following disclaimer
# in the documentation and/or other materials provided with the
# distribution.
# *       Neither the name of Sony Pictures Imageworks, nor
# Industrial Light & Magic, nor the names of their contributors may be used
# to endorse or promote products derived from this software without specific
# prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#-******************************************************************************

import os
import re
import sys
import copy

from PyQt4 import QtCore
from PyQt4 import QtGui
from PyQt4 import uic

import imath
import alembic
from abcview.io import Scene, Session, Mode
from abcview.gl import GLScene
from abcview.utils import find_objects, get_schema_info
from abcview.utils import sum_two_lists, diff_two_lists
from abcview import log, style, config

def message(info):
    dialog = QtGui.QMessageBox()
    dialog.setStyleSheet(style.DIALOG)
    dialog.setText(info)
    dialog.exec_()

class ArrayThread(QtCore.QThread):
    """
    Simple Qthread that emits values from a (long) array. Takes a
    QWidget parent, an array and a max number of elements to emit
    as arguments (if max is None, get all of them).
    """
    def __init__(self, parent, array, first=0, last=10):
        super(ArrayThread, self).__init__(parent)
        self.array = array
        self.__first = first
        self.__last = last

    def _get_first(self):
        return self.__first

    def _set_first(self, value):
        self.__first = value

    first = property(_get_first, _set_first, doc="array starting point")

    def _get_last(self):
        return self.__last

    def _set_last(self, value):
        self.__last = value

    last = property(_get_last, _set_last, doc="array ending point")

    def run(self):
        try:
            if type(self.array) in (str, unicode, int, float, bool):
                self.emit(QtCore.SIGNAL('arrayValue (PyQt_PyObject)'), (0, str(self.array)))
            else:
                for index, value in enumerate(self.array[self.first:]):
                    index = index + self.first
                    self.emit(QtCore.SIGNAL('arrayValue (PyQt_PyObject)'), (index, value))
                    if index >= self.last:
                        break
        except TypeError, e:
            self.emit(QtCore.SIGNAL('arrayValue (PyQt_PyObject)'), (0, str(self.array)))

class SceneLineEditor(QtGui.QLineEdit):
    """
    Top-level delegate for SceneTreeWidgetItems, used on the name column to
    facilitate scene renaming.
    """
    def __init__(self, parent, name):
        super(SceneLineEditor, self).__init__()
        self._parent = parent
        self.setObjectName("SceneLineEditor")
        self.setText(name)
        self.setFocusPolicy(QtCore.Qt.ClickFocus)

## alembic tree items ---------------------------------------------------------
class AbcTreeWidgetItem(QtGui.QTreeWidgetItem):
    """
    Base class from which all other tree widgets are derived.
    """
    def __init__(self, parent, object=None):
        super(QtGui.QTreeWidgetItem, self).__init__(parent)
        self._parent = parent
        self.seen = False
        self.valid = True
        self.object = object

    def __repr__(self):
        return '<%s "%s">' % (self.__class__.__name__, self.object)

    def _get_valid(self):
        return self.__valid

    def _set_valid(self, valid=True):
        if not valid:
            self.setTextColor(2, QtGui.QColor(1, 0, 0))
        else:
            self.setTextColor(2, QtGui.QColor(1, 1, 1))
        self.setDisabled(not valid)
        self.__valid = valid

    valid = property(_get_valid, _set_valid, "This object is valid")

    def name(self):
        return self.object.getName()

    def path(self):
        return self.object.getFullName()

    def root(self):
        parent = self.parent()
        while parent:
            parent = parent.parent()
        return parent

    def scene(self):
        parent = self
        while parent:
            if type(parent) == SceneTreeWidgetItem:
                return parent
            parent = parent.parent()

    def is_removable(self):
        return False

    def set_bad(self, bad):
        """
        Set this tree widget item as being "bad" for some reason, e.g. an
        undrawable scene, with a visual indicator.
        """
        log.debug("[%s.set_bad] %s" % (self, bad))

    def setText(self, name, value):
        """
        setText override that looks up the tree column header index.
        """
        super(AbcTreeWidgetItem, self).setText(self.treeWidget().colnum(name),
                str(value))

    def setToolTip(self, name, value):
        """
        setToolTip override that looks up the tree column header index.
        """
        super(AbcTreeWidgetItem, self).setToolTip(self.treeWidget().colnum(name),
                QtCore.QString(str(value)))

    def keyPressEvent(self, event):
        self._parent.keyPressEvent(event)

class ObjectTreeWidgetItem(AbcTreeWidgetItem):
    def __init__(self, parent, object=None):
        """
        Object tree widget item base class.

        :param object: IObject class object
        """
        super(ObjectTreeWidgetItem, self).__init__(parent, object)
        self.setChildIndicatorPolicy(QtGui.QTreeWidgetItem.DontShowIndicator)
        self.setIcon(0, QtGui.QIcon("%s/object.png" % config.ICON_DIR))

        self.object = object
        if object:
            if self.object.getNumChildren() > 0:
                self.setChildIndicatorPolicy(QtGui.QTreeWidgetItem.ShowIndicator)
            self.setExpanded(False)
            self.setText('name', object.getName())
            self.setToolTip('name', object.getFullName())

    def children(self):
        """
        Generator that yields ObjectTreeWidgetItem objects.
        """
        self.treeWidget().setCursor(QtCore.Qt.WaitCursor)
        for child in self.object.children:
            if self.seen:
                continue
            md = child.getMetaData()
            if alembic.AbcGeom.ICameraSchema.matches(md):
                yield CameraTreeWidgetItem(self, child)
            else:
                yield ObjectTreeWidgetItem(self, child)
        self.treeWidget().setCursor(QtCore.Qt.ArrowCursor)

    def properties(self):
        props = self.object.getProperties()
        for header in props.propertyheaders:
            yield props.getProperty(header.getName())

    def bounds(self, sample_index=0):
        return self.object.getProperties().getProperty(".geom").getProperty(".selfBnds").getValue(sample_index)

    def keyPressEvent(self, event):
        self.treeWidget().keyPressEvent(event)

class CameraTreeWidgetItem(ObjectTreeWidgetItem):
    def __init__(self, parent, object):
        """
        :param object: ICamera class object
        """
        super(CameraTreeWidgetItem, self).__init__(parent, object)
        self.setIcon(0, QtGui.QIcon("%s/camera.png" % config.ICON_DIR))
        self.__camera = None

    def camera(self):
        if self.__camera is None:
            self.__camera = alembic.AbcGeom.ICamera(self.object.getParent(),
                        self.object.getName())
        return self.__camera

class EditableTreeWidgetItem(AbcTreeWidgetItem):
    def __init__(self, parent, scene, property):
        super(EditableTreeWidgetItem, self).__init__(parent, property)
        self.scene = scene
        self.property = property

        for i in range(self.treeWidget().header().count()):
            self.treeWidget().header().hideSection(i)
        self.treeWidget().header().showSection(self.treeWidget().colnum('name'))
        self.treeWidget().header().showSection(self.treeWidget().colnum('value'))

        self.setText('name', self.name())
        self.setText('value', self.formatted())

        self.setToolTip('name', self.name())
        self.setToolTip('value', self.value())

    def name(self):
        return self.property[0]

    def value(self):
        return self.property[1]

    def formatted(self):
        if self.type() in (list, tuple):
            return ", ".join([str(v) for v in self.value()])
        elif self.type() in (unicode, str, int, float, bool):
            return str(self.value())

    def type(self):
        return type(self.value())

    def samples(self):
        return []

class PropertyTreeWidgetItem(AbcTreeWidgetItem):
    def __init__(self, parent, property):
        """
        :param property: IArray or IScalar Property object
        """
        super(PropertyTreeWidgetItem, self).__init__(parent, property)
        self.scene = parent
        self.property = property

        if self.property.isCompound():
            self.setChildIndicatorPolicy(QtGui.QTreeWidgetItem.ShowIndicator)
        else:
            self.setChildIndicatorPolicy(QtGui.QTreeWidgetItem.DontShowIndicator)

        for i in range(self.treeWidget().header().count()):
            self.treeWidget().header().showSection(i)
        self.treeWidget().header().hideSection(self.treeWidget().colnum('value'))

        self.setText('name', self.property.getName())
        self.setText('type', self.type())
        self.setText('constant', self.is_constant())
        self.setText('datatype', self.property.getDataType())
        self.setToolTip('name', self.property.getName())

    def _get_object(self):
        return self.property

    def type(self):
        if self.property.isCompound():
            return 'compound'
        elif self.property.isScalar():
            return 'scalar'
        elif self.property.isArray():
            return 'array'
        else:
            return 'unknown'

    def is_constant(self):
        if self.property.isCompound():
            return False
        return self.property.isConstant()

    def properties(self):
        for header in self.property.propertyheaders:
            yield self.property.getProperty(header.getName())

    def samples(self):
        if self.property.isCompound():
            return []
        else:
            return self.property.samples

class SampleTreeWidgetItem(AbcTreeWidgetItem):
    def __init__(self, parent, index, sample, property=None):
        """
        :param parent: AbcTreeWidget object
        :param index: Sample index value
        :param sample: Alembic Sample object
        :param property: PropertyTreeWidgetItem object
        """
        super(SampleTreeWidgetItem, self).__init__(parent, sample)
        self.property = property
        self.sample = sample

        self.setChildIndicatorPolicy(QtGui.QTreeWidgetItem.DontShowIndicator)

        self.setText('index', index)
        self.setText('size', self.length())
        self.setText('value', self.value())

    def _get_object(self):
        return self.sample

    def value(self):
        if self.property and self.property.object.isArray():
            return type(self.sample).__name__
        return self.sample

    def length(self):
        if not self.valid:
            return 0
        if self.property and self.property.object.isScalar():
            return self.property.object.getDataType().getExtent()
        else:
            return len(self.sample)

    def _set_valid(self, value=True):
        super(SampleTreeWidgetItem, self)._set_valid(value)
        self.setText('size', self.length())

class ArrayTreeWidgetItem(AbcTreeWidgetItem):
    def __init__(self, parent, index, value, array=None):
        super(ArrayTreeWidgetItem, self).__init__(parent, array)
        self.setChildIndicatorPolicy(QtGui.QTreeWidgetItem.DontShowIndicator)
        self.setText('index', index)
        self.setText('value', value)

## session tree items ---------------------------------------------------------
class SessionTreeWidgetItem(AbcTreeWidgetItem):
    def __init__(self, parent, object):
        """
        :param object: Session
        """
        super(SessionTreeWidgetItem, self).__init__(parent, object)
        self.setChildIndicatorPolicy(QtGui.QTreeWidgetItem.ShowIndicator)
        self.setExpanded(True)
        self.setText('name', self.object.name)
        self.setToolTip('name', self.object.filepath)
        self.setIcon(0, QtGui.QIcon("%s/session-open.png" % config.ICON_DIR))
        self.setCheckState(self.treeWidget().colnum(""), QtCore.Qt.Checked)

    def load(self):
        self.object.loaded = True
        self.object.visible = True
        self.treeWidget().emit(QtCore.SIGNAL('itemLoaded (PyQt_PyObject)'), self)
        self.setCheckState(self.treeWidget().colnum(""), QtCore.Qt.Checked)

    def unload(self):
        self.object.loaded = False
        self.object.visible = False
        self.treeWidget().emit(QtCore.SIGNAL('itemUnloaded (PyQt_PyObject)'), self)
        self.setCheckState(self.treeWidget().colnum(""), QtCore.Qt.Unchecked)

    def properties(self):
        return self.object.properties.items()

    def is_removable(self):
        return True

    def children(self):
        if self.seen:
            for index in range(self.childCount()):
                yield self.child(index)
        else:
            for item in self.object.items:
                if item.filepath.endswith(Session.EXT):
                    yield SessionTreeWidgetItem(self, item)
                elif item.filepath.endswith(Scene.EXT):
                    yield SceneTreeWidgetItem(self, item)
            self.seen = True

class SceneTreeWidgetItem(SessionTreeWidgetItem):
    def __init__(self, parent, object):
        """
        :param object: Scene
        """
        super(SceneTreeWidgetItem, self).__init__(parent, object)
        self.setToolTip('', 'visible')
        self.setIcon(0, QtGui.QIcon("%s/scene.png" % config.ICON_DIR))
        self.setExpanded(False)

        # for GL selection
        self.object.tree = self

        # color indicator delegate
        self.g = QtGui.QGroupBox()
        self.treeWidget().setItemWidget(self,
                self.treeWidget().colnum('color'), self.g)
        self.set_color(QtGui.QColor(*[x*500.0 for x in self.object.color]))

    def set_color(self, color):
        """
        Sets the color indicator delegate in the tree widget item.

        :param color: QColor object
        """
        # normalize color values
        rgb = (color.red(), color.green(), color.blue())
        normalized = [c / 500.0 for c in rgb]
        self.object.color = normalized

        # update delegate widget stylesheet
        self.g.setStyleSheet("background: rgb(%.2f, %.2f, %.2f);"
                % (color.red(), color.green(), color.blue()))

    def is_removable(self):
        return True

    def children(self):
        yield ObjectTreeWidgetItem(self, self.object.archive.getTop())

## tree Widgets ---------------------------------------------------------------
class DeselectableTreeWidget(QtGui.QTreeWidget):
    def mousePressEvent(self, event):
        self.clearSelection()
        QtGui.QTreeView.mousePressEvent(self, event)

class AbcTreeWidget(DeselectableTreeWidget):
    DEFAULT_COLUMN_NAMES = ['name',  ]
    DEFAULT_COLUMNS = dict(enumerate(DEFAULT_COLUMN_NAMES))
    DEFAULT_COLUMNS.update(dict(zip(DEFAULT_COLUMN_NAMES, range(len(DEFAULT_COLUMN_NAMES)))))
    COLUMNS = copy.copy(DEFAULT_COLUMNS)

    signal_item_selected = QtCore.pyqtSignal()
    signal_item_expanded = QtCore.pyqtSignal()
    signal_item_added = QtCore.pyqtSignal(AbcTreeWidgetItem)
    signal_item_removed = QtCore.pyqtSignal(AbcTreeWidgetItem)
    signal_item_clicked = QtCore.pyqtSignal()
    signal_item_doubleclicked = QtCore.pyqtSignal()

    def colnum(self, name):
        return self.COLUMNS.get(name, -1)

    @property
    def column_names(self):
        return [self.COLUMNS[el] for el in sorted(el for el in self.COLUMNS if type(el) == int)]

    def __init__(self, parent, main):
        super(QtGui.QTreeWidget, self).__init__(parent)
        self.main = main
        self.__row_height = 24

        # reserved for editable item
        self._item = None

        # set some default QtTreeWidget values
        self.setIconSize(QtCore.QSize(20, 20))
        self.setAllColumnsShowFocus(True)
        self.setAnimated(False)
        self.setAutoScroll(False)
        self.setUniformRowHeights(True)
        self.setSelectionMode(QtGui.QAbstractItemView.SingleSelection)
        self.setDragDropMode(QtGui.QAbstractItemView.NoDragDrop)
        self.setFocusPolicy(QtCore.Qt.NoFocus)
        self.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)

        self.init_header()

        #TODO: refactor all this event handling
        self.connect(self, QtCore.SIGNAL("itemSelectionChanged ()"),
                self.handle_item_selected)
        self.connect(self, QtCore.SIGNAL("itemClicked (QTreeWidgetItem *, int)"),
                self.handle_item_clicked)
        self.connect(self, QtCore.SIGNAL("itemDoubleClicked (QTreeWidgetItem *, int)"),
                self.handle_item_double_clicked)
        self.connect(self, QtCore.SIGNAL("itemExpanded (QTreeWidgetItem *)"),
                self.handle_item_expanded)
        self.connect(self, QtCore.SIGNAL("itemCollapsed (QTreeWidgetItem *)"),
                self.handle_item_collapsed)
        self.connect(self, QtCore.SIGNAL("customContextMenuRequested (const QPoint&)"),
                self.handle_context_menu)

    def init_header(self):
        self.COLUMNS = copy.copy(self.DEFAULT_COLUMNS)
        self.setup_header()
        self.setSortingEnabled(True)
        self.sortByColumn(self.colnum('date'), QtCore.Qt.DescendingOrder)

    def setup_header(self):
        self.setColumnCount(len(self.COLUMNS)/2)
        self.setHeaderLabels(self.column_names)
        self.header().resizeSection(self.colnum('index'), 50)
        self.header().resizeSection(self.colnum('size'), 75)
        self.header().resizeSection(self.colnum('name'), 300)

    def _get_row_height(self):
        return self.__row_height

    def _set_row_height(self, h):
        self.__row_height = h

    row_height = property(_get_row_height, _set_row_height, doc="Tree item row height")

    def handle_context_menu(self, pos):
        pass

    def handle_item_selected(self):
        """
        Item selected handler (select can happen with click or arrow keys).
        """
        items = self.selectedItems()
        if not items:
            return
        self.handle_item_clicked(items[0])

    def handle_item_clicked(self, item, col=0):
        """
        Item click handler.
        """
        self.scrollToItem(item, QtGui.QAbstractItemView.EnsureVisible)
        self.emit(QtCore.SIGNAL('itemClicked (PyQt_PyObject)'),
                    item)
        if col == self.colnum("") and type(item) == SceneTreeWidgetItem:
            if item.checkState(self.colnum("")) == QtCore.Qt.Checked:
                item.load()
            else:
                item.unload()

    def handle_item_double_clicked(self, item):
        """
        Item double-click handler.
        """
        self._item = item
        if type(item) in (SessionTreeWidgetItem, SceneTreeWidgetItem):
            value = item.object.name
            colnum = self.colnum('name')
        elif type(item) in (EditableTreeWidgetItem, ):
            value = item.formatted()
            colnum = self.colnum('value')
        else:
            return
        editor = SceneLineEditor(item, str(value))
        self._item.editor = editor
        self.setItemWidget(item, colnum, editor)
        self.emit(QtCore.SIGNAL('itemDoubleClicked (PyQt_PyObject)'),
                item)
        self._item.old_value = str(self._item.editor.text().toAscii())
        #editor.textEdited.connect(self.handle_value_change)
        editor.editingFinished.connect(self.handle_done_editing)

    def handle_value_change(self, value):
        """
        Handles text change for item delegate line edits.
        """
        if not self._item or not value:
            return
        value = str(value.toAscii())
        if type(self._item) in (SessionTreeWidgetItem, SceneTreeWidgetItem):
            self._item.object.name = value
            self._item.setText('name', value)
        elif type(self._item) in (EditableTreeWidgetItem, ):
            value = eval(value)
            setattr(self._item.scene.object, self._item.object[0], value)
            self._item.property = (self._item.object[0], value)
            self._item.setText('value', value)

    def handle_done_editing(self):
        """
        Line edit delegate done editing handler.
        """
        if not self._item or not self._item.editor:
            return

        def s2f(val):
            return [float(v.strip()) for v in val.split(",")]

        new_value = str(self._item.editor.text().toAscii())

        if type(self._item) in (SessionTreeWidgetItem, SceneTreeWidgetItem):
            column = "name"
        elif type(self._item) in (EditableTreeWidgetItem, ):
            column = "value"

        if new_value != self._item.old_value:

            # from the objects tree, you can only rename sessions and scenes
            if column == "name":
                self._item.object.name = new_value
                self._item.setText('name', new_value)

            # editable property tree widget item
            elif column == "value":
                if self._item.type() in (list, tuple):
                    new_value = s2f(new_value)
                    old_value = s2f(self._item.old_value)
                    diff_value = diff_two_lists(old_value, new_value)
                else:
                    diff_value = new_value

                # add override to session item
                self._item.scene.object.add_override(self._item.name(), new_value)

                self._item.property = (self._item.name(), new_value)
                self._item.setText('value', self._item.formatted())

        self.removeItemWidget(self._item, self.colnum(column))
        self._item = None

    def handle_item_expanded(self, item):
        raise NotImplementedError("implement in subclass")

    def handle_item_collapsed(self, item):
        raise NotImplementedError("implement in subclass")

    def selected(self):
        """
        Returns selected item's object
        """
        selected = [item for item in self.selectedItems()]
        if not selected:
            return []
        selected = selected[0]
        if type(selected) == SessionTreeWidgetItem:
            return selected.object
        elif type(selected) == SceneTreeWidgetItem:
            return selected.object.archive.getTop()
        else:
            return selected.object

    def find(self, name):
        """
        Searches each top level item for name, starting with
        nodes under /ABC, the top
        """
        self.clearSelection()
        if name.startswith('/'):
            name = name[1:]
        for index in range(self.topLevelItemCount()):
            item = self.topLevelItem(index)
            found = False
            while type(item) != ObjectTreeWidgetItem:
                item.setExpanded(True)
                item = item.child(0)
            for object in find_objects(item.object, name):
                parts = [p for p in object.getFullName().split('/') if p]
                item.setExpanded(True)
                while parts:
                    part = parts.pop(0)
                    for i in range(0, item.childCount()):
                        child = item.child(i)
                        check = str(child.text(self.colnum('name')))
                        if part == check:
                            child.setExpanded(True)
                            item = child
                            found = True
                            break
                        else:
                            child.setExpanded(False)
                            found = False

        if found:
            self.scrollToItem(item, QtGui.QAbstractItemView.PositionAtCenter)
            item.setExpanded(False)
            self.setItemSelected(item, True)

class ObjectTreeWidget(AbcTreeWidget):
    DEFAULT_COLUMN_NAMES = ['name', '', 'color', 'hidden', ]
    DEFAULT_COLUMNS = dict(enumerate(DEFAULT_COLUMN_NAMES))
    DEFAULT_COLUMNS.update(dict(zip(DEFAULT_COLUMN_NAMES, range(len(DEFAULT_COLUMN_NAMES)))))
    COLUMNS = copy.copy(DEFAULT_COLUMNS)

    # signal for viewing through selected camera
    signal_view_camera = QtCore.pyqtSignal(AbcTreeWidgetItem)

    def __init__(self, parent, main):
        super(ObjectTreeWidget, self).__init__(parent, main)
        self.setAutoFillBackground(False)
        self.setHorizontalScrollBarPolicy(QtCore.Qt.ScrollBarAsNeeded)

    def setup_header(self):
        self.setColumnCount(len(self.COLUMNS)/2)
        self.setHeaderLabels(self.column_names)
        self.header().setResizeMode(self.colnum('name'), QtGui.QHeaderView.Stretch)
        self.header().resizeSection(self.colnum('name'), 150)
        self.header().setResizeMode(self.colnum(''), QtGui.QHeaderView.Fixed)
        self.header().resizeSection(self.colnum(''), 25)
        self.header().setResizeMode(self.colnum('color'), QtGui.QHeaderView.Fixed)
        self.header().resizeSection(self.colnum('color'), 3)
        self.header().setResizeMode(self.colnum('hidden'), QtGui.QHeaderView.Fixed)
        self.header().resizeSection(self.colnum('hidden'), 5)
        self.header().setSectionHidden(self.colnum('hidden'), True)
        self.header().setStretchLastSection(False)

    def add_item(self):
        self.main.handle_import()

    def remove_item(self, item=None):
        '''
        removes an item from the tree and session.

        :param: objecttreewidgetitem or subclass.
        '''
        if item is None:
            if self.selectedItems():
                item = self.selectedItems()[0]
        if item is None or item.parent() is not None or not item.is_removable():
            message("Item is not removable")
            return
        self.takeTopLevelItem(self.indexOfTopLevelItem(item))
        self.signal_item_removed.emit(item)

    def handle_remove_item(self, triggered):
        '''
        remove item action handler, calls remove_item().
        '''
        if self.selectedItems():
            self.remove_item(self.selectedItems()[0])

    def handle_item_expanded(self, item):
        if type(item) == SessionTreeWidgetItem:
            item.setIcon(0, QtGui.QIcon("%s/session-open.png" % config.ICON_DIR))
        if not item.seen:
            for child in item.children():
                item.addChild(child)
            item.seen = True

    def handle_item_collapsed(self, item):
        if type(item) == SessionTreeWidgetItem:
            item.setIcon(0, QtGui.QIcon("%s/session.png" % config.ICON_DIR))

    def handle_set_color(self, ok):
        """
        Set item color menu handler

        :param item: SceneTreeWidgetItem
        """
        item = self.selectedItems()[0]
        dialog = QtGui.QColorDialog(QtGui.QColor(*item.object.color), self)
        rgba, ok = dialog.getRgba()
        if ok and type(item) == SceneTreeWidgetItem:
            item.set_color(QtGui.QColor(rgba))

            # add a session override
            item.object.add_override("color", item.object.color, apply=False)

    def handle_set_mode(self, mode):
        """
        Set item shading mode menu handler

        :param mode: abcview.io.Mode enum value
        """
        if self.sender(): # via menu action
            mode = self.sender().data().toInt()[0]

        self.setCursor(QtCore.Qt.WaitCursor)
        item = self.selectedItems()[0]
        item.object.mode = mode
        self.setCursor(QtCore.Qt.ArrowCursor)

        # add a session override
        item.object.add_override("mode", mode, apply=False)

    def handle_duplicate_item(self):
        raise NotImplementedError

    def view_camera(self):
        """
        view through selected camera
        """
        camera = self.selectedItems()[0]
        self.signal_view_camera.emit(camera)

    def copy_name(self):
        """
        copy item name to clipboard
        """
        item = self.selectedItems()[0]
        clipboard = QtGui.QApplication.clipboard()
        clipboard.setText(item.object.getFullName())

    def handle_context_menu(self, pos):
        """
        tree widget context menu handler
        """
        items = self.selectedItems()
        menu = QtGui.QMenu(self)
        if not items:
            self.add_action = QtGui.QAction("Add", self)
            self.connect(self.add_action, QtCore.SIGNAL("triggered (bool)"),
                    self.add_item)
            menu.addAction(self.add_action)

        elif type(items[0]) == SceneTreeWidgetItem:
            item = items[0]

            # color picker
            self.color_action = QtGui.QAction("Color", self)
            self.connect(self.color_action, QtCore.SIGNAL("triggered (bool)"),
                        self.handle_set_color)
            menu.addAction(self.color_action)

            # shading toggle menu item
            self.shading_menu = QtGui.QMenu("Shading", self)
            shading_group = QtGui.QActionGroup(self.shading_menu)

            self.clearAct = QtGui.QAction("Clear", self)
            self.clearAct.setCheckable(True)
            self.clearAct.setActionGroup(shading_group)
            self.clearAct.setData(-1)
            self.clearAct.setChecked(item.object.mode == -1)
            self.clearAct.toggled.connect(self.handle_set_mode)
            self.shading_menu.addAction(self.clearAct)
            self.shading_menu.addSeparator()

            self.offAct = QtGui.QAction("Off", self)
            #self.offAct.setShortcut("0")
            self.offAct.setCheckable(True)
            self.offAct.setActionGroup(shading_group)
            self.offAct.setData(Mode.OFF)
            self.offAct.setChecked(item.object.mode == Mode.OFF)
            self.offAct.toggled.connect(self.handle_set_mode)
            self.shading_menu.addAction(self.offAct)

            self.fillAct = QtGui.QAction("Fill", self)
            #self.fillAct.setShortcut("1")
            self.fillAct.setCheckable(True)
            self.fillAct.setActionGroup(shading_group)
            self.fillAct.setData(Mode.FILL)
            self.fillAct.setChecked(item.object.mode == Mode.FILL)
            self.fillAct.toggled.connect(self.handle_set_mode)
            self.shading_menu.addAction(self.fillAct)

            self.lineAct = QtGui.QAction("Line", self)
            #self.lineAct.setShortcut("2")
            self.lineAct.setCheckable(True)
            self.lineAct.setActionGroup(shading_group)
            self.lineAct.setData(Mode.LINE)
            self.lineAct.setChecked(item.object.mode == Mode.LINE)
            self.lineAct.toggled.connect(self.handle_set_mode)
            self.shading_menu.addAction(self.lineAct)

            self.pointAct = QtGui.QAction("Point ", self)
            #self.pointAct.setShortcut("3")
            self.pointAct.setCheckable(True)
            self.pointAct.setActionGroup(shading_group)
            self.pointAct.setData(Mode.POINT)
            self.pointAct.setChecked(item.object.mode == Mode.POINT)
            self.pointAct.toggled.connect(self.handle_set_mode)
            self.shading_menu.addAction(self.pointAct)

            self.bboxAct = QtGui.QAction("Bounds ", self)
            #self.bboxAct.setShortcut("4")
            self.bboxAct.setCheckable(True)
            self.bboxAct.setActionGroup(shading_group)
            self.bboxAct.setData(Mode.BOUNDS)
            self.bboxAct.setChecked(item.object.mode == Mode.BOUNDS)
            self.bboxAct.toggled.connect(self.handle_set_mode)
            self.shading_menu.addAction(self.bboxAct)

            menu.addMenu(self.shading_menu)
            menu.addSeparator()

            # hide/show
            if item.object.loaded:
                self.load_action = QtGui.QAction("Hide", self)
                self.connect(self.load_action, QtCore.SIGNAL("triggered (bool)"),
                        item.unload)
            else:
                self.load_action = QtGui.QAction("Show", self)
                self.connect(self.load_action, QtCore.SIGNAL("triggered (bool)"),
                        item.load)
            menu.addAction(self.load_action)

            # duplicate scene
            #self.duplicate_action = QtGui.QAction("Duplicate", self)
            #self.connect(self.duplicate_action, QtCore.SIGNAL("triggered (bool)"),
            #            self.handle_duplicate_item)
            #menu.addAction(self.duplicate_action)

            # remove scene
            self.remove_action = QtGui.QAction("Remove", self)
            self.connect(self.remove_action, QtCore.SIGNAL("triggered (bool)"),
                        self.handle_remove_item)
            menu.addAction(self.remove_action)

        else:
            item = items[0]
            if type(item) == CameraTreeWidgetItem:
                self.view_action = QtGui.QAction("Look through selected", self)
                self.connect(self.view_action, QtCore.SIGNAL("triggered (bool)"),
                        self.view_camera)
                menu.addAction(self.view_action)

            else:
                self.copy_name_action = QtGui.QAction("Copy name", self)
                self.connect(self.copy_name_action, QtCore.SIGNAL("triggered (bool)"),
                        self.copy_name)
                menu.addAction(self.copy_name_action)

        menu.popup(self.mapToGlobal(pos))

class PropertyTreeWidget(AbcTreeWidget):
    DEFAULT_COLUMN_NAMES = ['name', 'type', 'datatype', 'value', ]
    DEFAULT_COLUMNS = dict(enumerate(DEFAULT_COLUMN_NAMES))
    DEFAULT_COLUMNS.update(dict(zip(DEFAULT_COLUMN_NAMES, range(len(DEFAULT_COLUMN_NAMES)))))
    COLUMNS = copy.copy(DEFAULT_COLUMNS)
    EDITABLE = ["name", "filepath", "translate", "rotate", "scale", "color"]

    def __init__(self, parent, main):
        super(PropertyTreeWidget, self).__init__(parent, main)

    def setup_header(self):
        self.setColumnCount(len(self.COLUMNS)/2)
        self.setHeaderLabels(self.column_names)
        self.header().resizeSection(self.colnum('name'), 150)
        self.header().hideSection(self.colnum('value'))

    def show_properties(self, item):
        self.clear()
        if type(item) in (SessionTreeWidgetItem, SceneTreeWidgetItem):

            # special properties, like translate, rotate, scale...
            for editable in self.EDITABLE:
                property = (editable, getattr(item.object, editable))
                self.addTopLevelItem(EditableTreeWidgetItem(self, item, property))

            if type(item) == SceneTreeWidgetItem:
                info = alembic.Abc.GetArchiveInfo(item.object.archive)
                for key, value in info.items():
                    self.addTopLevelItem(EditableTreeWidgetItem(self, item, (key, value)))

            for property in item.properties():
                if property[0] in self.EDITABLE:
                    continue
                self.addTopLevelItem(EditableTreeWidgetItem(self, item, property))
        else:
            for property in item.properties():
                self.addTopLevelItem(PropertyTreeWidgetItem(self, property))

    def handle_item_expanded(self, item):
        if not item.seen:
            for property in item.properties():
                item.addChild(PropertyTreeWidgetItem(item, property))
            item.seen = True

class SampleTreeWidget(AbcTreeWidget):
    DEFAULT_COLUMN_NAMES = ['index', 'size', 'value', ]
    DEFAULT_COLUMNS = dict(enumerate(DEFAULT_COLUMN_NAMES))
    DEFAULT_COLUMNS.update(dict(zip(DEFAULT_COLUMN_NAMES, range(len(DEFAULT_COLUMN_NAMES)))))
    COLUMNS = copy.copy(DEFAULT_COLUMNS)

    def __init__(self, parent, main):
        super(SampleTreeWidget, self).__init__(parent, main)
        self.setRootIsDecorated(False)

    def show_samples(self, item):
        self.clear()
        for index in range(len(item.samples())):
            try:
                sample = item.samples()[index]
                valid = True
                tooltip = ""
            except RuntimeError, e:
                valid = False
                tooltip = str(e)
                msg = str(e).split("\n")[-1]
                sample = "[Error: %s]" % (msg)
            sample_item = SampleTreeWidgetItem(self, index, sample, item)
            sample_item.valid = valid
            sample_item.setToolTip("value", tooltip)
            self.addTopLevelItem(sample_item)

class ArrayTreeWidget(AbcTreeWidget):
    DEFAULT_COLUMN_NAMES = ['index', 'value', ]
    DEFAULT_COLUMNS = dict(enumerate(DEFAULT_COLUMN_NAMES))
    DEFAULT_COLUMNS.update(dict(zip(DEFAULT_COLUMN_NAMES, range(len(DEFAULT_COLUMN_NAMES)))))
    COLUMNS = copy.copy(DEFAULT_COLUMNS)

    def __init__(self, parent, main):
        super(ArrayTreeWidget, self).__init__(parent, main)
        self.setRootIsDecorated(False)

    def row_count(self):
        return (self.main.array_tree.size().height() / self.row_height) + 20

    def get_rows(self, start, end):
        thread = ArrayThread(self.main, self.item._get_object(), first=start, last=end)
        self.connect(thread, QtCore.SIGNAL("arrayValue (PyQt_PyObject)"), self.handle_add_value)
        thread.start()

    def show_values(self, array):
        self.clear()
        self.item = array
        if array.length() > 0:
            self.get_rows(0, self.row_count())

    def handle_add_value(self, indexValue):
        self.index, value = indexValue
        self.addTopLevelItem(ArrayTreeWidgetItem(self, self.index, value))

    ## base class overrides

    def wheelEvent(self, event):
        index = self.indexAt(self.viewport().rect().bottomLeft())
        lastRow = self.itemFromIndex(index)
        if lastRow:
            lastIndex = lastRow.text(self.colnum("index"))
            if self.index >= (len(self.item._get_object()) - 1):
                pass
            elif (int(lastIndex) + 10) >= int(self.index):
                self.get_rows(self.index, self.index + self.row_count())
        super(AbcTreeWidget, self).wheelEvent(event)
