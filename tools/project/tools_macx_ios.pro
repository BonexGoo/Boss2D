!include(tools.pri) {
    error("BOSS2D : Couldn't find the tools.pri file...")
}

macx{
	message("BOSS2D : The platform was decided to be macx...")
} else {
	ios{
		message("BOSS2D : The platform was decided to be ios...")
	} else {
		error("BOSS2D : You can not build on this platform!")
	}
}
