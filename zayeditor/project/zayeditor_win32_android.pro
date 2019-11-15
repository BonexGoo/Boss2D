!include(zayeditor.pri) {
    error("BOSS2D : Couldn't find the zayeditor.pri file...")
}

win32-msvc*{
	message("BOSS2D : The platform was decided to be win32-msvc*...")
} else {
	win32-g++{
		message("BOSS2D : The platform was decided to be win32-g++...")
	} else {
		android{
			message("BOSS2D : The platform was decided to be android...")
		} else {
			error("BOSS2D : You can not build on this platform!")
		}
	}
}
