#! /usr/bin/env python
#-******************************************************************************
#
# Copyright (c) 2012-2015,
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
import sys
import time
import copy
import uuid

import imath
import alembic
from abcview import config, log
from abcview.utils import get_object
from abcview.utils import json, sum_two_lists, diff_two_lists, mult_two_lists

__doc__ = """
The IO module handles serialization and deserialization of the assembled 
scenes and sessions. The hierarchy structure basically consists of a 
top-level Session object that contains children items which can be either
Scenes, Sessions, Cameras or ICAmeras. Each item in the hierarchy has
a properties attribute for storing custom attributes. Only Scenes and ICameras
reference Alembic archives. 

Session hierarchy: ::

   Session
       |- Properties
       |- Cameras
       `- Items
             `- Scene
                  |- Properties
                  `- file.abc

Sessions can also reference other session files.
"""

__all__ = ["Scene", "Session", "Camera", "ICamera",
           "AbcViewError", "Mode", ]

class Mode:
    OFF = 0
    BOUNDS = 1
    POINT = 2
    LINE = 3
    FILL = 4

class AbcViewError(Exception):
    pass

def DictListUpdate(lis1, lis2):
    for aLis1 in lis1:
        if aLis1 not in lis2:
            lis2.append(aLis1)
    return lis2

#TODO: consider a checksum of the data instead
def make_uuid():
    return uuid.uuid4().hex

class idict(object):
    """
    Dict-like class that supports the notion of local vs. inherited properties.
    Property values are first looked for in local, then inherited.
    """
    def __init__(self, *args, **kwargs):
        super(idict, self).__init__()
 
        self.local = dict(**kwargs)
        self.inherited = dict()
        self.saved = dict()
        
        if args:
            self.local = copy.deepcopy(args[0])

    def __repr__(self):
        return repr(self.properties)

    def __contains__(self, key):
        return key in self.properties

    def __eq__(self, other):
        return self.properties == other

    def __len__(self):
        return len(self.properties)

    def __iter__(self):
        return iter(self.keys())

    def __getattr__(self, key):
        return super(idict, self).__getattr__(key)

    def __setitem__(self, key, value):
        self.set(key, value)

    def __getitem__(self, key):
        return self.get(key)

    def _get_properties(self):
        return dict(self.items())

    def _set_properties(self, value):
        if type(value) in (dict, ):
            self.local = value

    properties = property(_get_properties, _set_properties, doc="properties")

    def clear(self):
        self.local.clear()

    def update(self, *args, **kwargs):
        self.local.update(*args, **kwargs)

    def get(self, key, default=None):
        return self.local.get(key, self.saved.get(key, default))

    def set(self, key, value):
        if key not in self.saved:
            self.saved[key] = value
        self.local[key] = value

    def has_key(self, key):
        return key in self.local or key in self.inherited

    def keys(self):
        return dict(self.inherited.items() + self.local.items()).keys()

    def values(self):
        return dict(self.inherited.items() + self.local.items()).values()

    def items(self):
        return dict(self.inherited.items() + self.local.items()).items()

class Base(object):
    def __init__(self, parent=None):
        self.uuid = make_uuid()
        self.loaded = True
        self.instance = 1
        self.parent = parent
        self.clear_properties()

    def __repr__(self):
        return "<%s \"%s\">" % (self.type(), self.name)

    def _get_name(self):
        raise NotImplementedError("implement in subclass")

    def _set_name(self, value):
        raise NotImplementedError("implement in subclass")

    name = property(_get_name, _set_name, doc="name")

    @classmethod
    def type(cls):
        return cls.__name__

    def serialize(self):
        raise NotImplementedError("must be implemented in a subclass")

    @classmethod
    def deserialize(self):
        raise NotImplementedError("must be implemented in a subclass")

    def clear_properties(self):
        self.overrides = idict()
        self.properties = idict()
        self.properties.local = {
            "translate": (0, 0, 0),
            "rotate": (0, 0, 0, 0),
            "scale": (1, 1, 1),
            #"color": (0.5, 0.5, 0.5),
            "mode": -1
        }

class FileBase(Base):
    SERIALIZE = []
    EXT = None
    def __init__(self, filepath, parent=None):
        super(FileBase, self).__init__(parent)
        self.__filepath = filepath
        self.__name = "Unnamed"

    def is_archive(self):
        return self.fileext == Scene.EXT

    def _get_name(self):
        return self.__name

    def _set_name(self, value):
        self.__name = value

    name = property(_get_name, _set_name, doc="name")

    def _get_filepath(self):
        return self.__filepath

    def _set_filepath(self, value):
        if value is not None:
            self.__filepath = value
            self.__name = os.path.basename(value)

    filepath = property(_get_filepath, _set_filepath, doc="file path")

    def _get_filetype(self):
        return self.__class__.__name__

    def _set_filetype(self, value):
        raise NotImplementedError("File type is immutable")

    filetype = property(_get_filetype, _set_filetype, doc="file type")

    def _get_fileext(self):
        if self.filepath:
            return self.filepath.split(".")[-1]
        return None

    def _set_fileext(self, value):
        raise NotImplementedError("File ext is immutable")

    fileext = property(_get_fileext, _set_fileext, doc="file extension")

    def _get_parent(self):
        if not hasattr(self, "_parent"):
            self._parent = None
        return self._parent

    def _set_parent(self, parent):
        if type(parent) in (Scene, Session) or parent == None:
            self._parent = parent
        else:
            log.warn("cannot parent to: %s" % parent)

    parent = property(_get_parent, _set_parent)

    def _get_session(self):
        parent = self.parent
        sess = parent
        while parent and type(parent) == Session:
            sess = parent
            parent = parent.parent
        return sess

    def _set_session(self, parent):
        raise NotImplementedError

    session = property(_get_session, _set_session)

    def instancepath(self):
        """
        Returns the instance uuid path to this scene in a nested hierarchy.
        """
        path = self.uuid
        parent = self.parent
        while parent and parent != self.session:
            path = ":".join([parent.uuid, path])
            parent = parent.parent
        return path

    def serialize(self):
        raise NotImplementedError

class EditableMixin:
    """
    Some convenience methods for classes that have high-level properties, and
    responsible for setting contextual overrides.
    """

    def add_override(self, name, value, apply=True):
        """
        Adds an override to this item on the item's session.

        :param name: property name
        :param value: property value
        :param apply: apply overrides to children
        """
        # get the top session item for this item
        top = self.instancepath().split(":")[0]
        for item in self.session.items:

            # update property overrides
            if item.uuid == top:
                overs = item.overrides.local.setdefault(self.instancepath(), {})
                props = overs.setdefault("properties", {})
                props.update({name: value})
                break

        if apply:
            self.apply_overrides(item)

    def apply_overrides(self, item):
        """
        Applies item overrides to children.

        :param item: Session or Scene item containing overrides
        """
        
        def reset_value_from_overrides(item, overs):
            for prop in ("translate", "rotate", "scale", "color", "mode"):
                item.properties.local[prop] = overs.get("properties", {}).get(prop, 
                    item.properties.get(prop)
                )
                item.properties.saved[prop] = overs.get("properties", {}).get(prop, 
                    item.properties.get(prop)
                )

        if item.type() == Camera.type():
            return

        for path, overs in item.overrides.items():
            if path == item.instancepath():
                reset_value_from_overrides(item, overs)

            if item.type() == Session.type():
                for child in item.walk():
                    if child.type() == Camera.type():
                        continue

                    # scene level overrides (exact match, reset the saved values)
                    elif child.instancepath() == path:
                        reset_value_from_overrides(child, overs)

                    # session level overrides (all children)
                    elif child.instancepath().startswith(path):
                        child.name = overs.get("name", child.name)
                        child.loaded = overs.get("loaded", child.loaded)
                        child.filepath = overs.get("filepath", child.filepath)
                        child.mode = overs.get("properties", {}).get("mode", child.mode)
                        #child.color = overs.get("properties", {}).get("color", child.color)
                        
                        child.translate = sum_two_lists(child.properties.saved.get("translate", child.translate),
                            overs.get("properties", {}).get("translate")
                        )
                        child.rotate = sum_two_lists(child.properties.saved.get("rotate", child.rotate),
                            overs.get("properties", {}).get("rotate")
                        )
                        child.scale = mult_two_lists(child.properties.saved.get("scale", child.scale),
                            overs.get("properties", {}).get("scale")
                        )

                    elif child.type() == Session.type():
                        self.apply_overrides(child)

    def _get_translate(self):
        return self.properties.get("translate", self.properties.saved.get("translate"))

    def _set_translate(self, value):
        value = [float(v) for v in value]
        self.properties["translate"] = value

    translate = property(_get_translate, _set_translate, doc="translate property")

    def _get_rotate(self):
        return self.properties.get("rotate", self.properties.saved.get("rotate"))

    def _set_rotate(self, value):
        value = [float(v) for v in value]
        self.properties["rotate"] = value

    rotate = property(_get_rotate, _set_rotate, doc="rotation property")

    def _get_scale(self):
        return self.properties.get("scale", self.properties.saved.get("scale"))

    def _set_scale(self, value):
        value = [float(v) for v in value]
        self.properties["scale"] = value

    scale = property(_get_scale, _set_scale, doc="scale property")

    def _get_mode(self):
        return self.properties.get("mode", self.properties.saved.get("mode"))

    def _set_mode(self, value):
        value = int(value)
        self.properties["mode"] = value

    mode = property(_get_mode, _set_mode, doc="GL polygon mode property")

    def _get_color(self):
        return self.properties.get("color", self.properties.saved.get("color", (0.5, 0.5, 0.5)))

    def _set_color(self, value):
        value = [float(v) for v in value]
        self.properties["color"] = value

    color = property(_get_color, _set_color, doc="color to display in viewer")

    def has_xform_overrides(self):
        return any(k in self.properties for k in ('translate', 'rotate', 'scale'))

class Scene(FileBase, EditableMixin):
    """
    Represents a single item in an AbcView file 
    (either .abc or .io file)
    """
    EXT = "abc"
    def __init__(self, filepath=None):
        super(Scene, self).__init__(filepath)
        self.filepath = os.path.abspath(filepath)

    def __repr__(self):
        return "<%s \"%s\">" % (self.type(), self.name)

    def serialize(self):
        return {
            "uuid": self.uuid,
            "filepath": self.filepath,
            "instance": self.instance,
            "loaded": self.loaded,
            "name": self.name,
            "overrides": {
                self.instancepath(): {
                    "properties": self.properties.local
                }
            }
        }

    @classmethod
    def deserialize(cls, data):
        """
        Deserializes an Alembic scene from json data.
        """
        item = cls(data.get("filepath"))
        item.uuid = data.get("uuid", make_uuid())
        item.name = data.get("name", "Unnamed")
        item.loaded = data.get("loaded", True)
        item.instance = data.get("instance", 1)
        item.overrides = idict(data.get("overrides", {}))
        return item

class CameraBase(Base):
    """
    Base class for camera objects.
    """
    def __init__(self, name):
        self.__name = name
        
        # draw toggles
        self.__auto_frame = False
        self.__draw_bounds = True
        self.__draw_hud = False
        self.__draw_labels = False
        self.__draw_grid = True
        self.__draw_normals = False
        self.__draw_mode = Mode.FILL

        # fixed aspect ratio toggle
        self.__fixed = False
        
        # draw visible objects only
        self.__visible = True

    def _get_name(self):
        return self.__name

    def _set_name(self, value):
        self.__name = value

    name = property(_get_name, _set_name, doc="Camera name")

    def _get_draw_grid(self):
        return self.__draw_grid

    def _set_draw_grid(self, force=None):
        if force is not None:
            self.__draw_grid = force
        else:
            self.__draw_grid = not self.__draw_grid

    draw_grid = property(_get_draw_grid, _set_draw_grid, doc="draw the scene grid")

    def _get_draw_normals(self):
        return self.__draw_normals

    def _set_draw_normals(self, force=None):
        if force is not None:
            self.__draw_normals = force
        else:
            self.__draw_normals = not self.__draw_normals

    draw_normals = property(_get_draw_normals, _set_draw_normals, doc="draw normals")

    def _get_draw_bounds(self):
        return self.__draw_bounds

    def _set_draw_bounds(self, force=None):
        if force is not None:
            self.__draw_bounds = force
        else:
            self.__draw_bounds = not self.__draw_bounds

    draw_bounds = property(_get_draw_bounds, _set_draw_bounds, doc="draw bounding boxes")

    def _get_draw_labels(self):
        return self.__draw_labels
    
    def _set_draw_labels(self, force=None):
        if force is not None:
            self.__draw_labels = force
        else:
            self.__draw_labels = not self.__draw_labels

    draw_labels = property(_get_draw_labels, _set_draw_labels, doc="draw scene labels")

    def _get_draw_hud(self):
        return self.__draw_hud
    
    def _set_draw_hud(self, force=None):
        if force is not None:
            self.__draw_hud = force
        else:
            self.__draw_hud = not self.__draw_hud

    draw_hud = property(_get_draw_hud, _set_draw_hud, doc="draw hud info")

    def _get_auto_frame(self):
        return self.__auto_frame
    
    def _set_auto_frame(self, force=None):
        if force is not None:
            self.__auto_frame = force
        else:
            self.__auto_frame = not self._auto_frame

    auto_frame = property(_get_auto_frame, _set_auto_frame, doc="automatically frame scene or selected")

    def _get_draw_mode(self):
        return self.__draw_mode

    def _set_draw_mode(self, mode):
        self.__draw_mode = mode

    mode = property(_get_draw_mode, _set_draw_mode, doc="global drawing mode")

    def _get_fixed(self):
        return self.__fixed
    
    def _set_fixed(self, force=None):
        if force is not None:
            self.__fixed = force
        else:
            self.__fixed = not self.__fixed

    fixed = property(_get_fixed, _set_fixed, doc="draw fixed aspect ratio")

    def _get_visible(self):
        return self.__visible
    
    def _set_visible(self, force=None):
        if force is not None:
            self.__visible = force
        else:
            self.__visible = not self.__visible

    visible = property(_get_visible, _set_visible, doc="draw visible objects only")

class Camera(CameraBase):
    """
    AbcView API Camera object. Camera attributes are not
    animatable as opposed to Alembic ICamera attributes.

    Acting de/serialization layer for GLCamera objects.
    """
    SERIALIZE = ["translation", "rotation", "scale", "aspect_ratio",
                 "fovx", "fovy", "near", "far", "center", "fixed", "mode",
                 "draw_hud", "draw_grid", "draw_normals", "draw_bounds",
                 "draw_labels", "name", "loaded", "visible"]

    def __init__(self, name, loaded=False):
        """
        :param name: camera name
        """
        super(Camera, self).__init__(name)
        self.loaded = loaded

    @classmethod
    def type(cls):
        return "Camera"

    def serialize(self):
        d = {
            "type": self.type(),
        }
        for attr in self.SERIALIZE:
            val = getattr(self, attr, None)
            if val is not None:
                if attr in ["translation", "rotation", "scale"]:
                    d[attr] = [val[0], val[1], val[2]]
                else:
                    d[attr] = val
        return d

    @classmethod
    def deserialize(cls, params):
        cam = cls(name=params.get("name"),
                  loaded=params.get("loaded"))
        for attr in cls.SERIALIZE:
            val = params.get(attr)
            if attr in ["translation", "rotation", "scale"]:
                val = imath.V3d(*val)
            setattr(cam, attr, val)
        return cam

class ICamera(CameraBase):
    """
    Alembic ICamera de/serialization wrapper class. Use this class
    for loading Alembic ICameras.

    Setting up a basic scene with a camera ::
        
        # imports
        >>> from abcview.io import Session, ICamera
        >>> from abcview.utils import get_object

        # create session, add scene file
        >>> session = Session()
        >>> session.add_file("scene.abc")

        # add icamera wrapped in ICamera IO class
        >>> session.add_camera(ICamera(get_object("shotcam.abc", 
                                                  "ShotCam")
                                       loaded=True))

        # save session
        >>> session.save("scene.io")
    """
    SERIALIZE = ["fixed", "mode", "draw_hud", "draw_grid", "draw_normals", 
                 "draw_labels", "name", "loaded", "draw_bounds", "visible"]

    def __init__(self, icamera, loaded=False):
        """
        :param icamera: Alembic ICamera object
        """
        self.icamera = icamera
        self.loaded = loaded
        self.__schema = None
        super(ICamera, self).__init__(self._get_name)

    def __repr__(self):
        return "<%s \"%s\">" % (self.__class__.__name__, self.name)

    @classmethod
    def type(cls):
        return "ICamera"

    def _not_settable(self, value):
        log.debug("ICamera name is immutable")

    def _get_name(self):
        return self.icamera.getName()

    name = property(_get_name, _not_settable, doc="Camera name")

    def schema(self):
        self.icamera = alembic.AbcGeom.ICamera(
                           self.icamera.getParent(),
                           self.name)
        if self.__schema is None and self.icamera:
            self.__schema = self.icamera.getSchema()
        return self.__schema

    def _ixform_sample(self, seconds):
        """
        Returns icamera's parent xform sample

        :param seconds: time in secs (derives index)
        """
        #TODO: (mem)cache sample lookups
        cp = self.icamera.getParent()
        xform = alembic.AbcGeom.IXform(cp.getParent(), 
                                       cp.getName())
        xs = xform.getSchema()
        ts = xs.getTimeSampling()
        index = ts.getNearIndex(seconds, 
                                xs.getNumSamples())
        return xs.getValue(index)

    def _icamera_sample(self, seconds):
        """
        Returns icamera's sample
        
        :param seconds: time in secs (derives index)
        """
        ts = self.schema().getTimeSampling()
        index = ts.getNearIndex(seconds, 
                                self.schema().getNumSamples())
        return self.schema().getValue(index)

    def translation(self, seconds=0):
        return self._ixform_sample(seconds).getTranslation()

    def rotation(self, seconds=0):
        samp = self._ixform_sample(seconds)
        return imath.V3d(samp.getXRotation(), samp.getYRotation(), 
                    samp.getZRotation())

    def scale(self, seconds=0):
        return self._ixform_sample(seconds).getScale()

    def matrix(self, seconds=0):
        return self._ixform_sample(seconds).getMatrix()

    def near(self, seconds=0):
        return self._icamera_sample(seconds).getNearClippingPlane()

    def far(self, seconds=0):
        return self._icamera_sample(seconds).getFarClippingPlane()

    def fovx(self, seconds=0):
        return self._icamera_sample(seconds).getFieldOfView()

    def aspect_ratio(self, seconds=0):
        """
        From Alembic/AbcGeom/CameraSample.h:
   
        The amount the camera's lens compresses the image horizontally
        (width / height aspect ratio)
        """
        samp = self._icamera_sample(seconds)
        return (samp.getHorizontalAperture() / samp.getVerticalAperture()) \
                 * samp.getLensSqueezeRatio()

    def screen_window(self, seconds=0):
        win = self._icamera_sample(seconds).getScreenWindow()
        return win["left"], win["bottom"], win["left"], win["right"]

    def serialize(self):
        d = {
            "filepath": self.icamera.getArchive().getName(), 
            "fullname": self.icamera.getFullName(),
            "type": self.type(),
        }
        for attr in self.SERIALIZE:
            val = getattr(self, attr, None)
            if val is not None:
                d[attr] = val
        return d

    @classmethod
    def deserialize(cls, params):
        cam = cls(get_object(
                       params.get("filepath"),
                       params.get("fullname")
                   ),
                   loaded=params.get("loaded")
               )
        for attr in cls.SERIALIZE:
            setattr(cam, attr, params.get(attr))
        return cam

class Session(FileBase, EditableMixin):
    """
    AbcView API Session object. Top level container layer that holds
    properties and child session or scene objects.

    De/serialization layer for AbcView sessions.
    """
    EXT = "io"
    def __init__(self, filepath=None):
        super(Session, self).__init__(filepath)
        self.clear() 
        if filepath and os.path.isfile(filepath):
            self.load(filepath)

    def __contains__(self, item):
        if type(item) in [str, unicode]:
            item = Scene(item)
        return item.filepath in [i.filepath for i in self.__items]

    def _get_items(self):
        return self.__items
    
    def _set_items(self):
        raise NotImplementedError("Use add_item() or add_file()")

    items = property(_get_items, _set_items, doc="child items")

    def _get_cameras(self):
        return [camera for camera in self.__cameras.values()]
    
    def _set_cameras(self):
        raise NotImplementedError("Use add_camera()")

    cameras = property(_get_cameras, _set_cameras, doc="cameras")

    def add_item(self, item):
        """
        Adds and item to the session

        :param item: Scene or Session object
        """
        log.debug("[%s.add_item] %s" % (self, item))
        found_instances = [i.filepath for i in self.items if i.filepath == item.filepath]
        item.instance = len(found_instances) + 1
        item.parent = self
        self.apply_overrides(item)
        self.__items.append(item)

    def remove_item(self, item):
        """
        Removes an item from the session

        :param item: Scene or Session object
        """
        log.debug("[%s.remove_item] %s" % (self, item))
        if item in self.__items:
            self.__items.remove(item)
        else:
            log.debug("Item not in session: %s" % item)

    def add_file(self, filepath):
        """
        Adds a filepath to the session

        :param filepath: path to file
        """
        log.debug("[%s.add_file] %s" % (self, filepath))
        if filepath.endswith(self.EXT):
            item = Session(filepath)
        elif filepath.endswith(Scene.EXT):
            item = Scene(filepath)
        else:
            raise AbcViewError("Unsupported file type: %s" % filepath)
        self.add_item(item)
        return item

    def add_camera(self, camera):
        """
        :param: GLCamera
        """
        log.debug("[%s.add_camera] %s" % (self, camera))
        if camera.name not in self.__cameras:
            self.__cameras[camera.name] = camera

    def remove_camera(self, camera):
        """
        :param: GLCamera
        """
        log.debug("[%s.remove_camera] %s" % (self, camera))
        if camera.name in self.__cameras:
            del self.__cameras[camera.name]

    def set_camera(self, camera):
        """
        Sets the "active" camera for a given session.

        :param: GLCamera
        """
        log.debug("[%s.set_camera] %s" % (self, camera))
        if camera.name not in self.__cameras:
            self.__cameras[camera.name] = camera
        for name, cam in self.__cameras.items():
            cam.loaded = False
        self.__cameras[camera.name].loaded = True

    def serialize(self):
        """
        Serializes the session object to a JSON dict.
        """
        def _serialize(item):
            if item.type() == "Session":
                return {
                    "uuid": item.uuid,
                    "name": item.name,
                    "filepath": item.filepath, 
                    "instance": item.instance,
                    "loaded": item.loaded,
                    "overrides": item.overrides.local,
                }
            else:
                return item.serialize()

        return {
            "items": [_serialize(item) for item in self.items],
        }

    def is_dirty(self):
        """
        Change that requires saving.
        """
        return self.__dirty

    def make_dirty(self):
        self.__dirty = True

    def make_clean(self):
        self.__dirty = False

    def clear(self):
        self.version = config.__version__
        self.program = config.__prog__
        self.clear_properties()
        self.date = time.time()
        self.min_time = 0
        self.max_time = 0
        self.current_time = 0
        self.frames_per_second = 24.0
        
        # stores objects that need special handling
        self.__cameras = {}
        self.__items = []
        
        self.make_clean()

    def merge(self, session):
        """
        Merges a given session into this session. 

        :param session: Session object to merge in
        """
        self.min_time = session.min_time
        self.max_time = session.max_time
        self.current_time = session.current_time
       
        # merge items
        self.__items = DictListUpdate(self.items, session.items)

        # merge cameras
        self.__cameras.update(session.cameras)

        # merge overrides
        self.overrides.update(session.overrides)
        
        # merge properties
        self.properties.update(session.properties)

    def walk(self):
        """
        Recursive generator that yields Session, Scene and Camera objects.

        :yield: Session, Scene or Camera objects
        """
        for item in self.items + self.cameras:
            if item.type() == Session.type():
                yield item
                for child in item.walk():
                    yield child
            else:
                yield item

    def load(self, filepath=None):
        """
        Loads a session .io file.
        """
        if filepath is None and self.filepath:
            filepath = self.filepath
        elif filepath:
            self.filepath = filepath
        else:
            raise AbcViewError("File path not set")

        # metadata and properties
        state = json.load(open(filepath, "r"))
        self.name = state.get("name", os.path.basename(filepath))
        self.version = state.get("app").get("version")
        self.program = state.get("app").get("program")
        self.date = state.get("date")
        self.instance = state.get("instance", 1)
        self.frames_per_second = state.get("frames_per_second", 
                           self.frames_per_second)
        self.min_time = state.get("min_time", self.min_time)
        self.max_time = state.get("max_time", self.max_time)
        self.current_time = state.get("current_time", self.current_time)

        # cameras
        for camera in state.get("cameras"):
            if camera.get("type") == Camera.type():
                self.add_camera(Camera.deserialize(camera))
            elif camera.get("type") == ICamera.type():
                self.add_camera(ICamera.deserialize(camera))

        # items
        data = state.get("data")
        for d in data.get("items"):
            fp = str(d.get("filepath"))

            if fp.endswith(Scene.EXT):
                self.add_item(Scene.deserialize(d))

            # TODO: replace with deserialization for sessions
            elif fp.endswith(Session.EXT):
                item = Session(fp)
                item.name = d.get("name", item.name)
                item.uuid = d.get("uuid", item.uuid)
                item.loaded = d.get("loaded", item.loaded)
                item.overrides.update(d.get("overrides", {}))
                self.add_item(item)

    def save(self, filepath=None):
        """
        Saves a session to a .io file.
        """
        if filepath is None and self.filepath:
            filepath = self.filepath
        if not filepath:
            raise AbcViewError("File path not set")
        elif not filepath.endswith(self.EXT):
            filepath += self.EXT
        self.filepath = filepath
        self.date = time.time()
        log.debug("[%s.save] %s" % (self, filepath))
        state = {
            "app": {
                "program": self.program,
                "version": self.version,
                "module": os.path.dirname(__file__),
            },
            "env": {
                "user": os.environ.get("USER", os.environ.get("USERNAME")),
                "host": os.environ.get("HOST", os.environ.get("HOSTNAME")),
                "platform": sys.platform,
            },
            "date": self.date,
            "min_time": self.min_time,
            "max_time": self.max_time,
            "current_time": self.current_time,
            "frames_per_second": self.frames_per_second,
            "cameras": [camera.serialize() for camera in self.__cameras.values()],
            "data": self.serialize()
        }
        json.dump(state, open(filepath, "w"), sort_keys=True, indent=4)
